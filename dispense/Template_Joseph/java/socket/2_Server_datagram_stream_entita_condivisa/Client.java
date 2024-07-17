import java.io.*;
import java.net.*;

public class Client {

    private static int N = 20;
    
    //INVOCAZIONE: java Client hostServerStream portServerStream hostServerDatagram portServerDatagram

    public static void main(String[] args) {
        InetAddress datagramAddr = null;
        InetAddress streamAddr = null;
		int datagramPort = -1;
        int streamPort = -1;

        try {
			if (args.length == 4) {
                streamAddr = InetAddress.getByName(args[0]);
                streamPort = Integer.parseInt(args[1]);

				if(streamPort<1024 || streamPort>6553){
					System.out.println("Usage: java Client hostServerStream [1024<=portServerStream<=6553] hostServerDatagram [1024<=portServerDatagram<=6553]");
					System.exit(1);
				}
				
		        datagramAddr = InetAddress.getByName(args[2]);
		        datagramPort = Integer.parseInt(args[3]);

				if(datagramPort<1024 || datagramPort>6553){
					System.out.println("Usage: java Client hostServerStream [1024<=portServerStream<=6553] hostServerDatagram [1024<=portServerDatagram<=6553]");
					System.exit(1);
				}

			} else {
				System.out.println("Usage: java Client hostServerStream portServerStream hostServerDatagram portServerDatagram");
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

        //socket stream
        Socket streamSocket = null;

        BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
        char operazione = 0;
		System.out.print("\n^D(Unix)+invio per uscire, altrimenti inserisci operazione desiderata\n[V] per la visualizzazione di tutte le stanze  [S] per la sospendere una stanza: ");

        try {
            String richiesta = null, entry = null;
			ByteArrayOutputStream boStream = null;
			DataOutputStream doStream = null;
			byte[] data = new byte[256];
			int res;
			ByteArrayInputStream biStream = null;
			DataInputStream diStream = null;
            

            while((operazione = (char) stdIn.read()) > 0){
				stdIn.read();//consumo l'unvio
                if(operazione == 'V'){

                    //creazione socket stream
                    try{
						streamSocket = new Socket(streamAddr, streamPort);
						streamSocket.setSoTimeout(30000);
						System.out.println("Creata la socket: " + streamSocket);
					}
					catch(Exception e){
						System.out.println("Problemi nella creazione della socket: ");
						e.printStackTrace();
						System.out
							.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti nome file: ");
						continue;
						// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					}

                    // creazione stream di input su socket
					try{
						diStream = new DataInputStream(streamSocket.getInputStream());
					}
					catch(IOException e){
						System.out
							.println("Problemi nella creazione dello stream di input su socket: ");
						e.printStackTrace();
						streamSocket.close();
						System.out
							.print("\n^D(Unix)/^Z(Win)+invio per uscire, oppure immetti nome file: ");
						continue;
						// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					}

                    //leggo lo stato delle stanze
                    try {
						System.out.println("Stato delle stanze:");
                        for(int i=0;i<N;i++){
							entry = diStream.readUTF();
							System.out.println(entry);
						}
						System.out.println();
                    } catch (Exception e) {
                        System.out.println("Errore nel leggere le stanza in input: ");
                        e.printStackTrace();
						streamSocket.close();
                    }

					streamSocket.close();

					System.out.print("\n^D(Unix)+invio per uscire, altrimenti inserisci operazione desiderata\n[V] per la visualizzazione di tutte le stanze  [S] per la sospendere una stanza: ");
                } //V
                else if(operazione == 'S'){
					
					System.out.print("Nome della stanza da sospendere? ");
					richiesta = stdIn.readLine();
					

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
						System.out
						.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti inserisci nome file (con estensione): ");
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
						System.out
							.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti inserisci nome file (con estensione): ");
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
						System.out
						  .print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti inserisci nome file (con estensione): ");
						continue;
						// il client continua l'esecuzione riprendendo dall'inizio del ciclo
					}


					System.out.print("\n^D(Unix)+invio per uscire, altrimenti inserisci operazione desiderata\n[V] per la visualizzazione di tutte le stanze  [S] per la sospendere una stanza: ");
                } //S
                else{
                    System.out.println("Argomento sbagliato, riprovare");
					System.out.print("\n^D(Unix)+invio per uscire, altrimenti inserisci operazione desiderata\n[V] per la visualizzazione di tutte le stanze  [S] per la sospendere una stanza: ");
                }
            } //while operazione

			streamSocket.close();
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
