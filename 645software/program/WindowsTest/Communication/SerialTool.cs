using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO.Ports;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace WindowsTest
{
	public class SerialTool
	{
		SerialPort _serialPort = null;
		private bool _Working = false;
		/// <summary>
		/// 外部程序用来标识该串口是否被某操作线程占用
		/// </summary>
		public bool Working
		{
			get { return _Working | (!isOpen()); }
			set { _Working = value; }
		}
		//定义委托
		public delegate void SerialPortDataReceiveEventArgs(object sender, SerialDataReceivedEventArgs e, byte[] bits);
		//定义接收数据事件
		public event SerialPortDataReceiveEventArgs DataReceived;
		//定义接收错误事件
		//public event SerialErrorReceivedEventHandler Error;
		//接收事件是否有效 false表示有效
		public bool ReceiveEventFlag = false;

		public List<byte> buffer = new List<byte>(4096);

		// private StringBuilder builder = new StringBuilder();
		// private long received_count = 0;//接收计数
		public int OverTime = 5000;

		#region 获取串口名
		private string protName;
		public string PortName
		{
			get { return _serialPort.PortName; }
			set
			{
				_serialPort.PortName = value;
				protName = value;
			}
		}
		#endregion

		#region 获取比特率
		private int baudRate;
		public int BaudRate
		{
			get { return _serialPort.BaudRate; }
			set
			{
				_serialPort.BaudRate = value;
				baudRate = value;
			}
		}
		#endregion

		#region 默认构造函数
		/// <summary>
		/// 默认构造函数，操作COM1，速度为9600，没有奇偶校验，8位字节，停止位为1 "COM1", 9600, Parity.None, 8, StopBits.One
		/// </summary>
		public SerialTool()
		{
			_serialPort = new SerialPort();
		}
		#endregion

		#region 构造函数
		/// <summary>
		/// 构造函数,
		/// </summary>
		/// <param name="comPortName"></param>
		public SerialTool(string comPortName)
		{
			_serialPort = new SerialPort(comPortName);
			_serialPort.BaudRate = 2400;
			_serialPort.Parity = Parity.Even;
			_serialPort.DataBits = 8;
			_serialPort.StopBits = StopBits.One;
			_serialPort.Handshake = Handshake.None;
			_serialPort.RtsEnable = true;
			_serialPort.ReadTimeout = -1;

			setSerialPort();
		}
		#endregion

		#region 构造函数,可以自定义串口的初始化参数
		/// <summary>
		/// 构造函数,可以自定义串口的初始化参数
		/// </summary>
		/// <param name="comPortName">需要操作的COM口名称</param>
		/// <param name="baudRate">COM的速度</param>
		/// <param name="parity">奇偶校验位</param>
		/// <param name="dataBits">数据长度</param>
		/// <param name="stopBits">停止位</param>
		public SerialTool(string comPortName, int baudRate, Parity parity, int dataBits, StopBits stopBits)
		{
			_serialPort = new SerialPort(comPortName, baudRate, parity, dataBits, stopBits);
			_serialPort.RtsEnable = true;  //自动请求
			_serialPort.ReadTimeout = -1;//超时
			setSerialPort();
		}
		#endregion

		#region 析构函数
		/// <summary>
		/// 析构函数，关闭串口
		/// </summary>
		~SerialTool()
		{
			if (_serialPort.IsOpen)
				_serialPort.Close();
		}
		#endregion

		#region 设置串口参数
		/// <summary>
		/// 设置串口参数
		/// </summary>
		/// <param name="comPortName">需要操作的COM口名称</param>
		/// <param name="baudRate">COM的速度</param>
		/// <param name="dataBits">数据长度</param>
		/// <param name="stopBits">停止位</param>
		public bool setSerialPort(string comPortName, int baudRate, string parity, int dataBits, string stopBits, int overtime)
		{
			bool ok = false;
			if (_serialPort.IsOpen)
				_serialPort.Close();
			_serialPort.PortName = comPortName;
			_serialPort.BaudRate = baudRate;

			switch (parity)
			{
				case "None":
					_serialPort.Parity = Parity.None;
					break;
				case "Even":
					_serialPort.Parity = Parity.Even;
					break;
				case "Odd":
					_serialPort.Parity = Parity.Odd;
					break;
			}
			_serialPort.DataBits = dataBits;
			switch (stopBits)
			{
				case "0":
					_serialPort.StopBits = StopBits.None;
					break;
				case "1":
					_serialPort.StopBits = StopBits.One;
					break;
				case "1.5":
					_serialPort.StopBits = StopBits.OnePointFive;
					break;
				case "2":
					_serialPort.StopBits = StopBits.Two;
					break;
			}
			_serialPort.Handshake = Handshake.None;
			//_serialPort.DtrEnable = true;
			_serialPort.RtsEnable = true;
			_serialPort.ReadTimeout = -1;
			_serialPort.NewLine = "/r/n";
			ok = setSerialPort();
			if (ok)
			{
				OverTime = overtime;
			}
			return ok;
		}
		#endregion

		#region 设置接收函数
		/// <summary>
		/// 设置串口资源,还需重载多个设置串口的函数
		/// </summary>
		public bool setSerialPort()
		{
			bool ok = false;
			if (_serialPort != null)
			{
				//设置触发DataReceived事件的字节数为1
				_serialPort.ReceivedBytesThreshold = 1;
				//接收到一个字节时，也会触发DataReceived事件
				//_serialPort.DataReceived += new SerialDataReceivedEventHandler(_serialPort_DataReceived);
				//接收数据出错,触发事件
				_serialPort.ErrorReceived += new SerialErrorReceivedEventHandler(_serialPort_ErrorReceived);
				//打开串口
				ok = openPort();
			}
			return ok;
		}
		#endregion

		#region 打开串口资源
		/// <summary>
		/// 打开串口资源
		/// <returns>返回bool类型</returns>
		/// </summary>
		public bool openPort()
		{
			bool ok = false;
			//如果串口是打开的，先关闭
			if (_serialPort.IsOpen)
				_serialPort.Close();
			try
			{
				//打开串口
				_serialPort.Open();
				ok = true;
			}
			catch
			{
				ok = false;
			}

			return ok;
		}
		#endregion

		#region 关闭串口
		/// <summary>
		/// 关闭串口资源,操作完成后,一定要关闭串口
		/// </summary>
		public void closePort()
		{
			//如果串口处于打开状态,则关闭
			if (_serialPort.IsOpen)
				_serialPort.Close();
		}
		#endregion

		#region 接收串口数据事件
		/// <summary>
		/// 接收串口数据事件
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		void _serialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
		{
			//禁止接收事件时直接退出
			if (ReceiveEventFlag)
			{
				return;
			}
			try
			{
				//System.Threading.Thread.Sleep(10);
				byte[] _data = new byte[_serialPort.BytesToRead];
				_serialPort.Read(_data, 0, _data.Length);
				if (_data.Length == 0) { return; }
				if (DataReceived != null)
				{
					DataReceived(sender, e, _data);
				}
				buffer.AddRange(_data);
				_serialPort.DiscardInBuffer();  //清空接收缓冲区 
			}
			catch (Exception ex)
			{
				throw ex;
			}
		}
		#endregion

		#region 接收数据出错事件
		/// <summary>
		/// 接收数据出错事件
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		void _serialPort_ErrorReceived(object sender, SerialErrorReceivedEventArgs e)
		{
		}
		#endregion

		#region 发送数据string类型
		public void SendData(string data)
		{
			//发送数据
			//禁止接收事件时直接退出
			if (ReceiveEventFlag)
			{
				return;
			}
			if (_serialPort.IsOpen)
			{
				_serialPort.Write(data);
			}
		}
		#endregion

		#region 发送数据byte类型
		/// <summary>
		/// 数据发送
		/// </summary>
		/// <param name="data">要发送的数据字节</param>
		public void SendData(byte[] data, int offset, int count)
		{
			//禁止接收事件时直接退出
			if (ReceiveEventFlag)
			{
				return;
			}
			try
			{
				if (_serialPort.IsOpen)
				{
					//_serialPort.DiscardInBuffer();//清空接收缓冲区
					_serialPort.Write(data, offset, count);
				}
			}
			catch (Exception ex)
			{
				throw ex;
			}
		}
		#endregion

		public bool isOpen() { return _serialPort.IsOpen; }

		#region 判断完整性
		List<byte> list_Cmd = new List<byte>();

		#region 2015-05-05 最新判断完整性,去掉红外的自发自收
		public bool fnCheckData(List<byte> l, byte[] SendData)
		{
			bool m_bisOK = true;
			try
			{
				list_Cmd = new List<byte>();

				byte[] bCmds = new byte[l.Count];

				l.CopyTo(bCmds, 0);
				string strData = DataConvert.fnGetStringFormat(DataConvert.Bytes2HexString(bCmds, 0, bCmds.Length));
				string strSendData = DataConvert.fnGetStringFormat(DataConvert.Bytes2HexString(SendData, 0, SendData.Length));
				if (strData.Contains(strSendData))
				{
					strData = strData.Replace(strSendData, "");
					if (strData.Length < 10)
					{
						return false;
					}
					bCmds = DataConvert.HEXString2Bytes(strData);
				}
				int iHead = 0;
				while (iHead < bCmds.Length)
				{
					if (bCmds[iHead] != (byte)0x68)
						iHead++;
					else
						break;
				}
				if (iHead > bCmds.Length)
				{
					return false;
				}
				else if (bCmds.Length - iHead < 12)
				{
					return false;
				}
				int iEnd = bCmds.Length - 1;
				while (iEnd > iHead)
				{
					if (bCmds[iEnd] != (byte)0x16)
						iEnd--;
					else
						break;
				}
				if (iEnd <= iHead)
				{
					return false;
				}
				else if ((bCmds[iHead] != (byte)0x68) || (bCmds[iHead + 7] != (byte)0x68))
				{
					return false;
				}
				int iDataLenth = bCmds.Length - iHead;
				byte[] bData = new byte[iDataLenth]; //以下用bData(去掉FE的)
				System.Array.Copy(bCmds, iHead, bData, 0, iEnd + 1 - iHead);
				int iControl = bData[8];
				if (iControl == 0xE1 || iControl == 0x81)//特殊处理
				{
					if (bData.Length < 12)
					{
						return false;
					}
					byte bCRC1 = DataConvert.fnBytes2CS(bData, 0, bData.Length - 2);//计算cs
					if (bData[bData.Length - 2] != bCRC1)
					{
						return false;
					}
					if (bData[bData.Length - 1] != 0x16)
					{
						return false;
					}
				}

				iDataLenth = bData[9];//iDataLenth 数据长度
				if (bData.Length < (iDataLenth + 12))
				{
					return false;
				}
				byte bCRC = DataConvert.fnBytes2CS(bData, 0, iDataLenth + 10);//计算cs  68....68 C DL
				if (bData[9 + iDataLenth + 1] != bCRC)
				{
					return false;
				}
				else if (bData[9 + iDataLenth + 2] != 0x16)
				{
					return false;
				}
				if (iEnd <= bCmds.Length - 1)
				{
					byte[] c = new byte[iHead + 9 + iDataLenth + 2 + 1];
					Array.Copy(bCmds, 0, c, 0, c.Length);
					list_Cmd.AddRange(c);
				}
			}
			catch { m_bisOK = false; }
			return m_bisOK;
		}

		public bool fnCheckData(List<byte> l, byte[] SendData, string type)
		{
			bool m_bisOK = true;
			try
			{
				if (type == "645")
				{
					list_Cmd = new List<byte>();

					byte[] bCmds = new byte[l.Count];

					l.CopyTo(bCmds, 0);
					string strData = DataConvert.fnGetStringFormat(DataConvert.Bytes2HexString(bCmds, 0, bCmds.Length));
					string strSendData =
						DataConvert.fnGetStringFormat(DataConvert.Bytes2HexString(SendData, 0, SendData.Length));
					if (strData.Contains(strSendData))
					{
						strData = strData.Replace(strSendData, "");
						if (strData.Length < 10)
						{
							return false;
						}
						bCmds = DataConvert.HEXString2Bytes(strData);
					}
					int iHead = 0;
					while (iHead < bCmds.Length)
					{
						if (bCmds[iHead] != (byte)0x68)
							iHead++;
						else
							break;
					}
					if (iHead > bCmds.Length)
					{
						return false;
					}
					else if (bCmds.Length - iHead < 12)
					{
						return false;
					}
					int iEnd = bCmds.Length - 1;
					while (iEnd > iHead)
					{
						if (bCmds[iEnd] != (byte)0x16)
							iEnd--;
						else
							break;
					}
					if (iEnd <= iHead)
					{
						return false;
					}
					else if ((bCmds[iHead] != (byte)0x68) || (bCmds[iHead + 7] != (byte)0x68))
					{
						return false;
					}
					int iDataLenth = bCmds.Length - iHead;
					byte[] bData = new byte[iDataLenth]; //以下用bData(去掉FE的)
					System.Array.Copy(bCmds, iHead, bData, 0, iEnd + 1 - iHead);
					int iControl = bData[8];
					if (iControl == 0xE1 || iControl == 0x81) //特殊处理
					{
						if (bData.Length < 12)
						{
							return false;
						}
						byte bCRC1 = DataConvert.fnBytes2CS(bData, 0, bData.Length - 2); //计算cs
						if (bData[bData.Length - 2] != bCRC1)
						{
							return false;
						}
						if (bData[bData.Length - 1] != 0x16)
						{
							return false;
						}
					}

					iDataLenth = bData[9]; //iDataLenth 数据长度
					if (bData.Length < (iDataLenth + 12))
					{
						return false;
					}
					//byte bCRC = DataConvert.fnBytes2CS(bData, 0, iDataLenth + 10); //计算cs  68....68 C DL
					//if (bData[9 + iDataLenth + 1] != bCRC)
					//{
					//    return false;
					//}
					else if (bData[9 + iDataLenth + 2] != 0x16)
					{
						return false;
					}
					if (iEnd <= bCmds.Length - 1)
					{
						byte[] c = new byte[iHead + 9 + iDataLenth + 2 + 1];
						Array.Copy(bCmds, 0, c, 0, c.Length);
						list_Cmd.AddRange(c);
					}
				}
				else if (type == "698Pass")
				{
					list_Cmd = new List<byte>();

					byte[] bCmds = new byte[l.Count];

					l.CopyTo(bCmds, 0);
					string strData = DataConvert.fnGetStringFormat(DataConvert.Bytes2HexString(bCmds, 0, bCmds.Length));
					string strSendData =
						DataConvert.fnGetStringFormat(DataConvert.Bytes2HexString(SendData, 0, SendData.Length));
					if (strData.Contains(strSendData))
					{
						strData = strData.Replace(strSendData, "");
						if (strData.Length < 10)
						{
							return false;
						}
						bCmds = DataConvert.HEXString2Bytes(strData);
					}
					int iHead = 0;
					while (iHead < bCmds.Length)
					{
						if (bCmds[iHead] != (byte)0x68)
							iHead++;
						else
							break;
					}
					if (iHead > bCmds.Length)
					{
						return false;
					}
					else if (bCmds.Length - iHead < 12)
					{
						return false;
					}
					int iEnd = bCmds.Length - 1;
					while (iEnd > iHead)
					{
						if (bCmds[iEnd] != (byte)0x16)
							iEnd--;
						else
							break;
					}
					if (iEnd <= iHead)
					{
						return false;
					}

					int iDataLenth = bCmds.Length - iHead;
					byte[] bData = new byte[iDataLenth]; //以下用bData(去掉FE的)
					System.Array.Copy(bCmds, iHead, bData, 0, iEnd + 1 - iHead);
					int iDataIndex = 22;

					iDataLenth = bData[iDataIndex]; //iDataLenth 数据长度
					if ((iDataLenth >= 0x81))
					{
						iDataIndex += 1;
						iDataLenth = bData[iDataIndex];
					}

					WriteLog.Write("698透传RX:" + Helps.ByteToStringWithSpace(bCmds));

					if (iEnd <= bCmds.Length - 1)
					{
						byte[] c = new byte[iDataLenth];
						Array.Copy(bCmds, (iDataIndex + 1), c, 0, c.Length);
						list_Cmd.AddRange(c);
					}
				}
				else if (type == "698")
				{
					list_Cmd = new List<byte>();

					byte[] bCmds = new byte[l.Count];

					l.CopyTo(bCmds, 0);
					string strData = DataConvert.fnGetStringFormat(DataConvert.Bytes2HexString(bCmds, 0, bCmds.Length));
					string strSendData =
						DataConvert.fnGetStringFormat(DataConvert.Bytes2HexString(SendData, 0, SendData.Length));
					if (strData.Contains(strSendData))
					{
						strData = strData.Replace(strSendData, "");
						if (strData.Length < 10)
						{
							return false;
						}
						bCmds = DataConvert.HEXString2Bytes(strData);
					}
					int iHead = 0;
					while (iHead < bCmds.Length)
					{
						if (bCmds[iHead] != (byte)0x68)
							iHead++;
						else
							break;
					}
					if (iHead > bCmds.Length)
					{
						return false;
					}
					else if (bCmds.Length - iHead < 12)
					{
						return false;
					}
					int iEnd = bCmds.Length - 1;
					while (iEnd > iHead)
					{
						if (bCmds[iEnd] != (byte)0x16)
							iEnd--;
						else
							break;
					}
					if (iEnd <= iHead)
					{
						return false;
					}

					int iDataLenth = bCmds.Length - iHead;
					byte[] bData = new byte[iDataLenth]; //以下用bData(去掉FE的)
					System.Array.Copy(bCmds, iHead, bData, 0, iEnd + 1 - iHead);
					int iDataIndex = 1;
					byte [] bTemp = new byte[2];
					System.Array.Copy(bData, 1, bTemp, 0, 2);
					int dataInt = 65536;
					int.TryParse(  DataConvert.ReverseBytesToHexStringWithoutSpace(bTemp, 0, bTemp.Length, true), NumberStyles.AllowHexSpecifier, null, out dataInt);

					if(dataInt > bCmds.Length - 2)
					{
						return false;
					}

					if (iEnd <= bCmds.Length - 1)
					{
						//byte[] c = new byte[iDataLenth];
						//Array.Copy(bCmds, (iDataIndex + 1), c, 0, c.Length);
						list_Cmd.AddRange(bCmds);
					}
				}
				else if (type == "Updata")
				{
					list_Cmd = new List<byte>();

					byte[] bCmds = new byte[l.Count];

					l.CopyTo(bCmds, 0);
					string strData = DataConvert.fnGetStringFormat(DataConvert.Bytes2HexString(bCmds, 0, bCmds.Length));
					string strSendData =
						DataConvert.fnGetStringFormat(DataConvert.Bytes2HexString(SendData, 0, SendData.Length));
					if (strData.Contains(strSendData))
					{
						strData = strData.Replace(strSendData, "");
						if (strData.Length < 8)
						{
							return false;
						}
						bCmds = DataConvert.HEXString2Bytes(strData);
					}
					int iHead = 0;
					while (iHead < bCmds.Length)
					{
						if (bCmds[iHead] != (byte)0x68)
							iHead++;
						else
							break;
					}
					if (iHead > bCmds.Length)
					{
						return false;
					}
					else if (bCmds.Length - iHead < 3)
					{
						return false;
					}
					int iEnd = bCmds.Length - 1;
					while (iEnd > iHead)
					{
						if (bCmds[iEnd] != (byte)0x16)
							iEnd--;
						else
							break;
					}
					if (iEnd <= iHead)
					{
						return false;
					}

					int iDataLenth = bCmds.Length - iHead;
					byte[] bData = new byte[iDataLenth]; //以下用bData(去掉FE的)
					System.Array.Copy(bCmds, iHead, bData, 0, iEnd + 1 - iHead);
					int iDataIndex = 22;

					//WriteLog.Write("RX:" + Helps.ByteToStringWithSpace(bCmds) );

					if (iEnd <= bCmds.Length - 1)
					{
						byte[] c = new byte[iDataLenth];
						Array.Copy(bCmds, iHead, c, 0, c.Length);
						list_Cmd.AddRange(c);
					}
				}
				else if (type == "MODBUS")
				{
					list_Cmd = new List<byte>();

					byte[] bCmds = new byte[l.Count];

					l.CopyTo(bCmds, 0);
					if (bCmds.Length < 6)
					{
						return false;
					}

					if (bCmds.Length <= (bCmds[2] + 4))
					{
						return false;
					}
					list_Cmd.AddRange(bCmds);
				}
				else if (type == "376.1")
				{
					list_Cmd = new List<byte>();
					byte[] bCmds = new byte[l.Count];
					l.CopyTo(bCmds, 0);
					int iHead = 0;
					while (iHead < bCmds.Length)
					{
						if (bCmds[iHead] != (byte)0x68)
							iHead++;
						else
							break;
					}
					if (iHead > bCmds.Length)
					{
						return false;
					}
					if (bCmds.Length - iHead < 30)
					{
						return false;
					}
					int iEnd = bCmds.Length - 1;
					while (iEnd > iHead)
					{
						if (bCmds[iEnd] != (byte)0x16)
							iEnd--;
						else
							break;
					}
					if (iEnd <= iHead)
					{
						return false;
					}
					if ((bCmds[iHead] != 0x68) || (bCmds[iHead + 5] != 0x68))
					{
						return false;
					}
					//int iDataLenth = bCmds.Length - iHead;                   
					//    byte[] c = new byte[iHead + 9 + iDataLenth + 2 + 1];
					//    Array.Copy(bCmds, 0, c, 0, c.Length);
					//    list_Cmd.AddRange(c);                  
					list_Cmd = l;
				}
				else if (type == "ASCII")
				{
					list_Cmd = new List<byte>();
					int i = 0;
					for (; i < l.Count; i++)
					{
						if (l[i] != 0xa)
						{
							list_Cmd.Add(l[i]);
						}
						else
						{
							break;
						}
					}

					if (i == l.Count)
					{
						return false;
					}
				}
				else if (type == "EncryptSCU")
				{
					list_Cmd = new List<byte>();

					byte[] bCmds = new byte[l.Count];

					l.CopyTo(bCmds, 0);
					int iDataLenth = 0;

					if (bCmds[1] == 0)
					{
						iDataLenth = bCmds[2]; //iDataLenth 数据长度
					}
					else
					{
						byte[] dataBytes = new byte[2];
						Array.Copy(bCmds, 1, dataBytes, 0, dataBytes.Length);
						iDataLenth = Convert.ToInt32( Helps.ByteToStringWithNoSpace(dataBytes), 16) ;
					}
					
                    if (bCmds.Length < (iDataLenth + 5) )
                    {
                        return false;
                    }
                    list_Cmd.AddRange(bCmds);
				}
				else
				{
					m_bisOK = false;
				}
			}
			catch { m_bisOK = false; }
			return m_bisOK;
		}
		#endregion

		#endregion

		#region 发送命令
		/// <summary>
		/// 发送命令
		/// </summary>
		/// <param name="SendData">发送数据</param>
		///   <param name="SendData">发送数据长度</param>
		/// <param name="Overtime">超时时间</param>
		/// <returns></returns>
		public byte[] SendCommand(byte[] SendData, int Sendlen, int Overtime)
		{
			if (_serialPort.IsOpen)
			{
				try
				{
					ReceiveEventFlag = true;        //关闭接收事件 
					_serialPort.DiscardInBuffer();  //清空接收缓冲区                
					_serialPort.Write(SendData, 0, Sendlen);
					int ret = 0;
					List<byte> list = new List<byte>();
					System.Threading.Thread.Sleep(50);
					ReceiveEventFlag = false;      //打开事件
					int i = 0;
					while (true)
					{
						byte[] _data = new byte[_serialPort.BytesToRead];
						_serialPort.Read(_data, 0, _data.Length);
						list.AddRange(_data);
						if(_data != null  )
						return _data;
						//if (fnCheckData(list, SendData))
						//{
						//	_data = new byte[list_Cmd.Count];
						//	foreach (byte bt in list_Cmd)
						//	{
						//		_data[i] = bt;
						//		i++;
						//	}
						//	return _data;
						//}
						ret++;
						if (ret > Overtime / 100)
						{
							break;
						}
						System.Threading.Thread.Sleep(100);
					}

					//while (num++ < Overtime/100)
					//{

					//    byte[] _data = new byte[_serialPort.BytesToRead];
					//    _serialPort.Read(_data, 0, _data.Length);
					//    if (_data.Length > 8)
					//        return _data;
					//    System.Threading.Thread.Sleep(100);
					//}
				}
				catch (Exception ex)
				{
					ReceiveEventFlag = false;//true表示接受事件无效
											 //throw ex;
				}
			}
			else
			{
				MessageBox.Show("请先打开串口!");
			}
			return null;
		}

		/// <summary>
		/// 发送命令
		/// </summary>
		/// <param name="SendData">发送数据</param>
		///   <param name="Type">类型</param>
		/// <returns></returns>
		public byte[] SendCommand(byte[] SendData, string Type)
		{
			if (_serialPort.IsOpen)
			{
				try
				{
					ReceiveEventFlag = true;        //关闭接收事件 
					_serialPort.DiscardInBuffer();  //清空接收缓冲区
					_serialPort.Write(SendData, 0, SendData.Length);
					int ret = 0;
					List<byte> list = new List<byte>();
					System.Threading.Thread.Sleep(15);
					ReceiveEventFlag = false;      //打开事件
					int i = 0;
					while (true)
					{
						byte[] _data = new byte[_serialPort.BytesToRead];
						_serialPort.Read(_data, 0, _data.Length);
						list.AddRange(_data);
						if (fnCheckData(list, SendData, Type))
						{
							_data = new byte[list.Count];
							foreach (byte bt in list)
							{
								_data[i] = bt;
								i++;
							}
							return _data;
						}
						ret++;
						if (ret > OverTime / 17)
						{
							break;
						}
						Thread.Sleep(15);
					}

					//while (num++ < Overtime/100)
					//{

					//    byte[] _data = new byte[_serialPort.BytesToRead];
					//    _serialPort.Read(_data, 0, _data.Length);
					//    if (_data.Length > 8)
					//        return _data;
					//    System.Threading.Thread.Sleep(100);
					//}
				}
				catch (Exception ex)
				{
					ReceiveEventFlag = false;//true表示接受事件无效
											 //throw ex;
				}
			}
			else
			{
				//MessageBox.Show("请先打开串口!");
			}
			return null;
		}
		
		/// <summary>
		/// 发送命令
		/// </summary>
		/// <param name="SendData">发送数据</param>
		///   <param name="Type">类型</param>
		/// <returns></returns>
		public byte[] SendCommand(string strCAN_AT)
		{
			if (_serialPort.IsOpen)
			{
				try
				{
					ReceiveEventFlag = true;        //关闭接收事件 
					_serialPort.DiscardInBuffer();  //清空接收缓冲区
					//_serialPort.Write(strCAN_AT);
					Encoding encoding = Encoding.ASCII;
					byte[] bytes = encoding.GetBytes(strCAN_AT);
					_serialPort.Write(bytes, 0, bytes.Length);
					int ret = 0;
					List<byte> list = new List<byte>();
					//System.Threading.Thread.Sleep(15);
					ReceiveEventFlag = false;      //打开事件
					int i = 0;
					string strRec = "";
					while (true)
					{
						byte[] _data = new byte[_serialPort.BytesToRead];
						_serialPort.Read(_data, 0, _data.Length);
						list.AddRange(_data);
						//if (fnCheckData(list, SendData, Type))
						//{
						//	_data = new byte[list_Cmd.Count];
						//	foreach (byte bt in list_Cmd)
						//	{
						//		_data[i] = bt;
						//		i++;
						//	}
						//	return _data;
						//}
						strRec += DataConvert.Byte2String(_data, 0, _data.Length);
						if (strRec.IndexOf("OK") > -1)
						{
							_data = new byte[list.Count];
							foreach (byte bt in list)
							{
								_data[i] = bt;
								i++;
							}
							return _data;
						}
						ret++;
						if (ret > OverTime / 6)
						{
							break;
						}
						Thread.Sleep(5);
					}

					//while (num++ < Overtime/100)
					//{

					//    byte[] _data = new byte[_serialPort.BytesToRead];
					//    _serialPort.Read(_data, 0, _data.Length);
					//    if (_data.Length > 8)
					//        return _data;
					//    System.Threading.Thread.Sleep(100);
					//}
				}
				catch (Exception ex)
				{
					ReceiveEventFlag = false;//true表示接受事件无效
											 //throw ex;
				}
			}
			else
			{
				//MessageBox.Show("请先打开串口!");
			}
			return null;
		}

		public int sendUpdata(byte[] SendBuf)
		{
			if (!_serialPort.IsOpen) return 1;
			try
			{
				byte[] ReceBuf = new byte[100];
				int TimerCounter = 0;
				ReceiveEventFlag = true;        //关闭接收事件 
				_serialPort.DiscardInBuffer();  //清空接收缓冲区                
				_serialPort.Write(SendBuf, 0, 1024);
				int t = _serialPort.BytesToRead;//得到缓冲区内的数据数量
				while (t == 0)
				{
					System.Threading.Thread.Sleep(10);
					t = _serialPort.BytesToRead;
					TimerCounter++;
					if (TimerCounter >= 300)//超过未接收到数据，认为电表处理错误
					{
						break;
						//return 2;
					}
				}
				TimerCounter = 0;
				_serialPort.Read(ReceBuf, 0, t);//读取缓冲区内数据，放入缓存数组中指定偏移

				//if (ReceBuf[0] != 0xdf || ReceBuf[0] != 0xff)
				//{
				//	return 3;
				//}

				return 0;
			}
			catch (Exception ex)
			{
				ReceiveEventFlag = false;//true表示接受事件无效
				return 4; //throw ex;
			}
		}

		public bool sendWithNoAsk(byte[] SendBuf, int count)
		{
			if (!_serialPort.IsOpen) return false;
			_serialPort.Write(SendBuf, 0, count);
			return true;
		}

		/// <summary>
		/// 监视串口
		/// </summary>
		/// <param name="SendData">发送数据</param>
		///   <param name="Type">类型</param>
		/// <returns></returns>
		public byte[] MoniterSerial()
		{
			byte[] _data = null;
			if (_serialPort.IsOpen)
			{
				try
				{
					ReceiveEventFlag = false;      //打开事件
					if (_serialPort.BytesToRead != 0)
					{
						System.Threading.Thread.Sleep(20);
						while (true)
						{
							if (_serialPort.BytesToRead != 0)
							{
								_data = new byte[_serialPort.BytesToRead];
								System.Threading.Thread.Sleep(20);
								_serialPort.Read(_data, 0, _data.Length);
								if (_data.Length != 0)
								{
									return _data;
								}
							}
						}
					}
				}
				catch (Exception ex)
				{
					ReceiveEventFlag = false;//true表示接受事件无效
				}
			}
			else
			{
				//MessageBox.Show("请先打开串口!");
			}
			return _data;
		}
		
		/// <summary>
		/// Clear
		/// </summary>
		/// <param name="SendData">发送数据</param>
		///   <param name="Type">类型</param>
		/// <returns></returns>
		public void DiscardInBuffer()
		{
			if (_serialPort.IsOpen)
			{
				try
				{
					_serialPort.DiscardInBuffer();  //清空接收缓冲区
				}
				catch (Exception ex)
				{
				}
			}
			else
			{
				//MessageBox.Show("请先打开串口!");
			}
		}

		public byte[] SendAndRecvCmds(byte[] SendData, int Sendlen, int Overtime)
		{
			if (_serialPort.IsOpen)
			{
				try
				{
					ReceiveEventFlag = true;        //关闭接收事件 
					_serialPort.DiscardInBuffer();  //清空接收缓冲区                
					_serialPort.Write(SendData, 0, Sendlen);
					int num = 0, ret = 0;
					List<byte> list = new List<byte>();
					ReceiveEventFlag = false;      //打开事件
					while (true)
					{
						byte[] _data = new byte[_serialPort.BytesToRead];
						_serialPort.Read(_data, 0, _data.Length);
						if (_data.Length > 11)
						{
							//if (_data[_data.Length - 1] == 0x10)
							{
								return _data;
							}
						}

						ret++;
						if (ret > Overtime / 100)
						{
							break;
						}
						System.Threading.Thread.Sleep(100);
					}
				}
				catch (Exception ex)
				{
					ReceiveEventFlag = false;//true表示接受事件无效
											 //throw ex;
				}
			}
			else
			{
				//MessageBox.Show("请先打开串口!");
			}
			return null;
		}

		public byte[] SendCommands(byte[] SendData, int Sendlen, int Overtimes)
		{
			if (_serialPort.IsOpen)
			{
				try
				{
					ReceiveEventFlag = true;        //关闭接收事件 
					_serialPort.DiscardInBuffer();  //清空接收缓冲区                
					_serialPort.Write(SendData, 0, Sendlen);
					System.Threading.Thread.Sleep(400);
					ReceiveEventFlag = false;      //打开事件

					int ret = 0;
					while (true)
					{
						System.Threading.Thread.Sleep(Overtimes);
						byte[] _data = new byte[_serialPort.BytesToRead];
						System.Threading.Thread.Sleep(10);
						_serialPort.Read(_data, 0, _data.Length);
						if (_data.Length > 11)
						{
							return _data;
						}

						ret++;
						if (ret > 5)
						{
							break;
						}
						System.Threading.Thread.Sleep(10);
					}
				}
				catch (Exception ex)
				{
					ReceiveEventFlag = false;//true表示接受事件无效
											 //throw ex;
				}
			}
			else
			{
				MessageBox.Show("请先打开串口!");
			}
			return null;
		}
		#endregion

		#region 获取串口
		/// <summary>
		/// 获取所有已连接设备的串口
		/// </summary>
		/// <returns></returns>
		public string[] serialsIsConnected()
		{
			List<string> lists = new List<string>();
			string[] seriallist = getSerials();
			foreach (string s in seriallist)
			{
			}
			return lists.ToArray();
		}
		#endregion

		#region 获取当前全部串口资源
		/// <summary>
		/// 获得当前电脑上的所有串口资源
		/// </summary>
		/// <returns></returns>
		public string[] getSerials()
		{
			return SerialPort.GetPortNames();
		}
		#endregion

		#region 字节型转换16
		/// <summary>
		/// 把字节型转换成十六进制字符串
		/// </summary>
		/// <param name="InBytes"></param>
		/// <returns></returns>
		public static string ByteToString(byte[] InBytes)
		{
			if (InBytes == null)
				return "";
			string StringOut = "";
			foreach (byte InByte in InBytes)
			{
				StringOut = StringOut + String.Format("{0:X2} ", InByte);
			}
			return StringOut;
		}
		#endregion

		#region 十六进制字符串转字节型
		/// <summary>
		/// 把十六进制字符串转换成字节型(方法1)
		/// </summary>
		/// <param name="InString"></param>
		/// <returns></returns>
		public static byte[] StringToByte(string InString)
		{
			string[] ByteStrings;
			ByteStrings = InString.Split(" ".ToCharArray());
			byte[] ByteOut;
			ByteOut = new byte[ByteStrings.Length];
			for (int i = 0; i <= ByteStrings.Length - 1; i++)
			{
				//ByteOut[i] = System.Text.Encoding.ASCII.GetBytes(ByteStrings[i]);
				ByteOut[i] = Byte.Parse(ByteStrings[i], System.Globalization.NumberStyles.HexNumber);
				//ByteOut[i] =Convert.ToByte("0x" + ByteStrings[i]);
			}
			return ByteOut;
		}
		#endregion

		#region 十六进制字符串转字节型
		/// <summary>
		/// 字符串转16进制字节数组(方法2)
		/// </summary>
		/// <param name="hexString"></param>
		/// <returns></returns>
		public static byte[] strToToHexByte(string hexString)
		{
			hexString = hexString.Replace(" ", "");
			if ((hexString.Length % 2) != 0)
				//hexString += " ";
				hexString = "0" + hexString;
			byte[] returnBytes = new byte[hexString.Length / 2];
			for (int i = 0; i < returnBytes.Length; i++)
				returnBytes[i] = Convert.ToByte(hexString.Substring(i * 2, 2), 16);
			return returnBytes;
		}
		#endregion

		#region 字节型转十六进制字符串
		/// <summary>
		/// 字节数组转16进制字符串
		/// </summary>
		/// <param name="bytes"></param>
		/// <returns></returns>
		public static string byteToHexStr(byte[] bytes)
		{
			string returnStr = "";
			if (bytes != null)
			{
				for (int i = 0; i < bytes.Length; i++)
				{
					returnStr += bytes[i].ToString("X2");
				}
			}
			return returnStr;
		}
		#endregion
	}
}
