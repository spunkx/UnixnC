#include "networking.h"

struct configSettings {
    char username[50];
    int serverPort;
    char serverIP[50];
    char downloadsDirectory[50];
    char uploadsDirectory[50];

};

int checkifExits(FILE *configFile);
void readConfig(FILE *configFile, struct configSettings *currconfigSettings);
void writeConfig(FILE *configFile, struct configSettings *currconfigSettings);
int readBuffer(int socket, char **buff);
void writeClient(int socket, char *buff, int size);
void stripBuffer(char *buff);
void enumbFileshare(struct configSettings *currconfigSettings, char **buff);
int uploadFile(FILE *filePtr, char **buff);

int main(){
    //struct date *object2=malloc(sizeof(struct date));
    //attempt to read file
    int returnCheck = checkifExits(fopen("settings.conf", "r"));

    struct configSettings currconfigSettings;
    strncpy(currconfigSettings.username, "defaultusername", 50);
    currconfigSettings.serverPort = 1337;
    strncpy(currconfigSettings.serverIP, "127.0.0.1", 50);
    strncpy(currconfigSettings.downloadsDirectory, "downloads", 50);
    strncpy(currconfigSettings.uploadsDirectory, "uploads", 50);

    //consolidating the calls to read and write because it's easier to look at
    //
    if(returnCheck == 0){
        readConfig(fopen("settings.conf", "rb"), &currconfigSettings);
    }
    else if(returnCheck == -1){
        writeConfig(fopen("settings.conf", "wb"), &currconfigSettings);
    }
    else{
        printf("An error occured");
    }

    char password[51];

    int connectSocket = connectClient(currconfigSettings.serverIP, currconfigSettings.serverPort);
    
    
    int size = strlen("Client Connected!");
    //write to read 1
    writeClient(connectSocket, password, size);
    
    printf("Please enter the password for the server: ");
    fgets(password, 50, stdin);
    stripBuffer(password);
    size = strlen(password);
    //write to read 2
    writeClient(connectSocket, password, size);
    
    char *successful;
    //read write 1
    readBuffer(connectSocket, &successful);
    printf("%s\n", successful);

    char *motd;
    //read write 2
    readBuffer(connectSocket, &motd);
    printf("Message of the day: %s\n", motd);

    //read write 3
    char *fileshareDirectory;
    readBuffer(connectSocket, &fileshareDirectory);
    printf("Fileshare contents on server: %s\n", fileshareDirectory);
    free(fileshareDirectory);


    char downloadFname[51];
    //write to read 3
    printf("What file would you like to download?\n");
    fgets(downloadFname, 50, stdin);
    stripBuffer(downloadFname);
    size = strlen(downloadFname);
    writeClient(connectSocket, downloadFname, size);

    char *contentsBytes;
    //read write 4
    readBuffer(connectSocket, &contentsBytes);
    printf("Current file contents below %s\n", contentsBytes);
    //call to assemble file here
    free(contentsBytes);

    char *uploadsDirectory;
    char uploadfileName[51];
    char *fileBuff;
    enumbFileshare(&currconfigSettings, &uploadsDirectory);
    printf("Your current upload directory: %s\n", uploadsDirectory);
    free(uploadsDirectory);

    printf("What file would you like to upload?\n");
    fgets(uploadfileName, 50, stdin);
    stripBuffer(uploadfileName);

    char dirFname [100];
    strncpy(dirFname, currconfigSettings.uploadsDirectory, 50);
    int dirfnameLen = strlen(dirFname);
    dirFname[dirfnameLen] = '/';
    strcat(dirFname, uploadfileName);

    //write to read 4
    size = strlen(uploadfileName);
    writeClient(connectSocket, uploadfileName, size);

    //write to read 5
    size = uploadFile(fopen(dirFname, "rb"), &fileBuff);
    writeClient(connectSocket, fileBuff, size);
    free(fileBuff);

    free(successful);
    close(connectSocket);
}

/*
void reformFile(){

}*/

int readBuffer(int socket, char **buff){
    //same read function as server
    int len;
    read(socket, (char *) &len, sizeof(int));
    *buff = (char *) malloc(len * sizeof(char)+1);
    (*buff)[len] = '\0';
    read(socket, *buff, len);
    return len;
}

void writeClient(int socket, char *buff, int size){
    //same write function as server
    write(socket, (char *) &size, sizeof(int));
    write(socket, buff, size);
}

void enumbFileshare(struct configSettings *currconfigSettings, char **buff){
    //strup
    //array of pointers
    DIR *d;
    struct dirent *dir;
    d = opendir(currconfigSettings->uploadsDirectory);
    if(d == NULL){
        printf("cannot open directory\n");
    }
    if(d){
        int directoryLength = 0;
        //list directories, grow dynamically
        *buff = (char *) malloc(directoryLength * sizeof(char));
        while((dir = readdir(d))){
            int currnameSize = strlen(dir->d_name);
            directoryLength += currnameSize +1;
            char *currName = dir->d_name;
            *buff = (char *) realloc(*buff, ((directoryLength) * sizeof(char))+1);
            strcpy(*buff+(directoryLength-currnameSize-1), currName);
            (*buff)[directoryLength-1] = '\n';
        }
        closedir(d);
    }
}

int uploadFile(FILE *filePtr, char **buff){
    //yea upload stuff
    fseek(filePtr, 0, SEEK_END);
    int len = ftell(filePtr);
    rewind(filePtr);
    *buff = (char *) malloc((len+1) * sizeof(char));
    fread(*buff, len, 1, filePtr);    //DO NOT, I REPEAT DO NOT FORGET THE POINTER!
    fclose(filePtr);

    return len;
}

void stripBuffer(char *buff){
    //get rid of that newline!
    int length = strlen(buff);
    buff[length-1] = '\0';
}

int checkifExits(FILE *configFile){
    int returnCheck;
    if(configFile == NULL){
        printf("This file does not exist!\nCreating file...\n");
        returnCheck = -1;
    }
    //check if readable code for posix, include in the header file please
    else if (configFile != NULL){
        printf("This does exist!\n");
        returnCheck = 0;
    }
    return returnCheck;
}

void readConfig(FILE *configFile, struct configSettings *currconfigSettings){
    fread(currconfigSettings, sizeof(*currconfigSettings), 1, configFile);
    fclose(configFile);
}

void writeConfig(FILE *configFile, struct configSettings *currconfigSettings){
    //fwrite is very nice as it seems to just put my struct out and then fread reads it back
    //not sure what magic this is but I'm glad it works!
    fwrite(currconfigSettings, sizeof(*currconfigSettings), 1, configFile);
    fclose(configFile);
}