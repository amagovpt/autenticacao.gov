using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using be.portugal.eid;

namespace pin_eid
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
            PTEID_ReaderSet.releaseSDK();

            Application.Exit();

        }

        private void btnLoad_Click(object sender, EventArgs e)
        {
            try
            {

                PTEID_ReaderSet ReaderSet;
                ReaderSet = PTEID_ReaderSet.instance();

                PTEID_ReaderContext Reader;
                Reader = ReaderSet.getReader();

                if (Reader.isCardPresent())
                {
                    if (Reader.getCardType() == PTEID_CardType.PTEID_CARDTYPE_EID
                        || Reader.getCardType() == PTEID_CardType.PTEID_CARDTYPE_FOREIGNER
                        || Reader.getCardType() == PTEID_CardType.PTEID_CARDTYPE_KIDS)
                    {
                        uint lRemaining=0;
                        if (Reader.getEIDCard().getPins().getPinByNumber(0).verifyPin("",ref lRemaining)) 
                            MessageBox.Show("Verification succeeded", "Information", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        else
                            MessageBox.Show("Verification failed\r\nTries left = " + lRemaining, "Information", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                    else
                    {
                        MessageBox.Show("The card doesn't support this feature", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    }
                }
                else
                {
                    MessageBox.Show("Please insert a card in the reader", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }

                PTEID_ReaderSet.releaseSDK();
            }

            catch (PTEID_Exception ex)
            {
                PTEID_ReaderSet.releaseSDK();
            }
            catch (Exception ex)
            {
                PTEID_ReaderSet.releaseSDK();
            }

        }

        private void button1_Click(object sender, EventArgs e)
        {
                        try
            {

                PTEID_ReaderSet ReaderSet;
                ReaderSet = PTEID_ReaderSet.instance();

                PTEID_ReaderContext Reader;
                Reader = ReaderSet.getReader();

                if (Reader.isCardPresent())
                {
                    if (Reader.getCardType() == PTEID_CardType.PTEID_CARDTYPE_EID
                        || Reader.getCardType() == PTEID_CardType.PTEID_CARDTYPE_FOREIGNER
                        || Reader.getCardType() == PTEID_CardType.PTEID_CARDTYPE_KIDS)
                    {
                        uint lRemaining=0;
                        if (Reader.getEIDCard().getPins().getPinByNumber(0).changePin("","",ref lRemaining)) 
                            MessageBox.Show("Verification succeeded", "Information", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        else
                            MessageBox.Show("Verification failed\r\nTries left = " + lRemaining, "Information", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                    else
                    {
                        MessageBox.Show("The card doesn't support this feature", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    }
                }
                else
                {
                    MessageBox.Show("Please insert a card in the reader", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }

                PTEID_ReaderSet.releaseSDK();
            }

            catch (PTEID_Exception ex)
            {
                PTEID_ReaderSet.releaseSDK();
            }
            catch (Exception ex)
            {
                PTEID_ReaderSet.releaseSDK();
            }
        }
    }
}