package be.eid.eidtestinfra.pcsccontrol.gui;

import java.awt.event.ActionEvent;
import java.io.ByteArrayInputStream;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.util.Arrays;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import javax.swing.AbstractAction;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;

import be.eid.eidtestinfra.pcsccontrol.Log;

/**
 * The unblock procedure is as follows:<br>
 * <ol>
 * <li>Test if card is a test card. If not a test card an error is shown.</li>
 * <li>Validate the puk code. If puk is not valid an error is shown.</li>
 * <li>Reset PIN. If failed an error is shown. If successful a message that indicates success is shown</li>
 * </ol>
 * 
 * @since 1.6
 * @author Rogier Taal
 * 
 */
public class CardUnblockAction extends AbstractAction implements Runnable {
	
	private static final long serialVersionUID = 1L;
	public static final String NAME = "ACTION_CardUnblock";
	private Main main;
	private String currentRdr;
	
	public CardUnblockAction(Main main) {
		super(NAME);
		this.main = main;
	}
	
	public void actionPerformed(ActionEvent e) {
		String rdr = e.getActionCommand();
		synchronized (this) {
			if(rdr.equals(currentRdr)) {
				return;
			}
			currentRdr = rdr;
		}
		
		final Runnable unblockCard = this;
		new Thread(new Runnable() {
			public void run() {
				try {
					main.getSession().runWithBusyCursor(unblockCard);
				} finally {
					synchronized (unblockCard) {
						currentRdr = null;
					}
				}
			}
		}).start();
	}
	
	public synchronized void run() {
		if (Log.logger != null)
			Log.logger.debug("Started CardUnblockAction thread");
		main.getSession().getCardReaderMonitor().pause();
		CountDownLatch l1 = new CountDownLatch(1);
		Thread t = startDaemon(l1);
		boolean timeout = false;
		try {
			timeout = !l1.await(15, TimeUnit.SECONDS);
		} catch(InterruptedException ie) {
			t.interrupt();
			return;
		}
		if(t.isAlive() && timeout) {
			t.interrupt();
			Toolkit.showErrorDialog(main.getFrame(), "No response recieved from card");
		}
		main.getSession().getCardReaderMonitor().resume();
		if (Log.logger != null)
			Log.logger.debug("Ending CardUnblockAction thread");
	}
	
	private Thread startDaemon(final CountDownLatch l1) {
		Thread t = new Thread(new Runnable() {
			public void run() {
				CardSession cs = main.getSession().getCardReaderMonitor().createCardSession(currentRdr);
				boolean connected = false;
				try {
					cs.connect();
					connected = true;
					unblock(cs);
				} catch(Exception e) {
					Toolkit.showErrorDialog(main.getFrame(), e);
				} finally {
					if(connected) {
						try {
							cs.disconnect();
						} catch(Exception ignored) {
						}
					}
					l1.countDown();
				}
			}
		});
		t.setDaemon(true);
		t.start();
		return t;
	}
	
	private void unblock(CardSession cs) throws Exception {
		if(isTestCard(cs)) {
			if(!isValidPuk(cs)) {
				Toolkit.showErrorDialog(main.getFrame(), "PUK is incorrect");
				return;
			}
			if(!resetPin(cs)) {
				Toolkit.showErrorDialog(main.getFrame(), "Failed to reset PIN");
				return;
			}
			SwingUtilities.invokeLater(new Runnable() {
				public void run() {
					String msg = "PIN was successfully unblocked and set to 1234";
					JOptionPane.showMessageDialog(main.getFrame(), msg, "PIN unblocked", JOptionPane.INFORMATION_MESSAGE);						
				}					
			});
		} else {
			Toolkit.showErrorDialog(main.getFrame(), "Only test cards can be unblocked.");
		}
	}
		
	private static final byte[] SELECT_FILE = {0x00,(byte)0xA4,0x08,0x0C,0x06,0x3F,0x00,(byte)0xDF,0x00,0x50,0x3B};
	private static final byte[] VERIFY_PUK = {0x00,0x20,0x00,0x02,0x08,0x2C,0x33,0x33,0x33,0x11,0x11,0x11,(byte)0xFF};
	private static final byte[] PIN_RESET = {0x00,0x24,0x01,0x01,0x08,0x24,0x12,0x34,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF};
	private static final byte[] READ_BINARY = {0x00,(byte)0xB0,0x00,0x00,0x00};
	private static final byte[] OK_RESPONSE = {(byte)0x90,0x00};
	private static final String TEST_CERT_AUTH = "eID test Root CA";
	
	private static boolean isValidPuk(CardSession cs) throws Exception {
		byte[] ret = cs.transmit(VERIFY_PUK);
		return Arrays.equals(ret, OK_RESPONSE);
	}
	
	private static boolean resetPin(CardSession cs) throws Exception {
		byte[] ret = cs.transmit(PIN_RESET);
		return Arrays.equals(ret, OK_RESPONSE);
	}
	
	private static boolean isTestCard(CardSession cs) throws Exception {
		cs.transmit(SELECT_FILE);
		byte[] read_bin = new byte[READ_BINARY.length];
		byte[] certBytes = null;
		int bufLen = 0;
		int le = 256;
		int la = 0;
		boolean first = true;
		
		while(le > 0) {			
			System.arraycopy(READ_BINARY, 0, read_bin, 0, read_bin.length);
			read_bin[2] = (byte) (bufLen >> 8);
			read_bin[3] = (byte) (bufLen & 255);
			read_bin[4] = (byte) le;
			byte[] ret = cs.transmit(read_bin);
			if(first) {
				first = false;
				la = (4 + 256 * (int)(ret[2]&0xFF) + (int)(ret[3]& 0xFF));
				certBytes = new byte[la];
			}
			int copyLen = (la > le ? le : la);
			System.arraycopy(ret, 0, certBytes, bufLen, copyLen);
			bufLen += copyLen;
			le = (la - bufLen > 256 ? 256 : la - bufLen);
		}
		
		X509Certificate cert;
		
		try {
			CertificateFactory cf = CertificateFactory.getInstance("X509");
			cert = (X509Certificate) cf.generateCertificate(new ByteArrayInputStream(certBytes));
		} catch(CertificateException ce) {
			return false;
		}
		
		String dn = cert.getSubjectX500Principal().toString();
		if (Log.logger != null)
			Log.logger.debug("isTestCard(): card's root cert DN = " + dn);
		return (dn.indexOf(TEST_CERT_AUTH) != -1);
	}
}
