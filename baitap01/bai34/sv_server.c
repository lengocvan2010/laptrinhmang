#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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

void log_student_info(struct sockaddr_in client_address, struct Student student_info, char* log_file) {
    time_t current_time;
    char timestamp[20];
    FILE* fp;

    // get current time
    current_time = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&current_time));

    // open log file in append mode
    fp = fopen(log_file, "a");
    if (fp == NULL) {
        printf("Failed to open log file\n");
        return;
    }

    // write student information to log file
    fprintf(fp, "%s %s %d %s %.2f\n", inet_ntoa(client_address.sin_addr), timestamp, student_info.studentID, student_info.name, student_info.grade);

    // close log file
    fclose(fp);
}

int main(int argc, char *argv[]) {
    int server_sock = 0, client_sock = 0;
    struct sockaddr_in server_address, client_address;
    struct Student student_info;
    char buffer[MAX_BUFFER_SIZE] = {0};
    int opt = 1;
    int addrlen = sizeof(server_address);

    if (argc != 3) {
        printf("Usage: %s <port> <log file>\n", argv[0]);
        return -1;
    }

    // create a socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        printf("Socket creation error\n");
        return -1;
    }

    // set server address and port
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        printf("Setsockopt error\n");
        return -1;
    }
    memset(&server_address, '0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(atoi(argv[1]));

    // bind socket to address and port
    if (bind(server_sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        printf("Bind failed\n");
        return -1;
    }

    // listen for incoming connections
    if (listen(server_sock, 3) < 0) {
        printf("Listen failed\n");
        return -1;
    }

    while (1) {
        // accept incoming connection
        if ((client_sock = accept(server_sock, (struct sockaddr *)&client_address, (socklen_t*)&addrlen)) < 0) {
            printf("Accept failed\n");
            continue;
        }

        // receive data from client
        int num_bytes_received = recv(client_sock, buffer, MAX_BUFFER_SIZE, 0);
        if (num_bytes_received < 0) {
            printf("Message receive failed\n");
            close(client_sock);
            continue;
        }

        // extract student information from buffer
        char* token;
        token = strtok(buffer, ",");
        student_info.studentID = atoi(token);
        token = strtok(NULL, ",");
        strcpy(student_info.name, token);
        token = strtok(NULL, ",");
        strcpy(student_info.dateOfBirth, token);
        token = strtok(NULL, ",");
        student_info.grade = atof(token);
        // log student information to file
        log_student_info(client_address, student_info, argv[2]);

        // send response back to client
        char response[] = "Student information received successfully\n";
        send(client_sock, response, strlen(response), 0);

        // close connection to client
        close(client_sock);
    }
    return 0;
}