package jpot;
import java.lang.*;
import java.util.*;

public class ElementInfo {
    public Object	defaultValue;
    Object	minValue;
    Object	maxValue;
    public char	lineControl;
    String	label;
    public String	dataType; // specify whether it is "int", "float" or "string"
    private String stringValue =  null;

    public ElementInfo() {	
    }
    
    public void setDefaultValue(Object value) {
	defaultValue = value;
    }
    
    public void setMinValue(Object value) {
	minValue = value;
    }
    
    public void setMaxValue(Object value) {
	maxValue = value;
    }
    
    public void setLineControl(char value) {
	lineControl = value;
    }
    
    public void setLabel(String value) {
	label = value;
    }
    
    public void setDataType(String value) {
	dataType = value;
    }
    
    public void setStringValue(String value) {
	stringValue = new String(value);
    }
    
    public String getStringValue() {
	return stringValue;
    }

    public void Debug() {
	System.out.println("Label is " + label);
	System.out.println("Datatype is " + dataType);
	System.out.println("Default value is " + defaultValue);
	System.out.println("Minimum value is " + minValue);
	System.out.println("Maximum value is " + maxValue);
	System.out.println("Line control is " + lineControl);
    }
}
