using System;
using System.Text;

namespace pt.portugal.eid
{
    public class PTEIDPin
    {
        public int pinType;
        public byte id;
        public int usageCode;
        public int triesLeft;
        public int flags;
        public String label;
        public String shortUsage;
        public String longUsage;

        internal PTEIDPin()
        {
        }
    }
}
