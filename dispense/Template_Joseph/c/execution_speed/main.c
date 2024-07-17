#include <math.h>
#include <stdlib.h>

int function(int num1, int num2) {
  if (num1 % 2 == 0) {
    return sqrt(num2);
  } else {
    return (int)pow(num1, 2);
  }
}
int main(int argc, char *argv[]) {
  int a = atoi(argv[1]);
  for (int i = 0; i < 1000000000; i++) {
    int res = function(i, a);
  }
}
