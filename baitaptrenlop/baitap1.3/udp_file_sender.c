#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    char IP[15];
    int port;
    char name_file[100];
    printf("Nhap IP: ");
    scanf("%s",IP);
     printf("Nhap cong: ");
    scanf("%d",&port);

    int sender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP);
    addr.sin_port = htons(port);
    printf("Nhap ten file: ");
    scanf("%s",name_file);
    FILE *f = fopen(name_file, "r");
    char buf[256];
    fscanf(f,"%s",buf);
    int ret = sendto(sender, buf, strlen(buf), 0,(struct sockaddr *)&addr, sizeof(addr));
    printf("%d bytes sent.\n", ret);
    
}