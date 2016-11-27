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
        private Mavlink mav = new Mavlink();

        private byte[] buffer = new byte[0];
        public Mutex m = new Mutex();

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
            Console.WriteLine("-- incomming packet type {0} --", e.Message.GetType());
            MavlinkMessage m = e.Message;

            if (e.Message.GetType() == typeof(MavLink.Msg_heartbeat))
            {
                Msg_heartbeat hb = (m as Msg_heartbeat);
                Console.WriteLine("heartbeat: type={0} version={1} autopilot={2}", hb.type, hb.mavlink_version, hb.autopilot);
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


        private Thread read_thread;
        public bool IsOpen = false;

        private bool run;

        void thr()
        {
            while (run)
            {
                if (serial_port.BytesToRead > 0)
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
        public string PortName { set { serial_port.PortName = value; } get { return serial_port.PortName; } }


        public string ReadExisting()
        {
            string ret = "";
            m.WaitOne();
            if (buffer.Length > 0)
            {
                ret = System.Text.Encoding.Default.GetString(buffer, 0, buffer.Length);
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
                byte LF = 10;
                i = Array.IndexOf(buffer, LF);
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
            char c = (char) buffer[0];
            m.WaitOne();
            byte[] new_buf = new byte[buffer.Length - 1];
            Array.Copy(buffer, new_buf, new_buf.Length);
            buffer = new_buf;
            m.ReleaseMutex();
            return c;
        }

        private void mavlink_send(byte[] buf)
        {
            MavlinkPacket p = new MavlinkPacket();
            Msg_serial_control sc = new Msg_serial_control();
            sc.data = new byte[buf.Length]; // System.Text.Encoding.UTF8.GetBytes(cmd);

            Array.Copy(buf, sc.data, buf.Length);
            sc.count = (byte) buf.Length;
            sc.device = 9;

            p.Message = sc;
            p.SystemId = 254;
            p.ComponentId = 0;
            byte[] packet = mav.Send(p);
            serial_port.Write(packet, 0, packet.Length);
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
                mavlink_send(b);
            }

        }
        public void Write(string buf)
        {
            if (mode == COMM_MODE.Serial)
                serial_port.Write(buf);
            else
            {
                byte[] b = System.Text.Encoding.UTF8.GetBytes(buf);
                mavlink_send(b);
            }
        }
        public void WriteLine(string buf)
        {
            if (mode == COMM_MODE.Serial)
                serial_port.WriteLine(buf);
            else
            {
                byte[] b = System.Text.Encoding.UTF8.GetBytes(buf + "\n");
                mavlink_send(b);
            }
        }
    }
}
