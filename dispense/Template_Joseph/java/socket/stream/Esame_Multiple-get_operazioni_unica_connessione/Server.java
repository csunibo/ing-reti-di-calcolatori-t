// Server Concorrente

import java.io.*;
import java.net.*;
import java.util.StringTokenizer;




public class Server {
	public static final int PORT = 1099; //default port

	public static void main(String[] args) throws IOException {

		int port = -1;

		/* controllo argomenti */
	    try {
	    	if (args.length == 1) {
	    		port = Integer.parseInt(args[0]);
	    		if (port < 1024 || port > 65535) {
	    			System.out.println("Usage: java LineServer [serverPort>1024]");
	    			System.exit(1);
	    		}
	    	} else if (args.length == 0) {
	    		port = PORT;
	    	} else {
	    		System.out
	    			.println("Usage: java Server or java Server port");
	    		System.exit(1);
	    	}
	    } //try
	    catch (Exception e) {
	    	System.out.println("Problemi, i seguenti: ");
	    	e.printStackTrace();
	    	System.out
	          	.println("Usage: java Server or java Server port");
	    	System.exit(1);
	    }

	    ServerSocket serverSocket = null;

	    try {
	    	serverSocket = new ServerSocket(port);
	    	serverSocket.setReuseAddress(true);
	    	System.out.println("PutFileServerCon: avviato ");
	    	System.out.println("Server: creata la server socket: " + serverSocket);
	    }
	    catch (Exception e) {
	    	System.err
	    		.println("Server: problemi nella creazione della server socket: "
	    				+ e.getMessage());
	    	e.printStackTrace();
	    	System.exit(1);
	    }

	    try {
	    	while (true) {
	    		System.out.println("Server: in attesa di richieste...\n");

                Socket clientSocket = null;

	    		try {
	    			// bloccante fino ad una pervenuta connessione
	    			clientSocket = serverSocket.accept();
	    			//clientSocket.setSoTimeout(100000);
	    			System.out.println("Server: connessione accettata: " + clientSocket);
	    		}
	    		catch (IOException e) {
	    			System.err
	    				.println("Server: problemi nella accettazione della connessione: "
	    						+ e.getMessage());
	    			e.printStackTrace();
	    			continue;
	    		}

	    		// servizio delegato ad un nuovo thread
	    		try {
	    			new ServerThread(clientSocket).start();
	    		}
	    		catch (Exception e) {
	    			System.err.println("Server: problemi nel server thread: "
	    					+ e.getMessage());
	    			e.printStackTrace();
	    			continue;
	    		}

	    	} // while
	    }
	    // qui catturo le eccezioni non catturate all'interno del while
	    // in seguito alle quali il server termina l'esecuzione
	    catch (Exception e) {
	    	e.printStackTrace();
	    	// chiusura di stream e socket
	    	System.out.println("ServerCon: termino...");
            serverSocket.close();
	    	System.exit(2);
	    }
	    
	}


} // Server class


// Thread lanciato per ogni richiesta accettata
// versione per il trasferimento di file binari
class ServerThread extends Thread{

    private Socket clientSocket = null;

    /**
     * Constructor
     * @param clientSocket
     */
    public ServerThread(Socket clientSocket) {
        this.clientSocket = clientSocket;
    }

    public void run() {
        DataInputStream inSock;
        DataOutputStream outSock;
        String operazione, dir, line;
        byte[] buffer = new byte[1];
        int cont = 0;
        int read_bytes = 0;
        long daTrasferire = 0;
        DataInputStream src_stream = null;

        System.out.println("ServerThread avviato");

        try {
            try {
                // creazione stream di input e out da socket
                inSock = new DataInputStream(clientSocket.getInputStream());
                outSock = new DataOutputStream(clientSocket.getOutputStream());
                System.out.println("Create le socket in input e output");
            }
            catch(SocketTimeoutException ste){
                System.out.println("[ServerThread] Timeout scattato: ");
                ste.printStackTrace();
                System.out
                    .print("\n[ServerThread] ^D(Unix)/^Z(Win)+invio per uscire, solo invio per continuare: ");
                return;          
            }        
            catch (IOException ioe) {
                System.out
                    .println("[ServerThread] Problemi nella creazione degli stream di input/output "
                            + "su socket: ");
                ioe.printStackTrace();
                // il server continua l'esecuzione riprendendo dall'inizio del ciclo
                return;
            }
            catch (Exception e) {
                System.out
                    .println("[ServerThread] Problemi nella creazione degli stream di input/output "
                            + "su socket: ");
                e.printStackTrace();
                return;
            }
            

            while(clientSocket.isConnected()){
                System.out.println("Dentro il while...");
                try{
                    operazione = inSock.readUTF();
                }catch(SocketTimeoutException ste){
                    System.out.println("Timeout scattato: ");
                    ste.printStackTrace();
                    System.out
                        .print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti nome file: ");
                    // il client continua l'esecuzione riprendendo dall'inizio del ciclo
                    break;         
                }
                catch(Exception e){
                    System.out
                        .println("Problemi nella ricezione dell'esito, i seguenti: ");
                    e.printStackTrace();
                    System.out
                        .print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti nome file: ");
                    break;
                    // il client continua l'esecuzione riprendendo dall'inizio del ciclo
                }
                
                if(operazione.equals("1")){
                    //contare il numero delle linee che contengono almeno uno specificato numero di occorrenze

                    System.out.println("[ServerThread] Arrivata richiesta di contare il numero delle linee che contengono almeno uno specificato numero di occorrenze");

                    //leggo direttorio, carattere e numero di occorrenze separati da uno ;
                    try {
                        dir = inSock.readUTF();
                    }catch(SocketTimeoutException ste){
                        System.out.println("Timeout scattato: ");
                        ste.printStackTrace();
                        System.out
                            .print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti nome file: ");
                        // il client continua l'esecuzione riprendendo dall'inizio del ciclo
                        break;         
                    }
                    catch(Exception e){
                        System.out
                            .println("Problemi nella ricezione dell'esito, i seguenti: ");
                        e.printStackTrace();
                        System.out
                            .print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti nome file: ");
                        break;
                        // il client continua l'esecuzione riprendendo dall'inizio del ciclo
                    }


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
                    
                    System.out.println("[ServerThread] Invio:\n" + res);

                    outSock.writeUTF(Integer.toString(res));

                }
                else{
                    //trasferire dal server al client tutti i file di binari di un direttorio con almeno un alfabetico

                    System.out.println("[ServerThread] Arrivata richiesta trasferire dal server al client tutti i file di binari di un direttorio con almeno un alfabetico");

                    //leggo direttorio
                    try {
                        dir = inSock.readUTF();
                    }catch(SocketTimeoutException ste){
                        System.out.println("Timeout scattato: ");
                        ste.printStackTrace();
                        System.out
                            .print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti nome file: ");
                        // il client continua l'esecuzione riprendendo dall'inizio del ciclo
                        break;         
                    }
                    catch(Exception e){
                        System.out
                            .println("Problemi nella ricezione dell'esito, i seguenti: ");
                        e.printStackTrace();
                        System.out
                            .print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti nome file: ");
                        break;
                        // il client continua l'esecuzione riprendendo dall'inizio del ciclo
                    }

                    File dirCorr = new File(dir);
                    if (dirCorr.exists() && dirCorr.isDirectory()) {
                        File[] files = dirCorr.listFiles();
                        for (int i = 0; i < files.length; i++) {
                            File fileCorr = files[i];
                            boolean containsAlphabetic = false;

                            //controllo se ha un alfabetico nel nome
                            for (char c : fileCorr.getName().toCharArray()) {
                                if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
                                    containsAlphabetic = true;
                            }

                            if (fileCorr.isFile() && containsAlphabetic) {
                                System.out.println("File con nome: " + fileCorr.getName());
                                outSock.writeUTF(fileCorr.getName());
                                String result = inSock.readUTF();
                                if (result.equals("attiva")) {
                                    System.out.println("Il file " + fileCorr.getName()
                                            + " NON e' presente sul server: inizio il trasferimento");
                                    daTrasferire = fileCorr.length();
                                    outSock.writeLong(daTrasferire);
                                    // In linea trasferiamo il file
                                    src_stream = new DataInputStream(new FileInputStream(fileCorr.getAbsolutePath()));
                                    // ciclo di lettura da sorgente e scrittura su destinazione
                                    try {
                                        // esco dal ciclo quando ho letto il numero di byte da trasferire
                                        cont = 0;
                                        while (cont < daTrasferire) {
                                            read_bytes = src_stream.read(buffer);
                                            outSock.write(buffer, 0, read_bytes);
                                            cont += read_bytes;
                                        }
                                        outSock.flush();
                                        System.out.println("Byte trasferiti: " + cont);
                                    }
                                    // l'eccezione dovrebbe scattare solo se ci aspettiamo un numero sbagliato di byte da leggere
                                    catch (EOFException e) {
                                        System.out.println("Problemi, i seguenti: ");
                                        e.printStackTrace();
                                    }
                                } else if (result.equals("salta file"))
                                    System.out.println("Il file " + fileCorr.getName()
                                            + " era gia' presente sul client e non e' stato sovrascritto");
                                else {
                                    System.out.println("ServerThread: violazione protocollo...");
                                    System.exit(4);
                                }
                            } else {
                                System.out.println("File saltato");
                            }
                        }
                    } else {
                        System.out.println("Error: the specified directory either does not exist or is not a directory");
                        //scrivo "no" per indicare che il direttorio non è presente
                        outSock.writeUTF("no");
                        continue;
                    }
        
                    // scrivo fine sulla socket per indicare che i file sono finiti
                    outSock.writeUTF("fine");

                    System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti immetti il nome della cartella: ");
                }
            }
        }
        // qui catturo le eccezioni non catturate all'interno del while
        // in seguito alle quali il server termina l'esecuzione
        catch (Exception e) {
            e.printStackTrace();
            System.out
            .println("Errore irreversibile, Server: termino...");
            System.exit(3);
        }

        System.out.println("ServerThread: termino...");
        try {
            if (clientSocket.isConnected())
                clientSocket.close();
        } catch (IOException e) {
            System.out.println("Impossibile chiudere la socket");
            e.printStackTrace();
        }

    } // run

} // ServerThread class

