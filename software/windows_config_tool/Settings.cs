using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Configuration;


namespace AlceOSD
{



    public class GDI
    {
        [System.Runtime.InteropServices.DllImport("gdi32.dll")]
        internal static extern bool SetPixel(IntPtr hdc, int X, int Y, uint crColor);
    }



    public class UserSettings : ApplicationSettingsBase
    {
        [UserScopedSetting()]
        [DefaultSettingValue("")]
        public String ComPort
        {
            get
            {
                return ((String)this["ComPort"]);
            }
            set
            {
                this["ComPort"] = (String)value;
            }
        }
    }

}
