#pragma once

#include <memory>

class DuplexSharedMemoryFile;

class Server {
public:
    Server();
    ~Server();

    void Run();

private:
    std::unique_ptr<DuplexSharedMemoryFile> sm_ {};
};
