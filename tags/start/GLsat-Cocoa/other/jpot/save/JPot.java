import jpot.*;
import java.io.*;
import java.lang.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.event.TreeSelectionListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.tree.TreeSelectionModel;
import java.net.URL;
import java.io.IOException;
import javax.swing.JEditorPane;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JFrame;


public class JPot extends JFrame {
	private HtmlPanel htmlPane ;
	
	public JPot() {
		super("Personal OpenGL Tutorial");
        // Create the nodes.
        DefaultMutableTreeNode top = new DefaultMutableTreeNode("Personal OpenGL Tutorial");
        NodeTreeCreate(top);

        // Create a tree that allows one selection at a time.
        final JTree tree = new JTree(top);
        tree.getSelectionModel().setSelectionMode
                (TreeSelectionModel.SINGLE_TREE_SELECTION);
	
		// Listen for when the selection changes
		tree.addTreeSelectionListener(new TreeSelectionListener() {
			public void valueChanged(TreeSelectionEvent e) {
				DefaultMutableTreeNode node = (DefaultMutableTreeNode)tree.getLastSelectedPathComponent();
				if (node == null) return;
				
				Object nodeInfo = node.getUserObject();
				if (node.isLeaf()) {
					LectureNote lecture = (LectureNote)nodeInfo;
					htmlPane.displayURL(lecture.lectureURL);
					//System.out.println(lecture.lectureURL);
				}
			}
			});
			
		// Create the scroll pane and add the tree to it. 
        JScrollPane treeView = new JScrollPane(tree);
		//getContentPane().add(treeView, BorderLayout.CENTER);
		
		// Create the HTML viewing pane
		htmlPane = new HtmlPanel();
		
		// Add the htmlPane to the split pane
		JSplitPane splitPane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT);
		splitPane.setLeftComponent(treeView);
		splitPane.setRightComponent(htmlPane);
		Dimension minimumSize = new Dimension(100, 50);
		htmlPane.setMinimumSize(minimumSize);
		treeView.setMinimumSize(minimumSize);
		splitPane.setDividerLocation(150);
		splitPane.setPreferredSize(new Dimension(600,400));
		
		// Add the split pane to the frame
		getContentPane().add(splitPane, BorderLayout.CENTER);

	}

	private void NodeTreeCreate(DefaultMutableTreeNode top) {
		FileInputStream configFile = null;
		String str, chapter = null, lesson, lessonPage,temp;
		StringTokenizer token;
		boolean beginChapter = true;
		
		DefaultMutableTreeNode chapterNode = null;
		DefaultMutableTreeNode lessonNode = null;
		
		// Open the configuration file to get the config information
		// about the chapter/lesson structure.
		try {
			configFile = new FileInputStream("ConfigFile");
		}
		catch (IOException ie) {
			System.err.println("Can't open ConfigFile " + ie);
			System.exit(1);
		}
		// Read the file line by line. For each line parse the string
		// to get the fields and build the data structure for the tree
		BufferedReader buffer = new BufferedReader(new InputStreamReader(configFile));
		while (true) {
			try {
				str = buffer.readLine();
				if (str == null) break;
			}
			catch (IOException ie) {
				System.err.println("Error reading ConfigFile" + ie);
				break;
			}

			// Tokening the string to parts.
			token = new StringTokenizer(str, "\t");
			if (beginChapter == true) {
				chapter = token.nextToken();
				chapterNode = new DefaultMutableTreeNode(chapter);
				top.add(chapterNode);
				beginChapter = false;
			}
			else {
				temp = token.nextToken();
				if (!chapter.equalsIgnoreCase(temp)) {					
					chapter = temp;
					chapterNode = new DefaultMutableTreeNode(chapter);
					top.add(chapterNode);
				}	
			}
			lesson		= token.nextToken();
			lessonPage	= token.nextToken();
			lessonNode = new DefaultMutableTreeNode(new LectureNote(lesson, lessonPage));
			chapterNode.add(lessonNode);
		}
		
		// Close the ConfigFile
		try {
			configFile.close();
		}
		catch (IOException ie) {
			System.err.println("Error closing ConfigFile" + ie);
			System.exit(1);
		}
	}
	
	private class LectureNote {
		public String lectureName;
		public URL lectureURL;
		public String prefix = "file:"
			+ System.getProperty("user.dir")
			+ System.getProperty("file.separator") 
			+ "html" + System.getProperty("file.separator");
		
		public LectureNote(String lecture, String filename) {
			lectureName = lecture;
			try {
				lectureURL = new URL(prefix + filename);
			}
			catch (java.net.MalformedURLException except) {
				System.err.println("Bad lecture note's URL: " + except);
				lectureURL = null;
			}
		}
		
		public String toString() {
			return lectureName;
		}
	}
	
	public static void main (String argc[]) {
		JFrame jpot = new JPot();
		jpot.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) {
				Runtime program = Runtime.getRuntime();
				Process pid = null;
				
				if (System.getProperty("os.name").startsWith("Windows")== false) {
					try {
						pid = program.exec("trigger quit");
					}
					catch (IOException ie) {
						System.err.println("On exit: Can't delete logfile " + ie);
						System.exit(-1);
					}
				}	   
				System.exit(0);
			}
        });  
		jpot.pack();
		jpot.setVisible(true);
	}
}
