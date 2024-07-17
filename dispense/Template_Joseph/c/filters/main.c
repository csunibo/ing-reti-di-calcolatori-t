#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

// #include <signal.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <sys/stat.h>
// #include <sys/types.h>
// #include <sys/un.h>
// #include <sys/wait.h>
// #include <unistd.h>

#include <stdio.h>

int main() {
  char *buf;
  int len = 1;

  int fd = open("test.txt", O_CREAT | O_WRONLY, 0666);

  buf = (char *)malloc(len * sizeof(char));

  buf[0] = 'a';
  write(fd, buf, len);

  buf[0] = 'b';
  write(fd, buf, len);

  buf[0] = (char)EOF;
  write(fd, buf, len);

  buf[0] = 'c';
  write(fd, buf, len);

  fd = open("test.txt", O_RDONLY);

  while (read(fd, buf, len) > 0) {
    write(1, buf, len);
  }

  // per stdin

  // char c;
  // while ((c = getchar()) != EOF) {
  //   write(1, &c, 1);
  // }
}