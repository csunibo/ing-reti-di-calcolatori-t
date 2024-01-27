# Motivazione e modello

RCP ci permette di fare **chiamate a procedure remote** disponibili su un **nodo remoto**:
- a **livello applicativo**
- in maniera **trasparente**
- utilizzando **linguaggi diversi**

Ci permettono quindi di **astrarre** dalle socket per gestire l'esecuzione di una **procedura remota**.

## Modello di riferimento
Il modello di riferimento delle RPC è quello del **Cliente/Servitore**

## Funzionamento

Importante ricordare che dal lato server è necessario un **demone** che 
- **rimanga in ascolto** di chiamate effettuate dai clienti 
- esegua il **dispatch** delle chiamate alla procedura adeguata

# Infrastruttura di supporto SUN

Il supporto fornisce:
- scambio di messaggi per consentire l'identificazione dei messaggi e delle procedure remote
- gestione dell'eterogeneità dei dati scambiati
	- marshalling/unmarshalling
	- serializzazione
- gestione di alcuni errori dovuti alla distribuzione
	- implementazione errata
	- errori utente
	- *roll-over*

## Semantica di interazione

### Guasto

A fronte del guasto, le semantiche disponibili sono:
- maybe
- at-least-once
- at-most-once
- exactly-once

Le semantiche offerte da SUN RPC sono **at-least-once** (default) e **at-most-once**. Questo è gestito dalle *internals* di RPC.


### Parallelismo e sincronizzazione

Un servitore può essere
- **sequenziale** (SUN RCP)
- parallelo

Un cliente può operare in maniera
- **sincrona** (SUN RCP)
- asincrona

## Implementazione Sun Microsystem

Utilizzeremo l'implementazione e il supporto RPC di Sun Microsistem: **Open Network Computing**.

Suite di prodotti che include:

| Nome | Funzione |
| --- | --- |
| eXternal Data Representation (**XDR**) | rappresentazione e conversione dati |
| Remote Procedure Call GENerator (**RPCGEN**) | Generazione del client e server stub|
| **Portmapper** | Risoluzione indirizzo servitore. Esegue la binding tra client e server in maniera dinamica |
| Network File System (**NFS**) | file system distribuito di Sun |

*Vedi slide per proiezione dello stack OSI su Sun RPC*
# Contratto RPC



1. Contratto tra le **operazioni che si possono chiedere e fornire**
	Sun sceglie di avere nomi basati sulla **tripla**
	`<#programma, #versione, #procedura>`

1. Specifica della procedura da invocare in base agli argomenti e al tipo di essi
	Sun sceglie di avere **un solo argomento in ingresso e un solo valore di ritorno**

Il contratto è formato da due parti

1. Definizioni dei programmi RPC, ovvero identificazione dei servizi offerti/da offrire e i rispettivi parametri tipati
2. **Definizioni XDR**, se e solo se i tipi dei dati specificati non sono tipi di default (già noti a RPC)

Il contratto è specificato in un file con estensione `.x`.

> [!warning] 
> Possiamo definire procedure che accettano solo **un valore in ingresso** e **un valore in uscita**.

> [!tip] Esempio di file `.x`
> ```RPC
> program STAMPAPROG {
> 	version STAMPAVERS {
> 		int PRINTMESSAGE(string) = 1;
> 	} = 1;
> } = 0x20000013;
> ```
> 
> Nota (vincoli di RPC di Sun):
> - ogni definizione ha **un** parametro di ingresso e **un** valore di ritorno
> - gli identificatori usano **solo lettere maiuscole**
> - ogni procedura è associata ad un **nome unico** all'interno del programma


# Implementazione del programma RPC

1. Il programma **cliente** implementa il `main` e la logica necessaria per reperimento e binding del/i servizio/i remoto/i
2. Il programma **servitore** NON implementa il `main` ma implementa **TUTTE** le procedure definite nel contratto.


### Esempio cliente

#### Caso locale
```c
#include <stdio.h>

int main(int argc, char *argv[]) { 
	char *message; 
	if (argc !=2) {
		fprintf(stderr,"uso:%s <messaggio>\n", argv[0]); exit(1);
	} 
	message = argv[1]; 
	if (!printmessage(message)) { 
	/* chiamata locale al servizio di stampa */
	fprintf(stderr,"%s: errore sulla stampa.\n", argv[0]); exit(1); 
	} 
	printf("Messaggio consegnato.\n"); exit(0); 
} 

// procedura locale per il servizio di stampa su video
int printmessage(char *msg) { 
	FILE *f; f = fopen("/dev/console","w"); 
	if (f == NULL) return 0; 
	fprintf(f,"%s\n",msg);
	fclose(f); 
	return 1;
}
```
#### Caso remoto
```c
#include <stdio.h>
#include <rpc/rpc.h> 
#include "msg.h" // prodotto da RPCGEN

int main(int argc, char *argv[]) {
	if (argc < 3) {
		fprintf(stderr,"uso: %s host msg\n", argv[0]);
		exit(1);
	} 
	
	char *server  = argv[1]; 
	char *message = argv[2]; 
	
	// creazione "gestore del trasporto"
	CLIENT *cl = clnt_create(server, STAMPAPROG, STAMPAVERS, "udp");
	if (cl==NULL) { 
		clnt_pcreateerror (server); exit(1);	
	} 
	
	// invocazione client stub
	int *result = printmessage_1(&message,cl); 
	
	// Controllo esito richiesta, dettagli più avanti
	if (result == NULL) {
		clnt_perror(cl, server); 
		exit(1);
	} 
	if (*result == 0) {
		fprintf(stderr,"%s: %s problema\n", argv[0], server); 
		exit(1);
	} 
	
	printf("Messaggio consegnato a %s\n",server); }
```

Si noti la creazione del **gestore del trasporto cliente** (`cl`)

## RPC e sistemi Unix

Di default Sun RPC utilizza **UDP**

## Limiti di Sun RPC

- Un programma implementa più **procedure remote**, anche con **versioni multiple**
- Solo **un argomento di ingresso** e **di uscita** per ogni procedura
- Mutua esclusione garantita dall'implementazione
	- Il server è sequenziale e una sola invocazione è eseguita per volta
	- Il client rimane sospeso in modo sincrono bloccante in attesa della risposta

Rischio di **deadlock** se il server richiede un servizio al programma chiamante.

# RPCGEN

A partire dal file `.x` RPCGEN crea gli stub sia per il client che per il server, ovvero quello che serve per unire il server / client ai livelli sottostanti.

# Come funziona RPC?

Ci sono tre attori che si devono coordinare

**Portmapper**: associa il servizio di ascolto (che si è registrato in precedenza) con una determinata porta.

Per la creazione del client si deve interrogare il portmapper per sapere qual è la porta del servizio.


# Portmapper

La prima cosa che fa il server quando si attiva è registrarsi sul portmapper.

Il portmapper può addirittura agire da proxy per il cliente. Per fare ciò ha bisogno del file delle definizioni.


# Livello basso API RPC

*Vedi slide*: [[10 - RPC Sun.pdf#page=45&selection=0,1,3,5|10 - Rpc Sun, pagina 45]]

### Client
`clnt_perror()` va a controllare nel gestore se c'è un errore e lo interpreta.


### Server

`pmap_unset()` distrugge *eventuali* precedenti registrazioni con lo stesso *numero di programma* e *versione*.

`svc_register()` registra la procedura **di dispatching**.

> [!warning] Attenzione
> Il binding è **a tempo di esecuzione**.

`svc_run()` non ritorna **mai** e rende il programma un **daemon** che rimane in attesa per le richieste dei client. All'interno ci sarà una `select`.

Le richieste dei clienti vengono servite in modo sequenziale.


# Modalità asincrona

Perché vogliamo fare cose asincrone? Perché così possiamo fare altro nel frattempo!

### Linee guida
- Di default lavoro in UDP, ma ho una semantica diversa dal semplice UDP. Se usassi questo modo avrei **poca qualità** per fare asincrono. -> **DEVI USARE TCP**, così che l'accordo sia piuttosto forte.
- **Non ci deve essere risultato (risposta)** -> `void`
- Per non aspettare **dobbiamo impostare il timeout a nullo nella client code**.

### Come si fa?
> [!tldr] `inproc, in`, puntatore alla procedura di trasformazione xdr e il puntatore all'area dell'input.

`tout` (timeout) deve essere impostato a `0`. nella `clnt_call` e anche nella `clnt_control`.


### Modalità batch

> [!tldr] Ricordiamo che la funzione `0` è sempre la funzione `NULLPROC`, in qualsiasi servitore RPC


> [!warning] Overrun
> Non c'è problema di sovrascrivere i dati con chiamate successive in quanto dopo la `clnt_call` so che i miei dati sono arrivati alla driver sotto (TCP).

