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
        String dir = null;
        char car = ' ';
        String request = null;
        String occString = null;
        int occ;

        String nomeFileRicevuto = null;
        long numeroByte;
        File fileCorr;
        FileOutputStream outFileCorr;

        byte[] buffer = new byte[1];
        int cont = 0;
        int read_bytes = 0;
        DataOutputStream dest_stream = null;

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
			while ( (operazione=stdIn.readLine().trim()) != null){
							
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

                    //invio direttorio, carattere e numero di occorrenze
                    System.out.print("Inserire il nome del direttorio: ");
                    dir = stdIn.readLine().trim();

                    System.out.print("Inserire carattere: ");
                    car = (char)stdIn.read();
         
                    while (true){
                        try {
                            System.out.print("Inserire numero di occorrenze: ");
                            occString = stdIn.readLine().trim();
                            occ = Integer.parseInt(occString);
                        } catch (NumberFormatException e) {
                            System.out.println("Errore! Inserire un intero\n");
                            continue;
                        }
                        break;
                    }
                    
                    
                    //invio richiesta
                    request = dir + ";" + car + ";" + occ;
                    try{
                        outSock.writeUTF(request);
                    }
                    catch(Exception e){
                        System.out.println("Problemi nell'invio della richiesta"
                            + ": ");
                        e.printStackTrace();
                        System.out
                        .print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure inserire:\n1) per contare il numero delle linee che contengono almeno uno specificato numero di occorrenze di un carattere specificato all'interno di tutti i file di testo presenti sul direttorio remoto,\n2) per trasferire dal server al client tutti i file di binari di un direttorio\n" + //
                                "");
                        break;
                    }


                    // ricezione esito (-1 se dir non esiste)
                    String esito;
                    try{
                        esito = inSock.readUTF();
                        int nEsito = Integer.parseInt(esito);
                        if(nEsito == -1){
                            System.out.println("Direttorio inesistente sul server");
                        }
                        else System.out.println("Numero di occorrenze trovate: " + esito);
                        
                        
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
                    //trasferire dal server al client tutti i file di binari di un direttorio con almeno un alfabetico

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

                    //invio direttorio
                    System.out.print("Inserire direttorio: ");
                    dir = stdIn.readLine().trim();

                    System.out.println("Request: " + dir);

                    try{
                        outSock.writeUTF(dir);
                    }
                    catch(Exception e){
                        System.out.println("Problemi nell'invio della richiesta"
                            + ": ");
                        e.printStackTrace();
                        System.out
                        .print("\n\n^D(Unix)/^Z(Win)+invio per uscire, oppure inserire:\n1) per contare il numero delle linee che contengono almeno uno specificato numero di occorrenze di un carattere specificato all'interno di tutti i file di testo presenti sul direttorio remoto,\n2) per trasferire dal server al client tutti i file di binari di un direttorio\n" + //
                                "");
                        break;
                    }


                    // ricezione 
                    try {        
                        while ((nomeFileRicevuto = inSock.readUTF()) != null && nomeFileRicevuto.compareTo("fine") != 0) {
                            if(nomeFileRicevuto.compareTo("no") == 0 ){
                                System.out.println("Direttorio non presente sul server\n");
                                break;
                            }
                            
                            fileCorr = new File(nomeFileRicevuto);
                            if (fileCorr.exists()) {
                                outSock.writeUTF("salta file");
                            } else {
                                outSock.writeUTF("attiva");
                                numeroByte = inSock.readLong();
                                System.out.println("Scrivo il file " + nomeFileRicevuto + " di " + numeroByte + " byte");
                                outFileCorr = new FileOutputStream(nomeFileRicevuto);
        
                                // Ricevo il file (in linea)
                                dest_stream = new DataOutputStream(outFileCorr);
                                cont = 0;
                                try {
                                    // esco dal ciclo quando ho letto il numero di byte da trasferire
                                    while (cont < numeroByte) {
                                        read_bytes = inSock.read(buffer);
                                        dest_stream.write(buffer, 0, read_bytes);
                                        cont += read_bytes;
                                    }
                                    dest_stream.flush();
                                    System.out.println("Byte trasferiti: " + cont);
                                }
                                // l'eccezione dovrebbe scattare solo se ci aspettiamo un numero sbagliato di byte da leggere
                                catch (EOFException e) {
                                    System.out.println("Problemi, i seguenti: ");
                                    e.printStackTrace();
                                }
        
                                outFileCorr.close();
                            }
                        } // while
                    } catch (EOFException eof) {
                        System.out.println("Raggiunta la fine delle ricezioni, chiudo...");
                        socket.close();
                        System.out.println("Client: termino...");
                        System.exit(0);
                    } catch (SocketTimeoutException ste) {
                        System.out.println("Timeout scattato: ");
                        ste.printStackTrace();
                        socket.close();
                        System.exit(1);
                    } catch (Exception e) {
                        System.out.println("Problemi, i seguenti : ");
                        e.printStackTrace();
                        System.out.println("Chiudo ed esco...");
                        socket.close();
                        System.exit(2);
                    }

                    // scrivo solo in caso di successo
                    if(nomeFileRicevuto != null && nomeFileRicevuto.compareTo("no") != 0){
                        System.out.println("Trasferimento completato\n");
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
