package jpot;
import java.lang.*;
import java.lang.Object;
import java.util.*;
import java.io.*;


public class OGLSource {
    StringBuffer contentBuffer = new StringBuffer();
    String contentTemplate = new String();
    final String newLine = System.getProperty("line.separator");
    String templateFile = new String();
    String initStr = new String();
    String fileName = new String(); 
 
    int noOfVar = 0;
    Vector varInfo = new Vector(3,1);
    CodeSwapInfo swapInfo;
	
    public OGLSource(String content, CodeSwapInfo info, Vector var, int component) {
	noOfVar = component;
	varInfo = var;
	contentBuffer = new StringBuffer(content);	
	if (System.getProperty("os.name").startsWith("Windows")) {
	    templateFile = "windows.tpl";
	    initStr = "  event = CreateEvent(NULL, TRUE, FALSE, " + '"' + "Refreshing" + '"' + ");";
	}
	else {
	    templateFile = "unix.tpl";
	    initStr = "  signal(SIGUSR2, sighandle);";			
	}
	swapInfo = info;
    }
	
    public void createFile(String filename) {
	File fp = new File(filename + ".c");
	RandomAccessFile access = null;
	String finalContent = new String();
	fileName = filename;
	setTemplate();	
	if ((noOfVar != 0) || (swapInfo.isCodeSwap() == true))
	    finalContent = makeContent();
	else 
	    finalContent = new String(contentBuffer);
	
	try {
	    fp.createNewFile();
	}
	catch (IOException ie) {
	    System.err.println("Can't create file " + filename + ".c" + " for output. " + ie);
	    System.exit(-1);
	}
	try {
	    access = new RandomAccessFile(fp,"rw");
	}
	catch (IOException ie) {
	    System.err.println("Error in accessing the new file " + filename + ".c " + ie);
	    System.exit(-1);
	}
	try {
	    access.writeBytes(finalContent);
	}
	catch (IOException ie) {
	    System.err.println("Error in writing to the file " + filename + ".c" + ie);
	    System.exit(-1);
	}
    }
    
    /**
     * process the input buffer (contentBuffer) which contents the whole original file.
     * Insert appropriate functions and variables, and modify the program to be able
     * to work in the tutorial.
     */
    private String makeContent() {
	BufferedReader buffer = new BufferedReader(new StringReader(new String(contentBuffer)));
	String thisLine = new String();
	String prevLine = new String();
	String finalString = new String();
	int posFound = 0;
	boolean setGlobalVar = true; // if true the insert the global function declaration
	boolean afterInit = false; // if true then insert the message initialization
	boolean afterDisplay = false; // if true then insert the readdata function
	boolean afterIdle = false; // if true then insert the checkdata function
	boolean hasIdle = false; // if idle function exists then set to be true
	boolean inMain = false; // used when declaring a non-existing idle function.

	try {
	    prevLine = buffer.readLine() + newLine;
	}
	catch (IOException ie) {
	    System.err.println("Can't read the first string from the buffer " + ie);
	    System.exit(-1);
	}
	try {
	    while ((thisLine = buffer.readLine()) != null) {
		finalString += prevLine;
	      	/**
		 *Insert the global variables declaration after the last #include statement.
		 */
		if ((isIncludeStatement(thisLine) == false) && (setGlobalVar == true)) {
		    finalString += contentTemplate + newLine; // value already been set in the setTemplate() function
		    setGlobalVar = false;
		    
		}
	        /**
		 * Insert the message initialization statement after into the init() function
		 */
		if (isFunction(prevLine,"void","init") == true) {
		    if (thisLine.trim().startsWith("{") == true) {
			afterInit = true;
			prevLine = thisLine + newLine;
			continue;
		    }
		    else
			finalString += initStr + newLine;
		    
		}
		if (afterInit == true) {
		    finalString += initStr + newLine;
		    afterInit = false;
		}
		/**
		 * Insert the call of ReadData() function to the display() function
		 */
		if (isFunction(prevLine,"void","display") == true) {
		    if (thisLine.trim().startsWith("{") == true) {
			afterDisplay = true;
			prevLine = thisLine + newLine;
			continue;
		    }
		    else
			finalString += "ReadData();" + newLine;
		}
		if (afterDisplay == true) {
		    finalString += "ReadData();" + newLine;
		    afterDisplay = false;
		}
		/**
		 * Insert the call of CheckRefresh() function to the idlefunc()
		 */
		if ((isFunction(prevLine,"void","idlefunc") == true) && (inMain == false)){
		    if (thisLine.trim().startsWith("{") == true){
			afterIdle = true;
			hasIdle = true;
			prevLine = thisLine + newLine;
			continue;
		    }
		    else
			finalString += "CheckRefresh();" + newLine;
		}
		if (afterIdle == true) {
		    finalString += "CheckRefresh();" + newLine;
		    afterIdle = false;
		}
		
		/**
		 * Check the main function.
		 * if get to the main function but has not met the idle function
		 * then have to build the idle function.
		 */
		if (isFunction(thisLine,"int", "main") == true) {
		    inMain = true;
		    if (hasIdle == false) {
			thisLine = "void idlefunc(void) {" + newLine +
			  "        CheckRefresh();" + newLine + "}" + newLine + thisLine;
		    }
		}
		
		/** 
		 * Check to insert the declaration of use for idle function if 
		 * necessary
		 */
		if ((thisLine.trim().startsWith("glutMainLoop")) && (inMain == true)) {
		  thisLine = "glutIdleFunc(idlefunc);" + newLine + thisLine;
		}

		/**
		 * Nothing need to check, do as normal
		 */				
		prevLine = thisLine + newLine;
	    }
	}
	catch (IOException ie) {
	    System.err.println("Error in reading from the buffer " + ie);
	    System.exit(-1);
	}
	finalString += prevLine;
	return finalString;
    }
    
    /**
     * Read the template file, and set the appropriate value for 
     * global variables and fscanf parameters
     */
    private void setTemplate()  {
	BufferedReader bufferIn = null;
	String thisLine = new String();
	final String varStr = new String("<Global Variable>");
	final String scanStr = new String("<Read Data>");
	int posFound = 0;
	
	try {
	    bufferIn = new BufferedReader(new FileReader(templateFile));
	}
	catch (FileNotFoundException fe) {
	    System.err.println("Error in reading from Windows C template file " + fe);
	    System.exit(-1);
	}
	try {
	    while ((thisLine = bufferIn.readLine()) != null) {			
		if ((posFound = thisLine.indexOf(varStr)) != -1) {
		    //StringBuffer tempBuffer = new StringBuffer(thisLine);
		    //tempBuffer.replace(posFound, varStr.length(), setGlobalVariables());
		    //thisLine = new String(tempBuffer);
		    thisLine = setGlobalVariables();
		}
		if ((posFound = thisLine.indexOf(scanStr)) != -1) {
		    //StringBuffer tempBuffer = new StringBuffer(thisLine);	
		    //tempBuffer.replace(posFound, scanStr.length(), setFScanString());
		    //thisLine = new String(tempBuffer);
		    thisLine = setFScanString();
		} 
		contentTemplate += thisLine + newLine;
	    }
	}
	catch (IOException ie) {
	    System.err.println("Can not read from the buffer " + ie);
	    System.exit(-1);
	}														  														
    }
    
    
    
    /**
     * Set the string to declare global variable to the OpenGL C file.
     */
    private String setGlobalVariables() {
	String variableList = new String();
	String tempStr = new String();
	ElementInfo element = new ElementInfo();
	for (int i = 0; i < noOfVar; i++) {
	    element = (ElementInfo)varInfo.elementAt(i);	
	    if (element.dataType.equals("int") || element.dataType.equals("string")) {
		tempStr = "GLint " + "JPOT$" + (i+1) + " = " + element.defaultValue + ";" + newLine;
	    } else 
		tempStr = "GLfloat " + "JPOT$" + (i+1) + " = " + element.defaultValue + ";" + newLine;
	    variableList += tempStr;
	    tempStr = "";
	}
	// Add the declaration for the data file
	variableList += "char *datafile = " + '"' + fileName.substring(4) + ".dat" + '"' + ";" + newLine;
	// Added the variable for code-swapping control if it exists.
	if (swapInfo.isCodeSwap()) 
	    variableList += "GLint JPOT$swap = 1;" + newLine;
	return variableList;
    }
    
    /**
     * Create the fscanf() statement to read data from the link file to 
     * obtain new parameters to run the illustrated example.
     */
    private String setFScanString() {
	String result = new String();
	String argStr = new String();
	String varStr = new String();
	ElementInfo element = new ElementInfo();
	
	for (int i = 0; i < noOfVar; i++) {
	    element = (ElementInfo)varInfo.elementAt(i);	
	    if (element.dataType.equals("int") || element.dataType.equals("string")) {
		argStr += " %d";
	    } else {
		argStr += " %f";
	    }	
	    varStr += ", &JPOT$" + (i + 1);	
	}
	// Insert the varibale for code-swapping control if it exists
	if (swapInfo.isCodeSwap()) {
	    argStr += " %d";
	    varStr += ", &JPOT$swap";
	}   
	result = "fscanf(fp," + '"' + argStr + '"'+ varStr + ");";
	return result;
    }
    
    /**
     * Check whether the string is a function's declaration.
     * Get 3 parameters: 
     *	str is the input string
     *	type is the type of the function (void, int, float...)
     *	function is the name of the function
     */
    private boolean isFunction(String str, String type, String function) {
	if (str.indexOf(type) != -1) {
	    if (str.indexOf(function) != -1)				
		return true;
	}
	return false;		 
    }
    
    /**
     * Check whether the string is the "#include" statement.
     */
    private boolean isIncludeStatement(String str) {
	if (str.trim().startsWith("#include") == true)
	    return true;
	else 
	    return false;
    }
}
