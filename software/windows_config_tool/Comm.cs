using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.IO;
using System.IO.Ports;
using MavLink;


namespace AlceOSD
{
    public class Comm
    {

        public Comm()
        {
            serial_port.BaudRate = 115200;
            serial_port.DataBits = 8;
            serial_port.StopBits = StopBits.One;
            serial_port.Parity = Parity.None;
            serial_port.DtrEnable = false;
            serial_port.RtsEnable = false;
            serial_port.ReadTimeout = 5000;

            mode = COMM_MODE.Serial;
        }

        ~Comm()
        {
            Close();
        }


        private static SerialPort serial_port = new SerialPort();
        public Mavlink mav = new Mavlink();

        private byte[] buffer = new byte[0];
        public Mutex m = new Mutex();
        public Mutex send_m = new Mutex();


        private void AppendToBuffer(byte[] inbuf)
        {
            int ilen = inbuf.Length;
            m.WaitOne();
            int blen = buffer.Length;
            Array.Resize(ref buffer, blen + ilen);
            Array.Copy(inbuf, 0, buffer, blen, ilen);
            m.ReleaseMutex();
        }


        private void Mav_PacketReceived(object sender, MavlinkPacket e)
        {
            //Console.WriteLine("-- incomming packet type {0} --", e.Message.GetType());
            MavlinkMessage m = e.Message;

            if (e.Message.GetType() == typeof(MavLink.Msg_heartbeat))
            {
                Msg_heartbeat hb = (m as Msg_heartbeat);
                Console.WriteLine("heartbeat from [{3};{4}] type={0} mav_version={1} autopilot={2}",
                        hb.type, hb.mavlink_version, hb.autopilot, e.SystemId, e.ComponentId);
            }
            else if (e.Message.GetType() == typeof(MavLink.Msg_serial_control))
            {
                Msg_serial_control sc = (m as Msg_serial_control);

                if (sc.device == 9)
                {
                    byte[] inbuf = new byte[sc.count];
                    Array.Copy(sc.data, inbuf, sc.count);
                    AppendToBuffer(inbuf);
                }
            }
        }

        public bool mav_packet_send(byte[] packet)
        {
            //Console.Write("mav_packet_send() [{0}] ", packet.Length);
            //packet.ToList().ForEach(_b => Console.Write(" {0:X}", _b));
            //Console.WriteLine("");
            bool ret = false;
            m.WaitOne();
            try
            {
                serial_port.Write(packet, 0, packet.Length);
            }
            catch
            {
                ret = true;
            }            
            m.ReleaseMutex();
            return ret;
        }


        private Mavlink tlog_mav = new Mavlink();
        public int progress = 0;
        public int seek = -1;

        private Thread tlog_thread;
        private bool tlog_active = false;
        private byte[] tlog_buf = new byte[0];
        void tlog()
        {
            tlog_mav.PacketReceived += Tlogmav_PacketReceived;
            int left = tlog_buf.Length;
            int total = left;
            int cnt = 20;
            int idx = 0;
            Console.WriteLine("got {0} .tlog bytes", left);
            do
            {
                if (seek != -1)
                {
                    idx = (seek * total) / 100;
                    left = total - idx;
                    seek = -1;
                }

                int c = Math.Min(left, cnt);
                byte[] b = new byte[cnt];
                Array.Copy(tlog_buf, idx, b, 0, c);
                tlog_mav.ParseBytes(b);
                idx += c;
                left -= c;

                progress = (idx * 100) / total;

                if (!tlog_active)
                    break;
            } while (left > 0);
            tlog_mav.PacketReceived -= Tlogmav_PacketReceived;
        }


        private void Tlogmav_PacketReceived(object sender, MavlinkPacket e)
        {
            //Console.WriteLine("will send packet from [{0};{1}] type {2}", e.SystemId, e.ComponentId, e.Message);
            byte[] b = tlog_mav.Send(e);
            if (mav_packet_send(b))
            {
                tlog_active = false;
            }

            System.Threading.Thread.Sleep(10);
            //Console.WriteLine("sent .tlog packet");
        }


        public void send_tlog(byte[] buf)
        {
            if (!IsOpen)
                return;

            Array.Resize(ref tlog_buf, buf.Length);
            Array.Copy(buf, tlog_buf, buf.Length);

            seek = -1;
            progress = 0;
            tlog_active = true;
            tlog_thread = new Thread(new ThreadStart(tlog));
            tlog_thread.Start();
        }

        public void stop_tlog()
        {
            if (tlog_active)
            {
                tlog_active = false;
                if (tlog_thread.IsAlive)
                    tlog_thread.Join();
            }
        }



        private Thread read_thread;
        public bool IsOpen = false;

        private bool run;

        void thr()
        {
            while (run)
            {
                int avail;
                try
                {
                    avail = serial_port.BytesToRead;
                }
                catch
                {
                    avail = 0;
                }
                


                if ( avail > 0)
                {
                    int len = serial_port.BytesToRead;
                    byte[] b = new byte[len];
                    serial_port.Read(b, 0, len);

                    if (mode == COMM_MODE.Serial)
                        AppendToBuffer(b);
                    else
                        mav.ParseBytes(b);
                }
                else
                {
                    Thread.Sleep(1);
                }
            }
        }



        public void Open()
        {
            if (!IsOpen)
            {
                serial_port.Open();
                read_thread = new Thread(new ThreadStart(thr));
                run = true;
                mav.PacketReceived += Mav_PacketReceived;
                read_thread.Start();
                IsOpen = true;
            }
                
        }

        public void Close()
        {
            if (IsOpen)
            {
                if (mode == COMM_MODE.Mavlink)
                {
                    stop_tlog();
                    byte[] dummy = new byte[1] { (byte) '\n' };
                    mavlink_send(dummy, true);
                }

                run = false;
                mav.PacketReceived -= Mav_PacketReceived;
                read_thread.Join();
                IsOpen = false;
                serial_port.Close();
            }

        }

        public enum COMM_MODE : uint
        {
            Serial = 0,
            Mavlink = 1,
        }
        public COMM_MODE mode
        {
            set { _comm_mode = value; }
            get { return _comm_mode; }
        }
        COMM_MODE _comm_mode = COMM_MODE.Serial;


        private byte[] mav_buf = new byte[0];

        public int BytesToRead
        {
            get { return buffer.Length; }
        }

        public int BaudRate { set { serial_port.BaudRate = value;  } get { return serial_port.BaudRate;  } }
        public int DataBits { set { serial_port.BaudRate = value; } get { return serial_port.BaudRate; } }
        public StopBits StopBits { set { serial_port.StopBits = value; } get { return serial_port.StopBits; } }
        public Parity Parity { set { serial_port.Parity = value; } get { return serial_port.Parity; } }
        public bool DtrEnable { set { serial_port.DtrEnable = value; } get { return serial_port.DtrEnable; } }
        public bool RtsEnable { set { serial_port.RtsEnable = value; } get { return serial_port.RtsEnable; } }
        public int ReadTimeout { set { serial_port.ReadTimeout = value; } get { return serial_port.ReadTimeout; } }
        public int WriteTimeout { set { serial_port.WriteTimeout = value; } get { return serial_port.WriteTimeout; } }
        public string PortName { set { serial_port.PortName = value; } get { return serial_port.PortName; } }


        public string ReadExisting()
        {
            string ret = "";
            m.WaitOne();
            if (buffer.Length > 0)
            {
                ret = System.Text.Encoding.UTF8.GetString(buffer, 0, buffer.Length);
                buffer = new byte[0];
            }
            m.ReleaseMutex();
            return ret;
        }

        public void Read(byte[] buf, int offset, int len)
        {
            int timeout = serial_port.ReadTimeout;
            while (buffer.Length < len)
            {
                Thread.Sleep(1);
                if (--timeout == 0)
                {
                    var exp = new Exception("Timeout waiting for chars");
                    throw exp;
                }
            }

            Array.Copy(buffer, 0, buf, offset, len);

            m.WaitOne();
            byte[] new_buf = new byte[buffer.Length - len];
            Array.Copy(buffer, len, new_buf, 0, new_buf.Length);
            buffer = new_buf;
            m.ReleaseMutex();
        }

        public string ReadLine()
        {
            int i, timeout = serial_port.ReadTimeout;
            do
            {
                i = Array.IndexOf(buffer, (byte) 10);
                System.Threading.Thread.Sleep(1);
                if (--timeout == 0)
                {
                    throw new Exception("Timeout waiting for chars");
                }
            } while (i < 0);

            string tmp = System.Text.Encoding.UTF8.GetString(buffer, 0, i);
            m.WaitOne();
            byte[] new_buf = new byte[buffer.Length - i - 1];
            Array.Copy(buffer, i+1, new_buf, 0, new_buf.Length);
            buffer = new_buf;
            m.ReleaseMutex();
            return tmp;
        }

        public char ReadChar()
        {
            int i, timeout = serial_port.ReadTimeout;
            do
            {
                i = buffer.Length;
                System.Threading.Thread.Sleep(1);
                if (--timeout == 0)
                {
                    throw new Exception("Timeout waiting for chars");
                }
            } while (i == 0);
            m.WaitOne();
            char c = (char)buffer[0];
            byte[] new_buf = new byte[buffer.Length - 1];
            Array.Copy(buffer, 1, new_buf, 0, new_buf.Length);
            buffer = new_buf;
            m.ReleaseMutex();
            return c;
        }

        private void mavlink_send(byte[] buf, bool close)
        {
            MavlinkPacket p = new MavlinkPacket();
            Msg_serial_control sc = new Msg_serial_control();

            int left = buf.Length;
            int idx = 0;

            do
            {
                int c = Math.Min(70, left);
                sc.data = new byte[c]; // System.Text.Encoding.UTF8.GetBytes(cmd);
                Array.Copy(buf, idx, sc.data, 0, c);
                sc.count = (byte) c;
                sc.device = 9;
                sc.baudrate = (uint)(close ? 0 : 1);

                //Console.WriteLine("mavlink_send() len={0} buf='{1}'",
                //    sc.count, System.Text.Encoding.UTF8.GetString(buf));

                p.Message = sc;
                p.SystemId = 201;
                p.ComponentId = 0;
                byte[] packet = mav.Send(p);
                serial_port.Write(packet, 0, packet.Length);

                idx += c;
                left -= c;

            } while (left > 0);
        }


        public void DiscardInBuffer()
        {
            buffer = new byte[0];
        }

        public void Write(byte[] buf, int offset, int len)
        {
            if (mode == COMM_MODE.Serial)
                serial_port.Write(buf, offset, len);
            else
            {
                byte[] b = new byte[len];
                Array.Copy(buf, b, len);
                mavlink_send(b, false);
            }

        }
        public void Write(string buf)
        {
            if (mode == COMM_MODE.Serial)
                serial_port.Write(buf);
            else
            {
                byte[] b = System.Text.Encoding.UTF8.GetBytes(buf);
                mavlink_send(b, false);
            }
        }
        public void WriteLine(string buf)
        {
            if (mode == COMM_MODE.Serial)
                serial_port.Write(buf + "\n");
            else
            {
                byte[] b = System.Text.Encoding.UTF8.GetBytes(buf + "\n");
                mavlink_send(b, false);
            }
        }
    }
}
