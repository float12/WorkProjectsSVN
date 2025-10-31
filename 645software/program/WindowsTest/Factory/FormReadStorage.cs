using System;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.Windows.Forms;
using TX.Framework.WindowUI.Forms;

namespace WindowsTest
{
	public partial class FormReadStorage : MainForm
	{
		public FormReadStorage()
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
		private DataTable dtRN8302Table = null;
		List<string> lsList = new List<string>();

		public FormReadStorage(string strAddr)
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

		private void FormAdjustMeter_Load(object sender, EventArgs e)
		{

		}

		private void bReadFactory_Click(object sender, EventArgs e)
		{
			lstrDataMark.Clear();
            try
            {
				lstrDataMark.Add("DBDF0301" );
				lstrDataMark.Add("04DFDFDB");
				wm = (winMain)this.Owner;
                string  m_strText = "读取场内保留存储区";
                tbFactoryData.Text = "";
                string diStr = "0301";
                string inData = Helps.fnAgainst(tbFactorySaveAddr.Text) + Helps.fnAgainst(Convert.ToInt32(tbFactorySaveLen.Text).ToString("X2"));
				string strErr = wm.FunDealMessage(inData, DLT645.ControlType.ReadFactory, lstrDataMark, out string strData);

				if (strErr != "")
				{
					if (strErr != "请先打开串口!")
					{
						MessageBox.Show(m_strText + strErr);
					}
					return;
				}
				else
				{
					if (strData == "")
					{
						tbFactoryData.Text = "返回数据失败!";
						return;
					}
					else
					{
						tbFactoryData.Text = Helps.fnAgainst(strData);
					}
				}
            }
            catch
            {
            }
		}

		private void bReadStorage_Click(object sender, EventArgs e)
		{
			lstrDataMark.Clear();

			wm = (winMain)this.Owner;
            try
            {
                string m_strText = "读取";
                rtbData.Text = "";
                string diStr = "";

                switch (comboBox_RegData.SelectedIndex)
                {
                        #region 读取标识
                    case 0:
                        diStr = "0400";
                        m_strText += "管理芯RAM数据";
                        break;
                    case 1:
                        diStr = "0401";
                        m_strText += "管理芯FLASH数据";
                        break;
                    case 2:
                        diStr = "0402";
                        m_strText += "管理芯第一片EEPROM";
                        break;
                    case 3:
                        diStr = "0403";
                        m_strText += "管理芯第二片EEPROM";
                        break;
                    case 4:
                        diStr = "0404";
                        m_strText += "管理芯第三片EEPROM";
                        break;
                    case 5:
                        diStr = "0405";
                        m_strText += "管理芯铁电";
                        break;
                    case 6:
                        diStr += "0406";
                        m_strText += "管理芯连续空间";
                        break;
                    case 7:
                        diStr += "0500";
                        m_strText += "计量芯RAM数据";
                        break;
                    case 8:
                        diStr += "0501";
                        m_strText += "计量芯FLASH数据";
                        break;
                    case 9:
                        diStr += "0502";
                        m_strText += "计量芯第一片EEPROM";
                        break;
                    case 10:
                        diStr += "0503";
                        m_strText += "计量芯第二片EEPROM";
                        break;
                    case 11:
                        diStr += "0504";
                        m_strText += "计量芯第三片EEPROM（外部flash）";
                        break;
                    case 12:
                        diStr += "0505";
                        m_strText += "计量芯铁电";
                        break;
                    case 13:
                        diStr += "0506";
                        m_strText += "计量芯连续空间";
                        break;
                    default:
                        break;
                        #endregion
                }
				lstrDataMark.Add("DBDF" + diStr);
				lstrDataMark.Add("04DFDFDB");
                string inData = Helps.fnAgainst(textBox_RegAddr.Text.Trim().PadLeft(8, '0')) + Helps.fnAgainst(Convert.ToInt32(textBox_RegLen.Text.Trim()).ToString("X4"));

				string strErr = wm.FunDealMessage(inData, DLT645.ControlType.ReadFactory, lstrDataMark, out string strData);

				if (strErr != "")
				{
					if (strErr != "请先打开串口!")
					{
						MessageBox.Show(m_strText + strErr);
					}
					return;
				}
				else
				{
					if (strData == "")
					{
						rtbData.Text = "返回数据失败!";
						return;
					}
					else
					{
						//rtbData.Text = Helps.fnAgainst(strData);
						rtbData.Text = (strData);
					}
				}
            }
            catch
            {
            }
		}

		private void bSetFactory_Click(object sender, EventArgs e)
		{
			lstrDataMark.Clear();
            try
            {
				lstrDataMark.Add("DBDF0301" );
				lstrDataMark.Add("04DFDFDB");
				wm = (winMain)this.Owner;
                string  m_strText = "写入场内保留存储区";
                tbFactoryData.Text = "";
                string diStr = "0301";
				string inData = Helps.fnAgainst(tbFactorySaveAddr.Text) + Helps.fnAgainst(Convert.ToInt32(tbFactorySaveLen.Text).ToString("X2")) + Helps.fnAgainst(tbFactoryData.Text);

				string strErr = wm.FunDealMessage(inData, DLT645.ControlType.Write, lstrDataMark, out string strData);
				if (strErr != "")
				{
					if (strErr != "请先打开串口!")
					{
						MessageBox.Show(m_strText + strErr);
					}
					return;
				}
				else
				{
					if (strData == "")
					{
						tbFactoryData.Text = "返回数据失败!";
						return;
					}
					else
					{
						tbFactoryData.Text = Helps.fnAgainst(strData);
					}
				}
            }
            catch
            {
            }
		}
	}
}
