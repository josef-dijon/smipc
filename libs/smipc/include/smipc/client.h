#pragma once

#include <memory>

class DuplexSharedMemoryFile;

class Client {
public:
    Client();
    ~Client();

    void Run();

private:
    std::unique_ptr<DuplexSharedMemoryFile> sm_ {};
};