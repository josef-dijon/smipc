# Smipc

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
