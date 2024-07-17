import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

public class Server_datagram {

    private static final int PORT = 1051;
    private static final int N = 20; //numero massimo di stanza
    private static final int K = 10; //numero massimo di utenti per stanza

    public static void main(String[] args) {

		System.out.println("Server_datagram: avviato");

		DatagramSocket socket = null;
		DatagramPacket packet = null;
		File stanzeFile = null;
		FileWriter initWriter = null;
		byte[] buf = new byte[256];
		int port = -1;

		// controllo argomenti input: 0 oppure 1 argomento (porta)
		if ((args.length == 0)) {
			port = PORT;
		} else if (args.length == 1) {
			try {
				port = Integer.parseInt(args[0]);
				// controllo che la porta sia nel range consentito 1024-65535
				if (port < 1024 || port > 65535) {
					System.out.println("Usage: java Server_datagram [serverPort>1024]");
					System.exit(1);
				}
			} catch (NumberFormatException e) {
				System.out.println("Usage: java Server_datagram [serverPort>1024]");
				System.exit(1);
			}
		} else {
			System.out.println("Usage: java Server_datagram [serverPort>1024]");
			System.exit(1);
		}

		try {
			socket = new DatagramSocket(port);
			packet = new DatagramPacket(buf, buf.length);
			System.out.println("Creata la socket: " + socket);
		}
		catch (SocketException e) {
			System.out.println("Problemi nella creazione della socket: ");
			e.printStackTrace();
			System.exit(1);
		}

		//inizializzazione del file
		//apro il file per controllare se esiste
		stanzeFile = new File("stanze.txt");
		if(!stanzeFile.exists()){
			//allora inizializzo il file
			try {
				if(!stanzeFile.createNewFile()){
					System.out.println("Impossibile creare il file");
					System.exit(2);
				}
				initWriter = new FileWriter(stanzeFile);
			} catch (FileNotFoundException fnf) {
				System.out.println("Errore! File stanze.txt non trovato: ");
				fnf.printStackTrace();
			}
			catch (IOException ioe){
				System.out.println("Problemi nella creazione del file");
				ioe.printStackTrace();
				System.exit(1);
			}

			try{
				initWriter.write("Nome: Park \tStato: P \tUtenti: Maria Anna L L L L L L L L\n");
			} catch(IOException e){
				System.out.println("Errore inizializzazione: impossibile scrivere la riga 1");
				e.printStackTrace();
			}

			try{
				initWriter.write("Nome: Auto \tStato: M \tUtenti: Marco Gino Antonio L L L L L L L\n");
			} catch(IOException e){
				System.out.println("Errore inizializzazione: impossibile scrivere la riga 2");
				e.printStackTrace();
			}

			//scrivo le restanti vuote
			for(int i=2; i<N; i++){
				try{
					initWriter.write("Nome: L \tStato: L \tUtenti: L L L L L L L L L L\n");
				} catch(IOException e){
					System.out.println("Errore inizializzazione: impossibile scrivere la riga " + i+1);
					e.printStackTrace();
				}
				
			}

			try {
				initWriter.close();
			} catch (IOException e) {
				System.out.println("Errore chiusura initWriter: ");
				e.printStackTrace();
			}
		}

		try {
			String richiesta = null;
			ByteArrayInputStream biStream = null;
			DataInputStream diStream = null;

			while (true) {
				System.out.println("\nIn attesa di richieste...");
				
				// ricezione del datagramma
				try {
					packet.setData(buf);
					socket.receive(packet);
				}
				catch (IOException e) {
					System.err.println("Problemi nella ricezione del datagramma: "
							+ e.getMessage());
					e.printStackTrace();
					continue;
					// il server continua a fornire il servizio ricominciando dall'inizio
					// del ciclo
				}

				try {
					biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
					diStream = new DataInputStream(biStream);
					richiesta = diStream.readUTF();
				}
				catch (Exception e) {
					System.err.println("Problemi nella lettura della richiesta: "
						+ richiesta);
					e.printStackTrace();
					continue;
					// il server continua a fornire il servizio ricominciando dall'inizio
					// del ciclo
				}

				System.out.println("Ricevuta richiesta di sospensione della stanza " + richiesta + ", delego il figlio");

				//una volta ricevuta la richiesta lancio il Thread
				new Server_datagramThread(socket, richiesta).start();
				

			} // while

		}
		// qui catturo le eccezioni non catturate all'interno del while
		// in seguito alle quali il server termina l'esecuzione
		catch (Exception e) {
			e.printStackTrace();
		}
		System.out.println("Server: termino...");
		socket.close();
	}
    
}


class Server_datagramThread extends Thread{
	private static final int N = 20; //numero massimo di stanza
    private static final int K = 10; //numero massimo di utenti per stanza

	private DatagramSocket socket = null;
	private String richiesta = null;

	public Server_datagramThread(DatagramSocket socket, String richiesta){
		this.socket = socket;
		this.richiesta = richiesta;
	}

	public void run() {
		DatagramPacket packet = null;
		File stanzeFile = null;
		BufferedReader br = null;
		ByteArrayOutputStream boStream = null;
		DataOutputStream doStream = null;
		byte[] data = null;
		byte[] buf = new byte[256];
        int res = -1;

		System.out.println("Info: Attivazione server thread. (Thread=" + getName() + ")");
	
		packet = new DatagramPacket(buf, buf.length);

		//apro il file 
		try {
			stanzeFile = new File("stanze.txt");
			br = new BufferedReader(new InputStreamReader(new FileInputStream(stanzeFile))); 
		} catch (FileNotFoundException fnf) {
			System.out.println("Errore! File stanze.txt non trovato: ");
			fnf.printStackTrace();
		}

		

		//leggo il file
		for(int i=0;i<N;i++){
			String line;
			try {
				line = br.readLine();
			} catch (IOException e) {
				System.out.println("Errore di IO sul file");
				try {
					socket.close();
					br.close();
				} catch (IOException e1) {
					System.out.println("Errore nel chiudere la socket: ");
					e1.printStackTrace();
				}
				return;
			}    

			if(	line.split(" ")[1].equals(richiesta) && 
				line.split("Stato: ")[1].charAt(0) != 'S'
				){
				System.out.println("La struttura contiene la stanza richiesta");
				res = 0;
			}					
		}

		try {
			br = new BufferedReader(new InputStreamReader(new FileInputStream(stanzeFile)));
		} catch (FileNotFoundException e) {
			System.out.println("Errore: file stanze.txt non trovato");
			e.printStackTrace();
		} 

		//aggiorno il file
		if(res == 0){ 
			try {
				File temp = new File("temp.txt");
				FileWriter writer = null;

				writer = new FileWriter(temp);
				
				for(int i =0; i<N; i++){
					String line;
					try {
						line = br.readLine();
					} catch (IOException e) {
						System.out.println("Errore di IO sul file");
						try {
							socket.close();
							br.close();
							writer.close();
						} catch (IOException e1) {
							System.out.println("Errore nel chiudere le risorse: ");
							e1.printStackTrace();
						}
						return;
					} 

					if(line.split(" ")[1].equals(richiesta)){
						String[] lineSplitted = line.split("Stato: ");
						line = lineSplitted[0] + "Stato: S" + lineSplitted[1];
					}
					
					writer.write(line + "\n");
				}

				stanzeFile.delete();
				writer.close();
				temp.renameTo(new File("stanze.txt"));
				
			} catch (IOException e) {
				System.out.println("Erroe writer: ");
				e.printStackTrace();
			}
		}

		//invio della risposta
		//lancia null pointer exception RIVEDI
		try {
			boStream = new ByteArrayOutputStream();
			doStream = new DataOutputStream(boStream);
			doStream.writeInt(res);
			data = boStream.toByteArray();
			System.out.println("Data: ");
			for(int i=0;i<data.length;i++){
				System.out.print(data[i]);
			}
			System.out.println();
			packet.setData(data, 0, data.length);
			socket.send(packet);
		}
		catch (Exception e) {
			System.err.println("Problemi nell'invio della risposta: "
			  + e.getMessage());
			e.printStackTrace();
		}

		try {
			br.close();
		} catch (IOException e) {
			e.printStackTrace();
		}

		System.out.println("Thread " + getName() +" termino...");
	}
}