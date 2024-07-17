package source.scratch.java.rmi.src;

public class EchoRMIServer extends java.rmi.server.UnicastRemoteObject implements EchoInterface {
	public EchoRMIServer() throws java.rmi.RemoteException {
		super();
	}

	public String getEcho(String echo) throws java.rmi.RemoteException {
		System.out.println("echo: " + echo);
		return echo;
	}

	public static void main(String[] args) {
		// Questo main fa da .run()

		try {
			EchoRMIServer serverRMI = new EchoRMIServer();

			java.rmi.Naming.rebind("//localhost:1099/EchoService", serverRMI);
		} catch (Exception e) {
			e.printStackTrace();
			System.exit(1);
		}

	}

}
