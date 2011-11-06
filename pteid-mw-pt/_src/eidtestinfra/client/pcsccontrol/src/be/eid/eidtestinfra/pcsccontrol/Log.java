package be.eid.eidtestinfra.pcsccontrol;

import org.apache.log4j.*;
import java.io.*;
import java.util.Properties;

public class Log
{
	public static Logger logger = null;

	static {
		try {
			File configFile = new File("log4j.conf");
			if (configFile.exists()) {
				Properties conf = new Properties();
				conf.load(new FileInputStream(configFile));
				
				// Init the logger
				Layout layout = new PatternLayout(
					conf.getProperty("patternLayout", "%-5p %d{HH:mm:ss} [%t]: %m%n"));
				Appender appender = new DailyRollingFileAppender(layout,
					conf.getProperty("logFile", "eidtestinfra.log"),
					conf.getProperty("logFileExt", "'.'yyyy-MM-dd"));
				logger = Logger.getLogger("eID Test Infra control lib");
				logger.removeAllAppenders();
				logger.addAppender(appender);
				logger.setLevel(Level.toLevel(
					conf.getProperty("logLevel", "info")));
			}
		}
		catch (Throwable t) {
			t.printStackTrace();
			logger = null;
		}
	}

	private static final char[] HEX_TABLE =
		{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

	/**
	 * Convert the byte[] to hex
	 */
	public static String toHex(byte[] ba)
	{
		if (ba == null)
			return "<null>";

		char out[] = new char[2 * ba.length];

		for (int i = 0; i < ba.length; i++) {
			int x = ba[i] < 0 ? ba[i] + 256 : ba[i];
			out[2 * i] = HEX_TABLE[x / 16];
			out[2 * i + 1] = HEX_TABLE[x % 16];
		}

		return new String(out);
	}
}
