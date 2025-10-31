using System;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.Windows.Forms;
using TX.Framework.WindowUI.Forms;

namespace WindowsTest
{
	public partial class FormChipRegisters : MainForm
	{
		public FormChipRegisters()
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

		public FormChipRegisters(string strAddr)
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
			CommonData();
			CommonData8302();
		}

		private void btnReadRegister_Click(object sender, EventArgs e)
		{
			//try
			//{
			//	foreach (Control ctrl in gbRegisterThree.Controls)
			//	{
			//		if (ctrl is CheckBox)
			//		{
			//			CheckBox _chk = (CheckBox)ctrl;
			//			if (_chk.Checked)
			//			{

			//			}
			//		}
			//	}
			//}
			//catch (Exception exception)
			//{
			//	Console.WriteLine(exception);
			//	throw;
			//}
			try
			{
				foreach (Control ctrl in gbRegisterThree.Controls)
				{
					if (ctrl is TextBox)
					{
						(ctrl as TextBox).Text = "";
					}
				}
				string diStr = "0601";
				List<string> inData = new List<string>();
				List<string> _strText = new List<string>();
				List<TextBox> tb_已选 = new List<TextBox>();
				if (ckb_A有功增益.Checked)
				{
					inData.Add("04200200");
					_strText.Add("A相有功功率增益");
					tb_已选.Add(tb_A有功功率);
				}
				if (ckb_B有功功率.Checked)
				{
					inData.Add("05200200");
					_strText.Add("B相有功功率增益");
					tb_已选.Add(tb_B有功功率);
				}
				if (ckb_C有功功率.Checked)
				{
					inData.Add("06200200");
					_strText.Add("C相有功功率增益");
					tb_已选.Add(tb_C有功功率);
				}
				if (ckb_A无功功率.Checked)
				{
					inData.Add("07200200");
					_strText.Add("A相无功功率增益");
					tb_已选.Add(tb_A无功功率);
				}
				if (ckb_B无功功率.Checked)
				{
					inData.Add("08200200");
					_strText.Add("B相无功功率增益");
					tb_已选.Add(tb_B无功功率);
				}
				if (ckb_C无功功率.Checked)
				{
					inData.Add("09200200");
					_strText.Add("C相无功功率增益");
					tb_已选.Add(tb_C无功功率);
				}
				if (ckb_A视在功率.Checked)
				{
					inData.Add("0A200200");
					_strText.Add("A相视在功率增益");
					tb_已选.Add(tb_A视在功率);
				}
				if (ckb_B视在功率.Checked)
				{
					inData.Add("0B200200");
					_strText.Add("B相视在功率增益");
					tb_已选.Add(tb_B视在功率);
				}
				if (ckb_C视在功率.Checked)
				{
					inData.Add("0C200200");
					_strText.Add("C相视在功率增益");
					tb_已选.Add(tb_C视在功率);
				}
				if (ckb_A相位0.Checked)
				{
					inData.Add("0D200200");
					_strText.Add("A相相位校正0");
					tb_已选.Add(tb_A相位0);
				}
				if (ckb_B相位0.Checked)
				{
					inData.Add("0E200200");
					_strText.Add("B相相位校正0");
					tb_已选.Add(tb_B相位0);
				}
				if (ckb_C相位0.Checked)
				{
					inData.Add("0F200200");
					_strText.Add("C相相位校正0");
					tb_已选.Add(tb_C相位0);
				}
				if (ckb_A相位1.Checked)
				{
					inData.Add("10200200");
					_strText.Add("A相位校正1");
					tb_已选.Add(tb_A相位1);
				}
				if (ckb_B相位1.Checked)
				{
					inData.Add("11200200");
					_strText.Add("B相位校正1");
					tb_已选.Add(tb_B相位1);
				}
				if (ckb_C相位1.Checked)
				{
                    inData.Add("12200200");
                    _strText.Add("C相位校正1");
                    tb_已选.Add(tb_C相位1);
                }
				if (ckb_A电压.Checked)
                {
                    inData.Add("17200200");
                    _strText.Add("A相电压增益");
                    tb_已选.Add(tb_A电压);
                }
                if (ckb_B电压.Checked)
                {
                    inData.Add("18200200");
                    _strText.Add("B相电压增益");
                    tb_已选.Add(tb_B电压);
                }
                if (ckb_C电压.Checked)
                {
                    inData.Add("19200200");
                    _strText.Add("C相电压增益");
                    tb_已选.Add(tb_C电压);
                }
                if (ckb_A电流.Checked)
                {
                    inData.Add("1A200200");
                    _strText.Add("A相电流增益");
                    tb_已选.Add(tb_A电流);
                }
                if (ckb_B电流.Checked)
                {
                    inData.Add("1B200200");
                    _strText.Add("B相电流增益");
                    tb_已选.Add(tb_B电流);
                }
                if (ckb_C电流.Checked)
                {
                    inData.Add("1C200200");
                    _strText.Add("C相电流增益");
                    tb_已选.Add(tb_C电流);
                }
                if (ckb_启动电流阈值设置.Checked)
                {
                    inData.Add("1D200200");
                    _strText.Add("启动电流阈值设置");
                    tb_已选.Add(tb_启动电流阈值设置);
                }
                if (ckb_高频脉冲输出设置.Checked)
                {
                    inData.Add("1E100200");
                    _strText.Add("高频脉冲输出设置");
                    tb_已选.Add(tb_高频脉冲输出设置);
                }
                if (ckb_脉冲加倍寄存器.Checked)
                {
                    inData.Add("33100200");
					_strText.Add("脉冲加倍寄存器");
					tb_已选.Add(tb_脉冲加倍寄存器);
				}
				if (ckb_相位补偿区域设置0.Checked)
				{
					inData.Add("37200200");
					_strText.Add("相位补偿区域设置0");
					tb_已选.Add(tb_相位补偿区域设置0);
				}
				if (ckb_相位补偿区域设置1.Checked)
				{
					inData.Add("60200200");
					_strText.Add("相位补偿区域设置1");
					tb_已选.Add(tb_相位补偿区域设置1);
				}
				if (ckb_Toffset校正寄存器.Checked)
				{
					inData.Add("6B200200");
					_strText.Add("Toffset校正寄存器");
					tb_已选.Add(tb_Toffset校正寄存器);
				}
				if (ckb_温度传感器数据寄存器.Checked)
				{
					inData.Add("2A400200");
					_strText.Add("温度传感器数据寄存器");
					tb_已选.Add(tb_温度传感器数据寄存器);
				}

				lstrDataMark.Clear();
				lstrDataMark.Add("DBDF0601");
				lstrDataMark.Add("04DFDFDB");
				wm = (winMain)this.Owner;
				for (int i = 0; i < inData.Count; i++)
				{
					//winMain._dlt645 = new DLT645(_strAddr );
					//if (!winMain._dlt645.Pack(DLT645.ControlType.ReadFactory, lstrDataMark, inData[i], 0, out baBytes, out string strErr ) )
					//         {
					//	MessageBox.Show(_strText[i] + "组包失败!");
					//	return;
					//         }

					// strErr = wm.FunMessage(baBytes, DLT645.ControlType.ReadFactory, lstrDataMark, out string strData);
					string strErr = wm.FunDealMessage(inData[i], DLT645.ControlType.ReadFactory, lstrDataMark, out string strData);

					if (strErr != "")
					{
						if (strErr != "请先打开串口!")
						{
							MessageBox.Show(_strText[i] + strErr);
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
							tb_已选[i].Text = Helps.fnAgainst(strData);
						}
					}
				}
			}
			catch
			{

			}
		}

		private void ckb_三相寄存器全选_CheckedChanged(object sender, EventArgs e)
		{
			if (ckb_三相寄存器全选.Checked)
			{
				foreach (Control ctrl in gbRegisterThree.Controls)
				{
					if (ctrl is CheckBox)
					{
						(ctrl as CheckBox).Checked = true;
					}
				}
			}
			else
			{
				foreach (Control ctrl in gbRegisterThree.Controls)
				{
					if (ctrl is CheckBox)
					{
						(ctrl as CheckBox).Checked = false;
					}
				}
			}
		}

		private void tbn_读7017_Click(object sender, EventArgs e)
		{
			try
			{
				foreach (Control ctrl in gbRegisterSingle.Controls)
				{
					if (ctrl is TextBox)
					{
						(ctrl as TextBox).Text = "";
					}
				}
				List<string> inData = new List<string>();
				List<string> _strText = new List<string>();
				List<TextBox> tb_已选 = new List<TextBox>();
				if (ckb_7017_有功功率校正.Checked)
				{
					inData.Add("50200200");
					_strText.Add("有功功率校正");
					tb_已选.Add(tb_7017_有功功率校正);
				}
				if (ckb_7017_无功功率校正.Checked)
				{
					inData.Add("51200200");
					_strText.Add(" L线无功功率校正");
					tb_已选.Add(tb_7017_无功功率校正);
				}
				if (ckb_7017_视在功率校正.Checked)
				{
					inData.Add("52200200");
					_strText.Add("L线视在功率校正");
					tb_已选.Add(tb_7017_视在功率校正);
				}
				if (ckb_7017_ADC通道增益选择.Checked)
				{
					inData.Add("59200200");
					_strText.Add("ADC通道增益选择");
					tb_已选.Add(tb_7017_ADC通道增益选择);
				}
				if (ckb_7017_起动功率寄存器.Checked)
				{
					inData.Add("5F200200");
					_strText.Add("起动功率寄存器");
					tb_已选.Add(tb_7017_起动功率寄存器);
				}
				if (ckb_7017_输出脉冲频率.Checked)
				{
					inData.Add("61200200");
					_strText.Add("输出脉冲频率");
					tb_已选.Add(tb_7017_输出脉冲频率);
				}
				if (ckb_7017_有功功率偏置校正.Checked)
				{
					inData.Add("65200200");
					_strText.Add("L线有功功率偏置校正");
					tb_已选.Add(tb_7017_有功功率偏置校正);
				}
				if (ckb_7017_L线有效值补偿.Checked)
				{
					inData.Add("69200200");
					_strText.Add("L线有效值补偿");
					tb_已选.Add(tb_7017_L线有效值补偿);
				}
				if (ckb_7017_N线有效值补偿.Checked)
				{
					inData.Add("6A200200");
					_strText.Add("N线有效值补偿");
					tb_已选.Add(tb_7017_N线有效值补偿);
				}
				if (ckb_7017_相位校正.Checked)
				{
					inData.Add("6D200200");
					_strText.Add("L线相位校正 ");
					tb_已选.Add(tb_7017_相位校正);
				}
				if (ckb_7017_L线有功功率偏置校正低字节.Checked)
				{
					inData.Add("76200200");
					_strText.Add("L线有功功率偏置校正低字节");
					tb_已选.Add(tb_7017_L线有功功率偏置校正低字节);
				}

				lstrDataMark.Clear();
				lstrDataMark.Add("DBDF0601");
				lstrDataMark.Add("04DFDFDB");
				wm = (winMain)this.Owner;
				for (int i = 0; i < inData.Count; i++)
				{
					//winMain._dlt645 = new DLT645(_strAddr );
					//if (!winMain._dlt645.Pack(DLT645.ControlType.ReadFactory, lstrDataMark, inData[i], 0, out baBytes, out string strErr ) )
					//         {
					//	MessageBox.Show(_strText[i] + "组包失败!");
					//	return;
					//         }
					// strErr = wm.FunMessage(baBytes, DLT645.ControlType.ReadFactory, lstrDataMark, out string strData);
					string strErr = wm.FunDealMessage( inData[i], DLT645.ControlType.ReadFactory, lstrDataMark, out string strData);
					if (strErr != "")
					{
						if (strErr != "请先打开串口!")
						{
							MessageBox.Show(_strText[i] + strErr);
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
							tb_已选[i].Text = Helps.fnAgainst(strData);
						}
					}
				}
			}
			catch
			{
			}
		}

		private void btnSetRegister_Click(object sender, EventArgs e)
		{
			try
			{
				List<string> inData = new List<string>();
				List<string> _strText = new List<string>();
				List<TextBox> tb_已选 = new List<TextBox>();
				if (ckb_A有功增益.Checked)
				{
					inData.Add("04200200");
					_strText.Add("A相有功功率增益");
					tb_已选.Add(tb_A有功功率);
				}
				if (ckb_B有功功率.Checked)
				{
					inData.Add("05200200");
					_strText.Add("B相有功功率增益");
					tb_已选.Add(tb_B有功功率);
				}
				if (ckb_C有功功率.Checked)
				{
					inData.Add("06200200");
					_strText.Add("C相有功功率增益");
					tb_已选.Add(tb_C有功功率);
				}
				if (ckb_A无功功率.Checked)
				{
					inData.Add("07200200");
					_strText.Add("A相无功功率增益");
					tb_已选.Add(tb_A无功功率);
				}
				if (ckb_B无功功率.Checked)
				{
					inData.Add("08200200");
					_strText.Add("B相无功功率增益");
					tb_已选.Add(tb_B无功功率);
				}
				if (ckb_C无功功率.Checked)
				{
					inData.Add("09200200");
					_strText.Add("C相无功功率增益");
					tb_已选.Add(tb_C无功功率);
				}
				if (ckb_A视在功率.Checked)
				{
					inData.Add("0A200200");
					_strText.Add("A相视在功率增益");
					tb_已选.Add(tb_A视在功率);
				}
				if (ckb_B视在功率.Checked)
				{
					inData.Add("0B200200");
					_strText.Add("B相视在功率增益");
					tb_已选.Add(tb_B视在功率);
				}
				if (ckb_C视在功率.Checked)
				{
					inData.Add("0C200200");
					_strText.Add("C相视在功率增益");
					tb_已选.Add(tb_C视在功率);
				}
				if (ckb_A相位0.Checked)
				{
					inData.Add("0D200200");
					_strText.Add("A相相位校正0");
					tb_已选.Add(tb_A相位0);
				}
				if (ckb_B相位0.Checked)
				{
					inData.Add("0E200200");
					_strText.Add("B相相位校正0");
					tb_已选.Add(tb_B相位0);
				}
				if (ckb_C相位0.Checked)
				{
					inData.Add("0F200200");
					_strText.Add("C相相位校正0");
					tb_已选.Add(tb_C相位0);
				}
				if (ckb_A相位1.Checked)
				{
					inData.Add("10200200");
					_strText.Add("A相位校正1");
					tb_已选.Add(tb_A相位1);
				}
				if (ckb_B相位1.Checked)
				{
					inData.Add("11200200");
					_strText.Add("B相位校正1");
					tb_已选.Add(tb_B相位1);
				}
				if (ckb_C相位1.Checked)
				{
					inData.Add("12200200");
					_strText.Add("C相位校正1");
					tb_已选.Add(tb_C相位1);
				}
				if (ckb_A电压.Checked)
				{
					inData.Add("17200200");
					_strText.Add("A相电压增益");
					tb_已选.Add(tb_A电压);
				}
				if (ckb_B电压.Checked)
				{
					inData.Add("18200200");
					_strText.Add("B相电压增益");
					tb_已选.Add(tb_B电压);
				}
				if (ckb_C电压.Checked)
				{
					inData.Add("19200200");
					_strText.Add("C相电压增益");
					tb_已选.Add(tb_C电压);
				}
				if (ckb_A电流.Checked)
				{
					inData.Add("1A200200");
					_strText.Add("A相电流增益");
					tb_已选.Add(tb_A电流);
				}
				if (ckb_B电流.Checked)
				{
					inData.Add("1B200200");
					_strText.Add("B相电流增益");
					tb_已选.Add(tb_B电流);
				}
				if (ckb_C电流.Checked)
				{
					inData.Add("1C200200");
					_strText.Add("C相电流增益");
					tb_已选.Add(tb_C电流);
				}
				if (ckb_启动电流阈值设置.Checked)
				{
					inData.Add("1D200200");
					_strText.Add("启动电流阈值设置");
					tb_已选.Add(tb_启动电流阈值设置);
				}
				if (ckb_高频脉冲输出设置.Checked)
				{
					inData.Add("1E100200");
					_strText.Add("高频脉冲输出设置");
					tb_已选.Add(tb_高频脉冲输出设置);
				}
				if (ckb_脉冲加倍寄存器.Checked)
				{
					inData.Add("33100200");
					_strText.Add("脉冲加倍寄存器");
					tb_已选.Add(tb_脉冲加倍寄存器);
				}
				if (ckb_相位补偿区域设置0.Checked)
				{
					inData.Add("37200200");
					_strText.Add("补偿区域设置0");
					tb_已选.Add(tb_相位补偿区域设置0);
				}
				if (ckb_相位补偿区域设置1.Checked)
				{
					inData.Add("60200200");
					_strText.Add("补偿区域设置1");
					tb_已选.Add(tb_相位补偿区域设置1);
				}
				if (ckb_Toffset校正寄存器.Checked)
				{
					inData.Add("6B200200");
					_strText.Add("Toffset校正寄存器");
					tb_已选.Add(tb_Toffset校正寄存器);
				}
				if (ckb_温度传感器数据寄存器.Checked)
				{
					inData.Add("2A400200");
					_strText.Add("温度传感器数据寄存器");
					tb_已选.Add(tb_温度传感器数据寄存器);
				}

				lstrDataMark.Clear();
				lstrDataMark.Add("DBDF0601");
				lstrDataMark.Add("04DFDFDB");
				wm = (winMain)this.Owner;
				for (int i = 0; i < inData.Count; i++)
				{
					//winMain._dlt645 = new DLT645(_strAddr );
					//if (!winMain._dlt645.Pack(DLT645.ControlType.Write, lstrDataMark, Helps.fnAgainst(inData[i] + Helps.fnAgainst(tb_已选[i].Text.ToString() ) ), 0, out baBytes, out string strErr ) )
					//         {
					//	MessageBox.Show(_strText[i] + "组包失败!");
					//	return;
					//         }
					//strErr = wm.FunMessage(baBytes, DLT645.ControlType.Write, lstrDataMark, out string strData);

					string strErr = wm.FunDealMessage( Helps.fnAgainst(inData[i] + Helps.fnAgainst(tb_已选[i].Text.ToString())), DLT645.ControlType.Write, lstrDataMark, out string strData);
					if (strErr != "")
					{
						if (strErr != "请先打开串口!")
						{
							MessageBox.Show(_strText[i] + strErr);
						}
						return;
					}
				}
				MessageBox.Show("设置成功!");
				return;
			}
			catch
			{

			}
		}

		private void btn_设7017_Click(object sender, EventArgs e)
		{
			try
			{
				List<string> inData = new List<string>();
				List<string> _strText = new List<string>();
				List<TextBox> tb_已选 = new List<TextBox>();
				if (ckb_7017_有功功率校正.Checked)
				{
					inData.Add("50200200");
					_strText.Add("有功功率校正");
					tb_已选.Add(tb_7017_有功功率校正);
				}
				if (ckb_7017_无功功率校正.Checked)
				{
					inData.Add("51200200");
					_strText.Add(" L线无功功率校正");
					tb_已选.Add(tb_7017_无功功率校正);
				}
				if (ckb_7017_视在功率校正.Checked)
				{
					inData.Add("52200200");
					_strText.Add("L线视在功率校正");
					tb_已选.Add(tb_7017_视在功率校正);
				}
				if (ckb_7017_ADC通道增益选择.Checked)
				{
					inData.Add("59200200");
					_strText.Add("ADC通道增益选择");
					tb_已选.Add(tb_7017_ADC通道增益选择);
				}
				if (ckb_7017_起动功率寄存器.Checked)
				{
					inData.Add("5F200200");
					_strText.Add("起动功率寄存器");
					tb_已选.Add(tb_7017_起动功率寄存器);
				}
				if (ckb_7017_输出脉冲频率.Checked)
				{
					inData.Add("61200200");
					_strText.Add("输出脉冲频率");
					tb_已选.Add(tb_7017_输出脉冲频率);
				}
				if (ckb_7017_有功功率偏置校正.Checked)
				{
					inData.Add("65200200");
					_strText.Add("L线有功功率偏置校正");
					tb_已选.Add(tb_7017_有功功率偏置校正);
				}
				if (ckb_7017_L线有效值补偿.Checked)
				{
					inData.Add("69200200");
					_strText.Add("L线有效值补偿");
					tb_已选.Add(tb_7017_L线有效值补偿);
				}
				if (ckb_7017_N线有效值补偿.Checked)
				{
					inData.Add("6A200200");
					_strText.Add("N线有效值补偿");
					tb_已选.Add(tb_7017_N线有效值补偿);
				}
				if (ckb_7017_相位校正.Checked)
				{
					inData.Add("6D200200");
					_strText.Add("L线相位校正 ");
					tb_已选.Add(tb_7017_相位校正);
				}
				if (ckb_7017_L线有功功率偏置校正低字节.Checked)
				{
					inData.Add("76200200");
					_strText.Add("L线有功功率偏置校正低字节");
					tb_已选.Add(tb_7017_L线有功功率偏置校正低字节);
				}

				lstrDataMark.Clear();
				lstrDataMark.Add("DBDF0601");
				lstrDataMark.Add("04DFDFDB");
				wm = (winMain)this.Owner;
				for (int i = 0; i < inData.Count; i++)
				{
					//               winMain._dlt645 = new DLT645(_strAddr );
					//if (!winMain._dlt645.Pack(DLT645.ControlType.Write, lstrDataMark, Helps.fnAgainst(inData[i] + Helps.fnAgainst(tb_已选[i].Text.ToString() ) ), 0, out baBytes, out string strErr ) )
					//         {
					//	MessageBox.Show(_strText[i] + "组包失败!");
					//	return;
					//         }

					//strErr = wm.FunMessage(baBytes, DLT645.ControlType.Write, lstrDataMark, out string strData);
					string strErr = wm.FunDealMessage( Helps.fnAgainst(inData[i] + Helps.fnAgainst(tb_已选[i].Text.ToString())), DLT645.ControlType.Write, lstrDataMark, out string strData);
					if (strErr != "")
					{
						if (strErr != "请先打开串口!")
						{
							MessageBox.Show(_strText[i] + strErr);
						}
						return;
					}
				}
				MessageBox.Show("设置成功!");
				return;
			}
			catch
			{
			}
		}

		private void ckb_7017_全选_CheckedChanged(object sender, EventArgs e)
		{
			if (ckb_7017_全选.Checked)
			{
				foreach (Control ctrl in gbRegisterSingle.Controls)
				{
					if (ctrl is CheckBox)
					{
						(ctrl as CheckBox).Checked = true;
					}
				}
			}
			else
			{
				foreach (Control ctrl in gbRegisterSingle.Controls)
				{
					if (ctrl is CheckBox)
					{
						(ctrl as CheckBox).Checked = false;
					}
				}
			}
		}

		private void button_xiaobiaoRead_Click(object sender, EventArgs e)
		{
			try
			{
				string strDataRegisters = Helps.fnAgainst(textBox_xiaobiaoAddr.Text.PadLeft(4, '0') );
                strDataRegisters += Helps.fnAgainst(Convert.ToInt32(textBox_xiaobiaoLen.Text.Trim()).ToString("X4") );
				lstrDataMark.Clear();
				lstrDataMark.Add("DBDF0601");
				lstrDataMark.Add("04DFDFDB");
				wm = (winMain)this.Owner;
				string strErr = wm.FunDealMessage(strDataRegisters, DLT645.ControlType.ReadFactory, lstrDataMark, out string strData);

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
						textBox_xiaobiaoData.Text = Helps.fnAgainst(strData);
					}
				}
			}
			catch (Exception exception)
			{
				//Console.WriteLine(exception);
				//throw;
			}
		}

		private void button_xiaobiaoSet_Click(object sender, EventArgs e)
		{
				lstrDataMark.Clear();
				lstrDataMark.Add("DBDF0601");
				lstrDataMark.Add("04DFDFDB");
				wm = (winMain)this.Owner;
				int iDataLen = Int32.Parse(textBox_xiaobiaoLen.Text.ToString() );
                string inData = Helps.fnAgainst(textBox_xiaobiaoAddr.Text.PadLeft(4, '0') ) + Helps.fnAgainst(Convert.ToInt32(textBox_xiaobiaoLen.Text).ToString("X4") ) + Helps.fnAgainst(textBox_xiaobiaoData.Text.PadLeft(iDataLen, '0') );
				string strErr = wm.FunDealMessage( Helps.fnAgainst(inData), DLT645.ControlType.Write, lstrDataMark, out string strData);
				if (strErr != "")
				{
					if (strErr != "请先打开串口!")
					{
						MessageBox.Show(strErr);
					}
					return;
				}

				MessageBox.Show("设置成功!");
				return;
		}

		/// <summary>
		/// 读取常用
		/// </summary>
		private void CommonData()
		{
				#region ReadData
				dgvCommon.AutoGenerateColumns = false;
				dtReaDataTable = new DataTable(null);
				dtReaDataTable.Columns.Add("选择", typeof(bool));
				//dtReaDataTable.Columns.Add("ID", typeof(string));
				//dtReaDataTable.Columns.Add("ParentID", typeof(string));
				dtReaDataTable.Columns.Add("Name", typeof(string));
				//dtReaDataTable.Columns.Add("DataMark", typeof(string));
				//dtReaDataTable.Columns.Add("DataMarkFather", typeof(string));
				dtReaDataTable.Columns.Add("寄存器地址", typeof(string));
				dtReaDataTable.Columns.Add("DataLen", typeof(string));
				//dtReaDataTable.Columns.Add("DataBlockType", typeof(string));
				//dtReaDataTable.Columns.Add("DataFormat", typeof(string));
				//dtReaDataTable.Columns.Add("DecimalPlaces", typeof(string));
				//dtReaDataTable.Columns.Add("DefaultValue", typeof(string));
				//dtReaDataTable.Columns.Add("DataUnit", typeof(string));
				dtReaDataTable.Columns.Add("数据值", typeof(string));
				//dtReaDataTable.Columns.Add("设置值", typeof(string));
				dtReaDataTable.Columns.Add("操作结论", typeof(string));
				lsList = new List<string>();
				if (tcbTypeRN.Text.ToString() == "增益类寄存器")
				{
					lsList.Add("TRUE-A相有功功率增益寄存器-A0-4");
					lsList.Add("TRUE-B相有功功率增益寄存器-A4-4");
					lsList.Add("TRUE-C相有功功率增益寄存器-A8-4");
					lsList.Add("TRUE-A相无功功率增益寄存器-AC-4");
					lsList.Add("TRUE-B相无功功率增益寄存器-B0-4");
					lsList.Add("TRUE-C相无功功率增益寄存器-B4-4");
					lsList.Add("TRUE-A相视在功率增益寄存器-B8-4");
					lsList.Add("TRUE-B相视在功率增益寄存器-BC-4");
					lsList.Add("TRUE-C相视在功率增益寄存器-C0-4");
					lsList.Add("TRUE-UA通道增益寄存器-4C-4");
					lsList.Add("TRUE-UB通道增益寄存器-50-4");
					lsList.Add("TRUE-UC通道增益寄存器-54-4");
					lsList.Add("TRUE-IA通道增益寄存器-58-4");
					lsList.Add("TRUE-IB通道增益寄存器-5C-4");
					lsList.Add("TRUE-IC通道增益寄存器-60-4");
					lsList.Add("TRUE-A有功功率相位校正寄存器-C4-4");
					lsList.Add("TRUE-B有功功率相位校正寄存器-C8-4");
					lsList.Add("TRUE-C有功功率相位校正寄存器-CC-4");
					lsList.Add("TRUE-IA通道相位校正寄存器-3C-4");
					lsList.Add("TRUE-IB通道相位校正寄存器-40-4");
					lsList.Add("TRUE-IC通道相位校正寄存器-44-4");
					lsList.Add("TRUE-A有功功率OFFSET校正寄存器-DC-4");
					lsList.Add("TRUE-B有功功率OFFSET校正寄存器-E0-4");
					lsList.Add("TRUE-C有功功率OFFSET校正寄存器-E4-4");
					lsList.Add("TRUE-IA有效值OFFSET校正寄存器-90-4");
					lsList.Add("TRUE-IB有效值OFFSET校正寄存器-94-4");
					lsList.Add("TRUE-IC有效值OFFSET校正寄存器-98-4");
				}
				else
				{
					lsList.Add("TRUE-CF脉冲配置寄存器-180-4");
					lsList.Add("TRUE-计量单元配置寄存器-188-4");
					lsList.Add("TRUE-计量控制寄存器-18C-4");
					lsList.Add("TRUE-过零计算配置及标志寄存器-1BC-4");
					lsList.Add("TRUE-直流OFFSET自动校正使能寄存器-1B8-4");
					//lsList.Add("TRUE-ADC控制寄存器-1000-4");
					lsList.Add("TRUE-高频脉冲常数寄存器-00-4");
					lsList.Add("TRUE-有功功率启动阈值寄存器-08-4");
					lsList.Add("TRUE-三相四线/三相三线模式选择寄存器-17C-4");
					lsList.Add("TRUE-相位补偿区域设置寄存器-18-4");
					lsList.Add("TRUE-相位补偿区域设置寄存器-1C-4");
					lsList.Add("TRUE-相位补偿区域设置寄存器-20-4");
					lsList.Add("TRUE-相位补偿区域设置寄存器-24-4");
					lsList.Add("TRUE-电压过零阈值寄存器-14-4");
					lsList.Add("TRUE-电压过零阈值寄存器-1B4-4");
				}

				foreach (var VARIABLE in lsList)
				{
					object[] oRow = VARIABLE.Split('-');
					dtReaDataTable.Rows.Add(oRow);
				}

				dgvCommon.DataSource = dtReaDataTable;
				#endregion
		}
		
		/// <summary>
		/// 读取常用
		/// </summary>
		private void CommonData8302()
		{
				#region ReadData
				dgvRN8302.AutoGenerateColumns = false;
				dtRN8302Table = new DataTable(null);
				dtRN8302Table.Columns.Add("选择", typeof(bool));
				dtRN8302Table.Columns.Add("Name", typeof(string));
				dtRN8302Table.Columns.Add("寄存器地址", typeof(string));
				dtRN8302Table.Columns.Add("DataLen", typeof(string));
				dtRN8302Table.Columns.Add("数据值", typeof(string));
				dtRN8302Table.Columns.Add("操作结论", typeof(string));
				lsList = new List<string>();
				if (tcbTypeRN.Text.ToString() == "增益类寄存器")
				{
					lsList.Add("TRUE-A相有功功率增益寄存器-128-2");
					lsList.Add("TRUE-B相有功功率增益寄存器-129-2");
					lsList.Add("TRUE-C相有功功率增益寄存器-12A-2");
					lsList.Add("TRUE-A相无功功率增益寄存器-12B-2");
					lsList.Add("TRUE-B相无功功率增益寄存器-12C-2");
					lsList.Add("TRUE-C相无功功率增益寄存器-12D-2");
					lsList.Add("TRUE-A相视在功率增益寄存器-12E-2");
					lsList.Add("TRUE-B相视在功率增益寄存器-12F-2");
					lsList.Add("TRUE-C相视在功率增益寄存器-130-2");
					lsList.Add("TRUE-UA通道增益寄存器-113-2");
					lsList.Add("TRUE-UB通道增益寄存器-114-2");
					lsList.Add("TRUE-UC通道增益寄存器-115-2");
					lsList.Add("TRUE-IA通道增益寄存器-116-2");
					lsList.Add("TRUE-IB通道增益寄存器-117-2");
					lsList.Add("TRUE-IC通道增益寄存器-118-2");
					lsList.Add("TRUE-A有功功率相位校正寄存器-131-2");
					lsList.Add("TRUE-B有功功率相位校正寄存器-132-2");
					lsList.Add("TRUE-C有功功率相位校正寄存器-133-2");
					lsList.Add("TRUE-IA通道相位校正寄存器-10F-4");
					lsList.Add("TRUE-IB通道相位校正寄存器-110-4");
					lsList.Add("TRUE-IC通道相位校正寄存器-111-4");
					lsList.Add("TRUE-A有功功率OFFSET校正寄存器-152-2");
					lsList.Add("TRUE-B有功功率OFFSET校正寄存器-153-2");
					lsList.Add("TRUE-C有功功率OFFSET校正寄存器-154-2");
					lsList.Add("TRUE-IA有效值OFFSET校正寄存器-124-2");
					lsList.Add("TRUE-IB有效值OFFSET校正寄存器-125-2");
					lsList.Add("TRUE-IC有效值OFFSET校正寄存器-126-2");
				}
				else
				{

				}

				foreach (var VARIABLE in lsList)
				{
					object[] oRow = VARIABLE.Split('-');
					dtRN8302Table.Rows.Add(oRow);
				}

				dgvRN8302.DataSource = dtRN8302Table;
				#endregion
		}

		private void tbReadRegister_Click(object sender, EventArgs e)
		{
			lstrDataMark.Clear();
			lstrDataMark.Add("DBDF0601");
			lstrDataMark.Add("04DFDFDB");
			wm = (winMain)this.Owner;
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
				for (int i = 0; i < rowCount; i++)
				{
					string strErr = wm.FunDealMessage(Helps.fnAgainst(drs[i]["寄存器地址"].ToString().PadLeft(4, '0') )+ Helps.fnAgainst(drs[i]["DataLen"].ToString().PadLeft(4, '0') ), DLT645.ControlType.ReadFactory, lstrDataMark, out string strData);

					if (strErr != "")
					{
						if (strErr != "请先打开串口!")
						{
							MessageBox.Show(drs[i]["Name"].ToString() + strErr);
						}
						return;
					}
					else
					{
						if (strData == "")
						{
							drs[i]["数据值"] = "";
							drs[i]["操作结论"] = "返回数据失败!";
							return;
						}
						else
						{
							drs[i]["数据值"] = Helps.fnAgainst(strData);
							drs[i]["操作结论"] = "读取成功!";
						}
					}
				}
				#endregion
			}
			catch (Exception exception)
			{
				//output("抄读过程中出现错误：" + exception.Message, true, true);
			}
		}

		private void tcbTypeRN_SelectedIndexChanged(object sender, EventArgs e)
		{
			CommonData();
		}

		private void tbWriteRegister_Click(object sender, EventArgs e)
		{
			lstrDataMark.Clear();
			lstrDataMark.Add("DBDF0601");
			lstrDataMark.Add("04DFDFDB");
			wm = (winMain)this.Owner;
			int iDataLen = Int32.Parse(textBox_xiaobiaoLen.Text.ToString() );
			try
			{
				DataRow[] drs = dtReaDataTable.Select(string.Format("选择 = '{0}' ", "true"), "");
				int rowCount = drs.GetLength(0);
				if (rowCount < 1)
				{
					return;
				}

				#region 设置
				bool isReadingErr = false;
				for (int i = 0; i < rowCount; i++)
				{
					if (drs[i]["数据值"].ToString() == "")
					{
						continue;
					}

					string strErr = "false";
					string strData = "";
					if (cbEEPROM.Checked)
					{
						strErr= wm.FunDealMessage(Helps.fnAgainst(Helps.fnAgainst("2" + drs[i]["寄存器地址"].ToString().PadLeft(3, '0') ) + Helps.fnAgainst(drs[i]["DataLen"].ToString().PadLeft(4, '0') ) + Helps.fnAgainst(drs[i]["数据值"].ToString() ) ), DLT645.ControlType.Write, lstrDataMark, out strData);
					}
					else
					{
						strErr= wm.FunDealMessage(Helps.fnAgainst( Helps.fnAgainst( drs[i]["寄存器地址"].ToString().PadLeft(4, '0') )+ Helps.fnAgainst(drs[i]["DataLen"].ToString().PadLeft(4, '0') ) + Helps.fnAgainst(drs[i]["数据值"].ToString() ) ), DLT645.ControlType.Write, lstrDataMark, out strData);
					}
					if (strErr != "")
					{
						if (strErr != "请先打开串口!")
						{
							MessageBox.Show(drs[i]["Name"].ToString() + strErr);
						}
						return;
					}
					else
					{
						if (strData == "")
						{
							drs[i]["操作结论"] = "设置成功!";
						}
						else
						{
							drs[i]["操作结论"] = "设置失败!";
							continue;
						}
					}
				}
				#endregion
			}
			catch (Exception exception)
			{
				//output("抄读过程中出现错误：" + exception.Message, true, true);
			}
		}

		private void txbReadRN8302_Click(object sender, EventArgs e)
		{
			lstrDataMark.Clear();
			lstrDataMark.Add("DBDF0601");
			lstrDataMark.Add("04DFDFDB");
			wm = (winMain)this.Owner;
			try
			{
				DataRow[] drs = dtRN8302Table.Select(string.Format("选择 = '{0}' ", "true"), "");
				int rowCount = drs.GetLength(0);
				if (rowCount < 1)
				{
					return;
				}

				#region 抄读
				bool isReadingErr = false;
				for (int i = 0; i < rowCount; i++)
				{
					string strErr = wm.FunDealMessage(Helps.fnAgainst(drs[i]["寄存器地址"].ToString().PadLeft(4, '0') )+ Helps.fnAgainst(drs[i]["DataLen"].ToString().PadLeft(4, '0') ), DLT645.ControlType.ReadFactory, lstrDataMark, out string strData);

					if (strErr != "")
					{
						if (strErr != "请先打开串口!")
						{
							MessageBox.Show(drs[i]["Name"].ToString() + strErr);
						}
						return;
					}
					else
					{
						if (strData == "")
						{
							drs[i]["数据值"] = "";
							drs[i]["操作结论"] = "返回数据失败!";
							return;
						}
						else
						{
							drs[i]["数据值"] = Helps.fnAgainst(strData);
							drs[i]["操作结论"] = "读取成功!";
						}
					}
				}
				#endregion
			}
			catch (Exception exception)
			{
				//output("抄读过程中出现错误：" + exception.Message, true, true);
			}
		}

		private void txbSetRN8302_Click(object sender, EventArgs e)
		{
			lstrDataMark.Clear();
			lstrDataMark.Add("DBDF0601");
			lstrDataMark.Add("04DFDFDB");
			wm = (winMain)this.Owner;
			int iDataLen = Int32.Parse(textBox_xiaobiaoLen.Text.ToString() );
			try
			{
				DataRow[] drs = dtRN8302Table.Select(string.Format("选择 = '{0}' ", "true"), "");
				int rowCount = drs.GetLength(0);
				if (rowCount < 1)
				{
					return;
				}

				#region 设置
				bool isReadingErr = false;
				for (int i = 0; i < rowCount; i++)
				{
					if (drs[i]["数据值"].ToString() == "")
					{
						continue;
					}

					string strErr = "false";
					string strData = "";
					if (cbEEPROM8302.Checked)
					{
						strErr= wm.FunDealMessage(Helps.fnAgainst(Helps.fnAgainst("2" + drs[i]["寄存器地址"].ToString().PadLeft(3, '0') ) + Helps.fnAgainst(drs[i]["DataLen"].ToString().PadLeft(4, '0') ) + Helps.fnAgainst(drs[i]["数据值"].ToString() ) ), DLT645.ControlType.Write, lstrDataMark, out strData);
					}
					else
					{
						strErr= wm.FunDealMessage(Helps.fnAgainst( Helps.fnAgainst( drs[i]["寄存器地址"].ToString().PadLeft(4, '0') )+ Helps.fnAgainst(drs[i]["DataLen"].ToString().PadLeft(4, '0') ) + Helps.fnAgainst(drs[i]["数据值"].ToString() ) ), DLT645.ControlType.Write, lstrDataMark, out strData);
					}
					if (strErr != "")
					{
						if (strErr != "请先打开串口!")
						{
							MessageBox.Show(drs[i]["Name"].ToString() + strErr);
						}
						return;
					}
					else
					{
						if (strData == "")
						{
							drs[i]["操作结论"] = "设置成功!";
						}
						else
						{
							drs[i]["操作结论"] = "设置失败!";
							continue;
						}
					}
				}
				#endregion
			}
			catch (Exception exception)
			{
				//output("抄读过程中出现错误：" + exception.Message, true, true);
			}
		}
	}
}
