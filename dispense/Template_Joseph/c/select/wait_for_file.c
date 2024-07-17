// write a program that waits for a file to update using select
// and then reads the file and prints the contents to stdout

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd == -1)
	{
		perror("open");
		exit(EXIT_FAILURE);
	}

	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);

	int nfds = fd + 1;
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	for (;;)
	{
		int retval = select(nfds, &rfds, NULL, NULL, &tv);
		if (retval == -1)
		{
			perror("select");
			exit(EXIT_FAILURE);
		}
		else if (retval)
		{
			char buf[BUFSIZ];
			ssize_t bytes_read = read(fd, buf, BUFSIZ);
			if (bytes_read == -1)
			{
				perror("read");
				exit(EXIT_FAILURE);
			}
			write(STDOUT_FILENO, buf, bytes_read);
		}
		else
		{
			fprintf(stderr, "No data within five seconds.\n");
		}
	}

	close(fd);
	return EXIT_SUCCESS;
}