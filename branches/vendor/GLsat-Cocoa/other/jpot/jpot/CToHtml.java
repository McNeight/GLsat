package jpot;
import java.lang.*;
import java.lang.Object;
import java.util.*;
import java.io.*;


public class CToHtml {
    StringBuffer contentBuffer = new StringBuffer();
    CodeSwapInfo swapInfo;
    int noOfVar = 0;
    final String includeStart = "<font color=" + '"' + "#0000ff" +'"' + ">";
    final String includeEnd = "</font> ";
    final String lessThan = "&lt;";
    final String greaterThan = "&gt;";
    final String functionStart = "<font color=" + '"' + "#0000ff" +'"' + ">";
    final String functionEnd = "</font>";
    final String varStart = "<font color=" + '"' + "#ff0000" + '"' + "><b>";
    final String varEnd = "</b></font>";
    final String glStart = "<font color=" + '"' + "#ff00ff" + '"' + ">";
    final String glEnd = "</font>";
    final String anchor = "<A name=" + '"' + "gohere" + '"' + "></a>";
    final String newLine = System.getProperty("line.separator");
    final String flipStr = "if (JPOT$swap == 1) {";
    final String flopStr = "if (JPOT$swap == 0) {";
    final String swapEnd = "} /*end swap*/";

    public CToHtml(String content, CodeSwapInfo info, int component) {
	
	noOfVar = component;
	/**
	 * Store the inputBuffer to process.
	 */
	contentBuffer = new StringBuffer(content);		
	/**
	 * Reset flip/flop string containers in the swapInfo object
	 */
	swapInfo = info;
	swapInfo.setFirstText("");
	swapInfo.setSecondText("");
    }
    
    private void setAttribute(String orgStr, String startDestStr, String endDestStr) {
	int posFound = 0;
	String tempStr = new String();
	String contentStr = new String(contentBuffer);
	
	while ((posFound = contentStr.indexOf(orgStr)) != -1) {
	    tempStr = startDestStr + orgStr.trim() + endDestStr;
	    contentBuffer = contentBuffer.replace(posFound, posFound + orgStr.length(), tempStr);
	    contentStr = new String(contentBuffer);
	}
    }
    
    private void replaceString(String orgStr, String destStr) {
	int posFound = 0;
	String contentStr = new String(contentBuffer);
	
	while ((posFound = contentStr.indexOf(orgStr)) != -1) {
	    contentBuffer = contentBuffer.replace(posFound, posFound + orgStr.length(), destStr);
	    contentStr = new String(contentBuffer);
	}
    }
    
    private void setVariableAttribute() {
	int counter = 0;
	int posFound = 0;
	int preFound = 0;
	boolean setAnchor = true;
	String tempStr = new String();
	String replaceStr = new String();
	String contentStr = new String();
	
	for (counter = noOfVar; counter > 0; counter--) {
	    tempStr = "JPOT$" + counter;
	    preFound = 0;

	    contentStr = new String(contentBuffer);
	    while ((posFound = contentStr.indexOf(tempStr, preFound)) != -1) {
		preFound = posFound + replaceStr.length();
		if ((counter == 1) && (setAnchor == true)) {
		    setAnchor = false;
		    replaceStr = anchor + varStart + tempStr + varEnd;
		}
		else
		    replaceStr = varStart + tempStr + varEnd;
		
		contentBuffer = contentBuffer.replace(posFound, posFound + tempStr.length(),replaceStr);
		contentStr = new String(contentBuffer);
	    }
	}
    }
    
    private boolean isGLFunction(String str) {
	if (str.trim().startsWith("gl"))
	    return true;
	return false;
    }
    
    private String setGLFunction(String str) {
	StringBuffer function = new StringBuffer(str);
	int posFound = str.indexOf("(");
	String glFunction = str.substring(0, posFound );
	
	function = function.replace(0, glFunction.length(), glFunction + glEnd);
	function = function.insert(0,glStart);
	return new String(function);
    }
    
    private void setGLFunctionAttribute() {
	BufferedReader buffer = new BufferedReader(new StringReader(new String(contentBuffer)));
	String thisLine = new String();
	String content = new String();
	try {
	    while ((thisLine = buffer.readLine()) != null) {
		if (isGLFunction(thisLine) == true) {
		    thisLine = setGLFunction(thisLine);
		}
		content += thisLine + newLine;
	    }
	}
	catch (IOException ie) {
	    System.err.println("Can't read from buffer " + ie);
	    System.exit(-1);
	}
	contentBuffer = new StringBuffer(content);
    }
    
    public void generateFile(String targetName) {
	String fileName = targetName + ".html";
	String content = new String();
	File fp = new File(fileName);
	RandomAccessFile access = null;
	int posFound; // use later in the code swap processing
	/**
	 * Process the '<' characters
	 */
	replaceString("<",lessThan);
	/**
	 * Process the '>' characters
	 */
	replaceString(">",greaterThan);
	/**
	 * Process the #include statements
	 */
	setAttribute("#include ",includeStart, includeEnd);
	/**
	 * Process the variables - setting different attributes (color and bold)
	 */
	setVariableAttribute();
	/**
	 * Process the function name - setting by different colors
	 */
	// Optional - Do this later
	setGLFunctionAttribute();
	
	/**
	 * Insert header and append footer
	 */
	contentBuffer = contentBuffer.insert(0,"<html><body><pre>" + newLine);
	contentBuffer = contentBuffer.append("</pre></body></html>");
	
	/**
	 * Write to the result file
	 */
	try {
	    fp.createNewFile();
	}
	catch (IOException ie) {
	    System.err.println("Couldn't create the new file" + ie);
	    System.exit(-1);
	}
	
	try {
	    access = new RandomAccessFile(fp,"rw");
	}
	catch (IOException ie) {
	    System.err.println("Error in accessing the file " + ie);
	    System.exit(-1);
	}
	
	try {
	    access.writeBytes(new String(contentBuffer));
	    access.close();
	}
	catch (IOException ie) {
	    System.err.println("Error in writing to file " + ie);
	    System.exit(-1);
	}
	// If there is code-swapping then we have to filt out all the if-related statements
	if (swapInfo.isCodeSwap()) {
	    BufferedReader bufferIn = null;
	    StringBuffer bufferOut = new StringBuffer();
	    String thisLine = new String();
	    String firstStr = new String();
	    String secondStr = new String();
	    String swapFile1 = targetName + ".swap1";
	    String swapFile2 = targetName + ".swap2";
	    File swapfp1 = new File(swapFile1);
	    File swapfp2 = new File(swapFile2);
	    boolean onFlip = false;
	    boolean onFlop = false;

	    try {
		bufferIn = new BufferedReader(new FileReader(fileName));
	    }
	    catch (FileNotFoundException fe) {
		System.err.println("File must be existed already " + fe);
		System.exit(-1);
	    }
	    try {
		while ((thisLine = bufferIn.readLine()) != null) {
		    if (thisLine.trim().startsWith("if (JPOT$swap == 1)")) {
			onFlip = true;
			continue;
		    }
		    if (thisLine.trim().startsWith("if (JPOT$swap == 0)")) {
			onFlop = true;
			continue;
		    }
		    if (thisLine.trim().startsWith("} /*end swap*/")) {
			if (onFlip == true) onFlip = false;
			if (onFlop == true) onFlop = false;
			continue;
		    }
		    if (onFlip == true) {
			// Append it to the firsStr
			firstStr += thisLine + newLine;
		    }
		    if (onFlop == true) {
			// Append it to the secondStr
			secondStr += thisLine +  newLine;
		    }
		}
		// Update  value of swapInfo object. The name of files
		// contain the strings will be passed
		swapInfo.setFirstText(swapFile1.substring(5));
		swapInfo.setSecondText(swapFile2.substring(5));
	    }
	    catch (IOException ie) {
		System.err.println("Error reading from buffer " + ie);
		System.exit(-1);
	    }
	    // Update the contentBuffer
	    replaceString(flipStr, " ");
	    replaceString(flopStr, " ");
	    replaceString(swapEnd, " ");
	    // Then re-write the final content of the html to the file
	    fp.delete();
	    try {
		fp.createNewFile();
	    }
	    catch (IOException ie) {
		System.err.println("Couldn't create the new file" + ie);
		System.exit(-1);
	    }
	    
	    try {
		access = new RandomAccessFile(fp,"rw");
	    }
	    catch (IOException ie) {
		System.err.println("Error in accessing the file " + ie);
		System.exit(-1);
	    }
	    
	    try {
		access.writeBytes(new String(contentBuffer));
		access.close();
	    }
	    catch (IOException ie) {
		System.err.println("Error in writing to file " + ie);
		System.exit(-1);
	    } 
	    // Finally write the content of swap-strings to files
	    try {
		swapfp1.createNewFile();
	    }
	    catch (IOException ie) {
		System.err.println("Can't create the file " + swapFile1 + " ie");
		System.exit(-1);
	    }
	    try {
		access = new RandomAccessFile(swapfp1, "rw");
	    }
	    catch (IOException ie) {
		System.err.println("Error in accessing the file " + ie);
		System.exit(-1);
	    }
	    try {
		access.writeBytes(firstStr);
		access.close();
	    }
	    catch (IOException ie) {
		System.err.println("Error in writing to swap file 1 " +  ie);
		System.exit(-1);
	    }
	    
	    try {
		swapfp2.createNewFile();
		access = new RandomAccessFile(swapfp2, "rw");
		access.writeBytes(secondStr);
		access.close();
	    }
	    catch (IOException ie) {
		System.err.println("Error in creating swap file 2 " + ie);
		System.exit(-1);
	    }
	}
    }
}
