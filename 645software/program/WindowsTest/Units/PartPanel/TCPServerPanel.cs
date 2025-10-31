using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using WindowsTest;
/*---------------作者：Maximus Ye----------------------*/
/*---------------时间：2013年8月16日---------------*/
/*---------------邮箱：yq@yyzq.net---------*/
/*---------------QQ：275623749---------*/
/*本软件也耗费了我不少的时间和精力，希望各位同行们尊重个人劳动成果，
 * 如果在此版本的基础上修改发布新的版本，请包含原作者信息（包括代码和UI界面相关信息)，为中国的
 * 开源事业做出一点贡献。*/
namespace LeafSoft.PartPanel
{
    public partial class TCPServerPanel : BasePanel
    {
        public TCPServerPanel()
        {
            InitializeComponent();
        }
		public static byte[] RecBuf = null;
		public static string strSender = "";
		
		#region Transshipment
		public static SerialTool stTransshipment = new SerialTool();
		bool TransshipmentMoniter = false;
		Thread ThTestL = null;
		#endregion

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
        private bool DataSender_EventDataSend(byte[] data)
        {
			TCPServerPanel.RecBuf = null;
			strSender = "";
            return Configer.SendData(data);
        }

        private void Configer_DataReceived(object sender, byte[] data)
        {
			if (strSender == sender.ToString())
			{
				RecBuf = new byte[data.Length];
				if (data != null)
				{
					Array.Copy(data, RecBuf, data.Length);
				}
				else
				{
					RecBuf = null;
				}

				tabDataReceiver.AddData(sender.ToString(),data);
				if (txcTransshipment.Checked)
				{
					int iCount = 0;
					while(true)
					{
						iCount++;
						if (iCount > 5000 / 50)
						{
							break;
						}
						if (data != null)
						{
							stTransshipment.sendWithNoAsk(data, data.Length);
							if (( data[data.Length - 1] != 0x16 ))
							{
								data = null;
								break;
							}
							else
							{
								data = null;
							}
						}
						Thread.Sleep(50);
					}
				}
			}
			else
			{
				tabDataReceiver.AddData(sender.ToString(),data);
			}
        }

        public override void ClearSelf()
        {
            Configer.ClearSelf();
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
					   //         if (EventDataSend != null)
					   //         {
					   //             if (EventDataSend(baReceBuf) == false)
					   //             {
					   //                 StopAutoSend();
					   //             }
								//	else
								//	{
								//		lblCount.Invoke(new MethodInvoker(delegate
								//		{
								//			lblCount.Text = (int.Parse(lblCount.Text) + baReceBuf.Length).ToString();
								//		}));
								//	}
								//}
								Configer.SendData(baReceBuf);
								int iCount = 0;
								List<byte> list = new List<byte>();
								//while (TCPServerPanel.RecBuf == null && (!( TCPServerPanel.RecBuf != null && TCPServerPanel.RecBuf[TCPServerPanel.RecBuf.Length - 1] != 0x16 )))
								//while(true)
								//{
								//	iCount++;
								//	if (iCount > 5000 / 90)
								//	{
								//		break;
								//	}
								//	Thread.Sleep(90);
								//	if (RecBuf != null)
								//	{
								//		stTransshipment.sendWithNoAsk(RecBuf, RecBuf.Length);
								//		RecBuf = null;
								//	}
									
								//	if (( RecBuf != null && RecBuf[RecBuf.Length - 1] != 0x16 ))
								//	{
								//		break;
								//	}
								//}
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
				//lblCount.Text = "0";
				stTransshipment.setSerialPort(txcbTransshipment.Text.ToString(), 9600, "Even", 8, "1", 5000);
				ThTestL = new Thread(new ParameterizedThreadStart(tTransshipmentMoniter));
				ThTestL.IsBackground = true;
				ThTestL.Start();
			}
		}
		#endregion

		private void DataSender_Load(object sender, EventArgs e)
		{
			int i = 0;
			this.txcbTransshipment.Items.Clear(); // 清除串口
			string[] gCOM = System.IO.Ports.SerialPort.GetPortNames(); // 获取设备的所有可用串口
			int j = gCOM.Length; // 得到所有可用串口数目
			for (i = 0; i < j; i++)
			{
				this.txcbTransshipment.Items.Add(gCOM[i]); // 依次添加到下拉框中
			}
		}

		public void txcbTransshipment_SelectedIndexChanged(object sender, EventArgs e)
		{
			//int i = 0;
			//this.txcbTransshipment.Items.Clear(); // 清除串口
			//string[] gCOM = System.IO.Ports.SerialPort.GetPortNames(); // 获取设备的所有可用串口
			//int j = gCOM.Length; // 得到所有可用串口数目
			//for (i = 0; i < j; i++)
			//{
			//	this.txcbTransshipment.Items.Add(gCOM[i]); // 依次添加到下拉框中
			//}
		}
	}
}
