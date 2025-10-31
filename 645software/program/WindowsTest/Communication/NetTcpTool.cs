using System;
using System.Text;
using System.Net.Sockets;
using System.Net;
using System.IO;
using System.Runtime.InteropServices;
namespace WindowsTest
{
	public class NetTcpTool
	{
		private static NetworkStream m_NetStream;

		public static IPAddress GetLocalIPV4()
		{
			IPAddress[] ips = Dns.GetHostEntry(Dns.GetHostName()).AddressList;
			//遍历获得的IP集以得到IPV4地址
			foreach (IPAddress ip in ips)
			{
				//筛选出IPV4地址
				if (ip.AddressFamily == AddressFamily.InterNetwork)
				{
					return ip;
				}
			}
			//如果没有则返回IPV6地址
			return ips[0];
		}
		/**/
		/**/
		/**/
		/// <summary>
		/// 连接使用 tcp 协议的服务端
		/// </summary>
		/// <param name="ip">服务端的ip地址</param>
		/// <param name="port">服务端的端口号</param>
		/// <returns></returns>
		public NetTcpTool()
		{
		}
		public NetTcpTool(Socket socket)
		{
			m_NetStream = new NetworkStream(socket);
			m_NetStream.ReadTimeout = -1;
			m_NetStream.WriteTimeout = -1;
			socket.Blocking = false;
		}
		/**/
		/**/
		/// <summary>
		/// 向远程主机发送数据
		/// </summary>
		/// <param name="socket">要发送数据且已经连接到远程主机的 Socket</param>
		/// <param name="buffer">待发送的数据</param>
		/// <param name="outTime">发送数据的超时时间，以秒为单位，可以精确到微秒</param>
		/// <returns>0:发送数据成功；-1:超时；-2:发送数据出现错误；-3:发送数据时出现异常</returns>
		/// <remarks >
		/// 当 outTime 指定为-1时，将一直等待直到有数据需要发送
		/// </remarks>

		public int SendData(Socket socket, byte[] buffer, int outTime)
		{
			if (socket == null || socket.Connected == false)
			{
				return -3;
				//throw new ArgumentException("参数socket 为null，或者未连接到远程计算机");
			}
			if (buffer == null || buffer.Length == 0)
			{
				return -3;
				throw new ArgumentException("参数buffer 为null ,或者长度为 0");
			}

			int flag = 0;
			try
			{
				int left = buffer.Length;
				int sndLen = 0;
				int recLen = socket.Available;
				if (recLen > 0)
				{
					byte[] tmp = new byte[recLen];
					socket.Receive(tmp);
				}
				while (true)
				{
					if ((socket.Poll(outTime * 1000, SelectMode.SelectWrite) == true))
					{   // 收集了足够多的传出数据后开始发送
						sndLen = socket.Send(buffer, sndLen, left, SocketFlags.None);
						left -= sndLen;
						if (left == 0)
						{                                        // 数据已经全部发送
							flag = 0;
							break;
						}
						else
						{
							if (sndLen > 0)
							{                                    // 数据部分已经被发送
								continue;
							}
							else
							{                                    // 发送数据发生错误
								flag = -2;
								break;
							}
						}
					}
					else
					{                                            // 超时退出
						flag = -1;
						break;
					}
				}
			}
			catch (SocketException e)
			{
				//LogTool.LogThis(@e.ToString(), eloglevel.error);
				flag = -3;
			}
			return flag;
		}
		/*
        public int SendData(Socket socket, byte[] buffer, int outTime)
        {
            int sndLen;
            if (socket == null || socket.Connected == false)
            {
                return -1;
                //throw new ArgumentException("参数socket 为null，或者未连接到远程计算机");
            }
            if (buffer == null || buffer.Length == 0)
            {
                return -1;
                throw new ArgumentException("参数buffer 为null ,或者长度为 0");
            }
            try
            {
                socket.NoDelay = true;
                sndLen = 0;
                int recLen = socket.Available;
                if (recLen > 0)
                {
                    byte[] tmp = new byte[recLen];
                    socket.Receive(tmp);
                }
                sndLen = socket.Send(buffer, 0, buffer.Length, SocketFlags.None);
            }
            catch (SocketException e)
            {
                LogTool.LogThis(@e.ToString(), eloglevel.error);
                //打印socket错误
                String logStr = "e.message:" + e.Message.ToString() + " e.ErrorCode:" + e.ErrorCode.ToString();
                LogTool.LogThis(@e.ToString(), eloglevel.error);
                sndLen = -1;
            }
            finally
            {
            }
            
            return sndLen;
        }
        */
		/**/
		/**/
		/**/
		/// <summary>
		/// 向远程主机发送数据
		/// </summary>
		/// <param name="socket">要发送数据且已经连接到远程主机的 Socket</param>
		/// <param name="buffer">待发送的字符串</param>
		/// <param name="outTime">发送数据的超时时间，以秒为单位，可以精确到微秒</param>
		/// <returns>0:发送数据成功；-1:超时；-2:发送数据出现错误；-3:发送数据时出现异常</returns>
		/// <remarks >
		/// 当 outTime 指定为-1时，将一直等待直到有数据需要发送
		/// </remarks>
		public int SendData(Socket socket, string buffer, int outTime)
		{
			if (buffer == null || buffer.Length == 0)
			{
				throw new ArgumentException("待发送的字符串长度不能为零.");
			}
			return (SendData(socket, System.Text.Encoding.Default.GetBytes(buffer), outTime));
		}
		/**/
		/**/
		/**/
		/// <summary>
		/// 接收远程主机发送的数据
		/// </summary>
		/// <param name="socket">要接收数据且已经连接到远程主机的 socket</param>
		/// <param name="buffer">接收数据的缓冲区</param>
		/// <param name="outTime">接收数据的超时时间，以秒为单位，可以精确到微秒</param>
		/// <returns>0:接收数据成功；-1:超时；-2:接收数据出现错误；-3:接收数据时出现异常</returns>
		/// <remarks >
		/// 1、当 outTime 指定为-1时，将一直等待直到有数据需要接收；
		/// 2、需要接收的数据的长度，由 buffer 的长度决定。
		/// </remarks>
		/*
        public  int RecvData(Socket socket, byte[] buffer, int outTime)
        {
            if (socket == null || socket.Connected == false)
            {
                return -3;
               // throw new ArgumentException("参数socket 为null，或者未连接到远程计算机");
            }
            if (buffer == null || buffer.Length == 0)
            {
                return -3;
               // throw new ArgumentException("参数buffer 为null ,或者长度为 0");
            }
            buffer.Initialize();
            int left = buffer.Length;
            int curRcv = 0;
            int flag = 0;

            try
            {
                while (true)
                {
                    if (socket.Poll(outTime * 1000, SelectMode.SelectRead) == true)
                    {        // 已经有数据等待接收
                        curRcv = socket.Receive(buffer, curRcv, left, SocketFlags.None);
                        left -= curRcv;
                        if (left == 0)
                        {                                    // 数据已经全部接收 
                            flag = 0;
                            break;
                        }
                        else
                        {
                            if (curRcv > 0)
                            {                                // 数据已经部分接收
                                continue;
                            }
                            else
                            {                                            // 出现错误
                                flag = -2;
                                break;
                            }
                        }
                    }
                    else
                    {                                                    // 超时退出
                        flag = -1;
                        break;
                    }
                }
            }
            catch (SocketException e)
            {
                LogTool.LogThis(@e.ToString(), eloglevel.error);
                //打印socket错误
                String logStr = "e.message:" + e.Message.ToString() + " e.ErrorCode:" +e.ErrorCode.ToString();
                LogTool.LogThis(@e.ToString(), eloglevel.error);
                flag = -3;
            }
            return flag;
        }
         * */
		public int RecvData(Socket socket, byte[] buffer, int outTime)
		{
			if (socket == null || socket.Connected == false)
			{
				return -2;
				// throw new ArgumentException("参数socket 为null，或者未连接到远程计算机");
			}
			if (buffer == null || buffer.Length == 0)
			{
				return -1;
				// throw new ArgumentException("参数buffer 为null ,或者长度为 0");
			}
			buffer.Initialize();
			int left = 0;
			int curRcv = 0;
			int ioutTime = 0;
			try
			{
				while (ioutTime < outTime)
				{
					if (socket.Poll(50 * 1000, SelectMode.SelectRead) == true)
					{
						int recLen = socket.Available;
						if (recLen > 0)
						{
							curRcv = socket.Receive(buffer, curRcv, recLen, SocketFlags.None);
							left += curRcv;
						}
						if (left > 20)
							break;
					}
					ioutTime += 50;
					//System.Threading.Thread.Sleep(50);//延时  
				}
			}
			catch (SocketException e)
			{
				//打印socket错误
				left = -3;
			}
			finally
			{

			}
			return left;
		}
		/**/
		/**/
		/**/
		/// <summary>
		/// 接收远程主机发送的数据
		/// </summary>
		/// <param name="socket">要接收数据且已经连接到远程主机的 socket</param>
		/// <param name="buffer">存储接收到的数据的字符串</param>
		/// <param name="bufferLen">待接收的数据的长度</param>
		/// <param name="outTime">接收数据的超时时间，以秒为单位，可以精确到微秒</param>
		/// <returns>0:接收数据成功；-1:超时；-2:接收数据出现错误；-3:接收数据时出现异常</returns>
		/// <remarks >
		/// 当 outTime 指定为-1时，将一直等待直到有数据需要接收；
		/// </remarks>
		public int RecvData(Socket socket, ref string buffer, int bufferLen, int outTime)
		{
			//buffer = RecvData(socket,outTime);
			//return 0;
			if (bufferLen <= 0)
			{
				throw new ArgumentException("存储待接收数据的缓冲区长度必须大于0");
			}
			byte[] tmp = new byte[bufferLen];
			int flag = 0;
			//.if ((flag = RecvData(socket, tmp, outTime)) == 0)
			{
				flag = RecvData(socket, tmp, outTime);
				buffer = System.Text.Encoding.Default.GetString(tmp).TrimEnd('\0');
			}
			return flag;
		}


		/**/
		/**/
		/**/
		/// <summary>
		/// 向远程主机发送文件
		/// </summary>
		/// <param name="socket" >要发送数据且已经连接到远程主机的 socket</param>
		/// <param name="fileName">待发送的文件名称</param>
		/// <param name="maxBufferLength">文件发送时的缓冲区大小</param>
		/// <param name="outTime">发送缓冲区中的数据的超时时间</param>
		/// <returns>0:发送文件成功；-1:超时；-2:发送文件出现错误；-3:发送文件出现异常；-4:读取待发送文件发生错误</returns>
		/// <remarks >
		/// 当 outTime 指定为-1时，将一直等待直到有数据需要发送
		/// </remarks>
		public int SendFile(Socket socket, string fileName, int maxBufferLength, int outTime)
		{
			if (fileName == null || maxBufferLength <= 0)
			{
				throw new ArgumentException("待发送的文件名称为空或发送缓冲区的大小设置不正确.");
			}

			int flag = 0;
			try
			{
				FileStream fs = new FileStream(fileName, FileMode.Open, FileAccess.Read);
				long fileLen = fs.Length;                        // 文件长度
				long leftLen = fileLen;                            // 未读取部分
				int readLen = 0;                                // 已读取部分
				byte[] buffer = null;

				if (fileLen <= maxBufferLength)
				{            /**//**//**//* 文件可以一次读取*/
					buffer = new byte[fileLen];
					readLen = fs.Read(buffer, 0, (int)fileLen);
					flag = SendData(socket, buffer, outTime);
				}
				else
				{                                    /**//**//**//* 循环读取文件,并发送 */
					buffer = new byte[maxBufferLength];
					while (leftLen != 0)
					{
						readLen = fs.Read(buffer, 0, maxBufferLength);
						if ((flag = SendData(socket, buffer, outTime)) < 0)
						{
							break;
						}
						leftLen -= readLen;
					}
				}
				fs.Close();
			}
			catch (IOException e)
			{
				flag = -4;
			}
			return flag;
		}

		/**/
		/**/
		/**/
		/// <summary>
		/// 向远程主机发送文件
		/// </summary>
		/// <param name="socket" >要发送数据且已经连接到远程主机的 socket</param>
		/// <param name="fileName">待发送的文件名称</param>
		/// <returns>0:发送文件成功；-1:超时；-2:发送文件出现错误；-3:发送文件出现异常；-4:读取待发送文件发生错误</returns>
		public int SendFile(Socket socket, string fileName)
		{
			return SendFile(socket, fileName, 2048, 1);
		}


		/**/
		/**/
		/**/
		/// <summary>
		/// 接收远程主机发送的文件
		/// </summary>
		/// <param name="socket">待接收数据且已经连接到远程主机的 socket</param>
		/// <param name="fileName">保存接收到的数据的文件名</param>
		/// <param name="fileLength" >待接收的文件的长度</param>
		/// <param name="maxBufferLength">接收文件时最大的缓冲区大小</param>
		/// <param name="outTime">接受缓冲区数据的超时时间</param>
		/// <returns>0:接收文件成功；-1:超时；-2:接收文件出现错误；-3:接收文件出现异常；-4:写入接收文件发生错误</returns>
		/// <remarks >
		/// 当 outTime 指定为-1时，将一直等待直到有数据需要接收
		/// </remarks>
		public int RecvFile(Socket socket, string fileName, long fileLength, int maxBufferLength, int outTime)
		{
			if (fileName == null || maxBufferLength <= 0)
			{
				throw new ArgumentException("保存接收数据的文件名称为空或发送缓冲区的大小设置不正确.");
			}

			int flag = 0;
			try
			{
				FileStream fs = new FileStream(fileName, FileMode.Create);
				byte[] buffer = null;

				if (fileLength <= maxBufferLength)
				{                /**//**//**//* 一次读取所传送的文件 */
					buffer = new byte[fileLength];
					if ((flag = RecvData(socket, buffer, outTime)) == 0)
					{
						fs.Write(buffer, 0, (int)fileLength);
					}
				}
				else
				{                                        /**//**//**//* 循环读取网络数据，并写入文件 */
					int rcvLen = maxBufferLength;
					long leftLen = fileLength;                        //剩下未写入的数据
					buffer = new byte[rcvLen];

					while (leftLen != 0)
					{
						if ((flag = RecvData(socket, buffer, outTime)) < 0)
						{
							break;
						}
						fs.Write(buffer, 0, rcvLen);
						leftLen -= rcvLen;
						rcvLen = (maxBufferLength < leftLen) ? maxBufferLength : ((int)leftLen);
					}
				}
				fs.Close();
			}
			catch (IOException e)
			{
				flag = -4;
			}
			return flag;
		}

		/**/
		/**/
		/**/
		/// <summary>
		/// 接收远程主机发送的文件
		/// </summary>
		/// <param name="socket">待接收数据且已经连接到远程主机的 socket</param>
		/// <param name="fileName">保存接收到的数据的文件名</param>
		/// <param name="fileLength" >待接收的文件的长度</param>
		/// <returns>0:接收文件成功；-1:超时；-2:接收文件出现错误；-3:接收文件出现异常；-4:写入接收文件发生错误</returns>
		public int RecvFile(Socket socket, string fileName, long fileLength)
		{
			return RecvFile(socket, fileName, fileLength, 2048, 1);
		}

		[DllImport("Iphlpapi.dll")]
		private static extern int SendARP(Int32 dest, Int32 host, ref Int64 mac, ref Int32 length);
		[DllImport("Ws2_32.dll")]
		private static extern Int32 inet_addr(string ip);

		public static string getRemoteMac(string localIP, string remoteIP)
		{
			Int32 ldest = inet_addr(remoteIP); //目的ip 
			Int32 lhost = inet_addr(localIP); //本地ip 

			try
			{
				Int64 macinfo = new Int64();
				Int32 len = 6;
				int res = SendARP(ldest, 0, ref macinfo, ref len);
				return strConvert(Convert.ToString(macinfo, 16).ToUpper());
			}
			catch (Exception err)
			{
				Console.WriteLine("Error:{0}", err.Message);
			}
			return 0.ToString();
		}

		private static string strConvert(string p)
		{
			StringBuilder str = new StringBuilder();
			for (int i = p.Length - 1; i >= 0; i = i - 2)
			{
				str.Append(p[i - 1]);
				str.Append(p[i]);
			}
			return str.ToString();
		}
		internal int SendData(string str, int p)
		{
			byte[] byteArray = System.Text.Encoding.Default.GetBytes(str);
			if ((m_NetStream != null) && (m_NetStream.CanWrite))
			{
				m_NetStream.Write(byteArray, 0, byteArray.Length);
				//isSendOK = true;
			}
			//
			return 0;
		}

		public byte[] RecvData(int outTime)
		{
			byte[] bData = new byte[4096];
			int iReadLen = 1;
			try
			{

				while (iReadLen > 0)
				{
					if (m_NetStream.CanRead)
					{
						if (m_NetStream.DataAvailable)
						{
							iReadLen = m_NetStream.Read(bData, 0, 2048);
							if (iReadLen > 0)//接收到客户端消息后触发事件将消息回传
							{
								//return System.Text.Encoding.Default.GetString(getDataPart(bData, iReadLen)).ToString();
								byte[] bTempBuf = new byte[iReadLen];
								Array.Copy(bData, bTempBuf, iReadLen);
								return bTempBuf;
							}
						}
					}
					break;
				}
			}
			catch (Exception e)
			{
				//LogTool.LogThis(@e.Message.ToString(), eloglevel.error);
			}
			finally
			{
				//m_NetStream.Close();
			}
			return null;
		}
		public int SendData(string str)
		{
			byte[] byteArray = System.Text.Encoding.Default.GetBytes(str);
			try
			{
				if ((m_NetStream != null) && (m_NetStream.CanWrite))
				{
					m_NetStream.Write(byteArray, 0, byteArray.Length);
					m_NetStream.Flush();
					return byteArray.Length;
				}
			}
			catch (Exception e)
			{
				//LogTool.LogThis(@e.Message.ToString()+":>>>" + str, eloglevel.error);
				return 0;
			}
			//
			return 0;
		}
		public int SendData(byte[] sengBuf)
		{
			try
			{
				if ((m_NetStream != null) && (m_NetStream.CanWrite))
				{
					m_NetStream.Write(sengBuf, 0, sengBuf.Length);
					m_NetStream.Flush();
					return sengBuf.Length;
				}
			}
			catch (Exception e)
			{
				return 0;
			}
			//
			return 0;
		}
		internal string RecvData(Socket socket, int outTime)
		{
			byte[] bData = new byte[1024];
			int iReadLen = 1;
			try
			{
				m_NetStream = new NetworkStream(socket);
				m_NetStream.ReadTimeout = -1;
				while (iReadLen > 0)
				{
					if (m_NetStream.CanRead)
					{
						iReadLen = m_NetStream.Read(bData, 0, 1024);
						if (iReadLen > 0)//接收到客户端消息后触发事件将消息回传
						{
							return System.Text.Encoding.Default.GetString(getDataPart(bData, iReadLen)).ToString();
						};
					}
					else
					{
						break;
					}
				}
			}
			catch (Exception e)
			{
			}
			finally
			{
				m_NetStream.Close();
			}
			return null;
		}
		private byte[] getDataPart(byte[] input, int Len)
		{
			if (input == null)
				return null;

			if (input.Length <= Len)
				return input;

			byte[] result = new byte[Len];
			for (int i = 0; i < Len; i++)
				result[i] = input[i];
			return result;
		}

		public void Dispose()
		{
			if (m_NetStream != null)
			{
				m_NetStream.Close(500);
				m_NetStream.Dispose();
				m_NetStream = null;
			}
		}
	}
}
