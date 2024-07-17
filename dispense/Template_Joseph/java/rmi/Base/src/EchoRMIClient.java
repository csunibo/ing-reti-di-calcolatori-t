package source.scratch.java.rmi.src;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.rmi.Naming;

public class EchoRMIClient {

	public static void main(String[] args) {

		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));

		try {
			EchoInterface serverRMI = (EchoInterface) Naming.lookup("//localhost:1099/EchoService");

			String message, echo;
			message = stdIn.readLine();

			echo = serverRMI.getEcho(message);
			System.out.println("echo: " + echo);
		} catch (Exception e) {
			e.printStackTrace();
			System.exit(1);
		}

	}

}
