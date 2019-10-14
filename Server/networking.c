#include "networking.h"
#include "logger.h"


int createSocket(int portNumber){
    struct sockaddr_in serverAddress;

    int boundFlag = 0;
    int listenSocket;
    if((listenSocket=socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0 ){
        perror("ERROR creating socket");
    }

    else{
        //set up the information for the socket structure from the socket header/library
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(portNumber);

        int flag = 1;
        //stop binding error
        setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(int));
        //bind socket
        boundFlag = bind(listenSocket, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in));
        
    }
    if (boundFlag < 0){
        perror("Error on binding");
    }

    return listenSocket;
}

void acceptmanyClients(int listenSocket){
    struct sockaddr_in clientAddress;
    //listen but limit connections to 128
    if(listen(listenSocket, 128) == 0){
    //handle signal
    //moving threads
        struct timeval timer;
        timer.tv_sec = 59;
        timer.tv_usec = 0;
        int clientSocket;
        unsigned int clientSize = sizeof(struct sockaddr_in);
        int i = 0;
        pthread_t tid[160];
        //while clients are connecting keep going!
        while((clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddress, &clientSize))){
            //set a timeout for clients
            setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timer, sizeof timer);
            printf("Client Connected!\n");
            writetoFork("Client Conencted!\n");
        
            createThreads(clientSocket, i, tid);
            if(clientSocket < 0){
                perror("ERROR on Accept");
                break;
            }

        }
    }
    else{
        perror("ERROR on listen or too many connections");
    }
    //0 is testing the state of the clientsocket

}
