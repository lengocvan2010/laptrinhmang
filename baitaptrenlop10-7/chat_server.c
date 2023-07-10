#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    int socket;
    char nickname[20];
    bool isOwner;
} Client;

Client clients[MAX_CLIENTS];
int clientCount = 0;

void broadcastMessage(char *message) {
    for (int i = 0; i < clientCount; i++) {
        write(clients[i].socket, message, strlen(message));
    }
}

void handleJoinCommand(Client *client, char *command) {
    sscanf(command, "JOIN %s", client->nickname);
    char message[BUFFER_SIZE];
    sprintf(message, "JOIN %s\n", client->nickname);
    broadcastMessage(message);
}

void handleMsgCommand(Client *client, char *command) {
    char message[BUFFER_SIZE];
    sprintf(message, "MSG %s: %s\n", client->nickname, command + 4);
    broadcastMessage(message);
}

void handlePMsgCommand(Client *client, char *command) {
    char recipient[20], message[BUFFER_SIZE];
    sscanf(command, "PMSG %s %[^\n]", recipient, message);
    sprintf(message, "PMSG %s %s\n", client->nickname, message);
    for (int i = 0; i < clientCount; i++) {
        if (strcmp(clients[i].nickname, recipient) == 0) {
            write(clients[i].socket, message, strlen(message));
            break;
        }
    }
}

void handleOpCommand(Client *client, char *command) {
    char newOwner[20];
    sscanf(command, "OP %s", newOwner);
    for (int i = 0; i < clientCount; i++) {
        if (strcmp(clients[i].nickname, newOwner) == 0) {
            clients[i].isOwner = true;
            break;
        }
    }
    char message[BUFFER_SIZE];
    sprintf(message, "OP %s\n", newOwner);
    broadcastMessage(message);
}

void handleKickCommand(Client *client, char *command) {
    char kickedUser[20], opUser[20];
    sscanf(command, "KICK %s %s", kickedUser, opUser);
    for (int i = 0; i < clientCount; i++) {
        if (strcmp(clients[i].nickname, kickedUser) == 0) {
            char message[BUFFER_SIZE];
            sprintf(message, "KICK %s %s\n", kickedUser, opUser);
            write(clients[i].socket, message, strlen(message));
            close(clients[i].socket);
            for (int j = i; j < clientCount - 1; j++) {
                clients[j] = clients[j + 1];
            }
            clientCount--;
            break;
        }
    }
}

void handleTopicCommand(Client *client, char *command) {
    char topic[BUFFER_SIZE], opUser[20];
    sscanf(command, "TOPIC %s %[^\n]", opUser, topic);
    char message[BUFFER_SIZE];
    sprintf(message, "TOPIC %s %s\n", opUser, topic);
    broadcastMessage(message);
}

void handleQuitCommand(Client *client) {
    char message[BUFFER_SIZE];
    sprintf(message, "QUIT %s\n", client->nickname);
    broadcastMessage(message);
    close(client->socket);
    for (int i = 0; i < clientCount; i++) {
        if (clients[i].socket == client->socket) {
            for (int j = i; j < clientCount - 1; j++) {
                clients[j] = clients[j + 1];
            }
            clientCount--;
            break;
        }
    }
}

void handleClientCommand(Client *client, char *command) {
    if (strncmp(command, "JOIN", 4) == 0) {
        handleJoinCommand(client, command);
    } else if (strncmp(command, "MSG", 3) == 0) {
        handleMsgCommand(client, command);
    } else if (strncmp(command, "PMSG", 4) == 0) {
        handlePMsgCommand(client, command);
    } else if (strncmp(command, "OP", 2) == 0) {
        handleOpCommand(client, command);
    } else if (strncmp(command, "KICK", 4) == 0) {
        handleKickCommand(client, command);
    } else if (strncmp(command, "TOPIC", 5) == 0) {
        handleTopicCommand(client, command);
    } else if (strncmp(command, "QUIT", 4) == 0) {
        handleQuitCommand(client);
    }
}

void *clientHandler(void *arg) {
    Client client = *(Client *)arg;
    char buffer[BUFFER_SIZE];
    while (true) {
        int bytesRead = read(client.socket, buffer, BUFFER_SIZE - 1);
        if (bytesRead <= 0) {
            handleQuitCommand(&client);
            break;
        }
        buffer[bytesRead] = '\0';
        handleClientCommand(&client, buffer);
    }
    return NULL;
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientLength;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Error opening socket");
        exit(1);
    }

    memset((char *)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(12345);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Error binding");
        exit(1);
    }

    listen(serverSocket, MAX_CLIENTS);
    printf("Server started. Listening on port 12345...\n");

    while (true) {
        clientLength = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
        if (clientSocket < 0) {
            perror("Error accepting connection");
            exit(1);
        }

        if (clientCount < MAX_CLIENTS) {
            Client newClient;
            newClient.socket = clientSocket;
            newClient.isOwner = false;
            clients[clientCount] = newClient;
            clientCount++;
            pthread_t thread;
            pthread_create(&thread, NULL, clientHandler, (void *)&clients[clientCount - 1]);
        } else {
            char *message = "Server is full. Please try again later.\n";
            write(clientSocket, message, strlen(message));
            close(clientSocket);
        }
    }

    close(serverSocket);
    return 0;
}
