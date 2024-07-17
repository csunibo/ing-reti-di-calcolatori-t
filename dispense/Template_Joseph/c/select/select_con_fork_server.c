#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>
#include <stdbool.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

void handle_client(int listenfd, char *ip, int port)
{
	char buffer[10];

	while (true)
	{
		read(listenfd, buffer, 10);
		printf("(%s:%i) %s\n", ip, port, buffer);
		write(listenfd, buffer, 10);
	}
	close(listenfd);
}

int main(int argc, char *argv[])
{
	int listenfd, connfd, fd_file, nready, maxfdp1, filefd, fileSize = -1;
	const int on = 1;
	char message[10];
	char response[10];
	int len, nread, nwrite, num, ris, port;

	fd_set rset;
	struct sockaddr_in cliaddr, servaddr;
	int client_socket, server_socket;

	// CONTROLLO ARGOMENTI
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

	// Loop principale
	while (1)
	{

		FD_SET(listenfd, &rset);

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

		if (FD_ISSET(listenfd, &rset))
		{
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

			// Gestione tramite processo figlio

			printf("Connessione accetata da %s dalla porta %s\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

			if (fork() == 0)
			{
				handle_client(connfd, inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
			}
			else
			{
				// Processo padre
				// close(client_socket); // Il processo padre non ha bisogno del socket client
			}
		}
	}
	return 0;
}