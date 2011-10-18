package be.eid.eidtestinfra.pcsccontrol;


/**
 * CardHolder contains a single {@link Card} and the file path for the Card.
 * 
 * @author Rogier Taal
 * 
 */
public class CardHolder {
	
	private Card card;
	private String file;
	
	/**
	 * 
	 * @param card or null
	 * @param file path for the given card
	 * @throws NullPointerException when file is null
	 */
	public CardHolder(Card card, String file) {
		if(file == null)
			throw new NullPointerException("file may not be null");
		this.card = card;
		this.file = file;
	}
	
	/**
	 * 
	 * @return true when the contained Card contains valid data
	 */
	public boolean containsValidCard() {
		return (card != null);
	}
	
	/**
	 * 
	 * @return
	 */
	public String getFile() {
		return file;
	}
	
	/**
	 * @see Card#getChipNumber()
	 * @return
	 */
	public String getChipNumber() {
		return card.chipNumber;
	}
	
	/**
	 * 
	 * @return
	 */
	public Card getCard() {
		return card;
	}
}
