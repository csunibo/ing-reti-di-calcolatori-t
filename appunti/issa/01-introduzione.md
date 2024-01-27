Questo corso si dovrebbe chiamare "sistemi distribuiti"

**Modello guida**: cliente / servitore 
(Esiste anche il modello a **scambio di messaggi**)

Lavoreremo con le **socket**, che servono per scambio di messaggi, ma sono molto adatte a creare relazioni di tipo cliente / servitore.

## Obiettivo del corso
Utilizziamo il protocollo TCP da utilizzatore, quindi cosa c'è sopra il TCP.
Ci interessano i livelli sopra il trasporto, di cui anche i dettagli architetturali.

Importante l'utilizzo delle risorse. -> Programmazione di sistema e NON applicativa

Lavoriamo a livello di infrastruttura e di **costo delle risorse**, in particolare **risorse di rete**.

# Obiettivo base del corso
Modello cliente / servitore

Il cliente chiede il servizio e aspetta la risposta.
Il servitore riceve una richiesta e manda la risposta.

## Modello filtro
Ricevo dei dati in ingresso, li elaboro tutti e fornisco dei dati in uscita.