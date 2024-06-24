#ifndef PROACTOR_HPP
#define PROACTOR_HPP

#include <pthread.h>
#include <unordered_map>
#include <atomic>
#include <functional>

typedef void *(*proactorFunc)(int);

class Proactor {
public:
    Proactor();

    ~Proactor();

    static pthread_t startProactor(int sockfd, proactorFunc threadFunc);

    static int stopProactor(pthread_t tid);

private:
    static void *proactorThread(void *arg);

    static std::unordered_map<pthread_t, Proactor *> proactors;
    static pthread_mutex_t proactorsMutex;

    int sockfd;
    proactorFunc threadFunc;
    std::atomic<bool> running;
    pthread_mutex_t graphMutex;
};

#endif // PROACTOR_HPP
