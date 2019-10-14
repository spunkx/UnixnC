#include "networking.h"

int connectClient(char *currserverIP, int currPort){
    struct sockaddr_in serverAddress;
    int returnValue = 0;

    int connectSocket;
    if((connectSocket=socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0){
        perror("ERROR creating socket");
    }
    else{
        serverAddress.sin_family = AF_INET;
        in_addr_t serverIP;
        inet_pton(AF_INET, currserverIP, &serverIP);
        serverAddress.sin_addr.s_addr = serverIP;
        serverAddress.sin_port = htons(currPort);

        if(connect(connectSocket, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in)) < 0){
            perror("ERROR connection");
        }

        returnValue = connectSocket;
        //close(connectSocket);

    }

    /*
    Just realised you are also creating the listen socket in your networking function
    This means every time a client connects it will try to rebind the port
    Need 2 functions. 1 for creating the listen socket and another to wait for a client
    */

    return returnValue;
}