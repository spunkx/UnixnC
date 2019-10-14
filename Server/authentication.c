#include "authentication.h"


//generateHash
//does the file exist?
//if file doesn't exist create it
//create first default user
//generate their hash with the default password

//authenticate
//grab password hash from file
//put into structure
//compare hashed user input to the hash from the file
//if it matches return an integer
//if it doesn't match kill the current client using threadid


int validatePassword(char *passwordAttempt){
    //heh
    char rightPassword[] = "dongs";
    int passwordSuccesss = -1;
    if(strcmp(rightPassword, passwordAttempt) == 0){
        passwordSuccesss = 0;
    }
    else{
        passwordSuccesss = -1;
    }

    return passwordSuccesss;
}