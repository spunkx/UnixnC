#ifndef LOGGER_H_
#define LOGGER_H_

#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

pid_t pid;
pid_t c_pid;

int checkifExits(FILE *configFile);
void setLogger(char *logFile);
void logger(char *action, FILE *logFile);
void readfrompipeLogger();
void writetoFork(char *buff);

#endif