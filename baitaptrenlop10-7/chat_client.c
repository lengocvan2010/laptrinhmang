#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_address;

    // Tạo socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("ERROR: Failed to create socket");
        return 1;
    }

    // Thiết lập địa chỉ server
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(12345);
    if (inet_pton(AF_INET, "127.0.0.1", &(server_address.sin_addr)) <= 0) {
        perror("ERROR: Failed to set server address");
        return 1;
    }

    // Kết nối tới server
    if (connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        perror("ERROR: Failed to connect to server");
        return 1;
    }

    // Gửi tin nhắn từ client tới server
    char message[BUFFER_SIZE];

    while (1) {
      
        fgets(message, BUFFER_SIZE, stdin);

        if (strncmp(message, "quit", 4) == 0) {
            break;
        }

        if (write(client_socket, message, strlen(message)) < 0) {
            perror("ERROR: Failed to send message to server");
            return 1;
        }

        // Đọc phản hồi từ server
        char response[BUFFER_SIZE];
        ssize_t bytes_read = read(client_socket, response, BUFFER_SIZE - 1);
        if (bytes_read < 0) {
            perror("ERROR: Failed to read response from server");
            return 1;
        }

        response[bytes_read] = '\0';
        printf("Response from server: %s\n", response);
    }

    // Đóng kết nối
    close(client_socket);

    return 0;
}
