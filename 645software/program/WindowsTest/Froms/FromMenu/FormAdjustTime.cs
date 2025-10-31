using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using TX.Framework.WindowUI.Forms;

namespace WindowsTest
{
	public partial class FormAdjustTime : MainForm
	{
		public FormAdjustTime()
		{
			InitializeComponent();
			this.DoubleBuffered = true;
			this.Location = new Point((SystemInformation.PrimaryMonitorSize.Width - this.Width) / 2, (SystemInformation.PrimaryMonitorSize.Height - this.Height) / 2);
		}
		private string _strAddr = "";
		public byte[] baBytes = null;
		List<string> lstrDataMark = new List<string>();
		winMain wm;
		public FormAdjustTime(string strAddr)
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
			wm = (winMain)this.Owner;
		}

		private void btn_ReadDate_Click(object sender, EventArgs e)
		{
			lstrDataMark.Clear();
			lstrDataMark.Add("04000101");
			//winMain._dlt645 = new DLT645(_strAddr );
			//         if (!winMain._dlt645.Pack(DLT645.ControlType.Read, lstrDataMark, "", 0, out baBytes, out string strErr ) )
			//         {
			//	MessageBox.Show("组包失败!");
			//	return;
			//         }

			//strErr = wm.FunMessage(baBytes, DLT645.ControlType.Read, lstrDataMark, out string strData);
			string strErr = wm.FunDealMessage( "", DLT645.ControlType.Read, lstrDataMark, out string strData);
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
					strData = DataConvert.fnFormatDateTime(Helps.fnAgainst(strData), "YYMMDDWW");
					txt_ReadMeterTime.Text = strData;
				}
			}
		}

		#region UI同步
		private delegate void SetControlThreadSafeDelegate2(DateTimePicker control, string propertyName, string message);
		public static void SetControlThreadSafe2(DateTimePicker control, string propertyName, string strMessage)
		{
			try
			{
				if (control.InvokeRequired)
					control.Invoke(new SetControlThreadSafeDelegate2(SetControlThreadSafe2), control, propertyName,
						strMessage);
				else
				{
					control.Text = strMessage;
				}
			}
			catch (System.Exception ex)
			{

			}
		}
		#endregion

		private object obj_SysTime = new object();

		/// <summary>
		/// 设置系统时间
		/// </summary>
		/// <param name="strDate"></param>
		public void SetSysTime(string strDate)
		{
			lock (obj_SysTime)
			{
				SetControlThreadSafe2(txt_SetMeterTime, "Text", strDate);
			}
		}

		private void rb_SysTime_CheckedChanged(object sender, EventArgs e)
		{
			timerSystem.Enabled = true;
		}

		private void rb_CustomTime_CheckedChanged(object sender, EventArgs e)
		{
			timerSystem.Enabled = false;
		}

		private void timerSystem_Tick(object sender, EventArgs e)
		{
			string strTmp = DateTime.Now.ToString("yy-MM-dd") + " " + DateTime.Now.ToString("HH:mm:ss");
			SetSysTime(strTmp);
		}

		private void btn_ReadTime_Click(object sender, EventArgs e)
		{
			lstrDataMark.Clear();
			lstrDataMark.Add("04000102");
			//winMain._dlt645 = new DLT645(_strAddr );
			//         if (!winMain._dlt645.Pack(DLT645.ControlType.Read, lstrDataMark, "", 0, out baBytes, out string strErr ) )
			//         {
			//	MessageBox.Show("组包失败!");
			//	return;
			//         }

			//strErr = wm.FunMessage(baBytes, DLT645.ControlType.Read, lstrDataMark, out string strData);
			string strErr = wm.FunDealMessage("", DLT645.ControlType.Read, lstrDataMark, out string strData);
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
					strData = DataConvert.fnFormatDateTime(Helps.fnAgainst(strData), "HHmmss");
					txt_ReadMeterTime.Text = strData;
				}
			}
		}

		private void btn_ReadMeterDateTime_Click(object sender, EventArgs e)
		{
			lstrDataMark.Clear();
			lstrDataMark.Add("0400010C");
			//winMain._dlt645 = new DLT645(_strAddr );
			//         if (!winMain._dlt645.Pack(DLT645.ControlType.Read, lstrDataMark, "", 0, out baBytes, out string strErr ) )
			//         {
			//	MessageBox.Show("组包失败!");
			//	return;
			//         }

			//strErr = wm.FunMessage(baBytes, DLT645.ControlType.Read, lstrDataMark, out string strData);
			string strErr = wm.FunDealMessage("", DLT645.ControlType.Read, lstrDataMark, out string strData);
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
					strData = DataConvert.fnFormatDateTime(Helps.fnAgainst(strData), "YYMMDDWWHHMMSS");
					txt_ReadMeterTime.Text = strData;
				}
			}
		}

		private void btn_SetMeterDate_Click(object sender, EventArgs e)
		{
			string strDateTime = txt_SetMeterTime.Value.ToString("yyMMdd") + ((int)txt_SetMeterTime.Value.DayOfWeek).ToString("X2");

			string strErr = "";
			lstrDataMark.Clear();
			lstrDataMark.Add("04000101");
			//winMain._dlt645 = new DLT645(_strAddr );
			//         if (!winMain._dlt645.Pack(DLT645.ControlType.Write, lstrDataMark, strDateTime, 0, out baBytes, out  strErr) )
			//         {
			//	MessageBox.Show("组包失败!");
			//	return;
			//         }

			//strErr = wm.FunMessage(baBytes, DLT645.ControlType.Write, lstrDataMark, out string strData);
			strErr = wm.FunDealMessage( strDateTime, DLT645.ControlType.Write, lstrDataMark, out string strData);
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
			//strDateTime += ((int)dtSet.DayOfWeek).ToString("00") + dtSet.ToString("HHmmss");
		}

		private void btn_RadioSetMeterTime_Click(object sender, EventArgs e)
		{
			string strDateTime = txt_SetMeterTime.Value.ToString("yyMMdd") + txt_SetMeterTime.Value.ToString("HHmmss");
			string strErr = "", strData = "";
			lstrDataMark.Clear();
			if (cb_TrueAddr.Checked)
			{
				//winMain._dlt645 = new DLT645(_strAddr );
				strErr = wm.FunDealMessage(strDateTime, DLT645.ControlType.Radio, lstrDataMark, out strData);
			}
			else
			{
				winMain._dlt645 = new DLT645("999999999999");
				if (!winMain._dlt645.Pack(DLT645.ControlType.Radio, lstrDataMark, strDateTime, 0, out baBytes, out strErr))
				{
					MessageBox.Show("组包失败!");
					return;
				}

				strErr = wm.FunMessage(baBytes, DLT645.ControlType.Radio, lstrDataMark, out strData);
			}

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

		private void btn_SetMeterTime_Click(object sender, EventArgs e)
		{
			string strDateTime = txt_SetMeterTime.Value.ToString("HHmmss");

			string strErr = "";
			lstrDataMark.Clear();
			lstrDataMark.Add("04000102");
			//winMain._dlt645 = new DLT645(_strAddr );
			//         if (!winMain._dlt645.Pack(DLT645.ControlType.Write, lstrDataMark, strDateTime, 0, out baBytes, out  strErr) )
			//         {
			//	MessageBox.Show("组包失败!");
			//	return;
			//         }

			//strErr = wm.FunMessage(baBytes, DLT645.ControlType.Write, lstrDataMark, out string strData);
			strErr = wm.FunDealMessage(strDateTime, DLT645.ControlType.Write, lstrDataMark, out string strData);
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

		private void btn_SetMeterDateTime_Click(object sender, EventArgs e)
		{
			string strDateTime = txt_SetMeterTime.Value.ToString("yyMMdd") + ((int)txt_SetMeterTime.Value.DayOfWeek).ToString("X2") + txt_SetMeterTime.Value.ToString("HHmmss");

			string strErr = "";
			lstrDataMark.Clear();
			lstrDataMark.Add("0400010C");
			//winMain._dlt645 = new DLT645(_strAddr );
			//         if (!winMain._dlt645.Pack(DLT645.ControlType.Write, lstrDataMark, strDateTime, 0, out baBytes, out  strErr) )
			//         {
			//	MessageBox.Show("组包失败!");
			//	return;
			//         }

			//strErr = wm.FunMessage(baBytes, DLT645.ControlType.Write, lstrDataMark, out string strData);
			strErr = wm.FunDealMessage(strDateTime, DLT645.ControlType.Write, lstrDataMark, out string strData);
			if (strErr != "")
			{
				if (strErr != "请先打开串口!")
				{
					MessageBox.Show(strErr);
					//MessageBox.Show(strErr, string.Empty, MessageBoxButtons.OK, MessageBoxIcon.None, MessageBoxDefaultButton.Button1, MessageBoxOptions.ServiceNotification, false);
				}
				return;
			}
			else
			{
				MessageBox.Show("设置成功!");
				return;
			}
		}

		private void txt_SetMeterTime_ValueChanged(object sender, EventArgs e)
		{

		}

		private void label10_Click(object sender, EventArgs e)
		{

		}
	}
}
