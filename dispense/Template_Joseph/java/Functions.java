import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.rmi.RemoteException;
import java.util.StringTokenizer;

public class Functions {

    /*
     * conteggio del numero di linee di un file se il file contiene una linea con una 
     * parola specifica: questa operazione richiede un nome di file e la parola e cont
     * a il numero di linee del file;
     * il metodo  accetta come parametro d’ingresso il nome del file e la parola
     * e ne restituisce il numero di linee, oppure -1 in caso il file non esista o 
     * non contenga la parola cercata
     */

    //Funzione implementata per RMI, se utilizzata in altro contesto va cambiata, ad esempio il lancio della RemoteException
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

    //contare il numero delle linee di tutti i file di un direttorio che contengono almeno uno specificato numero di occorrenze
    public int conta_linee_occorrenze(String dir) throws Exception{
        String carString;
        char car;
        int occ, res;
        StringTokenizer tok = new StringTokenizer(dir, ";");
        dir = tok.nextToken();
        carString = tok.nextToken();
        car = carString.charAt(0);
        occ = Integer.parseInt(tok.nextToken());

        System.out.println("Richiesta di esplorazione di " + dir + " per il carattere " + car + " un numero di volte " + occ);
        
        res = -1;
        File dirCorr = new File(dir);
        if (dirCorr.exists()) {
            File[] files = dirCorr.listFiles();
            res = 0;
            int nOcc;
            for (int i = 0; i < files.length; i++) {
                if(files[i].getName().endsWith(".txt")){
                    System.out.println("Controllo il file " + files[i].getName());
                    BufferedReader br = null;

                    try {
                        br = new BufferedReader(new InputStreamReader(new FileInputStream(files[i])));

                        String line = null;
                        // esco dal ciclo alla lettura di un valore negativo -> EOF
                        while( (line = br.readLine()) != null){
                            nOcc = 0;
                            for(int j=0;j<line.length();j++){
                                if(line.charAt(j) == car) nOcc++;
                            }
                            
                            if(nOcc >= occ) res++;
                        }

                        br.close();
                    } catch (Exception e) {
                        throw new Exception(e.toString());
                    }
                }
            }
        } else {
            System.out.println("Dir non esistente....\n");
        }

        return res;
    }

    //accetta come parametro d’ingresso il nome del direttorio, e
    //restituisce la lista dei primi N (N<=6) nomi dei sottodirettori di primo livello trovati, se ci sono,
    //che contengono almeno 6 file di testo. In caso di direttorio inesistente, prevedere una segnalazione
    //di errore
    //@Override
    public String[] lista_sottodirettori(String dir) throws RemoteException {
        String[] res = null;
        int dimRes = 0;
        int dirNum = 0;

        File dirCorr = new File(dir);
        if (dirCorr.exists()) {
			File[] files = dirCorr.listFiles();
			for (int i = 0; i < files.length && dirNum <= 6; i++) {
                System.out.println("Controllo " + files[i].getName());
				if(files[i].isDirectory()){
                    System.out.println("File nel direttorio: ");
                    for (File file : files) {
                        System.out.println(file);
                    }

                    System.out.println("Controllo i file in" + files[i].getName());
                    File[] files2 = files[i].listFiles();
                    System.out.println("File nel direttorio: ");
                    for (File file : files2) {
                        System.out.println(file);
                    }

                    int txtNum = 0;

                    for (int j = 0; j < files2.length; j++) {
                        if(files2[j].getName().endsWith(".txt")){
                            System.out.println("Trovato un file di testo: " + files2[j].getName());
                            txtNum++;
                        }
                        
                    }

                    if (txtNum >= 6)
                        dirNum++;
                }
				
			}

            res = new String[dirNum];
            for (int i = 0; i < files.length && dimRes < 6; i++) {
                System.out.println("Controllo " + files[i].getName());
				if(files[i].isDirectory()){
                    System.out.println("Controllo i file in" + files[i].getName());
                    File[] files2 = files[i].listFiles();
                    int txtNum = 0;

                    for (int j = 0; j < files2.length; j++) {
                        if(files2[j].getName().endsWith(".txt")){
                            System.out.println("Trovato un file di testo: " + files2[j].getName());
                            txtNum++;
                        }
                        
                    }

                    if (txtNum >= 6){
                        res[dimRes++] = files[i].getName();
                    }
                }
				
			}

		} else {
			System.out.println("Dir non esistente....\n");
		}

        return res;
    }

}