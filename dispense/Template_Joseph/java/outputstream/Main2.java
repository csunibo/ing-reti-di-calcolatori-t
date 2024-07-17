package source.scratch.java.outputstream;

import java.io.*;

public class Main2 {
	FileReader f1 = null;
	FileReader f2 = null;

	InputStream in = null;
	OutputStream out = null;

	public static void main(String[] args) {

		try {
			File f1 = new File(args[0]);
			File f2 = new File(args[1]);

			InputStream in = new FileInputStream(f1);
			OutputStream out = new FileOutputStream(f2);

			int c;
			while ((c = in.read()) != -1) {
				out.write((char) c);
			}

			out.flush();
			out.close();
			in.close();

		} catch (Exception e) {
		}

	}

}
