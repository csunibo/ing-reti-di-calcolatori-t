/*
 * xfactor_s.c
 */

#include "xfactor.h"
#include <rpc/rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 12 // Numero candidati

/*STATO INTERNO PRIVATO DEL SERVER*/
typedef struct {
    char candidato[MAX_NAME_SIZE];
    char giudice[MAX_NAME_SIZE];
    char categoria[2];
    char nomeFile[FILENAME_MAX];
    char fase[10];
    int  voto;
} Riga;

// variabili globali private (static)
static Riga t[N];
static int  inizializzato = 0;

/* Inizializza lo stato del server */
void inizializza() {
    int i;
    if (inizializzato == 1) {
        return;
    }

    // Tutti inizialmente liberi
    for (i = 0; i < N; i++) {
        strcpy(t[i].candidato, "L");
        strcpy(t[i].giudice, "L");
        strcpy(t[i].categoria, "L");
        strcpy(t[i].nomeFile, "L");
        strcpy(t[i].fase, "L");
        t[i].voto = -1;
    }

    // Inizializziamo qualche valore per i test
    strcpy(t[0].candidato, "Brasco");
    strcpy(t[0].giudice, "Bowie");
    strcpy(t[0].categoria, "U");
    strcpy(t[0].nomeFile, "brasco.txt");
    strcpy(t[0].fase, "A");
    t[0].voto = 75;

    strcpy(t[1].candidato, "Viga");
    strcpy(t[1].giudice, "Winehouse");
    strcpy(t[1].categoria, "D");
    strcpy(t[1].nomeFile, "viga.txt");
    strcpy(t[1].fase, "S");
    t[1].voto = 98;

    strcpy(t[2].candidato, "Paperino");
    strcpy(t[2].giudice, "Bowie");
    strcpy(t[2].categoria, "O");
    strcpy(t[2].nomeFile, "paperino.txt");
    strcpy(t[2].fase, "A");
    t[2].voto = 58;

    strcpy(t[2].candidato, "Mikes");
    strcpy(t[2].giudice, "Steve");
    strcpy(t[2].categoria, "B");
    strcpy(t[2].nomeFile, "mikes.txt");
    strcpy(t[2].fase, "B");
    t[2].voto = 45;

    inizializzato = 1;
    printf("Terminata inizializzazione struttura dati!\n");
}


// implementazione delle procedure definite nel file XDR
Output *classifica_giudici_1_svc(void *voidValue, struct svc_req *reqstp) {
    static Output res;
    int           i, k, presente, ind = 0, max, count = 0;
    Giudice       swap;

    if (inizializzato == 0) {
        inizializza();
    }

    // Inizializzo il risultato con valori di default
    for (i = 0; i < NUM_GIUDICI; i++) {
        strcpy(res.classificaGiudici[i].nomeGiudice, "L");
        res.classificaGiudici[i].punteggioTot = -1;
    }

    // Inserisco i giudici, calcolandone il punteggio
    for (i = 0; i < N; i++) {
        presente = 0;

        // Se giudice è già presente nell'array di risultato
        for (k = 0; k < NUM_GIUDICI; k++) {
            if (strcmp(res.classificaGiudici[k].nomeGiudice, t[i].giudice) == 0) {
                res.classificaGiudici[k].punteggioTot += t[i].voto;
                presente = 1;
            }
        }

        // Se no, inseriamolo
        if (presente == 0) {
            strcpy(res.classificaGiudici[ind].nomeGiudice, t[i].giudice);
            res.classificaGiudici[ind].punteggioTot = t[i].voto;
            ind++;
        }
    }

    // Ordino res in base al punteggio (Bubble sort)
    for (i = 0; i < NUM_GIUDICI - 1; i++) {
        for (k = 0; k < NUM_GIUDICI - i - 1; k++) {
            if (res.classificaGiudici[k].punteggioTot <
                res.classificaGiudici[k + 1]
                    .punteggioTot) /* For decreasing order use '<' instead of '>' */
            {
                swap                         = res.classificaGiudici[k];
                res.classificaGiudici[k]     = res.classificaGiudici[k + 1];
                res.classificaGiudici[k + 1] = swap;
            }
        }
    }

    return &res;
}

int *esprimi_voto_1_svc(Input *input, struct svc_req *reqstp) {
    static int found = -1;

    if (inizializzato == 0) {
        inizializza();
    }

    printf("Richiesta di esprimere un voto per %s\n", input->nomeCandidato);

    for (int i = 0; i < N; i++) {
        if (strcmp(t[i].candidato, input->nomeCandidato) == 0) {
            if (input->tipoOp == 'A') {
                t[i].voto++;
            } else if (input->tipoOp == 'S' && t[i].voto > 0) { // Voto min è 0
                t[i].voto--;
            } else {
                printf("Invalid operation!\n");
                break;
            }
            printf("Risultato: \n\t Cantante = %s \n\t Voti = %d\n", input->nomeCandidato,
                   t[i].voto);
            found = 0;
            break;
        }
    }
    if (found < 0) {
        printf("Problemi nell'attribuzione del voto, nome non trovato\n");
    }

    return (&found);
}
