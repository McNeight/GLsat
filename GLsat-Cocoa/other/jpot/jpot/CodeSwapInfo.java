package jpot;
import java.lang.*;
import java.util.*;

public class CodeSwapInfo {
    boolean hasCodeSwap;
    
    String firstLabel ;
    String secondLabel;
    String firstText;
    String secondText;
    

    public CodeSwapInfo() {
	hasCodeSwap = false;
	firstLabel = new String();
	secondLabel = new String();
	firstText = new String();
	secondText = new String();
    }

    public void setCodeSwap() {
	hasCodeSwap = true;
    }

    public boolean isCodeSwap() {
	return hasCodeSwap;
    }

    public void setFirstLabel(String label) {
	firstLabel = label;
    }

    public void setSecondLabel(String label) {
	secondLabel = label;
    }

    public void setFirstText(String text) {
	firstText = text;
    }
    
    public void setSecondText(String text) {
	secondText = text;
    }
    
    public String getFirstLabel() {
	return firstLabel;
    }

    public String getSecondLabel() {
	return secondLabel;
    }

    public String getFirstText() {
	return firstText;
    }

    public String getSecondText() {
	return secondText;
    }
    

}
