using LeafSoft.Units;
using System;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using TX.Framework.WindowUI.Forms;

namespace WindowsTest
{
	public partial class Form_DisplayDataBlock : MainForm
	{
		public Form_DisplayDataBlock()
		{
			InitializeComponent();
			this.DoubleBuffered = true;
			this.Location = new Point(Screen.PrimaryScreen.WorkingArea.Width / 4, Screen.PrimaryScreen.WorkingArea.Height / 4);
		}
		private string _valueStr = "";
		private DataTable DtSource = null;
		bool isHaveAA = false;
		public static string setDataStr = "";
		string strData = "";
		public Form_DisplayDataBlock(DataTable dtSource, string valueStr)
		{
			InitializeComponent();
			_valueStr = valueStr;
			DtSource = dtSource;
			this.DoubleBuffered = true;
			this.Location = new Point(Screen.PrimaryScreen.WorkingArea.Width / 4, Screen.PrimaryScreen.WorkingArea.Height / 4);
		}

		#region DrawCaption
		/// <summary>
		/// Draws the caption.
		/// </summary>
		/// <param name="g">The g.</param>
		protected override void DrawCaption(Graphics g)
		{
			base.DrawCaption(g);
			if (this.CaptionHeight > 0)
			{
				winMain.DrawRibbonBtn(g);
			}
		}
		#endregion

		/// <summary>
		/// 字符串转ASCII数组
		/// </summary>
		/// <param name="strData"></param>
		/// <returns></returns>
		public static byte[] Str2AsciibBytes(String strData)
		{
			return Encoding.Default.GetBytes(strData);
		}

		/// <summary>
		/// 字符串转ASCII字符串
		/// </summary>
		/// <param name="strData"></param>
		/// <returns>返回的结果不带空格</returns>
		public static string Str2AsciiStr(String strData)
		{
			byte[] zzBytes = Str2AsciibBytes(strData);
			string resultStr = "";
			for (int i = 0; i < zzBytes.Length; i++)
			{
				resultStr += zzBytes[i].ToString("X2");
			}

			return resultStr;
		}
		private void Form_DisplayDataBlock_Load(object sender, EventArgs e)
		{
			int len1 = 0;
			string brakestatus = "";
			string strBitData = "";
			bool bBit = false;
			dgv_dataBlock.DataSource = DtSource;
			if (!string.IsNullOrEmpty(_valueStr))
			{
				int len = 0;
				int iJudgeRow = 0;
				int iRowsCount = DtSource.Rows.Count;
				DataTable dtSource = winMain.DataBank.Find(x => x.TableName == "level2");
				DataRow[] drs = dtSource.Select(string.Format("DataBlockType = '{0}' ", DtSource.TableName));

				for (int i = 0; i < DtSource.Rows.Count; i++)
				{
					if (DtSource.Rows[i]["DataFormat"].ToString().Contains("bit"))
					{
						bBit = true;
						//if ( (DtSource.Rows[i]["Name"].ToString().Contains("Bit7") ) && (DtSource.Rows[i]["DataFormat"].ToString().Contains("bits") ) )
						//{
						//	int iRows = 0;

						//	strBitData = DataConvert.HexStringToBinary(_valueStr.Substring(i * 2, 2));

						//	_valueStr = _valueStr.Substring(0, i * 2) + strBitData + _valueStr.Substring(i * 2 + 2);
						//	for (int j = i; j < DtSource.Rows.Count; j++)
						//	{
						//		if (DtSource.Rows[j]["DataFormat"].ToString().Contains("bit"))
						//		{
						//			if (!int.TryParse(DtSource.Rows[j]["DataLen"].ToString(), out len1))
						//			{

						//			}
						//			if (DtSource.Rows[j]["DecimalPlaces"].ToString() == ("判断"))
						//			{
						//				DataTable DtAnalyze = null;
						//				string strAnalyze = DtSource.Rows[j]["DataUnit"].ToString();
						//				DtAnalyze = winMain.DataBank.Find(x => x.TableName == strAnalyze);
						//				if (DtAnalyze == null)
						//				{
						//					return;
						//				}
						//				string strTemp = (strBitData.Substring(0, len1));
						//				for (int k = 0; k < DtAnalyze.Rows.Count; k++)
						//				{
						//					if (strTemp == DtAnalyze.Rows[k]["Value"].ToString())
						//					{
						//						brakestatus = DtAnalyze.Rows[k]["Name"].ToString();
						//						iJudgeRow = j;
						//						break;
						//					}
						//				}
						//			}

						//			iRows += len1;
						//		}
						//		strBitData = strBitData.Substring(len1);
						//		if (iRows > 7)
						//		{
						//			i = j;
						//			len1 = 8;
						//			break;
						//		}
						//	}
						//}
						//else
						{
							if (i == 0)
							{
								if (DtSource.TableName == "主动上报状态字块" || DtSource.TableName.Contains("CAN"))
								{
									_valueStr = Helps.fnAgainst(_valueStr);
								}
								strBitData = DataConvert.HexStringToBinary(_valueStr);
								_valueStr =strBitData;
							}
						}

						//_valueStr = new string(_valueStr.ToCharArray().Reverse().ToArray());
					}
					else
					{
						if (DtSource.Rows[i]["DataLen"].ToString() == "")
						{
							if (string.IsNullOrEmpty(DtSource.Rows[i]["DataLen"].ToString()))
							{

							}
							continue;
						}
						else
						{
							if (!int.TryParse(DtSource.Rows[i]["DataLen"].ToString(), out len1))
							{
								Text = "数据库存储的数据长度有误，请检查数据库！";
								return;
							}
						}
					}

					len += len1;
				}

				if (DtSource.TableName.Contains("AA"))
				{
					isHaveAA = true;
					len += 10;
				}
				else
				{
					isHaveAA = false;
				}

				if (!DtSource.TableName.Contains("CAN") )
				{
					if (_valueStr.Length != 2 * len)
					{
						if ((drs[0]["DataLen"].ToString().Contains("*")))
						{
							string[] straLenA;
							string[] straLen;
							int iLen = 0;
							if (drs[0]["DataLen"].ToString().Contains("+"))
							{
								straLenA = drs[0]["DataLen"].ToString().Split('+');
								straLen = straLenA[1].Split('*');
								if (((_valueStr.Length / 2) - Int32.Parse(straLenA[0])) % Int32.Parse(straLen[0]) != 0)
								{
									Text = "数据长度有误，解析失败！";
									return;
								}
							}
							else
							{
								straLen = drs[0]["DataLen"].ToString().Split('*');
								if ((_valueStr.Length / 2) % Int32.Parse(straLen[0]) != 0)
								{
									Text = "数据长度有误，解析失败！";
									return;
								}
							}
						}
						else if (!bBit)
						{
							Text = "数据长度有误，解析失败！";
							return;
						}
					}
				}

				for (int i = 0; i < DtSource.Rows.Count; i++)
				{
					if (!int.TryParse(DtSource.Rows[i]["DataLen"].ToString(), out len1))
					{
						//Text = "数据库存储的数据长度有误，请检查数据库！";
						return;
					}

					string dataStr = "";
					if (DtSource.Rows[i]["DataFormat"].ToString().Contains("bit"))
					{
						dataStr = _valueStr.Substring(0, len1);
					}
					else
					{
						if ((drs[0]["DataLen"].ToString().Contains("*")))
						{
							if (_valueStr == "")
							{
								for (int j = 1; j <= iRowsCount - i; j++)
								{
									DtSource.Rows[iRowsCount - j].Delete();
								}
								break;
							}
						}
						dataStr = _valueStr.Substring(0, 2 * len1);
					}

					switch (DtSource.Rows[i]["DataFormat"])
					{
						case "BCD":
							if (!int.TryParse(DtSource.Rows[i]["DecimalPlaces"].ToString(), out int decimalPlaces))
							{
								decimalPlaces = 0;
							}

							dataStr = Helps.fnAgainst(dataStr);
							string strSymble = "";
							if ((DtSource.Rows[i]["Name"].ToString().Contains("功率因数")) || (DtSource.Rows[i]["DataUnit"].ToString().Contains("A")) || (DtSource.Rows[i]["DataUnit"].ToString().Contains("kW")) || (DtSource.Rows[i]["DataUnit"].ToString().Contains("Kvar")) || (DtSource.Rows[i]["DataUnit"].ToString().Contains("kVA")))
							{
								int iTe = 0;

								if ((Convert.ToInt64("0x" + dataStr.Substring(0, 1), 16) & 0x08) == 8)
								{
									strSymble = "-";
								}
								iTe = (int)(Convert.ToInt64("0x" + dataStr.Substring(0, 1), 16) & 0x07);
								dataStr = strSymble + iTe.ToString() + dataStr.Substring(1);
							}
							if (decimalPlaces != 0)
							{
								dataStr = dataStr.Insert(dataStr.Length - decimalPlaces, ".");
							}
							DtSource.Rows[i]["Value"] = dataStr;
							break;
						case "HEX10":
							UInt64 dataStrHEX10;
							if (!int.TryParse(DtSource.Rows[i]["DecimalPlaces"].ToString(), out int decimalPlace))
							{
								decimalPlace = 0;
							}
							dataStr = Helps.fnAgainst(dataStr);
							dataStrHEX10 = UInt64.Parse(dataStr, System.Globalization.NumberStyles.HexNumber);

							if (dataStrHEX10 != 0)
							{
								dataStr = dataStrHEX10.ToString();
							}

							if (decimalPlace != 0)
							{
								if (dataStr.Length == decimalPlace)
								{
									dataStr = dataStr.Insert(dataStr.Length - decimalPlace, "0.");
								}
								else
								{
									if (dataStr.Length < decimalPlace)
									{
										dataStr = dataStr.PadLeft((decimalPlace + 1), '0');
									}
									dataStr = dataStr.Insert(dataStr.Length - decimalPlace, ".");
								}
							}

							DtSource.Rows[i]["Value"] = dataStr;
							break;
						case "ASCII":
							if (DtSource.Rows[i]["DataUnit"].ToString() == "Positive")
							{
								dataStr = Helps.fnAgainst(dataStr);
							}
							//System.Text.ASCIIEncoding asciiEncoding = new System.Text.ASCIIEncoding();
							//dataStr = Helps.ASCII2Str(dataStr).PadRight(len1, asciiEncoding.GetChars(new byte[] { 0 })[0]);
							//DtSource.Rows[i]["Value"] = dataStr;

							string strData = "";
							byte[] bData = DataConvert.HEXString2Bytes(dataStr);
							for (int j = (dataStr.Length / 2); j > 0; j--)
							{
								string strValue = Char.ConvertFromUtf32(bData[j - 1]);

								strData += strValue;
							}

							DtSource.Rows[i]["Value"] = strData;
							break; ;
						case "BIN":
							dataStr = DataConvert.HexStringToBinary(dataStr);
							DtSource.Rows[i]["Value"] = dataStr;
							break;
						case "bit":
						case "bits":
							if (brakestatus == "")//顺序解析
							{
								DataTable DtAnalyze = null;
								string strAnalyze = DtSource.Rows[i]["DataUnit"].ToString();
								DtAnalyze = winMain.DataBank.Find(x => x.TableName == strAnalyze);
								if (DtAnalyze == null)
								{
									DtSource.Rows[i]["Value"] = dataStr;
								}
								else
								{
									string strTemp = dataStr;
									for (int j = 0; j < DtAnalyze.Rows.Count; j++)
									{
										if (strTemp == DtAnalyze.Rows[j]["Value"].ToString())
										{
											DtSource.Rows[i]["Value"] = DtAnalyze.Rows[j]["Name"].ToString();
											break;
										}
									}
								}
							}
							else
							{
								if (iJudgeRow == i)
								{
									DtSource.Rows[i]["Value"] = brakestatus;
								}
								else
								{
									DataTable DtAnalyze = null;
									string strAnalyze = brakestatus + "-" + DtSource.Rows[i]["DataUnit"].ToString();
									DtAnalyze = winMain.DataBank.Find(x => x.TableName == strAnalyze);
									if (DtAnalyze == null)
									{
										DtSource.Rows[i]["Value"] = dataStr;
									}
									else
									{
										string strTemp = dataStr;
										for (int j = 0; j < DtAnalyze.Rows.Count; j++)
										{
											if (strTemp == DtAnalyze.Rows[j]["Value"].ToString())
											{
												DtSource.Rows[i]["Value"] = DtAnalyze.Rows[j]["Name"].ToString();
												break;
											}
										}
									}
								}
							}
							//DtSource.Rows[i]["Value"] = dataStr;
							break;
						case "ssmmhhDDMMYY":
							DtSource.Rows[i]["Value"] = dataStr;
							break;
						case "Float":
							UInt32 ui32Temp = Convert.ToUInt32(Helps.fnAgainst(dataStr), 16);//字符串转16进制32位无符号整数
							float fData = BitConverter.ToSingle(BitConverter.GetBytes(ui32Temp), 0);//IEEE754 字节转换float
							DtSource.Rows[i]["Value"] = fData.ToString();
							break;
						default://一律按hex16格式处理;
							dataStr = Helps.fnAgainst(dataStr);
							DtSource.Rows[i]["Value"] = dataStr;
							break;
					}
					if (DtSource.Rows[i]["DataFormat"].ToString().Contains("bit"))
					{
						_valueStr = _valueStr.Substring(len1);
					}
					else
					{
						_valueStr = _valueStr.Substring(2 * len1);
					}
					if (isHaveAA && (i != (DtSource.Rows.Count - 1)))
					{
						if (_valueStr.Substring(0, 2) != "AA" && _valueStr.Substring(0, 2) != "aa")
						{
							Text = "错误：数据域结构错误！";
							return;
						}
						_valueStr = _valueStr.Substring(2);
					}
				}
			}
		}

		private void btn_确认_Click(object sender, EventArgs e)
		{
			this.DialogResult = DialogResult.Cancel;
			this.Close();
		}

		private void btn_添加到设置列_Click(object sender, EventArgs e)
		{
			setDataStr = "";
			for (int i = 0; i < DtSource.Rows.Count; i++)
			{
				if (DtSource.Rows[i]["DataLen"].ToString() == "")
				{
					if (string.IsNullOrEmpty(DtSource.Rows[i]["DataLen"].ToString()))
					{

					}
					continue;
				}

				if (string.IsNullOrEmpty(DtSource.Rows[i]["Value"].ToString()))
				{
					MessageBox.Show("请输入设置值");
					return;
				}

				if (!int.TryParse(DtSource.Rows[i]["DataLen"].ToString(), out int len1))
				{
					Text = "数据库存储的数据长度有误，请检查数据库！";
					return;
				}

				string dataStr = "";
				switch (DtSource.Rows[i]["DataFormat"])
				{
					case "BCD":
						if (!int.TryParse(DtSource.Rows[i]["DecimalPlaces"].ToString(), out int decimalPlaces))
						{
							decimalPlaces = 0;
						}
						if (decimalPlaces != 0)
						{
							double dataD;
							if (!double.TryParse(DtSource.Rows[i]["Value"].ToString(), out dataD))
							{
								MessageBox.Show("设置值非法，请检查");
								return;
							}
							dataStr = ((int)(dataD * Math.Pow(10, decimalPlaces))).ToString(string.Format("D{0}", (2 * len1)));
							if (dataStr.Length > 2 * len1)
							{
								MessageBox.Show("设置值非法，请检查");
								return;
							}
						}
						else
						{
							dataStr = DtSource.Rows[i]["Value"].ToString();
						}
						break;
					case "HEX10":
						if (!int.TryParse(DtSource.Rows[i]["DecimalPlaces"].ToString(), out int decimalPlaces1))
						{
							decimalPlaces1 = 0;
						}
						double dataD1;
						if (!double.TryParse(DtSource.Rows[i]["Value"].ToString(), out dataD1))
						{
							MessageBox.Show("设置值非法，请检查");
							return;
						}
						dataStr = ((int)(dataD1 * Math.Pow(10, decimalPlaces1))).ToString(string.Format("X{0}", (2 * len1)));
						if (dataStr.Length > 2 * len1)
						{
							MessageBox.Show("设置值非法，请检查");
							return;
						}
						break;
					case "ASCII":
						strData = DtSource.Rows[i]["Value"].ToString();
						dataStr = Str2AsciiStr(strData);
						//dataStr = Helps.fnAgainst(dataStr.Trim());
						if (strData.Length > len1)
						{
							MessageBox.Show("设置值长度非法，请检查");
							return ;
						}
						System.Text.ASCIIEncoding asciiEncoding = new System.Text.ASCIIEncoding();
						byte[] asciiByts = new byte[len1] ;
						if (Regex.IsMatch(strData, @"[\u4e00-\u9fa5]") )
						{
							byte [] ba = Helps.Str2AsciibBytes(strData);
							ba.CopyTo(asciiByts, 0);
						}
						else
						{
							asciiByts = Helps.Str2AsciibBytes(strData.PadRight(len1, asciiEncoding.GetChars(new byte[] { 0 })[0]));
						}

						dataStr = SerialTool.byteToHexStr(asciiByts);
						if (DtSource.Rows[i]["DataUnit"].ToString() == "Positive")
						{
							dataStr = Helps.fnAgainst(dataStr);
						}
						break;
					case "BIN":
						dataStr = DtSource.Rows[i]["Value"].ToString();
						dataStr = DataConvert.strBinaryToHexString(dataStr);
						break;
					case "Float":
						dataStr = DtSource.Rows[i]["Value"].ToString();
						float fData = float.Parse(dataStr);
						byte[] ieee754Bytes = BitConverter.GetBytes(fData);
						dataStr = Helps.fnAgainst(DataConvert.fnGetStringFormat(DataConvert.Bytes2HexString(ieee754Bytes, 0, ieee754Bytes.Length) ) ) ;
						break;
					case "bit":
					case "bits":
						int iSelectionLengthBit = 0;
						string strTemp = "";
						for (int m = i; m < DtSource.Rows.Count; m++)
						{
							if (DtSource.Rows[i]["DataUnit"].ToString() != "")
							{
								DataTable DtAnalyze = null;
								string strAnalyze = DtSource.Rows[m]["DataUnit"].ToString();
								DtAnalyze = winMain.DataBank.Find(x => x.TableName == strAnalyze);
								strTemp = DtSource.Rows[m]["Value"].ToString();
								if (DtAnalyze != null)
								{
									for (int j = 0; j < DtAnalyze.Rows.Count; j++)
									{
										if (strTemp == DtAnalyze.Rows[j]["Name"].ToString())
										{
											strTemp = DtAnalyze.Rows[j]["Value"].ToString();
											break;
										}
									}
								}
							}

							iSelectionLengthBit += Int32.Parse(DtSource.Rows[m][1].ToString());
							dataStr += strTemp;
							if (iSelectionLengthBit % 8 == 0)
							{
								dataStr = DataConvert.strBinaryToHexString(dataStr);
								i = m;
								break;
							}
						}
						break;
					default:
						dataStr = (DtSource.Rows[i]["Value"].ToString()).PadLeft(len1 *2, '0');
						break;
				}

				setDataStr += Helps.fnAgainst(dataStr.Trim());
			}

			this.DialogResult = DialogResult.OK;
			this.Close();
		}

		private void tbOutPut_Click(object sender, EventArgs e)
		{
			SaveFileDialog saveFileDialog1 = new SaveFileDialog();
			saveFileDialog1.Filter = "Excel文件（*.xlsx）|*.xlsx|Excel文件（*.xls）|*.xls";
			saveFileDialog1.FilterIndex = 0;
			saveFileDialog1.RestoreDirectory = false;
			string errStr = "";
			if (saveFileDialog1.ShowDialog() == DialogResult.OK)
			{
				List<DataTable> dts = new List<DataTable>();
				dts.Add(DtSource);

				if (!AsposeExcelTools.DataTableListToExcel(dts, saveFileDialog1.FileName, new string[] { "数据块" }, out errStr))
				{
					MessageBox.Show("导出数据失败：" + errStr);
					return;
				}
				else
				{
					MessageBox.Show("导出成功!");
				}
			}
		}
	}
}
