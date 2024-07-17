#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//esplorazione fino a a direttori di secondo livello

#define LINE_LENGTH 256



int main(){

    DIR           *dir1, *dir2, *dir3;
    struct dirent *dd1, *dd2;
    char           dir[LINE_LENGTH], newDir[LINE_LENGTH];

    strcpy(dir, "/");

    if ((dir1 = opendir(dir)) != NULL) { // direttorio presente
                        while ((dd1 = readdir(dir1)) != NULL) {
                            if (strcmp(dd1->d_name, ".") != 0 && strcmp(dd1->d_name, "..") != 0) {
                                // build new path
                                sprintf(newDir, "%s/%s", dir, dd1->d_name);
                                if ((dir2 = opendir(newDir)) != NULL) { // dir sec livello
                                    while ((dd2 = readdir(dir2)) != NULL) {
                                        if (strcmp(dd2->d_name, ".") != 0 &&
                                            strcmp(dd2->d_name, "..") != 0)
                                        {
                                            // build new path
                                            strcat(newDir, "/");
                                            strcat(newDir, dd2->d_name);
                                            // se non è un direttorio, è un file da considerare
                                            if ((dir3 = opendir(newDir)) == NULL)
                                            {
                                                //cose da fare con file nel direttorio di secondo livello

                                                //nome file: dd2->d_name

                                            } // if file 2 livello
                                        }     // if not . and .. 2 livello
                                    }         // while in 2* livello
                                    printf("Fine invio\n");
                                } // if dir 2 livello
                            }     // if opendir 2 livello
                        }         // while frst livello

                    } // if open dir 1 livello
    else{ // err apertura dir
        printf("Impossibile aprire il direttorio\n");
    }

}