//Server che si occupa della visualizzazione delle stanze

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;



class Server_streamThread extends Thread{

    private Socket clientSocket = null;
    private static final int N = 20; //numero massimo di stanza
    private static final int K = 10; //numero massimo di utenti per stanza

    /**
	 * Constructor
	 * @param clientSocket
	 */
	public Server_streamThread(Socket clientSocket) {
		this.clientSocket = clientSocket;
	}

    public void run(){
        File stanzeFile = null;
        BufferedReader br = null;
        DataOutputStream outputStream = null;

        System.out.println("Server_streamThread avviato...");

        try {
            stanzeFile = new File("stanze.txt");
            br = new BufferedReader(new InputStreamReader(new FileInputStream(stanzeFile)));
        } catch (FileNotFoundException fnf) {
            System.out.println("Errore! File stanze.txt non trovato: ");
            fnf.printStackTrace();
            try {
                clientSocket.close();
            } catch (IOException e) {
                System.out.println("Errore nel chiudere la socket: ");
                e.printStackTrace();
            }
            System.exit(1);
        }

        // creazione stream di ouput su socket
		try{
			outputStream = new DataOutputStream(clientSocket.getOutputStream());
		}
		catch(IOException e){
			System.out.println("Problemi nella creazione dello stream di output su socket: ");
			e.printStackTrace();
            return;
		}
        
        System.out.println("Server: leggo il file stanze.txt");

        //leggo il file
        for(int i=0;i<N;i++){
            String line;
            try {
                line = br.readLine();
            } catch (IOException e) {
                System.out.println("Errore di IO sul file");
                try {
                    clientSocket.close();
                    br.close();
                } catch (IOException e1) {
                    System.out.println("Errore nel chiudere la socket: ");
                    e1.printStackTrace();
                }
                return;
            }    

            System.out.println("[Debug] Linea da scrivere:\n" + line);
            
            //scrivo le informazioni della stanza sulla socket
            try {
                outputStream.writeUTF(line);
            } catch (IOException e) {
                System.out.println("Errore! Impossibile scrivere lo stato delle stanze sullo stream di output");
                try {
                    clientSocket.close();
                    br.close();
                } catch (IOException e1) {
                    System.out.println("Errore nel chiudere la socket: ");
                    e1.printStackTrace();
                }
                return;
            }
            
        }

        
        System.out.println("Server_streamThread termino...");
        try {
            clientSocket.close();
            br.close();
        } catch (IOException e) {
            System.out.println("Errore nel chiudere la socket: ");
            e.printStackTrace();
        }


    } //run


}// Server_streamThread class




public class Server_streamCon{

    public static final int PORT = 1052; //default port
    
        public static void main(String[] args) throws IOException {

            int port = -1;

            /* controllo argomenti */
            try {
                if (args.length == 1) {
                    port = Integer.parseInt(args[0]);
                    if (port < 1024 || port > 65535) {
                        System.out.println("Usage: java Server_stream [serverPort>1024]");
                        System.exit(1);
                    }
                } else if (args.length == 0) {
                    port = PORT;
                } else {
                    System.out
                        .println("Usage: java Server_stream or java Server_stream port");
                    System.exit(1);
                }
            } //try
            catch (Exception e) {
                System.out.println("Problemi, i seguenti: ");
                e.printStackTrace();
                System.out
                    .println("Usage: java Server_stream or java Server_stream port");
                System.exit(1);
            }

            ServerSocket serverSocket = null;
            Socket clientSocket = null;

            try {
                serverSocket = new ServerSocket(port);
                serverSocket.setReuseAddress(true);
                System.out.println("Server_stream: avviato ");
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
    
                    try {
                        // bloccante fino ad una pervenuta connessione
                        clientSocket = serverSocket.accept();
                        clientSocket.setSoTimeout(30000);
                        System.out.println("Server: connessione accettata: " + clientSocket);
                    }
                    catch (Exception e) {
                        System.err
                            .println("Server: problemi nella accettazione della connessione: "
                                    + e.getMessage());
                        e.printStackTrace();
                        continue;
                    }
    
                    // serizio delegato ad un nuovo thread
                    try {
                        new Server_streamThread(clientSocket).start();
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
                System.out.println("PutFileServerCon: termino...");
                System.exit(2);
            }

        }//main


} //Server_stream class
