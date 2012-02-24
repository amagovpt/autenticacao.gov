using System;
using System.Text;

namespace pt.portugal.eid
{
    public class PTEID_PIC
    {
        public short version;
        public byte[] cbeff;
        public byte[] facialrechdr;
        public byte[] facialinfo;
        public byte[] imageinfo;
        public byte[] picture;

        internal PTEID_PIC()
        {
        }
    }
}
