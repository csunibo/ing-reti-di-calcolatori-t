### Cos'è il dispatching nello skeleton?

Capire quale metodo è stato invocato dal client

## Lookup e registrazione

> [!warning] 
> - I server possono registrarsi solo su un **registry RMI locale**
> - I client possono fare *lookup* anche su **registry RMI remoti**

^d66a27


> [!warning]
> Il client, dopo aver fatto il lookup, **non passa più dal registry RMI**, ma avvia una connessione diretta con il server che fornisce il servizio.
