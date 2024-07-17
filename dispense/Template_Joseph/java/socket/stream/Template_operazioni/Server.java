// Server Concorrente

import java.io.*;
import java.net.*;




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
        String operazione;

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
                    //OPERAZIONE 1
                }
                else{
                    //OPERAZIONE 2
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

