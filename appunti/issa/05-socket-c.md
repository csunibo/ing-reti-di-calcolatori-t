`h_addr`

## Socket stream

Il processo applicativo non rimane in attesa, è la driver che rimane in attesa dopo il comando impartito dal processo.

Abbiamo quindi una porta utilizzata dal servitore per più canali.

### Protocollo di socket stream in C

Sia il cliente che il servitore:
- `socket` creazione della socket
- `bind` legame al sistema dei nomi locale

Parte **asimmetrica** - Servitore
- `listen`
- `accept`
Consente al servitore di essere disaccoppiato dai clienti. Altrimenti sarebbero accoppiati in modo stretto (rendevouz).

Parte **asimmetrica** - Cliente
- `connect`

### Read e write sulle socket

```c
recnum  = read(s, buff, lenght);
sentnum = write(s, buff, lenght);
```


### Primitive: `socket`

```c
int socket(address_family, type, protocol);
// Esempio
int s = socket(AF_INET, SOCK_STREAM, 0);
```


> [!warning] **Fallisce** se:
> - i parametri sono errati

> [!warning]
> Non interagisce con le driver!

### Primitive: `bind`

> [!warning] **Fallisce** se:
> - l'IP non è della mia macchina
> - la porta è già impegnata
> - non ho i permessi di bindare a quella porta

### Primitiva: `connect`

Dopo la `connect` esiste la connessione
- lato client e 
- lato servitore **SOLO A LIVELLO DELLA DRIVER** (non a livello dell'applicazione)

> [!warning]
> **Il server deve aver prima fatto la `listen`, altrimenti il three-way handshaking fallisce `RST`.**
#### Caratteristiche
- Il three-way handshaking è fatto dalla `connect`.
- Se fallisce il three-way handshaking **fallisce la `connect`**.
- La connect **ritenta** se fallisce.

#### Valore di ritorno
Ritorna un valore $<0$ se fallisce.

### Primitiva: `listen`

> [!warning] Fallisce se:
> - Socket non creata
> - `bind` non fatta

Crea la coda a livello trasporto per le richieste di connessione.

### Primitiva: `accept`

# Modalità delle socket
## Opzioni per le socket

```c
#include <sys/types.h>
#include <sys/socket.h>

int getsockopt(int s, level, optname, *optval, *optlen);
int setsockopt(int s, level, optname,  optval, optlen);
```

- `s`: socket descriptor
- `level`: livello di procollo. Noi useremo `SOL_SOCKET`
- `optname`: nome dell'opzione
- `optval`: puntatore ad un area di memoria per valore
- `optlen`: lunghezza del quarto argomento

### Timeout per le operazioni
```c
int wait_time = 10;
int result = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &waittime, sizeof(waittime));
```


### Dimensioni buffer
```c
int buf_size = 10;
int result = setsockopt(s, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size));
```

### `SO_REUSEADDR`
Di solito il sistema non permette a più di un utilizzatore alla volta di utilizzare una porta locale.

Con l'opzione, si chiede che la socket sia **senza controllo dell'unicità del'associazione**.

E' utile nel caso di **server che possono riavviarsi velocemente in caso di crash**.

Si ricordi che la porta rimane impegnata dopo una close per anche molte decine di secondi e senza l’opzione dovremmo aspettare troppo per il riavvio.

Il processo che tenti di agganciarsi alla stessa porta per il riavvio, senza la opzione potrebbe incorrere in un fallimento **fino a che la memoria della porta non fosse libera (e i dati tutti regolarmente inviati)**.

### Durata close sulla socket

`SO_LINGER` modifica il comportamento della `close`.

La `SO_LINGER` **sincronizza la socket con la driver**. Dopo che la faccio la `linger` la `close` aspetta che vengano inviati tutti i dati.

```c
struct linger {
	int l_onoff;  // linger attiva / non attiva
	int l_linger; // attesa in secondi
}
```

### Roba meno standard - `ioctl` e `fcntl`

Le primitive in UNIX sono **sincrone bloccanti rispetto alla visione locale a livelli**, ovvero aspettano finché non ho *consegnato le cose* al livello sottostante.

`ioctl` e `fcntl` permettono di cambiare il comportamento di un file aperto. Posso quindi fare operazioni **sincrone non bloccanti**.

**Di solito queste primitive sono poco portabili tra diversi sistemi.**

## Socket non bloccanti

> [!warning]
> In generale se faccio una `write` devo aspettare la `SIGIO` prima di modificare l'area di memoria che ho passato alla driver. 

Non vogliamo creare un processo per ogni socket aperta, che essa sia datagram o stream o listen.

C'è un problema se vogliamo fare servitori **concorrenti NON paralleli**. Il processo non può vedere **senza bloccare** se una socket ha dati, il kernel si però.

### Primitiva `select` - multi receive


