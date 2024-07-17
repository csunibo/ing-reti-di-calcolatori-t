// Client.java

import java.net.*;
import java.io.*;

public class Client {

	public static void main(String[] args) throws IOException {
   
		InetAddress addr = null;
		int port = -1;
		
		try{ //check args
			if(args.length == 2){
				addr = InetAddress.getByName(args[0]);
				port = Integer.parseInt(args[1]);
                if (port < 1024 || port > 65535) {
	    			System.out.println("Usage: java LineServer [serverPort>1024]");
	    			System.exit(1);
	    		}
			} else{
				System.out.println("Usage: java Client serverAddr serverPort");
				System.exit(1);
			}
		} //try
		catch (UnknownHostException e) {
			System.out
		      .println("Problemi nella determinazione dell'endpoint dei server : ");
			e.printStackTrace();
			System.out.println("LineClient: interrompo...");
			System.exit(2);
		}
        catch (NumberFormatException ne){
            System.out.println("Formato porta/e non valido: ");
            ne.printStackTrace();
            System.out.println("LineClient: interrompo...");
			System.exit(2);
        }
		
		
		Socket socket = null;
		DataInputStream inSock = null;
		DataOutputStream outSock = null;
		String operazione = null;


		// creazione stream di input da tastiera
		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));

        // creazione socket
        try{
            socket = new Socket(addr, port);
            socket.setSoTimeout(100000);
            System.out.println("Creata la socket: " + socket);
        }
        catch(Exception e){
            System.out.println("Problemi nella creazione della socket: ");
            e.printStackTrace();
            System.exit(1);
        }

        // creazione stream di input/output su socket
        try{
            inSock = new DataInputStream(socket.getInputStream());
            outSock = new DataOutputStream(socket.getOutputStream());
        }
        catch(IOException e){
            System.out
                .println("Problemi nella creazione degli stream su socket: ");
            e.printStackTrace();
            System.exit(1);
        }

		System.out
		    .print("Client Started.\n\n^D(Unix)/^Z(Win)+invio per uscire, oppure inserire:\n1) per contare il numero delle linee che contengono almeno uno specificato numero di occorrenze di un carattere specificato all'interno di tutti i file di testo presenti sul direttorio remoto,\n2) per trasferire dal server al client tutti i file di binari di un direttorio\n");

		try{
			while ((operazione=stdIn.readLine().trim()) != null){
							
				/* Invio operazione e attesa esito dal server */		

                if(operazione.equals("1")){
                    //contare il numero delle linee che contengono almeno uno specificato numero di occorrenze

                    System.out.println("Selezionata operazione " +operazione+ " : invio al server");

                    // trasmissione dell'operazione
                    try{
                        outSock.writeUTF(operazione);
                    }
                    catch(Exception e){
                        System.out.println("Problemi nell'invio dell'operazione"
                            + ": ");
                        e.printStackTrace();
                        System.out
                        .print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure inserire:\n1) per contare il numero delle linee che contengono almeno uno specificato numero di occorrenze di un carattere specificato all'interno di tutti i file di testo presenti sul direttorio remoto,\n2) per trasferire dal server al client tutti i file di binari di un direttorio\n" + //
                                "");
                        break;
                    }

                }
                else if(operazione.equals("2")){
                    //OPERAZIONE 2

                    System.out.println("Selezionata operazione 2: invio al server");

                    // trasmissione dell'operazione
                    try{
                        outSock.writeUTF(operazione);
                    }
                    catch(Exception e){
                        System.out.println("Problemi nell'invio dell'operazione"
                            + ": ");
                        e.printStackTrace();
                        System.out
                        .print("\n\n^D(Unix)/^Z(Win)+invio per uscire, oppure inserire:\n1) per contare il numero delle linee che contengono almeno uno specificato numero di occorrenze di un carattere specificato all'interno di tutti i file di testo presenti sul direttorio remoto,\n2) per trasferire dal server al client tutti i file di binari di un direttorio\n" + //
                                "");
                        break;
                    }

                }
                else{
                    System.out.println("Operazione non valida");
                    System.out
				    .print("\n\n^D(Unix)/^Z(Win)+invio per uscire, oppure inserire:\n1) per contare il numero delle linee che contengono almeno uno specificato numero di occorrenze di un carattere specificato all'interno di tutti i file di testo presenti sul direttorio remoto,\n2) per trasferire dal server al client tutti i file di binari di un direttorio\n" + //
				            "");
                    continue;
                }

				// tutto ok, pronto per nuova richiesta
				System.out
				    .print("\n\n^D(Unix)/^Z(Win)+invio per uscire, oppure inserire:\n1) per contare il numero delle linee che contengono almeno uno specificato numero di occorrenze di un carattere specificato all'interno di tutti i file di testo presenti sul direttorio remoto,\n2) per trasferire dal server al client tutti i file di binari di un direttorio\n" + //
				            "");
				
			}
			socket.close();
			System.out.println("Client: termino...");
		}
		// qui catturo le eccezioni non catturate all'interno del while
		// quali per esempio la caduta della connessione con il server
		// in seguito alle quali il client termina l'esecuzione
		catch(Exception e){
			System.err.println("Errore irreversibile, il seguente: ");
			e.printStackTrace();
			System.err.println("Chiudo!");
			System.exit(3); 
	    }
	} // main
} // Client
