package be.eid.eidtestinfra.pcsccontrol.gui;

import javax.smartcardio.Card;
import javax.smartcardio.CardChannel;
import javax.smartcardio.CardTerminal;
import javax.smartcardio.CommandAPDU;
import be.eid.eidtestinfra.pcsccontrol.Log;

/**
 * Smarcardio version of {@link CardSession}.
 * 
 * @since 1.6
 * @author Rogier Taal
 * 
 */
public class CardSessionImpl implements CardSession {
	private CardTerminal term;
	private Card card = null;
	private String rdr;
	
	public CardSessionImpl(CardTerminal term, String rdr) {
		this.term = term;
		this.rdr = rdr;
	}

	public void connect() throws Exception {
		if(term == null)
			throw new IllegalStateException(rdr + " (Unknown)");
		card = term.connect("T=0");
		if (Log.logger != null)
			Log.logger.debug("Connected to card in reader " + term);
	}		

	public void disconnect() throws Exception {
		card.disconnect(false);				
		if (Log.logger != null)
			Log.logger.debug("Disconnected to card in reader " + term);
	}

	public byte[] transmit(byte[] apdu) throws Exception {
		CardChannel chan = card.getBasicChannel();
		if (Log.logger != null)
			Log.logger.debug("APDU to card: " + Log.toHex(apdu));
		byte[] ret = chan.transmit(new CommandAPDU(apdu)).getBytes();
		if (Log.logger != null)
			Log.logger.debug("  Response from card: " + Log.toHex(ret));
		return ret;
	}
}
