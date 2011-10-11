package be.portugal.pteid;

import java.io.*;
import javax.swing.*;
import java.awt.*;
import java.util.*;
import java.util.logging.*;
import java.net.URL;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.lang.reflect.*;
// import the JNI jar
import be.portugal.eid.*;


/*
 *	Base class for the different eID cards we can read
 */
class ImageCanvas extends Canvas
{
    Container 	pappy;
    Image 		image;
    boolean 	trueSizeKnown = false;
    Dimension 	minSize;
    int w, h;

    public ImageCanvas(	  Image image
    					, Container parent
    					, int initialWidth
    					, int initialHeight
    				  )
    {
		if (image == null)
		{
		    System.err.println("ImageCanvas::paint()");
		    return;
		}

		this.image 	= image;
	    pappy 		= parent;

		w = initialWidth;
		h = initialHeight;

		minSize = new Dimension(w,h);
    }

    public Dimension preferredSize()
    {
 		return minimumSize();
    }

    public synchronized Dimension minimumSize()
    {
		return minSize;
    }

    public void paint (Graphics g)
    {
		if (image != null)
		{
		    if (!trueSizeKnown)
		    {
		        int imageWidth = image.getWidth(this);
		        int imageHeight = image.getHeight(this);

		        if ((imageWidth > 0) && (imageHeight > 0))
		        {
				    trueSizeKnown = true;

				    //Component-initiated resizing.
				    w = imageWidth;
				    h = imageHeight;
				    minSize = new Dimension(w,h);
				    resize(w, h);
				    pappy.validate();
		        }
		    }

		    g.drawImage(image, 0, 0, this);
		}
    }
}

class Card
{
	Card()
	{
		reset();
	}
	public void reset()
	{
		m_cardType = PTEID_CardType.PTEID_CARDTYPE_UNKNOWN;
	}
	PTEID_CardType getCardType()
	{
		return m_cardType;
	}
	public String getLogicalNumber() throws java.lang.Exception
	{
		return "";
	}
	public String getChipNumber() throws java.lang.Exception
	{
		return "";
	}
	public String getValidityBeginDate() throws java.lang.Exception
	{
		return "";
	}
	public String getValidityEndDate() throws java.lang.Exception
	{
		return "";
	}
	public String getIssuingMunicipality() throws java.lang.Exception
	{
		return "";
	}
	public String getNationalNumber() throws java.lang.Exception
	{
		return "";
	}
	public String getName() throws java.lang.Exception
	{
		return "";
	}
	public String getSurname() throws java.lang.Exception
	{
		return "";
	}
	public String getFirstName() throws java.lang.Exception
	{
		return "";
	}
	public String getInitials() throws java.lang.Exception
	{
		return "";
	}
	public String getNationality() throws java.lang.Exception
	{
		return "";
	}
	public String getLocationOfBirth() throws java.lang.Exception
	{
		return "";
	}
	public String getDateOfBirth() throws java.lang.Exception
	{
		return "";
	}
	public String getGender() throws java.lang.Exception
	{
		return "";
	}
	public String getNobility() throws java.lang.Exception
	{
		return "";
	}
	public String getStreet() throws java.lang.Exception
	{
		return "";
	}
	public String getZipCode() throws java.lang.Exception
	{
		return "";
	}
	public String getMunicipality() throws java.lang.Exception
	{
		return "";
	}
	public String getCountry() throws java.lang.Exception
	{
		return "";
	}
	public String getSpecialStatus()  throws java.lang.Exception
	{
		return "";
	}
	public PTEID_Picture GetPicture()
	{
		return null;
	}

	PTEID_CardType m_cardType = PTEID_CardType.PTEID_CARDTYPE_UNKNOWN;
}

/*
 * class eID card, derived from base class Card
 */

class EIDCard extends Card
{
	EIDCard()
	{
		m_eid	   = null;
		m_picture  = null;
		m_cardType = PTEID_CardType.PTEID_CARDTYPE_UNKNOWN;
	}
	EIDCard(PTEID_EIDCard card)
	{
		try
		{
			m_eid	   = card.getID();
			m_picture  = card.getPicture();
			m_cardType = PTEID_CardType.PTEID_CARDTYPE_EID;
		}
		catch( Exception e)
		{
			m_eid	   = null;
			m_picture  = null;
			m_cardType = PTEID_CardType.PTEID_CARDTYPE_UNKNOWN;
		}
	}
	EIDCard(PTEID_KidsCard card)
	{
		try
		{
			m_eid		= card.getID();
			m_picture	= card.getPicture();
			m_cardType  = PTEID_CardType.PTEID_CARDTYPE_KIDS;
		}
		catch( Exception e )
		{
			m_eid		= null;
			m_picture	= null;
			m_cardType	= PTEID_CardType.PTEID_CARDTYPE_UNKNOWN;
		}
	}
	EIDCard(PTEID_ForeignerCard card)
	{
		try
		{
			m_eid = card.getID();
			m_picture = card.getPicture();
			m_cardType = PTEID_CardType.PTEID_CARDTYPE_FOREIGNER;
		}
		catch (Exception e)
		{
			m_eid = null;
			m_picture = null;
			m_cardType = PTEID_CardType.PTEID_CARDTYPE_UNKNOWN;
		}
	}
	public void reset()
	{
		super.reset();
		m_cardType = PTEID_CardType.PTEID_CARDTYPE_UNKNOWN;

		if (m_eid != null)
		{
			m_eid = null;
		}
		if (m_picture != null)
		{
			m_picture = null;
		}
	}

	public String getLogicalNumber() throws java.lang.Exception
	{
		if (m_eid != null)
		{
			return m_eid.getLogicalNumber();
		}
		return "";
	}
	public String getChipNumber() throws java.lang.Exception
	{
		if (m_eid != null)
			return m_eid.getChipNumber();
		return "";
	}
	public String getValidityBeginDate() throws java.lang.Exception
	{
		if (m_eid != null)
			return m_eid.getValidityBeginDate();
		return "";
	}
	public String getValidityEndDate() throws java.lang.Exception
	{
		if (m_eid != null)
			return m_eid.getValidityEndDate();
		return "";
	}
	public String getIssuingMunicipality() throws java.lang.Exception
	{
		if (m_eid != null)
			return m_eid.getIssuingMunicipality();
		return "";
	}
	public String getNationalNumber() throws java.lang.Exception
	{
		if (m_eid != null)
			return m_eid.getNationalNumber();
		return "";
	}
	public String getSurname() throws java.lang.Exception
	{
		if (m_eid != null)
		{
			return m_eid.getSurname();
		}
		return "";
	}
	public String getFirstName() throws java.lang.Exception
	{
		if (m_eid != null)
		{
			return m_eid.getFirstName();
		}
		return "";
	}
	public String getNationality() throws java.lang.Exception
	{
		if (m_eid != null)
			return m_eid.getNationality();
		return "";
	}
	public String getLocationOfBirth() throws java.lang.Exception
	{
		if (m_eid != null)
			return m_eid.getLocationOfBirth();
		return "";
	}
	public String getDateOfBirth()  throws java.lang.Exception
	{
		if (m_eid != null)
			return m_eid.getDateOfBirth();
		return "";
	}
	public String getGender()  throws java.lang.Exception
	{
		if (m_eid != null)
			return m_eid.getGender();
		return "";
	}
	public String getNobility()  throws java.lang.Exception
	{
		if (m_eid != null)
			return m_eid.getNobility();
		return "";
	}
	public String getStreet() throws java.lang.Exception
	{
		if (m_eid != null)
			return m_eid.getStreet();
		return "";
	}
	public String getZipCode() throws java.lang.Exception
	{
		if (m_eid != null)
			return m_eid.getZipCode();
		return "";
	}
	public String getMunicipality() throws java.lang.Exception
	{
		if (m_eid != null)
			return m_eid.getMunicipality();
		return "";
	}
	public String getCountry() throws java.lang.Exception
	{
		if (m_eid != null)
			return m_eid.getCountry();
		return "";
	}
	public String getSpecialStatus()  throws java.lang.Exception
	{
		if (m_eid != null)
			return m_eid.getSpecialStatus();
		return "";
	}
	public PTEID_Picture GetPicture()
	{
		return m_picture;
	}


	PTEID_EId			m_eid		= null;
	PTEID_Picture		m_picture	= null;
}

/*
* SISCard class
*/

class SISCard extends Card
{
	SISCard()
	{
		reset();
	}

	SISCard(PTEID_SISCard card)
	{
		try
		{
			m_sisid    = card.getID();
			m_cardType = PTEID_CardType.PTEID_CARDTYPE_SIS;
		}
		catch ( Exception e)
		{
			reset();
		}
	}
	public void reset()
	{
		m_sisid = null;
		m_cardType = PTEID_CardType.PTEID_CARDTYPE_UNKNOWN;
	}
	public String getLogicalNumber() throws java.lang.Exception
	{
		return m_sisid.getLogicalNumber();
	}
	public String getNationalNumber() throws java.lang.Exception
	{
		return m_sisid.getSocialSecurityNumber();
	}
	public String getValidityBeginDate() throws java.lang.Exception
	{
		return m_sisid.getValidityBeginDate();
	}
	public String getValidityEndDate() throws java.lang.Exception
	{
		return m_sisid.getValidityEndDate();
	}
	public String getSurname() throws java.lang.Exception
	{
		return m_sisid.getSurname();
	}
	public String getFirstName() throws java.lang.Exception
	{
		return m_sisid.getName();
	}
	public String getInitials() throws java.lang.Exception
	{
		return m_sisid.getInitials();
	}
	public String getGender() throws java.lang.Exception
	{
		String genderCode = m_sisid.getGender();
		String gender = "F";
		if (genderCode.equals("1"))
		{
			gender = "M";
		}
		return gender;
	}
	public String getDateOfBirth() throws java.lang.Exception
	{
		return m_sisid.getDateOfBirth();
	}

	PTEID_SisId m_sisid = null;
}

//*****************************************************************
// Java applet to read a Belgian eID card
//*****************************************************************
public class PTEID_ImgApplet extends JApplet
{
	JButton jButtonRead = new JButton();
	JLabel lblImage = new JLabel();

	protected long		m_nrReaders		= 0;
	protected String 	m_strReaderName = "";
	Container 			m_container		= this.getContentPane();
	PTEID_ReaderContext 	m_readerContext = null;
	Card				m_card			= null;
	boolean				m_debug			= false;

	private static void loadLibraryInternal(String libraryName)
	{
		String sunAppletLauncher = System.getProperty("sun.jnlp.applet.launcher");
		boolean usingJNLPAppletLauncher = Boolean.valueOf(sunAppletLauncher).booleanValue();

		boolean loaded = false;
		if (usingJNLPAppletLauncher)
		{
			try
			{
				Class jnlpAppletLauncherClass = Class.forName("org.jdesktop.applet.util.JNLPAppletLauncher");
				Method jnlpLoadLibraryMethod = jnlpAppletLauncherClass.getDeclaredMethod("loadLibrary", new Class[] { String.class });
				jnlpLoadLibraryMethod.invoke(null, new Object[] { libraryName });
				loaded = true;
			}
			catch (ClassNotFoundException ex)
			{
				System.err.println("loadLibrary(" + libraryName + ")");
				System.err.println(ex);
				System.err.println("Attempting to use System.loadLibrary instead");
			}
			catch (Exception e)
			{
				Throwable t = e;
				if (t instanceof InvocationTargetException)
				{
					t = ((InvocationTargetException)t).getTargetException();
				}
				if (t instanceof Error)
					throw (Error)t;
				if (t instanceof RuntimeException)
				{
					throw (RuntimeException)t;
				}
				// Throw UnsatisfiedLinkError for best compatibility with System.loadLibrary()
				throw (UnsatisfiedLinkError)new UnsatisfiedLinkError().initCause(e);
			}
		}

		if (!loaded)
		{
			System.loadLibrary(libraryName);
		}
	}

	//-------------------------------------------------------------
	// make sure we always load the wrapper DLL
	//-------------------------------------------------------------
	static
	{
		try
		{
		int	 Retval = 0;
		String 	osName  = System.getProperty("os.name");
		String  JavaWrapper = "pteidlibJava_Wrapper";

			if (-1 != osName.indexOf("Windows"))
			{
				JavaWrapper = "pteid35libJava_Wrapper";
				System.out.println("[Info]  Windows system!!");
				System.out.println("[Info]  Loading Java wrapper: " + JavaWrapper);
				loadLibraryInternal(JavaWrapper);
				System.out.println("[Info]  Java wrapper loaded");
			}
			else
			{
				System.out.println("[Info]  Loading Java wrapper: " + JavaWrapper);
				loadLibraryInternal(JavaWrapper);
				System.out.println("[Info]  Java wrapper loaded");
			}
		}
		catch (SecurityException e)
		{
			System.err.println("[Error] SecurityException when loading pteid35libJava_Wrapper: " + e.getMessage());
		}
		catch (UnsatisfiedLinkError e)
		{
			String property = System.getProperty("java.library.path");
			String err = e.getMessage();
			System.err.println("[ERROR] UnsatisfiedLinkError: " + err);
			System.err.println("java.library.path = " + property);
			System.err.println("java.class.path = " + System.getProperty("java.class.path"));
		}
		catch(Exception e)
		{
			System.err.println("[Error] Exception when loading pteid35libJava_Wrapper");
		}
	}

	//-------------------------------------------------------------
	// ctor
	//-------------------------------------------------------------
	public PTEID_ImgApplet()
	{
		try
		{
			jbInit();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}

	//-------------------------------------------------------------
	// init function of the applet.
	//-------------------------------------------------------------
	public void init()
	{
		try
		{
			String param;
			String value;
		}
		catch(NullPointerException e)
		{
      		System.err.println("[Error] NullPointerException: PTEID_ImgApplet::init()");
		}
		catch (SecurityException e)
		{
			System.err.println("[Error] Security exception: PTEID_ImgApplet::init()");
		}
		catch (UnsatisfiedLinkError e)
		{
			System.err.println("[Error] Unsatisfied link error: PTEID_ImgApplet::init()");
		}
		catch (Exception e)
		{
			String msg = e.getMessage();
			System.err.println("[Error] Exception: PTEID_ImgApplet::init(): " + msg);
			e.printStackTrace();
		}
	}

	//-------------------------------------------------------------
	// the object is destroyed, so exit the SDK library properly
	//-------------------------------------------------------------
	public void destroy()
	{
		try
		{
			System.err.println("[INFO]  PTEID_ImgApplet::destroy(): ");
			exitLib();
		}
		catch( Exception e)
		{
			String msg = e.getMessage();
			System.err.println("[Error] Exception: PTEID_ImgApplet::init(): " + msg);
			e.printStackTrace();
		}
	}

	//-------------------------------------------------------------
	// initialize the SDK library
	//-------------------------------------------------------------
	public int InitLib(String strReaderName) //throws java.lang.Exception
	{
		try
		{
			reset();

			m_nrReaders = 0;
			lblImage.setIcon(null);

			if (strReaderName != null)
			{
				m_strReaderName = strReaderName;
			}

			m_nrReaders = PTEID_ReaderSet.instance().readerCount();

			return (int)m_nrReaders;
		}
		catch( Exception e)
		{
			System.err.println("[Error] Exception: PTEID_ImgApplet::InitLib(" + strReaderName + "): " + e.getMessage());
		}
		return (int)m_nrReaders;
	}

	//-------------------------------------------------------------
	// check if a card is present
	//-------------------------------------------------------------
	public boolean isCardPresent(String strReaderName) throws java.lang.Exception
	{
		if ( strReaderName != null)
		{
			PTEID_ReaderContext readerContext = PTEID_ReaderSet.instance().getReaderByName(strReaderName);
			if (readerContext==null )
			{
				return false;
			}
			if (readerContext.isCardPresent())
			{
				return true;
			}
		}
		return false;
	}
	//-------------------------------------------------------------
	// get the number of readers connected to the machine
	//-------------------------------------------------------------
	public long readerCount()
	{
		return m_nrReaders;
	}

	//-------------------------------------------------------------
	// get the reader by index
	//-------------------------------------------------------------
	public String getReaderByNum(long idx) //throws java.lang.Exception
	{
		String readerName = "";
		try
		{
			long nrReaders = PTEID_ReaderSet.instance().readerCount();

			if ( idx > nrReaders || idx < 0)
			{
				return readerName;
			}
			readerName = PTEID_ReaderSet.instance().getReaderName(idx);

			m_strReaderName = readerName;
		}
		catch (Exception e)
		{
		}
		return m_strReaderName;
 	}

	//-------------------------------------------------------------
	// exit the library
	//-------------------------------------------------------------
	public void exitLib() //throws java.lang.Exception
	{
		try
		{
			reset();
			PTEID_ReaderSet.releaseSDK();
		}
		catch(Exception e)
		{
			System.err.println("[ERROR] exitLib() thorws exception: " + e.getMessage());
		}
	}

	//-------------------------------------------------------------
	// reset the objects
	//-------------------------------------------------------------
	private void reset()
	{
		if (m_readerContext != null)
		{
			m_readerContext = null;
		}
		if (m_card!=null)
		{
			m_card.reset();
			m_card = null;
		}
	}
	//-------------------------------------------------------------
	// bring up dialog to ask for acceptance of test card
	//-------------------------------------------------------------
	private boolean askAcceptTestCard()
	{
		Object[] options = { "YES", "NO" };
		String msg = "The root certificate is not correct\nThis may be a testcard.\n\n Do you want to accept it?";
		int selectedOption = JOptionPane.showOptionDialog(null, msg
															, "Belgian eID Middleware"
															, JOptionPane.DEFAULT_OPTION
															, JOptionPane.WARNING_MESSAGE
															, null
															, options
															, options[0]
															);
		if (selectedOption == 0)
		{
			return true;
		}
		return false;
	}

	//-------------------------------------------------------------
	// Get the data from a Belgian EID card
	//-------------------------------------------------------------
	private boolean getEidCardData() //throws java.lang.Exception
	{
		try
		{
			if (m_card == null)
			{
				PTEID_EIDCard card = m_readerContext.getEIDCard();
				if (card.isTestCard() && !card.getAllowTestCard())
				{
					if (!askAcceptTestCard())
					{
						m_card = new EIDCard();
						return false;
					}
					card.setAllowTestCard(true);
				}
				m_card = new EIDCard(card);
			}
			return true;
		}
		catch(Exception e)
		{
			System.err.println("[ERROR] getEidCardData() thorws exception: " + e.getMessage());
		}
		return false;
	}
	//-------------------------------------------------------------
	// Get the data from a Belgian kids EID card
	//-------------------------------------------------------------
	private boolean getKidsCardData() throws java.lang.Exception
	{
		if (m_card == null)
		{
			PTEID_KidsCard card = m_readerContext.getKidsCard();
			if (card.isTestCard() && !card.getAllowTestCard())
			{
				if (!askAcceptTestCard())
				{
					m_card = new EIDCard();
					return false;
				}
				card.setAllowTestCard(true);
			}
			m_card = new EIDCard(card);
		}
		return true;
	}
	//-------------------------------------------------------------
	// Get the data from a Belgian foreigner EID card
	//-------------------------------------------------------------
	private boolean getForeignerCardData() throws java.lang.Exception
	{
		if (m_card == null)
		{
			PTEID_ForeignerCard card = m_readerContext.getForeignerCard();
			if (card.isTestCard() && !card.getAllowTestCard())
			{
				if (!askAcceptTestCard())
				{
					m_card = new EIDCard();
					return false;
				}
				card.setAllowTestCard(true);
			}
			m_card = new EIDCard(card);
		}
		return true;
	}

	//-------------------------------------------------------------
	// Get the data from a Belgian SIS card
	//-------------------------------------------------------------
	private boolean getSISCardData() throws java.lang.Exception
	{
		if (m_card == null)
		{
			PTEID_SISCard card = m_readerContext.getSISCard();
			m_card = new SISCard(card);
		}
		return true;
	}
	//-------------------------------------------------------------
	// check if a card is loaded and get the data
	//-------------------------------------------------------------
	private boolean isCardLoaded() //throws java.lang.Exception
	{
		boolean retVal = false;

		try
		{
			if (m_readerContext == null)
			{
				if (m_strReaderName == "")
				{
					return false;
				}

				PTEID_ReaderSet readerset = PTEID_ReaderSet.instance();
				if (readerset == null)
				{
					return false;
				}
				m_readerContext = readerset.getReaderByName(m_strReaderName);
				if (m_readerContext==null )
				{
					return false;
				}
			}
			if (m_readerContext.isCardPresent())
			{
				PTEID_CardType cardType = m_readerContext.getCardType();
				if (cardType == PTEID_CardType.PTEID_CARDTYPE_EID)
				{
					retVal = getEidCardData();
				}
				else if (cardType == PTEID_CardType.PTEID_CARDTYPE_KIDS)
				{
					retVal = getKidsCardData();
				}
				else if (cardType == PTEID_CardType.PTEID_CARDTYPE_FOREIGNER)
				{
					retVal = getForeignerCardData();
				}
				else if (cardType == PTEID_CardType.PTEID_CARDTYPE_SIS)
				{
					retVal = getSISCardData();
				}
				else
				{
					retVal = false;
				}
			}
		}
		catch(Exception e)
		{
			exitLib();
			System.err.println("[INFO] isCardLoaded()" + ": Calling InitLib(null)");

			InitLib(null);
		}
		return retVal;
	}

	//-------------------------------------------------------------
	// Helper functions to get the data from the card
	//-------------------------------------------------------------
	public String getCardNumber() throws java.lang.Exception
	{
		String retVal = "";

		if (isCardLoaded())
		{
			retVal = m_card.getLogicalNumber();
		}
 		return retVal;
	}
	public String getChipNumber() throws java.lang.Exception
	{
		String retVal = "";

		if (isCardLoaded())
		{
			retVal = m_card.getChipNumber();
		}
		return retVal;
	}
	public String getValidityDateBegin() throws java.lang.Exception
	{
		String retVal = "";

		if (isCardLoaded())
		{
			retVal = m_card.getValidityBeginDate();
		}
		return retVal;
	}
	public String getValidityDateEnd() throws java.lang.Exception
	{
		String retVal = "";

		if (isCardLoaded())
		{
			retVal = m_card.getValidityEndDate();
		}
		return retVal;
	}
	public String getIssMunicipality() throws java.lang.Exception
	{
		String retVal = "";

		if (isCardLoaded())
		{
			retVal = m_card.getIssuingMunicipality();
		}
		return retVal;
	}

	public String getNationalNumber() throws java.lang.Exception
	{
		String retVal = "";

		if (isCardLoaded())
		{
			retVal = m_card.getNationalNumber();
		}
		return retVal;
	}
	public String getSurname() //throws java.lang.Exception
	{
		String retVal = "";

		try
		{
			if (isCardLoaded())
			{
				retVal = m_card.getSurname();
			}

		}
		catch (Exception e)
		{
		}
		return retVal;
	}
	public String getFirstName() throws java.lang.Exception
	{
		String retVal = "";

		if (isCardLoaded())
		{
			retVal = m_card.getFirstName();
		}
		return retVal;
	}
	public String getInitials() throws java.lang.Exception
	{
		String retVal = "";

		if (isCardLoaded())
		{
			retVal = m_card.getInitials();
		}
		return retVal;
	}
	public String getNationality() throws java.lang.Exception
	{
		String retVal = "";

		if (isCardLoaded())
		{
			retVal = m_card.getNationality();
		}
		return retVal;
	}
	public String getBirthLocation() throws java.lang.Exception
	{
		String retVal = "";

		if (isCardLoaded())
		{
			retVal = m_card.getLocationOfBirth();
		}
		return retVal;
	}
	public String getBirthDate() throws java.lang.Exception
	{
		String retVal = "";

		if (isCardLoaded())
		{
			retVal = m_card.getDateOfBirth();
		}
		return retVal;
	}
	public String getSex() throws java.lang.Exception
	{
		String retVal = "";

		if (isCardLoaded())
		{
			retVal = m_card.getGender();
		}
		return retVal;
	}
	public String getNobleCondition() throws java.lang.Exception
	{
		String retVal = "";

		if (isCardLoaded())
		{
			retVal = m_card.getNobility();
		}
		return retVal;
	}
	public String getWhiteCane()
	{
		String retVal = "";
		return retVal;
	}
	public String getYellowCane()
	{
		String retVal = "";
		return retVal;
	}
	public String getExtendedMinority()
	{
		String retVal = "";
		return retVal;
	}
	public String getStreet() throws java.lang.Exception
	{
		String retVal = "";

		if (isCardLoaded())
		{
			retVal = m_card.getStreet();
		}
		return retVal;
	}
	public String getStreetNumber()
	{
		String retVal = "";
		return retVal;
	}
	public String getBoxNumber()
	{
		String retVal = "";
		return retVal;
	}
	public String getZip() throws java.lang.Exception
	{
		String retVal = "";

		if (isCardLoaded())
		{
			retVal = m_card.getZipCode();
		}
		return retVal;
	}
	public String getMunicipality() throws java.lang.Exception
	{
		String retVal = "";

		if (isCardLoaded())
		{
			retVal = m_card.getMunicipality();
		}
		return retVal;
	}
	public String getCountry() throws java.lang.Exception
	{
		String retVal = "";

		if (isCardLoaded())
		{
			retVal = m_card.getCountry();
		}
		return retVal;
	}
	public String getSpecialStatus() throws java.lang.Exception
	{
		String retVal = "";

		if (isCardLoaded())
		{
			retVal = m_card.getSpecialStatus();
		}
		return retVal;
	}


	public void centerScreen(JDialog dialog)
	{
		Dimension dim = getToolkit().getScreenSize();
		dialog.setLocation((dim.width - dialog.getWidth()) / 2, (dim.height - dialog.getHeight()) / 2);
		dialog.requestFocus();
	}


	//-------------------------------------------------------------
	// get the picture from the card
	//-------------------------------------------------------------
	public String GetPictureBase64() //throw	s java.lang.Exception
	{
		try
		{
			byte[] pictRaw = GetPicture();
			String encBytes = Base64.encodeBytes( pictRaw );
			return encBytes;
		}
		catch (Exception e)
		{
		}
		return "";
	}
	public byte[] GetPicture() throws java.lang.Exception
	{
		if (m_card == null)
		{
			return null;
		}
		PTEID_Picture picture = m_card.GetPicture();
		if (picture == null)
		{
			return null;
		}
		PTEID_ByteArray pictureData = picture.getData();
		byte[] pictureBytes = pictureData.GetBytes();
		try
		{
			Rectangle abounds = getBounds();
			java.awt.Toolkit toolkit = Toolkit.getDefaultToolkit();
			Image tempImage = toolkit.createImage(pictureBytes);
			lblImage.setIcon(new ImageIcon(tempImage.getScaledInstance(abounds.width, abounds.height, Image.SCALE_SMOOTH)));
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}

		return pictureBytes;

	}
	//-------------------------------------------------------------
	// initialize the applet
	//-------------------------------------------------------------
	private void jbInit() throws Exception
	{
		jButtonRead.setActionCommand("jButtonRead");
		jButtonRead.setText("Read");
		jButtonRead.addActionListener(new PTEID_ImgApplet_jButtonRead_actionAdapter(this));
		m_container.add(lblImage);
	}

	public void jButtonRead_actionPerformed(ActionEvent e)
	{
		try
		{
			System.err.println("[INFO] jButtonRead_actionPerformed()" + ": Calling InitLib(null)");
			InitLib(null);
		}
		catch( Exception err)
		{
			System.err.println("[ERROR] jButtonRead_actionPerformed()" + err.getMessage());
		}
	}
}

class PTEID_ImgApplet_jButtonRead_actionAdapter
    implements ActionListener
{
	private PTEID_ImgApplet adaptee;
	PTEID_ImgApplet_jButtonRead_actionAdapter(PTEID_ImgApplet adaptee)
	{
		this.adaptee = adaptee;
	}

	public void actionPerformed(ActionEvent e)
	{
		adaptee.jButtonRead_actionPerformed(e);
	}
}
