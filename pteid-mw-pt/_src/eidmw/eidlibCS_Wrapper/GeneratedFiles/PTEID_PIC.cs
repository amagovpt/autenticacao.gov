using System;
using System.Text;

namespace eidpt
{
    public class PteidPic
    {
        public short version;
        public byte[] cbeff;
        public byte[] facialrechdr;
        public byte[] facialinfo;
        public byte[] imageinfo;
        public byte[] picture;

        internal PteidPic()
        {
        }
    }
}
