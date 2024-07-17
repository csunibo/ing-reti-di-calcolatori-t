const N=10;
const K=7;
const MAX_STRING = 20;

struct AggStanza{
    char nomeStanza[MAX_STRING];
    char tipo;
};

struct Utente{
    char nome[MAX_STRING];
};

struct Stanza{
    char nomeStanza[MAX_STRING];
    char tipo;
    Utente utente[K];
};

struct ListaStanze{
    int ris;
    Stanza elenco[N];
};

struct AggUtente{
    char nomeStanza[MAX_STRING];
    char nomeUtente[MAX_STRING];
};

program OPFILEPROG{
    version OPFILEVERS{
        int AGGIUNGI_STANZA(AggStanza) = 1;
        ListaStanze ELIMINA_UTENTE(string)=2;
        int AGGIUNGI_UTENTE(AggUtente)=3;
        int ELIMINA_STANZA(string)=4;
        ListaStanze VISUALIZZA_UTENTE(string)=5;
    }=1;
}=0x20000016;
