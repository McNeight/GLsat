import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.io.*;
import java.util.*;    
import java.net.URL;

public class UITemplate extends JFrame {
	static int WIDTH = 500;
	static int HEIGHT = 600;
	static Process pid = null;	
	static boolean isWindows = true;
	/**
	 *  pane that contains the source code
	 */
	JEditorPane textPane = new JEditorPane(); 
	JScrollPane scrollPane = new JScrollPane(); // main text pane with scroll bars
	
	static final String windowTitle = 
	/**
	 * Name of the OpenGL example source program
	 */
	static String exampleSource = "html" + System.getProperty("file.separator") + 
	/**
	 * Name of the executable file illustrates for the example.
	 */
	String cmd = "ogl" + System.getProperty("file.separator") + 
	// End of the vary part.

		
	public UITemplate() {
		super(windowTitle);
		int i;
	
		Runtime program = Runtime.getRuntime();
		Container content = getContentPane();
		
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
		JButton exitButton = new JButton("Exit");
		exitButton.setToolTipText("Exit the Program");

		JPanel mainPanel = new JPanel();
	
		content.add(mainPanel, BorderLayout.SOUTH);
		mainPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
		
		content.add(scrollPane, BorderLayout.CENTER);
		scrollPane.setBorder(BorderFactory.createLoweredBevelBorder());
		scrollPane.getViewport().add(textPane);
		
		mainPanel.add(exitButton, BorderLayout.CENTER);
	
		/**
		 *  Set and view the source code on the frame
		 */
		textPane.setEditable(false);
		textPane.setContentType("text/html; charset=EUC-JP");
		try {
			URL url = new URL("file:" + exampleSource);
			textPane.setPage(url);	
		}
		catch (IOException ie) {
			System.err.println("Error in opening html source file " + ie);
			System.exit(-1);
		}
		
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
		}
		catch (IOException ie) {
			System.err.println("Couldn't run " + ie);
			System.exit(-1);
		}
		/**
		 *  Set the initial status for the window
		 */
		addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) { 
				pid.destroy();		  
			}
			});
		setSize(WIDTH, HEIGHT);
		setLocation(500,0);
		setVisible(true);
	}
	
	
	void doExitCommand() {
		pid.destroy();
		setVisible(false);
	}
	
}
