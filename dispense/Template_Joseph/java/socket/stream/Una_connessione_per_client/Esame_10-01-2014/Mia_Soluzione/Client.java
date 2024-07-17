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
				System.out.println("Usage: java PutFileClient serverAddr serverPort");
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
        String tipo = null;
        String luogo = null;
        String descrizione = null;
        String prezzo = null;
        String request = null;

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
		    .print("Client Started.\n\n^D(Unix)/^Z(Win)+invio per uscire, oppure inserire:\n1) per la visualizzazione di tutti gli eventi di un certo tipo in un determinato luogo\n2) per la visualizzazione di tutti gli eventi disponibili e con prezzo inferiore o uguale a un prezzo dato\n");

		try{
			while ( (operazione=stdIn.readLine().trim()) != null){
							
				/* Invio operazione e attesa esito dal server */		

                if(operazione.equals("1")){
                    //visualizzazione di tutti gli eventi di un certo tipo in un determinato luogo

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
                        .print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure inserire:\n" + //
                                "1) per la visualizzazione di tutti gli eventi di un certo tipo in un determinato luogo\n" + //
                                "2) per la visualizzazione di tutti gli eventi disponibili e con prezzo inferiore o uguale a un prezzo dato\n" + //
                                "");
                        break;
                    }

                    //invio tipo e luogo
                    System.out.print("Inserire tipo di evento: ");
                    tipo = stdIn.readLine().trim();

                    System.out.print("Inserire luogo: ");
                    luogo = stdIn.readLine().trim();

                    //invio richiesta
                    request = tipo + ";" + luogo;
                    try{
                        outSock.writeUTF(request);
                    }
                    catch(Exception e){
                        System.out.println("Problemi nell'invio della richiesta"
                            + ": ");
                        e.printStackTrace();
                        System.out
                        .print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure inserire:\n" + //
                                "1) per la visualizzazione di tutti gli eventi di un certo tipo in un determinato luogo\n" + //
                                "2) per la visualizzazione di tutti gli eventi disponibili e con prezzo inferiore o uguale a un prezzo dato\n" + //
                                "");
                        break;
                    }


                    // ricezione esito
                    String esito;
                    try{
                        esito = inSock.readUTF();
                        System.out.println("Esito:\n" + esito);
                        
                    }
                    catch(SocketTimeoutException ste){
                        System.out.println("Timeout scattato: ");
                        ste.printStackTrace();
                        socket.close();
                        System.out
                            .print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti nome file: ");
                            break;          
                    }
                    catch(Exception e){
                        System.out
                            .println("Problemi nella ricezione dell'esito, i seguenti: ");
                        e.printStackTrace();
                        socket.close(); 
                        System.out
                            .print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti nome file: ");
                        break;
                    }

                }
                else if(operazione.equals("2")){
                    //visualizzazione di tutti gli eventi disponibili e con prezzo inferiore o uguale a un prezzo dato

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
                        .print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure inserire:\n" + //
                                "1) per la visualizzazione di tutti gli eventi di un certo tipo in un determinato luogo\n" + //
                                "2) per la visualizzazione di tutti gli eventi disponibili e con prezzo inferiore o uguale a un prezzo dato\n" + //
                                "");
                        break;
                    }

                    //invio descrizione e prezzo
                    System.out.print("Inserire descrizione dell'evento: ");
                    descrizione = stdIn.readLine().trim();

                    System.out.print("Inserire prezzo massimo consentito: ");
                    prezzo = stdIn.readLine().trim();

                    //invio richiesta
                    request = descrizione + ";" + prezzo;

                    System.out.println("Request: " + request);

                    try{
                        outSock.writeUTF(request);
                    }
                    catch(Exception e){
                        System.out.println("Problemi nell'invio della richiesta"
                            + ": ");
                        e.printStackTrace();
                        System.out
                        .print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure inserire:\n" + //
                                "1) per la visualizzazione di tutti gli eventi di un certo tipo in un determinato luogo\n" + //
                                "2) per la visualizzazione di tutti gli eventi disponibili e con prezzo inferiore o uguale a un prezzo dato\n" + //
                                "");
                        break;
                    }


                    // ricezione esito
                    String esito;
                    try{
                        esito = inSock.readUTF();
                        System.out.println("Esito: " + esito);
                        
                    }
                    catch(SocketTimeoutException ste){
                        System.out.println("Timeout scattato: ");
                        ste.printStackTrace();
                        socket.close();
                        System.out
                            .print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti nome file: ");
                        break;         
                    }
                    catch(Exception e){
                        System.out
                            .println("Problemi nella ricezione dell'esito, i seguenti: ");
                        e.printStackTrace();
                        socket.close();
                        System.out
                            .print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti nome file: ");
                        break;
                    }

                }
                else{
                    System.out.println("Operazione non valida");
                    System.out
				    .print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure inserire:\n" + //
				            "1) per la visualizzazione di tutti gli eventi di un certo tipo in un determinato luogo\n" + //
				            "2) per la visualizzazione di tutti gli eventi disponibili e con prezzo inferiore o uguale a un prezzo dato\n" + //
				            "");
                    continue;
                }

				// tutto ok, pronto per nuova richiesta
				System.out
				    .print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure inserire:\n" + //
				            "1) per la visualizzazione di tutti gli eventi di un certo tipo in un determinato luogo\n" + //
				            "2) per la visualizzazione di tutti gli eventi disponibili e con prezzo inferiore o uguale a un prezzo dato\n" + //
				            "");
				
			}
			socket.close();
			System.out.println("PutFileClient: termino...");
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
