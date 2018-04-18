/*
    C ECHO client example using sockets
*/
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <sys/time.h>

long int getTime(char response[]);

int main(int argc , char *argv[])
{
    int sock, read_size;
    struct sockaddr_in server;
    char message[1000] , server_reply[5000];
    clock_t start, end;
    char fname[64];
    struct timeval tv1, tv2;

    printf("Type the file name:\n");
    scanf("%s", fname);
    fgetc(stdin);

    FILE *f = fopen(strcat(fname, ".txt"), "w");

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    //gets server confirmation messages
    for (int i = 0; i < 1; i++) {
      //Receive a reply from the server
      if( (read_size = recv(sock , server_reply , 5000 , 0)) < 0)
      {
          puts("recv failed");
          break;
      }
      server_reply[read_size] = '\0';

      puts("Server reply:");
      puts(server_reply);
    }

    //keep communicating with server
    while(1)
    {
        printf("Enter message: ");
        fgets(message, sizeof(message), stdin);

        if(strcmp(message, "stop\n") == 0)
            break;

        gettimeofday(&tv1, NULL);
        //Send some data
        if( send(sock , message , strlen(message) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }

        //Receive a reply from the server
        if( (read_size = recv(sock , server_reply , 5000 , 0)) < 0)
        {
            puts("recv failed");
            break;
        }

        //Write time in file
        gettimeofday(&tv2, NULL);
        server_reply[read_size] = '\0';
        fprintf(f, strcat(strcat(message, "Time taken: %ld\n"), "Server processing: %ld\n"),
                tv2.tv_usec-tv1.tv_usec, getTime(server_reply));

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
