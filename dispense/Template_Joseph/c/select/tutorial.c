// La funzione select in C è una chiamata di sistema utilizzata per monitorare più file descriptor (descrittori di file), inclusi quelli associati a socket, per determinare se sono pronti per la lettura, la scrittura o se si sono verificati errori. Questa è una tecnica comune utilizzata in programmazione di socket per gestire più connessioni contemporaneamente senza dover attendere attivamente ogni socket. È particolarmente utile in scenari multi-client, come server che gestiscono più connessioni in parallelo.

// Ecco come funziona la funzione select sia nel caso di un singolo processo che nel caso di una fork per gestire le richieste e le risposte dei client:

// 1. Utilizzo di select in un singolo processo:

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    fd_set read_fds;
    int max_fd;
    
    // Creare il socket del server
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    // Inizializzare la struttura server_addr e bind
    // ...

    // Iniziare ad accettare connessioni
    listen(server_socket, 5);

    // Inizializzare il set dei descrittori di file
    FD_ZERO(&read_fds);
    FD_SET(server_socket, &read_fds);
    max_fd = server_socket;
	socklen_t client_len = sizeof(client_addr);

    while (1) {
        fd_set temp_fds = read_fds;

        // Usare select per monitorare i descrittori di file
        select(max_fd + 1, &temp_fds, NULL, NULL, NULL);

        // Verificare se il socket del server ha eventi pendenti (nuove connessioni)
        if (FD_ISSET(server_socket, &temp_fds)) {
            client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
            FD_SET(client_socket, &read_fds);
            if (client_socket > max_fd) {
                max_fd = client_socket;
            }
        }

        // Verificare gli altri socket per eventi di lettura
        for (int i = server_socket + 1; i <= max_fd; i++) {
            if (FD_ISSET(i, &temp_fds)) {
                // Leggere dati dal socket i
                // ...

                // Gestire la richiesta del client
                // ...

                // Chiudere il socket se necessario
                // ...
            }
        }
    }

    close(server_socket);
    return 0;
}

// 2. Utilizzo di select con una fork per gestire i client in processi figli:

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void handle_client(int client_socket) {
    // Gestire la connessione del client qui
    // ...

    close(client_socket);
}

int main2() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    fd_set read_fds;
    int max_fd;

    // Creare il socket del server
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    // Inizializzare la struttura server_addr e bind
    // ...

    // Iniziare ad accettare connessioni
    listen(server_socket, 5);

    // Inizializzare il set dei descrittori di file
    FD_ZERO(&read_fds);
    FD_SET(server_socket, &read_fds);
    max_fd = server_socket;

    while (1) {
        fd_set temp_fds = read_fds;

        // Usare select per monitorare i descrittori di file
        select(max_fd + 1, &temp_fds, NULL, NULL, NULL);

        // Verificare se il socket del server ha eventi pendenti (nuove connessioni)
        if (FD_ISSET(server_socket, &temp_fds)) {
            client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
            FD_SET(client_socket, &read_fds);
            if (client_socket > max_fd) {
                max_fd = client_socket;
            }
            // Fork per gestire il client in un processo figlio
            if (fork() == 0) {
                close(server_socket); // Il processo figlio non ha bisogno del socket del server
                handle_client(client_socket);
                exit(0);
            }
            close(client_socket); // Il processo padre chiude il socket client
        }
    }

    close(server_socket);
    return 0;
}

// In entrambi gli esempi, select viene utilizzato per gestire più connessioni in modo efficiente, consentendo al server di rispondere a molteplici client contemporaneamente. Nel secondo esempio, la fork viene utilizzata per creare un processo figlio separato per gestire ciascun client, il che consente di gestire le richieste e le risposte dei client in modo parallelo.