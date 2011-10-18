package be.eid.eidtestinfra.pcsccontrol;

/**
 * This class contains Visibility constants. Visibility is the attribute set inside the control file.
 * 
 * @author Rogier Taal
 * 
 */
public class Visibility {
	
	private static final String REAL_FIRST_VAL = "REAL_FIRST";
	private static final String REAL_LAST_VAL = "REAL_LAST";
	private static final String HIDE_REAL_VAL = "HIDE_REAL";
	private static final String HIDE_VIRTUAL_VAL = "HIDE_VIRTUAL";
		
	public static final Visibility REAL_FIRST = new Visibility(REAL_FIRST_VAL);
	public static final Visibility REAL_LAST = new Visibility(REAL_LAST_VAL);
	public static final Visibility HIDE_REAL = new Visibility(HIDE_REAL_VAL);
	public static final Visibility HIDE_VIRTUAL = new Visibility(HIDE_VIRTUAL_VAL);

	private String val;
	
	private Visibility() {
	}
	
	private Visibility(String val) {
		this.val = val;
	}
	
	public static Visibility get(String val) {
		if(REAL_FIRST_VAL.equals(val))
			return REAL_FIRST;
		if(REAL_LAST_VAL.equals(val))
			return REAL_LAST;
		if(HIDE_REAL_VAL.equals(val))
			return HIDE_REAL;
		if(HIDE_VIRTUAL_VAL.equals(val))
			return HIDE_VIRTUAL;

		return null;
	}
	
	public String getValue() {
		return val;
	}
	
	public int hashCode() {
		return 31*val.hashCode()+1;
	}
	
	public boolean equals(Object anObject) {
		if(this == anObject)
			return true;
		return this.hashCode() == ((Visibility)anObject).hashCode();
	}
}
