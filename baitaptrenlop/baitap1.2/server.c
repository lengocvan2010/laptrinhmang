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
    int clientAddrLen = sizeof(addr);
    int client = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (client == -1)
    {
        perror("accept() failed");
        return 1;
    }
     char buf[101];

     
    int ret;
     
     
    char *index;
    char end[110]="";
    char search[]="0123456789";
    int count=0;
    
  while(1){
    ret = recv(client, buf, sizeof(buf), 0);
     if (ret <= 0)
            break;
        strcat(end,buf);
        while (1)
        {
           index= strstr(end,search);
           
           if(index != NULL){
            *index='m';
            count++;
           }
            else break;
        }
        memset(end,0,sizeof(end));
         strncpy(end,buf+ret-8,8);
        memset(buf,0,sizeof(buf));
}
       
       printf("So chuoi tim duoc la: %d\n",count); 
  
   close(listener);
   close(client);
    
}
