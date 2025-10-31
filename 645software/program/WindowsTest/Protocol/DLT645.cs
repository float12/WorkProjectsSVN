using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;

namespace WindowsTest
{
	public class DLT645
	{
		public DLT645()
		{
			throw new Exception("多回路表通信地址复杂，建议每次使用本类组包时都重新赋值确认通信地址，不用使用默认构造函数！");
		}

		/// <summary>
		/// 进来的是倒序的地址
		/// </summary>
		/// <param name="addrStr"></param>
		public DLT645(string addrStr)
		{
			AddrStr = Helps.fnAgainst(addrStr);
		}

		/// <summary>
		/// 是否包含前导字符
		/// </summary>
		public static bool IsHaveLeaderChar = true;

		/// <summary>
		/// 前导字符
		/// </summary>
		public string LeaderChar = "FEFEFEFE";
		public static string StartChar1 = "68";
		private static string _AddrStr = "AAAAAAAAAAAA";
		SCUProxy m_SCUProxy = new SCUProxy();
		/// <summary>
		/// 通信地址
		/// </summary>
		private static string AddrStr
		{
			get
			{
				return _AddrStr.PadLeft(12, '0');
			}
			set
			{
				if (string.IsNullOrEmpty(value))
				{
					_AddrStr = "AAAAAAAAAAAA";
				}
				else
				{
					if (value.Length > 12)
					{
						_AddrStr = value.Substring(0, 12);
					}
					else
					{
						_AddrStr = value.PadLeft(12, '0');
					}
				}
				addrStrReverse = Helps.fnAgainst(_AddrStr);
			}
		}
		/// <summary>
		/// 通信地址的倒序值
		/// </summary>
		private static string addrStrReverse = "AAAAAAAAAAAA";

		public static string StartChar2 = "68";
		public enum ControlType
		{
			/// <summary>
			/// 主站请求帧-证书交换、身份认证、密钥协商
			/// </summary>
			[Description("00")]
			SafetyCertificationLTU,
			
			/// <summary>
			/// 主站请求帧-请求读电能表数据
			/// </summary>
			[Description("01")]
			SafetyReadLTU,
			
			/// <summary>
			/// 主站请求帧-请求读电能表数据
			/// </summary>
			[Description("01")]
			SafetyReadLoadLTU,

			/// <summary>
			/// 主站请求帧-请求读后续数据
			/// </summary>
			[Description("02")]
			SafetyReadFollowLTU,
			
			/// <summary>
			/// 主站请求帧-请求读电能表通信地址
			/// </summary>
			[Description("03")]
			SafetyReadAddrLTU,
			
			/// <summary>
			/// 主站请求帧-主站向从站请求设置数据(或编程)
			/// </summary>
			[Description("04")]
			SafetyWriteLTU,
			
			/// <summary>
			/// 主站请求帧-设置某从站的通信地址
			/// </summary>
			[Description("05")]
			SafetyWriteAddrLTU,
			
			/// <summary>
			/// 主站请求帧-强制从站与主站时间同步
			/// </summary>
			[Description("0E")]
			SafetyMsAdjustTimeLTU,

			/// <summary>
			/// 主站请求帧-电能表安全认证相关操作
			/// </summary>
			[Description("03")]
			SafetyCertification,

			/// <summary>
			/// 主站确认
			/// </summary>
			[Description("06")]
			MasterConfirmation,

			/// <summary>
			/// 主站请求帧-透传报文到目的地址
			/// </summary>
			[Description("07")]
			TransparentTransmission,

			/// <summary>
			/// 广播校时
			/// </summary>
			[Description("08")]
			Radio,

			/// <summary>
			/// 读
			/// </summary>
			[Description("11")]
			Read,

			/// <summary>
			/// 读给定块数的负荷记录
			/// </summary>
			[Description("11")]
			Readm1,

			/// <summary>
			/// 液晶查看命令
			/// </summary>
			[Description("11")]
			LcdView,

			/// <summary>
			/// 读给定时间、块数的负荷记录
			/// </summary>
			[Description("11")]
			ReadLoad,

			[Description("11")]
			Read_zuhe,

			/// <summary>
			/// 厂内扩展读命令，两个数据标识
			/// </summary>
			[Description("11")]
			ReadFactory,

			/// <summary>
			/// 厂内扩展读命令，两个数据标识
			/// </summary>
			[Description("11")]
			ReadTopo,

			/// <summary>
			/// 读后续帧
			/// </summary>
			[Description("12")]
			ReadFollow,

			/// <summary>
			/// 设置
			/// </summary>
			[Description("14")]
			Write,

			/// <summary>
			/// 设置Topo
			/// </summary>
			[Description("14")]
			WriteTopo,

			/// <summary>
			/// 开关控制
			/// </summary>
			[Description("1C")]
			WriteSwitch,

			/// <summary>
			/// 清零
			/// </summary>
			[Description("19")]
			DemandClear,

			/// <summary>
			/// 清零
			/// </summary>
			[Description("1A")]
			Clear,

			/// <summary>
			/// 事件清零
			/// </summary>
			[Description("1B")]
			EventClear,

			/// <summary>
			/// 控制
			/// </summary>
			[Description("1C")]
			Control,

			/// <summary>
			/// 扩展山东升级
			/// </summary>
			[Description("1D")]
			Updata,

			/// <summary>
			/// 设置多功能端子输出信号类别
			/// </summary>
			[Description("1D")]
			MultifunctionTerminal,
			
			/// <summary>
			/// ms对时
			/// </summary>
			[Description("1E")]
			msAdjustTime,

			/// <summary>
			/// 从站上报
			/// </summary>
			[Description("86")]
			SlaveReport,
			
			/// <summary>
			/// 从站搜表地址回复
			/// </summary>
			[Description("91")]
			SlaveReplyAddr,
			/// <summary>
			/// 从站搜表地址回复
			/// </summary>
			[Description("9E")]
			SlaveReplyTime,
		}

		#region 密码相关

		public static bool IsUsePublicPassWord = true;

		public static byte PassWordLevel = 2;

		public static string PassWord = "000000";
		#endregion

		private static string _OperatorStr = "00000000";
		/// <summary>
		/// 操作者代码
		/// </summary>
		public static string OperatorStr
		{
			get
			{
				return _OperatorStr;
			}
			set
			{
				if (string.IsNullOrEmpty(value))
				{
					_OperatorStr = "00000000";
				}
				else
				{
					if (value.Length > 8)
					{
						_OperatorStr = value.Substring(0, 8);
					}
					else
					{
						_OperatorStr = value.PadLeft(8, '0');
					}
				}
			}
		}

		public byte EndByte = 0x16;

		/// <summary>
		/// 组包
		/// </summary>
		/// <param name="controlType"></param>
		/// <param name="dataMarks">数据标识集合，需要保证数据标识的格式都是合法的</param>
		/// <param name="data"></param>
		/// <param name="seq">后续帧帧序号</param>
		/// <param name="sendMessageBytes"></param>
		/// <param name="strPasswordLevel"></param>
		/// <param name="errStr"></param>
		/// <returns></returns>
		public bool Pack(ControlType controlType, List<string> dataMarks, string data, byte seq, out byte[] sendMessageBytes, out string errStr, string strPasswordLevel = "02", string strPassWord = "000000", string strMins = "88888888") 
		{
			errStr = "";
			sendMessageBytes = null;
			try
			{
				string sendMessageStr = StartChar1 + addrStrReverse + StartChar2 + controlType.GetDescription().PadLeft(2, '0');
				string dataField = "";
				switch (controlType)
				{
					case ControlType.Read:
						switch (dataMarks.Count)
						{
							case 1:
								dataField += Helps.fnAgainst(dataMarks[0]);
								break;
							case 2:
								dataField += Helps.fnAgainst(dataMarks[1]);
								dataField += Helps.fnAgainst(dataMarks[0]);
								dataField += "";
								break;
							default:
								throw new Exception("抄读时数据标识有误");
						}
						break;
					case ControlType.SafetyReadLTU:
						switch (dataMarks.Count)
						{
							case 1:
								dataField += Helps.fnAgainst(dataMarks[0]);
								//TODO:调用动态库 返回MAC
								byte[] baDataBytes = null;
								Helps.TryStrToHexByte(dataField, out baDataBytes, out string strErr);
								byte [] baSendBytes = m_SCUProxy.HY_SendToProxyBuf(0x01, 0x14, 1, baDataBytes);
								//output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
								byte[] baRecBytes =  winMain.SpWuCha.SendCommand(baSendBytes, "EncryptSCU");
								//output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);
								baDataBytes = new byte[baRecBytes.Length - 13] ;
								Array.Copy(baRecBytes, 12, baDataBytes, 0, baDataBytes.Length);
								dataField += Helps.ByteToStringWithNoSpace(baDataBytes);
								break;
							//case 2:
							//	dataField += Helps.fnAgainst(dataMarks[1]);
							//	dataField += Helps.fnAgainst(dataMarks[0]);
							//	dataField += "";
							//	break;
							default:
								throw new Exception("抄读时数据标识有误");
						}
						break;
					case ControlType.SafetyReadFollowLTU:
						switch (dataMarks.Count)
						{
							case 1:
								dataField += Helps.fnAgainst(dataMarks[0]) + seq.ToString("X2");
								//TODO:调用动态库 返回MAC
								byte[] baDataBytes = null;
								Helps.TryStrToHexByte(dataField, out baDataBytes, out string strErr);
								byte [] baSendBytes = m_SCUProxy.HY_SendToProxyBuf(0x01, 0x14, 1, baDataBytes);
								//output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
								byte[] baRecBytes =  winMain.SpWuCha.SendCommand(baSendBytes, "EncryptSCU");
								//output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);
								baDataBytes = new byte[baRecBytes.Length - 9] ;
								Array.Copy(baRecBytes, 8, baDataBytes, 0, baDataBytes.Length);
								dataField = Helps.ByteToStringWithNoSpace(baDataBytes);
								break;
							//case 2:
							//	dataField += Helps.fnAgainst(dataMarks[1]);
							//	dataField += Helps.fnAgainst(dataMarks[0]);
							//	dataField += "";
							//	break;
							default:
								throw new Exception("抄读时数据标识有误");
						}
						break;
					case ControlType.SafetyReadLoadLTU:
						switch (dataMarks.Count)
						{
							case 1:
								dataField += Helps.fnAgainst(dataMarks[0]);
								dataField += data;
								//TODO:调用动态库 返回MAC
								byte[] baDataBytes = null;
								Helps.TryStrToHexByte(dataField, out baDataBytes, out string strErr);
								byte [] baSendBytes = m_SCUProxy.HY_SendToProxyBuf(0x01, 0x14, 1, baDataBytes);
								//output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
								byte[] baRecBytes =  winMain.SpWuCha.SendCommand(baSendBytes, "EncryptSCU");
								//output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);
								baDataBytes = new byte[baRecBytes.Length - 9] ;
								Array.Copy(baRecBytes, 8, baDataBytes, 0, baDataBytes.Length);
								dataField = Helps.ByteToStringWithNoSpace(baDataBytes);
								break;
							//case 2:
							//	dataField += Helps.fnAgainst(dataMarks[1]);
							//	dataField += Helps.fnAgainst(dataMarks[0]);
							//	dataField += "";
							//	break;
							default:
								throw new Exception("抄读时数据标识有误");
						}
						break;
					case ControlType.SafetyWriteLTU:
						switch (dataMarks.Count)
						{
							case 1:
								dataField += Helps.fnAgainst(dataMarks[0]) + strPasswordLevel + Helps.fnAgainst(strPassWord) + Helps.fnAgainst(OperatorStr) + Helps.fnAgainst(data);
								//TODO:调用动态库 返回MAC
								byte[] baDataBytes = null;
								Helps.TryStrToHexByte(dataField, out baDataBytes, out string strErr);
								byte [] baSendBytes = m_SCUProxy.HY_SendToProxyBuf(0x01, 0x14, 1, baDataBytes);
								//output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
								byte[] baRecBytes =  winMain.SpWuCha.SendCommand(baSendBytes, "EncryptSCU");
								//output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);
								baDataBytes = new byte[baRecBytes.Length - 9] ;
								Array.Copy(baRecBytes, 8, baDataBytes, 0, baDataBytes.Length);
								string strDataEn = Helps.ByteToStringWithNoSpace(baDataBytes);
								strDataEn = strDataEn.Substring(strDataEn.Length - 8, 8);
								dataField = dataField.Substring(0, dataField.Length - 8) + strDataEn;
								break;
							//case 2:
							//	dataField += Helps.fnAgainst(dataMarks[1]);
							//	dataField += Helps.fnAgainst(dataMarks[0]);
							//	dataField += "";
							//	break;
							default:
								throw new Exception("抄读时数据标识有误");
						}
						break;
					case ControlType.SafetyCertificationLTU:
						dataField += data;
						break;
					case ControlType.ReadFactory:
					case ControlType.ReadLoad:
						switch (dataMarks.Count)
						{
							case 1:
								dataField += Helps.fnAgainst(dataMarks[0]);
								dataField += data;
								break;
							case 2:
								dataField += Helps.fnAgainst(dataMarks[1]);
								dataField += Helps.fnAgainst(dataMarks[0]);
								dataField += data;
								break;
							default:
								throw new Exception("抄读时数据标识有误");
						}
						break;
					#region 南网2020扩展功能
					case ControlType.Read_zuhe:
						if (dataMarks.Count > 20)
						{
							throw new Exception("组合抄读最多支持20个数据标识");
						}

						dataField += dataMarks.Count.ToString("D2") + "EEEEEE";
						for (int i = 0; i < dataMarks.Count; i++)
						{
							dataField += Helps.fnAgainst(dataMarks[i]);
							if (i != dataMarks.Count - 1)
							{
								dataField += "BB";
							}
						}
						break;
					#endregion
					case ControlType.ReadFollow:
						if (dataMarks.Count <= 0)
						{
							throw new Exception("抄读时数据标识为空");
						}

						if (dataMarks.Count == 1)
						{
							dataField += Helps.fnAgainst(dataMarks[0]) + seq.ToString("X2");
						}
						else//TODO:读后续帧要完成一下
						{
							dataField += dataMarks.Count.ToString("D2") + "EEEEEE" + seq.ToString("X2");
						}
						break;
					//              case ControlType.ReadTopo:
					//dataField = Helps.fnAgainst(dataMarks[0]) + data;
					//break;
					case ControlType.Write:
					case ControlType.WriteSwitch:
					case ControlType.Updata:
						if (data.Length == 0)
						{

						}
						else if (string.IsNullOrEmpty(data) || data.Length % 2 != 0)
						{
							throw new Exception("设置值非法");
						}

						switch (dataMarks.Count)
						{
							case 1:
								if (dataMarks[0].Length != 8)
								{
									throw new Exception("设置的数据标识非法");
								}
								//dataField = Helps.fnAgainst(dataMarks[0]) + PassWordLevel.ToString("X2") + Helps.fnAgainst(PassWord) + Helps.fnAgainst(OperatorStr) + Helps.fnAgainst(data);

								dataField = Helps.fnAgainst(dataMarks[0]) + strPasswordLevel + Helps.fnAgainst(strPassWord) + Helps.fnAgainst(OperatorStr) + Helps.fnAgainst(data);
								break;
							case 2:
								if (dataMarks[0].Length != 8 || dataMarks[1].Length != 8)
								{
									throw new Exception("设置的数据标识非法");
								}
								//dataField = Helps.fnAgainst(dataMarks[1]) + PassWordLevel.ToString("X2") + Helps.fnAgainst(PassWord) + Helps.fnAgainst(OperatorStr) + Helps.fnAgainst(dataMarks[0])+ "88888888" + Helps.fnAgainst(data);
								dataField = Helps.fnAgainst(dataMarks[1]) + strPasswordLevel + Helps.fnAgainst(strPassWord) + Helps.fnAgainst(OperatorStr) + Helps.fnAgainst(dataMarks[0]) + strMins + Helps.fnAgainst(data);
								break;
							default:
								throw new Exception("设置的数据标识非法");
								break;
						}
						break;
					case ControlType.WriteTopo:
					case ControlType.ReadTopo:
						dataField = Helps.fnAgainst(dataMarks[0]) + data;
						break;
					case ControlType.Radio:
					case ControlType.msAdjustTime:
					case ControlType.SafetyMsAdjustTimeLTU:
						if (string.IsNullOrEmpty(data) || data.Length % 2 != 0)
						{
							throw new Exception("设置值非法");
						}

						dataField = Helps.fnAgainst(data);
						break;
					case ControlType.Control:
					case ControlType.DemandClear:
					case ControlType.Clear:
					case ControlType.EventClear:
						if (string.IsNullOrEmpty(data) || data.Length % 2 != 0)
						{
							throw new Exception("设置值非法");
						}
						dataField = (strPasswordLevel + Helps.fnAgainst(strPassWord) + Helps.fnAgainst(OperatorStr) + (data));
						break;
					case ControlType.MultifunctionTerminal:
						if (string.IsNullOrEmpty(data) || data.Length % 2 != 0)
						{
							throw new Exception("设置值非法");
						}
						dataField = (data);
						break;
				}

				#region 计算数据域长度+33

				string errStr1;
				byte[] dataFieldBytes;
				if (!Helps.TryStrToHexByte(dataField, out dataFieldBytes, out errStr1))
				{
					throw new Exception(errStr1);
				}

				for (int i = 0; i < dataFieldBytes.Length; i++)
				{
					dataFieldBytes[i] += 0x33;
				}

				if (!Helps.TryStrToHexByte(sendMessageStr, out sendMessageBytes, out errStr1))
				{
					throw new Exception(errStr1);
				}

				sendMessageBytes = sendMessageBytes.Concat(new byte[] { (byte)dataFieldBytes.Length }).ToArray().Concat(dataFieldBytes).ToArray();
				#endregion

				#region 校验和+16
				byte sum = Helps.Ssum(sendMessageBytes);
				byte[] endBytes = new[] { sum, EndByte };
				#endregion

				sendMessageBytes = sendMessageBytes.Concat(endBytes).ToArray();
				return true;
			}
			catch (Exception e)
			{
				errStr += e.Message;
				return false;
			}
		}

		/// <summary>
		/// 7326 长报文升级
		/// </summary>
		/// <param name="controlType"></param>
		/// <param name="dataMarks">数据标识集合，需要保证数据标识的格式都是合法的</param>
		/// <param name="data"></param>
		/// <param name="sendMessageBytes"></param>
		/// <param name="errStr"></param>
		/// <returns></returns>
		public bool PackUpdata(string dataMarks, string data, out byte[] sendMessageBytes, out string errStr)
		{
			errStr = "";
			sendMessageBytes = null;
			try
			{
				//string sendMessageStr = StartChar1 + dataMarks + Helps.fnAgainst((data.Length / 2).ToString("x4") ) + Helps.fnAgainst(data);
				string sendMessageStr = StartChar1 + "02" + Helps.fnAgainst((data.Length / 2).ToString("x4")) + Helps.fnAgainst(data);
				if (!Helps.TryStrToHexByte(sendMessageStr, out sendMessageBytes, out errStr))
				{
					throw new Exception(errStr);
				}

				#region 校验和+16
				byte sum = Helps.Ssum(sendMessageBytes);
				byte[] endBytes = new[] { sum, EndByte };
				sendMessageBytes = sendMessageBytes.Concat(endBytes).ToArray();
				#endregion
				return true;
			}
			catch (Exception e)
			{
				errStr += e.Message;
				return false;
			}
		}
		
		public bool PackMoniter(ControlType controlType, string dataMarks, string data, out byte[] sendMessageBytes, out string errStr)
		{
			errStr = "";
			sendMessageBytes = null;
			try
			{
				string sendMessageStr =StartChar1 + addrStrReverse + StartChar2 + controlType.GetDescription().PadLeft(2, '0') + (data.Length / 2 + dataMarks.Length / 2).ToString("X2") + DataConvert.fnEncodingData((dataMarks) )+ DataConvert.fnEncodingData(Helps.fnAgainst(data) );
				if (!Helps.TryStrToHexByte(sendMessageStr, out sendMessageBytes, out errStr))
				{
					throw new Exception(errStr);
				}

				#region 校验和+16
				byte sum = Helps.Ssum(sendMessageBytes);
				byte[] endBytes = new[] { sum, EndByte };
				sendMessageBytes = sendMessageBytes.Concat(endBytes).ToArray();
				#endregion
				return true;
			}
			catch (Exception e)
			{
				errStr += e.Message;
				return false;
			}
		}

		/// <summary>
		/// 判断执行是否成功，吐出数据域-33
		/// </summary>
		/// <param name="recBytes">收到的报文</param>
		/// <param name="controlType">发送报文的控制码</param>
		/// <param name="dataMarks"></param>
		/// <param name="dataField"></param>
		/// <param name="errStr"></param>
		/// <returns></returns>
		public bool UnPack(byte[] recBytes, ControlType controlType, List<string> dataMarks, byte seq, out bool isHaveHouxu, out string dataField, out string errStr)
		{
			errStr = "";
			dataField = "";
			isHaveHouxu = false;
			if (recBytes == null)
			{
				errStr += "收到的返回报文为空！";
				return false;
			}

			#region 判断长度
			int iHead = 0;
			while (iHead < recBytes.Length)
			{
				if (recBytes[iHead] != (byte)0x68)
					iHead++;
				else
					break;
			}

			if (iHead > recBytes.Length)
			{
				return false;
			}
			else if (recBytes.Length - iHead < 12)
			{
				return false;
			}

			int iEnd = recBytes.Length - 1;
			while (iEnd > iHead)
			{
				if (recBytes[iEnd] != (byte)0x16)
					iEnd--;
				else
					break;
			}

			if (iEnd <= iHead)
			{
				return false;
			}
			else if ((recBytes[iHead] != (byte)0x68) || (recBytes[iHead + 7] != (byte)0x68))
			{
				return false;
			}
			#endregion

			byte[] recByte = new byte[iEnd - iHead + 1];
			Array.Copy(recBytes, iHead, recByte, 0, recByte.Length);

			#region 检查数据域长度
			//TODO:检查数据域长度

			#endregion

			#region 检查通信地址
			if (AddrStr != "AAAAAAAAAAAA" && AddrStr != "aaaaaaaaaaaa")
			{
				if (addrStrReverse != recByte[1].ToString("X2") + recByte[2].ToString("X2") + recByte[3].ToString("X2") + recByte[4].ToString("X2") + recByte[5].ToString("X2") +
					recByte[6].ToString("X2"))
				{
					errStr = "收到的报文通信地址错误！";
					//return false;
				}
			}
			#endregion

			for (int i = 10; i < (recByte[9] + 10); i++)
			{
				recByte[i] -= 0x33;
			}

			try
			{
				switch (controlType)
				{
					case ControlType.Read:
					case ControlType.ReadLoad:
					case ControlType.ReadFactory:
						switch (recByte[8])
						{
							case 0x91:
							case 0xb1:
								if (recByte[9] < 5)
								{
									if(controlType != ControlType.ReadLoad)
									{
										errStr = "收到的报文数据域长度非法！";
										return false;
									}
								}

								switch (dataMarks.Count)
								{
									case 1:
										string sTemp = recByte[13].ToString("X2") + recByte[12].ToString("X2") + recByte[11].ToString("X2") + recByte[10].ToString("X2");
										if (dataMarks[0] != recByte[13].ToString("X2") + recByte[12].ToString("X2") + recByte[11].ToString("X2") + recByte[10].ToString("X2"))
										{
											errStr = "收到的报文数据标识错误！";
											return false;
										}
										byte[] dataBytes = new byte[recByte[9] - 4];
										Array.Copy(recByte, 14, dataBytes, 0, dataBytes.Length);
										Array.Reverse(dataBytes);
										dataField = Helps.ByteToStringWithNoSpace(dataBytes);
										if (recByte[8] == 0xb1)
										{
											isHaveHouxu = true;
										}
										break;
									case 2:
										if ((dataMarks[1] != recByte[13].ToString("X2") + recByte[12].ToString("X2") + recByte[11].ToString("X2") + recByte[10].ToString("X2")) || (dataMarks[0].Substring(4) != recByte[15].ToString("X2") + recByte[14].ToString("X2")))
										{
											errStr = "收到的报文数据标识错误！";
											return false;
										}
										dataBytes = new byte[recByte[9] - 6];
										Array.Copy(recByte, 16, dataBytes, 0, dataBytes.Length);
										Array.Reverse(dataBytes);
										dataField = Helps.ByteToStringWithNoSpace(dataBytes);
										if (recByte[8] == 0xb1)
										{
											isHaveHouxu = true;
										}
										break;
									default:
										errStr = "数据标识有误！";
										return false;
								}
								break;
							case 0xd1:
								//TODO:解析失败原因
								errStr = "抄读失败！";
								return false;
							default:
								errStr = "收到的报文控制码不匹配！";
								return false;
						}
						break;
					case ControlType.Read_zuhe:
						#region 组合抄读
						switch (recByte[8])
						{
							case 0x91:
							case 0xb1:
								if (recByte[9] < 5)
								{
									errStr = "收到的报文数据域长度非法！";
									return false;
								}

								if (("EEEEEE" + dataMarks.Count.ToString("D2")) != recByte[13].ToString("X2") + recByte[12].ToString("X2") + recByte[11].ToString("X2") + recByte[10].ToString("X2"))
								{
									errStr = "收到的报文数据标识错误！";
									return false;
								}
								byte[] dataBytes = new byte[recByte[9] - 4];
								Array.Copy(recByte, 14, dataBytes, 0, dataBytes.Length);
								dataField = Helps.ByteToStringWithNoSpace(dataBytes);
								if (recByte[8] == 0xb1)
								{
									isHaveHouxu = true;
								}
								break;
							case 0xd1:
								//TODO:解析失败原因
								errStr = "组合抄读失败！";
								return false;
							default:
								errStr = "收到的报文控制码不匹配！";
								return false;
						}


						#endregion
						break;
					case ControlType.ReadFollow:
						switch (recByte[8])
						{
							case 0x92:
							case 0xb2:
								byte[] dataBytes = new byte[recByte[9] - 5];
								if (recByte[recByte.Length - 3] != seq)
								{
									errStr = "收到的帧序号不对！";
									return false;
								}
								Array.Copy(recByte, 14, dataBytes, 0, dataBytes.Length);
								Array.Reverse(dataBytes);
								dataField = Helps.ByteToStringWithNoSpace(dataBytes);
								if (recByte[8] == 0xb2)
								{
									isHaveHouxu = true;
								}
								break;
							default:
								errStr = "抄读后续帧失败！";
								return false;
						}
						break;
					case ControlType.SafetyReadFollowLTU:
						switch (recByte[8])
						{
							case 0x82:
							case 0xA2:
								byte[] dataBytes = new byte[recByte[9] - 9];
								if (recByte[recByte.Length - 7] != seq)
								{
									errStr = "收到的帧序号不对！";
									return false;
								}
								Array.Copy(recByte, 14, dataBytes, 0, dataBytes.Length);
								Array.Reverse(dataBytes);
								dataField = Helps.ByteToStringWithNoSpace(dataBytes);
								if (recByte[8] == 0xA2)
								{
									isHaveHouxu = true;
								}
								break;
							default:
								errStr = "抄读后续帧失败！";
								return false;
						}
						break;
					case ControlType.Write:
						switch (recByte[8])
						{
							case 0x94:
								if (recByte[9] != 00)
								{
									errStr = "收到的报文数据域非法！";
									return false;
								}
								break;
							case 0xD4:
								//TODO:解析失败原因
								errStr = "设置失败！";
								return false;
							default:
								errStr = "收到的报文控制码不匹配！";
								return false;
						}
						break;
					case ControlType.SafetyCertificationLTU:
						switch (recByte[8])
						{
							case 0x80:
								if (recByte[9] < 5)
								{
									errStr = "收到的报文数据域长度非法！";
									return false;
								}

								byte[] dataBytes = new byte[recByte[9]];
								Array.Copy(recByte, 10, dataBytes, 0, dataBytes.Length);
								dataField = Helps.ByteToStringWithNoSpace(dataBytes);
								break;
							case 0xC0:
								//TODO:解析失败原因
								errStr = "抄读失败！";
								return false;
							default:
								errStr = "收到的报文控制码不匹配！";
								return false;
						}
						break;
					case ControlType.SafetyReadLTU:
					case ControlType.SafetyReadLoadLTU:
						switch (recByte[8])
						{
							case 0x81:
							case 0xA1:
								if (recByte[9] < 5)
								{
									errStr = "收到的报文数据域长度非法！";
									return false;
								}
								string sTemp = recByte[13].ToString("X2") + recByte[12].ToString("X2") + recByte[11].ToString("X2") + recByte[10].ToString("X2");
								if (dataMarks[0] != sTemp)
								{
									errStr = "收到的报文数据标识错误！";
									return false;
								}
								byte[] baDataBytes = new byte[recByte[9] ];
								Array.Copy(recByte, 10, baDataBytes, 0, baDataBytes.Length);
								byte [] baSendBytes = m_SCUProxy.HY_SendToProxyBuf(0x01, 0x14, 0x11, baDataBytes);
								//output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
								byte[] baRecBytes =  winMain.SpWuCha.SendCommand(baSendBytes, "EncryptSCU");
								//output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);
								sTemp = baRecBytes[11].ToString("X2") + baRecBytes[10].ToString("X2") + baRecBytes[9].ToString("X2") + baRecBytes[8].ToString("X2");
								if (dataMarks[0] != sTemp)
								{
									errStr = "收到的报文数据标识错误！";
									return false;
								}
								baDataBytes = new byte[baRecBytes.Length - 13] ;
								Array.Copy(baRecBytes, 12, baDataBytes, 0, baDataBytes.Length);
								dataField = Helps.ByteToStringWithNoSpace(baDataBytes);
								dataField = Helps.fnAgainst(dataField);
								if (recByte[8] == 0xA1)
								{
									isHaveHouxu = true;
								}
								break;
							case 0xC0:
								//TODO:解析失败原因
								errStr = "抄读失败！";
								return false;
							case 0xC1:
								//TODO:解析失败原因
								errStr = "抄读失败！";
								return false;
							default:
								errStr = "收到的报文控制码不匹配！";
								return false;
						}
						break;
					default:
						if (((recByte[8] & 0x80) == 0x80))
						{
							if (((recByte[8] & 0x40) == 0x40))
							{
								errStr = "失败！";
								return false;
							}
						}
						break;
				}
				return true;
			}
			catch (Exception e)
			{
				errStr += e.Message;
				return false;
			}
		}

		#region 厂内模式
		//         lstrDataMark.Clear();
		//         lstrDataMark.Add("04DFDFDB");
		//         lstrDataMark.Add("DBDF0201");

		//          _dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString() ) );
		//if (!_dlt645.Pack(DLT645.ControlType.Write, lstrDataMark, "01", 0, out baSendBytes, out strErr, pmc[7].Value.ToString(), pmc[8].Value.ToString() ) )
		//{

		//}

		//         if (sendOK(baSendBytes, 0x94))
		//         {
		//             output(" 打开编程模式成功", false, true);
		//         }
		//         else
		//         {
		//             output(" 通信异常,请检查", true, true);
		//         }

		////byte[] ReceBuf = _sp1.SendCommand(ClassFactory.GetFactoryBytes(Parent.ServerID), ClassFactory.GetFactoryBytes(Parent.ServerID).Length, waitDelay, "645");

		//         //ShowMessageInRecvMessageText(ClassFactory.GetFactoryBytes(Parent.ServerID), Color.Black);
		//         System.Threading.Thread.Sleep(1000);//等待1秒
		//         if (ReceBuf == null)
		//         {
		//             //SendMessageTextAppend(RecvMessage, DateTime.Now.ToString() + " 未收到报文返回,请检查电表通讯!" + "\n", Color.Red);
		//             return;
		//         }
		//         else
		//         {
		//	//ShowMessageInRecvMessageText(ReceBuf, Color.Black);
		//             int iIndex = Helps.RecCutNum(ReceBuf, "645");
		//	if (ReceBuf[iIndex + 8] == 0x94)
		//	{
		//		//SendMessageTextAppend(RecvMessage, "打开厂内模式成功!", Color.Green);
		//	}
		//	else
		//	{
		//		//SendMessageTextAppend(RecvMessage, "打开厂内模式失败!", Color.Red);
		//	}
		//         }
		#endregion

		#region MyRegion
		//public bool UnPackMoninter(byte[] recBytes, out ControlType controlType, List<string> dataMarks,out string dataField, out string errStr)
		//{

		//}
		

		#endregion
	}
}
