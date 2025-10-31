using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using TX.Framework.WindowUI.Forms;

namespace WindowsTest
{
	public partial class FormControlClear : MainForm
	{
		public FormControlClear()
		{
			InitializeComponent();
			this.DoubleBuffered = true;
			this.Location = new Point((SystemInformation.PrimaryMonitorSize.Width - this.Width) / 2, (SystemInformation.PrimaryMonitorSize.Height - this.Height) / 2);
		}
		private string _strAddr = "";
		public byte[] baBytes = null;
		byte bCycle = 0x00;
		List<string> lstrDataMark = new List<string>();
		winMain wm;
		DateTime dtSeTime;
		public FormControlClear(string strAddr)
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
			string strTmp = DateTime.Now.ToString("yy-MM-dd") + " " + DateTime.Now.AddHours(1).ToString("HH:mm:ss");
			SetSysTime(strTmp);
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
				SetControlThreadSafe2(dtp_ControlTime, "Text", strDate);
				SetControlThreadSafe2(dtp_ClearControlTime, "Text", strDate);
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
			string strTmp = DateTime.Now.ToString("yy-MM-dd") + " " + DateTime.Now.AddHours(1).ToString("HH:mm:ss");
			SetSysTime(strTmp);
		}

		private void btn_Control_Click(object sender, EventArgs e)
		{
			dtSeTime = DateTime.ParseExact(dtp_ControlTime.Text, "yy-MM-dd HH:mm:ss", System.Globalization.CultureInfo.CurrentCulture);
			string strData = cmb_Control.Text.ToString() + "00" + Helps.fnAgainst(dtSeTime.ToString("yyMMddHHmmss"));

			//winMain._dlt645 = new DLT645(_strAddr);

			//if (!winMain._dlt645.Pack(DLT645.ControlType.Control, lstrDataMark, strData, 0, out baBytes, out string strErr))
			//{
			//	MessageBox.Show("组包失败!");
			//	return;
			//}
			//wm = (winMain)this.Owner;
			//strErr = wm.FunMessage(baBytes, DLT645.ControlType.Control, lstrDataMark, out strData);
			string strErr = wm.FunDealMessage( strData, DLT645.ControlType.Control, lstrDataMark, out strData);
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
				MessageBox.Show("操作成功!");
				return;
			}
		}

		/// <summary>
		///  控制操作
		/// </summary>
		/// <param name="erctCommand"></param>
		/// <param name="strRecoveryTime"></param>
		private void FunControl(eumRemoteControlType erctCommand, string strRecoveryTime = "00")
		{
			dtSeTime = DateTime.ParseExact(dtp_ControlTime.Text, "yy-MM-dd HH:mm:ss", System.Globalization.CultureInfo.CurrentCulture);

			string strData = (erctCommand | (eumRemoteControlType)bCycle).ToString("X") + strRecoveryTime + Helps.fnAgainst(dtSeTime.ToString("yyMMddHHmmss"));

			//winMain._dlt645 = new DLT645(_strAddr);

			//if (!winMain._dlt645.Pack(DLT645.ControlType.Control, lstrDataMark, strData, 0, out baBytes, out string strErr))
			//{
			//	MessageBox.Show("组包失败!");
			//	return;
			//}
			//wm = (winMain)this.Owner;
			//strErr = wm.FunMessage(baBytes, DLT645.ControlType.Control, lstrDataMark, out strData);
			string strErr = wm.FunDealMessage( strData, DLT645.ControlType.Control, lstrDataMark, out strData);
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
				MessageBox.Show("操作成功!");
				return;
			}
		}

		private void btnWindUp_Click(object sender, EventArgs e)
		{
			FunControl(eumRemoteControlType.eumWindUp);
		}

		private void btnAllowWindDown_Click(object sender, EventArgs e)
		{
			FunControl(eumRemoteControlType.eumAllowWindDown);
		}

		private void btnWindDown_Click(object sender, EventArgs e)
		{
			FunControl(eumRemoteControlType.eumWindDown);
		}

		private void btnReadyWindUpToAllow_Click(object sender, EventArgs e)
		{
			string strRcoveryTime = "00";
			if (cmb_StartWindUpTime.Text.ToString() != "跳闸自动恢复时间")
			{
				strRcoveryTime = (int.Parse(cmb_StartWindUpTime.Text) / 5).ToString("X2");
			}
			FunControl(eumRemoteControlType.eumReadyWindUpToAllow, strRcoveryTime);
		}

		private void btnReadyWindUpToDown_Click(object sender, EventArgs e)
		{
			string strRcoveryTime = "00";
			if (cmb_StartWindUpTime.Text.ToString() != "跳闸自动恢复时间")
			{
				strRcoveryTime = (int.Parse(cmb_StartWindUpTime.Text) / 5).ToString("X2");
			}
			FunControl(eumRemoteControlType.eumReadyWindUpToDown, strRcoveryTime);
		}

		private void btnAlarm_Click(object sender, EventArgs e)
		{
			FunControl(eumRemoteControlType.eumAlarm);
		}

		private void btnLiftAlarm_Click(object sender, EventArgs e)
		{
			FunControl(eumRemoteControlType.eumLiftAlarm);
		}

		private void btnPaulPower_Click(object sender, EventArgs e)
		{
			FunControl(eumRemoteControlType.eumPaulPower);
		}

		private void btnLiftPaulPower_Click(object sender, EventArgs e)
		{
			FunControl(eumRemoteControlType.eumLiftPaulPower);
		}

		private void btnMeterClear_Click(object sender, EventArgs e)
		{
			dtSeTime = DateTime.ParseExact(dtp_ClearControlTime.Text, "yy-MM-dd HH:mm:ss", System.Globalization.CultureInfo.CurrentCulture);

			string strData = "1A" + "00" + Helps.fnAgainst(dtSeTime.ToString("yyMMddHHmmss"));

			//winMain._dlt645 = new DLT645(_strAddr);

			//if (!winMain._dlt645.Pack(DLT645.ControlType.Clear, lstrDataMark, strData, 0, out baBytes, out string strErr))
			//{
			//	MessageBox.Show("组包失败!");
			//	return;
			//}
			//wm = (winMain)this.Owner;
			//strErr = wm.FunMessage(baBytes, DLT645.ControlType.Clear, lstrDataMark, out strData);
			string strErr = wm.FunDealMessage(strData, DLT645.ControlType.Clear, lstrDataMark, out strData);
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
				MessageBox.Show("操作成功!");
				return;
			}
		}

		private void btnDemandClear_Click(object sender, EventArgs e)
		{
			dtSeTime = DateTime.ParseExact(dtp_ClearControlTime.Text, "yy-MM-dd HH:mm:ss", System.Globalization.CultureInfo.CurrentCulture);

			string strData = "19" + "00" + Helps.fnAgainst(dtSeTime.ToString("yyMMddHHmmss"));

			//winMain._dlt645 = new DLT645(_strAddr);

			//if (!winMain._dlt645.Pack(DLT645.ControlType.DemandClear, lstrDataMark, strData, 0, out baBytes, out string strErr))
			//{
			//	MessageBox.Show("组包失败!");
			//	return;
			//}
			//wm = (winMain)this.Owner;
			//strErr = wm.FunMessage(baBytes, DLT645.ControlType.DemandClear, lstrDataMark, out strData);
			string strErr = wm.FunDealMessage(strData, DLT645.ControlType.DemandClear, lstrDataMark, out strData);
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
				MessageBox.Show("操作成功!");
				return;
			}
		}

		private void btnEventClear_Click(object sender, EventArgs e)
		{
			dtSeTime = DateTime.ParseExact(dtp_ClearControlTime.Text, "yy-MM-dd HH:mm:ss", System.Globalization.CultureInfo.CurrentCulture);

			string strData = "1B" + "00" + Helps.fnAgainst(dtSeTime.ToString("yyMMddHHmmss")) + Helps.fnAgainst(txt_ClearEvent.Text.ToString());

			//winMain._dlt645 = new DLT645(_strAddr);

			//if (!winMain._dlt645.Pack(DLT645.ControlType.EventClear, lstrDataMark, strData, 0, out baBytes, out string strErr))
			//{
			//	MessageBox.Show("组包失败!");
			//	return;
			//}
			//wm = (winMain)this.Owner;
			//strErr = wm.FunMessage(baBytes, DLT645.ControlType.EventClear, lstrDataMark, out strData);

			string strErr = wm.FunDealMessage(strData, DLT645.ControlType.EventClear, lstrDataMark, out strData);
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
				MessageBox.Show("操作成功!");
				return;
			}
		}

		private void btn_MultifunctionalTerminalOutput_Click(object sender, EventArgs e)
		{
			string strData = "";
			if (rb_MultifunctionalTerminalOutput_Second.Checked == true)
			{
				strData = "00";
			}
			else if (rb_MultifunctionalTerminalOutput_Demand.Checked == true)
			{
				strData = "01"; //需量周期
			}
			else
			{
				strData = "02"; //时段投切
			}

			//winMain._dlt645 = new DLT645(_strAddr);

			//if (!winMain._dlt645.Pack(DLT645.ControlType.MultifunctionTerminal, lstrDataMark, strData, 0, out baBytes, out string strErr))
			//{
			//	MessageBox.Show("组包失败!");
			//	return;
			//}
			//wm = (winMain)this.Owner;
			//strErr = wm.FunMessage(baBytes, DLT645.ControlType.MultifunctionTerminal, lstrDataMark, out strData);
			string strErr = wm.FunDealMessage(strData, DLT645.ControlType.MultifunctionTerminal, lstrDataMark, out strData);
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
				MessageBox.Show("操作成功!");
				return;
			}
		}

		private void cbCycle_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (cbCycle.Text == "第一回路")
			{
				bCycle = 0x00;
			}
			else if (cbCycle.Text == "第二回路")
			{
				bCycle = 0x40;
			}
			else if (cbCycle.Text == "第三回路")
			{
				bCycle = 0x80;
			}
			else if (cbCycle.Text == "三路同时")
			{
				bCycle = 0xC0;
			}
		}
	}
}
