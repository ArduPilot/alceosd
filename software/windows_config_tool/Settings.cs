using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Configuration;


namespace AlceOSD
{
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
