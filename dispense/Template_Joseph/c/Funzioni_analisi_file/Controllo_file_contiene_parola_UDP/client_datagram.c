/* dgram_client.c
 * chiede nome file e la parola, invia al server,
 * attende l’esito dell’operazione
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

int main(int argc, char **argv) {
    struct hostent    *host;
    struct sockaddr_in clientaddr, servaddr;
    int                port, nread, sd, ris, len = 0;
    char               nomeFile[LINE_LENGTH], parola[LINE_LENGTH];
    int                okTerminator;
    char               terminator = '\0';
    req_t          req;

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

    /* INIZIALIZZAZIONE INDIRIZZO CLIENT -------------------------- */
    memset((char *)&clientaddr, 0, sizeof(struct sockaddr_in));
    clientaddr.sin_family      = AF_INET;
    clientaddr.sin_addr.s_addr = INADDR_ANY;
    clientaddr.sin_port        = 0;

    /* INIZIALIZZAZIONE INDIRIZZO SERVER -------------------------- */
    memset((char *)&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    host                = gethostbyname(argv[1]);
    if (host == NULL) {
        printf("Error while resolving %s to IP address\n", argv[1]);
        exit(2);
    } else {
        servaddr.sin_addr.s_addr = ((struct in_addr *)(host->h_addr))->s_addr;
        servaddr.sin_port        = htons(port);
    }

    /* CREAZIONE SOCKET ---------------------------------- */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("Open socket: ");
        exit(3);
    }
    printf("Client: created socket sd=%d\n", sd);

    /* BIND SOCKET  -------------------------------------- */
    if (bind(sd, (struct sockaddr *)&clientaddr, sizeof(clientaddr)) < 0) {
        perror("bind socket ");
        exit(1);
    }
    printf("Client: socket bound to port %i\n", clientaddr.sin_port);

    /* CORPO DEL CLIENT:
     ciclo di accettazione di richieste da utente ------- */

    printf("Dammi il nome di file, EOF per terminare: ");
    while (gets(nomeFile)) {
        //controllo se la stringa contiene il terminatore, per sapere se la stringa scritta dall'utente eccede il numero di caratteri massimo che può contenere
        okTerminator = 0;
        for(int i=0; i<FILE_NAME_LENGTH; i++){
            if(nomeFile[i] == terminator){
                okTerminator = 1;
                break;
            }
        }

        if(!okTerminator){
            printf("La stringa inserita eccede il massimo dei caratteri consentiti\n\n");
            printf("Inserire nome del file, EOF per terminare: ");
            continue;
        }

        strcpy(req.fileName, nomeFile);

        printf("Parola da cercare: ");
        if (!gets(parola)) {
            printf("Input error!\n");
            break;
        }
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

        strcpy(req.parola, parola);

        printf("Invio richiesta di eliminazione della parola %s, nel file %s.\n", req.parola,
               req.fileName);

        /* invio richiesta */
        len = sizeof(servaddr);
        if (sendto(sd, &req, sizeof(req), 0, (struct sockaddr *)&servaddr, len) < 0) {
            perror("sendto");
            // se questo invio fallisce il client torna all'inzio del ciclo
            printf("Dammi il nome di file, EOF per terminare: ");
            continue;
        }

        /* ricezione del risultato */
        if (recvfrom(sd, &ris, sizeof(ris), 0, (struct sockaddr *)&servaddr, &len) < 0) {
            perror("Error in recvfrom: ");
            // se questo invio fallisce il client torna all'inzio del ciclo
            printf("Dammi il nome di file, EOF per terminare: ");
            continue;
        }

        switch (ris) {
            case 1:
                    printf("Il file %s contiene la parola %s\n", nomeFile, parola);
                    break;
            case -1:
                    printf("Errore: il file non esiste sul server\n");
                    break;
            case -2:
                    printf("Errore: file %s non contiene nessuna occorrenza della parola %s\n", nomeFile, parola);
                    break;
            default: 
                    printf("Errore sconosciuto\n");
                    break;

        }

        printf("Dammi il nome di file, EOF per terminare: ");
    } // while
} // main