# Smipc

## Overview

## Overview

Smipc is a shared memory inter-process communication library. It is designed to
be used in a multi-threaded environment where multiple processes need to
communicate with each other. The library is designed to be used in a
client-server architecture where the server is the owner of the shared memory
and the clients are the users of the shared memory. The server is responsible
for creating the shared memory and the clients are responsible for connecting
to the shared memory.

The library uses a many to one server-client connection model where the server
can have multiple clients connected to it. The server maintains a list of
connected clients. The clients can send and receive messages to and from the
server. The server can send messages to any or all of the connected clients.

Each connection between the server and the client is a separate shared memory
to avoid contention between the clients. The server creates a shared memory for
each connection.

The server serves requests from the clients in a round-robin fashion,
responding to each client in turn. The server can also broadcast messages to all
the connected clients as a form of notification.

Because each connection maintains its own shared memory, simple atomic locks
are used to avoid contention between the server and the clients. In this way,
the server and the clients can operate independently of each other, whilst
still maintaining a high level of concurrency and performance. Although this
means there could be many shared memory segments created, the shared memory
segments are small and are only created when a client connects to the server.

## Features

- Many to one server-client connection model.
- Server can have multiple clients connected to it.
- Clients can send and receive messages to and from the server.
- Server can send messages to any or all of the connected clients.
- Each connection between the server and the client is a separate shared memory.
- Server serves requests from the clients in a round-robin fashion.
- Server can broadcast messages to all the connected clients.
- Simple atomic locks are used to avoid contention between the server and the clients.

## Message Format

```
+-SharedMemory------------------------------------------------------------------------+
| +-RingBuffer----------------------------------------------------------------------+ |
| |  rx waiting  |  tx waiting  |  turn  |  front  |   next   |   free   |   count  | |
| +-Packet--------------------------------------------------------------------------+ |
| |      |      |          |             |              | +-Serialisation segment-+ | |
| | lock | size | checksum | transfer_id | packet_count | |         data          | | |
| |      |      |          |             |              | +-----------------------+ | |
| +-Packet--------------------------------------------------------------------------+ |
| |      |      |          |             |              | +-Serialisation segment-+ | |
| | lock | size | checksum | transfer_id | packet_count | |         data          | | |
| |      |      |          |             |              | +-----------------------+ | |
| +-Packet--------------------------------------------------------------------------+ |
| |      |      |          |             |              | +-Serialisation segment-+ | |
| | lock | size | checksum | transfer_id | packet_count | |         data          | | |
| |      |      |          |             |              | +-----------------------+ | |
| +---------------------------------------------------------------------------------+ |
+-------------------------------------------------------------------------------------+
```

```c++
struct Packet {
    std::atomic_bool lock {false};
    uint32_t size {};
    uint32_t transfer_id {};
    uint32_t packet_count {};
    uint32_t checksum {};
    std::vector<std::byte> data {};
};
```

## Core Components

### Server Components
- **ConnectionBroker**: A dedicated pipe for handling new client connection requests
- **ClientChannel**: Server-side end of a dedicated client communication channel
- Connection number management for tracking and recycling client connection IDs

### Client Components
- Temporary connection to server's ConnectionBroker (only for connection establishment)
- **ServerChannel**: Client-side end of a dedicated communication channel

## Naming Scheme

### ConnectionBroker Names
Pattern: `/smipc.<server_application_name>.broker.{rx|tx}`

Example for application "myapp":
- `/smipc.myapp.broker.rx` - Server receives connection requests
- `/smipc.myapp.broker.tx` - Server sends responses

### Channel Names
Pattern: `/smipc.<server_application_name>.client.<N>.{rx|tx}`

Where:
- `N` is a connection number assigned by the server
- `rx` and `tx` denote receive and transmit channels respectively

## Connection Management

### Connection Number Assignment
The server maintains:
- A running connection count
- A list of available (recycled) connection numbers

Assignment algorithm:
1. If available numbers exist, use the lowest available number
2. Otherwise, use the current connection count and increment it

### Number Recycling Logic
When a connection closes:
1. If it was the highest active number (equal to connection_count - 1):
   - Decrement connection_count
   - Clean up any recycled numbers above new count
2. Otherwise:
   - Add number to available_numbers list for reuse

This approach:
- Keeps connection numbers as low as possible
- Automatically shrinks when connections close in sequence
- Requires minimal memory overhead
- Makes it easy to spot problems (unexpectedly high numbers)

## Connection Lifecycle

### Connection Establishment
1. Client creates temporary connection to ConnectionBroker
2. Client sends connection request
3. Server allocates next connection number
4. Server creates ClientChannel with numbered address
5. Server sends connection number to client through ConnectionBroker
6. Client disconnects from ConnectionBroker
7. Client creates ServerChannel using received number
8. Connection established

### Normal Communication
- Client sends messages through ServerChannel
- Server receives via ClientChannel
- Server sends responses through ClientChannel
- Client receives via ServerChannel

### Disconnection
1. Client sends close request through ServerChannel
2. Client closes ServerChannel
3. Server receives close request via ClientChannel
4. Server cleans up ClientChannel
5. Connection number recycled according to rules above

Note: All communication after initial connection, including disconnection requests, happens through the dedicated channel pair. The ConnectionBroker is only used for initial connection establishment.

## Resource Considerations

### Shared Memory Usage
- Each connection requires 2 shared memory segments
- For N clients, total segments = 2 + (2 * N)
  - 2 for ConnectionBroker (rx/tx)
  - 2 per client connection (rx/tx)

### Security Considerations
- Shared memory access controlled by Windows permissions
- No built-in encryption or authentication
- Suitable for trusted processes on same machine
- Additional security could be implemented in message protocol if needed

## Implementation Notes

The system is implemented in C++, taking advantage of:
- RAII for resource management
- STL containers for connection tracking
- Windows API for shared memory operations
- Class-based encapsulation of server/client logic

## Key Design Decisions

1. **ConnectionBroker Role**: 
   - Handles only connection establishment
   - Single request/response pattern
   - Temporary connections only
   - No involvement in ongoing communication or disconnection

2. **Channel Communication**:
   - All post-connection communication through dedicated channels
   - Includes disconnection requests and error handling
   - No need to reopen ConnectionBroker connection after initial setup
   - Each channel pair fully manages its own lifecycle

## Proposed class diagram

classDiagram
    class SharedMemoryPipe {
        -HANDLE mapping_handle
        -void* memory_ptr
        -size_t size
        +read(void* buffer, size_t size)
        +write(const void* data, size_t size)
        +close()
        #create_or_open(const std::string& name, size_t size)
    }

    class Server {
        -ConnectionBroker broker
        -std::vector~ClientChannel~ channels
        -int connection_count
        -std::vector~int~ available_numbers
        +start()
        +stop()
        -handle_new_connection()
        -get_next_connection_number()
        -release_connection_number(int number)
    }

    class ConnectionBroker {
        -SharedMemoryPipe rx_pipe
        -SharedMemoryPipe tx_pipe
        +wait_for_connection()
        +send_channel_number(int number)
    }

    class ClientChannel {
        -SharedMemoryPipe rx_pipe
        -SharedMemoryPipe tx_pipe
        -int channel_number
        +send_message(const Message& msg)
        +receive_message() Message
        +is_connected() bool
    }

    class Client {
        -ServerChannel channel
        +connect(const std::string& server_name)
        +disconnect()
        +send_message(const Message& msg)
        +receive_message() Message
    }

    class ServerChannel {
        -SharedMemoryPipe rx_pipe
        -SharedMemoryPipe tx_pipe
        -int channel_number
        +send_message(const Message& msg)
        +receive_message() Message
        +is_connected() bool
    }

    class Message {
        +enum Type
        +std::vector~uint8_t~ data
        +serialize() std::vector~uint8_t~
        +deserialize(const std::vector~uint8_t~&)
    }

    SharedMemoryPipe <|-- ConnectionBroker
    SharedMemoryPipe <|-- ClientChannel
    SharedMemoryPipe <|-- ServerChannel
    Server *-- ConnectionBroker
    Server *-- "0..*" ClientChannel
    Client *-- ServerChannel
    ClientChannel ..> Message
    ServerChannel ..> Message
    
## Sequence diagram

sequenceDiagram
    participant CL as Client
    participant CB as ConnectionBroker
    participant SV as Server
    participant CC as ClientChannel
    participant SC as ServerChannel
    
    Note over SV,CB: Server Startup
    SV->>CB: create_broker_pipes()
    Note over SV,CB: Creates /smipc.app.broker.{rx,tx}

    Note over CL,SC: Connection Establishment
    CL->>CB: open_broker_pipes()
    CB->>SV: wait_for_connection()
    SV->>SV: get_next_connection_number()
    
    Note over SV,CC: Server creates channel pipes
    SV->>CC: create_channel_pipes(number)
    Note over SV,CC: Creates /smipc.app.client.N.{rx,tx}
    
    SV->>CB: send_channel_number(number)
    CB->>CL: return channel number
    CL->>CB: close_broker_pipes()
    
    Note over CL,SC: Client opens existing channel
    CL->>SC: open_channel_pipes(number)
    Note over CL,SC: Opens /smipc.app.client.N.{rx,tx}
    Note over CL,SC: Connection Established

    Note over CL,SC: Normal Communication
    CL->>SC: send_message(msg)
    SC->>CC: write to shared memory
    CC->>SV: receive_message()
    SV->>CC: send_message(response)
    CC->>SC: write to shared memory
    SC->>CL: receive_message()

    Note over CL,SC: Graceful Disconnection
    CL->>SC: send_message(disconnect_request)
    SC->>CC: write to shared memory
    CC->>SV: receive_message()
    SV->>SV: release_connection_number(number)
    CL->>SC: close_channel_pipes()
    SC->>SC: unmap shared memory
    CC->>CC: detect client disconnection
    
    Note over SV,CC: Server destroys channel
    SV->>CC: destroy_channel_pipes()
    Note over SV,CC: Removes /smipc.app.client.N.{rx,tx}
    Note over CL,SC: Connection Terminated

## Shared Memory IPC Message Protocol Specification

### 1. Overview
This document specifies the message protocol used for communication between client and server over shared memory pipes in the IPC system.

### 2. Message Structure

#### 2.1 Message Layout
Each message consists of a fixed-size header followed by an optional variable-length payload:

```
+----------------+------------------+
|    Header      |     Payload     |
| (Fixed Size)   | (Variable Size) |
+----------------+------------------+
```

#### 2.2 Header Format
The header is a fixed-size structure containing:

```cpp
struct MessageHeader {
    MessageType type;                     // 4 bytes
    ApplicationMessageType app_type;      // 4 bytes
    uint32_t payload_size;               // 4 bytes
    uint32_t sequence_number;            // 4 bytes
};
// Total size: 16 bytes
```

##### 2.2.1 Field Descriptions
- `type`: Identifies system-level message type
- `app_type`: Identifies application-specific message type
- `payload_size`: Size of the payload in bytes (0 if no payload)
- `sequence_number`: Message sequence number for ordering/tracking

#### 2.3 Message Types

##### 2.3.1 System Message Types
```cpp
enum class MessageType : uint32_t {
    CloseConnection = 0,    // Request to close the channel
    Heartbeat = 1,         // Keep-alive message
    Application = 1000     // Container for application messages
};
```

##### 2.3.2 Application Message Types
```cpp
enum class ApplicationMessageType : uint32_t {
    // Defined by the application
    // Only meaningful when MessageType == Application
};
```

### 3. Protocol Rules

#### 3.1 Message Type Rules
1. When `type != MessageType::Application`:
   - `app_type` field is ignored
   - Message is handled at the channel level
   - Not forwarded to application

2. When `type == MessageType::Application`:
   - `app_type` must contain a valid application message type
   - Message is forwarded to application handlers

#### 3.2 Payload Rules
1. `payload_size` must accurately reflect the size of attached payload
2. If `payload_size == 0`, no payload follows the header
3. Maximum payload size is implementation-defined based on shared memory segment size

#### 3.3 Sequence Numbers
1. Start at 0 when channel is established
2. Increment by 1 for each message sent
3. Separate sequences for each direction (client→server and server→client)
4. Wrap around to 0 after reaching maximum value

### 4. System Messages

#### 4.1 CloseConnection
- Type: `MessageType::CloseConnection`
- Payload: None (`payload_size = 0`)
- Direction: Client → Server
- Response: None (server closes channel)

#### 4.2 Heartbeat
- Type: `MessageType::Heartbeat`
- Payload: None (`payload_size = 0`)
- Direction: Bidirectional
- Response: None

### 5. Application Messages

#### 5.1 Structure
- Type: `MessageType::Application`
- App Type: Valid `ApplicationMessageType` value
- Payload: Application-defined
- Direction: Bidirectional

#### 5.2 Application Requirements
1. Must define their own `ApplicationMessageType` enum
2. Must handle serialization/deserialization of payload
3. Must respect maximum payload size limits

### 6. Error Handling

#### 6.1 Invalid Messages
The channel should close if:
1. Header cannot be read completely
2. Invalid message type received
3. Payload size exceeds maximum
4. Payload cannot be read completely

#### 6.2 Sequence Errors
1. Out of sequence messages should be logged
2. Implementation may choose to close channel or attempt recovery

### 7. Implementation Notes

#### 7.1 Endianness
1. All integer fields are little-endian
2. Applications must handle endianness for payload data

#### 7.2 Alignment
1. Header is 16-byte aligned
2. Payload begins immediately after header
3. No padding between header and payload