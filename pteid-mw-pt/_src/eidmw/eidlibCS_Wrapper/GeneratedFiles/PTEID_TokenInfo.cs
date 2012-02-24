using System;
using System.Text;

namespace pt.portugal.eid
{
    public class PTEID_TokenInfo
    {

        public String label { get; private set; }
        public String serial { get; private set; }

        internal PTEID_TokenInfo(String label, String serial)
        {
            this.label = label;
            this.serial = serial;
        }
    }
}
