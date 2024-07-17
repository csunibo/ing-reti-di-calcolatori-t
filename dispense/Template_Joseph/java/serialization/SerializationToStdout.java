import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.io.Serializable;

class MyObject implements Serializable {
    private static final long serialVersionUID = 1L;
    private String data;

    public MyObject(String data) {
        this.data = data;
    }

    public String getData() {
        return data;
    }
}

public class SerializationToStdout {
    public static void main(String[] args) {
        MyObject objToSerialize = new MyObject("Hello, Serialization!");

        try {
            // Creazione di un ByteArrayOutputStream per catturare l'output serializzato
            ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();

            // Creazione di ObjectOutputStream per scrivere l'oggetto serializzato nel ByteArrayOutputStream
            ObjectOutputStream objectOutputStream = new ObjectOutputStream(byteArrayOutputStream);

            // Serializza l'oggetto nell'ObjectOutputStream
            objectOutputStream.writeObject(objToSerialize);

            // Chiude gli stream
            objectOutputStream.close();

            // Converti l'output serializzato in un array di byte
            byte[] serializedBytes = byteArrayOutputStream.toByteArray();

            // Stampa l'output serializzato sulla console
            System.out.println("Oggetto serializzato:");
            for (byte b : serializedBytes) {
                System.out.print((char)b);
            }
			System.out.println("");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
