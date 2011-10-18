using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using be.belgium.eid;

namespace get_exception
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void btnQuit_Click(object sender, EventArgs e)
        {
            this.Close();
            BEID_ReaderSet.releaseSDK();

            Application.Exit();

        }

        private void btnTest_Click(object sender, EventArgs e)
        {
            this.txtInfo.Text = "";
            this.txtInfo.Text += "eID SDK sample program: get_exception\r\n";
	        this.txtInfo.Text += "\r\n";

		    string readerName="test";
		    BEID_ReaderContext reader;
		    BEID_SISCard card;
		    BEID_XMLDoc doc;
		    BEID_SisId docId;

		    try
		    {
			    reader = BEID_ReaderSet.instance().getReaderByName(readerName);
		    }
            catch (BEID_ExParamRange ex)
            {
                this.txtInfo.Text += "SUCCESS => The reader named " + readerName + " does not exist\r\n";
            }
		    catch(BEID_Exception ex)
		    {
			    this.txtInfo.Text += "FAILS => Other BEID_Exception (code = " + ex.GetError().ToString("x") + ")\r\n";
                return;
            }
		    catch
		    {
			    this.txtInfo.Text += "FAILS => Other exception\r\n";
                return;
		    }

		    try
		    {
                reader = BEID_ReaderSet.instance().getReader();
			    this.txtInfo.Text += "SUCCESS => Reader found\r\n";
		    }
		    catch(BEID_ExNoReader ex)
		    {
			    this.txtInfo.Text += "FAILS => No reader found\r\n";
                return;
		    }
 		    catch(BEID_Exception ex)
		    {
			    this.txtInfo.Text += "FAILS => Other BEID_Exception (code = "  + ex.GetError().ToString("x") + ")\r\n";
                return;
		    }
		    catch
		    {
			    this.txtInfo.Text += "FAILS => Other exception\r\n";
                return;
		    }

		    try
		    {
			    card = reader.getSISCard();
			    this.txtInfo.Text += "SUCCESS => SIS card found\r\n";
		    }
		    catch(BEID_ExNoCardPresent ex)
		    {
			    this.txtInfo.Text += "FAILS => No card found\r\n";
                return;
		    }
		    catch(BEID_ExCardBadType ex)
		    {
			    this.txtInfo.Text += "FAILS => This is not a SIS card\r\n";
                return;
		    }
		    catch(BEID_Exception ex)
		    {
			    this.txtInfo.Text += "FAILS => Other BEID_Exception (code = " + ex.GetError().ToString("x") + ")\r\n";
                return;
		    }
		    catch
		    {
			   this.txtInfo.Text += "FAILS => Other exception\r\n";
               return;
		    }

		    try
		    {
			    doc = card.getDocument(BEID_DocumentType.BEID_DOCTYPE_PICTURE);
		    }
		    catch(BEID_ExDocTypeUnknown ex)
		    {
			    this.txtInfo.Text += "SUCCESS => No picture on this card\r\n";
		    }
 		    catch(BEID_Exception ex)
		    {
			    this.txtInfo.Text += "FAILS => Other BEID_Exception (code = " + ex.GetError().ToString("x") + ")\r\n";
                return;
		    }
		    catch
		    {
			    this.txtInfo.Text += "FAILS => Other exception\r\n";
                return;
		    }

		    try
		    {
                docId = card.getID();
			    this.txtInfo.Text += "SUCCESS => Your name is " + docId.getName() + "\r\n";
		    }
		    catch(BEID_Exception ex)
		    {
			    this.txtInfo.Text += "FAILS => Other BEID_Exception (code = " + ex.GetError().ToString("x") + ")\r\n";
                return;
		    }
		    catch
		    {
			    this.txtInfo.Text += "FAILS => Other exception\r\n";
                return;
		    }
        }
    }
}