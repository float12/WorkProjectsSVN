using System;
using System.Collections.Generic;
using System.Drawing;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Forms;
using TX.Framework.WindowUI.Controls;
using TX.Framework.WindowUI.Forms;

namespace WindowsTest
{
	public partial class FromContinuedFreezeTest : MainForm
	{
		public FromContinuedFreezeTest()
		{
			InitializeComponent();
			this.DoubleBuffered = true;
			this.Location = new Point((SystemInformation.PrimaryMonitorSize.Width - this.Width) / 2, (SystemInformation.PrimaryMonitorSize.Height - this.Height) / 2);
		}

		#region Variable
		private string _strAddr = "";
		public byte[] baBytes = null;
		List<string> lstrDataMark = new List<string>();
		winMain wm;
		bool bAutoFreezeTest = false;
		#endregion

		public FromContinuedFreezeTest(string strAddr)
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

		private void FromContinuedFreezeTest_Load(object sender, EventArgs e)
		{
			wm = (winMain)this.Owner;
		}

		/// <summary>
		/// AdjustTime
		/// </summary>
		/// <param name="dtDateTimeSet"></param>
		/// <returns></returns>
		private bool FunAdjustTime(DateTime dtDateTimeSet, bool bMS = false)
		{
			bool bResult = false;

			string strErr = "";
			lstrDataMark.Clear();
			lstrDataMark.Add("0400010C");
			Console.WriteLine(dtDateTimeSet.ToString("yyyy-M-d dddd HHmmss"));
			if (bMS == true)
			{
				strErr = wm.FunDealMessage( dtDateTimeSet.ToString("yyMMdd") + dtDateTimeSet.ToString("HHmmss") + "0000", DLT645.ControlType.msAdjustTime, lstrDataMark, out string strData);
			}
			else
			{
				strErr = wm.FunDealMessage( dtDateTimeSet.ToString("yyMMdd") + ((int)dtDateTimeSet.DayOfWeek).ToString("X2") + dtDateTimeSet.ToString("HHmmss"), DLT645.ControlType.Write, lstrDataMark, out string strData);
			}
			
			if (strErr != "")
			{
				if (strErr != "请先打开串口!")
				{
					MessageBox.Show(strErr);
				}
				return bResult;
			}

			bResult = true;
			return bResult;
		}

		/// <summary>
		/// CommonParaSet
		/// </summary>
		/// <param name="strDataMark"></param>
		/// <param name="strData"></param>
		/// <returns></returns>
		private bool FunSetPara(string strDataMark, string strData )
		{
			bool bResult = false;

			string strErr = "";
			lstrDataMark.Clear();
			lstrDataMark.Add(strDataMark);
			strErr = wm.FunDealMessage( strData, DLT645.ControlType.Write, lstrDataMark, out string strOutData);
			if (strErr != "")
			{
				if (strErr != "请先打开串口!")
				{
					MessageBox.Show(strErr);
				}
				return bResult;
			}

			bResult = true;
			return bResult;
		}

		/// <summary>
		/// AdjustTime
		/// </summary>
		/// <param name="dtRead"></param> 
		/// <returns></returns>
		private bool FunReadTime(out DateTime dtRead, bool bMS = false)
		{
			dtRead = new DateTime();
			bool bResult = false;
			string strErr = "";

			lstrDataMark.Clear();
			lstrDataMark.Add("0400010C");
			strErr = wm.FunDealMessage("", DLT645.ControlType.Read, lstrDataMark, out string strData);
			if (strErr != "")
			{
				if (strErr != "请先打开串口!")
				{
					MessageBox.Show(strErr);
				}
				return bResult;
			}
			else
			{
				if (strData == "")
				{
					MessageBox.Show("返回数据失败!");
					return bResult;
				}
				else
				{
					strData = Helps.fnAgainst(strData);
					if (bMS == true)
					{
						strData = strData.Substring(0, strData.Length - 4);
					}

					{
						if (!DateTime.TryParseExact("20" + (strData.Remove(6, 2)), new[] { "yyyyMMddHHmmss" }, CultureInfo.CurrentCulture, DateTimeStyles.None, out dtRead))
						{
							return false;
						}
					}
				}
			}

			bResult = true;
			return bResult;
		}

		/// <summary>
		/// AutoFreezeTest
		/// </summary>
		/// <param name="oType"></param>
		private void TAutoFreezeTest(object oType)
		{
			int itxtbCycle = int.Parse(txtbCycle.Text);
			try
			{
				if (txcbAdjustTime.Checked)
				{
					if (oType == "MinuteLTU")
					{
						if (!FunAdjustTime(DateTime.Now, true))
						{
							FnShowpbProgressHandler(0);
							return;
						}
					}
					else
					{
						if (!FunAdjustTime(DateTime.Now))
						{
							FnShowpbProgressHandler(0);
							return;
						}
					}
				}

				DateTime dtDateTimeRead = new DateTime();
				DateTime dtDateTimeSet = new DateTime();
				DateTime dtDateTimeSettlementDate = new DateTime();
				if (oType == "MinuteLTU")
				{
					if (!FunReadTime(out dtDateTimeRead, true))
					{
						FnShowpbProgressHandler(0);
						return;
					}
				}
				else
				{
					if (!FunReadTime(out dtDateTimeRead))
					{
						FnShowpbProgressHandler(0);
						return;
					}
				}

				if (oType == "Day")
				{
					dtDateTimeSet = new DateTime(dtDateTimeRead.Year, dtDateTimeRead.Month, dtDateTimeRead.Day, 23, 59, 55);
				}
				else if (oType == "SettlementDate")
				{
					dtDateTimeSettlementDate = new DateTime(dtDateTimeRead.Year, dtDateTimeRead.Month, 1, 0, 0, 0);
					dtDateTimeSettlementDate = dtDateTimeSettlementDate.AddMonths(1);
				}
				else
				{
					dtDateTimeSet = new DateTime(dtDateTimeRead.Year, dtDateTimeRead.Month, dtDateTimeRead.Day, dtDateTimeRead.Hour, 59, 55);
				}

				for (int i = 0; i < Int32.Parse(txtbCounts.Text.ToString()) && bAutoFreezeTest; i++)
				{
					if (oType == "SettlementDate")
					{
						dtDateTimeSet = dtDateTimeSettlementDate.AddSeconds(-6);
					}
					
					if (oType == "MinuteLTU")
					{
						if (!FunAdjustTime(dtDateTimeSet, true))
						{
							FnShowpbProgressHandler(0);
							return;
						}
					}
					else
					{
						if (!FunAdjustTime(dtDateTimeSet))
						{
							FnShowpbProgressHandler(0);
							return;
						}
					}

					fnWait(10000);
					foreach (Control ctrl in gbFreeze.Controls)
					{
						if (ctrl.Name == "txbFreeze" + oType.ToString())
						{
							if (!ctrl.Text.Contains("停止"))
							{
								FnShowpbProgressHandler(0);
								return;
							}
						}
					}

					FnShowpbProgressHandler((i + 1));
					if (oType == "Day")
					{
						dtDateTimeSet = dtDateTimeSet.AddDays(1);
					}
					else if (oType == "Hour")
					{
						dtDateTimeSet = dtDateTimeSet.AddHours(1);
					}
					else if ( (oType == "Minute") || (oType == "MinuteLTU") )
					{
						dtDateTimeSet = dtDateTimeSet.AddMinutes(itxtbCycle);
					}
					else if (oType == "SettlementDate")
					{
						dtDateTimeSettlementDate = dtDateTimeSettlementDate.AddMonths(1);
					}
				}
				MessageBox.Show("测试完成!");
				FnShowpbProgressHandler(0);
			}
			catch (Exception e)
			{
				//Console.WriteLine(e);
				//throw;
			}
		}

		/// <summary>
		/// StopAutoFreezeTest
		/// </summary>
		private void StopAutoFreezeTest()
		{
			bAutoFreezeTest = false;
			txbFreezeDay.Text = "日冻结测试";
			txbFreezeHour.Text = "整点冻结测试";
			txbFreezeMinute.Text = "负荷曲线测试";
			txbFreezeMinuteLTU.Text = "LTU负荷曲线测试";
			txbFreezeSettlementDate.Text = "结算日转存测试";

			FnShowpbProgressHandler(0);
		}

		#region 进度条
		public delegate void OnPregssFreeze(object objIndex);
		OnPregssFreeze dlgSetControlFreeze;
		private object m_objectShow = new object();
		private object m_objectShowUpdata = new object();
		public void FnShowpbProgressHandler(object objIndex)
		{
			try
			{
				if (dlgSetControlFreeze == null)
				{
					dlgSetControlFreeze = new OnPregssFreeze(ShowpbProgress);
				}
				Invoke(dlgSetControlFreeze, new object[] { objIndex });
			}
			catch
			{

			}
		}

		/// <summary>
		/// 刷新冻结第几次
		/// </summary>
		/// <param name="objIndex"></param>
		public void ShowpbProgress(object objIndex)
		{
			lock (m_objectShow)
			{
				int count = (int)objIndex;
				pbFreeze.Value = count;
				if (count == 0)
				{
					txbFreezeDay.Text = "日冻结测试";
					txbFreezeHour.Text = "整点冻结测试";
					txbFreezeMinute.Text = "负荷曲线测试";
					txbFreezeMinuteLTU.Text = "LTU负荷曲线测试";
					txbFreezeSettlementDate.Text = "结算日转存测试";
					txbFreezeHour.Enabled = true;
					txbFreezeDay.Enabled = true;
					txbFreezeMinute.Enabled = true;
					txbFreezeMinuteLTU.Enabled = true;
					txbFreezeSettlementDate.Enabled = true;
				}
			}
		}
		#endregion

		#region 延时函数
		[DllImport("kernel32")]
		public static extern uint GetTickCount();
		/// <summary>
		/// 等待，单位ms毫秒，以100ms为单位执行多次等待
		/// （时间不精确的！！）
		/// </summary>
		/// <param name="iMSWait"></param>
		protected void fnWait(int iMSWait)
		{
			int time_cal, outtime_cal;
			time_cal = (int)GetTickCount();

			while (bAutoFreezeTest)
			{
				outtime_cal = (int)GetTickCount() - time_cal;
				if (outtime_cal > iMSWait)
				{
					break;
				}
				Application.DoEvents();
				Thread.Sleep(1);
			}
		}
		#endregion

		private void txbFreezeDay_Click(object sender, EventArgs e)
		{
			try
			{
				if (txbFreezeDay.Text == "日冻结测试")
				{
					pbFreeze.Maximum = Int32.Parse(txtbCounts.Text.ToString());
					pbFreeze.Value = 0;
					txbFreezeDay.Text = "停止日冻结测试";
					txbFreezeHour.Enabled = false;
					txbFreezeMinute.Enabled = false;
					txbFreezeMinuteLTU.Enabled = false;
					txbFreezeSettlementDate.Enabled = false;
					bAutoFreezeTest = true;
					Thread ThTestL = new Thread(new ParameterizedThreadStart(TAutoFreezeTest));
					ThTestL.IsBackground = true;
					ThTestL.Start("Day");
				}
				else
				{
					StopAutoFreezeTest();
				}
			}
			catch (Exception exception)
			{
				Console.WriteLine(exception);
				//throw;
			}
			finally
			{
				//fnWait(1000);
				//StopAutoFreezeTest();
			}
		}

		private void txbFreezeHour_Click(object sender, EventArgs e)
		{
			if (txbFreezeHour.Text == "整点冻结测试")
			{
				pbFreeze.Maximum = Int32.Parse(txtbCounts.Text.ToString());
				pbFreeze.Value = 0;
				txbFreezeHour.Text = "停止整点冻结测试";
				txbFreezeDay.Enabled = false;
				txbFreezeMinute.Enabled = false;
				txbFreezeMinuteLTU.Enabled = false;
				txbFreezeSettlementDate.Enabled = false;
				bAutoFreezeTest = true;
				Thread ThTestL = new Thread(new ParameterizedThreadStart(TAutoFreezeTest));
				ThTestL.IsBackground = true;
				ThTestL.Start("Hour");
			}
			else
			{
				StopAutoFreezeTest();
			}
		}

		private void txbFreezeMinute_Click(object sender, EventArgs e)
		{
			if (txbFreezeMinute.Text == "负荷曲线测试")
			{
				pbFreeze.Maximum = Int32.Parse(txtbCounts.Text.ToString());
				pbFreeze.Value = 0;
				txbFreezeMinute.Text = "停止负荷曲线测试";
				txbFreezeHour.Enabled = false;
				txbFreezeDay.Enabled = false;
				txbFreezeSettlementDate.Enabled = false;
				txbFreezeMinuteLTU.Enabled = false;
				bAutoFreezeTest = true;
				Thread ThTestL = new Thread(new ParameterizedThreadStart(TAutoFreezeTest));
				ThTestL.IsBackground = true;
				ThTestL.Start("Minute");
			}
			else
			{
				StopAutoFreezeTest();
			}
		}

		private void tbSettlementDate_Click(object sender, EventArgs e)
		{
			if (txbFreezeSettlementDate.Text == "结算日转存测试")
			{
				pbFreeze.Maximum = Int32.Parse(txtbCounts.Text.ToString());
				pbFreeze.Value = 0;
				txbFreezeSettlementDate.Text = "停止结算日转存测试";
				txbFreezeHour.Enabled = false;
				txbFreezeDay.Enabled = false;
				txbFreezeMinute.Enabled = false;
				txbFreezeMinuteLTU.Enabled = false;
				bAutoFreezeTest = true;
				//设置第1结算日
				if ( !FunSetPara("04000B01", "0100") )
				{
					MessageBox.Show("设置第1结算日失败!");
					return;
				}
				//设置第2结算日
				if ( !FunSetPara("04000B02", "9999") )
				{
					MessageBox.Show("设置第2结算日失败!");
					return;
				}
				//设置第1结算日
				if ( !FunSetPara("04000B03", "9999") )
				{
					MessageBox.Show("设置第3结算日失败!");
					return;
				}

				Thread ThTestL = new Thread(new ParameterizedThreadStart(TAutoFreezeTest));
				ThTestL.IsBackground = true;
				ThTestL.Start("SettlementDate");
			}
			else
			{
				StopAutoFreezeTest();
			}
		}

		private void txbFreezeMinuteLTU_Click(object sender, EventArgs e)
		{
			if (txbFreezeMinuteLTU.Text == "LTU负荷曲线测试")
			{
				pbFreeze.Maximum = Int32.Parse(txtbCounts.Text.ToString());
				pbFreeze.Value = 0;
				txbFreezeMinuteLTU.Text = "停止LTU负荷曲线测试";
				txbFreezeHour.Enabled = false;
				txbFreezeDay.Enabled = false;
				txbFreezeSettlementDate.Enabled = false;
				txbFreezeMinute.Enabled = false;
				bAutoFreezeTest = true;
				Thread ThTestL = new Thread(new ParameterizedThreadStart(TAutoFreezeTest));
				ThTestL.IsBackground = true;
				ThTestL.Start("MinuteLTU");
			}
			else
			{
				StopAutoFreezeTest();
			}
		}
	}
}
