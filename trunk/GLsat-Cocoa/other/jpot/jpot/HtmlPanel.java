package jpot;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.text.*;
import javax.accessibility.*;

import java.awt.*;
import java.util.*;
import java.net.URL;
import java.net.MalformedURLException;
import java.io.IOException;

public class HtmlPanel extends JPanel implements HyperlinkListener {
    JEditorPane html;
	static Process pid = null;

    public HtmlPanel() {
	//setBorder(swing.emptyBorder10);
    setLayout(new BorderLayout());
	getAccessibleContext().setAccessibleName("HTML panel");
	getAccessibleContext().setAccessibleDescription("A panel for viewing HTML documents, and following their links");
	
	try {
           URL url = null;
		   
		   String prefix = "file:" + System.getProperty("user.dir") +
                   System.getProperty("file.separator") + "html" + System.getProperty("file.separator");
            try {
                url = new URL(prefix + "FrontPage.html");
            } catch (java.net.MalformedURLException exc) {
                   System.err.println("Attempted to open example.html "
                                      + "with a bad URL: " + url);
                   url = null;
            }
       
            if (url != null) {
                html = new JEditorPane(url);
                html.setEditable(false);
                html.addHyperlinkListener(this);
                JScrollPane scroller = new JScrollPane();
                //scroller.setBorder(swing.loweredBorder);
                JViewport vp = scroller.getViewport();
                vp.add(html);
                vp.setBackingStoreEnabled(true);
                add(scroller, BorderLayout.CENTER);
            }
	} catch (MalformedURLException e) {
	    System.out.println("Malformed URL: " + e);
	} catch (IOException e) {
	    System.out.println("IOException: " + e);
	}
    }

    /**
     * Notification of a change relative to a 
     * hyperlink.
     */
    public void hyperlinkUpdate(HyperlinkEvent e) {
	if (e.getEventType() == HyperlinkEvent.EventType.ACTIVATED) {
	    linkActivated(e.getURL());
	}
    }

    /**
     * Follows the reference in an
     * link.  The given url is the requested reference.
     * By default this calls <a href="#setPage">setPage</a>,
     * and if an exception is thrown the original previous
     * document is restored and a beep sounded.  If an 
     * attempt was made to follow a link, but it represented
     * a malformed url, this method will be called with a
     * null argument.
     *
     * @param u the URL to follow
     */
    protected void linkActivated(URL u) {
	Cursor c = html.getCursor();
	Cursor waitCursor = Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR);
	html.setCursor(waitCursor);
	SwingUtilities.invokeLater(new PageLoader(u, c));
    }

	/**
	 * set the new URL for viewing on the pane
	 */
	public void displayURL(String str) {
		try {
			html.setPage(str);
		}
		catch (IOException e) {
			System.err.println("Attempt to read a bad URL: " + str);
		}
	}
	
	public void displayURL(URL url) {
		try {
			html.setPage(url);
		}
		catch (IOException e) {
			System.err.println("Attempt to read a bad URL: " + url);
		}
	}
	
    /**
     * temporary class that loads synchronously (although
     * later than the request so that a cursor change
     * can be done).
     */
    class PageLoader implements Runnable {
		
		PageLoader(URL u, Cursor c) {
			url = u;
			cursor = c;
		}
		
		private String getClassName(String str) {
			String _getResult = "";
			//String _delim = System.getProperty("file.separator");
			String _delim = "/";
			// First get the last string that is the class of the Java Illustrated program.
			StringTokenizer _strToken = new StringTokenizer(str, _delim);
			while (_strToken.hasMoreTokens())
				_getResult = _strToken.nextToken();
			// System.out.println("_getResult final is " + _getResult);
			// Cut the prefix .class out of the final string then return it.
			return (_getResult.substring(0, _getResult.length() - 6)); //".class" = 6
		}
		
        public void run() {
			if (url == null) {
				// restore the original cursor
				html.setCursor(cursor);
				// PENDING(prinz) remove this hack when 
				// automatic validation is activated.
				Container parent = html.getParent();
				parent.repaint();
			} else  {
				if (url.toString().endsWith("class")) {				
					//System.out.println("Link to java class application");
					Runtime program = Runtime.getRuntime();
					String cmd = "java " + getClassName(url.getFile());
					/*
					if (pid != null) pid.destroy();
					try {
						pid = program.exec(cmd);
					}
					catch (IOException ie){
						System.out.println("IOException " + ie);
						//System.exit(-1);
					}
					/*/
					Class cls = null;	
					try {
						cls = Class.forName(getClassName(url.getFile()));
					}
					catch (ClassNotFoundException ce) {
						System.out.println("ClassNotFoundException " + ce);
						//System.exit(-1);
					}
					try {
							Object obj = cls.newInstance();
					}
					catch (InstantiationException ie)  {
						System.out.println(ie );
						//System.exit(-1);
					}
					catch (IllegalAccessException ie) {
						System.out.println(ie);
						//System.exit(-1);
					}
					
					url = null;
					SwingUtilities.invokeLater(this);
				}else if (url.toString().startsWith("file:")){
					Document doc = html.getDocument();
					try {
						html.setPage(url);	
					} catch (IOException ioe) {
						html.setDocument(doc);
						getToolkit().beep();
					} finally {
						// schedule the cursor to revert after
						// the paint has happended.
						url = null;
						SwingUtilities.invokeLater(this);
					}
				}
			}
		}
		URL url;
		Cursor cursor;
	}
}
