#include <stdio.h>
#include <string.h>

int main(){
    char c, str[64];
    int iter = 0;

    while((c = getchar()) != 0){
        getchar(); //consumo l'invio
        iter++;
        printf("[%d]Carattere letto: %c\n", iter, (char) c);
        memset(&c, 0, 1);
    }
}