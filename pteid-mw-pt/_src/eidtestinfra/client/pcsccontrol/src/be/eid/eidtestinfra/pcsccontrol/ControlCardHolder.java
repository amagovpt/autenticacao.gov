package be.eid.eidtestinfra.pcsccontrol;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;

import be.eid.eidtestinfra.pcsccontrol.Control.Virtualcard;
import be.eid.eidtestinfra.pcsccontrol.gui.Toolkit;

/**
 * ControlCardHolder contains a single {@link Virtualcard} as well as a list of {@link CardHolder}
 * which hard chip number matches that of the Virtualcard.
 * 
 * @author Rogier Taal
 * 
 */
public class ControlCardHolder {	
	private Control.Virtualcard controlCard = new Control.Virtualcard();
	private List<CardHolder> cards = new ArrayList<CardHolder>();
	
	/** Comparator for a ControlCardHolder that compares on hard chip number alone. */
	public static final Comparator<ControlCardHolder> HARDCHIPNR_COMPARATOR = new HardChipnrComparator();
	
	public ControlCardHolder(String hardChipnr) {
		controlCard.setHardchipnr(hardChipnr);
	}

	/**
	 * @return the number of containing CardHolders
	 */
	public int size() {
		return cards.size();
	}
	
	/**
	 * @param i
	 * @return the CardHolder on the given position 
	 */
	public CardHolder get(int i) {
		return cards.get(i);
	}
	
	/**
	 * Adds the given CardHolder. If the CardHolder is already contains a CardHolder with
	 * the same file then it will not be added again.
	 * @param chAdd
	 */
	public void add(CardHolder chAdd) {
		for(CardHolder chListed : cards) {
			boolean equals = (Toolkit.isWindows ? chAdd.getFile().equalsIgnoreCase(chListed.getFile())
					: chAdd.getFile().equals(chListed.getFile()));
			if(equals) {
				return;
			}
		}
		cards.add(chAdd);
	}
	
	/**
	 * 
	 * @return the hard chip number which can never be null
	 */
	public String getHardchipnr() {
		return controlCard.hardchipnr;
	}
	
	/**
	 * 
	 * @return the file or null
	 */
	public String getFile() {
		return controlCard.file;
	}
	
	/**
	 * 
	 * @param file or null
	 */
	public void setFile(String file) {
		controlCard.setFile(file);
	}
	
	/**
	 * Comparator for a ControlCardHolder that compares on hard chip number alone.
	 * @author Rogier Taal
	 *
	 */
	private static class HardChipnrComparator implements Comparator<ControlCardHolder> {
		public int compare(ControlCardHolder o1, ControlCardHolder o2) {
			return o1.controlCard.hardchipnr.compareTo(o2.controlCard.hardchipnr);
		}		
	}
}
