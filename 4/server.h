#pragma once
#include <iostream>
#include <vector>
#include <list>
#include <functional>
#include <algorithm>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <fcntl.h>
#include <sys/select.h>

// Function declarations
void Newgraph(int numVertices, int numEdges);
void Newedge(int u, int v);
void Removeedge(int u, int v);
void Kosaraju(int client_fd);
void handle_client(int client_fd, fd_set &master_set, fd_set &read_fds);

