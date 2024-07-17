import java.io.*;

public class DirFunctions {
	public static void listDir(String dirName) throws Exception {

		File dirCorr = new File(dirName);
		if (dirCorr.exists()) {
			File[] files = dirCorr.listFiles();
			for (int i = 0; i < files.length; i++) {
				//qui ad esempio controllo che sia un file fi testo
				if(files[i].getName().endsWith(".txt")){
					System.out.println("Controllo il file " + files[i].getName());
					BufferedReader br = null;
					String line = null;
					//leggo il file riga per riga
					try {
						br = new BufferedReader(new InputStreamReader(new FileInputStream(files[i])));

						// esco dal ciclo alla lettura di un valore negativo -> EOF
						while((line = br.readLine()) != null){
							System.out.println(line);
						}

						br.close();
					} catch (Exception e) {
						throw new Exception(e.toString());
					}
				}
			}
		} else {
			System.out.println("Dir non esistente....\n");
		}
	}

	// esplora una directory fino alle directory di secondo livello
	public static void expolreDir(String dir){

        File dirCorr = new File(dir);
        if (dirCorr.exists()) {
			// direttorio corrente
			File[] files = dirCorr.listFiles();
			for (int i = 0; i < files.length; i++) {
                System.out.println("File nel direttorio: ");
				for (File file : files) {
					System.out.println(file);
				}
				
				if(files[i].isDirectory()){
					// directory di primo livello
                    File[] files2 = files[i].listFiles();

                    System.out.println("File nel direttorio di primo livello: ");
                    for (File file : files2) {
                        System.out.println(file);
                    }

                    for (int j = 0; j < files2.length; j++) {
                        if(files2[j].isDirectory()){
							// directory di secondo livello
							File[] files3 = files2[j].listFiles();

							System.out.println("File nel direttorio di secondo livello: ");
							for (File file : files3) {
								System.out.println(file);
							}
						}
                    }
                }
				
			}

		} else {
			System.out.println("Dir non esistente....\n");
		}


	}

	public static void main(String[] args){
		expolreDir("Dir");
	}

}

