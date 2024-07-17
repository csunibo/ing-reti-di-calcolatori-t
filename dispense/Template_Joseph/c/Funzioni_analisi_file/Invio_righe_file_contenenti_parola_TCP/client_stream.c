/* stream_client.c
 * Cliente invia nome file e parola
 * e riceve linee contenenti parola
 */

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define LINE_LENGTH 256
#define WORD_LENGTH 64
#define FILE_NAME_LENGTH 64

/********************************************************/
typedef struct {
    char fileName[FILE_NAME_LENGTH];
    char parola[WORD_LENGTH];
} req_t;

/********************************************************/


int main(int argc, char *argv[]) {
    int                port, nread, sd, nsend, ris;
    int                okTerminator;
    char               terminator = '\0', zero = 0;
    char               fileName[LINE_LENGTH], buffChar, parola[WORD_LENGTH];
    req_t              req;
    struct hostent    *host;
    struct sockaddr_in servaddr;

    /* CONTROLLO ARGOMENTI ---------------------------------- */
    if (argc != 3) {
        printf("Usage: %s serverAddress serverPort\n", argv[0]);
        exit(1);
    }
    nread = 0;
    while (argv[2][nread] != '\0') {
        if ((argv[2][nread] < '0') || (argv[2][nread] > '9')) {
            printf("Second argument must be an integer!\n");
            printf("Usage: %s serverAddress serverPort\n", argv[0]);
            exit(2);
        }
        nread++;
    }
    port = atoi(argv[2]);
    if (port < 1024 || port > 65535) {
        printf("Port must be in the range [1024, 65535]\n");
        exit(2);
    }

    /* INIZIALIZZAZIONE INDIRIZZO SERVER -------------------------- */
    memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    host                = gethostbyname(argv[1]);
    if (host == NULL) {
        printf("Error while resolving %s to IP address\n", argv[1]);
        exit(1);
    }
    servaddr.sin_addr.s_addr = ((struct in_addr *)(host->h_addr))->s_addr;
    servaddr.sin_port        = htons(port);

    /* CREAZIONE SOCKET ------------------------------------ */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("Open socket: ");
        exit(3);
    }
    printf("Client: created socket sd=%d\n", sd);

    /* Operazione di BIND implicita nella connect */
    if (connect(sd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in)) < 0) {
        perror("connect");
        exit(3);
    }
    printf("Client: connect ok\n");

    /* CORPO DEL CLIENT:
     ciclo di accettazione di richieste da utente ------- */
    printf("Inserire nome del file, EOF per terminare: ");
    while (gets(fileName)) {
        //controllo se la stringa contiene il terminatore, per sapere se la stringa scritta dall'utente eccede il numero di caratteri massimo che può contenere
        okTerminator = 0;
        for(int i=0; i<FILE_NAME_LENGTH; i++){
            if(fileName[i] == terminator){
                okTerminator = 1;
                break;
            }
        }

        if(!okTerminator){
            printf("La stringa inserita eccede il massimo dei caratteri consentiti\n\n");
            printf("Inserire nome del file, EOF per terminare: ");
            continue;
        }

        printf("Inserisci parola: ");
        gets(parola);
        okTerminator = 0;
        for(int i=0; i<WORD_LENGTH; i++){
            if(parola[i] == terminator){
                okTerminator = 1;
                break;
            }
        }

        if(!okTerminator){
            printf("La stringa inserita eccede il massimo dei caratteri consentiti\n\n");
            printf("Inserire nome del file, EOF per terminare: ");
            continue;
        }

        strcpy(req.fileName, fileName);
        strcpy(req.parola, parola);

        printf("Invio richiesta al server: %s e %s\n", req.fileName, req.parola);

        if(send(sd, &req, sizeof(req), 0) != sizeof(req)){
            perror("Write request");
            exit(1);
        }

        // Lettura risposta dal server
        if(recv(sd, &ris, sizeof(ris), 0) < 0){
            perror("Recv ris");
            exit(1);
        }

        if(ris < 0){
            printf("File remoto non esistente\n");
        }
        else{
            printf("Linee di %s che contengono %s:\n", fileName, parola);
            //leggo a caratteri fino a zero
            while (read(sd, &buffChar, sizeof(char)) > 0 && buffChar != 0) {
                write(1, &buffChar, 1);
            }
        }

        printf("\nInserire nome del file, EOF per terminare: ");
    }
    /* Chiusura socket in ricezione */
    close(sd);
    printf("\nClient: termino...\n");
    exit(0);
}