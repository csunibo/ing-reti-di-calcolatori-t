import java.io.*;
import java.net.*;

public class Client {

    //private static int N = 20;
    
    //INVOCAZIONE: java Client hostServerDatagram portServerDatagram

    public static void main(String[] args) {
        InetAddress datagramAddr = null;
		int datagramPort = -1;

        try {
			if (args.length == 2) {
		        datagramAddr = InetAddress.getByName(args[0]);
		        datagramPort = Integer.parseInt(args[1]);

				if(datagramPort<1024 || datagramPort>6553){
					System.out.println("Usage: java Client hostServerDatagram [1024<=portServerDatagram<=6553]");
					System.exit(1);
				}
			} else {
				System.out.println("Usage: java Client hostServerDatagram portServerDatagram");
			    System.exit(1);
			}
		} catch (UnknownHostException e) {
			System.out
		      .println("Problemi nella determinazione dell'endpoint dei server : ");
			e.printStackTrace();
			System.out.println("Client: interrompo...");
			System.exit(2);
		}
        catch (NumberFormatException ne){
            System.out.println("Formato porta/e non valido: ");
            ne.printStackTrace();
            System.out.println("Client: interrompo...");
			System.exit(2);
        }

        // creazione della socket datagram, settaggio timeout di 30s
		// e creazione datagram packet
        DatagramSocket datagramSocket = null;
		DatagramPacket datagramPacket = null;
		byte[] buf = new byte[256];

        try {
			datagramSocket = new DatagramSocket();
			datagramSocket.setSoTimeout(30000);
			datagramPacket = new DatagramPacket(buf, buf.length, datagramAddr, datagramPort);
			System.out.println("\nClient: avviato");
			System.out.println("Creata la socket: " + datagramSocket);
		} catch (SocketException e) {
			System.out.println("Problemi nella creazione della socket: ");
			e.printStackTrace();
			System.out.println("Client: interrompo...");
			System.exit(1);
		}


        BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
		System.out.print("\n^D(Unix)+invio per uscire, altrimenti inserisci nome della stanza da sospendere: ");
		//questo client si occupa solo della sospensione della stanza

        try {
            String richiesta = null;
			ByteArrayOutputStream boStream = null;
			DataOutputStream doStream = null;
			byte[] data = new byte[256];
			int res;
			ByteArrayInputStream biStream = null;
			DataInputStream diStream = null;
            

            while((richiesta = stdIn.readLine() ) != null){
					// riempimento e invio del pacchetto
					try {
						boStream = new ByteArrayOutputStream();
						doStream = new DataOutputStream(boStream);
						doStream.writeUTF(richiesta);
						data = boStream.toByteArray();
						datagramPacket.setData(data);
						datagramSocket.send(datagramPacket);
						System.out.println("Richiesta " + richiesta + " inviata a " + datagramAddr + ", " + datagramPort);
					} catch (IOException e) {
						System.out.println("Problemi nell'invio della richiesta: ");
						e.printStackTrace();
						System.out.print("\n^D(Unix)+invio per uscire, altrimenti inserisci nome della stanza da sospendere: ");
						continue;
						// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					}

					try {
						// settaggio del buffer di ricezione
						datagramPacket.setData(buf);
						datagramSocket.receive(datagramPacket);
						// sospensiva solo per i millisecondi indicati, dopodiché solleva una
						// SocketException
					} catch (IOException e) {
						System.out.println("Problemi nella ricezione del datagramma: ");
						e.printStackTrace();
						System.out.print("\n^D(Unix)+invio per uscire, altrimenti inserisci nome della stanza da sospendere: ");
						continue;
						// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					}

					try {
						biStream = new ByteArrayInputStream(datagramPacket.getData(), 0, datagramPacket.getLength());
						diStream = new DataInputStream(biStream);
						res = diStream.readInt();
						System.out.println("[Debug] Risposta: " + res);

						if(res < 0){
							System.out.println("Errore: nome della stanza non esistente o già sospesa");
						}
						else{
							System.out.println("Sospensione della stanza andata a buon fine");
						}
					} catch (IOException e) {
						System.out.println("Problemi nella lettura della risposta: ");
						e.printStackTrace();
						System.out.print("\n^D(Unix)+invio per uscire, altrimenti inserisci nome della stanza da sospendere: ");
						continue;
						// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					}


					System.out.print("\n^D(Unix)+invio per uscire, altrimenti inserisci nome della stanza da sospendere: ");
                
            } //while operazione

			datagramSocket.close();
			System.out.println("Client: termino...");

        } catch (Exception e) {
            System.err.println("Errore irreversibile, il seguente: ");
			e.printStackTrace();
			System.err.println("Chiudo!");
			System.exit(3); 
        }

    }//main
}
