package jpot;
import java.lang.*;
import java.lang.Object;
import java.util.*;
import java.io.*;


public class JavaUIStatic {
	String content = new String();
	final String newLine = System.getProperty("line.separator");
	final String templateFile = "java.tpl";
	int noOfVar = 0;
	int noOfRow = 1;
	Vector varInfo = new Vector(3,1);
	
	
	public JavaUIStatic() {
	}
	
	private void createContent(String className) {
		BufferedReader buffer = null;
		String thisLine = new String();
		
		try {
			buffer = new BufferedReader(new FileReader("javastatic.tpl"));
		}
		catch (FileNotFoundException fe) {
			System.err.println("Template file for the Java Interface not found " + fe);
			System.exit(-1);
		}
		
		try {
			while ((thisLine = buffer.readLine()) != null) {
				if (thisLine.trim().startsWith("public class UITemplate")) {
					thisLine = "public class " + className + " extends JFrame {";
					content += thisLine + newLine;
					continue;
				}
				if (thisLine.trim().startsWith("static final String windowTitle =")) {
					StringBuffer tempBuffer = new StringBuffer(thisLine);
					tempBuffer.append('"' + "Example Source Code" + '"' + ";");
					thisLine = new String(tempBuffer);
					content += thisLine + newLine;
					continue;
				}
				if (thisLine.trim().startsWith("static String exampleSource")) {
					StringBuffer tempBuffer = new StringBuffer(thisLine);
					tempBuffer.append('"' + className + ".html" + '"' + ";");
					thisLine = new String(tempBuffer);
					content += thisLine + newLine;
					continue;
				}
				if (thisLine.trim().startsWith("String cmd =")) {
					StringBuffer tempBuffer = new StringBuffer(thisLine);
					tempBuffer.append('"' + className + '"' + ";");
					thisLine = new String(tempBuffer);
					content += thisLine + newLine;
					continue;
				}
				if (thisLine.trim().startsWith("public UITemplate()")) {
					thisLine = "	public " + className + "() {";
					content += thisLine + newLine;
					continue;
				}
				content += thisLine + newLine;
			}
		}
		catch (IOException ie) {
			System.err.println("Can't read from the buffer to build the java interface " + ie);
			System.exit(-1);
		}
	}
	
	
	public void createFile(String filename) {
		File fp = new File(filename + ".java");
		RandomAccessFile access = null;
		
		/**
		 * Create the content of the Java Interface for this example
		 */
		createContent(filename);
		/**
		 * Write the created content to the file.
		 */
		try {
			fp.createNewFile();
		}
		catch (IOException ie) {
			System.err.println("Error in creating source file for the Java Interface " + ie);
			System.exit(-1);
		}
		try {
			access = new RandomAccessFile(fp, "rw");
		}
		catch (IOException ie) {
			System.err.println("Error in accessing the new file " + ie);
			System.exit(-1);
		}
		try {
			access.writeBytes(content);
		}
		catch (IOException ie) {
			System.err.println("Error in writing to the file " + ie);
			System.exit(-1);
		}
	}
}
