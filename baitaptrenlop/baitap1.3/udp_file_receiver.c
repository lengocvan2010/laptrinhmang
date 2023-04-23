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
    int port;
    printf("Nhap cong: ");
    scanf("%d",&port);
    int receiver = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    bind(receiver, (struct sockaddr *)&addr, sizeof(addr));
    FILE *f;
    char buf[256];
    struct sockaddr_in sender_addr;
    int sender_addr_len = sizeof(sender_addr);

    while (1)
    {
        int ret = recvfrom(receiver, buf, sizeof(buf), 0,(struct sockaddr *)&sender_addr, &sender_addr_len);
        if (ret < sizeof(buf))
            buf[ret] = 0;
        f = fopen("output.txt","w");
        fprintf(f,"%s",buf);
        if (f != NULL) {
        printf("Đa nhan du lieu va ghi vao file: output.txt!\n");
      return 1;
   }
    }
}