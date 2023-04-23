#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>

int main() 
{
int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

     if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) 
    {
        perror("bind() failed");
        return 1;
    }

    if (listen(listener, 5)) 
    {
        perror("listen() failed");
        return 1;
    }

    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    int client = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (client == -1)
    {
        perror("accept() failed");
        return 1;
    }
     char buf[2048];
     
    int ret = recv(client, buf, sizeof(buf), 0);
     buf[ret]=0;
    char name[64];
    char drive;
    short int size;
    int pos=0;
    strcpy(name,buf);
    printf("Ten thiet bi: %s\n",name);
    pos=strlen(name)+1;
     int numberDrive=(ret-pos)/3;
    printf("So luong o: %d\n",numberDrive);
    for (int i = 1; i <=numberDrive; i++)
    {
      drive= buf[pos];
      pos++;
      memcpy(&size,buf+pos,sizeof(short int));
      printf("%c: %hdGB\n",drive,size);
      pos+=sizeof(short int);
    }
    
    
    

    
       
    
   
   // close(listen);
    
}
