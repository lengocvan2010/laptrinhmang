#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 5000
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE] = {0};

    // Tạo socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Failed to create socket\n");
        return -1;
    }

    // Thiết lập thông tin địa chỉ server
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_ADDRESS, &server_address.sin_addr) <= 0) {
        printf("Invalid address or address not supported\n");
        return -1;
    }

    // Kết nối tới server
    if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    
    
    // Nhận và hiển thị dữ liệu từ server
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(sock, buffer, BUFFER_SIZE);
        printf("Server send: %s\n", buffer);
        printf("reply: ");
        fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = 0;

    // Gửi tên của client tới server
    char client_id[1035];
    sprintf(client_id, "client_id: %s", buffer);
    send(sock, client_id, strlen(client_id), 0);

    }

    return 0;
}
