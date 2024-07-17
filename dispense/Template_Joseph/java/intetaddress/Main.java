package source.scratch.java.intetaddress;

import java.net.InetAddress;

public class Main {
	public static void main(String[] args){
		try {
			InetAddress[] addr = InetAddress.getAllByName("localhost");
			for (InetAddress a : addr)
				System.out.println(a);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}