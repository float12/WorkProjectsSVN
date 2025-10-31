using System;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.Threading.Tasks;
using System.Windows.Forms;
using TX.Framework.WindowUI.Forms;

namespace WindowsTest
{
	public partial class FormReadMeterSystemEvent : MainForm
	{
		public FormReadMeterSystemEvent()
		{
			InitializeComponent();
			this.DoubleBuffered = true;
			this.Location = new Point((SystemInformation.PrimaryMonitorSize.Width - this.Width) / 2, (SystemInformation.PrimaryMonitorSize.Height - this.Height) / 2);
		}
		private string _strAddr = "";
		public byte[] baBytes = null;
		List<string> lstrDataMark = new List<string>();
		winMain wm;
		public FormReadMeterSystemEvent(string strAddr)
		{
			InitializeComponent();
			_strAddr = strAddr;
			this.DoubleBuffered = true;
			this.Location = new Point((SystemInformation.PrimaryMonitorSize.Width - this.Width) / 2, (SystemInformation.PrimaryMonitorSize.Height - this.Height) / 2);
		}

        private async Task HandleClickAsync()
        {
            // Can use ConfigureAwait here.
	        await Task.Delay(1000).ConfigureAwait(continueOnCapturedContext: false);
        }


		private void FormAdjustMeter_Load(object sender, EventArgs e)
		{

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

		private async void btnEventRead_Click(object sender, EventArgs e)
		{
#if !User
			try
			{
				IniClass ini = new IniClass(Application.StartupPath + @"\Config.ini");
				string diStr = "";
				if (comboBox_EventType.SelectedIndex == 0)
				{
					if (cbType.Checked)
					{
						diStr += "FE";
					}
					else
					{
						diStr += "00";
					}
				}
				else
				{
					if (cbType.Checked)
					{
						diStr += "FD";
					}
					else
					{
						diStr += "01";
					}
				}
				int eventBegin = Convert.ToInt32(textBox_EventBegin.Text.Trim());
				int eventEnd = Convert.ToInt32(textBox_EventEnd.Text.Trim());

				if (eventBegin > eventEnd)
				{
					MessageBox.Show("结束值应比开始值大");
					return;
				}
				if (eventEnd / 6 + 1 > 254)
				{
					MessageBox.Show("结束值超出最大值");
					return;
				}
				dataGridView_Event.Rows.Clear();
				wm = (winMain)this.Owner;
				for (int i = eventBegin / 6 + 1; i < eventEnd / 6 + 1; i++)
				{
					string inDIStr = diStr + i.ToString("X2");

					lstrDataMark.Clear();
					lstrDataMark.Add("DBDF" + inDIStr);
					lstrDataMark.Add("04DFDFDB");
					//winMain._dlt645 = new DLT645(_strAddr );
					//if (!winMain._dlt645.Pack(DLT645.ControlType.Read, lstrDataMark, "", 0, out baBytes, out string strErr ) )
					//         {
					//	MessageBox.Show("组包失败!");
					//	return;
					//         }

					//strErr = wm.FunMessage(baBytes, DLT645.ControlType.Read, lstrDataMark, out string strData);
					string strErr = "";
					string strData = "";
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
						strErr = wm.FunDealMessage( "", DLT645.ControlType.Read, lstrDataMark, out  strData);
					}
					
					if (strErr == "")
					{
						strData = Helps.fnAgainst(strData);
						for (int j = 0; j < 6; j++)
						{
							int rowIndex = dataGridView_Event.Rows.Add();
							string strPart = "";
							if (comboBox_EventType.SelectedIndex == 0)
							{
								strPart = strData.Substring((5 - j) * 20, 20);
								dataGridView_Event.Rows[rowIndex].Cells[1].Value = strPart.Trim().Substring(12, 4).Trim();
								dataGridView_Event.Rows[rowIndex].Cells[0].Value = strPart.Trim().Substring(16, 4).Trim();
								int Num;
								if (int.TryParse(strPart.Trim().Substring(16, 4).Trim(), System.Globalization.NumberStyles.AllowHexSpecifier, null, out Num))
								{
									dataGridView_Event.Rows[rowIndex].Cells[3].Value = ini.IniReadValue("自由事件解析", Num.ToString(), "");
								}
							}
							else
							{
								strPart = strData.Substring((5 - j) * 16, 16);
								string strEx = strPart.Trim().Substring(12, 4).Trim();
								dataGridView_Event.Rows[rowIndex].Cells[0].Value = strEx;
								switch (strEx.Trim().Substring(0, 1))
								{
									case "0":
										int num;
										if (int.TryParse(strEx.Trim(), System.Globalization.NumberStyles.AllowHexSpecifier, null, out num))
										{
											dataGridView_Event.Rows[rowIndex].Cells[3].Value = ini.IniReadValue("Err-Flag", num.ToString(), "");
										}
										break;
									case "8":
										dataGridView_Event.Rows[rowIndex].Cells[3].Value = ini.IniReadValue("异常事件解析", strEx.Trim(), "");
										break;
									case "C":
										if (int.TryParse(strEx.Trim(), System.Globalization.NumberStyles.AllowHexSpecifier, null, out num))
										{
											dataGridView_Event.Rows[rowIndex].Cells[3].Value = "断言存储的异常事件,断言行号：" + (num - 0xC000).ToString();
										}
										break;
									default:
										break;
								}
							}

							dataGridView_Event.Rows[rowIndex].HeaderCell.Value = "上" + (6 * (i - 1) + j + 1).ToString() + "次";
							dataGridView_Event.Rows[rowIndex].Cells[2].Value = strPart.Trim().Substring(0, 2) + "年" + strPart.Trim().Substring(2, 2) + "月" + strPart.Trim().Substring(4, 2) + "日" + strPart.Trim().Substring(6, 2) + "时" + strPart.Trim().Substring(8, 2) + "分" + strPart.Trim().Substring(10, 2) + "秒";
							dataGridView_Event.CurrentCell = dataGridView_Event.Rows[rowIndex].Cells[0];
						}
					}

					if (strErr == "请先打开串口!")
					{
						return;
					}
				}
			}
			catch
			{

			}
#endif
		}

		private void btn_导出至Excel_Click(object sender, EventArgs e)
		{
			SaveFileDialog saveFileDialog1 = new SaveFileDialog();
			saveFileDialog1.Filter = "Excel文件（*.xlsx）|*.xlsx|Excel文件（*.xls）|*.xls";
			saveFileDialog1.FilterIndex = 1;
			saveFileDialog1.RestoreDirectory = true;
			if (saveFileDialog1.ShowDialog() == DialogResult.OK)
			{
				List<DataTable> dts = new List<DataTable>();
				DataTable dtScheme = new DataTable();
				{
					dtScheme.Columns.Add("次数", typeof(string));
					dtScheme.Columns.Add("事件类型（HEX）", typeof(string));
					dtScheme.Columns.Add("事件附加记录", typeof(string));
					dtScheme.Columns.Add("事件发生时间", typeof(string));
					dtScheme.Columns.Add("事件解析", typeof(string));
					for (int i = 0; i < dataGridView_Event.Rows.Count; i++)
					{
						object[] objects = new[] { dataGridView_Event.Rows[i].HeaderCell.Value, dataGridView_Event[0, i].Value, dataGridView_Event[1, i].Value, dataGridView_Event[2, i].Value, dataGridView_Event[3, i].Value, };
						dtScheme.Rows.Add(objects);
					}
				}
				dts.Add(dtScheme);
				if (!AsposeExcelTools.DataTableListToExcel(dts, saveFileDialog1.FileName, new string[] { "事件" }, out string strErr))
				{
					MessageBox.Show("导出EXCEL文件失败，失败原因：" + strErr);
					return;
				}
				MessageBox.Show("导出Excel成功！");
			}
		}
	}
}
