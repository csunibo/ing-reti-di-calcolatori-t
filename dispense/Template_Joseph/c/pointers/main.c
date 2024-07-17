#include <stdio.h>
#include <sys/types.h>

int main(){
	// indirizzo a 64 bit
	u_long x = 0;
	if ((void*)x == NULL)
		puts("ok");
	return 0;
}
