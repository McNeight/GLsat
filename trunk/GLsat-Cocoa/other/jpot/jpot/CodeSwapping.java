package jpot;
import java.lang.*;
import java.lang.Object;
import java.util.*;
import java.io.*;

public class CodeSwapping {
    // Start command line flat
    final String startFlag = "/*$$$swap";
    // End command line flag
    final String endFlag = "$$$*/";
    // Additional flags
    boolean isFlip = false;
    boolean isFlop = false;
    boolean isEnd  = false;
    String sourceFile = new String();
    // Final result of the text file 
    static StringBuffer finalStringBuffer = new StringBuffer();
    // Element that contains information about the code swapping part
    CodeSwapInfo info;

    public CodeSwapping(String filename) {
	boolean isCommandLine = false;
	boolean isCodeLine = true;
	boolean onFlip = false;
	boolean onFlop = false;

	BufferedReader bufferIn = null;
	info = new CodeSwapInfo();
	
	String firstTitle = new String();  // label of the first radio button
	String secondTitle = new String(); // label of the second radio button
	String commandLine = new String();
	String thisLine = new String ();
	String newLine = System.getProperty("line.separator");
	sourceFile = filename;
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
	
	try {
	    while ((thisLine = bufferIn.readLine()) != null) {
		if (isCommandLine == false) {
		    if (checkIfCommandLine(thisLine) == true) {
			// Process the case of command line
			isCommandLine = true;
			// turn the flag to mark that code swap existed.
			info.setCodeSwap();
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
			    if ((isFlip == true))  {
			      onFlip = true;
			      thisLine = "if (JPOT$swap == 1) {" + newLine + thisLine;
			      isFlip = false;
			    }

			    if ((isEnd == true) && (isFlop == false)) {
			      // This is the case that there is a gap of code lines
			      // between the flip and the flop, also to handle the 
			      // last end after the flop part.
			      thisLine = "} /*end swap*/" + newLine + thisLine;
			      isEnd = false;
			    }
			    
			    if ((isFlop == true)) {
			      if (isEnd == true) {
				// Get to the situation that end of flip and start of flop
				thisLine = "} /*end swap*/" + newLine +
				  "if (JPOT$swap == 0) {" + newLine + thisLine;
				isEnd = false;
			      }
			      else {
				thisLine = "if (JPOT$swap == 0) {" + newLine + thisLine;
			      }
			      onFlop = true;
			      isFlop = false;
			    }
			    
			    //if ((isEnd == true) && (onFlop == true)){
			    //	isEnd  = false;
			    //	onFlip = false;
			    //	onFlop = false;
			    //	thisLine = "} /*end swap*/" + newLine + thisLine;
			    //}

			    thisLine = thisLine + newLine;
			    finalStringBuffer.append(thisLine);
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
    }
    
    /**
     *  Parse the command line	
     */
    private void parseCommandLine(String line) {
	String tempStr;

	// First trim all the space at two end of the line
	String str = line.trim();
	// Check the catergory of the command line
	if (str.startsWith("/*$$$swap-flip")) {
	  isFlip = true;
	  isFlop = false;
	  //isEnd  = false;
	}
	if (str.startsWith("/*$$$swap-flop")) {
	  isFlop = true;
	  isFlip = false;
	  //isEnd  = false;
	}
	if (str.startsWith("/*$$$swap-end")) {
	  isEnd = true;
	}
	// Cut off the start and end flags (the /*$$$ and $$$*/) of the 
	// command line and trim all the spaces at two ends...
	str = str.substring(5,str.length()- 5).trim();
	// The result would be [.....],[....],[]
	// Parse the line to get the label of the radio button
	// this will be done only if the command line is not the command end.
	//if (isEnd == false) 
	//  str = str.substring(9);
	if (isFlip == true)
	  info.setFirstLabel(str.substring(9));
	if (isFlop == true)
	  info.setSecondLabel(str.substring(9));
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
     * Return the string that contain the final file name after parsing
     */
    public String parseFile() {
	String filename = sourceFile + ".tmp1";
	File fp = new File(filename);
	RandomAccessFile access = null;
	boolean delete = fp.delete();

	try {
	    fp.createNewFile();
	}
	catch (IOException ie) {
	    System.err.println("Couldn't create temporary file " + filename + " " + ie);
	    System.exit(-1);
	}
	try {
	    access = new RandomAccessFile(fp, "rw");
	}
	catch (IOException ie) {
	    System.err.println("Error in accessin the temporary file " + ie);
	    System.exit(-1);
	}
	try {
	    access.writeBytes(new String(finalStringBuffer));
	    access.close();
	}
	catch (IOException ie) {
	    System.err.println("Error in writing to the temporary file " + ie);
	    System.exit(-1);
	}
	return filename;
    }

    // Return the informatin object that contain information about
    // the code swapping
    public CodeSwapInfo getSwapInfo() {
	return info;
    }
}
