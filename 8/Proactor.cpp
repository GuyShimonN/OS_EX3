#include "Proactor.hpp"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

std::unordered_map<pthread_t, Proactor *> Proactor::proactors;
pthread_mutex_t Proactor::proactorsMutex = PTHREAD_MUTEX_INITIALIZER;

Proactor::Proactor() : running(false) {
    pthread_mutex_init(&graphMutex, nullptr);
}

Proactor::~Proactor() {
    pthread_mutex_destroy(&graphMutex);
}

pthread_t Proactor::startProactor(int sockfd, proactorFunc threadFunc) {
    Proactor *proactor = new Proactor();
    proactor->sockfd = sockfd;
    proactor->threadFunc = threadFunc;
    proactor->running = true;

    pthread_t tid;
    pthread_create(&tid, nullptr, proactorThread, proactor);

    pthread_mutex_lock(&proactorsMutex);
    proactors[tid] = proactor;
    pthread_mutex_unlock(&proactorsMutex);

    return tid;
}

int Proactor::stopProactor(pthread_t tid) {
    pthread_mutex_lock(&proactorsMutex);
    auto it = proactors.find(tid);
    if (it == proactors.end()) {
        pthread_mutex_unlock(&proactorsMutex);
        return -1;  // Proactor not found
    }
    Proactor *proactor = it->second;
    proactors.erase(it);
    pthread_mutex_unlock(&proactorsMutex);

    proactor->running = false;
    pthread_join(tid, nullptr);
    delete proactor;
    return 0;
}

void *Proactor::proactorThread(void *arg) {
    Proactor *proactor = static_cast<Proactor *>(arg);

    while (proactor->running) {
        int clientSock = accept(proactor->sockfd, nullptr, nullptr);
        if (clientSock < 0) {
            if (proactor->running) {
                std::cerr << "Accept error" << std::endl;
            }
            continue;
        }

        pthread_t clientThread;
        pthread_create(&clientThread, nullptr,
                       [](void *arg) -> void * {
                           int *clientSockPtr = static_cast<int *>(arg);
                           int clientSock = *clientSockPtr;
                           delete clientSockPtr;

                           Proactor *proactor;
                           pthread_mutex_lock(&proactorsMutex);
                           for (const auto &pair: proactors) {
                               if (pthread_equal(pthread_self(), pair.first)) {
                                   proactor = pair.second;
                                   break;
                               }
                           }
                           pthread_mutex_unlock(&proactorsMutex);

                           pthread_mutex_lock(&proactor->graphMutex);
                           void *result = proactor->threadFunc(clientSock);
                           pthread_mutex_unlock(&proactor->graphMutex);

                           close(clientSock);
                           return result;
                       },
                       new int(clientSock)
        );
        pthread_detach(clientThread);
    }

    return nullptr;
}
