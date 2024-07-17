#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLINE 256

int main(int argc,char ** argv){
    struct sockaddr_in servaddr;
    struct hostent * host;
    int nread,port,sd;
    char nomeDir[MAXLINE];
    char buff[MAXLINE];

    if(argc!=3){
            printf("Error,%s IpAddress NumberPort",argv[0]);
            exit(-1);
    }

    memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    host                = gethostbyname(argv[1]);
    if (host == NULL) {
        printf("%s not found in /etc/hosts\n", argv[1]);
        exit(2);
    }

    nread = 0;
    while (argv[2][nread] != '\0') {
        if ((argv[2][nread] < '0') || (argv[2][nread] > '9')) {
            printf("Secondo argomento non intero\n");
            exit(2);
        }
        nread++;
    }
    port = atoi(argv[2]);
    if (port < 1024 || port > 65535) {
        printf("Porta scorretta...");
        exit(2);
    }

    servaddr.sin_addr.s_addr = ((struct in_addr *)(host->h_addr))->s_addr;
    servaddr.sin_port        = htons(port);

    printf("Nome directory da cercare: ");
    while(gets(nomeDir)){
        sd=socket(AF_INET,SOCK_STREAM,0);
        if(sd<0){
                perror("Errore socket");
                exit(3);
        }

        printf("Creata la socket sd=%d\n", sd);

        if (connect(sd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) < 0) {
            perror("Errore in connect");
            exit(4);
        }
        printf("Connect ok\n");

        if (write(sd, nomeDir, (strlen(nomeDir) + 1)) < 0) {
            perror("write");
            close(sd);
            printf("Nome del file da richiedere: ");
            continue;
        }
        printf("Richiesta del direcotry %s inviata... \n", nomeDir);
        shutdown(sd, 1);

        printf("Lista dei file:\n");

        while(recv(sd,buff,sizeof(buff),0)>0){
            printf("%s",buff);
        }
        shutdown(sd, 0);
        close(sd);
        printf("Nome directory da cercare: ");

    }
}
