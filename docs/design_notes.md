# Smipc

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
