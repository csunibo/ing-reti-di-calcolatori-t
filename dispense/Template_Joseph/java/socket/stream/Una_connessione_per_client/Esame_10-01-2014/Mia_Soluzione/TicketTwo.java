import java.io.Serializable;

public class TicketTwo implements Serializable {
    private String descrizione;
    private String tipo;
    private String data;
    private String luogo;
    private String disponibilita;
    private String prezzo;

    public TicketTwo(){
        this.descrizione = "L";
        this.tipo = "L";
        this.data = "L";
        this.luogo = "L";
        this.disponibilita = "L";
        this.prezzo = "L";
    }

    public String getDescrizione(){
        return this.descrizione;
    }

    public String getTipo(){
        return this.tipo;
    }

    public String getData(){
        return this.data;
    }

    public String getDisponibilita(){
        return this.disponibilita;
    }

    public String getPrezzo(){
        return this.prezzo;
    }

    public String getLuogo(){
        return this.luogo;
    }

    public void set(String descrizione, String tipo, String data, String luogo, String disp, String prezzo){
        this.descrizione = descrizione;
        this.tipo = tipo;
        this.data = data;
        this.luogo = luogo;
        this.disponibilita = disp;
        this.prezzo = prezzo;
    }

    public String toString(){
        return "Descrizione: " + this.descrizione + " \tTipo: " + this.tipo + " \tData: " + this.data + " \tLuogo: " + this.luogo + " \tDisponibilità: " + this.disponibilita + " \tPrezzo: " + this.prezzo + "\n";
    }

}
