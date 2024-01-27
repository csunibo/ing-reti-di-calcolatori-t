PDF: [[2 - Integrazione di sistemi.pdf]]

## Risorse
Le risorse non sono solamente quelle Hardware, ma sono anche quelle create dai software.

Parliamo di comunicazione, quindi di diverse macchine che parlano l'una con l'altra.

## Tipologia di programmi

- Programmi piccoli (**in the small**)
- Schemi di soluzione definiti e standard

## Leggibilità

E' importante scrivere un programma per quanto possibile *semplice* e ***leggibile***.

## Parte statica e dinamica del programma

La fase *statica* del programma *ci interessa relativamente*.

Siamo invece molto interessati alla **parte dinamica**.

## Deployment

> Il deployment è la fase in cui decidiamo come allocare le risorse logiche alle risorse fisiche disponibili (che abbiamo preparato decidendo una configurazione di esecuzione)

[[2 - Integrazione di sistemi.pdf#page=8&selection=40,0,59,1|2_IntegrazSistema_24, page 8]]

## Binding dinamico

Siccome il binding deve essere dinamico, c'è un servizio fondamentale: il **servizio di nomi**.

## Servizio di nomi
Posso fare riferimento ai servizi di nomi per **chiedere l'attuale binding di una risorsa logica**.

## Sistemi a livelli

## Variabili statiche e dinamiche

### Variabili dinamiche
- Variabili locali
- Variabili di heap

### Variabili statiche
Se deve stare per tutta la parte del programma in utilizzo conviene porla come statica.

## Primitive
> [!info] Ricorda
> Le primitive sono **ATOMICHE**

## Trattamento di file

- Un file è sempre uno **stream di byte**.
- Un file **non sta in memoria**. Anche se è 20 bytes.

#### Tipi di file
Per convenienza distinguiamo i
- file generici
- file testo, ovvero costituiti da caratteri stampabili e il contenuto è solitamente diviso in linee.

In UNIX **non c'è nel contenuto del file un carattere che indica la fine del file**. La fine del file è contenuto nel file descriptor.
