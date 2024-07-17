#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int main(int argc, char** argv)
{
	struct hostent* hp;
	struct sockaddr_in peeraddr;

	peeraddr.sin_family = AF_INET;
	peeraddr.sin_port = htons(80);

	hp = gethostbyname(argv[1]);

	if (hp) {
		peeraddr.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr))->s_addr;
	}

	// print all peeraddr.sin_addr.addr_list
	for (int i = 0; hp->h_addr_list[i] != NULL; i++) {
		printf("%s\n", inet_ntoa(*(struct in_addr*)hp->h_addr_list[i]));

		printf("%i\n", (unsigned int)peeraddr.sin_addr.s_addr);
	}
}
