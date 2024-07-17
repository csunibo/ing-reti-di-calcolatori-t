#include <stdio.h>
#include <stdlib.h>

void addone(int* num){
	*num = *num + 1;
}

int main() {
	int x = 10;
	printf("%i\n",x);
	addone(&x);
	printf("%i\n",x);

	int* cane;
	cane = malloc(sizeof(int));
	*cane = 10;
	printf("%i\n",*cane);
	addone(cane);
	printf("%i\n",*cane);
	free(cane);
	
	return 0;
}
