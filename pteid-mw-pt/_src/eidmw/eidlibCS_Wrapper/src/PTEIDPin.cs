using System;
using System.Text;

namespace eidpt
{
    public class PteidPin
    {
        private int PinType;
        private byte Id;
        private int UsageCode;
        private int TriesLeft;
        private int Flags;
        private string Label;
        private string ShortUsage;
        private string LongUsage;

        public int pinType
        {
            get
            {
                return PinType;
            }
            internal set
            {
                PinType = value;
            }
        }
        public byte id
        {
            get
            {
                return Id;
            }
            internal set
            {
                Id = value;
            }
        }
        public int usageCode
        {
            get
            {
                return UsageCode;
            }
            internal set
            {
                UsageCode = value;
            }
        }
        public int triesLeft
        {
            get
            {
                return TriesLeft;
            }
            internal set
            {
                TriesLeft = value;
            }
        }
        public int flags
        {
            get
            {
                return Flags;
            }
            internal set
            {
                Flags = value;
            }
        }
        public string label
        {
            get
            {
                return Label;
            }
            internal set
            {
                Label = value;
            }
        }
        public string shortUsage
        {
            get
            {
                return ShortUsage;
            }
            internal set
            {
                ShortUsage = value;
            }
        }
        public string longUsage
        {
            get
            {
                return LongUsage;
            }
            internal set
            {
                LongUsage = value;
            }
        }


        internal PteidPin()
        {
        }
    }
}
