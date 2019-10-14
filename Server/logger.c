#include "logger.h"
#include "networking.h"
#include <sys/wait.h>

//inter process handler
/*
void cleanup(){

}*/

void readfrompipeLogger(){
    char *buff;
    //use FIFO to recieve stuff from pipes
    if(mkfifo("/tmp/logger_fifo", 0666) == -1){
        perror("FIFO exists?");
    }
    int fd = open("/tmp/logger_fifo", O_RDONLY);
    if(fd == -1){
        perror("Cannot open for reading!");
        exit(0);
    }
    else{
        int num;
        //loop while reading stuff
        do{
            int len;
            num = read(fd, (char *) &len, sizeof(int));
            if(num == 0) break;
            buff = (char *) malloc(len * sizeof(char)+1);
            buff[len] = '\0';
            read(fd, buff, len);
            printf("Action is... %s\n", buff);
            logger(buff, fopen("log.dongs", "a"));
        } while (num > 0);
        close(fd);
        wait(0);
    }
    free(buff);
}

void gettimeStamp(char *timeStamp){
    //uses posix to get the time stamp
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strncpy(timeStamp, asctime(timeinfo), 30);
}

void setLogger(char *logFile){

    //setting up the logger file
    int exists = checkifExits(fopen(logFile, "r"));

    if(exists == -1){
        logger("Created logger file!\n", fopen(logFile, "w"));
    }
    else if(exists == 0){
        //set logger to read only mode
        logger("First Logger entry for this instance!\n", fopen(logFile, "a"));
    }
}

//log some stuff
void logger(char* action, FILE* logFile){
    //out putting logger information
    char timeStamp[100];
    gettimeStamp(timeStamp);

    fwrite(action, strlen(action), 1, logFile);
    fwrite(timeStamp, strlen(timeStamp), 1, logFile);
    fclose(logFile);
}

