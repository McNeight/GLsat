import jpot.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.text.*;
import javax.swing.JComponent.*;
import java.io.*;
import java.util.*;    
import java.net.URL;
import java.util.Vector;

public class Material extends JFrame {
	static int WIDTH = 500;
	static int HEIGHT = 600;
	static Process pid = null;	
	static boolean isWindows = true;
	static int round = 0;
	static Point scrollLoc = new Point(0,0);
	/**
	 *  pane that contains the source code
	 */
	JEditorPane textPane = new JEditorPane(); 
	JScrollPane scrollPane = new JScrollPane(); // main text pane with scroll bars
	// This lines will be vary for each class
	/**
	 * Windows title
	 */
	static final String windowTitle = "Example Source Code";
	/**
	 * Name of the OpenGL example source program
	 */
	static String exampleSource = "html" + System.getProperty("file.separator") + "Material.html";
	/**
	 * Number of sliders used in the frame.
	 */
	static int COMPONENTS = 13;
	/**
	 * Number of rows to arrange sliders in the frame
	 */
	static int ROWS = 5;
	/**
	 * Vector of sliders used in the frame
	 */
	static Vector vSlider = new Vector(COMPONENTS);
	/**
	 * Vector of Panels used to place the sliders on.
	 */
	static Vector vSubPanel = new Vector(ROWS);
	
	/**
	 * Name of the executable file illustrates for the example.
	 */
	String cmd = "ogl" + System.getProperty("file.separator") + "Material";
	
	/**
	 * Data file to provide interchance data during the interaction
	 */
	String dataFile = "Material.dat";
	
	/**
	 * Procedure to create the vector of sliders that will be used when
	 * locating sliders on the frame
	 */
	void createSliderVector() { 
		PSlider slider;
		slider = new PSlider("Amb_Red",0.0,1.0,1.0,'c');
		vSlider.addElement(slider);
		slider = new PSlider("Amb_Green",0.0,1.0,1.0,'c');
		vSlider.addElement(slider);
		slider = new PSlider("Amb_Blue",0.0,1.0,1.0,'c');
		vSlider.addElement(slider);
		slider = new PSlider("Diff_Red",0.0,1.0,1.0,'n');
		vSlider.addElement(slider);
		slider = new PSlider("Diff_Green",0.0,1.0,1.0,'c');
		vSlider.addElement(slider);
		slider = new PSlider("Diff_Blue",0.0,1.0,1.0,'c');
		vSlider.addElement(slider);
		slider = new PSlider("Spec_Red",0.0,1.0,1.0,'n');
		vSlider.addElement(slider);
		slider = new PSlider("Spec_Green",0.0,1.0,1.0,'c');
		vSlider.addElement(slider);
		slider = new PSlider("Spec_Blue",0.0,1.0,1.0,'c');
		vSlider.addElement(slider);
		slider = new PSlider("Shininess",0,100,50,'n');
		vSlider.addElement(slider);
		slider = new PSlider("Emis_Red",0.0,1.0,0.0,'n');
		vSlider.addElement(slider);
		slider = new PSlider("Emis_Green",0.0,1.0,0.0,'c');
		vSlider.addElement(slider);
		slider = new PSlider("Emis_Blue",0.0,1.0,0.0,'c');
		vSlider.addElement(slider);
	}
	// End of the vary part.

	
	
	/**
	 * Procedure to create the vector of panels that will be used when
	 * locating sliders on the frame.
	 */
	void createPanelVector() {
		int i = 0;
		JPanel panel;
		for (i = 0; i < ROWS; i++) {
			panel = new JPanel();
			panel.setLayout(new GridLayout(1,0));
			vSubPanel.addElement(panel);
		}
	}
	
	
	public Material() {
		super(windowTitle);
		int i;
		int currentPanel; // Value to specify which is the current panel in the vector
		JPanel subPanel = new JPanel(); // value used when adding sliders to the frame
		Runtime program = Runtime.getRuntime();
		Container content = getContentPane();
		
		createSliderVector();
		createPanelVector();
		/**
		 * Get current OS of the system.
		 */
		if (System.getProperty("os.name").startsWith("Windows"))
			isWindows = true;
		else 
			isWindows = false;
		/**
		 * Set interactive buttons for the user interface
		 */
		JButton exeButton = new JButton("Execute");
		exeButton.setToolTipText("Re-execute the program");
		JButton resetButton = new JButton("Reset");
		resetButton.setToolTipText("Reset Value");
		JButton exitButton = new JButton("Exit");
		exitButton.setToolTipText("Exit the Program");

		if (COMPONENTS == 0) {
			exeButton.setVisible(false);
			resetButton.setVisible(false);
		}
		
		JPanel mainPanel = new JPanel();
		JPanel subPanel1 = new JPanel();
		JPanel subPanel2 = new JPanel();

		//JScrollPane scrollPane = new JScrollPane(); // main text pane with scroll bars
		content.add(mainPanel, BorderLayout.SOUTH);
		mainPanel.setLayout(new  BoxLayout(mainPanel, BoxLayout.Y_AXIS));
		mainPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
		
		content.add(scrollPane, BorderLayout.CENTER);
		scrollPane.setBorder(BorderFactory.createLoweredBevelBorder());
		scrollPane.getViewport().add(textPane);
		
		mainPanel.add(subPanel1);
		mainPanel.add(subPanel2);
		subPanel1.setLayout(new GridLayout(0,1));
		/**
		 * Add subPanel elements to the subPanel1, each would be a row of sliders
		 */
		for (i = 0; i < ROWS; i++)
			subPanel1.add((JPanel)vSubPanel.elementAt(i));
		/**
		 *  Set the first element in the Panel Vector as the current subPanel.
		 */
		currentPanel = 0; 
		subPanel = (JPanel)vSubPanel.elementAt(currentPanel);
		/**
		 *  Allocate sliders to the sub-panels.
		 */
		for (i = 0; i < COMPONENTS; i++)  {
			PSlider slider = (PSlider)vSlider.elementAt(i);
			if (slider.getResideValue() == 'n') {
				currentPanel += 1;
				subPanel = (JPanel)vSubPanel.elementAt(currentPanel);
			}
			subPanel.add((PSlider)vSlider.elementAt(i));
		}

		/**
		 *  Set and view the source code on the frame
		 */
		textPane.setEditable(false);
		//textPane.setContentType("text/html; charset=EUC-JP");
		
		subPanel2.setLayout(new GridLayout(0,3));
		subPanel2.add(exeButton);
		exeButton.addActionListener (new ActionListener() {
			public void actionPerformed (ActionEvent e) {
				doExeCommand();
			}
			});
		subPanel2.add(resetButton);
		resetButton.addActionListener (new ActionListener() {
			public void actionPerformed (ActionEvent e) {
				doResetCommand();
			}
			});
		subPanel2.add(exitButton);
		exitButton.addActionListener (new ActionListener() {
			public void actionPerformed (ActionEvent e) {
				doExitCommand();
			}
			});
			
		/**
		 * Run the illustrated program
		 */
		try {
			pid = program.exec(cmd);
			if (isWindows == false) {
				Process pid2 =  null;
				pid2 = program.exec("getpid " + cmd.substring(4));
			}
			
		}
		catch (IOException ie) {
			System.err.println("Couldn't run " + ie);
			//System.exit(-1);
		}
		/**
		 *  Set the initial status for the window
		 */

		addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) { 
				File fp = new File(dataFile);
				//doResetCommand();
				boolean delete = fp.delete();
				pid.destroy();		  
			}
			});
		
		doExeCommand();
		setSize(WIDTH, HEIGHT);
		setLocation(500,0);
		setVisible(true);
	}
	
	void doSourceFileUpdate() {
		BufferedReader bufferIn = null;
		String str1 = new String();
		String strContent = new String();
		String sliderValue = new String();
		String newLine = System.getProperty("line.separator");
		StringBuffer strBuf = new StringBuffer();
		RandomAccessFile access = null;
		String tempFile = "temp" + round + ".html";
		File fp = new File(tempFile);
		
		int posFound = 0;
		int i = 0;
		PSlider slider; 
		
		//round += 1;
		// Read the original source file to input buffer
		try {
			bufferIn = new BufferedReader(new FileReader(exampleSource));
		}
		catch (FileNotFoundException fe) {
			System.err.println("Example Source File not found " + fe);
			//System.exit(-1);
		}
		//get the first line of the buffer.
		try {
			str1 = bufferIn.readLine();
		}
		catch (IOException ie) {
			System.err.println("Error reading line from the buffer " + ie);
			//System.exit(-1);
		}
		// Transfer the whole content of the input buffer to the string
		try {
			do 
				strContent += str1 + newLine; 
			while ((str1 = bufferIn.readLine()) != null);
		}
		catch (IOException ie) {
			System.err.println("Error readding content of the input buffer " + ie);
			//System.exit(-1);
		}
		//do the replacement.
		
		for (i = COMPONENTS; i > 0; i--) {
			// get the current value of slider
			slider = (PSlider)vSlider.elementAt(i-1);
			sliderValue = slider.getValue();
			// construct the search string
			str1 = "JPOT$" + i ;
			// get the position of the search string in the content string.
			while ((posFound = strContent.indexOf(str1)) != -1) {
			strBuf = new StringBuffer(strContent);
			strBuf.replace(posFound, posFound + str1.length(), sliderValue);
			strContent = new String(strBuf);
			}
		}
		
		boolean delete = fp.delete();

		try {
			fp.createNewFile();
		}
		catch (IOException ie) {
			System.err.println("Couldn't create the new file " + ie);
			//System.exit(-1);
		}
		
		try {
			access = new RandomAccessFile(fp, "rw");
		}
		catch (IOException ie) {
			System.err.println("Error in accessing the file " + ie);
			//System.exit(-1);
		}

		try {
			access.writeBytes(strContent);
			access.close();
		}
		catch (IOException ie) {
			System.err.println("Error in writing to file " + ie);
			//System.exit(-1);
		}
		try {
			textPane.getDocument().putProperty(Document.StreamDescriptionProperty,null); 
			URL url = new URL("file:"+tempFile + "#gohere");
			textPane.setPage(url);
			delete = fp.delete();
		}
		catch (IOException ie) {
			System.err.println("Can not get the example html file " + ie);
			//System.exit(-1);
		}
		
		
	}
	
	void doResetCommand() {
		int i;
		PSlider slider;
		for (i = 0; i < COMPONENTS; i++) {
			slider = (PSlider)vSlider.elementAt(i);
			if (slider.isInteger == true)
				slider.setValue(slider.getIntDefaultValue());	
			else
				slider.setValue(slider.getFloatDefaultValue());	
		}
		doExeCommand();
	}
	
	void doExitCommand() {
		File fp = new File(dataFile);
		boolean delete = fp.delete();
		pid.destroy();
		doResetCommand();
		setVisible(false);
	}
	
	void doExeCommand() {
		JViewport viewport = scrollPane.getViewport();
		String strContent = new String();
		PSlider slider;
		int i;
		File fp = new File(dataFile);
		
		RandomAccessFile access = null;
		Runtime program = Runtime.getRuntime();
		String cmdTrigger = "trigger";

		boolean delete = fp.delete();

		try {
			fp.createNewFile();
		}
		catch (IOException ie) {
			System.err.println("Couldn't create the new file " + ie);
			//System.exit(-1);
		}
		
		try {
			access = new RandomAccessFile(fp, "rw");
		}
		catch (IOException ie) {
			System.err.println("Error in accessing the file " + ie);
			//System.exit(-1);
		}
		
		for (i = 0; i < COMPONENTS; i++) {
			slider = (PSlider)vSlider.elementAt(i);
			/* Modified on March 20th to satisfy advisors' new request */
			if (slider.isString == true)
				strContent = strContent + slider.getRealStringValue() + " ";
			else
				strContent = strContent + slider.getValue() + " ";
		}
		
		try {
			access.writeBytes(strContent);
			access.close();
		}
		catch (IOException ie) {
			System.err.println("Error in writing to file " + ie);
			//System.exit(-1);
		}
		// Trigger the OpenGL program to update with new values
		try {
			Process pid = program.exec(cmdTrigger);
		}
		catch (IOException ie) {
			System.err.println("Couldn't run " + ie);
			//System.exit(-1);
		}
		doSourceFileUpdate();
	}
}

