#ifndef CLIENT_H_
#define CLIENT_H_

#include <memory>

class DuplexSharedMemoryFile;

class Client {
public:
    Client();
    ~Client();

    void run();

private:
    std::unique_ptr<DuplexSharedMemoryFile> m_sharedMemory {};
};

#endif // CLIENT_H_
