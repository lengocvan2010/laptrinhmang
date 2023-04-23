#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// Client gửi file sang server

int main()
{
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect() failed");
        return 1;
    }


    FILE *f = fopen("input.txt", "r");

    char buf[100];
    int pos=0;
    while (!feof(f))
    {
        ret = fread(buf+pos, 1, sizeof(buf), f);
        if (ret <=0)
            break;
        if (ret== sizeof(buf)){
            send(client, buf, ret, 0);
            memset(buf, 0, ret);
        }
        
    }
    
    buf[ret]=0;
    send(client, buf, ret, 0);
    printf("Truyen du lieu thanh cong!\n");
    fclose(f);
    close(client);
}