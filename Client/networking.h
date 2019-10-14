#ifndef NETWORKING_H_
#define NETWORKING_H_
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

int connectClient(char *currserverIP, int currPort);

#endif