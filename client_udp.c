/*
    C ECHO client example using sockets
*/
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <sys/time.h>

long int getTime(char response[]);

#define SERVER "127.0.0.1"
#define TIMEOUT_SEC 1
#define PORT 8888
#define MESSAGE_SIZE 1000
#define SERVER_REPLY_SIZE 5000

int main(int argc , char *argv[])
{
    struct sockaddr_in server;
    int sock, read_size, slen = sizeof(server);
    char message[MESSAGE_SIZE] , server_reply[SERVER_REPLY_SIZE];
    clock_t start, end;
    char fname[64];
    struct timeval tv_timeout, tv1, tv2;

    printf("Type the file name:\n");
    scanf("%s", fname);
    fgetc(stdin);

    FILE *f = fopen(strcat(fname, ".txt"), "w");

    //Create socket
    sock = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP);
    tv_timeout.tv_sec = TIMEOUT_SEC;
    tv_timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv_timeout, sizeof(tv_timeout)) < 0) {
        puts("Could not set timeout for socket");
        return 1;
    }
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //initialize server structure
    memset((char *) &server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons( PORT );
    if (inet_aton(SERVER , &server.sin_addr) == 0) {
        puts("inet_aton() failed");
        return 1;
    }

    //first contact with server:
    strcpy(message, "help");
    if (sendto(sock, message, strlen(message), 0, (struct sockaddr*) &server, slen) == -1){
        puts("Send failed");
        return 1;
    }
    if ((read_size = recvfrom(sock, server_reply, SERVER_REPLY_SIZE, 0, (struct sockaddr *) &server, &slen)) == -1){
        puts("Receive failed");
        return 1;
    }
    server_reply[read_size] = '\0';
    puts(server_reply);

    //Communicate with server
    while(1)
    {
        printf("Enter message: ");
        fgets(message, sizeof(message), stdin);

        //remove '\n' in the end
        message[strlen(message)-1]='\0';

        if(strcmp(message, "stop") == 0)
            break;

        gettimeofday(&tv1, NULL);

        //Send some data
        if (sendto(sock, message, strlen(message), 0, (struct sockaddr*) &server, slen) == -1){
            puts("Send failed");
            return 1;
        }

        //Receive some data
        if ((read_size = recvfrom(sock, server_reply, SERVER_REPLY_SIZE, 0, (struct sockaddr *) &server, &slen)) == -1){
            puts("Server did not respond. Try again.");
            continue;
        }
        server_reply[read_size] = '\0';

        //Write time in file
        gettimeofday(&tv2, NULL);

        fprintf(f, strcat(strcat(strcat(message, "Total time taken: %ld\n"), "Server processing: %ld\n"), "Connection time: %ld\n"),
                tv2.tv_usec-tv1.tv_usec, getTime(server_reply), (tv2.tv_usec-tv1.tv_usec)-getTime(server_reply));

        puts("Server reply:");
        puts(server_reply);
    }

    fclose(f);
    close(sock);
    return 0;
}

long int getTime(char response[]){
    long int res = 0;

    for(int i = 0;response[i] >= '0' && response[i] <= '9';i++){
        res = res*10;
        res = res + response[i] - '0';
    }

    return res;
}
