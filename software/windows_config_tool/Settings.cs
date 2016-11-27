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
        public string ComPort
        {
            get
            {
                return ((string)this["ComPort"]);
            }
            set
            {
                this["ComPort"] = (string)value;
            }
        }
        [UserScopedSetting()]
        [DefaultSettingValue("115200")]
        public int MavlinkBaudrate
        {
            get
            {
                return ((int)this["MavlinkBaudrate"]);
            }
            set
            {
                this["MavlinkBaudrate"] = (int)value;
            }
        }
    }
}
