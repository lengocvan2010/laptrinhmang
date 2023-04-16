#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sock = 0;
    struct sockaddr_in server_address;
    char buffer[MAX_BUFFER_SIZE] = {0};

    if (argc != 3) {
        printf("Usage: %s <IP address> <port>\n", argv[0]);
        return -1;
    }

    // create a socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    // set server address and port
    memset(&server_address, '0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &server_address.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        return -1;
    }

    // connect to server
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    // get input from keyboard and send to server
    while (1) {
        printf("Enter message: ");
        fgets(buffer, MAX_BUFFER_SIZE, stdin);

        if (strlen(buffer) == 1) {
            continue;
        }

        // send message to server
        if (send(sock, buffer, strlen(buffer), 0) < 0) {
            printf("Message send failed\n");
            break;
        }

        // clear buffer
        memset(buffer, 0, sizeof(buffer));
    }

    // close socket
    close(sock);

    return 0;
}