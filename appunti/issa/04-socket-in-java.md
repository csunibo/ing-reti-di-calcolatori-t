> Le socket rappresentano il terminale locale (end point) di un canale di comunicazione bidirezionale (da e per l’esterno)

[[4 - Socket in Java.pdf#page=4&selection=35,0,53,20|4_SocketJava_24, pagina 4]]

### Tipi di comunicazione
- con **connessione** - socket **stream**
- senza connessione - socket **datagram**

### Classi in Java
#### Senza connessione
- `DatagramSocket` (Client / Server)

#### Con connessione
- `Socket` (Client)
- `ServerSocket` (Server)


### Nomi per le socket
Un servizio su un nodo è identificato da un *nome globale*, o **endpoint**, composto da:
- indirizzo IP (4 byte per IPv4)
- porta (16 bit)

**La socket lega il processo ad un nome globale** per inviare o ricevere dei messaggi.

> [!warning]
> Non è necessario conoscere il PID locale del processo!

## Socket DATAGRAM

Classe `java.net.DatagramSocket`.

> Il costruttore DatagramSocket crea socket UDP e fa un binding locale a una specificata porta e nome IP: la socket è pronta

[[4 - Socket in Java.pdf#page=17&selection=71,0,77,47|4_SocketJava_24, pagina 17]]

Metodi definiti:
```java
void send(DatagramPacket p);    // Sends a datagram packet from this socket
void receive(DatagramPacket p); // Receives a datagram packet from this socket
void close();                   // Closes this datagram socket
```

La `send` è asincrona, ovvero delega l'operazione al kernel e ritorna *subito*.
La `receive` delega la ricezione al kernel e rimane in attesa fino all'arrivo locale dell'informazione.

#### Classe `InetAddress`

> Classe con cui vengono rappresentati gli indirizzi Internet, astraendo dal modo con cui vengono specificati (a numeri o a lettere) → portabilità e trasparenza 
> Sono costanti

[[4 - Socket in Java.pdf#page=30&selection=0,6,52,7|4_SocketJava_24, pagina 30]]

**Non ci sono costruttori!**

```java
public static InetAddress getByName(String hostname);
```
Restituisce un oggetto `InetAddress` rappresentante l'host specificato (come nome o indirizzo numerico). 

Con il parametro `null` ci si riferisce all'indirizzo di default della macchina locale.

```java
public static InetAddress[] getAllByName(String hostname);
```
Restituisce un array di oggetti `InetAddress`; utile in casi di più indirizzi IP registrati con lo stesso nome logico.

```java
public static InetAddress getLocalHost();
```
Restituisce un oggetto `InetAddress` corrispondente alla macchina locale. 

Se tale macchina non è registrata oppure è protetta da un firewall, l’indirizzo è quello di loopback: `127.0.0.1`.

#### Classe `DatagramPacket`

Classe per **preparare** ed **usare datagrammi** che specificano:
- cosa comunicare (array di byte)
- con chi comunicare (interi per la porta e `InetAddress`)


##### Parte dati per `DatagramPacket`

```java
public DatagramPacket(
	byte[]      buf,     // array di byte dati
	int         offset,  // indirizzo inizio
	int         lenght,  // lunghezza dati
	InetAddress address, // IP destinazione
	int         port     // porta destinazione
);

public InetAddress getAddress();
public void setAddress(InetAddress addr);

public int getPort();
public void setPort();

public byte[] getData();
public void setData();
```

### Protocollo di comunicazione C/S

#### Invio
```java
// Creazione socket mittente
DatagramSocket socket = new DatagramSocker();

// Preparazione informazione da inviare
byte[] buf = {'C', 'i', 'a', 'o'};
InetAddress addr = InetAddress.getByName("xxx.xxx.xxx.xxx");
int port = 1900;

DatagramPacket packet = new DatagramPacket(buf, buf.lenght, addr, port);
socket.send(packet);
```

#### Ricezione
```java
// Creazione socket Ricevente Set di IP e porta
InetAddress addr = InetAddress.getByName("xxx.xxx.xxx.xxx");
DatagramSocket socket = new DatagramSocket(addr, 1900); 

// Parte ricevente di comunicazione: Preparazione, attesa, e ricezione
int recport; 
InetAddress recadd;
byte[] res = new byte[200];
DatagramPacket packet = new DatagramPacket(res, res.length, recadd, recport);
packet.setData(res); // riaggancio della struttura dati
socket.receive(packet); // ricezione con attesa sincrona? 

// estrazione delle informazione dal datagramma 
recport = packet.getPort(); 
recaddress = packet.getAddress(); 
res = packet.getData(); 

...
```

### `DataOutputStream` e `DataInputStream`

`DataOutputStream` e `DataInputStream` offrono una serie di metodi per l’invio e la ricezione di tipi primitivi Java tra macchine virtuali diverse.

### Comunicazione multicast

I socket multicast sono un'**ulteriore classe** (classe D).

```java
MulticastSocket(int multicastport);
```

```java
InetAddress gruppo = InetAddress.getByName("229.5.6.7");
MulticastSocket s  = new MulticastSocket(6666);

s.joinGroup(gruppo); // entra nel gruppo `gruppo`
...
s.leaveGroup(gruppo); // esci dal gruppo `gruppo`
```

#### Invio di un msg
```java
byte[] msg = {'H', 'e', 'l', 'l', 'o'};

InetAddress gruppo = InetAddress.getByName("229.5.6.7");
DatagramPacket packet = new DatagramPacket(msg, msg.length, gruppo, 6666);

s.joinGroup(gruppo); 
s.send(packet); 
...
```

#### Ricezione di un msg
```java
byte[] buf = new byte[1000];
DatagramPacket recv = new DatagramPacket(buf, buf.length);
s.receive(recv); 
...
s.leaveGroup(gruppo);
```


### Opzioni di un socket

```java
void setSoTimeout(int timeout) throws ...
```
Definisce un `timeout` in millisecondi, dopo il quale la `receive` lancia eccezione. 
Se `timeout = 0`, nessun timeout.

```java
void setSendBufferSize(int size) throws ...
void setReceiveBufferSize(int size) throws ...
```
Varia il buffer di invio / ricezione.

```java
void setReuseAddress();
```
Per collegare più processi ad una certa porta senza lanciare eccezione per `port already in use`.


## Socket STREAM

Un canale è rappresentato da 
$$
\text{canale} := \{\underbrace{\langle P_C,IP_C\rangle}_{E_S},\underbrace{\langle P_S, IP_S \rangle}_{E_S} \}
$$

**NON esistono contemporaneamente canali con la stessa quadrupla**

Un canale TCP è uno **stream di byte**.

Banda infinita nei due versi.

Basta una variazione della quadrupla, quindi posso avere 
- tanti client che si connettono sulla stessa porta o 
- lo stesso client con più porte che si connette alla stessa porta

TCP ha una logica **AT MOST ONCE**.

> C'è anche un canale per dati urgenti largo 1 byte.

Se qualcosa va male TCP chiude, e non c'è garanzia.

In java ci sono due class: `Socket` (Cliente) e `ServerSocket` (Server).

A regime ci sono due istanze di `Socket`, ma la socket nel server si crea grazie alla `ServerSocket`.

## Esempio di trasferimento file in memoria

Caricare parti di file in memoria e trasferirli. **Non caricare tutto il file in memoria.**

Per mandare l'EOF, chiudiamo il nostro lato della socket, in modo tale che dall'altra parte arrivi l'EOF sulla socket.

Non vogliamo coordinare troppo quello che succede nel Client con quello che succede nel Server.

Nel canale TCP non mi serve coordinarmi con il ricevitore dei dati; se ne occupa il protocollo.

