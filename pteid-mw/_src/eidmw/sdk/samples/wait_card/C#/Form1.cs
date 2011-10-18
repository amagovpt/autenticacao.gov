using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.Runtime.InteropServices;

using be.belgium.eid;

namespace wait_card
{
    public partial class Form1 : Form
    {
        #region Win32

        [DllImport("Kernel32.dll")] static extern bool Beep(uint dwFreq, uint dwDuration);

        #endregion

        BEID_ReaderSet m_ReaderSet;
        bool m_bShowButton;
        string m_sMessage;

        System.Collections.Hashtable MyReadersSet = new System.Collections.Hashtable();

        public Form1()
        {
            InitializeComponent();

            m_bShowButton = true;
            m_sMessage = "Click on the 'Start' button and  follow the instruction";

            m_ReaderSet = BEID_ReaderSet.instance();
        }

        private void btnQuit_Click(object sender, EventArgs e)
        {
            try
            {
                this.Close();

                //Never forget to release the sdk before leaving
                BEID_ReaderSet.releaseSDK();

                Application.Exit();
            }
            catch (BEID_Exception ex)
            {
                MessageBox.Show("Crash BEID_Exception : " + ex.GetError().ToString());
                BEID_ReaderSet.releaseSDK();
                Application.Exit();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Crash System.Exception!");
                BEID_ReaderSet.releaseSDK();
                Application.Exit();
            }

        }

        private void RefreshMessage() 
        {
            //The control properties must be access in the thread that create them
            if (this.lblMessage.InvokeRequired)
            {
                //If the calling thread is not the creator thread,
                //we need to call it using BeginInvoke
                this.BeginInvoke(new MethodInvoker(this.RefreshMessage));
            } 
            else 
            {
                this.btnTest.Visible = m_bShowButton;
                this.btnQuit.Visible = m_bShowButton;
                this.lblMessage.Text = m_sMessage;
            }
        }

        private void btnTest_Click(object sender, EventArgs e)
        {
            //We start the RunThread function in an other thread
            Thread th = new Thread(new ThreadStart(RunThread));
            th.Start();
        }

        private void RunThread()
        {
            try
            {
                string sName="";

                m_bShowButton = false;
                m_sMessage = "Please insert your card";

                RefreshMessage();

                //Get the reader in which a card is inserted (timeout = 5 sec)
                BEID_ReaderContext reader = WaitForCardPresent(5);
                if (reader == null)
                {
                    m_sMessage = "You haven't inserted a card. Click on 'start' to try again.";
                    RefreshMessage();
                }
                else
                {
                    //Get the card (if it's not an eid card, an exception is thrown)
                    BEID_EIDCard card = reader.getEIDCard();
                    card.setAllowTestCard(true);

                    //Get data from the card
                    sName = card.getID().getFirstName() + " " + card.getID().getSurname();
                    m_sMessage = sName + ", please remove your card.";
                    RefreshMessage();

                    //Wait until the card has been remove (timeout = 5 sec)
                    if (!WaitForCardAbsent(5))
                    {
                        //Remind the user to remove his card with a beep
                        while (!WaitForCardAbsent(1))
                        {
                            m_sMessage = sName + ", don't forget to remove your card!";
                            RefreshMessage();
                            Beep(150,150); //beep
                        }
                    }
                    m_sMessage = "Click on the 'Start' button and  follow the instruction";
                    RefreshMessage();
                }
                m_bShowButton = true;
                RefreshMessage();
            }
            catch (BEID_Exception ex)
            {
                MessageBox.Show("Crash BEID_Exception : " + ex.GetError().ToString());
            }
            catch (Exception ex)
            {
                MessageBox.Show("Crash System.Exception!");
            }
        }

        //*****************************************************************************
        // WaitForCardPresent function
        // This function hangs until a card has been inserted into the reader
        // Or for a maximum of Timeout seconds
        // It returns the reader context or null if a timeout occured
        //*****************************************************************************
        private BEID_ReaderContext WaitForCardPresent(int Timeout)
        {
            BEID_ReaderContext reader=null;
            try
            {
                bool bContinue = true;
                int Count=0;

                while(bContinue)
                {
                    //Get a reader. getReader (without parameter) returns the first reader with a card.
                    //or the first reader (if no card is present)
                    //if no reader is connected, an BEID_ExNoReader exception is thrown
                    reader = m_ReaderSet.getReader();

                    //If a card is present, we can leave the loop
                    if(reader.isCardPresent())
                    {
                        bContinue=false;
                    }
                    //If timeout occured, we leave the loop and prepare the exception
                    else if (Count > Timeout)
                    {
                        bContinue = false;
                        reader = null;
                    }
                    //Else we sleep 1 second and check again
                    else
                    {
                        Thread.Sleep(1000);
                        Count++;
                    }
                }
                return reader;
            }
            catch (BEID_ExNoReader ex)
            {
                MessageBox.Show("No reader connected");
            }
            catch (BEID_Exception ex)
            {
                MessageBox.Show("Crash BEID_Exception : " + ex.GetError().ToString());
            }
            catch (Exception ex)
            {
                MessageBox.Show("Crash System.Exception!");
            }

            return null;
        }

        //*****************************************************************************
        // WaitForCardAbsent Function
        // This function hangs until no more card is pluged in the reader
        // Or for a maximum of Timeout seconds
        // It returns true if the no card is inserted and false if timeout occured
        //*****************************************************************************
        private bool WaitForCardAbsent(int Timeout)
        {
            try
            {
                BEID_ReaderContext reader = null;
                int Count = 0;

                while (true)
                {
                    //Get a reader. getReader (without parameter) return the first reader with a card.
                    //or the first reader (if no card is present)
                    //if no reader is connected, an BEID_ExNoReader exception is thrown
                    reader = m_ReaderSet.getReader();

                    //If no card is present, we return true
                    if (!reader.isCardPresent())
                    {
                        return true;
                    }
                    //If timeout occured, we return false
                    else if (Count > Timeout)
                    {
                        return false;
                    }
                    //Else we sleep 1 second and check again
                    else
                    {
                        Thread.Sleep(1000);
                        Count++;
                    }
                }
            }
            catch (BEID_ExNoReader ex)
            {
                MessageBox.Show("No reader connected");
            }
            catch (BEID_Exception ex)
            {
                MessageBox.Show("Crash BEID_Exception : " + ex.GetError().ToString());
            }
            catch (Exception ex)
            {
                MessageBox.Show("Crash System.Exception!");
            }

            return false;
        }

    }
}