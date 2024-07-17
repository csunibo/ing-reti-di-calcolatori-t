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
        String dir = null;
        String nomeFileRicevuto = null;

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

		System.out.print("Client Started\n");

        System.out.print("Inserire nome direttorio: ");

		try{
			while ((dir=stdIn.readLine().trim()) != null){
                //invio direttorio

                System.out.println("Invio il nome del direttorio " + dir);

                try{
                    outSock.writeUTF(dir);
                }
                catch(Exception e){
                    System.out.println("Problemi nell'invio della richiesta"
                        + ": ");
                    e.printStackTrace();
                    System.out
                    .print("\n\n" + //
                            "");
                    break;
                }


                // ricezione 
                try {
                    long numeroByte;
                    File fileCorr;
                    FileOutputStream outFileCorr;
    
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

                    } // while file
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

                System.out.print("Inserire nome direttorio: ");
			} //while true
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
