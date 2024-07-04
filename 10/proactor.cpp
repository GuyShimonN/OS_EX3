#include "proactor.hpp"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Structure to hold the arguments for the thread function
struct ThreadArgs {
    int sockfd;
    proactorFunc threadFunc;
};

// Wrapper function to match pthread_create signature
void* threadWrapper(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int sockfd = args->sockfd;
    proactorFunc threadFunc = args->threadFunc;
    void* result = threadFunc(sockfd);
    delete args; // Free the dynamically allocated memory
    return result;
}

pthread_t startProactor(int sockfd, proactorFunc threadFunc) {
    pthread_t tid;
    ThreadArgs* args = new ThreadArgs{sockfd, threadFunc}; // Dynamically allocate memory for arguments
    if (pthread_create(&tid, NULL, threadWrapper, (void*)args) != 0) {
        perror("pthread_create");
        delete args; // Clean up if thread creation fails
        return -1;
    }
    std::cout << "Started proactor on socket: " << sockfd << ", thread ID: " << tid << std::endl;
    return tid;
}

int stopProactor(pthread_t tid) {
    if (pthread_cancel(tid) != 0) {
        perror("pthread_cancel");
        return -1;
    }
    if (pthread_join(tid, NULL) != 0) {
        perror("pthread_join");
        return -1;
    }
    return 0;
}
// make
// ./server
// open a new terminal
// nc localhost 9033
//Newgraph 6 5
//0 1
//1 2
//2 0
//3 4
//4 5
//Kosaraju
//open a new terminal
// Removeedge 0 1
//Kosaraju
//Exit