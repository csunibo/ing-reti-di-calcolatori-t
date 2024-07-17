// Server Concorrente

import java.io.*;
import java.net.*;
import java.util.StringTokenizer;




public class Server {
    private static final int N = 20;
    private static TicketTwo[] stato = new TicketTwo[N];
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
	    			.println("Usage: java PutFileServerThread or java PutFileServerThread port");
	    		System.exit(1);
	    	}
	    } //try
	    catch (Exception e) {
	    	System.out.println("Problemi, i seguenti: ");
	    	e.printStackTrace();
	    	System.out
	          	.println("Usage: java PutFileServerThread or java PutFileServerThread port");
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

        System.out.println("\nInizializzo la struttura");
        //Inizializzo la struttura
        for(int i=0;i<N;i++){
            stato[i] = new TicketTwo();
        }
        stato[0].set("Sting", "Concerto", "11/01/2014", "Verona", "40", "40" );
        stato[1].set("Junentus-Inter", "Calcio", "03/05/2014", "Torino", "21", "150");
        stato[2].set("Junentus-Milan", "Calcio", "05/07/2014", "Torino", "45", "100");
        stato[3].set("Lady Gaga", "Concerto", "01/10/2015", "Verona", "23", "50");

        
        for(int i=0;i<N;i++){
            System.out.println(stato[i].toString());
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
	    			new ServerThread(clientSocket, stato).start();
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
class ServerThread extends Thread {

    private static final int N = 20;
    private Socket clientSocket = null;
    private TicketTwo[] stato = new TicketTwo[N];

    /**
     * Constructor
     * @param clientSocket
     */
    public ServerThread(Socket clientSocket, TicketTwo[] stato) {
        this.clientSocket = clientSocket;
        this.stato = stato;
    }

    public void run() {
        DataInputStream inSock;
        DataOutputStream outSock;
        String operazione, request;

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
                    //visualizzazione di tutti gli eventi di un certo tipo in un determinato luogo

                    System.out.println("[ServerThread] Arrivata richiesta di visualizzazione di tutti gli eventi di un certo tipo in un determinato luogo");

                    //leggo luogo e tipo separati da uno spazio
                    try {
                        request = inSock.readUTF();
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


                    String luogo, tipo, res;
                    StringTokenizer tok = new StringTokenizer(request, ";");
                    tipo = tok.nextToken();
                    luogo = tok.nextToken();

                    System.out.println("Richiesta di " + tipo + " a " + luogo);
                    
                    res = "";
                    for(int i=0; i<N; i++){
                        if(stato[i].getTipo().equals(tipo) && stato[i].getLuogo().equals(luogo)){
                            res += stato[i].toString();
                        }
                    }

                    System.out.println("[ServerThread] Invio:\n" + res);

                    outSock.writeUTF(res);

                }
                else{
                    //visualizzazione di tutti gli eventi disponibili e con prezzo inferiore o uguale a un prezzo dato

                    System.out.println("[ServerThread] Arrivata richiesta di visualizzazione di tutti gli eventi disponibili e con prezzo inferiore o uguale a un prezzo dato");

                    //leggo descrizione e prezzo massimo separati da uno spazio
                    try {
                        request = inSock.readUTF();
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

                    String descrizione, res;
                    int prezzoMax = 0;
                    StringTokenizer tok = new StringTokenizer(request, ";");
                    descrizione = tok.nextToken();
                    prezzoMax = Integer.parseInt(tok.nextToken());
                    
                    System.out.println("Richiesta per evento " + descrizione + " con un prezzo masssimo di " + prezzoMax);

                    res = "";
                    for(int i=0; i<N; i++){
                        if(stato[i].getDescrizione().equals(descrizione) &&
                            Integer.parseInt(stato[i].getPrezzo()) <= prezzoMax
                        ){
                            res += stato[i].toString();
                        }
                    }

                    System.out.println("[ServerThread] Invio:\n" + res);

                    outSock.writeUTF(res);
                }
            }
        }
        // qui catturo le eccezioni non catturate all'interno del while
        // in seguito alle quali il server termina l'esecuzione
        catch (Exception e) {
            e.printStackTrace();
            System.out
            .println("Errore irreversibile, PutFileServerThread: termino...");
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