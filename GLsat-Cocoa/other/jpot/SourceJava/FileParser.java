import jpot.*;
import java.lang.*;
import java.lang.Object;
import java.util.*;
import java.io.*;

class FileParser {
    // Start command line flat
    final String startFlag = "/*$$$";
    // End command line flag
    final String endFlag = "$$$*/";
    // Number of variables in the file
    static int numOfElement = 0;
    // Vector contains the object of variables
    static Vector elementList = new Vector(0,1);
    // Final result of the text file 
    static StringBuffer finalFile = new StringBuffer();
    
    public FileParser(String filename) {
	boolean isCommandLine = false;
	boolean isCodeLine = true;
	BufferedReader bufferIn = null;
	
	String title = new String("");
	String commandLine = new String("");
	String thisLine = new String ("");
	String newLine = System.getProperty("line.separator");
	
	/**
	 *  Read the whole file to the buffer
	 */
	
	try {
	    bufferIn = new BufferedReader(new FileReader(filename));
	}
	catch (FileNotFoundException fe) {
	    System.err.println("File not found " + fe);
	    System.exit(-1);
	}
       	/**
	 * Main part of the parser: 
	 * Read each line from the buffer. If it is the command line then
	 * combine them have the complete command line to process. If it is
	 * not the command line the put into the buffer to create the final
	 * C file.
	 */
	try {
	    while ((thisLine = bufferIn.readLine()) != null) {
		if (isCommandLine == false) {
		    if (checkIfCommandLine(thisLine) == true) {
			// Process the case of command line
			isCommandLine = true;
			commandLine = commandLine + thisLine;
			if (checkEndCommandLine(thisLine)== true) {
			    parseCommandLine(commandLine);
			    isCommandLine = false;
			    isCodeLine = true;
			    commandLine = " ";
			}
		    }
		    else {
			if (isCodeLine == true) {
			    // Process the case of code line
			    thisLine = thisLine + newLine;
			    finalFile.append(thisLine);
			}
		    }
		}
		else {
		    commandLine = commandLine + thisLine;
		    if (checkEndCommandLine(thisLine) == true) {
			parseCommandLine(commandLine);
			commandLine = " ";
			isCommandLine = false;
			isCodeLine = true;
		    }
		}
	    }
	}
	catch (IOException ie) {
	    System.err.println("Can not read from the buffer " + ie);
	    System.exit(-1);
	}
	//System.out.println("Finale File********");
	//System.out.println(finalFile);
    }
    
    /**
     *  Parse the command line	
     */
    private void parseCommandLine(String line) {
	String tempStr;
	ElementInfo element;
	// First trim all the space at two end of the line
	String str = line.trim();
	// Cut off the start and end flags of the command line
	// and trim all the spaces at two ends...
	str = str.substring(5,str.length()- 5).trim();	
	// The result would be [.....],[....],[]
	// Parse each command of the string. For each comand, get
	// the object and keep it in the list (Vector)
	StringTokenizer token = new StringTokenizer(str,"[");
	while (token.hasMoreTokens()) {
	    tempStr = "[" + token.nextToken();
	    /*
	      // There maybe constants in the function, not all variables.
	      if (tempStr.length() < 6) {
	      element = new ElementInfo();
	      element.setDataType("constant");
	      }
	      else {
	      element = parseCommand(tempStr);
	      numOfElement = numOfElement + 1;
	      }*/
	    element = parseCommand(tempStr);
	    numOfElement += 1;
	    elementList.addElement(element);
	}
    }
    /**
     * Parse each individual command
     */
    private ElementInfo parseCommand(String line) {
	ElementInfo element = new ElementInfo();
	String tempStr;
	/**
	 *  Chop the characters '[' and ']' out of the string
	 */
	String str = line.substring(1, line.length() - 1);		
	StringTokenizer token = new StringTokenizer(str,",");
	/**
	 *  Get the label
	 */
	if (token.hasMoreTokens()) {
	    tempStr = token.nextToken();
	    element.setLabel(tempStr.trim());
	}
	/** 
	 * Get the datatype - Modified to add the string-slider as requested of advisor
    	 */
	if (token.hasMoreTokens()) {
	    tempStr = token.nextToken();
	    /* added start */
	    if (tempStr.trim().equals("s")) {
		element.setDataType("string");
	    } /* added end - code in the else is the origin */
		else {
		    if (tempStr.trim().equals("f")) 
			element.setDataType("float");
		    else 
			if (tempStr.trim().equals("i"))
			    element.setDataType("int");			
		}
	}
	
	/**
	 *  Get the default value- Modified to add the string-slider as requested of advisor
	 */
	if (token.hasMoreTokens()) {
	    tempStr = token.nextToken();
	    /* added start */
	    if (element.dataType.equals("string")) {
		element.setDefaultValue(new String(tempStr.trim()));
	    } /* added end - code in the else is the origin */
	    else {
		if (element.dataType.equals("int"))
		    element.setDefaultValue(new Integer(tempStr.trim()));
		else element.setDefaultValue(new Float(tempStr.trim()));
	    }
	}
	
	/**
	 *  Get the minimum value
	 *  MODIFIED - If the case is string-slider, it will get the string, set value for
	 *  ElementInfo object then return, otherwise go on for getting other value.
	 *  Added March 20th to handle new request of advisor.
	 */
	if (token.hasMoreTokens()) {
	    tempStr = token.nextToken();
	    /* added start */
	    if (element.dataType.equals("string")) {
		element.setStringValue(tempStr);
		return element;
	    } /* end start - code in the else block is the origin */
		else {
		    if (element.dataType.equals("int"))
			element.setMinValue(new Integer(tempStr.trim()));
		    else element.setMinValue(new Float(tempStr.trim()));
		}
	}
	/**
	 *  Get the maximum value
	 */
	if (token.hasMoreTokens()) {
	    tempStr = token.nextToken();
	    if (element.dataType.equals("int"))
		element.setMaxValue(new Integer(tempStr.trim()));
	    else element.setMaxValue(new Float(tempStr.trim()));
	}
	/**
	 *  Get the line control
	 */
	if (token.hasMoreTokens()) {
	    tempStr = token.nextToken();
	    element.setLineControl(tempStr.trim().charAt(0));
	}
	return element;
    }
    
    /**
     *  Check whether the line is a command line
     */
    private boolean checkIfCommandLine(String line){
	String str = line.trim();
	try {
	    if (str.startsWith(startFlag) == true)
		return true;
	}
	catch (NullPointerException ne) {
	    System.err.println(ne);
	}
	return false;
    }
    
    /**
     *  Check whether it is the end of the command line
     */
    private boolean checkEndCommandLine(String line) {
	String str = line.trim();
	try {
	    if (str.endsWith(endFlag) == true)
		return true;
	}
	catch (NullPointerException ne) {
	    System.err.println(ne);
	}
	return false;	
    }
    
    /**
     *  Get number of variables in the string, base on counting
     *  number of appearance of "["
     */
    private int getNoVars(String line) {
	int counter = 0;
	StringTokenizer string = new StringTokenizer(line,"[");
	counter = string.countTokens() - 1;
	return counter;
    }
    
    /**
     * Return the string that contain the final file after parsing
     */
    public String parseFile() {
	return new String(finalFile);
    }
    
    public static void main(String argc[]){
	// System.out.println("argc[0]" + argc[0] + " argc[1]" + argc[1]);
	// Check for the code swapping part before doing anything further
	CodeSwapping codeSwap = new CodeSwapping(argc[0]);
	String fileResult = codeSwap.parseFile();
	CodeSwapInfo swapInfo = new CodeSwapInfo();
	swapInfo = codeSwap.getSwapInfo();
	
	FileParser file = new FileParser(fileResult);	
	String result = file.parseFile();
	
	CToHtml html = new CToHtml(result, swapInfo, numOfElement);
	html.generateFile("html" + System.getProperty("file.separator") + argc[1]);
	
	OGLSource source = new OGLSource(result, swapInfo, elementList,numOfElement);
	source.createFile("ogl" + System.getProperty("file.separator") + argc[1]);
	

	if ((numOfElement == 0) && (swapInfo.isCodeSwap() == false)){
	    JavaUIStatic java = new JavaUIStatic();
	    java.createFile(argc[1]);
	}
	else {
	    JavaUI java = new JavaUI(swapInfo, elementList, numOfElement);
	    java.createFile(argc[1]);
	}
	
    }
}
