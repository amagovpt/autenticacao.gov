using System;
using System.Text;

namespace pt.portugal.eid
{
    public class PteidException : Exception 
    {
        private int m_status;
        private string m_message;

        public PteidException(int i)
        {
            m_status = i;
        }

        public int getStatus()
        {
            return m_status;
        }

        public string getMessage()
        {
            return m_message;
        }
    }
}
