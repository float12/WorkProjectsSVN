using System;
using System.Collections.Generic;
using System.Data;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace WindowsTest
{
	class Helps
	{
		public static string AsciiByt2Str(byte[] buf)
		{
			string result = "";
			try
			{
				result = System.Text.Encoding.ASCII.GetString(buf);
			}
			catch (Exception e)
			{

			}
			return result;
		}

		public static byte[] Str2AsciibBytes(String xmlStr)
		{
			return Encoding.Default.GetBytes(xmlStr);
		}

		public const string PATTERN = @"([^A-Fa-f0-9]|\s+?)+";

		/// <summary>
		/// 判断十六进制字符串hex是否正确
		/// </summary>
		/// <param name="hex">十六进制字符串</param>
		/// <returns>true：不正确，false：正确</returns>
		public static bool IsIllegalHexadecimal(string hex)
		{
			return System.Text.RegularExpressions.Regex.IsMatch(hex, PATTERN);
		}

		public static IList<char> HexSet = new List<char>()
			{ '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','a','b','c','d','e','f' };

		/// <summary>
		/// 判断十六进制字符串hex是否正确
		/// </summary>
		/// <param name="hex">十六进制字符串</param>
		/// <returns>true：不正确，false：正确</returns>
		public static bool IsIllegalHexadecimal1(string hex)
		{
			foreach (char item in hex)
			{
				if (!HexSet.Contains<char>(item))
					return true;
			}
			return false;
		}

		/// <summary>
		/// 00 解析错误
		/// </summary>
		/// <param name="xmlStr"></param>
		/// <returns>返回的结果不带空格，首字节为字节数</returns>
		public static string Str2AsciiStr(String xmlStr)
		{
			byte[] bytes;
			if (!TryStrToHexByte(xmlStr, out bytes))
			{
				return "";
			}
			return DataConvert.Byte2String(bytes, 0, bytes.Length);
			//return System.Text.Encoding.ASCII.GetString(bytes);
			//byte[] zzBytes = Str2AsciibBytes(xmlStr);
			//string resultStr = zzBytes.Length.ToString("X2");
			//for (int i = 0; i < zzBytes.Length; i++)
			//{
			//    resultStr += zzBytes[i].ToString("X2");
			//}

			//return resultStr;
		}


        /// <summary>
        /// 含中文字符串转ASCII
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        public static string Str2ASCII(String str)
        {
            try
            {
                //这里我们将采用2字节一个汉字的方法来取出汉字的16进制码
                byte[] textbuf = Encoding.Default.GetBytes(str);
                //用来存储转换过后的ASCII码
                string textAscii = string.Empty;

                for (int i = 0; i < textbuf.Length; i++)
                {
                    textAscii += textbuf[i].ToString("X");
                }
                return textAscii;
            }
            catch (Exception ex)
            {
                Console.WriteLine("含中文字符串转ASCII异常" + ex.Message);
            }
            return "";
        }

        /// <summary>
        /// ASCII转含中文字符串
        /// </summary>
        /// <param name="textAscii">ASCII字符串</param>
        /// <returns></returns>
        public static string ASCII2Str(string textAscii)
        {
            try
            {
                int k = 0;//字节移动偏移量

                byte[] buffer = new byte[textAscii.Length / 2];//存储变量的字节

                for (int i = 0; i < textAscii.Length / 2; i++)
                {
                    //每两位合并成为一个字节
                    buffer[i] = byte.Parse(textAscii.Substring(k, 2), System.Globalization.NumberStyles.HexNumber);
                    k = k + 2;
                }
                //将字节转化成汉字
                return Encoding.Default.GetString(buffer);
            }
            catch (Exception ex)
            {
                Console.WriteLine("ASCII转含中文字符串异常" + ex.Message);
            }
            return "";
        }

		public static double Daddition(double a, double b, int point)
		{
			String kn = (a + b).ToString(string.Format("N{0}", point));
			return Convert.ToDouble(kn);
		}

		public static double Dsubtraction(double a, double b, int point)
		{
			String kn = (a - b).ToString(string.Format("N{0}", point));
			return Convert.ToDouble(kn);
		}

		/// <summary>
		/// 字符串转16进制字节数组(字节间有没有空格均可)
		/// </summary>
		/// <param name="hexString"></param>
		/// <returns></returns>
		public static bool TryStrToHexByte(string hexString, out byte[] resultBytes)
		{
			resultBytes = null;
			try
			{
				if (string.IsNullOrEmpty(hexString))
				{
					return false;
				}

				hexString = hexString.Replace(" ", "");
				if ((hexString.Length % 2) != 0)
					hexString += " ";
				resultBytes = new byte[hexString.Length / 2];
				for (int i = 0; i < resultBytes.Length; i++)
					resultBytes[i] = Convert.ToByte(hexString.Substring(i * 2, 2), 16);
				return true;
			}
			catch (Exception e)
			{
				return false;
			}
		}

		/// <summary>
		/// 字符串转16进制字节数组(字节间有没有空格均可)
		/// </summary>
		/// <param name="hexString"></param>
		/// <param name="resultBytes"></param>
		/// <param name="errStr">转换失败的原因</param>
		/// <returns></returns>
		public static bool TryStrToHexByte(string hexString, out byte[] resultBytes, out string errStr)
		{
			resultBytes = null;
			errStr = "";
			try
			{
				if (string.IsNullOrEmpty(hexString))
				{
					errStr = "传入的内容为空";
					return false;
				}

				hexString = hexString.Replace(" ", "");
				if ((hexString.Length % 2) != 0)
				{
					errStr = "传入的数据长度不是偶数";
					return false;
				}
				resultBytes = new byte[hexString.Length / 2];
				for (int i = 0; i < resultBytes.Length; i++)
				{
					//resultBytes[i] = Convert.ToByte(hexString.Substring(i * 2, 2), 16);
					if (!byte.TryParse(hexString.Substring(i * 2, 2), System.Globalization.NumberStyles.AllowHexSpecifier, null, out resultBytes[i]))
					{
						errStr = "存在无法转换的非法字符串";
						return false;
					}
				}
				return true;
			}
			catch (Exception e)
			{
				errStr = e.Message;
				return false;
			}
		}

		/// <summary>
		/// 把字节型转换成十六进制字符串 
		/// </summary>
		/// <param name="InBytes"></param>
		/// <returns></returns>
		public static string ByteToStringWithSpace(byte[] InBytes)
		{
			if (InBytes == null)
			{
				return "";
			}
			string StringOut = "";
			foreach (byte InByte in InBytes)
			{
				StringOut = StringOut + " " + String.Format("{0:X2}", InByte);
			}
			//WriteLog.Write(DateTime.Now.ToString("HH:mm:ss:fff " + StringOut));   
			return StringOut.Trim();

		}

		/// <summary>
		/// 把字节型转换成十六进制字符串
		/// </summary>
		/// <param name="InBytes"></param>
		/// <returns></returns>
		public static string ByteToStringWithNoSpace(byte[] InBytes)
		{
			if (InBytes == null)
				return "";
			string StringOut = "";
			foreach (byte InByte in InBytes)
			{
				StringOut += InByte.ToString("X2");
			}

			return StringOut.Trim();
		}

		/// <summary>
		/// 将16进制转换为有符号的10进制
		/// </summary>
		/// <param name="hexstr"></param>
		/// <returns></returns>
		public static string ConvertHexToSIntStr(string hexstr)
		{

			if (hexstr.StartsWith("0x"))
			{
				hexstr = hexstr.Substring(2);
			}

			//如果不是有效的16进制字符串或者字符串长度大于16或者是空，均返回NULL

			if (!IsHexadecimal(hexstr) || hexstr.Length > 16 || string.IsNullOrEmpty(hexstr))
			{
				return null;
			}
			if (hexstr.Length > 8)
			{
				return Convert.ToInt64(hexstr, 16).ToString();
			}
			else if (hexstr.Length > 4)
			{
				return Convert.ToInt32(hexstr, 16).ToString();
			}
			else
			{
				return Convert.ToInt16(hexstr, 16).ToString();
			}
		}

		/// <summary>
		/// 数据域颠倒
		/// </summary>
		/// <param name="strValue"></param>
		/// <returns></returns>
		public static string fnAgainst(string strValue)
		{
			string strRet = "";
			for (int i = 0; i < strValue.Length; i = i + 2)
			{
				strRet = strValue.Substring(i, 2) + strRet;
			}
			return strRet;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="dataStr"></param>
		/// <param name="byteLen">字节数</param>
		/// <returns></returns>
		public static string GetHexStrReverse(string dataStr, int byteLen)
		{
			dataStr = dataStr.Replace(".", "");
			UInt32 data = Convert.ToUInt32(dataStr);
			return fnAgainst(data.ToString("X" + (byteLen * 2).ToString()));
		}

		/// <summary>
		/// 计算累加和的方法
		/// </summary>
		/// <param name="a"></param>       
		/// <returns></returns>
		public static byte Ssum(byte[] a)
		{
			byte[] b = { 0x00 };
			for (int i = 0; i < a.Length; i++)
			{
				b[0] += a[i];
			}
			return b[0];
		}

		/// <summary>
		/// 计算累加和的方法
		/// </summary>
		/// <param name="a"></param>       
		/// <returns></returns>
		public static string funSum(string strIn)
		{
			int iSum = 0;
			byte[] baData;
			TryStrToHexByte(strIn, out baData, out string strErr);
			for (int i = 0; i < baData.Length; i++)
			{
				iSum += baData[i];
			}
			return fnAgainst(iSum.ToString("X4") );
		}

		/// <summary>
		/// 去掉FE前导字节前的无用字节
		/// </summary>
		/// <param name="Rec">收到的报文</param>
		/// <returns>正确报文开始的位置，不包含FE</returns>
		public static int RecCutNum(byte[] Rec)
		{
			int count = -1;
			if (Rec.Length > 5)
			{
				for (int i = 4; i < Rec.Length; i++)
				{
					if ((Rec[i] == 0x68) && (Rec[i - 1] == 0xFE) && (Rec[i - 2] == 0xFE) && (Rec[i - 3] == 0xFE) && (Rec[i - 4] == 0xFE))
					{
						count = i - 4;
					}
				}
			}
			return count;
		}

		/// <summary>
		/// 返回68所在位置
		/// </summary>
		/// <param name="Rec"></param>
		/// <returns></returns>
		public static int RecCutNum(byte[] Rec, string type)
		{
			try
			{
				switch (type)
				{
					case "645":
						{
							for (int i = 0; i < Rec.Length; i++)
							{
								if (Rec[i] == 0x68)
								{
									if (Rec[i + 7] == 0x68)
									{
										return i;
									}
								}
							}

							break;
						}
					case "376.1":
						{
							for (int i = 0; i < Rec.Length; i++)
							{
								if (Rec[i] == 0x68)
								{
									if (Rec[i + 5] == 0x68)
									{
										return i;
									}
								}
							}

							break;
						}
					case "698":
						for (int i = 0; i < Rec.Length - 2; i++)
						{
							if (Rec[i] == 0x68)
							{
								int length = Rec[i + 1];
								length += Rec[i + 2] * 0x100;
								if (Rec.Length < ((length + i + 2) - 1))
								{
									continue;
								}

								if (Rec[(length + i) + 1] == 0x16)
								{
									return i;
								}
							}
						}
						//int length = buff[index++];
						////Length 
						//length += buff[index++] * 0x100;
						//if (buff.Length < ((length + index) - 1))
						//{
						//    throw new Exception("非法数据帧：数据域不全");
						//}
						//if (buff[(length + startIndex) + 1] != 0x16)
						//{
						//    throw new Exception("非法数据帧：未找到结束帧");

						break;
				}

				return -1;
			}
			catch (Exception e)
			{
				return -1;
			}

		}

		/// <summary>
		/// JudgeMessageCorrect
		/// </summary>
		/// <param name="Rec"></param>
		/// <param name="strType"></param>
		/// <returns></returns>
		public static string JudgeMessageCorrect(byte[] Rec, string strType)
		{
			try
			{
				byte[] baCRC = null;
				switch (strType)
				{
					case "645":
						{
							for (int i = 0; i < Rec.Length; i++)
							{
								if (Rec[i] == 0x68)
								{
									byte[] copy = new byte[Rec.Length - 2 - i];
									Array.Copy(Rec, i, copy, 0, copy.Length);

									if (Rec[Rec.Length - 2] != Helps.Ssum(copy))
									{
										return "校验错误!";
									}

									if (Rec[i + 7] == 0x68)
									{
										if ((Rec[i + 8] & 0x40) == 0x40)
										{
											return "失败";
										}
										else
										{
											return "";
										}
									}
								}
							}
							break;
						}
					case "MODBUS":
						{
							byte[] copy = new byte[Rec.Length - 2];
							Array.Copy(Rec, 0, copy, 0, copy.Length);
							MODBUS_CRC16.RsCrc(copy, copy.Length, out byte crcH, out byte crcL);
							if (crcH != Rec[Rec.Length - 2] || crcL != Rec[Rec.Length - 1])
							{
								return "校验错误!";
							}
							if ((Rec[1] & 0x80) == 0x80)
							{
								return "失败";
							}
							else
							{
								return "";
							}
							break;
						}
					case "376.1":
						{
							for (int i = 0; i < Rec.Length; i++)
							{
								if (Rec[i] == 0x68)
								{
									if (Rec[i + 5] == 0x68)
									{
										return "";
									}
								}
							}
							break;
						}
					case "698":
						for (int i = 0; i < Rec.Length - 2; i++)
						{
							if (Rec[i] == 0x68)
							{
								byte[] copy = new byte[2];
								Array.Copy(Rec, i, copy, 0, copy.Length);
								ushort uCountFCS16 = WSDFCS16.CountFCS16(Rec, (i + 1), Rec.Length - 4 - i);

								Helps.TryStrToHexByte(uCountFCS16.ToString("X4"), out copy);
								if (copy[0] != Rec[Rec.Length - 3] || copy[1] != Rec[Rec.Length - 2])
								{
									return "校验错误!";
								}

								int length = Rec[i + 1];
								length += Rec[i + 2] * 0x100;
								//if (Rec.Length < ((length + i + 2) - 1))
								//{
								//	continue;
								//}

								if (Rec[(length + i) + 1] == 0x16)
								{
									return "";
								}
							}
						}
						break;
				}
				return "失败";
			}
			catch (Exception e)
			{
				return "失败";
			}
		}

		/// <summary>
		/// 判断是否是十六进制格式字符串
		/// </summary>
		/// <param name="str"></param>
		/// <returns></returns>
		public static bool IsHexadecimal(string str)
		{
			const string PATTERN = @"[A-Fa-f0-9]+$";
			return System.Text.RegularExpressions.Regex.IsMatch(str, PATTERN);
		}

		/// <summary>
		/// 使用Guid产生的种子生成真随机数
		/// </summary>
		public static byte[] GetRandomByGuid(byte len)
		{
			byte[] array = new byte[len];
			//int len = array.Length;
			Random random = new Random(GetRandomSeedbyGuid() + DateTime.Now.Month * 100000000 + DateTime.Now.Day * 1000000 + DateTime.Now.Hour * 10000 + DateTime.Now.Minute * 100 + DateTime.Now.Millisecond);
			for (byte i = 0; i < len; i++)
			{
				int randInt = random.Next(0, (int)len);
				array[i] = (byte)randInt;
			}
			return array;
		}

		/// <summary>
		/// 使用Guid生成种子
		/// </summary>
		/// <returns></returns>
		static int GetRandomSeedbyGuid()
		{
			return new Guid().GetHashCode();
		}

		/// <summary>
		/// 等待时间(ms)
		/// </summary>
		/// <param name="iMSWait"></param>
		public static void fnWait(int iMSWait)
		{
			int ii = iMSWait / 1;
			if (ii <= 0) ii = 1;
			while (ii-- > 0)
			{
				System.Threading.Thread.Sleep(1);
				System.Windows.Forms.Application.DoEvents();
			}
		}

		public static void DealRTCData(ref string tmpstr)
		{
			#region RTC数据处理
			if (!DataConvert.IsUnsign(tmpstr))
			{
				//负数最高位置1 注意不是取反加一
				tmpstr = tmpstr.Substring(1); //去掉符号
				double dTimeClock = double.Parse(tmpstr);
				dTimeClock = dTimeClock * 0.0869 * 1000; //PPM转化为日计时误差           
				int iTimeClock = (int)dTimeClock;
				int m = iTimeClock | 0x8000;
				tmpstr = m.ToString("X4");
			}
			else
			{
				double dTimeClock = double.Parse(tmpstr);
				dTimeClock = dTimeClock * 0.0869 * 1000; //PPM转化为日计时误差     
				int iTimeClock = (int)dTimeClock;
				tmpstr = iTimeClock.ToString("X4");
			}
			#endregion
		}

		bool Value2Ppm(string Value, out double resultD)
		{
			resultD = 99.99d;
			double zhongzhuanD;
			if (string.IsNullOrEmpty(Value))
			{
				return false;
			}

			if (Value.Length < 4)
			{
				return false;
			}

			if (Value.Substring(Value.Length - 3, 3) == "MHZ")
			{
				Value = Value.Substring(0, Value.Length - 3);
				if (!double.TryParse(Value, out zhongzhuanD))
				{
					return false;
				}

				resultD = Helps.Dsubtraction(zhongzhuanD, 1000, 5) * 1000.00;
			}
			else if (Value.Substring(Value.Length - 2, 2) == "HZ")
			{
				Value = Value.Substring(0, Value.Length - 2);
				if (!double.TryParse(Value, out zhongzhuanD))
				{
					return false;
				}

				resultD = Helps.Dsubtraction(zhongzhuanD, 1, 8) * 1000000.00;
			}

			return true;
		}

		/// <summary>
		/// 写Bin文件
		/// </summary>
		/// <param name="strPathOut"></param>
		/// <param name="strPathIn"></param>
		/// <param name="strLength"></param>
		/// <param name="ImageCRC"></param>
		public static void CombineMethod(string strPathOut, string strPathIn, string strLength, uint ImageCRC)
		{
			uint num = 0;
			try
			{
				BinaryReader reader = null;
				FileStream output = File.Open(strPathOut.Trim(), FileMode.Create, FileAccess.Write);
				BinaryWriter writer = new BinaryWriter(output);
				{
					FileStream input = File.Open(strPathIn.ToString(), FileMode.Open, FileAccess.Read);
					reader = new BinaryReader(input);
					reader.BaseStream.Seek(0L, SeekOrigin.Begin);
					//bin文件不是4字节对齐
					uint u = 0;
					while (reader.BaseStream.Position < reader.BaseStream.Length)
					{
						u = (uint)(reader.BaseStream.Length - reader.BaseStream.Position);
						if (u < 4)
						{
							num += u;
							if (u == 1)
							{
								char cData = reader.ReadChar();
								writer.Write(cData);
							}
							else if (u == 2)
							{
								ushort uData16 = reader.ReadUInt16();
								writer.Write(uData16);
							}
							else
							{
								ushort uData16 = reader.ReadUInt16();
								writer.Write(uData16);
								num += 2;
								char cData = reader.ReadChar();
								writer.Write(cData);
							}
						}
						else
						{
							uint num3 = reader.ReadUInt32();
							writer.Write(num3);
							num += 4;
						}
					}
					writer.Flush();
					input.Close();
					if (strLength != "")
					{
						uint num2 = Convert.ToUInt32(strLength, 0x10);
						if ((num2 > 0) && (num < (num2 - 1)))
						{
							while (num < num2)
							{
								int uAddr = (int)(num2 - num - 1);
								if ((uAddr <= 1))
								{
									if (u == 3)
									{
										num += 4;
										writer.Write(uint.MaxValue);
										num += 1;
										writer.Write((byte)(0xff));
									}
									else if (u == 2)
									{
										num += 2;
										writer.Write(ushort.MaxValue);
									}
									else//补三个字节
									{
										num += 2;
										writer.Write(ushort.MaxValue);

										num += 1;
										writer.Write((byte)(0xff));
									}
								}
								else
								{
									writer.Write(uint.MaxValue);
									num += 4;
								}
							}

							writer.Flush();
						}
					}

					writer.Write(ImageCRC);
					writer.Flush();
				}
				writer.Close();
				output.Close();

				//MessageBox.Show("合并完成!!!");
			}
			catch
			{
				//MessageBox.Show("合并异常!!!");
			}
		}

		/// <summary>
	    /// lbaArray[]数组保存成文件
	    /// </summary>
	    /// <param name="lbaArray">byte[]数组</param>
	    /// <param name="fileName">保存至硬盘的文件路径</param>
	    /// <returns></returns>
	    public static bool ByteToFile(List<byte[]> lbaArray, string fileName)
	    {
	        bool result = false;
	        try
	        {
				//FileStream output = File.Open(fileName.Trim(), FileMode.Create, FileAccess.Write);
				//BinaryWriter writer = new BinaryWriter(output);
				//for (int i = 0; i < lbaArray.Count; i++)
				//{

				//	output.Write(lbaArray[i], i * lbaArray[i].Length, lbaArray[i].Length);
				//}

				//char cData = reader.ReadChar();
				//				writer.Write(cData);

				using (FileStream fs = new FileStream(fileName, FileMode.OpenOrCreate, FileAccess.Write))
				{
					if (fs.Length != 0)
					{
						fs.Close();
						File.Delete(fileName);

						using ( FileStream fsNew = new FileStream(fileName, FileMode.Create, FileAccess.Write))
						{
							for (int i = 0; i < lbaArray.Count; i++)
							{
								fsNew.Write(lbaArray[i], 0, lbaArray[i].Length);
							}
						}
					}
					else
					{
						for (int i = 0; i < lbaArray.Count; i++)
						{
							fs.Write(lbaArray[i], 0, lbaArray[i].Length);
						}
					}
					result = true;
				}
			}
	        catch
	        {
	            result = false;
	        }
	        return result;
	    }

		/// <summary>
		/// 执行DataTable中的查询返回新的DataTable
		/// </summary>
		/// <param name="dt">源数据DataTable</param>
		/// <param name="condition">查询条件</param>
		/// <returns></returns>
		public static DataTable GetNewDataTable(DataTable dt, string condition)
		{
			if (dt == null) return null;
			DataTable newdt = new DataTable();
			newdt = dt.Clone();
			DataRow[] dr = dt.Select(condition);
			for (int i = 0; i < dr.Length; i++)
			{
				newdt.ImportRow((DataRow)dr[i]);
			}
			return newdt;
		}

		/// <summary>
		/// 按照长度拆分字符串
		/// </summary>
		/// <param name="strs"></param>
		/// <param name="len"></param>
		/// <returns></returns>
		public static String[] SubString(string strData, int ilen)
		{
			if (strData.Length / ilen != 0)
			{
				MessageBox.Show("分割字符串长度不是整数倍!");
				return null;
			}
			string[] straData = new string[int.Parse(Math.Ceiling((double)(strData.Length / ilen)).ToString())];
			for (int j = 0; j < straData.Length; j++)
			{
				ilen = ilen <= strData.Length ? ilen : strData.Length;
				straData[j] = strData.Substring(0, ilen);
				strData = strData.Substring(ilen, strData.Length - ilen);
			}
			return straData;
		}

		public static DataTable FunCreateTableBlockColumns()
		{
			DataTable dtTableBlock = new DataTable();
			dtTableBlock.Columns.Add("Name", typeof(string));
			dtTableBlock.Columns.Add("DataLen", typeof(string));
			dtTableBlock.Columns.Add("DataFormat", typeof(string));
			dtTableBlock.Columns.Add("DecimalPlaces", typeof(string));
			dtTableBlock.Columns.Add("DataUnit", typeof(string));
			dtTableBlock.Columns.Add("Value", typeof(string));
			return dtTableBlock;
		}
	}
}
