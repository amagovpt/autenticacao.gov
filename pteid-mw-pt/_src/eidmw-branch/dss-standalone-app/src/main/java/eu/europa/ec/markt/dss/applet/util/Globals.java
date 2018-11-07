package eu.europa.ec.markt.dss.applet.util;

import java.util.Locale;

public class Globals {
	
	private static Globals _instance = new Globals();
	private Locale my_locale;
	
	private Globals()
	{
		
	}
	
	public static Globals getInstance()
	{
		return _instance;
	}
	
	public synchronized void setLocale(String language_code)
	{ 
		my_locale = new Locale(language_code);
	}
	
	public Locale getLocale() { return my_locale; };

}
