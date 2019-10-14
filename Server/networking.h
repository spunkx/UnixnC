#ifndef NETWORKING_H_
#define NETWORKING_H_

#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>


int createSocket(int portNumber);
void acceptmanyClients(int listenSocket);
void createThreads(int clientSocket, int i, pthread_t *tid);
void writetoFork(char *buff);

#endif