using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using be.portugal.eid;

namespace events_eid
{
    public partial class Form1 : Form
    {
        PTEID_ReaderSet ReaderSet;
        private class ReaderRef
        {
            public PTEID_ReaderContext reader;
            public uint eventHandle;
            public IntPtr ptr;
            public uint cardId;
        }

        System.Collections.Hashtable MyReadersSet = new System.Collections.Hashtable();

        public Form1()
        {
            InitializeComponent();

            ReaderSet = PTEID_ReaderSet.instance();

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
                PTEID_ReaderSet.releaseSDK();

                Application.Exit();
            }
            catch (PTEID_Exception ex)
            {
                MessageBox.Show("Crash PTEID_Exception!");
                PTEID_ReaderSet.releaseSDK();
                Application.Exit();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Crash System.Exception!");
                PTEID_ReaderSet.releaseSDK();
                Application.Exit();
            }

        }

        private void btnTest_Click(object sender, EventArgs e)
        {
            try
            {
                string strList = "";
                string strCard = "";

                PTEID_ReaderContext reader;

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
            catch (PTEID_Exception ex)
            {
                MessageBox.Show("Crash PTEID_Exception!");
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
                PTEID_ReaderContext reader;
                ReaderRef readerRef;
                uint i;

                PTEID_SetEventDelegate MyCallback= new PTEID_SetEventDelegate(CallBack);

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
            catch (PTEID_Exception ex)
            {
                MessageBox.Show("Crash PTEID_Exception!");
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
                PTEID_ReaderContext reader;

                foreach(ReaderRef readerRef in MyReadersSet.Values)
                {
                    reader = readerRef.reader;
                    reader.StopEventCallback(readerRef.eventHandle);
                }
                MyReadersSet.Clear();
            }
            catch(PTEID_Exception ex)
            {
                MessageBox.Show("Crash PTEID_Exception!");
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
            catch (PTEID_Exception ex)
            {
                MessageBox.Show("Crash PTEID_Exception!");
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
            catch (PTEID_Exception ex)
            {
                MessageBox.Show("Crash PTEID_Exception!");
            }
            catch (Exception ex)
            {
                MessageBox.Show("Crash System.Exception!");
            }

        }
    }
}