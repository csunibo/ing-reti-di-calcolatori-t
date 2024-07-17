import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketTimeoutException;


public class TCP_Server {
    private final static int MAXEVENTS = 10;
    private static String[][] events = new String[MAXEVENTS][6];
    /**
    * @param args - Usage: java TCP_Server <port>
    */
    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Errore: Numero di parametri errato.\nUso: java TCP_Server <port>");
            System.exit(1);
        }
        int port = -1;
        try {
            port = Integer.parseInt(args[0]);
            if (port < 1024 || port > 65535) {
            throw new IllegalArgumentException("Port is outside the range of valid ports. [1024-65535]");
            }
        } catch (IllegalArgumentException e) {
            System.out.println("Errore: Parametro errato.\nUso: java TCP_Server <port>");
            e.printStackTrace();
            System.exit(1);
        }

        // Inizializzazione struttura dati
        for (int i = 0; i < MAXEVENTS; i++) {
            for (int j = 0; j < 6; j++) {
                events[i][j] = "L";
            }
        }

        events[1][0] = "String";
        events[1][1] = "Concerto";
        events[1][2] = "11/01/2014";
        events[1][3] = "Verona";
        events[1][4] = "40";
        events[1][5] = "40";

        events[2][0] = "Junentus-Inger";
        events[2][1] = "Calcio";
        events[2][2] = "03/05/2014";
        events[2][3] = "Torino";
        events[2][4] = "21";
        events[2][5] = "150";


        events[4][0] = "GP Bologna";
        events[4][1] = "Formula 1";
        events[4][2] = "07/09/2014";
        events[4][3] = "Bologna";
        events[4][4] = "10";
        events[4][5] = "200";
        System.out.println("\tDescrizione\tTipo\tData\tLuogo\tDisp\tPrezzo");

        for (int i = 0; i < MAXEVENTS; i++) {
            debugPrint(i, events[i]);
        }

        ServerSocket serverSocket = null;

        try {
            serverSocket = new ServerSocket(port);
            serverSocket.setReuseAddress(true);
            System.out.println("Info: Server in ascolto sulla porta " +
            serverSocket.getLocalPort());
        } catch (IOException e) {
            System.out.println("Errore: Creazione socket fallita.");
            e.printStackTrace();
            System.exit(1);
        }
        try {
            while (true) {
                System.out.println("Info: Server in attesa di un client...");

                Socket clientSocket = null;

                try {
                    clientSocket = serverSocket.accept();
                    System.out.println("Info: Connessione accettata " + clientSocket);
                } catch (IOException e) {
                    System.out.println("Errore: Il server non puo' accettare la connessione.");
                    e.printStackTrace();
                    continue;
                }

                new TCP_Server_Thread(clientSocket, events).start();
            }
        } catch (Exception e) {
            System.out.println("Errore: Errore irreversibile del server.");
            e.printStackTrace();
        } finally {
            System.out.println("Info: Termino...");
        }
    }


    protected static void debugPrint(int index, String[] event) {
        System.out.print("[" + index + "]");

        for (String param : event) {
            System.out.print("\t"+param);
        }

        System.out.println();

    }
}
    class TCP_Server_Thread extends Thread {
        private Socket socket = null;
        private String[][] events;

        //costruttore
        public TCP_Server_Thread(Socket socket, String[][] events) {
            this.socket = socket;
            this.events = events;
        }

        public void run() {
            System.out.println("Info: Attivazione server thread. (Thread=" + getName() + ")");

            DataInputStream inputStream = null;
            DataOutputStream outputStream = null;

            try {
                inputStream = new DataInputStream(socket.getInputStream());
                outputStream = new DataOutputStream(socket.getOutputStream());
            } catch (IOException e) {
                System.out.println("Errore: Creazione stream fallita. (Thread=" +
                getName() + ")");
                e.printStackTrace();
                return;
            }

            try {
                while (true) {
                    int action = inputStream.readInt();

                    // eventi luogo
                    if (action == 0) {
                        String type = inputStream.readUTF();
                        String location = inputStream.readUTF();
                        int nEvents = 0;

                        for (String[] event : events) {
                            if (event[1].equalsIgnoreCase(type) && event[3].equalsIgnoreCase(location)) {
                                nEvents++;
                            }
                        }

                        int count = 0;
                        String[][] filteredEvents = new String[nEvents][6];

                        for (String[] event : events) {
                            if (event[1].equalsIgnoreCase(type) && event[3].equalsIgnoreCase(location)) {
                                filteredEvents[count] = event;
                                count++;
                            }
                        }
                
                        outputStream.writeInt(nEvents);

                        for (String[] event : filteredEvents) {
                            for (String param : event) {
                                outputStream.writeUTF(param);
                            }
                        }
                    } //eventi prezzo
                    else if (action == 1) {
                        int prezzo = inputStream.readInt();
                        System.out.println("Ricevuta richiesta per ricerca eventi con prezzo inferiore a: " + prezzo);
                        int nEvents = 0;

                        for (String[] event : events) {
                            if (!event[5].equalsIgnoreCase("L") && Integer.parseInt(event[5]) <= prezzo) {
                                nEvents++;
                            }
                        
                        }
                        int count = 0;
                        String[][] filteredEvents = new String[nEvents][6];

                        for (String[] event : events) {
                            if (!event[5].equalsIgnoreCase("L") && Integer.parseInt(event[5]) <= prezzo) {
                                filteredEvents[count] = event;
                                count++;
                            }
                        }
                        outputStream.writeInt(nEvents);

                        for (String[] event : filteredEvents) {
                            for (String param : event) {
                                outputStream.writeUTF(param);
                            }
                        }
                    }
                }
                
            } catch (EOFException e) {
                System.out.println("Terminazione. (Thread=" + getName() + ")");
            } catch (SocketTimeoutException e) {
                System.out.println("Errore: Socket timed out. (Thread=" + getName
                () + ")");
                e.printStackTrace();
            } catch (IOException e) {
                System.out.println("Errore: Lettura/Scrittura su stream fallita.(Thread=" + getName() + ")");
                e.printStackTrace();
            } catch (Exception e) {
                System.out.println("Errore: Errore irreversibile. (Thread=" +
                getName() + ")");
                e.printStackTrace();
            } finally {
                System.out.println("Info: Terminazione server thread. (Thread=" +
                getName() + ")");

                try {
                    socket.close();
                } catch (IOException e) {
                    System.out.println("Errore: Chiusura socket fallita. (Thread="
                    + getName() + ")");
                    e.printStackTrace();
                }
            }
        }
    }
