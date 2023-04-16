#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

struct Student {
    int studentID;
    char name[50];
    char dateOfBirth[20];
    float grade;
};

int main(int argc, char *argv[]) {
    int client_sock = 0;
    struct sockaddr_in server_address;
    struct Student student_info;
    char buffer[MAX_BUFFER_SIZE] = {0};

    if (argc != 3) {
        printf("Usage: %s <IP address> <port>\n", argv[0]);
        return -1;
    }

    // create a socket
    if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    // set server address and port
    memset(&server_address, '0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(argv[1]);
    server_address.sin_port = htons(atoi(argv[2]));

    // connect to server
    if (connect(client_sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    // get student information from user
    printf("Enter student information:\n");
    printf("Student ID: ");
    scanf("%d", &student_info.studentID);
    printf("Name: ");
    scanf("%s", student_info.name);
    printf("Date of Birth (YYYY-MM-DD): ");
    scanf("%s", student_info.dateOfBirth);
    printf("Grade: ");
    scanf("%f", &student_info.grade);

    // encapsulate student information in buffer
    int buffer_size = sprintf(buffer, "%d,%s,%s,%.2f", student_info.studentID, student_info.name, student_info.dateOfBirth, student_info.grade);

    // send buffer to server
    if (send(client_sock, buffer, buffer_size, 0) < 0) {
        printf("Message send failed\n");
        return -1;
    }

    // receive response from server
    int num_bytes_received = recv(client_sock, buffer, MAX_BUFFER_SIZE, 0);
    if (num_bytes_received < 0) {
        printf("Message receive failed\n");
        return -1;
    }

    // print response from server
    buffer[num_bytes_received] = '\0';
    printf("Server response: %s\n", buffer);

    // close socket
    close(client_sock);

    return 0;
}