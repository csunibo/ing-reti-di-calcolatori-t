package source.scratch.java.eventloop;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.AsynchronousServerSocketChannel;
import java.nio.channels.AsynchronousSocketChannel;
import java.nio.channels.CompletionHandler;
import java.util.concurrent.Future;

public class Main {
    public static void main(String[] args) throws IOException {
        AsynchronousServerSocketChannel server = AsynchronousServerSocketChannel.open();
        InetSocketAddress address = new InetSocketAddress("localhost", 5000);
        server.bind(address);

        System.out.println("Server is listening on " + address);

        CompletionHandler<AsynchronousSocketChannel, Void> handler = new CompletionHandler<>() {
            @Override
            public void completed(AsynchronousSocketChannel client, Void attachment) {
                // Accetta nuove connessioni
                server.accept(null, this);

                ByteBuffer buffer = ByteBuffer.allocate(1024);
                try {
                    Future<Integer> readResult = client.read(buffer);
                    readResult.get();

                    buffer.flip();
                    String message = new String(buffer.array()).trim();
                    System.out.println("Received message: " + message);

                    buffer.clear();

                    if (!message.isEmpty()) {
                        // Rispondi al client
                        String response = "Server received: " + message;
                        buffer.put(response.getBytes());
                        buffer.flip();

                        client.write(buffer);
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }

            @Override
            public void failed(Throwable exc, Void attachment) {
                System.err.println("Failed to accept connection");
                exc.printStackTrace();
            }
        };

        // Accetta la connessione del primo client
        server.accept(null, handler);

        // Per simulare un event loop infinito come in Node.js
        while (true) {
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
