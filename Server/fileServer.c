/*set up the server*/
#include "networking.h"
#include "logger.h"
#include "authentication.h"

struct serverSettings {
    int portNumber;
    char IPAddress[50];
    char fileshareDirectory[50];
    char logfilePath[50];
    char motd[50];
};
void readConfig(FILE *configFile, struct serverSettings *currconfigSettings);
void writeConfig(FILE *configFile, struct serverSettings *currconfigSettings);
void enumbFileshare(struct serverSettings *currserverSettings, char **buff);
void writeClient(int socket, char *buff, int size);
int readBuffer(int socket, char **buff);
int downloadFile(FILE *filePtr, char **buff);
void reformFile(FILE *filePtr, char *buff, int fileLen);
void *connectHandler(void *sockDesc);
void isitReable(char* buff);

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/*
void iohandler(){
    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    fclose(fp1);
    fclose(fp2);
    exit(1);
}*/

int main(){
    
    //forks are 3**2 (note exponents are right to left associvity)
    struct serverSettings currserverSettings;

    currserverSettings.portNumber = 1337;
    strncpy(currserverSettings.IPAddress, "127.0.0.1", 50);
    strncpy(currserverSettings.fileshareDirectory, "fileshare/", 50);
    strncpy(currserverSettings.logfilePath, "log.dongs", 50);
    strncpy(currserverSettings.motd, "hey fuck off client!", 50);

    setLogger(currserverSettings.logfilePath);

    int returnCheck = checkifExits(fopen("settings.conf", "r")); 

    if(returnCheck == 0){
        readConfig(fopen("settings.conf", "rb"), &currserverSettings);
    }
    else if(returnCheck == -1){
        writeConfig(fopen("settings.conf", "wb"), &currserverSettings);
    }
    else{
        printf("An error occured");
    }
    int quit = -1;

    isitReable("settings.conf");

    printf("waiting on a client to connect\n");
    int boundSocket = createSocket(currserverSettings.portNumber);

    c_pid = fork();
    if(c_pid == -1){
        perror("fork fail!");
    }
    else if(c_pid == 0){
        readfrompipeLogger();
    }
    else{
        char loggerStarted[100] = "Logger Started! on address ";
        strncat(loggerStarted, currserverSettings.IPAddress, 100);
        strncat(loggerStarted, " on port ", 100);
        strncat(loggerStarted, "1337\n", 100);

        writetoFork(loggerStarted);
        while(quit != 0){
            acceptmanyClients(boundSocket);
        }
    }
    unlink("/tmp/logger_fifo");
}

void writetoFork(char *buff){
    int code = mkfifo("/tmp/logger_fifo", 0666);
    if(code == -1){
        
    }
    int fd = open("/tmp/logger_fifo", O_WRONLY);
    if(fd == -1){
        perror("cannot open fifo");
        exit(0);
    }
    int len = strlen(buff);

    write(fd, (char *) &len, sizeof(int));
    write(fd, buff, strlen(buff));
    printf("writing bytes %d\n", fd);

}

void createThreads(int clientSocket, int i, pthread_t *tid){
    //create threads
    int checkThread = pthread_create(&tid[i], NULL, connectHandler, (void*)&clientSocket);
    writetoFork("Thread Created!\n");
    if (checkThread != 0){
        perror("Failed to create thread");
    }
    //join threads
    if(i >= 128){
        i = 0;
        while(i < 128){
            pthread_join(tid[i++], NULL);
        }
        i = 0;
    }
}


void *connectHandler(void *socketAddr){
    //semaphore?
    struct serverSettings currserverSettings;
    int clientSocket = *(int*)socketAddr;


    //mutex locks to prevent race conditions and such
    pthread_mutex_lock(&lock);
    readConfig(fopen("settings.conf", "rb"), &currserverSettings);
    pthread_mutex_unlock(&lock);

    pthread_mutex_lock(&lock);
    //ran out of time to add a client id
    //was going to do it by threadid but it was 8am
    writetoFork("settings file read by Client: \n");
    pthread_mutex_unlock(&lock);


    char *connected;
    //we read 1
    readBuffer(clientSocket, &connected);
    //SIGKILL and SIGTERM to kill child process

    printf("%s", connected);

    char *passwordAttempt;
    //we read 2
    readBuffer(clientSocket, &passwordAttempt);
    int passwordSuccess = validatePassword(passwordAttempt);
    free(passwordAttempt);

    int size;

    if(passwordSuccess == 0){
        writetoFork("Client signed in!\n");
        //alarm?
        size = strlen("Successful Login!");
        //we write 1
        writeClient(clientSocket, "Successful Login!", size);

        size = strlen(currserverSettings.motd);
        //we write 2
        writeClient(clientSocket, currserverSettings.motd, size);
        writetoFork("Sent message of the day to the client!\n");

        char *directoryBuffer;
        enumbFileshare(&currserverSettings, &directoryBuffer);
        size = strlen(directoryBuffer);
        //we write 3
        writeClient(clientSocket, directoryBuffer, size);
        writetoFork("Showed the client the file share directory!\n");
        free(directoryBuffer);
            

        char *downloadFilename;
        //we read 3
        readBuffer(clientSocket, &downloadFilename);
        char tempDfname[100] = "Recieved a filename to download: ";
        strncat(tempDfname, downloadFilename, 100);
        writetoFork(tempDfname);
        strcat(currserverSettings.fileshareDirectory, downloadFilename);
        free(downloadFilename);

        char *downloadBuffer;
        pthread_mutex_lock(&lock);
        size = downloadFile(fopen(currserverSettings.fileshareDirectory, "rb"), &downloadBuffer);
        pthread_mutex_unlock(&lock);
        //we write 4
        writeClient(clientSocket, downloadBuffer, size);
        writetoFork("Sent file to client!");
        free(downloadBuffer);
        
        char *uploadedfileName;
        //we read 4
        readBuffer(clientSocket, &uploadedfileName);
        char *uploadedfileContents;
        int filecontentsLen = 0;
        //we read 5
        filecontentsLen = readBuffer(clientSocket, &uploadedfileContents);

        char dirFname [100];
        strncpy(dirFname, currserverSettings.fileshareDirectory, 50);
        int dirfnameLen = strlen(dirFname);
        dirFname[dirfnameLen] = '/';
        strcat(dirFname, uploadedfileName);

        free(uploadedfileName);

        pthread_mutex_lock(&lock);
        reformFile(fopen(dirFname, "wb"), uploadedfileContents, filecontentsLen);
        pthread_mutex_unlock(&lock);

        char tempUfname[100] = "Recieved a filename to upload: ";
        strncat(tempUfname, uploadedfileName, 100);
        writetoFork(tempUfname);

        free(uploadedfileContents);
    }

    else if (passwordSuccess == -1){
        size = strlen("Unsuccessful Login!");
        writeClient(clientSocket, "Unsuccessful Login!\nDisconnected from server\n", size);
        writetoFork("Client login unseccessful\n");
        pthread_exit(NULL);
    }
    else{
        printf("An error occured!\n");
        pthread_exit(NULL);
    }

    pthread_exit(NULL);
}

//validate password

void reformFile(FILE *filePtr, char *buff, int fileLen){
    //fileLen+1 to account for the things removed initially in the readBuffer function
    //image will still be visiable even if the bytes are off but the preview won't work
    fwrite(buff, fileLen+1, 1, filePtr);
    fclose(filePtr);
}

void enumbFileshare(struct serverSettings *currserverSettings, char **buff){
    //strup?
    //array of pointers
    DIR *d;
    struct dirent *dir;
    d = opendir(currserverSettings->fileshareDirectory);
    if(d == NULL){
        printf("cannot open directory\n");
        //quit gracefully?
    }
    if(d){
        int directoryLength = 0;
        *buff = (char *) malloc(directoryLength * sizeof(char));
        //get all directory contents
        while((dir = readdir(d))){
            int currnameSize = strlen(dir->d_name);
            directoryLength += currnameSize +1;
            //dynamically grow array for files, useful for when multiple clients are adding files at the same time
            char *currName = dir->d_name;
            *buff = (char *) realloc(*buff, ((directoryLength) * sizeof(char))+1);
            strcpy(*buff+(directoryLength-currnameSize-1), currName);
            (*buff)[directoryLength-1] = '\n';
        }
        closedir(d);
    }
}

int downloadFile(FILE *filePtr, char **buff){
    //perror is really good
    fseek(filePtr, 0, SEEK_END);
    int len = ftell(filePtr);
    rewind(filePtr);

    //it is bytes therefore no +1 is required
    //grow dynamically
    *buff = (char *) malloc((len + 1) * sizeof(char));

    fread(*buff, len, 1, filePtr);
    fclose(filePtr);

    return len;
}


void exitThread(int detectRead){
    //kill thread if nothing is being read from the client
    //not the best way of doing it, but it works
    if(detectRead == 0){
        writetoFork("Client disconnected\n");
        pthread_exit(NULL);
    }
}


int readBuffer(int socket, char **buff){
    int len;
    //read from the client and dynamically grow the buffer for the input size recieved
    int detectRead = read(socket, (char *) &len, sizeof(int));
    exitThread(detectRead);
    pthread_mutex_lock(&lock);
    *buff = (char *) malloc(len * sizeof(char)+1);
    pthread_mutex_unlock(&lock);
    (*buff)[len] = '\0';
    detectRead = read(socket, *buff, len);
    exitThread(detectRead);
    return len;
}

void writeClient(int socket, char *buff, int size){
    //write some stuff to the client
    write(socket, (char *) &size, sizeof(int));
    write(socket, buff, size);
}


int checkifExits(FILE *configFile){
    int returnCheck;
    if(configFile == NULL){
        printf("This file does not exist!\n");
        //write to logger
        logger("The file does not exist ", fopen("log.dongs", "a"));
        returnCheck = -1;
    }
    //check if readable code for posix, include in the header file please
    else if (configFile != NULL){
        returnCheck = 0;
    }
    return returnCheck;
}

void isitReable(char* buff){
    //yea posix again
    if(access(buff, R_OK) == -1){
        printf("\nFile is Not readable!\n");
        logger("Program exited ", fopen("log.dongs", "a"));
        exit(0);
    }  
}

//file IO
void readConfig(FILE *configFile, struct serverSettings *currconfigSettings){
    fread(currconfigSettings, sizeof(*currconfigSettings), 1, configFile);
    fclose(configFile);
}

void writeConfig(FILE *configFile, struct serverSettings *currconfigSettings){
    fwrite(currconfigSettings, sizeof(*currconfigSettings), 1, configFile);
    fclose(configFile);
}