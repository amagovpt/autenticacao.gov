using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using be.belgium.eid;

namespace events_eid
{
    public partial class Form1 : Form
    {
        BEID_ReaderSet ReaderSet;
        private class ReaderRef
        {
            public BEID_ReaderContext reader;
            public uint eventHandle;
            public IntPtr ptr;
            public uint cardId;
        }

        System.Collections.Hashtable MyReadersSet = new System.Collections.Hashtable();

        public Form1()
        {
            InitializeComponent();

            ReaderSet = BEID_ReaderSet.instance();

            AttachEvents();
            MessageBox.Show("An event has been set on each reader");
        }

        private void btnQuit_Click(object sender, EventArgs e)
        {
            try
            {
                DetachEvents();

                MessageBox.Show("Events have been removed");

                this.Close();
                BEID_ReaderSet.releaseSDK();

                Application.Exit();
            }
            catch (BEID_Exception ex)
            {
                MessageBox.Show("Crash BEID_Exception!");
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

        private void btnTest_Click(object sender, EventArgs e)
        {
            try
            {
                string strList = "";
                string strCard = "";

                BEID_ReaderContext reader;

                foreach(ReaderRef readerRef in MyReadersSet.Values)
                {
                    reader = readerRef.reader;
                    if(reader.isCardPresent())
                        strCard = "Card present";
                    else
                        strCard = "No card";

                    strList += reader.getName() + " : " + strCard + "\r\n";
                }

                MessageBox.Show(strList);

            }
            catch (BEID_Exception ex)
            {
                MessageBox.Show("Crash BEID_Exception!");
            }
            catch (Exception ex)
            {
                MessageBox.Show("Crash System.Exception!");
            }
        }

        private void AttachEvents()
        {
            try
            {
                BEID_ReaderContext reader;
                ReaderRef readerRef;
                uint i;

                BEID_SetEventDelegate MyCallback= new BEID_SetEventDelegate(CallBack);

                string readerName;

                for(i = 0;i<ReaderSet.readerCount();i++)
                {
                    reader = ReaderSet.getReaderByNum(i);
                    readerName = ReaderSet.getReaderName(i);

                    readerRef = new ReaderRef();

                    readerRef.reader = reader;
                    readerRef.ptr = System.Runtime.InteropServices.Marshal.StringToHGlobalAnsi(readerName);
                    readerRef.cardId = 0;
                    MyReadersSet.Add(readerName, readerRef);
                    readerRef.eventHandle = reader.SetEventCallback(MyCallback, readerRef.ptr);
                }

            }
            catch (BEID_Exception ex)
            {
                MessageBox.Show("Crash BEID_Exception!");
            }
            catch (Exception ex)
            {
                MessageBox.Show("Crash System.Exception!");
            }

        }

        private void DetachEvents()
        {
            try
            {
                BEID_ReaderContext reader;

                foreach(ReaderRef readerRef in MyReadersSet.Values)
                {
                    reader = readerRef.reader;
                    reader.StopEventCallback(readerRef.eventHandle);
                }
                MyReadersSet.Clear();
            }
            catch(BEID_Exception ex)
            {
                MessageBox.Show("Crash BEID_Exception!");
            }
            catch(Exception ex)
            {
                MessageBox.Show("Crash System.Exception!");
            }

        }

        public void CallBack(int lRe, uint lState, System.IntPtr p)
        {
            try
            {
                string action = "";
                string readerName;
                ReaderRef readerRef;
                bool bChange;

                readerName = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(p);
                readerRef = (ReaderRef)MyReadersSet[readerName];

                bChange = false;

                if(readerRef.reader.isCardPresent())
                {
                    if(readerRef.reader.isCardChanged(ref readerRef.cardId))
                    {
                        action = "inserted in";
                        bChange = true;
                    }
                }
                else
                {
                    if(readerRef.cardId != 0)
                    {
                        action = "removed from";
                        bChange = true;
                    }
                }

                if(bChange)
                    MessageBox.Show("A card has been " + action + " the reader : " + readerName);
            }
            catch (BEID_Exception ex)
            {
                MessageBox.Show("Crash BEID_Exception!");
            }
            catch (Exception ex)
            {
                MessageBox.Show("Crash System.Exception!");
            }
       }

        private void ctrTimer_Tick(object sender, EventArgs e)
        {
            try
            {
                uint count;

                if(ReaderSet.isReadersChanged())
                {
                    DetachEvents();
                    count = ReaderSet.readerCount(true); //Force the read of reader list
                    AttachEvents();

                    MessageBox.Show("Readers has been plugged/unplugged\r\nNumber of readers : " + count);
                }
            }
            catch (BEID_Exception ex)
            {
                MessageBox.Show("Crash BEID_Exception!");
            }
            catch (Exception ex)
            {
                MessageBox.Show("Crash System.Exception!");
            }

        }
    }
}