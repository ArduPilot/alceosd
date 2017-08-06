using System;
using System.Collections.Generic;
using System.Linq;
using System.Globalization;
using System.Text;
using System.Threading.Tasks;

namespace AlceOSD
{
    class Widgets
    {
        public Widgets()
        {

        }
        ~Widgets()
        {

        }

        public Dictionary<string, string> name_map = new Dictionary<string, string>();
        /*{
        {"Altitude", "ALTITUD"},
        {"Alarms", "ALARMS"},
        {"Battery", "BATTERY"},
        {"Console", "CONSOLE"},
        {"Compass", "COMPASS"},
        {"Flight Info", "FLTINFO"},
        {"Flight Mode", "FLTMODE"},
        {"StOrM32 gimbal", "GIMBAL"},
        {"GPS info", "GPSINFO"},
        {"Home info", "HOMEINF"},
        {"Artificial Horizon", "HORIZON"},
        {"Radar", "RADAR"},
        {"RC Channels", "RCCHAN"},
        {"RSSI", "RSSI"},
        {"Sonar", "SONAR"},
        {"Speed", "SPEED"},
        {"Temperature", "TEMPER"},
        {"Throttle", "THROTTL"},
        {"Variometer", "VARIO"},
        {"Video levels", "VIDLVL"},
        {"Video profile", "VIDPRF"},
        {"Wind info", "WINDINF"},
        };*/

        Dictionary<string, int> id_map = new Dictionary<string, int>();

        //public Dictionary<string, Widget> widgets = new Dictionary<string, Widget>() { };
        public Dictionary<string, Dictionary<int, Widget>> widgets = new Dictionary<string, Dictionary<int, Widget>>() { };

        public void add_id_map(string mavname, int id)
        {
            if (!id_map.ContainsKey(mavname))
                id_map[mavname] = id;
        }

        public void add_name_map(string mavname, string title)
        {
            if (!name_map.ContainsKey(mavname))
                name_map[mavname] = title;
        }

        public string get_widget_title(string mavname)
        {
            if (name_map.ContainsKey(mavname))
                return name_map[mavname];
            else
                return mavname;
        }

        public int get_widget_id(string mavname)
        {
            if (id_map.ContainsKey(mavname))
                return id_map[mavname];
            else
                return -1;
        }

        /*
        public void set_known_titles()
        {
            foreach (KeyValuePair<string, Dictionary<int, Widget>> w1 in widgets)
            {
                foreach (KeyValuePair<int, Widget> w2 in w1.Value)
                {
                    if (name_map.ContainsKey(w2.Value.name))
                        w2.Value.title = name_map[w2.Value.name];
                }
            }
        }*/


        public Widget[] get_widget_list(int tab = -1)
        {
            List<Widget> result = new List<Widget>() { };
            foreach (KeyValuePair<string, Dictionary<int, Widget>> w1 in widgets)
            {
                foreach (KeyValuePair<int, Widget> w2 in w1.Value)
                {
                    if (tab != -1 && w2.Value.tab == tab)
                        result.Add(w2.Value);
                }
            }
            return result.ToArray();
        }

        public Widget get_widget(string mavname_uid)
        {
            foreach (KeyValuePair<string, Dictionary<int, Widget>> w1 in widgets)
            {
                foreach (KeyValuePair<int, Widget> w2 in w1.Value)
                {
                    if (w2.Value.name + w2.Value.uid == mavname_uid)
                        return w2.Value;
                }
            }
            return null;
        }

        public Widget add_widget(string mavname)
        {
            int uid;
            if (widgets.ContainsKey(mavname))
            {
                int[] uids = widgets[mavname].Keys.ToArray();
                for (uid = 0; uid < 10; uid++)
                {
                    if (!uids.Contains(uid))
                        break;
                }
                if (uid == 10)
                    return null;
            } else
            {
                widgets[mavname] = new Dictionary<int, Widget>();
                uid = 0;
            }

            Widget w = new Widget(this, mavname, uid);
            widgets[mavname][uid] = w;
            return w;
        }

        public void remove_widget(string mavname_uid)
        {
            string mavname = mavname_uid.Substring(0, mavname_uid.Length - 1);
            int uid = (int)mavname_uid.ElementAt(mavname_uid.Length - 1) - '0';
            widgets[mavname].Remove(uid);
        }

        public void parse_mavcfg(string value)
        {
            string[] entry = value.Split('=');
            if (entry.Count() < 2)
                return;

            string val = entry.ElementAt(1).Trim();
            double dval = Convert.ToDouble(val, CultureInfo.InvariantCulture.NumberFormat);

            entry = entry.ElementAt(0).Trim().Split('_');
            string mavname_uid = entry.ElementAt(0).Trim();
            string param = entry.ElementAt(1).Trim();

            string mavname = mavname_uid.Substring(0, mavname_uid.Length - 1);
            int uid = (int)mavname_uid.ElementAt(mavname_uid.Length - 1) - '0';

            if (!widgets.ContainsKey(mavname))
            {
                widgets[mavname] = new Dictionary<int, Widget>();
            }
            if (!widgets[mavname].ContainsKey(uid))
                widgets[mavname][uid] = new Widget(this, mavname, uid);

            switch (param)
            {
                case "TAB":
                    widgets[mavname][uid].tab = (int)dval;
                    break;
                case "X":
                    widgets[mavname][uid].x = (int)dval;
                    break;
                case "Y":
                    widgets[mavname][uid].y = (int)dval;
                    break;
                case "HJUST":
                    widgets[mavname][uid].h = (int)dval;
                    break;
                case "VJUST":
                    widgets[mavname][uid].v = (int)dval;
                    break;
                case "MODE":
                    widgets[mavname][uid].mode = (int)dval;
                    break;
                case "SOURCE":
                    widgets[mavname][uid].source = (int)dval;
                    break;
                case "UNITS":
                    widgets[mavname][uid].units = (int)dval;
                    break;
                case "PARAM1":
                    widgets[mavname][uid].param[0] = (int)dval;
                    break;
                case "PARAM2":
                    widgets[mavname][uid].param[1] = (int)dval;
                    break;
                case "PARAM3":
                    widgets[mavname][uid].param[2] = (int)dval;
                    break;
                case "PARAM4":
                    widgets[mavname][uid].param[3] = (int)dval;
                    break;
                default:
                    break;
            }
        }

    }

    class Widget
    {
        public Widget(Widgets parent, string mavname, int uid)
        {
            this.parent = parent;
            this.name = mavname;
            this.uid = uid;
        }
        ~Widget()
        {

        }

        Widgets parent;

        /* title */
        //public string title;
        /* mavlink name */
        public string name;
        /* global ID */
        //public int id;
        /* local/unique ID */
        public int uid;

        /* config */
        public int tab;
        public int x, y, h, v;
        public int mode, source, units;
        public int[] param = new int[4];

        public string get_title()
        {
            return parent.get_widget_title(this.name);
        }

        public int get_id()
        {
            return parent.get_widget_id(this.name);
        }

        public string get_iduid()
        {
            return get_id() + "+" + this.uid;
        }

        /*
        Canvas canvas;

        private struct Canvas
        {
            public byte[] bitmap;
            public int width;
            public int height;
            public int x0;
            public int y0;

            public bool shown;
        }
        */

    }
}
