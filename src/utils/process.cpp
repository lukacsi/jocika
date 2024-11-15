#include "utils/process.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>  // Include this header for waitpid
#include <signal.h>
#include <fcntl.h>
#include <vector>
#include <cstring>

Process::Process(const std::vector<std::string>& command) : fd(-1), pid(-1), running(false) {
    // Prepare the arguments for execvp
    std::vector<char*> args;
    for (const auto& arg : command) {
        args.push_back(const_cast<char*>(arg.c_str()));
    }
    args.push_back(nullptr);

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        // Child process
        close(pipefd[0]); // Close read end
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
        close(pipefd[1]);

        // Convert vector<char*> to char*[]
        execvp(args[0], args.data());
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        close(pipefd[1]); // Close write end
        fd = pipefd[0];
        running = true;
    }
}

Process::~Process() {
    terminate();
    if (fd != -1) {
        close(fd);
    }
}

bool Process::start() {
    return running;
}

void Process::stop() {
    if (running) {
        if (kill(pid, SIGSTOP) == -1) {
            perror("kill(SIGSTOP)");
        }
    }
}

void Process::resume() {
    if (running) {
        if (kill(pid, SIGCONT) == -1) {
            perror("kill(SIGCONT)");
        }
    }
}

void Process::terminate() {
    if (running) {
        if (kill(pid, SIGTERM) == -1) {
            perror("kill(SIGTERM)");
        }
        if (waitpid(pid, nullptr, 0) == -1) {
            perror("waitpid");
        }
        running = false;
    }
}

ssize_t Process::read(char* buffer, size_t size) {
    return ::read(fd, buffer, size);
}

