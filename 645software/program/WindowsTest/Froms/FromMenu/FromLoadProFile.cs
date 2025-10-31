using System;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using TX.Framework.WindowUI.Forms;

namespace WindowsTest
{
	public partial class FromLoadProFile : MainForm
	{
		public FromLoadProFile()
		{
			InitializeComponent();
			this.DoubleBuffered = true;
			this.Location = new Point((SystemInformation.PrimaryMonitorSize.Width - this.Width) / 2, (SystemInformation.PrimaryMonitorSize.Height - this.Height) / 2);
		}
		private string _strAddr = "";
		public byte[] baBytes = null;
		List<string> lstrDataMark = new List<string>();
		DataTable dtLoadRecord = new DataTable();
		DataTable dtLoad = new DataTable();
		winMain wm;
		int iPeriod = 15;
		public FromLoadProFile(string strAddr)
		{
			InitializeComponent();
			_strAddr = strAddr;
			this.DoubleBuffered = true;
			this.Location = new Point((SystemInformation.PrimaryMonitorSize.Width - this.Width) / 2, (SystemInformation.PrimaryMonitorSize.Height - this.Height) / 2);
		}


		private void FormAdjustMeter_Load(object sender, EventArgs e)
		{
			dtLoad = winMain.DataBank.Find(x => x.TableName == "负荷记录");
			if (dtLoad == null)
			{
				MessageBox.Show("无'负荷记录'sheet");
				return;
			}

			for (int i = 0; i < dtLoad.Rows.Count; i++)
			{
				cb_LoadCurveType.Items.Add(dtLoad.Rows[i]["Name"].ToString());
			}
			cb_LoadCurveType.SelectedIndex = 0;
			cb_LoadModulType.SelectedIndex = 1;
			dtp_LoadTime.Value = DateTime.Now;
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

		private void cb_LoadCurveType_SelectedIndexChanged(object sender, EventArgs e)
		{
			#region 13版
			if (cb_LoadCurveType.Text == "负荷记录" || cb_LoadCurveType.Text == "第1类负荷记录" || cb_LoadCurveType.Text == "第2类负荷记录" || cb_LoadCurveType.Text == "第3类负荷记录" || cb_LoadCurveType.Text == "第4类负荷记录" || cb_LoadCurveType.Text == "第5类负荷记录" || cb_LoadCurveType.Text == "第6类负荷记录" || cb_LoadCurveType.Text == "第6类负荷记录-谐波数据")
			{
				cb_LoadModulType.Enabled = true;
			}
			else
			{
				cb_LoadModulType.Enabled = true;
				cb_LoadModulType.Text = "给定时间记录块";
				dtp_LoadTime.Enabled = true;
			}
			if (cb_LoadCurveType.Text == "负荷记录")
			{
				cb_LoadModulType.Text = "最早记录块";
				dtp_LoadTime.Enabled = false;
			}
			else if (cb_LoadCurveType.Text == "第1类负荷记录")
			{
				cb_LoadModulType.Text = "最早记录块";
				dtp_LoadTime.Enabled = false;
			}
			else if (cb_LoadCurveType.Text == "第2类负荷记录")
			{
				cb_LoadModulType.Text = "最早记录块";
				dtp_LoadTime.Enabled = false;
			}
			else if (cb_LoadCurveType.Text == "第3类负荷记录")
			{
				cb_LoadModulType.Text = "最早记录块";
				dtp_LoadTime.Enabled = false;
			}
			else if (cb_LoadCurveType.Text == "第4类负荷记录")
			{
				cb_LoadModulType.Text = "最早记录块";
				dtp_LoadTime.Enabled = false;
			}
			else if (cb_LoadCurveType.Text == "第5类负荷记录")
			{
				cb_LoadModulType.Text = "最早记录块";
				dtp_LoadTime.Enabled = false;
			}
			else if (cb_LoadCurveType.Text == "第6类负荷记录")
			{
				cb_LoadModulType.Text = "最早记录块";
				dtp_LoadTime.Enabled = false;
			}
			#endregion
		}

		private void cb_LoadModulType_SelectedIndexChanged(object sender, EventArgs e)
		{
			#region 判断
			if (cb_LoadModulType.Text == "最早记录块")
			{
				tb_ModulNum.Enabled = true;

				//if (cb_LoadCurveType.Text.Contains("负荷记录") )
				{
					dtp_LoadTime.Enabled = false;
				}
			}
			else if (cb_LoadModulType.Text == "最近一个记录块")
			{
				tb_ModulNum.Text = "1";
				tb_ModulNum.Enabled = false;

				//if (cb_LoadCurveType.Text.Contains("负荷记录") )
				{
					dtp_LoadTime.Enabled = false;
				}
			}
			else//给定时间记录块
			{
				tb_ModulNum.Enabled = true;
				dtp_LoadTime.Enabled = true;
			}
			#endregion
		}

        private async Task HandleClickAsync()
        {
            // Can use ConfigureAwait here.
	        await Task.Delay(1000).ConfigureAwait(continueOnCapturedContext: false);
        }




		private async  void btn_ReadLoadCurve_Click(object sender, EventArgs e)
		{
			int iCount, iCountAll, iCircle = 0;

			if (!int.TryParse(tb_ModulNum.Text, out iCountAll))
			{
				return;
			}
			DateTime dtTimeRead = DateTime.ParseExact(dtp_LoadTime.Text.ToString(), "yy-MM-dd HH:mm", System.Globalization.CultureInfo.CurrentCulture);
			string dataMark = "";
			int iLenData = 0;//字节数
			int iPartLen = 0;

			//TODO:暂时不需要
			//if (cb_LoadCurveType.SelectedIndex < 7)
			//{
			//	switch (cb_LoadCurveType.Text) 
			//	{
			//	case "负荷记录":
			//		dataMark = "060000" + cb_LoadModulType.SelectedIndex.ToString("X2");
			//		break;
			//             case "第1类负荷记录":
			//                 dataMark = "060100" + cb_LoadModulType.SelectedIndex.ToString("X2");
			//		break;
			//             case "第2类负荷记录":
			//                 dataMark = "060200" + cb_LoadModulType.SelectedIndex.ToString("X2");
			//		break;
			//             case "第3类负荷记录":
			//                 dataMark = "060300" + cb_LoadModulType.SelectedIndex.ToString("X2");
			//		break;
			//             case "第4类负荷记录":
			//                 dataMark = "060400" + cb_LoadModulType.SelectedIndex.ToString("X2");
			//		break;
			//             case "第5类负荷记录":
			//                 dataMark = "060500" + cb_LoadModulType.SelectedIndex.ToString("X2");
			//		break;
			//             case "第6类负荷记录":
			//                 dataMark = "060600" + cb_LoadModulType.SelectedIndex.ToString("X2");
			//		break;
			//             }
			//}
			//else
			{
				for (int i = 0; i < dtLoad.Rows.Count; i++)
				{
					if (cb_LoadCurveType.Text.ToString() == dtLoad.Rows[i]["Name"].ToString())
					{
						dataMark = dtLoad.Rows[i]["DataMark"].ToString();
					}
				}
			}

			dgv_LoadCurve.DataSource = dtLoadRecord;
			while (dtLoadRecord.Rows.Count > 0)
			{
				dgv_LoadCurve.Invoke(new Action(() => dtLoadRecord.Rows.RemoveAt(0)));
			}
			//winMain._dlt645 = new DLT645(_strAddr);
			wm = (winMain)this.Owner;
			string strData = "", strErr = "";
			if (cb_LoadModulType.Text.ToString() == "给定时间记录块")
			{
				lstrDataMark.Clear();
				lstrDataMark.Add("04000A02");
				//if (!winMain._dlt645.Pack(DLT645.ControlType.Read, lstrDataMark, "", 0, out baBytes, out strErr))
				//{
				//	MessageBox.Show("组包失败!");
				//	return;
				//}

				//strErr = wm.FunMessage(baBytes, DLT645.ControlType.Read, lstrDataMark, out strData);
				//strErr = wm.FunDealMessage("", DLT645.ControlType.Read, lstrDataMark, out strData);
                if (wm.bBLE)
                {
				    var stra = wm.asyncFunDealMessage(strData, DLT645.ControlType.ReadLoad, lstrDataMark);
                     try
                      {
                        // Can't use ConfigureAwait here.
                        
	                    await HandleClickAsync();
                      }
                      finally
                      {
                        // We are back on the original context for this method.
	                    //button1.Enabled = true;
					    strData = stra.Result.Split(';')[1];
                      }
                }
                else
                {
                    strErr = wm.FunDealMessage("", DLT645.ControlType.Read, lstrDataMark, out strData);
                }
                if (strData == "")
				{
					MessageBox.Show("返回数据为空!");
					return;
				}
				else
				{
					iPeriod = Int32.Parse(Helps.fnAgainst(strData));
				}
			}
			lstrDataMark.Clear();
			lstrDataMark.Add(dataMark);
			//for (int m = 0; m < iCircle; m++) 
			int m = 0;
			dtLoadRecord = new DataTable();

			while (iCountAll > 0)
			{
				if (iCountAll <= 99)
				{
					iCount = iCountAll;
				}
				else
				{
					iCount = 99;
				}
				string timeStr = dtTimeRead.ToString("yyMMddHHmm");
				if (cb_LoadModulType.SelectedIndex == 0 || cb_LoadModulType.SelectedIndex == 2)
				{
					timeStr = string.Empty;
				}
				strData = iCount.ToString("D2") + Helps.fnAgainst(timeStr);
				//if (!winMain._dlt645.Pack(DLT645.ControlType.ReadLoad, lstrDataMark, strData, 0, out baBytes, out strErr))
				//{
				//	MessageBox.Show("组包失败!");
				//	return;
				//}

				//strErr = wm.FunMessage(baBytes, DLT645.ControlType.ReadLoad, lstrDataMark, out strData);
				//strErr = wm.FunDealMessage(strData, DLT645.ControlType.ReadLoad, lstrDataMark, out strData);
				if (wm.bBLE)
                {
				    var stra = wm.asyncFunDealMessage(strData, DLT645.ControlType.ReadLoad, lstrDataMark);
                    try
                    {
                        // Can't use ConfigureAwait here.
                        await HandleClickAsync();
                    }
                    finally
                    {
                        // We are back on the original context for this method.
	                    //button1.Enabled = true;
                        if (stra.Result.Contains(";"))
                        {
                            strData = stra.Result.Split(';')[1];
                        }
                        else
                        {
                            strErr = stra.Result;
                        }
                    }
                }
                else
                {
                    strErr = wm.FunDealMessage(strData, DLT645.ControlType.ReadLoad, lstrDataMark, out strData);
                }
				if (strErr == "")
				{
					if (strData == "")
					{
						MessageBox.Show("返回数据为空!");
						return;
					}
					else
					{
						//if ( (dataMark.Substring(0, 4) == "0610") || (dataMark.Substring(0, 4) == "06A0") )//连续抄读
						{
							DataTable dtSource = null;
							if ((dataMark.Substring(6, 2) == "FF"))
							{
								dtSource = winMain.DataBank.Find(x => x.TableName == cb_LoadCurveType.Text.ToString());
								if (dtSource == null)
								{
									MessageBox.Show("无'" + cb_LoadCurveType.Text.ToString() + "'sheet");
									return;
								}
							}
							else
							{
								DataRow[] drs = dtLoad.Select(string.Format("DataMark = '{0}' ", dataMark));

								if (drs.Length != 0)
								{
									dtSource = dtLoad.Copy();
									dtSource.Rows.Clear();
									DataRow dr = dtSource.NewRow();
									for (int i = 0; i < drs[0].ItemArray.Length; i++)
									{
										dr[i] = drs[0][i];
									}
									dtSource.Rows.Add(dr);
								}
								else
								{
									MessageBox.Show("Data中负荷记录无<" + dataMark + ">数据标识");
									return;
								}
							}
							iPartLen = 0;
							if (m == 0)
							{
								dtLoadRecord.Columns.Add(new DataColumn("序号"));
								dtLoadRecord.Columns.Add(new DataColumn("曲线时标"));
							}
							for (int i = 0; i < dtSource.Rows.Count; i++)
							{
								if (m == 0)
								{
									dtLoadRecord.Columns.Add(new DataColumn(dtSource.Rows[i]["Name"].ToString()));
								}
								
								iLenData = int.Parse(dtSource.Rows[i]["DataLen"].ToString());
								iPartLen += iLenData;
							}
							dtTimeRead = DateTime.ParseExact(Helps.fnAgainst(strData.Substring(0, 10)), "yyMMddHHmm", System.Globalization.CultureInfo.CurrentCulture);

							strData = strData.Substring(10);
							for (int i = 0; i < iCount; i++)
							{
								if (i > 0)
								{
									dtTimeRead = dtTimeRead.AddMinutes(iPeriod);
								}
								DataRow drData = dtLoadRecord.NewRow();
								drData["序号"] = (i + 1) + m * 99;


								string strTime = dtTimeRead.ToString("yy-MM-dd HH:mm");
								drData["曲线时标"] = strTime;
								if (strData != "")
								{
									string strPart = strData.Substring(0, iPartLen * 2);
									for (int j = 0; j < dtSource.Rows.Count; j++)
									{
										if (!int.TryParse(dtSource.Rows[j]["DecimalPlaces"].ToString(), out int decimalPlaces))
										{
											decimalPlaces = 0;
										}
										iLenData = int.Parse(dtSource.Rows[j]["DataLen"].ToString());
										string strTemp = Helps.fnAgainst(strPart.Substring(0, iLenData * 2));
										if (!strTemp.Contains("FFFF"))
										{
											string strSymble = "";
											if ((dtSource.Rows[j]["Name"].ToString().Contains("功率因数")) || (dtSource.Rows[j]["DataUnit"].ToString() == "A") || (dtSource.Rows[j]["DataUnit"].ToString() == "kW") || (dtSource.Rows[j]["DataUnit"].ToString() == "Kvar") || (dtSource.Rows[j]["DataUnit"].ToString() == "kVA"))
											{
												int iTe = 0;

												if ((Convert.ToInt64("0x" + strTemp.Substring(0, 1), 16) & 0x08) == 8)
												{
													strSymble = "-";
												}
												iTe = (int)(Convert.ToInt64("0x" + strTemp.Substring(0, 1), 16) & 0x07);
												strTemp = strSymble + iTe.ToString() + strTemp.Substring(1);
											}
											 
											if (dtSource.Rows[j]["DataFormat"].ToString() == ("HEX"))
                                            {
												Int64 i64Data = Int64.Parse(strTemp, System.Globalization.NumberStyles.HexNumber);
												strTemp = i64Data.ToString("D8");
                                            }
											if (decimalPlaces != 0)
											{
												strTemp = strTemp.Insert(strTemp.Length - decimalPlaces, ".");
											}
										}

										drData[j + 2] = strTemp;
										strPart = strPart.Substring(iLenData * 2);
									}
									dtLoadRecord.Rows.Add(drData);
									strData = strData.Substring(iPartLen * 2);
								}
							}
							dtTimeRead = dtTimeRead.AddMinutes(iPeriod);
						}
						//else
						//{

						//}
					}
				}
				else
				{
					MessageBox.Show(strErr);
				}
				iCountAll -= 99;
				m++;
			}

			dgv_LoadCurve.DataSource = dtLoadRecord;
			dgv_LoadCurve.Refresh();
		}
	}
}
