## Due livelli di TCP/IP

- TCP - Transmission control protocol
	- Flusso di byte bidirezionale a **canale virtuale** *best effort*. Dati:
		- non duplicati
		- affidabili
		- con controllo di flusso
- UDP - User datagram protocol


Lo **STACK** dei protocolli è
- a **basso overhead**
- **best effort**

## Protocollo ARP

**ARP** è un protocollo **solo locale**, basato su **broadcast**

Utilizzo di una **memoria cache** per mantenere le associazioni {indirizzo ip - indirizzo fisico}

Inoltre, per questioni di efficienza:
- l'associazione relativa alla macchina richiedente memorizzata anche dalla macchina che risponde
- ogni richiesta broadcast viene memorizzata da tutti
- ogni nuova macchina al collegamento invia sulla rete locale un broadcast con la propria coppia.

## Protocollo RARP

## DHCP

*RFC 2131*: Dynamic Host Configuration Protocol (DHCP)


### ICMP
*Internet Control Message Protocol*

- sono imbustati in un datagramma IP
- sono soggetti alle stesse regole di routing
- non hanno priorità
- possono essere persi
-  possono causare ulteriori congestioni