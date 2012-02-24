using System;
using System.Text;

namespace Project1
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
