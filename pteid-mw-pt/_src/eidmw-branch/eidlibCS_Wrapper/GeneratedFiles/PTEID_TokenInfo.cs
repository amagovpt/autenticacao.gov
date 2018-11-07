using System;
using System.Text;

namespace eidpt
{
    public class PteidTokenInfo
    {

        private String Label; 
        private String Serial;

        public string label
        {
            get
            {
                return Label;
            }
            private set
            {
                Label = value;
            }
        }

        public string serial
        {
            get
            {
                return Serial;
            }
            private set
            {
                Serial = value;
            }
        }

        internal PteidTokenInfo(String label, String serial)
        {
            this.label = label;
            this.serial = serial;
        }
    }
}
