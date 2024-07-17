
/**
 * ServerImple.java
 * 	Implementa i servizi dichiarati in RemOp.
 * */

import java.io.*;
import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class ServerImpl extends UnicastRemoteObject implements RemOp {

	// Costruttore
	public ServerImpl() throws RemoteException {
		super();
	}

	/*
     * conteggio del numero di linee di un file se il file contiene una linea con una 
     * parola specifica: questa operazione richiede un nome di file e la parola e cont
     * a il numero di linee del file;
     * il metodo  accetta come parametro d’ingresso il nome del file e la parola
     * e ne restituisce il numero di linee, oppure -1 in caso il file non esista o 
     * non contenga la parola cercata
     */

    public int conta_numero_linee(String nomeFile, String parola) throws RemoteException {
        int res = 0;
        String line;
        boolean containsParola = false;

        File f = new File(nomeFile);

        if(!f.exists() || f.isDirectory()){
            return -1;
        }

        BufferedReader br = null;

        try {
            br = new BufferedReader(new InputStreamReader(new FileInputStream(f)));

            // esco dal ciclo alla lettura di un valore negativo -> EOF
            while( (line = br.readLine()) != null){
                if( (line.indexOf(parola)!= -1) && !containsParola)
                    containsParola = true;
                
                res++;
            }

            br.close();

            if(containsParola) return res;
            else return -1;
        } catch (Exception e) {
            throw new RemoteException(e.toString());
        }
    }


    /*
     * lista dei file di un direttorio che contengono al loro interno una linea con 
     * una parola specifica: questa operazione richiede un nome di direttorio
     * e una parola, quindi visualizza la lista dei file che si trovano nel 
     * direttorio richiesto e che contengono al loro interno almeno una
     * linea con una o più un’occorrenze della parola cercata;
     * questo metodo accetta come parametri d’ingresso il nome del direttorio e la 
     * parola cercata e restituisce la lista dei nomi di file che
     * contengono al loro interno almeno una linea con una o più occorrenze
     * della parola cercata.
     */

    public String[] lista_nomi_file_contenenti_parola_in_linea(String direttorio,
        String parola) throws RemoteException {
        String[] temp;
        File f = new File(direttorio);

        if ((!f.exists()) || (!f.isDirectory()))
            return null;
        
        temp=f.list();
        int nomiFileDaRestituire=0;
        boolean[] fileDaRestituire = new boolean[temp.length];

        for( int i=0; i<fileDaRestituire.length; i++)
        fileDaRestituire[i]=false;
        for(int i=0; i<temp.length; i++)
            if( conta_numero_linee(temp[i], parola) > 0 ){
                fileDaRestituire[i]=true;
                nomiFileDaRestituire++;
            }

        String[] risultato = new String[nomiFileDaRestituire];
        // Uso la variabile come indice per riempire risultato
        nomiFileDaRestituire=0;

        for(int i=0; i<temp.length; i++)
            if( fileDaRestituire[i] == true ){
                risultato[nomiFileDaRestituire]=temp[i];
                nomiFileDaRestituire++;
            }

        return risultato;
    }

	public static void main(String[] args) {

		int registryPort = 1099;
		String registryHost = "localhost";
		String serviceName = "Server";

		// Controllo parametri
		if (args.length != 0 && args.length != 1) {
			System.out.println("Sintassi: ServerImpl [registryPort]");
			System.exit(1);
		}
		if (args.length == 1) {
			try {
				registryPort = Integer.parseInt(args[0]);
				if(registryPort<1024 || registryPort>65535){
					System.out.println("\"Usage: java Server_stream [1024<=serverPort<=6553]\"");
					System.exit(2);
				}
			} catch (Exception e) {
				System.out.println("Sintassi: ServerImpl [registryPort], registryPort intero");
				System.exit(2);
			}
		}

		// Registrazione del servizio RMI
		String completeName = "//" + registryHost + ":" + registryPort + "/" + serviceName;
		try {
			ServerImpl serverRMI = new ServerImpl();
			Naming.rebind(completeName, serverRMI);
			System.out.println("Server RMI: Servizio \"" + serviceName + "\" registrato");
		} catch (Exception e) {
			System.err.println("Server RMI \"" + serviceName + "\": " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}