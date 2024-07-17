import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;

public class TCP_Client {
    /**
* @param args - Usage: java TCP_Client <address> <port>
*/
public static void main(String[] args) {
    if (args.length != 2) {
        System.out.println("Errore: Numero di parametri errato.\nUso: java TCP_Client <address> <port>");
        System.exit(1);
    }

    InetAddress address = null;
    int port = -1;
    try {
        address = InetAddress.getByName(args[0]);
        port = Integer.parseInt(args[1]);

        if (port < 1024 || port > 65535) {
            throw new IllegalArgumentException("Port is outside the range of valid ports. [1024-65535]");
        }
    } catch (UnknownHostException e) {
        System.out.println("Errore: Parsing dei parametri fallito.\nUso: java TCP_Client <address> <port>");
        e.printStackTrace();
        System.exit(1);
    } catch (IllegalArgumentException e) {
        System.out.println("Errore: Parametro errato.\nUso: java TCP_Client <address> <port>");
        e.printStackTrace();
        System.exit(1);
    }

    Socket socket = null;
    DataInputStream inputStream = null;
    DataOutputStream outputStream = null;

    try {
        socket = new Socket(address, port);
        inputStream = new DataInputStream(socket.getInputStream());
        outputStream = new DataOutputStream(socket.getOutputStream());
    } catch (IOException e) {
        System.out.println("Errore: Creazione socket fallita.");
        e.printStackTrace();
        System.exit(1);
    }

    BufferedReader stdInput = new BufferedReader(new InputStreamReader
    (System.in));
    String input = null;
    System.out.println("Per cosa vuoi filtrare gli eventi?\n- Luogo\n- Prezzo\n");

    try {
        while ((input = stdInput.readLine()) != null) {
            if (input.equalsIgnoreCase("luogo")) {
                System.out.print("Inserisci il tipo di evento: ");
                String type = stdInput.readLine();
                System.out.print("Inserisci il luogo: ");
                String location = stdInput.readLine();

                outputStream.writeInt(0);
                outputStream.writeUTF(type);
                outputStream.writeUTF(location);

                int nEvents = inputStream.readInt();
                String[][] events = new String[nEvents][6];
                for (int i = 0; i < nEvents; i++) {
                    for (int j = 0; j < 6; j++) {
                        events[i][j] = inputStream.readUTF();
                    }
                }

                if (nEvents == 0) {
                    System.out.println("Nessun evento soddisfa i criteri di ricerca.");
                } else {
                    System.out.println(nEvents + " eventi trovati:");
                    System.out.println("\tDescrizione\tTipo\tData\tLuogo\tDisp\tPrezzo");

                    for (int i = 0; i < nEvents; i++) {
                        debugPrint(i, events[i]);
                    }
                    System.out.println("\n");
                }
        }
        else if (input.equalsIgnoreCase("prezzo")) {
            System.out.print("Inserisci il prezzo massimo per biglietto: ");
            int prezzo = 0;

            try{
                prezzo = Integer.parseInt(stdInput.readLine());
            }catch(NumberFormatException e){
                System.out.println("Valore inserito non intero.");
                System.out.println("Per cosa vuoi filtrare gli eventi?\n- Luogo\n- Prezzo\n");
                continue;
            }
            outputStream.writeInt(1);
            outputStream.writeInt(prezzo);

            int nEvents = inputStream.readInt();
            String[][] events = new String[nEvents][6];

            for (int i = 0; i < nEvents; i++) {
                for (int j = 0; j < 6; j++) {
                    events[i][j] = inputStream.readUTF();
                }
            }

            if (nEvents == 0) {
                System.out.println("Nessun evento soddisfa i criteri di ricerca.");
            } else {
                System.out.println(nEvents + " eventi trovati:");
                System.out.println("\tDescrizione\tTipo\tData\tLuogo\tDisp\tPrezzo");

                for (int i = 0; i < nEvents; i++) {
                    debugPrint(i, events[i]);
                }
                System.out.println();
            }
        }
        
        System.out.println("Per cosa vuoi filtrare gli eventi?\n-Luogo\n- Prezzo\n");
        }
    } catch (SocketException e) {
        System.out.println("Errore: Accesso socket fallito.");
        e.printStackTrace();
    } catch (IOException e) {
        System.out.println("Errore: Lettura da stream fallita.");
        e.printStackTrace();
    } finally {
        System.out.println("Info: Termino socket.");
        try {
        socket.shutdownOutput();
        socket.shutdownInput();
        } catch (IOException e) {
            System.out.println("Errore: Chiusura socket fallita.");
            e.printStackTrace();
        }
        System.out.println("Info: Termino...");
    }
}

    private static void debugPrint(int index, String[] event) {
        System.out.print("[" + index + "]");

        for (String param : event) {
            System.out.print("\t"+param);
        }
        
        System.out.println();
    }
}
    

