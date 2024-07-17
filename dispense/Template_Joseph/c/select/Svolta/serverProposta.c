/* Server Select
 * 	Nuovo figlio per ogni richiesta file.
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define DIM_BUFF 256
#define LENGTH_FILE_NAME 256
#define max(a, b) ((a) > (b) ? (a) : (b))


void gestore(int signo)
{
    int stato;
    printf("esecuzione gestore di SIGCHLD\n");
    wait(&stato);
}
/********************************************************/

int main(int argc, char **argv)
{
    DIR *dir, *dir2;
    struct dirent *dd, *dd2;
    int listenfd, connfd, udpfd, fd_file, nready, maxfdp1;
    const int on = 1;
    char buff[DIM_BUFF], dirName[LENGTH_FILE_NAME], richiesta[LENGTH_FILE_NAME], c, fileName[LENGTH_FILE_NAME], dir2Level[LENGTH_FILE_NAME], parola[256];
    fd_set rset;
    int len, nread, nwrite, num, ris, port, fdDel, fdTemp, i = 0, fileNameSize = 0, parolaSize = 0, buffSize = 0, countWordDel = 0;
    struct sockaddr_in cliaddr, servaddr;

    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 2)
    {
        printf("Error: %s port\n", argv[0]);
        exit(1);
    }

    nread = 0;
    while (argv[1][nread] != '\0')
    {
        if ((argv[1][nread] < '0') || (argv[1][nread] > '9'))
        {
            printf("Terzo argomento non intero\n");
            exit(2);
        }
        nread++;
    }
    port = atoi(argv[1]);
    if (port < 1024 || port > 65535)
    {
        printf("Porta scorretta...");
        exit(2);
    }

    /* INIZIALIZZAZIONE INDIRIZZO SERVER E BIND ---------------------------- */
    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);
    printf("Server avviato\n");

    /* CREAZIONE SOCKET TCP ------------------------------------------------ */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        perror("apertura socket TCP ");
        exit(1);
    }
    printf("Creata la socket TCP d'ascolto, fd=%d\n", listenfd);

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        perror("set opzioni socket TCP");
        exit(2);
    }
    printf("Set opzioni socket TCP ok\n");

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind socket TCP");
        exit(3);
    }
    printf("Bind socket TCP ok\n");

    if (listen(listenfd, 5) < 0)
    {
        perror("listen");
        exit(4);
    }
    printf("Listen ok\n");

    /* CREAZIONE SOCKET UDP ------------------------------------------------ */
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpfd < 0)
    {
        perror("apertura socket UDP");
        exit(5);
    }
    printf("Creata la socket UDP, fd=%d\n", udpfd);

    if (setsockopt(udpfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        perror("set opzioni socket UDP");
        exit(6);
    }
    printf("Set opzioni socket UDP ok\n");

    if (bind(udpfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind socket UDP");
        exit(7);
    }
    printf("Bind socket UDP ok\n");

    /* AGGANCIO GESTORE PER EVITARE FIGLI ZOMBIE -------------------------------- */
    signal(SIGCHLD, gestore);

    /* PULIZIA E SETTAGGIO MASCHERA DEI FILE DESCRIPTOR ------------------------- */
    FD_ZERO(&rset);
    maxfdp1 = max(listenfd, udpfd) + 1;

    /* CICLO DI RICEZIONE EVENTI DALLA SELECT ----------------------------------- */
    for (;;)
    {
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);

        if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0)
        {
            if (errno == EINTR)
                continue;
            else
            {
                perror("select");
                exit(8);
            }
        }

        /* GESTIONE RICHIESTA DI FILE PRESENTI IN UN DIRETTORIO------------------------------------- */
        if (FD_ISSET(listenfd, &rset))
        {
            printf("Ricevuta richiesta di nomi di file presenti in un direttorio\n");
            len = sizeof(struct sockaddr_in);
            if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len)) < 0)
            {
                if (errno == EINTR)
                    continue;
                else
                {
                    perror("accept");
                    exit(9);
                }
            }

            if (fork() == 0)
            { /* processo figlio che serve la richiesta di operazione */
                close(listenfd);
                printf("Dentro il figlio, pid=%i\n", getpid());
                /* non c'e' piu' il ciclo perche' viene creato un nuovo figlio */
                /* per ogni richiesta di file */
                if (read(connfd, &dirName, sizeof(dirName)) <= 0)
                {
                    perror("read");
                    break;
                }

                printf("Richiesto direttorio %s\n", dirName);

                dir = opendir(dirName);
                if (dir == NULL){
                    printf("Direttorio inesistente\n");
                    num = htons(-1);
                    write(connfd, &num, sizeof(num));
                    continue;
                }


                while ((dd = readdir(dir)) != NULL){
                    if(strcmp(dd->d_name, ".") != 0 && strcmp(dd->d_name, "..") != 0){
                    strcpy(dir2Level, dirName);
                    strcat(dir2Level, "/");
                    strcat(dir2Level, dd->d_name);
                    dir2 = opendir(dir2Level);
                    if(dir2 != NULL){
                        //direttorio di secondo livello
                        printf("Trovato il direttorio %s, lo espoloro\n", dd->d_name);
                        strcpy(buff, "File del direttorio ");
                        strcat(buff, dir2Level);
                        strcat(buff, ":\n");
                        if((write(connfd, buff, sizeof(buff))) < 0){
                            perror("write dir2");
                            //break;
                        }
                        while((dd2 = readdir(dir2))!=NULL){
                            printf("%s\n", dd2->d_name);//debug
                            strcpy(buff, dd2->d_name);
                            strcat(buff, "\n");
                            if((write(connfd, buff, sizeof(buff))) < 0){
                                perror("write dir2");
                                //break;
                            }
                        }
                    }
                    else {//direttorio di primo livello
                        printf("Trovato il file %s\n", dd->d_name);
                        strcpy(buff, dd->d_name);
                        strcat(buff, "\n");
                        if((write(connfd, buff, sizeof(buff))) < 0){
                            perror("write dir1");
                            //break;
                        }
                    }

                
                }
                }//while direttorio primo livello
                closedir(dir);
                
                /*la connessione assegnata al figlio viene chiusa*/
                printf("Figlio %i: termino\n", getpid());
                shutdown(connfd, 0);
                shutdown(connfd, 1);
                close(connfd);
                exit(0);
            } // figlio-fork
            /* padre chiude la socket dell'operazione */
            /*shutdown(connfd,0);
            shutdown(connfd,1);
            close(connfd);*/
        } /* fine gestione richieste di file */

        /* GESTIONE ELIMINAZIONE OCCRRENZE DI UNA PAROLA------------------------------------------ */
        if (FD_ISSET(udpfd, &rset))
        {
            printf("Ricevuta richiesta eliminazione occorrenze di una parola\n");

            len = sizeof(struct sockaddr_in);
            if (recvfrom(udpfd, richiesta, LENGTH_FILE_NAME, 0, (struct sockaddr *)&cliaddr, &len) <
                0)
            {
                perror("recvfrom");
                continue;
            }
            c = '0';

            while(c != ' '){
                c = richiesta[i++];
                if(c != ' ')
                    fileName[fileNameSize++] = c;
            }
            fileName[fileNameSize++] = '\0';

            while(c != '\0'){
                c = richiesta[i++];
                parola[parolaSize++] = c;
            }
            parola[parolaSize] = '\0';

            printf("Richiesta eliminazione di %s del file %s\n", parola, fileName);

            if((fdDel = open(fileName, O_RDONLY)) < 0){
                printf("Il file non esiste\n");
                num = -1;
                ris = htons(num);
                if (sendto(udpfd, &ris, sizeof(ris), 0, (struct sockaddr *)&cliaddr, len) < 0)
                {
                perror("sendto");
                continue;
                }
            }


            if((fdTemp = open("temp.txt", O_CREAT|O_RDWR, 0777))<0){
                perror("create temp");
                break;
            }


            while((read(fdDel, &c, sizeof(c)))>0){
                //(c != ' ' && c != ',' && c != '\n' && c != '.' && c != ';')
                if ((c>='A' && c<= 'Z') || (c>='a' && c<= 'z') || (c>='1' && c<='9')){ 
                    buff[buffSize++] = c;
                }
                else{
                    buff[buffSize] = '\0';

                    if(strcmp(buff, parola) != 0){
                        if((write(fdTemp, buff, buffSize)) < 0){
                            perror("write temp (word)");
                            break;
                        }
                        //scrivo il separatore
                        if((write(fdTemp, &c, sizeof(c)))<0){
                        perror("write temp (sep)");
                        break;
                        }
                    }//se ho trovato la parola da eliminare scrivo il separatore solo se non è uno spazio
                    else {
                        countWordDel++;
                        if(c!=' '){
                            if((write(fdTemp, &c, sizeof(c)))<0){
                            perror("write temp (sep)");
                            break;
                            }
                        }
                    }

                    buffSize = 0;
                    memset(buff, 0, sizeof(buff));
                }
            }

            num = htons(countWordDel);
            if (sendto(udpfd, &num, sizeof(num), 0, (struct sockaddr *)&cliaddr, len) < 0)
            {
                perror("sendto");
                continue;
            }
            close(fdDel);
            close(fdTemp);
            remove(fileName);
            rename("temp.txt", fileName);
            memset(buff, 0, sizeof(buff));
            buffSize=0;
            parolaSize=0;
            memset(parola, 0, sizeof(parola));
            countWordDel = 0;
            memset(fileName, 0 , sizeof(fileName));
            fileNameSize = 0;
            i=0;
            memset(richiesta, 0, sizeof(richiesta));
        } /* fine gestione eliminazione parola da file */

    } /* ciclo for della select */
    /* NEVER ARRIVES HERE */
    exit(0);
}
