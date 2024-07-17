package source.scratch.java.rmi.src;

public interface EchoInterface extends java.rmi.Remote {
	String getEcho(String echo) throws java.rmi.RemoteException;
}
