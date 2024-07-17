package source.scratch.java.outputstream;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;

public class Main {
	
	public static void main(String[] args) {
		ByteArrayOutputStream boStream = null;
		DataOutputStream doStream = null;

		try {
			boStream = new ByteArrayOutputStream();
			doStream = new DataOutputStream(boStream);
			doStream.writeUTF("Ciao");
			doStream.writeUTF("Mondo");
			doStream.writeUTF("!");
			doStream.flush();
			byte[] data = boStream.toByteArray();
			System.out.println(data.length);
			for (byte b : data)
				System.out.print(b + " ");
			System.out.println();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
