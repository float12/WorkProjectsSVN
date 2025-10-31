using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.IO.Ports;
using System.Threading;

namespace WindowsTest
{
    public class SCUProxy
    {
        private SerialPort m_SerialPort=new SerialPort();
        public Thread PThread_Rev;//接收线程
        byte[] SendBuf = new byte[4096];
        int SendPointer = 0;
        byte[] Recvbuf = new byte[4096];
        int RecvPoint = 0;
        bool RecvFinished = false;
        private List<byte> ListBuffer = new List<byte>(10 * 1024); // 缓存待处理的接收数据


        private void EB_Frame_MakeHead()
        {
            SendPointer = 0;
            SendBuf[SendPointer++] = 0xEB;
            SendBuf[SendPointer++] = 0;
            SendBuf[SendPointer++] = 0;
            SendBuf[SendPointer++] = 0xEB;
            SendBuf[SendPointer++] = 0;
            
        }

        private void EB_Frame_MakeEnd()
        {
            int frame_len = (SendPointer - 4) & 0x07FF;
            SendBuf[1] = (byte)((frame_len >> 8) & 0xff);
            SendBuf[2] = (byte)(frame_len & 0xff);
            byte cs = GetSum(SendBuf, SendPointer);
            SendBuf[SendPointer++] = cs;
            SendDataFrame();

        }

        private void SendDataFrame()
        {
            try
            {
                if (SendPointer > 0)
                {
                    m_SerialPort.Write(SendBuf, 0, SendPointer);
                    SendPointer = 0;
                    RecvFinished = false;
                }
            }
            catch (Exception ep)
            {
                
            }

        }

        private void ReceiveData_Tread()
        {
            try
            {
                byte[] RevBuf = new byte[4096 * 2];   //接收的数据
                int RecPointer = 0;
                while (true)
                {
                    System.Threading.Thread.Sleep(1);
                    // 从串口读取数据
                    int n = m_SerialPort.BytesToRead;
                    if (n < 1)
                        continue;
                    try
                    {
                        int Count = m_SerialPort.Read(RevBuf, RecPointer, n);
                        if (Count > 0)
                        {
                            RecPointer += Count;
                        }
                        else
                        {
                            continue;
                        }
                    }
                    catch (Exception ex)
                    {
                        throw new Exception();
                    }
                    if (RecPointer > 0)
                    {
                        //log("R:" + DateTime.Now.ToString("yyy-MM-dd HH:mm:ss:fff"));
                        byte[] tmp = new byte[RecPointer];
                        Array.Copy(RevBuf, tmp, RecPointer);

                        
                        RecPointer = 0;

                        if ((ListBuffer.Count + tmp.Length) > (10 * 1024))
                        {
                            ListBuffer.Clear();
                        }
                        ListBuffer.AddRange(tmp);
                    }

                    while (ListBuffer.Count >= 9)
                    {
                        //帧校验
                        if ((ListBuffer[0] == 0xEB) && (ListBuffer[3] == 0xEB))
                        {
                            int len = ListBuffer[1] << 8 | ListBuffer[2];
                            if (ListBuffer.Count >= (len + 5))//数据长度满足
                            {
                                //校验判断
                                byte[] data = new byte[len + 5];
                                ListBuffer.CopyTo(0, data, 0, len + 5);
                                byte cs = GetSum(data, len + 4);
                                if (cs == data[len + 4]) // CS校验失败
                                {
                                    Array.Copy(data, 0, Recvbuf, 0, len + 5);
                                    RecvPoint = len + 5;
                                    RecvFinished = true;
                                }
                                else
                                {
                                   
                                }

                                //lock (rev_obj)
                                //{
                                ListBuffer.RemoveRange(0, len + 5);
                                //}
                                //return;//循环下一次处理
                            }
                            else
                            {
                                break;//继续接收
                            }
                        }
                        else
                        {
                            //lock (rev_obj)
                            //{
                            ListBuffer.RemoveAt(0);
                            //}
                        }
                    }
                }
            }
            catch
            {
                
            }
        }

        //ret=1 正确返回；ret=0 错误返回，内容为错误码；ret=-1超时或其他错误
        private int RecvDataFrame(ref byte[] buf,ref int len)
        {
            int ret = -1;
            for (int i = 0; i < 10; i++)
            {
                if (RecvFinished == true)
                {
                    ret = 1;
                    Array.Copy(Recvbuf, 8, buf, 0, RecvPoint - 9);
                    len = RecvPoint - 9;
                    if (Recvbuf[7] == 0x00)
                    {
                        ret = 0;
                    }
                    return ret;
                }
                Thread.Sleep(50);
            }
            return ret;
        }
    
        private byte GetSum(byte[] data, int len)
        {
            byte i = 0;
            int j = 0;
            for (j = 0; j < len; j++)
            {
                i += data[j];
            }
            return i;
        }

        private int HY_SendToProxy(byte node_index, byte type, byte content_type, byte[] send, int slen, ref byte[] recv, ref int rlen)
        {
            int ret = -1;
            try
            {
                EB_Frame_MakeHead();
                SendBuf[SendPointer++] = node_index;
                SendBuf[SendPointer++] = type;
                SendBuf[SendPointer++] = content_type;
                if (send != null)
                {
                    Array.Copy(send, 0, SendBuf, SendPointer, slen);
                    SendPointer += slen;
                }
                EB_Frame_MakeEnd();
                ret = RecvDataFrame(ref recv, ref rlen);
                return ret;
            }
            catch
            {
                return ret;
            }
        }
		
        /// <summary>
        /// 
        /// </summary>
        /// <param name="node_index"></param>
        /// <param name="type"></param>
        /// <param name="content_type"></param>
        /// <param name="baSendBytes"></param>
		/// <returns></returns>
		public byte[] HY_SendToProxyBuf(byte node_index, byte type, byte content_type, byte[] baSendBytes = null)
        {
            byte [] baSend = null;
            try
            {
                EB_Frame_MakeHead();
                SendBuf[SendPointer++] = node_index;
                SendBuf[SendPointer++] = type;
                SendBuf[SendPointer++] = content_type;
                if (baSendBytes != null)
                {
                    Array.Copy(baSendBytes, 0, SendBuf, SendPointer, baSendBytes.Length);
                    SendPointer += baSendBytes.Length;
                }
                EB_Frame_MakeEnd();
                baSend = new byte[SendPointer ];
                Array.Copy(SendBuf, 0, baSend, 0, baSend.Length);
                //ret = RecvDataFrame(ref recv, ref rlen);
                return baSend;
            }
            catch
            {
                return baSend;
            }
        }

/// <summary>
/// 
/// </summary>
/// <param name="strType"></param>
/// <returns></returns>
		public string HY_SendToSecModuleRandom(string strType)
        {
            string strRandom= "";
			try
            {
				strRandom = strType;
				strRandom = "1B" + (strRandom.Length / 2).ToString("X4") + strRandom;
                return strRandom;
            }
            catch
            {
                return strRandom;
            }
        }

        public int HY_Open_Dev(SerialPort serial)
        {
            try
            {
                m_SerialPort = serial;
                if (m_SerialPort.IsOpen)//已经被打开
                {
                    return 0;
                }
                m_SerialPort.Open();

                if (PThread_Rev != null)
                {
                    PThread_Rev.Abort();
                    PThread_Rev = null;
                }
                //接收线程
                PThread_Rev = new Thread(new ThreadStart(ReceiveData_Tread));//开始接收线程
                PThread_Rev.IsBackground = true;
                PThread_Rev.Start();

                return 1;
            }
            catch
            {
                return -1;
            }

        }

        public int HY_Close_Dev()
        {
            try
            {
                if (PThread_Rev != null)
                {
                    PThread_Rev.Abort();
                    PThread_Rev = null;
                }
                m_SerialPort.Close();
                return 1;
            }
            catch
            {
                return -1;
            }
        }
                        
        public int HY_GetProxyCer(byte module_index,ref byte[] buf,ref int len, byte part_num)
        {
            if (part_num < 1 || part_num > 3)
                return -1;
            try
            {
                HY_SendToProxy(module_index, 0x13, part_num, null, 0, ref buf, ref len);
                return 1;
            }
            catch
            {
                return -1;
            }
            
        }

        public int HY_GetSecModuleCerRequest(byte module_index, ref byte[] buf, ref int len, byte part_num)
        {
            if (part_num !=4 && part_num !=6 && part_num != 8)
                return -1;
            try
            {
                HY_SendToProxy(module_index, 0x13, part_num, null, 0, ref buf, ref len);
                return 1;
            }
            catch
            {
                return -1;
            }

        }

        public int HY_WriteSecModuleCer(byte module_index, ref byte[] buf, ref int len, byte[] sbuf,int slen, byte part_num)
        {
            int ret = -1;
            if (part_num != 5 && part_num != 7 && part_num != 9)
                return ret;
            try
            {
                ret=HY_SendToProxy(module_index, 0x13, part_num, sbuf, slen, ref buf, ref len);
                return ret;
            }
            catch
            {
                return ret;
            }

        }

        public int HY_AuthExec(byte module_index, ref byte[] buf, ref int len, byte[] sbuf, int slen, byte step)
        {
            int ret = -1;
            if (step < 0 || step > 4) 
                return ret;
            try
            {
                byte content_type = (byte)(step + 0x09);
                ret = HY_SendToProxy(module_index, 0x13, content_type, sbuf, slen, ref buf, ref len);
                return ret;
            }
            catch
            {
                return ret;
            }
        }
        
        public int HY_GetAuthStatus(byte module_index, ref byte[] buf, ref int len)
        {
            int ret = -1;
            try
            {
                ret=HY_SendToProxy(module_index, 0x13, 0x0E, null, 0, ref buf, ref len);
                return ret;                
            }
            catch
            {
                return -1;
            }
        }

        public int HY_CalcMac(byte module_index, ref byte[] buf, ref int len, byte[] sbuf, int slen)
        {
            int ret = -1;
            try
            {
                ret = HY_SendToProxy(module_index, 0x14, 0x01, sbuf, slen, ref buf, ref len);
                return ret;
            }
            catch
            {
                return -1;
            }
		}
		
        public int HY_CheckMac(byte module_index, ref byte[] buf, ref int len, byte[] sbuf, int slen)
        {
            int ret = -1;
            try
            {
                ret = HY_SendToProxy(module_index, 0x14, 0x11, sbuf, slen, ref buf, ref len);
                return ret;
            }
            catch
            {
                return -1;
            }
        }

        public int HY_BroadcaseEncrypt(byte module_index, ref byte[] buf, ref int len,  byte[] sbuf, int slen)
        {
            int ret = -1;
            try
            {
                ret = HY_SendToProxy(module_index, 0x14, 0x02, sbuf, slen, ref buf, ref len);
                return ret;
            }
            catch
            {
                return -1;
            }

        }

    }
}
