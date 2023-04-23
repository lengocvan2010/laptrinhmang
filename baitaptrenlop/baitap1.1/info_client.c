#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main() {
    char name[50];
    short int soodia;
    

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
  
   
    printf("Nhap ten thiet bi: ");
   fgets(name,sizeof(name),stdin);
   int pos = strlen(name);
   name[strlen(name)-1]=0;
    printf("Nhap so o dia: ");
    scanf("%hd",&soodia);
    getchar();
    char buf[2048];
    char drive;
    short int size;
    
    sprintf(buf, "%s", name);

    for(int i=1; i<=soodia; i++){
        
        printf("Nhap o dia %d:",i);
        
        scanf("%c %hd",&drive,&size);
        getchar();
       
        buf[pos]=drive;
        pos++;
        memcpy(buf+pos,&size,sizeof(size));
        pos+=sizeof(size);
      
      
        
        
    }
   


  
     
      
     send(client, buf, pos,0);
    
   
    
   close(client);
    return 0;
}
