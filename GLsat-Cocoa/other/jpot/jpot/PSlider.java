package jpot;
import java.util.StringTokenizer;
import java.util.Hashtable;
import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;

public class PSlider extends JPanel {
  final JSlider slider;
  final JLabel label = new JLabel();
  /**
   * scale for converting the number between doubleing point and integer
   */
  private double number = 1000.0;
  /**
   * label of the Slider when appearing on the frame
   */
  private String labelTitle = new String();
  /**
   * Default value for the slider. Integer value
   */
  private int iDefaultValue = 0;
  /**
   * Default value for the siler. doubleing point value.
   */
  private double fDefaultValue = 0.0;
  /**
   * Value to specify whether the slider will be in a new row or not
   * if 'n' then will be at the new row, and 'c' otherwise. Default is 'c'
   */
  private char resideValue = 'c';
  /**
   * Flag to specify whether the Slider is integer or floating point.
   */
  public boolean isInteger = false;
  /**
   * Flat to specify whether the Slider is String constatnt
   */
  public boolean isString = false;
  /**
   * Array to contain the content of the String Slider
   */
  private String stringValue[][] = new String[100][2];
  
  public PSlider(String title, double min, double max, double value, char at) {
    /**
     * Map the double value to the integer value for createing the slider
     */
    int imin = (int)(min*number);
    int imax = (int)(max*number);
    int ivalue = (int)(value*number);
    /** 
     * Create the slider base on the integer mapping value
     */
    slider = new JSlider(imin, imax, ivalue);
    labelTitle = title + " : ";
    label.setText(labelTitle + value);
    label.setForeground(Color.black);
    /**
     * Set the value for the reside data member.
     */
    resideValue = at;
    
    slider.addChangeListener(new ChangeListener() {
	public void stateChanged(ChangeEvent e) {
	  int currentvalue = slider.getValue();
	  label.setText(labelTitle + currentvalue/number);
	}
      });								 	
    /**
     * Create layout manager and add the components to it.
     */
    setLayout(new GridLayout(2,1));
    add(label);
    add(slider);
    /**
     * Set the value for default value.
     */
    fDefaultValue = value;
  }			
  
  public PSlider(String title, int min, int max, int value, char at) {
    /** 
     * Create the slider base on the integer mapping value
     */
    slider = new JSlider(min, max, value);
    labelTitle = title + " : ";
    label.setText(labelTitle + value);
    label.setForeground(Color.black);
    /**
     * Set the value for the reside data member.
     */
    resideValue = at;
    
    slider.addChangeListener(new ChangeListener() {
	public void stateChanged(ChangeEvent e) {
	  label.setText(labelTitle + slider.getValue());
	}
      });								 	
    /**
     * Create layout manager and add the components to it.
     */
    setLayout(new GridLayout(2,1));
    add(label);
    add(slider);
    /**
     * Set the value for default value.
     */
    iDefaultValue = value;
    isInteger = true;
  }			
  
  public PSlider(String title, String defaultValue, String values, char at) {
    /**
     * This option will share the getDefaultValue and setValue functions
     * with the integer slider. Can't do more as this is requested to added
     * after all everythis is almost done.
     * isString boolean variable is set when this object is used.
     * getValue/setValue is the same , but if want to get the REAL value of the
     * slider, then use the getRealStringValue() function. Otherwise you get the
     * "label value" instead of the real value.
     */
    int minIntValue = 0, maxIntValue = 0, defaultIntValue = 0;
    int counter = 0, numVal = 0;
    boolean data = false;
    
    resideValue = at;
    isString = true;
    
    /**
     * Parser the string to get the value at first
     */
    StringTokenizer token = new StringTokenizer(values, " ");
    while (token.hasMoreTokens()) {
      if (data == false) {
	stringValue[counter][0] = token.nextToken();
	if (defaultValue.equals(stringValue[counter][0]))
	  defaultIntValue = counter;
	data = true;
      } else {
	stringValue[counter][1] = token.nextToken();
	data = false;
	counter++;
      }
    }
    iDefaultValue = defaultIntValue;
    minIntValue = 0;
    maxIntValue = counter - 1;
    /** 
     * Create the slider base on the integer mapping value
     */
    slider = new JSlider(minIntValue, maxIntValue, defaultIntValue);
    /**
     * Construct the slider
     */
    Hashtable h = new Hashtable();
    for (counter = 0; counter <= maxIntValue; counter ++) {
      h.put(new Integer(counter), new JLabel(stringValue[counter][0]));
    }
    slider.setLabelTable(h);
    slider.setMajorTickSpacing(1);
    slider.setPaintTicks(true);
    slider.setSnapToTicks(true);
    slider.setForeground(Color.red);
    //slider.setPaintLabels(true);
    //slider.setInverted(true);
    /**
     * Set slider's title 
     */
    labelTitle = title + " : ";
    label.setText(labelTitle + stringValue[defaultIntValue][0]);
    label.setForeground(Color.black);
    /**
     * Set the value for the reside data member.
     */
    resideValue = at;
    
    slider.addChangeListener(new ChangeListener() {
	public void stateChanged(ChangeEvent e) {
	  label.setText(labelTitle + stringValue[slider.getValue()][0]);
	}
      });								 	
    /**
     * Create layout manager and add the components to it.
     */
    setLayout(new GridLayout(2,1));
    add(label);
    add(slider);
    /**
     * Set the value for default value.
     */
    iDefaultValue = defaultIntValue;
    
  }
  
  public String getValue() {
    
    if (isString == true) {
      int currentValue = slider.getValue();
      return stringValue[currentValue][0];
    }
    if (isInteger == false) {
      double currentvalue = (double)(slider.getValue()/number);
      return String.valueOf(currentvalue);
    }
    else {
      int currentvalue = slider.getValue();
      return String.valueOf(currentvalue);
    }
  }
  
  public String getRealStringValue() {
    int currentValue = slider.getValue();
    return String.valueOf(stringValue[currentValue][1]);
  }

  public void setValue(double value) {
    slider.setValue((int)(value*number));
    label.setText(labelTitle + String.valueOf(value));
  }
  
  public void setValue(int value) {
    slider.setValue(value);
    if (isString == false)
      label.setText(labelTitle + String.valueOf(value));
    else
      label.setText(labelTitle + stringValue[value][0]);
  }
  
  public double getFloatDefaultValue() {
    return fDefaultValue;
    }
  
  public int getIntDefaultValue() {
    return iDefaultValue;
  }
  
  public char getResideValue() {
    return resideValue;
  }
}	




