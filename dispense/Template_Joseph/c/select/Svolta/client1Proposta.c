
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
    struct sockaddr_in servaddr,clientaddr;
    struct hostent * host;
    int port,sd,len,nread;
    int esito;
    short esito2;
    char nomeFile[MAXLINE];
    char parola[MAXLINE];
    char temp[MAXLINE];
    char msg[MAXLINE];

    if(argc!=3){
        perror("Number of parameters incorrect");
        exit(-1);
    }
    /* INIZIALIZZAZIONE INDIRIZZO SERVER--------------------- */
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

     /* INIZIALIZZAZIONE INDIRIZZO CLIENT--------------------- */
    memset((char *)&clientaddr, 0, sizeof(struct sockaddr_in));
    clientaddr.sin_family      = AF_INET;
    clientaddr.sin_addr.s_addr = INADDR_ANY;
    clientaddr.sin_port        = 0;

    printf("Client avviato\n");

    /* CREAZIONE SOCKET ---------------------------- */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("apertura socket");
        exit(3);
    }
    printf("Creata la socket sd=%d\n", sd);

    /* BIND SOCKET, a una porta scelta dal sistema --------------- */
    if (bind(sd, (struct sockaddr *)&clientaddr, sizeof(clientaddr)) < 0) {
        perror("bind socket ");
        exit(1);
    }
    printf("Client: bind socket ok, alla porta %i\n", clientaddr.sin_port);

    printf("Nome del file:\n");

    while(gets(nomeFile)){
        printf("Nome della parola:\n");
        gets(parola);
        strcpy(temp,nomeFile);
        strcat(temp," ");
        strcpy(msg,temp);
        strcat(msg,parola);
        printf("Messaggio inviato %s\n",msg);
        len=sizeof(servaddr);
        if(sendto(sd,msg,strlen(msg)+1,0,(struct sockaddr *) &servaddr,len)<0){
                perror("Scrittura socket");
                printf("Nome file nome parola\n");
                continue;
        }
        /* ricezione del risultato */
        printf("Attesa del risultato...\n");
        if(recvfrom(sd,&esito,sizeof(esito),0,(struct sockaddr *) &servaddr,&len)<0){
                perror("Lettura socket");
                continue;
        }

        esito2=ntohs(esito);

        if(esito2<0)
            printf("nomeFile inserito incorretto!\n");
        else
            printf("%s la parola %s e' stata eliminata %u volte\n",nomeFile,parola,esito2);

        printf("Nome del file:\n");
    }

    printf("\nClient: termino...\n");
    close(sd);
    exit(0);
}
