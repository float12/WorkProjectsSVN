using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using LeafSoft.Lib;
using System.Threading;
using LeafSoft.PartPanel;
using WindowsTest;

/*---------------作者：Maximus Ye----------------------*/
/*---------------时间：2013年8月16日---------------*/
/*---------------邮箱：yq@yyzq.net---------*/
/*---------------QQ：275623749---------*/
/*本软件也耗费了我不少的时间和精力，希望各位同行们尊重个人劳动成果，
 * 如果在此版本的基础上修改发布新的版本，请包含原作者信息（包括代码和UI界面相关信息)，为中国的
 * 开源事业做出一点贡献。*/
namespace LeafSoft.Units
{
    public partial class DataSend : UserControl
    {
        BindingList<Model.CMD> lstCMD = new BindingList<Model.CMD>();

        public event LeafEvent.DataSendHandler EventDataSend;

        /// <summary>
        /// 是否在自动循环发送状态
        /// </summary>
        bool AutoSend = false;
        bool Upgrade = false;
		private int iPartSize = 1344, iRepeatNum = 3;
		private List<byte[]> lbaHexBytes = new List<byte[]>();
		ushort usCalcuCRC16 = 0;
		
		List<string> lstrDataMark = new List<string>();
		byte[] baSendBytes;
		string strErr = "";

		#region Transshipment
		public static SerialTool stTransshipment = new SerialTool();
		bool TransshipmentMoniter = false;
		Thread ThTestL = null;
		#endregion

        public DataSend()
        {
            InitializeComponent();
            dgCMD.AutoGenerateColumns = false;
            lstCMD.Add(new Model.CMD(EnumType.DataEncode.ASCII, new ASCIIEncoding().GetBytes("Test!")));
            dgCMD.DataSource = lstCMD;
        }

        private void dgCMD_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {
            if (e.ColumnIndex == 3 && e.RowIndex>=0)
            {//点击了发送按钮 
                if (EventDataSend != null)
                {
                    if (EventDataSend(lstCMD[e.RowIndex].Bytes) == false)
                    {
                        StopAutoSend();
                    }
                    else
                    {
                        lblCount.Invoke(new MethodInvoker(delegate
                        {
                            lblCount.Text = (int.Parse(lblCount.Text) + lstCMD[e.RowIndex].Bytes.Length).ToString();
                        }));
                    }
                }
            }
        }

        private void dgCMD_RowsAdded(object sender, DataGridViewRowsAddedEventArgs e)
        {
            dgCMD.Rows[e.RowIndex].Cells[3].Value = "发送";
        }

        /// <summary>
        /// 删除调试命令
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MS_Delete_Click(object sender, EventArgs e)
        {
            if (dgCMD.SelectedRows.Count > 0)
            {
                lstCMD.RemoveAt(dgCMD.SelectedRows[0].Index);
            }
        }

        /// <summary>
        /// 自动发送
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnAutoSend_Click(object sender, EventArgs e)
        {
            if (AutoSend == false)
            {
                btnAutoSend.Text = "停止循环";
                dgCMD.Enabled = false;
                nmDelay.Enabled = false;
                AutoSend = true;
                Thread ThTestL = new Thread(new ParameterizedThreadStart(TAutoSend));
                ThTestL.IsBackground = true;
                ThTestL.Start(nmDelay.Value);
            }
            else
            {
                StopAutoSend();
            }
        }

        /// <summary>
        /// 自动发送命令线程
        /// </summary>
        private void TAutoSend(object Interval)
        {
            try
            {
                object sendlock = new object();
                int SendInterval = Convert.ToInt32(Interval);
                while (AutoSend)
                {
                    for (int i = 0; i < lstCMD.Count; i++)
                    {
                        if (AutoSend)
                        {
                            this.Invoke(new MethodInvoker(delegate
                            {
                                object cbxValue = dgCMD.Rows[i].Cells[0].Value;
                                if (cbxValue is bool && cbxValue.Equals(true))
                                {
                                    if (EventDataSend != null)
                                    {
                                        if (EventDataSend(lstCMD[i].Bytes) == false)
                                        {
                                            StopAutoSend();
                                        }
                                        else
                                        {
                                            lblCount.Invoke(new MethodInvoker(delegate
                                            {
                                                lblCount.Text = (int.Parse(lblCount.Text) + lstCMD[i].Bytes.Length).ToString();
                                            }));
                                        }
                                    }
                                }
                            }));
                            Thread.Sleep(SendInterval);
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }
            catch { };
        }

        /// <summary>
        /// 停止循环发送
        /// </summary>
        private void StopAutoSend()
        {
            AutoSend = false;
            btnAutoSend.Text = "循环发送";
            dgCMD.Enabled = true;
            nmDelay.Enabled = true;
        }        
		
		/// <summary>
        /// 停止循环发送
        /// </summary>
        private void StopUpgrade()
        {
            Upgrade = false;
            btn_升级.Text = "开始升级";
        }

        private void lblCount_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            lblCount.Text = "0";
        }

		private void nmDelay_ValueChanged(object sender, EventArgs e)
		{

		}

		/// <summary>
		/// 获取电表版本6025
		/// </summary>
		private void GetMeterVersion()
		{
			int t = 0;
			byte[] TxBufGetVersion = null;//{ 0xfe, 0xfe, 0xfe, 0xfe, 0x68, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x68, 0x11, 0x08, 0xe, 0x12, 0x12, 0x37, 0xe, 0x3b, 0x12, 0xe, 0xeb, 0x16 };
			
			string strDataSend = "68" + Helps.fnAgainst("000000000001") + "6811080E1212370E3B120E" ;
			Helps.TryStrToHexByte(strDataSend, out TxBufGetVersion);

			#region 校验和+16
			byte sum = Helps.Ssum(TxBufGetVersion);
			byte[] endBytes = new[] { sum, (byte)0x16 };
			#endregion

			TxBufGetVersion = TxBufGetVersion.Concat(endBytes).ToArray();
			byte[] FVersion = new byte[2];
			try
			{
				//sendOK(TxBufGetVersion, 0xfe);
			}
			catch (Exception exx)
			{
				MessageBox.Show(exx.Message);
				return;
			}
		}

		private void bPass_Click(object sender, EventArgs e)
		{
			//string strTemp = "68 31 00 43 05 63 26 13 50 03 00 00 00 6C 09 07 00 F2 01 02 01 03 02 08 01 00 00 1E 00 64 10 68 45 56 64 45 56 64 68 11 04 34 33 39 38 BB 16 00 52 4A 16";
			string strTemp = "68 31 00 43 45 AA AA AA AA AA AA 00 A6 58 09 07 00 F2 01 02 01 03 02 08 01 00 00 1E 00 64 10 68 45 56 64 45 56 64 68 11 04 34 33 39 38 BB 16 00 52 4A 16";
			byte [] sendBytes = new byte[0];
			byte [] recBytes = new byte[0];
			Helps.TryStrToHexByte(strTemp, out sendBytes);
                if (EventDataSend != null)
                {
                    if (EventDataSend(sendBytes) == false)
                    {
                        StopAutoSend();
                    }
                    else
                    {
                        lblCount.Invoke(new MethodInvoker(delegate
                        {
                            lblCount.Text = (int.Parse(lblCount.Text) + sendBytes.Length).ToString();
                        }));
                    }
                }
		}

		private void bVersion_Click(object sender, EventArgs e)
		{
			byte[] TxBufGetVersion ={ 0xfe, 0xfe, 0xfe, 0xfe, 0x68, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x68, 0x11, 0x08, 0xe, 0x12, 0x12, 0x37, 0xe, 0x3b, 0x12, 0xe, 0xeb, 0x16 };
			//TCPServerPanel.RecBuf = null;
            if (EventDataSend != null)
            {
                if (EventDataSend(TxBufGetVersion) == false)
                {
                    StopAutoSend();
                }
                else
                {
                    lblCount.Invoke(new MethodInvoker(delegate
                    {
                        lblCount.Text = (int.Parse(lblCount.Text) + TxBufGetVersion.Length).ToString();
                    }));
                }
            }
			
			int iCount = 0;
			while (TCPServerPanel.RecBuf == null)
			{
				iCount++;
				if (iCount > 8000 / 21)
				{
					break;
				}
				Thread.Sleep(20);
			}

			if (TCPServerPanel.RecBuf != null)
			{
				sendOK(TCPServerPanel.RecBuf);
			}
			else
			{
					MessageBox.Show("无返回报文!");
					return ;
			}
		}

		#region 升级线程
		/// <summary>
		/// 线程
		/// </summary>
		private void FuncUpdata(object oType)
		{
			string strPartData = "";
			int iCount = 0;
			int i = 0;

			#region 2-升级请求
			//output("2-升级请求", false, false);
			if (iPartSize == 192)
			{
				strPartData = Helps.fnAgainst(Helps.fnAgainst((((lbaHexBytes.Count - 1) * iPartSize + lbaHexBytes[lbaHexBytes.Count - 1].Length).ToString("X8"))) + Helps.fnAgainst(lbaHexBytes.Count.ToString("X4")) + Helps.fnAgainst((usCalcuCRC16.ToString("X4"))));
			}

			lstrDataMark.Clear();
			if (oType == "F460")
			{
				lstrDataMark.Add("0F0F0F01");
			}
			
			if (!winMain.dlt645TCP.Pack(DLT645.ControlType.Updata, lstrDataMark, strPartData, 0, out baSendBytes, out strErr, "02", "000000"))
			{

			}
			//TCPServerPanel.RecBuf = null;
            this.Invoke(new MethodInvoker(delegate
            {
                if (EventDataSend != null)
                {
                    if (EventDataSend(baSendBytes) == false)
                    {
                        StopUpgrade();
                    }
                    else
                    {
                        lblCount.Invoke(new MethodInvoker(delegate
                        {
                            lblCount.Text = (int.Parse(lblCount.Text) + baSendBytes.Length).ToString();
                        }));
                    }
                }

				iCount = 0;
				while (TCPServerPanel.RecBuf == null)
				{
					iCount++;
					if (iCount > 5000 / 21)
					{
						break;
					}
					Thread.Sleep(20);
				}

				if (TCPServerPanel.RecBuf != null)
				{
					sendOK(TCPServerPanel.RecBuf);
				}
				else
				{
						MessageBox.Show("无返回报文!");
						StopUpgrade();
						return ;
				}
            }));
			//	//FnShowpbProgressHandler(0, oType);
			#endregion

			if (oType == "F460")
			{
				if (btn_升级.Text.ToString() != "取消升级")
				{
					//FnShowpbProgressHandler(0, oType);
					return;
				}
			}

			#region 3-传输文件
			//output("3-传输文件", false, false);

			#region 3.1执行文件传输
			for (int m = 0; m < lbaHexBytes.Count; m++)
			{
				if (oType == "F460")
				{
					if (btn_升级.Text.ToString() != "取消升级")
					{
						//FnShowpbProgressHandler(0, oType);
						return;
					}
				}

				string updataStr = "";

				foreach (byte variable in lbaHexBytes[m])
				{
					updataStr += variable.ToString("X2");
				}
				ushort usCalCRC16Part = WSDCRC16.FileCalCRC16(0xFFFF, lbaHexBytes[m], (ushort)lbaHexBytes[m].Length);
				strPartData = usCalCRC16Part.ToString("X4") + Helps.fnAgainst(updataStr) + (m.ToString("X4"));
				lstrDataMark.Clear();
				if (oType == "F460")
				{
					lstrDataMark.Add("0F0F0F02");
				}

				if (!winMain.dlt645TCP.Pack(DLT645.ControlType.Updata, lstrDataMark, strPartData, 0, out baSendBytes, out strErr, "02", "000000"))
				{

				}
				for ( i = 0; i < iRepeatNum; i++)
				{
					//TCPServerPanel.RecBuf = null;
		            this.Invoke(new MethodInvoker(delegate
		            {
	                    if (EventDataSend != null)
						{
	                        if (EventDataSend(baSendBytes) == false)
	                        {
	                            StopUpgrade();
	                        }
	                        else
	                        {
	                            lblCount.Invoke(new MethodInvoker(delegate
	                            {
	                                lblCount.Text = (int.Parse(lblCount.Text) + baSendBytes.Length).ToString();
	                            }));
	                        }
	                    }
		            }));

					iCount = 0;
					while (TCPServerPanel.RecBuf == null)
					{
						iCount++;
						if (iCount > 5000 / 21)
						{
							break;
						}
						Thread.Sleep(20);
					}

					if (TCPServerPanel.RecBuf != null)
					{
						sendOK(TCPServerPanel.RecBuf);
					}
					else
					{
						continue;
					}
					//FnShowpbProgressHandler((m + 1), oType);
					break;
				}
				this.Invoke(new MethodInvoker(delegate
				{
					if (i >= iRepeatNum)
					{
						MessageBox.Show("重试次数3次,取消升级!");
						StopUpgrade();
						//FnShowpbProgressHandler(0, oType);
						return;
					}
				}));
			}
			#endregion

			#endregion

			#region 4-启动升级
			//output("4-启动升级", false, false);
			lstrDataMark.Clear();

			if (oType == "F460")
			{
				lstrDataMark.Add("0F0F0F03");
			}

			if (!winMain.dlt645TCP.Pack(DLT645.ControlType.Updata, lstrDataMark, "55", 0, out baSendBytes, out strErr, "02", "000000"))
			{

			}

			//TCPServerPanel.RecBuf = null;
            this.Invoke(new MethodInvoker(delegate
            {
                if (EventDataSend != null)
                {
                    if (EventDataSend(baSendBytes) == false)
                    {
                        StopUpgrade();
                    }
                    else
                    {
                        lblCount.Invoke(new MethodInvoker(delegate
                        {
                            lblCount.Text = (int.Parse(lblCount.Text) + baSendBytes.Length).ToString();
                        }));
                    }
                }
				 iCount = 0;
				while (TCPServerPanel.RecBuf == null)
				{
					iCount++;
					if (iCount > 5000 / 21)
					{
						break;
					}
					Thread.Sleep(20);
				}

				if (TCPServerPanel.RecBuf != null)
				{
					sendOK(TCPServerPanel.RecBuf);
				}
				else
				{
						MessageBox.Show("无返回报文!");
						StopUpgrade();
						return ;
				}
            }));
			#endregion

			//{
			//	#region 5-读取升级后的校验码
			//	//output("5-读取升级后的校验码");
			//	#region 1读取版本
			//	string strVersion = "";
			//	//output("延时60S后,读校验码", false, true);
			//	Thread.Sleep(60000);
			//	//if (oType == "F460")
			//	//{
			//	//	//bVersion_Click(null, null);
			//	//	//if (FunReadVersionF460(out strVersion) != "")
			//	//	//{
			//	//	//	MessageBox.Show("读版本失败!");
			//	//	//	return;
			//	//	//}
			//	//}
			//	MessageBox.Show("升级完成,请读取版本,检查升级是否成功!");
			//	//MessageBox.Show("升级后校验码:" + strVersion + "\n 升级成功!");
			//	#endregion

			//	#endregion
			MessageBox.Show("升级固件下发完成,结束升级!");
            this.Invoke(new MethodInvoker(delegate
            {
				StopUpgrade();
			}));
			//	//FnShowpbProgressHandler(0, oType);
			//}
		}
		#endregion

		private void btn_升级_Click(object sender, EventArgs e)
		{
			pbProgressF460.Value = 0;
			bool bVersionSupport = false;
			if (btn_升级.Text.ToString() == "取消升级")
			{
				btn_升级.Text = "开始升级";
				return;
			}
			lblCount.Text = "0";
			#region Tips
			if (tbFilePath.Text == "")
			{
				MessageBox.Show("请导入升级文件！");
				return;
			}
			#endregion
			byte[] baReceBuf = null;
			winMain.dlt645TCP = new DLT645("888888888888");
			#region 1读取版本
			//if (FunReadVersionF460(out string strVersionF460) != "")
			//{
			//	MessageBox.Show("读版本失败!");
			//	return;
			//}

			btn_升级.Text = "取消升级";
			#endregion

			#region 进行升级
			//FuncUpdata("F460");
			pbProgressF460.Maximum = lbaHexBytes.Count;
			pbProgressF460.Value = 0;
			Thread tThreadUpdata = new Thread(FuncUpdata) { IsBackground = true };
			tThreadUpdata.Start("F460");
			#endregion
		}

		private void btnInputF460_Click(object sender, EventArgs e)
		{
			pbProgressF460.Value = 0;
			btn_升级.Text = "开始升级";
			if (tbFilePath.Text == "")
			{
				openFileDialog1.InitialDirectory = Environment.CurrentDirectory;
			}
			else
			{
				openFileDialog1.InitialDirectory = openFileDialog1.FileName.Replace("\\" + openFileDialog1.SafeFileName, "");
			}

			openFileDialog1.Filter = "可导入文件类型|*.bin;*.BIN";

			openFileDialog1.FilterIndex = 0;
			openFileDialog1.RestoreDirectory = true;
			if (openFileDialog1.ShowDialog() != DialogResult.OK) return;
			tbFilePath.Text = openFileDialog1.FileName;
			//if (!(tbFilePath.Text.ToString().Contains("F460")))
			//{
			//	MessageBox.Show("导入的文件不包含F460,请检查固件是否正确!");
			//	tbFilePath.Text = "";
			//	return;
			//}

			FileStream fs = new FileStream(openFileDialog1.FileName, FileMode.Open, FileAccess.Read);
			BinaryReader read = new BinaryReader(fs);

			try
			{
				//if (cbMessageType.Text != "长报文")
				{
					iPartSize = 192;
				}

				int index = 0;
				lbaHexBytes.Clear();
				while (fs.Length > index)
				{
					int lenght = (int)fs.Length - index < iPartSize ? (int)(fs.Length - index) : iPartSize;
					byte[] buff = read.ReadBytes(lenght);
					if (buff.Length % 192 != 0)
					{
						List<byte> listData = new List<byte>();

						listData.AddRange(buff);
						for (int i = 0; i < 192 - (buff.Length % 192); i++)
						{
							listData.Add(0xFF);
						}
						buff = new byte[192];
						buff = listData.ToArray();
					}
					lbaHexBytes.Add(buff);
					index += lenght;
				}
				//释放Bin文件
				fs.Close();
				read.Close();

				usCalcuCRC16 = 0;
				ushort usCRC16Init = 0xFFFF;
				for (int i = 0; i < lbaHexBytes.Count; i++)
				{
					usCalcuCRC16 = WSDCRC16.FileCalCRC16(usCRC16Init, lbaHexBytes[i], (ushort)lbaHexBytes[i].Length);
					usCRC16Init = usCalcuCRC16;
				}
				llAllCout.Text = (lbaHexBytes.Count * 220 + 57).ToString();
				//Console.WriteLine(DataConvert.Bytes2HexString(lbaHexBytes[lbaHexBytes.Count - 1 ], 0, lbaHexBytes[lbaHexBytes.Count - 1 ].Length));
			}
			catch (Exception e1)
			{
				//output("导入升级程序有误：" + e1.Message, true, true);
			}
		}

		/// <summary>
		/// 返回68所在位置
		/// </summary>
		/// <param name="Rec"></param>
		/// <returns></returns>
		public int RecCutNum(byte[] Rec)
		{
			try
			{
				if (Rec == null || Rec.Length < 1) 
					return -1;
				else
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
				}
				return -1;
			}
			catch (Exception e)
			{
				return -1;
			}

		}

		#region Transshipment
		/// <summary>
		/// 自动线程
		/// </summary>
		private void tTransshipmentMoniter(object Interval)
		{
			try
			{
				while (TransshipmentMoniter)
				{
					if (TransshipmentMoniter )//&& SpMeter.Working
					{
						byte[] baReceBuf = stTransshipment.MoniterSerial();
						if (baReceBuf != null)
						{
							if (baReceBuf.Length > 0)
							{
								//TransshipmentMoniter = false;
					            if (EventDataSend != null)
					            {
					                if (EventDataSend(baReceBuf) == false)
					                {
					                    StopAutoSend();
					                }
									else
									{
										lblCount.Invoke(new MethodInvoker(delegate
										{
											lblCount.Text = (int.Parse(lblCount.Text) + baReceBuf.Length).ToString();
										}));
									}
								}
								
								int iCount = 0;
								List<byte> list = new List<byte>();
								//while (TCPServerPanel.RecBuf == null && (!( TCPServerPanel.RecBuf != null && TCPServerPanel.RecBuf[TCPServerPanel.RecBuf.Length - 1] != 0x16 )))
								while(true)
								{
									iCount++;
									if (iCount > 5000 / 90)
									{
										break;
									}
									Thread.Sleep(90);
									if (TCPServerPanel.RecBuf != null)
									{
										stTransshipment.sendWithNoAsk(TCPServerPanel.RecBuf, TCPServerPanel.RecBuf.Length);
										TCPServerPanel.RecBuf = null;
									}
									
									if (( TCPServerPanel.RecBuf != null && TCPServerPanel.RecBuf[TCPServerPanel.RecBuf.Length - 1] != 0x16 ))
									{
										break;
									}
								}
							}
						}
						//TransshipmentMoniter = true;
						Thread.Sleep(50);
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
		}

		/// <summary>
		/// 停止
		/// </summary>
		private void StopTransshipmentMoniter()
		{
			TransshipmentMoniter = false;
			//SpMeter.Working = false;
			//txcTransshipment.CheckState = CheckState.Unchecked;
		}

		private void txcTransshipment_CheckedChanged(object sender, EventArgs e)
		{
			if (!txcTransshipment.Checked)
			{
				stTransshipment.closePort();
				TransshipmentMoniter = false;
			}
			else
			{
				TransshipmentMoniter = true;
				lblCount.Text = "0";
				stTransshipment.setSerialPort(txcbTransshipment.Text.ToString(), 9600, "Even", 8, "1", 5000);
				ThTestL = new Thread(new ParameterizedThreadStart(tTransshipmentMoniter));
				ThTestL.IsBackground = true;
				ThTestL.Start();
			}
		}
		#endregion

		bool sendOK(byte[] sendBytes)
		{
			try
			{
			string strType = "";
			int index = RecCutNum(sendBytes);
			if (sendBytes[9 + index] == (sendBytes.Length - 12 - index) )
			{
				strType = "645";
			}
			else
			{
				strType = "698";
			}

			if (strType == "645")
			{
				if ((sendBytes[10 + index] == 0x0e) && (sendBytes[14 + index] == 0x0e)&& (sendBytes[15 + index] == 0x3b) && strType == "645")
				{
						if (sendBytes[8 + index] != 0x91)
						{
							MessageBox.Show("读取版本失败!" );
							//output("读取版本失败!", true, false);//Reading version error
							return false;
						}
						string 校验码Str = (((sendBytes[index + 30] - 0x33)) & 0xff).ToString("X2") + ((sendBytes[index + 31] - 0x33) & 0xff).ToString("X2");
						string VersionNum = (((sendBytes[index + 25] - 0x33)) & 0xff).ToString("X2") + ((sendBytes[index + 24] - 0x33) & 0xff).ToString("X2");
						MessageBox.Show("版本号：" + VersionNum + "；校验码：" + 校验码Str);
				}
				else if ((sendBytes[8 + index] == 0x9D))
				{
					
				}
				else if (sendBytes[8 + index] != 0xDD)
				{
					MessageBox.Show("升级失败!");
					return false;
				}
			}

				return true;
			}
			catch (Exception e)
			{
				return false;
			}
		}
	}
}
