OSI è un modo di inquadrare qualsiasi soluzione.

E' nato per la **gestione di sistemi**, quindi **comunicare per fare gestione**

> Controllare, coordinare, monitorare sistemi interconnessi eterogenei per consentirne una gestione efficiente e a distanza senza porre limiti di località e di coresidenza

[[6 - OSI.pdf#page=2&selection=47,0,61,11|6 - OSI, pagina 2]]

## Standard generale
OSI nasce per razionalizzare **qualunque tipo di comunicazione tra sistemi diversi**.

> OSI propone standard e schemi di progetto astratti

### Caratteristiche
- organizzato a **livelli**
- descritto ad **oggetti**
- **astratto** e **senza legami** con realizzazioni

### Da chi viene fatto?
OSI è uno standard di comitati

- Organizzazioni pubbliche
	- ISO
	- IEC
	- CCITT

- Organizzazioni *industriali*:
	- ECMA
	- IEEE
	- EIA

*Vedi slides*

## Architettura

Dividiamo un problema in **livelli** basati sui **principio di astrazione**. 

La formalizzazione dei livelli è **molto precisa**.

> [!warning] 
> **I livelli di OSI non sono saltabili**
> 
> Le implementazioni però possono implementare più livelli, cortocircuitare livelli, etc...

>[!warning] 
>Il livello **applicazione** è **di supporto all'applicazione**

Ogni livello ha un obiettivo preciso:
- **Implementazione**: Tra i livelli in **orizzontale** si instaurano i **PROTOCOLLI**
- **Semantica**: Tra i livelli in **verticale** si creano delle **INTERFACCE**, ovvero ha un servizio che fornisce al livello sovrastante. (OSI non dice **come** fornire il servizio ai livelli superiori)

Interfaccia: servizio offerto
Protocollo: come realizzo il servizio offerto


> [!tip] Di solito i sistemi intermediari non devono implementare tutto lo stack dei livelli.

## Connessioni OSI

Tutte le connessioni OSI sono con QoS, quindi non come in IP che i pacchetti devono passare sempre dallo stesso circuito.

**Cosa succede se cade un nodo?** Bisogna ristabilire la connessione!

## Nomi ed entità

Per comunicare con B devo sapere tutti i *nomi* di B (a partire dal network).

## Service access point
Il modo in cui il livello sovrastante chiede servizi al livello sottostante.

## Meccanismi
Lo standard OSI implementa solamente i **meccanismi**, non le politiche con cui vengono implementati questi.

In questo modo con gli stessi meccanismi possono essere utilizzati per molto tempo, se fatti bene, mentre le policy cambiano più spesso.

## Formati
I messaggi scambiati dai diversi componenti si chiamano **data unit**. Le dividiamo in:
- Protocol data unit (PDU), tra componenti dello stesso livello
- Service data unit (SDU) o interface data unit (IDU), tra componenti di livello diverso (adiacenti)

## Connessione

Una connessione **ha qualità**.

La connessione viene se distrutta **ricreata dal livello di connessione**.

Qualità per OSI vuol dire **qualità a tutti i costi**. Potrebbe dire anche *qualsiasi tipo di guasto ci sia*, anche se questo è irreale.
### Primitive e forme

Una generica implementazione non deve per forza implementare tutte le forme. Potrebbe solamente implementare un servizio non confermato o un servizio confermato.

Normalmente se c'è una connessione che garantisce la qualità posso mandare i dati in maniera asincrona (non ho bisogno della conferma).


## Livelli e trasporto

**Si può avere multiplexing in alto o in basso.** 

### Trasporto

Livello **end-to-end**

In OSI tutti gli intermedi partecipano alla qualità.

### Livello di sessione 

*La sessione è il **dialogo***

Si dice spesso che le socket sono *di sessione* ma non hanno niente di implementato di sessione OSI.

- La sessione è **bidirezionale**
- Può garantire diverse **attività** separate e diverse
- Ha garanzie di **correttezza** e **affidabilità**

#### Primitive

Ovviamente **apertura** e **chiusura** della sessione.

Gestone dell'interazione:
- half-duplex, full-duplex, simplex

Gestione della **sincronizzazione**: checkpoints - primitive sincrone che mi confermano il fatto che sono arrivati i dati

Gestione dell'**autorizzazione**


Esistono le **eccezioni**.

#### Punti di sincronizzazione

- Maggiori: si devono confermare per forza e mi risponde solo se ha ricevuto tutti i dati precedenti


### Livello di presentazione

- Parte **astratta** dell'accordo
- Parte **concreta** dell'accordo

