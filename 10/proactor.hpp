#ifndef PROACTOR_H
#define PROACTOR_H

#include <pthread.h>

// Type definition for the proactor function
typedef void* (*proactorFunc)(int sockfd);

// Starts a new proactor and returns the proactor thread id
pthread_t startProactor(int sockfd, proactorFunc threadFunc);

// Stops the proactor by thread id
int stopProactor(pthread_t tid);

#endif // PROACTOR_H