package be.eid.eidtestinfra.pcsccontrol.gui.jpcsc;

import com.linuxnet.jpcsc.Apdu;
import com.linuxnet.jpcsc.Card;
import com.linuxnet.jpcsc.Context;
import com.linuxnet.jpcsc.PCSC;

import be.eid.eidtestinfra.pcsccontrol.gui.CardSession;
import be.eid.eidtestinfra.pcsccontrol.Log;

/**
 * JPCSC implementation of {@link CardSession}.
 * 
 * @author Rogier Taal
 */
public class CardSessionImpl implements CardSession {	
	private Context ctx;
	private String rdr;
	private Card card;
	
	public CardSessionImpl(Context ctx, String rdr) {
		this.ctx = ctx;
		this.rdr = rdr;
	}

	public void connect() throws Exception {
		card = ctx.Connect(rdr, PCSC.SHARE_SHARED, PCSC.PROTOCOL_T0);
		if (Log.logger != null)
			Log.logger.debug("Connected to card in reader " + rdr);
	}

	public void disconnect() throws Exception {
		card.Disconnect();
		if (Log.logger != null)
			Log.logger.debug("Disconnected to card in reader " + rdr);
	}

	public byte[] transmit(byte[] apdu) throws Exception {
		if (Log.logger != null)
			Log.logger.debug("APDU to card: " + Log.toHex(apdu));
		byte[] ret = card.Transmit(new Apdu(apdu));
		if (Log.logger != null)
			Log.logger.debug("  Response from card: " + Log.toHex(ret));
		return ret;
	}
}
