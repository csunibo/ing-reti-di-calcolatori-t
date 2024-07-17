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
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define DIM_BUFF 100
#define LENGTH_FILE_NAME 256
#define max(a, b) ((a) > (b) ? (a) : (b))
char *list[0];



//funzione per verifica se è un file o una directory

int isFile(const char* name)
{
    DIR* directory = opendir(name);

    if(directory != NULL)
    {
     closedir(directory);
     return 0;
    }

    if(errno == ENOTDIR)
    {
     return 1;
    }

    return -1;
}
/********************************************************/
void gestore(int signo)
{
    int stato;
    printf("esecuzione gestore di SIGCHLD\n");
    wait(&stato);
}
/********************************************************/

int main(int argc, char **argv)
{
    int listenfd, connfd, udpfd, fd_file, nready, maxfdp1;
    const int on = 1;
    int err = -1;
    char buff[DIM_BUFF], nome_file[LENGTH_FILE_NAME], nome_dir[LENGTH_FILE_NAME];
    fd_set rset;
    int len, nread, nwrite, num, ris, port;
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

        /* RICHIESTE NOMI FILE SOTTODIRETTORI REMOTI DI SECONDO LIVELLO ------------------------------------- */
        if (FD_ISSET(listenfd, &rset))
        {
            printf("Ricevuta richiesta di sottodirettori di un file\n");
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
                if (read(connfd, &nome_dir, sizeof(nome_dir)) <= 0)
                {
                    perror("read");
                    break;
                }

                printf("Richiesto file %s\n", nome_dir);
                DIR *dir = opendir(nome_dir);                 //directory
                DIR *dir1;                                    //sub-directory
                struct dirent *de;
                struct dirent *de1;
                char **fileLists;                             

//--------------apro la directory
                if(dir == NULL){
                    printf("Impossibile aprire la directory %s", nome_dir);
                    write(connfd, err, sizeof(err));
                }else{

               while((de = readdir(dir)) != NULL){
                //verifico che de sia una sub-directory e in caso positivo conto i file
                if((isFile(de->d_name)) == 0){
    
    dir1 = opendir(de -> d_name);
    if (dir1 == NULL)
        exit(1);
    while ((de1 = readdir(dir1)) != NULL)
    {
        printf("Trovato il file %s\n", de1->d_name);
        char * file;
        strcpy(file, de1->d_name);
         write(connfd, &file, sizeof(file));
        
    }
   
    closedir(dir);
                    lista_file(de->d_name);
                }
               }

                  
                    printf("Terminato invio lista file\n");
                    /* non e' piu' necessario inviare al client un segnale di terminazione */
                    close(nome_dir);
                }
            }
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
         /* fine gestione richieste di file */

        /* ELIMINARE LE OCCORRENZE DI UNA PAROLA ------------------------------------------ */
        char line[255];
        char *file, *parola;
        int fd1, fd2, i = 0, count = 0, n = -1, menoUno;
        menoUno = htons(n);
        char c;
        
        if (FD_ISSET(udpfd, &rset))
        {
            printf("Ricevuta richiesta di eliminare le occorrenze di una parola\n");

            len = sizeof(struct sockaddr_in);
            if (recvfrom(udpfd, line, sizeof(line), 0, (struct sockaddr *)&cliaddr, &len) <
                0)
            {
                perror("recvfrom");
                continue;
            }
            file = strtok(line, ' ');
            parola = strtok(0, ' ');



            printf("Richiesta eliminazione delle occorrenze di %s nel file %s\n", parola, file);
            
            if((fd1 = open(file, O_RDONLY)) < 0){
                perror("File fd1");
                sendto(udpfd, menoUno, sizeof(menoUno), 0, (struct sockaddr *)&cliaddr, len);
                continue;
            }

            if((fd2 = open("temp", O_CREAT, 777) < 0)){       //file di copia
                perror("File fd2");
                continue;
            }

              while ((nread = read(fd1, &c, 1)) > 0){ // leggo a caratteri per individuare lo spazio di fine parola
                if (c != ' ') {
                    buff[i++] = c;
                } else{
                    buff[i] = '\0';
                    i = 0;
                    if(strcmp(buff, parola) == 0){
                    count++;
                    }else {
                    write(fd2, &buff, sizeof(buff));
                    }
                    memset(buff, 0, 255);
                    }
                }
                   
        }
            
            
            printf("Risultato del conteggio: %i\n", count);
           

            /*
             * Cosa accade se non commentiamo le righe di codice qui sotto?
             * Cambia, dal punto di vista del tempo di attesa del client,
             * l'ordine col quale serviamo le due possibili richieste?
             * Cosa cambia se utilizziamo questa realizzazione, piuttosto
             * che la prima?
             *
             */
            /*
            printf("Inizio sleep\n");
            sleep(30);
            printf("Fine sleep\n");*/

            if (sendto(udpfd, count, sizeof(count), 0, (struct sockaddr *)&cliaddr, len) < 0)
            {
                perror("sendto");
                continue;
            }
            remove(nome_file);
            rename("temp", nome_file);

        } /* fine gestione richieste di conteggio */
    exit(0);

    } /* ciclo for della select */
    /* NEVER ARRIVES HERE */

