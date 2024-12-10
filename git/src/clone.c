#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) { perror(msg); exit(0); }

int clone(const char* url) {
    char* host = "github.com";
    int portno = 9418;
    char* message = "GET /Bbeluco/QueryMysqlCpp.git/info/refs?service=git-upload-pack HTTP/1.1\r\n\r\n";

    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total;
    char response[4096];
    
    printf("Request:\n%s\n", message);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        error("Error while opening socket");
    }

    server = gethostbyname(host);
    if(server == NULL) {
        error("ERROR no such host");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);

    memcpy(&serv_addr.sin_addr.s_addr,server->h_addr, server->h_length);

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }

    total = strlen(message);
    sent = 0;
    do {
        bytes = write(sockfd, message+sent, total-sent);
        if(bytes < 0) {
            error("ERROR writing message to socket");
        }
        if(bytes == 0) {
            break;
        }

        sent+=bytes;
    }while(sent < total);

    memset(response, 0, sizeof(response));
    total = sizeof(response) - 1;
    received = 0;
    do {
        bytes = read(sockfd, response+received, total-received);
        if(bytes < 0) {
            error("ERROR reading message to socket");
        }
        if(bytes == 0) {
            break;
        }
    }while(received < total);

    if(received == total) {
        error("ERROR storing complete response from socket");
    }

    close(sockfd);

    printf("Response:\n%s\n", response);

    return 0;
}