#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <time.h>


#define PORT 5000
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1023

int main(int argc, char *argv[]) {
    int server_socket, new_socket, max_sd, sd, activity, valread;
    struct sockaddr_in address;
    fd_set readfds;
    char buffer[BUFFER_SIZE];

    // Tạo socket của server
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Cấu hình địa chỉ và port của server
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    int addrlen = sizeof(server_socket);

    // Gán địa chỉ và port cho socket của server
    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Binding error");
        exit(EXIT_FAILURE);
    }

    // Lắng nghe kết nối từ các client
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Listening error");
        exit(EXIT_FAILURE);
    }

    // Khởi tạo danh sách các socket kết nối đến server
    int client_socket[MAX_CLIENTS] = {0};

    // Chờ các kết nối đến và xử lý dữ liệu
    while (1) {
        FD_ZERO(&readfds);

        // Thêm socket của server vào danh sách
        FD_SET(server_socket, &readfds);
        max_sd = server_socket;

        // Thêm các socket của client vào danh sách
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_socket[i];

            if (sd > 0)
                FD_SET(sd, &readfds);

            if (sd > max_sd)
                max_sd = sd;
        }

        // Sử dụng hàm select() để chờ sự kiện xảy ra trên các socket
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            perror("Select error");
        }

        // Kiểm tra sự kiện xảy ra trên socket của server
        if (FD_ISSET(server_socket, &readfds)) {
            if ((new_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t*) &addrlen)) < 0) {
                perror("Accept error");
                exit(EXIT_FAILURE);
            }

            // Gửi yêu cầu tên của client đến socket mới kết nối
            char *message = "Enter your name? ";
            if (send(new_socket, message, strlen(message), 0) != strlen(message)) {
                perror("Send error");
            }

            // Thêm socket mới kết nối vào danh sách các socket
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    break;
               
        }
    }

    // Kiểm tra sự kiện xảy ra trên các socket của client
    for (int i = 0; i < MAX_CLIENTS; i++) {
        sd = client_socket[i];
        time_t rawtime;
        struct tm *timeinfo;
        char timestr[50];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(timestr, sizeof(timestr), "%Y/%m/%d %I:%M:%S%p", timeinfo);

        if (FD_ISSET(sd, &readfds)) {
            // Xử lý dữ liệu nhận được từ socket của client
            if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0) {
                // Đóng kết nối nếu client đã ngắt kết nối
                getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                printf("Client disconnected, ip %s, port %d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                close(sd);
                client_socket[i] = 0;
            } else {
                // Xử lý tên của client nếu client chưa gửi tên
                if (strlen(buffer) > 0 && strncmp(buffer, "client_id: ", 11) == 0) {
                    char client_name[BUFFER_SIZE] = {0};
                    strncpy(client_name, buffer + 11, strlen(buffer) - 11 );
                    printf("Client %s joined\n", client_name);

                    // Gửi thông báo cho các client khác về việc client mới đã tham gia
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (client_socket[j] != 0 && client_socket[j] != sd) {
                            char join_message[2000];
                            sprintf(join_message, "%s %s: joined the chat\n",timestr, client_name);
                            send(client_socket[j], join_message, strlen(join_message), 0);
                        }
                    }
                } else {
                    // Gửi dữ liệu từ client đến các client khác
                    getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                    char sender_name[BUFFER_SIZE] = {0};
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (client_socket[j] == sd) {
                            strncpy(sender_name, buffer, j + 1);
                            break;
                        }
                    }

                    char message[5000];
                    
                    sprintf(message, "%s %s: %s",timestr, sender_name, buffer);
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (client_socket[j] != 0 && client_socket[j] != sd) {
                            send(client_socket[j], message, strlen(message), 0);
                        }
                    }
                }
            }
        }
    }
        }
}
 return 0;
 }


