#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int server_sock = 0, client_sock = 0;
    struct sockaddr_in server_address, client_address;
    char buffer[MAX_BUFFER_SIZE] = {0};

    if (argc != 4) {
        printf("Usage: %s <port> <hello string> <file name>\n", argv[0]);
        return -1;
    }

    // create a socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    // set server address and port
    memset(&server_address, '0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(atoi(argv[1]));

    // bind socket to server address and port
    if (bind(server_sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        printf("Bind failed\n");
        return -1;
    }

    // listen for incoming connections
    if (listen(server_sock, 3) < 0) {
        printf("Listen failed\n");
        return -1;
    }

    // accept incoming connections
    socklen_t client_address_len = sizeof(client_address);
    if ((client_sock = accept(server_sock, (struct sockaddr *)&client_address, &client_address_len)) < 0) {
        printf("Accept failed\n");
        return -1;
    }

    // send hello string to client
    if (send(client_sock, argv[2], strlen(argv[2]), 0) < 0) {
        printf("Message send failed\n");
        return -1;
    }

    // open file for writing
    FILE *file = fopen(argv[3], "w");
    if (file == NULL) {
        printf("File open failed\n");
        return -1;
    }

    // receive data from client and save to file
    int num_bytes_received = 0;
    while ((num_bytes_received = recv(client_sock, buffer, MAX_BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, num_bytes_received, file);
        memset(buffer, 0, sizeof(buffer));
    }

    // close file and sockets
    fclose(file);
    close(client_sock);
    close(server_sock);

    return 0;
}