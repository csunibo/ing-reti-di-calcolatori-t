PDF: [[3 - Modelli di base.pdf]]


## Problemi teorici
- Concorrenza
- **Nessun tempo globale**
- Fallimenti indipendenti
## Vantaggi
- Trasparenza della locazione (sistema di nomi)
- Dinamicità del sistema (molta flessibilità)
- Quality of service

## Architetture di interesse

Architetture **MIMD** (multiple instruction multiple data)

Ogni nodo è capace di operare indipendentemente e comunicare

## Architettura software

- **Mapping**: *configurazione* per l'architettura e **l'allocazione della parte logica a risorse fisiche e località**
- **Binding**: come ogni entità dell'**applicazione** si lega a **risorse effettive del sistema**

La gestione è quindi **dinamica** (binding).

## Modello d'azione
- Processi come **attività**
- **Azioni** specifiche su dati
- **Protocolli** per **azioni coordinate**

> **Protocollo**: complesso di regole e procedure cui ci si deve attenere in determinate attività per la esecuzione corretta

[[3 - Modelli di base.pdf#page=7&selection=36,0,40,46|3_Modelli_24, page 7]]

## Modelli di comunicazione
Entrambi permettono di esprimere qualsiasi azione si possa desiderare ed esprimere nel distribuito.

- ==**Cliente servitore**==
	Il modello di servizio prevede un **fruitore** e un **fornitore** dello stesso servizio

- **Scambio di messaggi**
	Abbiamo un mittente che manda informazioni ad uno o più riceventi **senza prevedere risposta**

| Client server | Scambio di messaggi |
| --- | --- |
| sincrono (*c'è risposta*) | **a**sincrono (*non c'è risposta*) |
| bloccante (*attende la risposta*) | **non** bloccante (*non attende la risposta*) |
| comunicazione diretta | comunicazione **in**diretta |
| **singolo ricevente** | **molteplici** riceventi |

Utilizzeremo lo *scambio di messaggi* per realizzare il *C/S*. E poi strumenti *C/S* di livello applicativo.


### *C/S* usando lo *scambio di messaggi*

Consideriamo due API locali:
- `send()`
- `receive()`

Il C/S si implementa con **due** scambi di messaggi e **alcune regole di protocollo**.

1. Il cliente invia con una `send` (request) la richiesta al servitore e aspetta il risultato, che riceve con una `recv` (answer).
2. Il servitore considera una richiesta tra quelle in arrivo depositate con `recv` (request) e, dopo aver prodotto un risultato, lo manda al cliente con `send` (response).



## Nostro default per C/S
- è sempre **molti ad 1**
- **sincrono**
- **bloccante**
- **asimmetrico**
- **dinamico**

## C/S base

- Il cliente ha **l'iniziativa**, chiede un servizio e **aspetta una risposta**.
- Il servitore deve **ricevere** la richiesta, **eseguire il servizio** e **dare una risposta** al cliente
- Il servitore deve servire **molti clienti**.


Quindi abbiamo due entità:
- il cliente **chiede**
- il server **risponde ai molteplici clienti**

Il cliente invoca il servizio e *ne aspetta il completamento*.
- Modello **sincrono**: prevedo una risposta (decisione **semantica**, di protocollo)
- **Bloccante**: attendo la risposta (decisione **locale**)

Il servitore attende richieste e le riceve, le realizza e poi risponde:
- *diverse realizzazioni*

Il problema del **rendez-vous** (come sincronizzo i processi comunicanti?) è risolto dal server che è sempre in attesa di richieste di servizio.

**Se il server non è attivo si segnala l'errore**

### Proprietà del modello

| Nome        | Descrizione |
| ----------- | ----------- | 
| Sincrono    | si prevede la risposta dal servitore al cliente |
| Bloccante   | il cliente **aspetta** la risposta dal servitore |
| Asimmetrico | il cliente conosce il servitore **a priori**, il contrario non è vero |
| Dinamico    | il legame tra cliente e servitore è dinamico, ossia può cambiare il servitore che risponde alle richieste tra diverse invocazioni |

### Progetto del cliente / servitore

Il progetto del server è più complesso di quello del client.

Il server deve:
- essere **sempre pronto** alle eventuali richieste
- durare all'infinito (demone)

Come server ci preoccuperemo anche di accedere alle risorse locali del sistema, considerando molteplici clienti.

*In teoria*, come server dovremmo preoccuparci anche di:
- integrità dei dati
- accessi concorrenti
- autenticazione
- autorizzazione
- privacy
*Nel modello base le trascuriamo*.

### Osservazioni per il cliente (variazioni rispetto al modello base)

Se la risposta non arriva (entro un timeout) possiamo considerare il server down? No!

Può essere che sia solo **congestionato**.

Ma quanto aspetta il client? Per sempre? Non è accettabile!

Impostiamo un **timeout**, nella fase di design, che ci dia un modo di fare ***recovery***.

Dopo il timeout il cliente genera **eccezione**, permettendoci di:
- ripetere?
- provare con un altro server?
- chiudere tutto?
- chiedere all'utente?

### Modelli di interazione del client

- modello di interazione **pull**: il client ha sempre l'iniziativa
- modello di interazione **push**: il cliente fa richiesta e fa altro mentre attende che il server gli invii la risposta

### Modelli di interazione del server

# Servizi e server di nomi

Il supporto al sistemi di nomi è implementato tramite un **servizio di risoluzione dei nomi** a cui i server si registrano.

### Principi di organizzazione
- **Partizionamento**: molti servitori, ciascuno con un area di competenza e di gestione **limitata** e **diversa**
- **Replicazione**: molti servitori, ciascuno capace di lavorare anche **sulla stessa area**.
- **Efficienza del servizio**: uso di **cache per località**

## DNS
I singoli nomi sono **case insensitive** e al max **63 char** per dominio e **max 255 char in totale**.

### Server di zona
I domini sono organizzati su **responsabilità primaria di un server** (*server di zona*).

### Replicazione del DNS
Ogni dominio corrisponde a più server di nomi.
- **Primary master**
- **Secondary master**: copie del primary, con consistenza (non massima) garantita dal protocollo DNS.
- I secondari entrano in funzione solo in caso di guasto al primario.
- Il primario di una zona può diventare il **backup** (master secondario) di un altra zona
- allo startup e ad intervalli prefissati i secondari aggiornano i dai dal primario.

### Protocolli di DNS

- replicazione dei servitori
- partizionamento
- località
