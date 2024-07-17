package source.scratch.java.files;

import java.io.File;

public class FileSize {
    public static void main(String[] args) {
        // Specifica il percorso del file
        String filePath = args[0]; // Sostituisci con il percorso reale del tuo file

        // Crea un oggetto File
        File file = new File(filePath);

        // Verifica se il file esiste
        if (file.exists()) {
            // Ottieni la dimensione del file in byte
            long fileSize = file.length();

            System.out.println("La dimensione del file è " + fileSize + " byte");
        } else {
            System.out.println("Il file non esiste.");
        }
    }
}