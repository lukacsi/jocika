#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>

class Process {
public:
    Process(const std::vector<std::string>& command);
    ~Process();

    bool start();
    void stop();
    void resume();
    void terminate();

    ssize_t read(char* buffer, size_t size);

private:
    int fd;
    pid_t pid;
    bool running;
};

#endif

