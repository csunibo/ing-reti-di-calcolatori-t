#include <pthread.h>
#include <stdio.h>

int x = 0;

int MAX = 100;

void increase(int *arg) {
  while (x < MAX) {
    printf("%d: x = %d\n", *arg, x);
    x++;
  }
}

int main() {
  // generate two threads and let them increase x
  pthread_t t1, t2;

  int i1 = 1;
  int i2 = 2;

  int nthreads = 10;

  pthread_t *threads;
  threads = (pthread_t *)malloc(nthreads * sizeof(pthread_t));
  int *ids;
  ids = (int *)malloc(nthreads * sizeof(int));

  for (int i = 0; i < nthreads; i++) {
    ids[i] = i;
    pthread_create(&(threads[i]), NULL, increase, &(ids[i]));
  }

  for (int i = 0; i < nthreads; i++) {
    pthread_join(threads[i], NULL);
  }

  // print the result
  printf("x = %d\n", x);
}
