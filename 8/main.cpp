#include "Proactor.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

#define PORT 8080
#define BUFFER_SIZE 1024

volatile sig_atomic_t running = 1;

void* handleClient(int sockfd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    while ((bytesRead = recv(sockfd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytesRead] = '\0';
        std::cout << "Received: " << buffer << std::endl;
        send(sockfd, buffer, bytesRead, 0);
    }

    if (bytesRead == 0) {
        std::cout << "Client disconnected" << std::endl;
    } else {
        std::cerr << "recv error" << std::endl;
    }

    return nullptr;
}

void signalHandler(int signum) {
    running = 0;
}

int main() {
    int serverSocket;
    struct sockaddr_in address;
    pthread_t proactorId;

    // Set up signal handler
    signal(SIGINT, signalHandler);

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Setsockopt error" << std::endl;
        return -1;
    }

    // Bind socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }

    // Listen
    if (listen(serverSocket, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return -1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    // Start proactor
    proactorId = Proactor::startProactor(serverSocket, handleClient);

    // Wait for signal to stop
    while (running) {
        sleep(1);
    }

    // Stop proactor
    Proactor::stopProactor(proactorId);

    // Close server socket
    close(serverSocket);

    std::cout << "Server stopped" << std::endl;

    return 0;
}