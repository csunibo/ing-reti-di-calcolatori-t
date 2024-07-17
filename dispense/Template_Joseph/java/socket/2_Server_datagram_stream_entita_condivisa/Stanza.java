import java.io.Serializable;

public class Stanza implements Serializable{
    private static final int K = 10; //numero massimo di utenti per stanza
    private String nome;
    private String stato;
    private String[] utenti = new String[K];

    public Stanza(String nome, String stato){
        this.nome = nome;
        this.stato = stato;
        for(int i=0; i<K; i++){
            utenti[i] = "L";
        }
    }


    public String getNome(){
        return this.nome;
    }

    public String getStato(){
        return this.stato;
    }

    public String[] getUtenti(){
        return this.utenti;
    }

    public void setUtenti(String[] utenti){
        for(int i=0; i<utenti.length;i++){
            this.utenti[i] = utenti[i];
        }
        
    } 

    public boolean eliminaUtente(String nomeUtente){
        boolean res = false;

        for(int i=0; i<utenti.length; i++){
            if(utenti[i].compareTo(nomeUtente)==0){
                utenti[i] = "L";
                res = true;
            }
        }

        if(res){
            //metto gli utenti in ordine
            String[] newUtenti = new String[K];
            int dimNewUtenti = 0;
            for(int i =0;i<K;i++){
                if(this.utenti[i] != "L"){
                    newUtenti[dimNewUtenti++] = utenti[i];
                }
            }
            for(int j = dimNewUtenti; j<K; j++){
                newUtenti[j] = "L";
            }
            this.utenti = newUtenti;
        }
        

        return res;
    }
}
