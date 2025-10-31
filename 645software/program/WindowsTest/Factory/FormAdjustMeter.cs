using System;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.Windows.Forms;
using TX.Framework.WindowUI.Forms;

namespace WindowsTest
{
	public partial class FormAdjustMeter : MainForm
	{
		public FormAdjustMeter()
		{
			InitializeComponent();
			this.DoubleBuffered = true;
			this.Location = new Point((SystemInformation.PrimaryMonitorSize.Width - this.Width) / 2, (SystemInformation.PrimaryMonitorSize.Height - this.Height) / 2);
		}
		private string _strAddr = "";
		public byte[] baBytes = null;
		List<string> lstrDataMark = new List<string>();
		winMain wm;
		private DataTable dtReaDataTable = null;
		List<string> lsList = new List<string>();

		public FormAdjustMeter(string strAddr)
		{
			InitializeComponent();
			_strAddr = strAddr;
			this.DoubleBuffered = true;
			this.Location = new Point((SystemInformation.PrimaryMonitorSize.Width - this.Width) / 2, (SystemInformation.PrimaryMonitorSize.Height - this.Height) / 2);
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
		/// 
		/// </summary>
		/// <returns></returns>
		private byte GetDataFromGroupBox55()
		{
			byte ret = 0;

			#region

			if (radioButton_xiaobiao_00.Checked)
			{
				ret = 0x00;
			}
			else if (radioButton_xiaobiao_01.Checked)
			{
				ret = 0x01;
			}
			else if (radioButton_xiaobiao_02.Checked)
			{
				ret = 0x02;
			}
			else if (radioButton_xiaobiao_03.Checked)
			{
				ret = 0x51;
			}
			else if (radioButton_xiaobiao_04.Checked)
			{
				ret = 0x52;
			}
			else if (radioButton_xiaobiao_05.Checked)
			{
				ret = 0x81;
			}
			else if (radioButton_xiaobiao_06.Checked)
			{
				ret = 0x82;
			}
			else if (radioButton_xiaobiao_07.Checked)
			{
				ret = 0xd1;
			}
			else if (radioButton_xiaobiao_08.Checked)
			{
				ret = 0xd2;
			}
			else if (radioButton_xiaobiao_09.Checked)
			{
				ret = 0xff;
			}
			else if (radioButton_xiaobiao_10.Checked)
			{
				ret = 0x08;
			}
			else if (radioButton_xiaobiao_11.Checked)
			{
				ret = 0x09;
			}
			#endregion

			return ret;
		}

		private void btnAdjustMeter_Click(object sender, EventArgs e)
		{
			string inData = "AA";
			inData += GetDataFromGroupBox55().ToString("X2");
			inData += "AA";
			inData += Helps.fnAgainst(textBox_xiaobiao_HF.Text.Trim());
			inData += "AA";
			inData += (Helps.GetHexStrReverse(textBox_xiaobiao_UA.Text, 4));
			inData += (Helps.GetHexStrReverse(textBox_xiaobiao_IA.Text, 4));
			inData += (Helps.GetHexStrReverse(textBox_xiaobiao_PA.Text, 4));
			if (!checkBox3.Checked)
			{
				inData += "AA";
				inData += (Helps.GetHexStrReverse(textBox_xiaobiao_UB.Text, 4));
				inData += (Helps.GetHexStrReverse(textBox_xiaobiao_IB.Text, 4));
				inData += (Helps.GetHexStrReverse(textBox_xiaobiao_PB.Text, 4));
				inData += "AA";
				inData += (Helps.GetHexStrReverse(textBox_xiaobiao_UC.Text, 4));
				inData += (Helps.GetHexStrReverse(textBox_xiaobiao_IC.Text, 4));
				inData += (Helps.GetHexStrReverse(textBox_xiaobiao_PC.Text, 4));

			}
			else
			{
				inData += "AAAA";
			}
			inData += "AA";

			//baBytes = DLT645.GetFactorySetMessage( "0602", inData);
			inData = Helps.fnAgainst(inData);
			lstrDataMark.Clear();
			if (cbCycleTwo.Checked)
			{
				lstrDataMark.Add("DBDF0604");
			}
			else
			{
				lstrDataMark.Add("DBDF0602");
			}
			
			lstrDataMark.Add("04DFDFDB");

			//winMain._dlt645 = new DLT645(_strAddr );
			//         if (!winMain._dlt645.Pack(DLT645.ControlType.Write, lstrDataMark, inData, 0, out baBytes, out string strErr ) )
			//         {
			//	MessageBox.Show("组包失败!");
			//	return;
			//         }
			//
			// strErr = wm.FunMessage(baBytes, DLT645.ControlType.Write, lstrDataMark, out string strData);
			wm = (winMain) this.Owner;
			string strErr = wm.FunDealMessage(inData, DLT645.ControlType.Write, lstrDataMark, out string strData);
			if (strErr != "")
			{
				if (strErr != "请先打开串口!")
				{
					MessageBox.Show(strErr);
				}
				return;
			}
			else
			{
				MessageBox.Show("设置成功!");
				return;
			}
		}

		private void FormAdjustMeter_Load(object sender, EventArgs e)
		{

		}

		private void button_RTCNorR_Click(object sender, EventArgs e)
		{
			textBox_RTCNor.Text = "";

			lstrDataMark.Clear();
			lstrDataMark.Add("DBDF0900");
			lstrDataMark.Add("04DFDFDB");
			string strErr = "";
			//winMain._dlt645 = new DLT645(_strAddr );
			//        if (!winMain._dlt645.Pack(DLT645.ControlType.Read, lstrDataMark, "", 0, out baBytes, out string strErr ) )
			//        {
			//MessageBox.Show("组包失败!");
			//return;
			//        }
			//strErr = wm.FunMessage(baBytes, DLT645.ControlType.Read, lstrDataMark, out string strData);
			wm = (winMain)this.Owner;
			strErr = wm.FunDealMessage("", DLT645.ControlType.Read, lstrDataMark, out string strData);
			if (strErr != "")
			{
				if (strErr != "请先打开串口!")
				{
					MessageBox.Show(strErr);
				}
				return;
			}
			else
			{
				if (strData == "")
				{
					MessageBox.Show("返回数据失败!");
					return;
				}
				else
				{
					textBox_RTCNor.Text = Helps.fnAgainst(strData);
				}
			}
		}

		private void button_RTCNorW_Click(object sender, EventArgs e)
		{
			wm = (winMain) this.Owner;
			if (textBox_RTCNor.Text == "")
			{
				MessageBox.Show("请先填写校正dataResultStr!");
				return;
			}
			else
			{
				string strRTCAdjust = textBox_RTCNor.Text.ToString();
				Helps.DealRTCData(ref strRTCAdjust);
				lstrDataMark.Clear();
				lstrDataMark.Add("DBDF0900");
				lstrDataMark.Add("04DFDFDB");
				//         winMain._dlt645 = new DLT645(_strAddr );
				//         if (!winMain._dlt645.Pack(DLT645.ControlType.Write, lstrDataMark, strRTCAdjust, 0, out baBytes, out string strErr ) )
				//         {
				//	MessageBox.Show("组包失败!");
				//	return;
				//         }
				//wm = (winMain) this.Owner;
				// strErr = wm.FunMessage(baBytes, DLT645.ControlType.Write, lstrDataMark, out string strData);
				string strErr = wm.FunDealMessage( strRTCAdjust, DLT645.ControlType.Write, lstrDataMark, out string strData);
				if (strErr != "")
				{
					if (strErr != "请先打开串口!")
					{
						MessageBox.Show(strErr);
					}
					return;
				}
				else
				{
					MessageBox.Show("设置成功!");
					return;
				}
			}
		}

		private void checkBox3_CheckedChanged(object sender, EventArgs e)
		{
			if (checkBox3.Checked)
			{
				textBox_xiaobiao_UB.Enabled = false;
				textBox_xiaobiao_UC.Enabled = false;
				textBox_xiaobiao_IB.Enabled = false;
				textBox_xiaobiao_IC.Enabled = false;
				textBox_xiaobiao_PB.Enabled = false;
				textBox_xiaobiao_PC.Enabled = false;

			}
			else
			{
				textBox_xiaobiao_UB.Enabled = true;
				textBox_xiaobiao_UC.Enabled = true;
				textBox_xiaobiao_IB.Enabled = true;
				textBox_xiaobiao_IC.Enabled = true;
				textBox_xiaobiao_PB.Enabled = true;
				textBox_xiaobiao_PC.Enabled = true;
			}
		}

		private void tbAdjustAnalogQuantity_Click(object sender, EventArgs e)
		{
			string inData = "AA";
			if (trbZero.Checked)
			{
				inData += "FF";
			}
			else if (trbGain.Checked)
			{
				inData += "51";
			}
			inData += "AA";
			inData += Helps.fnAgainst(textBox_xiaobiao_HF.Text.Trim());
			inData += "AA";
			inData += (Helps.GetHexStrReverse(textBox_xiaobiao_UA.Text, 4));
			inData += (Helps.GetHexStrReverse(textBox_xiaobiao_IA.Text, 4));
			inData += (Helps.GetHexStrReverse(textBox_xiaobiao_PA.Text, 4));
			//if (!checkBox3.Checked)
			{
				inData += "AA";
				inData += (Helps.GetHexStrReverse(textBox_xiaobiao_UB.Text, 4));
				inData += (Helps.GetHexStrReverse(textBox_xiaobiao_IB.Text, 4));
				inData += (Helps.GetHexStrReverse(textBox_xiaobiao_PB.Text, 4));
				inData += "AA";
				inData += (Helps.GetHexStrReverse(textBox_xiaobiao_UC.Text, 4));
				inData += (Helps.GetHexStrReverse(textBox_xiaobiao_IC.Text, 4));
				inData += (Helps.GetHexStrReverse(textBox_xiaobiao_PC.Text, 4));

			}
			//else
			//{
			//	inData += "AAAA";
			//}
			inData += "AA";

			inData = Helps.fnAgainst(inData);
			lstrDataMark.Clear();
			lstrDataMark.Add("DBDF0603");
			lstrDataMark.Add("04DFDFDB");

			wm = (winMain) this.Owner;
			string strErr = wm.FunDealMessage(inData, DLT645.ControlType.Write, lstrDataMark, out string strData);
			if (strErr != "")
			{
				if (strErr != "请先打开串口!")
				{
					MessageBox.Show(strErr);
				}
				return;
			}
			else
			{
				MessageBox.Show("设置成功!");
				return;
			}
		}

		/// <summary>
		/// 读取常用
		/// </summary>
		private void ReadCommonData(bool bCycle2 = false )
		{
				#region ReadData
				dgvCommon.AutoGenerateColumns = false;
				dtReaDataTable = new DataTable(null);
				dtReaDataTable.Columns.Add("选择", typeof(bool));
				dtReaDataTable.Columns.Add("Name", typeof(string));
				dtReaDataTable.Columns.Add("DataMark", typeof(string));
				dtReaDataTable.Columns.Add("DataMarkFather", typeof(string));
				dtReaDataTable.Columns.Add("DataLen", typeof(string));
				dtReaDataTable.Columns.Add("寄存器及长度", typeof(string));
				dtReaDataTable.Columns.Add("<1%", typeof(string));
				dtReaDataTable.Columns.Add("1%", typeof(string));
				dtReaDataTable.Columns.Add("2%", typeof(string));
				dtReaDataTable.Columns.Add("5%", typeof(string));
				dtReaDataTable.Columns.Add("10%", typeof(string));
				dtReaDataTable.Columns.Add("20%", typeof(string));
				dtReaDataTable.Columns.Add("50%", typeof(string));
				dtReaDataTable.Columns.Add("100%", typeof(string));
				dtReaDataTable.Columns.Add("200%", typeof(string));
				dtReaDataTable.Columns.Add("400%", typeof(string));
				dtReaDataTable.Columns.Add("操作结论", typeof(string));
				if (bCycle2 == false)
				{
					lsList = new List<string>();
					{
						lsList.Add("FALSE-A相增益系数-DBDF0601-04DFDFDB-20-00A11400---------");
						lsList.Add("FALSE-A相角差系数-DBDF0601-04DFDFDB-20-10A11400---------");
						lsList.Add("FALSE-B相增益系数-DBDF0601-04DFDFDB-20-01A11400---------");
						lsList.Add("FALSE-B相角差系数-DBDF0601-04DFDFDB-20-11A11400---------");
						lsList.Add("FALSE-C相增益系数-DBDF0601-04DFDFDB-20-02A11400---------");
						lsList.Add("FALSE-C相角差系数-DBDF0601-04DFDFDB-20-12A11400---------");
						lsList.Add("FALSE-A相无功系数-DBDF0601-04DFDFDB-9-20A10900---------");
						lsList.Add("FALSE-B相无功系数-DBDF0601-04DFDFDB-9-21A10900---------");
						lsList.Add("FALSE-C相无功系数-DBDF0601-04DFDFDB-9-22A10900---------");
						lsList.Add("FALSE-正向有功精度修正系数-DBDF0601-04DFDFDB-9-36A10900---------");
						lsList.Add("FALSE-反向有功精度修正系数-DBDF0601-04DFDFDB-9-30A10900---------");
					}
				}
				else
				{
					lsList = new List<string>();
					{
						lsList.Add("FALSE-A相增益系数-DBDF0603-04DFDFDB-20-00A11400---------");
						lsList.Add("FALSE-A相角差系数-DBDF0603-04DFDFDB-20-10A11400---------");
						lsList.Add("FALSE-B相增益系数-DBDF0603-04DFDFDB-20-01A11400---------");
						lsList.Add("FALSE-B相角差系数-DBDF0603-04DFDFDB-20-11A11400---------");
						lsList.Add("FALSE-C相增益系数-DBDF0603-04DFDFDB-20-02A11400---------");
						lsList.Add("FALSE-C相角差系数-DBDF0603-04DFDFDB-20-12A11400---------");
						lsList.Add("FALSE-A相无功系数-DBDF0603-04DFDFDB-9-20A10900---------");
						lsList.Add("FALSE-B相无功系数-DBDF0603-04DFDFDB-9-21A10900---------");
						lsList.Add("FALSE-C相无功系数-DBDF0603-04DFDFDB-9-22A10900---------");
						lsList.Add("FALSE-正向有功精度修正系数-DBDF0603-04DFDFDB-9-36A10900---------");
						lsList.Add("FALSE-反向有功精度修正系数-DBDF0603-04DFDFDB-9-30A10900---------");
					}
				}
				foreach (var VARIABLE in lsList)
				{
					object[] oRow = VARIABLE.Split('-');
					dtReaDataTable.Rows.Add(oRow);
				}

				dgvCommon.DataSource = dtReaDataTable;
				#endregion
		}

		private void txbReadDatas_Click(object sender, EventArgs e)
		{
			try
			{
				DataRow[] drs = dtReaDataTable.Select(string.Format("选择 = '{0}' ", "true"), "");
				int rowCount = drs.GetLength(0);
				if (rowCount < 1)
				{
					return;
				}

				#region 抄读
				bool isReadingErr = false;

				for (int i = 0; i < rowCount ; i++)
				{
					byte seq = 0;
					string dataResultStr = "";

					if (drs[i]["DataMark"] == null || drs[i]["DataMark"].ToString().Length != 8)
					{
						drs[i]["操作结论"] = "抄读失败：数据标识非法！";
						continue;
					}
					string inData = drs[i]["寄存器及长度"].ToString();
					lstrDataMark.Clear();
					lstrDataMark.Add(drs[i]["DataMark"].ToString() );
					lstrDataMark.Add(drs[i]["DataMarkFather"].ToString());

					wm = (winMain) this.Owner;
					string strErr = wm.FunDealMessage(inData, DLT645.ControlType.ReadFactory, lstrDataMark, out  dataResultStr);
					if (strErr != "")
					{
						if (strErr != "请先打开串口!")
						{
							MessageBox.Show(strErr);
						}
						return;
					}

					int dataLen;

					if (!int.TryParse(drs[i]["DataLen"].ToString(), out dataLen))
					{
						drs[i]["操作结论"] = "数据项长度非法，请排查！";
						continue;
					}
					dataResultStr = Helps.fnAgainst(dataResultStr);
					if (dataResultStr.Length != dataLen * 2)
					{
						drs[i]["操作结论"] = "失败：返回数据长度非法";
						continue;
					}
					else
					{
						if (dataResultStr.Length == 40)
						{
							for (int j = 0; j < 10; j++)
							{
								drs[i][j + 6] = dataResultStr.Substring(4 * j, 4);
							}
							drs[i]["操作结论"] = "读取成功!";
							//drs[i]["<1%"] = dataResultStr.Substring(0, 4);
							//drs[i]["1%"] = dataResultStr.Substring(4, 4);
							//drs[i]["2%"] = dataResultStr.Substring(8, 4);
							//drs[i]["5%"] = dataResultStr.Substring(12, 4);
							//drs[i]["10%"] = dataResultStr.Substring(16, 4);
							//drs[i]["20%"] = dataResultStr.Substring(20, 4);
							//drs[i]["50%"] = dataResultStr.Substring(24, 4);
							//drs[i]["100%"] = dataResultStr.Substring(28, 4);
							//drs[i]["200%"] = dataResultStr.Substring(32, 4);
							//drs[i]["400%"] = dataResultStr.Substring(36, 4);
						}
						else if (dataResultStr.Length == 18)
						{
							for (int j = 0; j < 9; j++)
							{
								drs[i][j + 7] = dataResultStr.Substring(2 * j, 2);
							}
							drs[i]["操作结论"] = "读取成功!";
							//drs[i]["1%"] = dataResultStr.Substring(0, 2);
							//drs[i]["2%"] = dataResultStr.Substring(2, 2);
							//drs[i]["5%"] = dataResultStr.Substring(4, 2);
							//drs[i]["10%"] = dataResultStr.Substring(6, 2);
							//drs[i]["20%"] = dataResultStr.Substring(8, 2);
							//drs[i]["50%"] = dataResultStr.Substring(10, 2);
							//drs[i]["100%"] = dataResultStr.Substring(12, 2);
							//drs[i]["200%"] = dataResultStr.Substring(14, 2);
							//drs[i]["400%"] = dataResultStr.Substring(16, 2);
						}
					}
				}
				#endregion
			}
			catch (Exception exception)
			{
				MessageBox.Show("抄读过程中出现错误：" + exception.Message);
			}
		}

		private void ttcAdjust_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (ttcAdjust.SelectedTab.Name ==  "tpAdjustRegister")
			{
				ReadCommonData();
			}
		}

		private void tbSet_Click(object sender, EventArgs e)
		{
			try
			{
				DataRow[] drs = dtReaDataTable.Select(string.Format("选择 = '{0}' ", "true"), "");
				int rowCount = drs.GetLength(0);
				if (rowCount < 1)
				{
					return;
				}

				#region 抄读
				bool isReadingErr = false;

				for (int i = 0; i < rowCount ; i++)
				{
					byte seq = 0;
					string dataResultStr = "";

					if (drs[i]["DataMark"] == null || drs[i]["DataMark"].ToString().Length != 8)
					{
						drs[i]["操作结论"] = "抄读失败：数据标识非法！";
						continue;
					}
					string inData = drs[i]["寄存器及长度"].ToString();
					lstrDataMark.Clear();
					lstrDataMark.Add(drs[i]["DataMark"].ToString() );
					lstrDataMark.Add(drs[i]["DataMarkFather"].ToString());
					if (!int.TryParse(drs[i]["DataLen"].ToString(), out int dataLen))
					{
						drs[i]["操作结论"] = "数据项长度非法，请排查！";
						continue;
					}
					string strDataTemp = "";
					if (dataLen == 20)
					{
						for (int j = 0; j < 10; j++)
						{
							strDataTemp += drs[i][j + 6].ToString();
						}
					}
					else if (dataLen == 9)
					{
						for (int j = 0; j < 9; j++)
						{
							strDataTemp += drs[i][j + 7].ToString();
						}
					}

					if (strDataTemp.Length != dataLen * 2)
					{
						drs[i]["操作结论"] = "数据长度非法";
						continue;
					}

					strDataTemp = Helps.fnAgainst(strDataTemp);
					inData += strDataTemp;
					inData = Helps.fnAgainst(inData);
					wm = (winMain) this.Owner;
					string strErr = wm.FunDealMessage(inData, DLT645.ControlType.Write, lstrDataMark, out  dataResultStr);
					if (strErr != "")
					{
						if (strErr != "请先打开串口!")
						{
							MessageBox.Show(strErr);
						}
						drs[i]["操作结论"] = "设置失败：" + strErr;
						return;
					}
					else
					{
						drs[i]["操作结论"] = "设置成功";
					}
				}
				#endregion
			}
			catch (Exception exception)
			{
				MessageBox.Show("设置过程中出现错误：" + exception.Message);
			}
		}

		private void 全选ToolStripMenuItem_Click(object sender, EventArgs e)
		{
			dgvCommon.EndEdit();
			for (int i = 0; i < dtReaDataTable.Rows.Count; i++)
			{
                 DataGridViewCheckBoxCell checkBox = (DataGridViewCheckBoxCell)this.dgvCommon.Rows[i].Cells[0];
				checkBox.Value = 1; //设置为1的时候全选
			}
		}

		private void 取消全选ToolStripMenuItem_Click(object sender, EventArgs e)
		{
			dgvCommon.EndEdit();
			for (int i = 0; i < dtReaDataTable.Rows.Count; i++)
			{
                 DataGridViewCheckBoxCell checkBox = (DataGridViewCheckBoxCell)this.dgvCommon.Rows[i].Cells[0];
				checkBox.Value = 0; //设置为0 取消全选
			}
		}

		private void cbCycle2_CheckedChanged(object sender, EventArgs e)
		{
			ReadCommonData(cbCycle2.Checked);
		}
	}
}
