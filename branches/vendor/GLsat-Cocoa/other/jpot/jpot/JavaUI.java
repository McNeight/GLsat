package jpot;
import java.lang.*;
import java.lang.Object;
import java.util.*;
import java.io.*;


public class JavaUI {
    String content = new String();
    final String newLine = System.getProperty("line.separator");
    final String templateFile = "java.tpl";
    int noOfVar = 0;
    int noOfRow = 1;
    Vector varInfo = new Vector(3,1);
    CodeSwapInfo swapInfo;
    
    public JavaUI(CodeSwapInfo info, Vector var, int component) {
	ElementInfo element = new ElementInfo();
	noOfVar = component;
	varInfo = var;
	/**
	 * get number of rows for arranging sliders on the window.
	 */
	for (int i = 0; i < component; i++) {
	    element = (ElementInfo)varInfo.elementAt(i);
	    if (element.lineControl == 'n')
		noOfRow += 1;
	}
	swapInfo = info;
	
    }
    
    private void createContent(String className) {
	BufferedReader buffer = null;
	String thisLine = new String();
	
	try {
	    if (swapInfo.isCodeSwap())
		buffer = new BufferedReader(new FileReader("javaswap.tpl"));
	    else
		buffer = new BufferedReader(new FileReader("java.tpl"));
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
		if (thisLine.trim().startsWith("static int COMPONENTS")) {
		    StringBuffer tempBuffer = new StringBuffer(thisLine);
		    tempBuffer.append(noOfVar + ";");
		    thisLine = new String(tempBuffer);
		    content += thisLine + newLine;
		    continue;
		}
		if (thisLine.trim().startsWith("static int ROWS")) {
		    StringBuffer tempBuffer = new StringBuffer(thisLine);
		    tempBuffer.append(noOfRow + ";");
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
		if (thisLine.trim().startsWith("final String fileStr1 =")) {
		    StringBuffer tempBuffer = new StringBuffer(thisLine);
		    tempBuffer.append('"' + swapInfo.getFirstText() + '"' + ";");
		    thisLine = new String(tempBuffer);
		    content += thisLine + newLine;
		    continue;
		}
		if (thisLine.trim().startsWith("final String fileStr2 =")) {
		    StringBuffer tempBuffer = new StringBuffer(thisLine);
		    tempBuffer.append('"' + swapInfo.getSecondText() + '"' + ";");
		    thisLine = new String(tempBuffer);
		    content += thisLine + newLine;
		    continue;
		}		
		if (thisLine.trim().startsWith("final String radLabel1 =")) {
		    StringBuffer tempBuffer = new StringBuffer(thisLine);
		    tempBuffer.append('"' + swapInfo.getFirstLabel() + '"' +";");
		    thisLine = new String(tempBuffer);
		    content += thisLine + newLine;
		    continue;
		}
		if (thisLine.trim().startsWith("final String radLabel2 =")) {
		    StringBuffer tempBuffer = new StringBuffer(thisLine);
		    tempBuffer.append('"' + swapInfo.getSecondLabel() + '"' + ";");
		    thisLine = new String(tempBuffer);
		    content += thisLine + newLine;
		    continue;
		}
		if (thisLine.trim().startsWith("String dataFile =")) {
		    StringBuffer tempBuffer = new StringBuffer(thisLine);
		    tempBuffer.append('"' + className + ".dat" + '"' + ";");
		    thisLine = new String(tempBuffer);
		    content += thisLine + newLine;
		    continue;
		}

		if (thisLine.trim().startsWith("void createSliderVector()")) {
		    thisLine = new String(createSliderVector());
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
    
    
    private String createSliderVector() {
	String funcStr = "	void createSliderVector() { " +
	    newLine + "		PSlider slider;" + newLine;
	final String addStr = "		vSlider.addElement(slider);";
	String tempStr = new String();
	ElementInfo element = new ElementInfo();
	
	for (int i = 0; i < noOfVar; i++) {
	    element = (ElementInfo)varInfo.elementAt(i);
	    if (element.dataType.equals("string")) {
		tempStr = "		slider = new PSlider(" + 
		    '"' + element.label + '"' + "," +
		    '"' + element.defaultValue + '"' + "," +
		    '"' + element.getStringValue() + '"' + "," +
		    "'" + element.lineControl + "');";
		funcStr += tempStr + newLine + addStr + newLine;
	    }
	    else {
		tempStr = "		slider = new PSlider(" + 
		    '"' + element.label + '"' + "," +
		    element.minValue + "," +
		    element.maxValue + "," +
		    element.defaultValue + "," +
		    "'" + element.lineControl + "');";
		funcStr += tempStr + newLine + addStr + newLine;
	    }
	}
	funcStr += "	}";
	return funcStr;
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
