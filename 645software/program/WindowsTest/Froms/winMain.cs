using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using System.Net.Sockets;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using Windows.Devices.Bluetooth;
using TX.Framework.WindowUI.Controls;
using TX.Framework.WindowUI.Forms;
using Windows.Devices.Enumeration;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using System.Threading.Tasks;
using Windows.Storage.Streams;
using Windows.Security.Cryptography;
using Windows.Storage.Streams;
using System.Text.RegularExpressions;
using Windows.Foundation;
using Aspose.Cells;
using Point = System.Drawing.Point;
using Size = System.Drawing.Size;
using LeafSoft.PartPanel;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.TaskbarClock;
using System.Runtime.InteropServices;
using static System.Net.Mime.MediaTypeNames;
using Application = System.Windows.Forms.Application;
using System.IO.Ports;
using static WindowsTest.winMain;
using System.Runtime.InteropServices.ComTypes;
using Test.Froms.FromMenu;

namespace WindowsTest
{
	public partial class winMain : MainForm
	{
		public static winMain mainFrm;
		public winMain()
		{
			mainFrm = this;
			InitializeComponent();
			this.DoubleBuffered = true;
			this.Location = new Point(Screen.PrimaryScreen.WorkingArea.Width / 4, Screen.PrimaryScreen.WorkingArea.Height / 4);
			this.tabPage4.Parent = null;
			//this.tpEncryptTest.Parent = null;

			this.Text = this.Text.Replace(")", File.GetLastWriteTime(this.GetType().Assembly.Location).ToString("yyyyMMdd") + ")");
			//tabPage10.Parent = null;
			this.tpCAN.Visible = false;
#if NOBLE
			this.tpEncryptTest.Parent = null;
#endif

#if User
			this.tpCAN.Visible = false;
			this.Text = this.Text.Replace(")", "U)");
			btnTest.Visible = false;
			txcbSetControl.Visible = false;
			cb698.Visible = false;
			tcbErrStop.Visible = false;
			this.tabPage2.Parent = null;
			this.tpPowerControl.Parent = null;
			//this.tpReadData.Parent = null;
			this.tabPage6.Parent = null;
			this.tabPage7.Parent = null;
			this.tabPage1.Parent = null;

			//控制清零
			tsmiControlClear.Visible = true;
			//负荷记录
			tsmiLoadProFile.Visible = false;
			//厂内
			Factory.Visible = false;
			btnMeterClear.Visible = false;
			//Btn_厂内.Visible = false;
			this.tpSpecialTest.Parent = null;
			
			ReceieveMinus33.Visible = false;
			txbOneKey.Visible = false;
			//txcbBLE.Visible = false;
			btn_清空全部.Visible = false;
			btn_导入.Visible = false;
			btn_导出.Visible = false;
			txcbCheckData.Visible = false;
			txbReadVersion.Visible = false;
			this.tpFunction.Parent = null;
			this.tpTopoZX.Parent = null;
			this.tabPage9.Parent = null;
			this.tpSpecialTest.Parent = null;
			this.tpMoniter.Parent = null;
			this.tpReadData.Parent = null;
			this.tpCAN.Parent = null;
			this.tpRailMeter.Parent = null;

#elif Special
			this.Text = this.Text.Replace(")", "T)");
			btnTest.Visible = false;
			txcbSetControl.Visible = false;
			cb698.Visible = false;
			this.tabPage2.Parent = null;
			this.tabPage6.Parent = null;
			this.tabPage7.Parent = null;
			this.tpEncryptTest.Parent = null;
			this.tpTopoZX.Parent = null;
            this.tpSpecialTest.Parent = null;
            this.tabPage9.Parent = null;
			this.tpMoniter.Parent = null;
            this.tpXL1140.Parent = null;
            this.tpTripping.Parent = null;
            this.tpSensor.Parent = null;
            this.tpReadDataExcel.Parent = null;
            //控制清零
            tsmiControlClear.Visible = false;
			//负荷记录
			tsmiLoadProFile.Visible = false;
			//厂内
			Factory.Visible = false;
			btnMeterClear.Visible = false;
            //Btn_厂内.Visible = false;
            this.tpMoniter.Parent = null;
            this.tpCAN.Parent = null;
            this.tpRailMeter.Parent = null;
            this.tpFunction.Parent = null;
            this.tpPowerControl.Parent = null;
#elif Project
			this.Text = this.Text.Replace(")", "P)");
			btnTest.Visible = false;
			txcbSetControl.Visible = false;
			cb698.Visible = false;
			tcbErrStop.Visible = false;
			txbOneKey.Visible = false;
			txcbBLE.Visible = false;
			txcbCheckData.Visible = false;
			this.tpSpecialTest.Parent = null;
			this.tpReadData.Parent = null;
			this.tpTopoZX.Parent = null;
			this.tabPage6.Parent = null;
			this.tabPage7.Parent = null;
			this.tpRailMeter.Parent = null;
			this.tpPowerControl.Parent = null;
			this.PartUpgrade.Parent = null;
			this.tpTCP.Parent = null;
			//this.tp6025Updata.Parent = null;
			this.tp6025Updata4G.Parent = null;
			this.tpF460Updata.Parent = null;
			this.tp7326Updata.Parent = null;
			this.tpUpdataZX.Parent = null;
			this.tpEncryptTest.Parent = null;
			this.tpFunction.Parent = null;
			this.tpCAN.Parent = null;
			this.tabPage10.Parent = null;
			//控制清零
			tsmiControlClear.Visible = false;
			//负荷记录
			tsmiLoadProFile.Visible = false;
			//厂内
			Factory.Visible = false;
			//Btn_厂内.Visible = false;
			btnMeterClear.Visible = false;
#elif ShortUpGrade
			this.Text = this.Text.Replace(")", "SU)");
			btnTest.Visible = false;
			txcbSetControl.Visible = false;
			cb698.Visible = false;
			tcbErrStop.Visible = false;
			txbOneKey.Visible = false;
			txcbBLE.Visible = false;
			this.tpSpecialTest.Parent = null;
			this.tpReadData.Parent = null;
			this.tpTopoZX.Parent = null;
			this.tabPage6.Parent = null;
			this.tabPage7.Parent = null;
			this.tp6025Updata.Parent = null;
			this.tp6025Updata4G.Parent = null;
			this.tpF460Updata.Parent = null;
			this.tp7326Updata.Parent = null;
			this.tpUpdataZX.Parent = null;
			this.tpEncryptTest.Parent = null;
			this.tpFunction.Parent = null;
			this.tpCAN.Parent = null;
			this.tabPage10.Parent = null;
			this.tpTCP.Parent = null;
			this.tpRailMeter.Parent = null;
			this.tpPowerControl.Parent = null;
			//this.tabPage2.Parent = null;
			//控制清零
			tsmiControlClear.Visible = false;
			//负荷记录
			tsmiLoadProFile.Visible = false;
			//厂内
			Factory.Visible = false;
			Btn_厂内.Visible = false;
			txbReadVersion.Visible = false;
			btnMeterClear.Visible = false;
			txcbCheckData.Visible = false;
			btn_导入.Visible = false;
			btn_导出.Visible = false;
#elif lr
			this.Text = this.Text.Replace(")", "LR)");
			btnTest.Visible = false;
			txcbSetControl.Visible = false;
			cb698.Visible = false;
			txbOneKey.Visible = false;
			txcbBLE.Visible = false;
			tcbErrStop.Visible = false;
			this.tpSpecialTest.Parent = null;
			this.tpReadData.Parent = null;
			this.tpFunction.Parent = null;
			//this.tpTopoZX.Parent = null;
			this.tabPage6.Parent = null;
			this.tabPage5.Parent = null;
			this.tp6025Updata.Parent = null;
			this.tp6025Updata4G.Parent = null;
			//this.tpF460Updata.Parent = null;
			this.tp7326Updata.Parent = null;
			this.tpUpdataZX.Parent = null;
			this.tpEncryptTest.Parent = null;
			//控制清零
			tsmiControlClear.Visible = false;
			//负荷记录
			tsmiLoadProFile.Visible = false;
			//厂内
			Factory.Visible = false;
			Btn_厂内.Visible = false;
			btnMeterClear.Visible = false;
#endif
		}

		#region Variable
		public int loopSelIndex = 0; // 循环选择索引
		public static SerialTool SpMeter = new SerialTool(); // 串口工具实例，用于测量
		public static SerialTool SpWuCha = new SerialTool(); // 串口工具实例，用于误差检测
		public static SerialTool stCAN = new SerialTool(); // 串口工具实例，用于CAN通信
		public static SerialTool stSource = new SerialTool(); // 串口工具实例，用于源控制
		private int _repeatCount = 1; // 重复次数
		private string strNetServerIP = ""; // 网络服务器IP地址
		private string strNetServerPort = ""; // 网络服务器端口
		public static NetTcpTool netTcptool = null; // 网络TCP工具实例
		public static Socket ClientSocket = null; // 客户端Socket实例
		public bool bListener = true; // 监听器状态
		public bool bBLE = false; // 蓝牙状态

		//LowVoltageDistrict
		//注册表分支 wsd.EMS22 环保监测软件 Environmental monitoring software根据可实际软件写
#if User
		string strSubKey = "wsd.EMS22.User";
#elif ShortUpGrade
		string strSubKey = "wsd.EMS22ShortUpGrade";
#elif Special
        string strSubKey = "wsd.EMS22Special";	
#else
		string strSubKey = "wsd.EMS22";
#endif


		/// <summary>
		/// 是否在自动循环发送状态
		/// </summary>
		bool AutoSend = false;

		/// <summary>
		/// 等待超时后是否重发
		/// </summary>
		private bool isRepeat = true;

		PropertyManageCls pmc = new PropertyManageCls();
		Property pp = new Property("选择串口", "COM1", false, true);
		Property addr = new Property("点击右侧按钮可探测通信地址", "AAAAAAAAAAAA", false, true) { Category = "电表串口通信", DisplayName = "7.通信地址", Editor = new PropertyGridAddr() };
		Property pData = new Property("data文件选择", "data.xlsx", false, true) { Category = "data选择", DisplayName = "当前data", Converter = new DropDownListConverterData(new[] { "data.xlsx", }) };
		public static List<DataTable> DataBank = new List<DataTable>();
		private DataTable dt_RW = null; // 读写数据表
		private DataTable dtCAN = null; // CAN数据表
		private DataTable dtMockCAN = null; // 模拟CAN数据表
		private DataTable dtReaDataTable = null; // 实际数据表
		private DataTable dtTripDataTable = null; // 实际数据表
		private DataTable dtSpecialReaDataTable = null; // 特殊实际数据表
		private DataTable dtCheckErrDataTable = null; // 检查错误数据表
		private DataTable dtTeleindication = null; // 远程指示数据表
		private DataTable dtMoniter = null; // 监控数据表
		private DataTable dtHWDC = null; // HWDC
		private Dictionary<string, DataTable> savedTables = new Dictionary<string, DataTable>();//保存最新读取数据页面的表格
		private Dictionary<string, DataTable> AllReadDataTables = new Dictionary<string, DataTable>();//保存所有读取数据项的表格
		bool bTripTest = false;
		//List<string> lsList = new List<string>();
		private DataTable dtVersion7326 = null;
		private DataTable dtVersionF460 = null;
		public static DataTable dtL2 = new DataTable();
		DataTable dtLoadRecord = new DataTable();
		IniClass ini = new IniClass(Application.StartupPath + @"\Config.ini");
		int iTimerRowIndex = 0;
		string strDataParse = "";
		SCUProxy m_SCUProxy = new SCUProxy();

		private float fHighTemperatureI = 0;
		private float fHighTemperatureU = 0;
		private float fEffectiveValueJudgeI = 0;
		private float fEffectiveValueJudgeU = 0;

		#region BLE
		//找到的设备
		private Dictionary<string, DeviceInformation> devices = new Dictionary<string, DeviceInformation>();
		private DeviceWatcher deviceWatcher;
		//当前选中的BLE设备
		BluetoothLEDevice ble_Device;
		string Dev_Name_Str;
		string Dev_ID_Str;
		readonly uint E_BLUETOOTH_ATT_WRITE_NOT_PERMITTED = 0x80650003;
		readonly uint E_BLUETOOTH_ATT_INVALID_PDU = 0x80650004;
		readonly uint E_ACCESSDENIED = 0x80070005;
		readonly uint E_DEVICE_NOT_AVAILABLE = 0x800710df; // HRESULT_FROM_WIN32(ERROR_DEVICE_NOT_AVAILABLE)
														   //当前选中的BLE服务
		GattDeviceService ble_Service = null;
		//当前设备提供的服务组
		IReadOnlyList<GattDeviceService> ble_Services = null;
		//当前选中的BLE属性组
		IReadOnlyList<GattCharacteristic> ble_Characteristics = null;
		//当前选中的BLE属性FFF1
		GattCharacteristic ble_Characteristic_0xFFF1 = null;
		//当前选中的BLE属性FFF2
		GattCharacteristic ble_Characteristic_0xFFF2 = null;
		//上一个BLE属性FFF1
		GattCharacteristic last_ble_Characteristic_0xFFF1 = null;

		//当前选中的BLE属性订阅subscribe
		GattCharacteristic ble_CharacteristicSubscribe = null;
		//当前选中的BLE属性写Write
		GattCharacteristic ble_CharacteristicWrite = null;
		//上一个BLE属性FFF1
		GattCharacteristic last_ble_CharacteristicSubscribe = null;

		int iServiceBLE = 0xFFF0; // BLE 服务标识符
		int iCharacteristicSubscribeBLE = 0xFFF1; // BLE 订阅特征标识符
		int iCharacteristicWriteBLE = 0xFFF2; // BLE 写特征标识符

		//数据格式
		GattPresentationFormat presentationFormat;
		private List<byte> ListBuffer = new List<byte>(10 * 1024);
		public object rev_obj = new object();
		private byte[] SendBuf = new byte[10 * 1024];
		private byte[] RecBuf = null;
		private int SendPointer = 0;
		#endregion

		#region PartUpGrade
		private List<byte[]> lbaBeforeUpGrade = new List<byte[]>();
		private List<byte[]> lbaAfterUpGrade = new List<byte[]>();
		private List<byte[]> lbaUpGrade = new List<byte[]>();
		private List<int> liUpGrade = new List<int>();
		private bool bPartUpGrade = false;

		#endregion
		public static DLT645 dlt645TCP = new DLT645("888888888888");
		#endregion

		#region ReadDataExcel
		private DataTable dtReadDataExcel = null;
		bool bMessageTestExcel = false;
		Int64 i64MessageTestCount = 0;
		#endregion

		/// <summary>
		/// 检查注册表注册码
		/// </summary>
		/// <param name="strLicense">注册码</param>
		/// <returns></returns>
		public bool CheckRegister(string strLicense)
		{
			//注册表分支 strSubKey 可根据实际软件写
			RegistryKey retkey = Registry.CurrentUser.OpenSubKey("SOFTWARE", true).CreateSubKey("wsd").CreateSubKey(strSubKey);
			foreach (string strRNum in retkey.GetSubKeyNames())
			{
				if (strRNum == strLicense)
				{
					return true;
				}
			}
			return false;
		}

		#region DrawRibbonBtn
		protected override void DrawCaption(Graphics g)
		{
			base.DrawCaption(g);
			if (this.CaptionHeight > 0)
			{
				winMain.DrawRibbonBtn(g);
			}
		}

		/// <summary>
		/// 重绘制水晶按钮
		/// </summary>
		/// <param name="g">The g.</param>
		public static void DrawRibbonBtn(Graphics g)
		{
			Rectangle rect = new Rectangle(3, 1, 34, 34);
			rect.Inflate(-1, -1);
			GDIHelper.FillEllipse(g, rect, Color.White);
			Color c1 = Color.Empty, c2 = Color.Empty, c3 = Color.FromArgb(232, 246, 250);
			Blend blend = new Blend();
			blend.Positions = new float[] { 0f, 0.3f, 0.5f, 0.8f, 1f };
			blend.Factors = new float[] { 0.15f, 0.55f, 0.7f, 0.8f, 0.95f };

			c1 = Color.FromArgb(255, 255, 255);
			c2 = Color.FromArgb(255, 251, 232);

			blend.Positions = new float[] { 0f, 0.3f, 0.5f, 0.85f, 1f };
			blend.Factors = new float[] { 0.95f, 0.70f, 0.45f, 0.3f, 0.15f };

			GDIHelper.DrawCrystalButton(g, rect, c1, c2, c3, blend);
			Color borderColor = Color.FromArgb(65, 177, 199);
			GDIHelper.DrawEllipseBorder(g, rect, borderColor, 1);
			Size imgSize = new System.Drawing.Size(25, 25);
			GDIHelper.DrawImage(g, rect, Test.Properties.Resources.ico, imgSize);
			g.ResetClip();
		}
		#endregion

		private void winMain_Load(object sender, EventArgs e)
		{
#if License

			#region 注册码
			bool isLicense = true;
			Computer computer = new Computer();
			string machineCode = computer.CpuID + strSubKey + computer.DiskID + computer.LoginUserName;
			byte[] machineCodeBytes = Encoding.ASCII.GetBytes(machineCode);
			byte add = 0;
			for (int i = 0; i < machineCodeBytes.Length; i++)
			{
				if (i % 2 == 0)
				{
					machineCodeBytes[i] += add++;
				}
			}

			machineCode = Helps.ByteToStringWithNoSpace(machineCodeBytes);
			if (machineCode.Length < 72)
			{
				string str = "100040001FDD00009FEC0000E79F0000EF9F10005000F79F0000FF9F000007A000000FA0";
				machineCode += str;
			}
			if (machineCode.Length > 72)
			{
				machineCode = machineCode.Substring(0, 72);
			}

			string liscenseStr = DataConvert.GetLicense(machineCode);
			if (!CheckRegister(liscenseStr))
			{
				Register license = new Register(machineCode);

				if (license.ShowDialog() == DialogResult.OK)
				{
					if (liscenseStr == license.license)
					{
						//注册表分支 wsd.EMS22 环保监测软件 Environmental monitoring software根据可实际软件写
						RegistryKey retkey = Registry.CurrentUser.OpenSubKey("Software", true).CreateSubKey("wsd").CreateSubKey(strSubKey).CreateSubKey(liscenseStr);
						retkey.SetValue("Userliscense", liscenseStr);
					}
					else
					{
						MessageBox.Show("输入的注册码不对");
						this.Close();
						return;
					}
				}
				else
				{
					this.Close();
					return;
				}
			}
			#endregion

#endif
			string strIniReadValue = ini.IniReadValue("Main", "CurrentDirectory", "data.xlsx");
			string[] strFiles = Directory.GetFiles(Environment.CurrentDirectory, "*.xlsx");
			object[] oaObjects = new object[strFiles.Length];
			bool bContains = false;
			for (int i = 0; i < strFiles.Length; i++)
			{
				if (strFiles[i].Contains("data"))
				{
					oaObjects[i] = strFiles[i].Replace(Environment.CurrentDirectory + "\\", "");
				}

				if (strFiles[i].Contains(strIniReadValue))
				{
					bContains = true;
				}
			}
			oaObjects = oaObjects.Where(val => val != null).ToArray();
			if (!bContains)
			{
				strIniReadValue = oaObjects[0].ToString();
			}
			string errStr = "";
			if (!AsposeExcelTools.ExcelAllSheetToDataTableList(Environment.CurrentDirectory + "\\" + strIniReadValue, out DataBank, out errStr))
			{
				MessageBox.Show("加载数据库失败：" + strIniReadValue + errStr);
				if (!AsposeExcelTools.ExcelAllSheetToDataTableList(Environment.CurrentDirectory + "\\data.xlsx", out DataBank, out errStr))
				{
					MessageBox.Show("加载数据库失败：data.xlsx" + errStr);
					this.Close();
					return;
				}
			}

			isHex解析电能 = true;
			string[] paraStrings = new[] { "COM3", "9600", "Even", "8", "1", "8000", "AAAAAAAAAAAA", "02", "000000", "88888888", "1000", "50", "data.xlsx" };

			paraStrings[0] = ini.IniReadValue("Main", "ComPortName", "COM3");
			paraStrings[1] = ini.IniReadValue("Main", "BaudRate", "9600");
			paraStrings[2] = ini.IniReadValue("Main", "Parity", "Even");
			paraStrings[3] = ini.IniReadValue("Main", "DataBits", "8");
			paraStrings[4] = ini.IniReadValue("Main", "stopBits", "1");
			paraStrings[5] = ini.IniReadValue("Main", "OverTime", "5000");
			paraStrings[7] = ini.IniReadValue("Main", "PasswordLevel", "02");
			paraStrings[8] = ini.IniReadValue("Main", "Password", "000000");
			paraStrings[9] = ini.IniReadValue("Main", "Mins", "88888888");
			paraStrings[10] = ini.IniReadValue("Main", "Period", "1000");
			paraStrings[11] = ini.IniReadValue("Main", "SendThenDelay", "50");
			paraStrings[12] = strIniReadValue;

			pp.Category = "电表串口通信";
			pp.DisplayName = "1.选择串口";
			pp.Converter = new DropDownListCom();
			pp.Value = paraStrings[0];
			pmc.Add(pp);

			pp = new Property("波特率", paraStrings[1], false, true)
			{
				Category = "电表串口通信",
				DisplayName = "2.波特率",
				Converter = new DropDownListConverter(new[] { "1200", "2400", "4800", "7200", "9600", "19200", "38400", "57600", "115200", "125000", "128000" })
			};
			pmc.Add(pp);

			pp = new Property("校验位", paraStrings[2], false, true) { Category = "电表串口通信", DisplayName = "3.校验位", Converter = new DropDownListConverter(new[] { "None", "Even", "Odd" }) };
			pmc.Add(pp);

			pp = new Property("数据位", paraStrings[3], false, true) { Category = "电表串口通信", DisplayName = "4.数据位", Converter = new DropDownListConverter(new[] { "7", "8" }) };
			pmc.Add(pp);

			pp = new Property("停止位", paraStrings[4], false, true) { Category = "电表串口通信", DisplayName = "5.停止位", Converter = new DropDownListConverter(new[] { "1", "1.5", "2" }) };
			pmc.Add(pp);

			pp = new Property("等待超时,单位：ms", paraStrings[5], false, true) { Category = "电表串口通信", DisplayName = "6.等待超时（ms）" };
			pmc.Add(pp);
			pmc.Add(addr);

			#region Password
			pp = new Property("密码等级", paraStrings[7], false, true)
			{
				Category = "密码",
				DisplayName = "8.密码等级",
				Converter = new DropDownListConverter(new[] { "00", "02", "03", "04", "96", "97", "98" })
			};
			pmc.Add(pp);

			pp = new Property("密码", paraStrings[8], false, true)
			{
				Category = "密码",
				DisplayName = "9.密码",
				Converter = new DropDownListConverter(new[] { "000000", "330000", "123456", })
			};
			pmc.Add(pp);

			pp = new Property("相对分钟数", paraStrings[9], false, true)
			{
				Category = "相对分钟数",
				DisplayName = "10.相对分钟数",
				Converter = new DropDownListConverter(new[] { "88888888", "55AAAA55" })
			};
			pmc.Add(pp);
			#endregion

			#region MessageTest
			pp = new Property("发送周期,单位：ms", paraStrings[10], false, true)
			{
				Category = "报文测试(单位：ms)",
				DisplayName = "1.发送周期",
			};
			pmc.Add(pp);

			pp = new Property("接收后延时时间,单位：ms", paraStrings[11], false, true)
			{
				Category = "报文测试(单位：ms)",
				DisplayName = "2.接收后延时",
			};
			pmc.Add(pp);
			#endregion

			#region SelcetData

			pData = new Property("data文件选择", paraStrings[12], false, true)
			{
				Category = "data选择",
				DisplayName = "当前data",
				Converter = new DropDownListConverter(oaObjects)
			};
			pmc.Add(pData);
			#endregion

			ppg_基本参数.SelectedObject = pmc;

			#region 加载树

			if (treeView1 != null)
			{
				treeView1.Nodes.Clear();
			}

			DataTable dtL1 = DataBank.Find(x => x.TableName == "level1");
			dtL2 = DataBank.Find(x => x.TableName == "level2");
			for (int i = 0; i < dtL1.Rows.Count; i++)
			{
				if (dtL1.Rows[i]["Name"] != null)
				{
					string IDstr = dtL1.Rows[i]["ID"].ToString();
					TreeNode tn = new TreeNode(dtL1.Rows[i]["Name"].ToString())
					{
						Tag = IDstr
					};
					DataRow[] drs = dtL2.Select(string.Format("ParentID = '{0}' ", IDstr));
					for (int j = 0; j < drs.Length; j++)
					{
						int ParentID;
						if (drs[j]["Name"] == null || (!int.TryParse(drs[j]["ParentID"].ToString(), out ParentID)))
						{
							continue;
						}
						tn.Nodes.Add(new TreeNode(drs[j]["Name"].ToString())
						{
							Tag = drs[j]["ID"].ToString()
						});
					}

					treeView1.Nodes.Add(tn);
				}
			}
			#endregion

			#region 读写参数
			System.Type dgvtype = typeof(DataGridView);
			System.Reflection.PropertyInfo dgvPropertyInfo = dgvtype.GetProperty("DoubleBuffered", System.Reflection.BindingFlags.Instance | System.Reflection.BindingFlags.NonPublic);
			dgvPropertyInfo.SetValue(dgv_RW, true, null);
			//dgv_RW.AutoGenerateColumns = false;
			dt_RW = new DataTable(null);

			dt_RW.Columns.Add("选择", typeof(bool));
			dt_RW.Columns.Add("ID", typeof(string));
			dt_RW.Columns.Add("ParentID", typeof(string));
			dt_RW.Columns.Add("Name", typeof(string));
			dt_RW.Columns.Add("DataMark", typeof(string));
			dt_RW.Columns.Add("DataMarkFather", typeof(string));
			dt_RW.Columns.Add("DataLen", typeof(string));
			dt_RW.Columns.Add("DataBlockType", typeof(string));
			dt_RW.Columns.Add("DataFormat", typeof(string));
			dt_RW.Columns.Add("DecimalPlaces", typeof(string));
			dt_RW.Columns.Add("DefaultValue", typeof(string));
			dt_RW.Columns.Add("DataUnit", typeof(string));
			dt_RW.Columns.Add("读取值", typeof(string));
			dt_RW.Columns.Add("设置值", typeof(string));
			dt_RW.Columns.Add("操作结论", typeof(string));
			dgv_RW.DataSource = dt_RW;
			#endregion

			ReadCommonData();
			gb485BLE.Visible = false;
		}

		/// <summary>
		/// RefreshTree
		/// </summary>
		/// <returns></returns>
		public static bool RefreshTree()
		{
			#region 加载树
			winMain.mainFrm.treeView1.Nodes.Clear();
			DataTable dtL1 = DataBank.Find(x => x.TableName == "level1");
			dtL2 = DataBank.Find(x => x.TableName == "level2");
			for (int i = 0; i < dtL1.Rows.Count; i++)
			{
				if (dtL1.Rows[i]["Name"] != null)
				{
					string IDstr = dtL1.Rows[i]["ID"].ToString();
					TreeNode tn = new TreeNode(dtL1.Rows[i]["Name"].ToString())
					{
						Tag = IDstr
					};
					DataRow[] drs = dtL2.Select(string.Format("ParentID = '{0}' ", IDstr));
					for (int j = 0; j < drs.Length; j++)
					{
						int ParentID;
						if (drs[j]["Name"] == null || (!int.TryParse(drs[j]["ParentID"].ToString(), out ParentID)))
						{
							continue;
						}
						tn.Nodes.Add(new TreeNode(drs[j]["Name"].ToString())
						{
							Tag = drs[j]["ID"].ToString()
						});
					}

					winMain.mainFrm.treeView1.Nodes.Add(tn);
				}
			}
			#endregion
			return true;
		}

		#region 标签处理
		private void treeView1_AfterCheck(object sender, TreeViewEventArgs e)
		{
			TreeNode tn = e.Node;

			if (tn.Checked)
			{
				if (tn.Nodes.Count == 0)
				{
					DataTable dt = DataBank.Find(x => x.TableName == "level" + (tn.Level + 1));
					DataRow[] drs = dt.Select(string.Format("ID = '{0}' ", tn.Tag));
					object[] objects = new[] { true, drs[0]["ID"], drs[0]["ParentID"], drs[0]["Name"], drs[0]["DataMark"], drs[0]["DataMarkFather"], drs[0]["DataLen"], drs[0]["DataBlockType"], drs[0]["DataFormat"], drs[0]["DecimalPlaces"], "", drs[0]["DataUnit"], "", drs[0]["DefaultValue"], "" };
					if (dt_RW.Select(string.Format("ID = '{0}' ", tn.Tag)).Length == 0)
					{
						dt_RW.Rows.Add(objects);
					}
					dgv_RW.CurrentCell = dgv_RW.Rows[dgv_RW.RowCount - 1].Cells[0];
				}
				else
				{
					SelectTreeNode(tn, true);
				}
			}
			else
			{
				SelectTreeNode(tn, false);
			}
		}

		/// <summary>
		/// 选中节点后如有子节点，全部选中或全不选
		/// </summary>
		/// <param name="tn">被操作的节点</param>
		/// <param name="flag">是否选中</param>
		private void SelectTreeNode(TreeNode tn, bool flag)
		{
			for (var i = 0; i < tn.Nodes.Count; i++)
			{
				tn.Nodes[i].Checked = flag;

				if ((flag))
				{
					DataTable dt = DataBank.Find(x => x.TableName == "level2");
					DataRow[] drs = dt.Select(string.Format("ID = '{0}' ", tn.Nodes[i].Tag));
					object[] objects = new[] { true, drs[0]["ID"], drs[0]["ParentID"], drs[0]["Name"], drs[0]["DataMark"], drs[0]["DataMarkFather"], drs[0]["DataLen"], drs[0]["DataBlockType"], drs[0]["DataFormat"], drs[0]["DecimalPlaces"], "", drs[0]["DataUnit"], "", drs[0]["DefaultValue"], "" };
					if (dt_RW.Select(string.Format("ID = '{0}' ", tn.Nodes[i].Tag)).Length == 0)
					{
						dt_RW.Rows.Add(objects);
					}

					dgv_RW.CurrentCell = dgv_RW.Rows[dgv_RW.RowCount - 1].Cells[0];
				}
				if (tn.Nodes[i].Nodes.Count != 0)
				{
					SelectTreeNode(tn.Nodes[i], flag);
				}
			}
		}

		private void treeView1_NodeMouseDoubleClick(object sender, TreeNodeMouseClickEventArgs e)
		{
			TreeNode tn = e.Node;
			if (tn.Nodes.Count > 0)
			{
			}
			else
			{
				DataTable dt = DataBank.Find(x => x.TableName == "level" + (tn.Level + 1));
				DataRow[] drs = dt.Select(string.Format("ID = '{0}' ", tn.Tag));
				object[] objects = new[] { true, drs[0]["ID"], drs[0]["ParentID"], drs[0]["Name"], drs[0]["DataMark"], drs[0]["DataMarkFather"], drs[0]["DataLen"], drs[0]["DataBlockType"], drs[0]["DataFormat"], drs[0]["DecimalPlaces"], "", drs[0]["DataUnit"], "", drs[0]["DefaultValue"], "" };
				dt_RW.Rows.Add(objects);
				dgv_RW.CurrentCell = dgv_RW.Rows[dgv_RW.RowCount - 1].Cells[0];
			}
		}

		#endregion

		/// <summary>
		/// 数据分割
		/// </summary>
		/// <param name="byte[] input, int iStart, int iLen"></param>
		/// <returns></returns>
		public static byte[] FnGetPartData(byte[] input, int iStart, int iLen)
		{
			//起始长度直接超出范围了
			if (input.Length < iStart)
				return new byte[] { };

			//从0开始。。。
			if (iStart == 0 && iLen > input.Length)
				return (byte[])input.Clone();

			var iRealLen = iLen;
			if (iStart + iLen > input.Length)
				iRealLen = input.Length - iStart;

			var result = new byte[iRealLen];

			for (var i = 0; i < iRealLen; i++)
				result[i] = input[i + iStart];

			return result;
		}

		/// <summary>
		/// 智能转换：HEX码字符串到byte数组，遇到0~F以及空格以外的字符马上退出
		/// </summary>
		/// <param name="strInput"></param>
		/// <returns></returns>
		public static byte[] fnString2HEX(string strInput)
		{
			strInput = strInput.Trim().ToUpper();
			var result = new byte[strInput.Length / 2];

			var iPtr = 0;
			var bCouple = true;
			byte b = 0;
			foreach (var c in strInput)
			{
				//首先验证范围
				if ((c >= (char)'0' && c <= (char)'9') || (c >= (char)'A' && c <= (char)'F'))
				{
					b = byte.Parse(c.ToString(), System.Globalization.NumberStyles.HexNumber);
					if (bCouple)
						b *= (byte)0x10;
					result[iPtr] += b;
					bCouple = !bCouple;

					if (bCouple)
						iPtr++;
				}
				else if (c == ' ')
					continue;
				else
					break;
			}

			return FnGetPartData(result, 0, iPtr);
		}

		/// <summary>
		/// 返回68所在位置
		/// </summary>
		/// <param name="Rec"></param>
		/// <returns></returns>
		public int RecCutNum(byte[] Rec, string type)
		{
			try
			{
				if (Rec == null || Rec.Length < 1) return -1;
				if (type == "645")
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

				if (type == "376.1")
				{
					for (int i = 0; i < Rec.Length; i++)
					{
						if (Rec[i] == 0x68)
						{
							if (Rec[i + 5] == 0x68)
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

		/// <summary>
		/// 解码-33
		/// </summary>
		/// <param name="strValue">待解码数据</param>
		/// <returns></returns>
		private string fnDecodingDataMinus33(string strValue)
		{
			byte[] bTmp = new byte[strValue.Length / 2];
			bTmp = DataConvert.HEXString2Bytes(strValue);
			for (int i = 0; i < bTmp.Length; i++)
			{
				byte b = bTmp[i];
				b -= 0x33;
				bTmp[i] = b;
			}
			return DataConvert.Bytes2HexString(bTmp, 0, bTmp.Length);
		}

		private void winMain_FormClosed(object sender, FormClosedEventArgs e)
		{
			Application.Exit();
		}

		private void winMain_Paint(object sender, PaintEventArgs e)
		{
			ToolStrip tsType = (ToolStrip)sender;
			Graphics g = e.Graphics;
			//抗锯齿
			g.SmoothingMode = SmoothingMode.HighQuality;

			if (tsType is MenuStrip || tsType is ToolStrip)
			{
				//指定填充Menu栏与ToolBar栏的背景色的画刷,使用线性渐变画刷
				LinearGradientBrush lgBursh = new LinearGradientBrush(new Point(0, 0),
					new Point(0, tsType.Height),
					Color.FromArgb(252, 253, 254),
					Color.FromArgb(224, 229, 245)); // FromArgb(255, Color.White) FromArgb(225, 230,245)
				GraphicsPath path = new GraphicsPath(FillMode.Winding);
				int diameter = 10; //直径
				Rectangle rect = new Rectangle(Point.Empty, tsType.Size);
				Rectangle arcRect = new Rectangle(rect.Location, new Size(diameter, diameter));

				path.AddLine(0, 0, 10, 0);
				// 右上角
				arcRect.X = rect.Right - diameter;
				path.AddArc(arcRect, 270, 90);

				// 右下角
				arcRect.Y = rect.Bottom - diameter;
				path.AddArc(arcRect, 0, 90);

				// 左下角
				arcRect.X = rect.Left;
				path.AddArc(arcRect, 90, 90);
				path.CloseFigure();

				////设置控件的窗口区域
				tsType.Region = new Region(path);

				//填充窗口区域
				g.FillPath(lgBursh, path);
			}
		}

		private void ppg_基本参数_SelectedObjectsChanged(object sender, EventArgs e)
		{
			ppg_基本参数.Tag = ppg_基本参数.PropertySort;
			ppg_基本参数.PropertySort = PropertySort.CategorizedAlphabetical;
			ppg_基本参数.Paint += new PaintEventHandler(ppg_基本参数_Paint);
		}

		private void ppg_基本参数_Paint(object sender, PaintEventArgs e)
		{
			var categorysinfo = ppg_基本参数.SelectedObject.GetType().GetField("categorys", BindingFlags.NonPublic | BindingFlags.Instance);
			if (categorysinfo != null)
			{
				var categorys = categorysinfo.GetValue(ppg_基本参数.SelectedObject) as List<String>;
				ppg_基本参数.CollapseAllGridItems();
				GridItemCollection currentPropEntries =
					ppg_基本参数.GetType().GetField("currentPropEntries", BindingFlags.NonPublic | BindingFlags.Instance).GetValue(ppg_基本参数) as GridItemCollection;
				var newarray = currentPropEntries.Cast<GridItem>().OrderBy((t) => categorys.IndexOf(t.Label)).ToArray();
				currentPropEntries.GetType().GetField("entries", BindingFlags.NonPublic | BindingFlags.Instance).SetValue(currentPropEntries, newarray);
				ppg_基本参数.ExpandAllGridItems();
				ppg_基本参数.PropertySort = (PropertySort)ppg_基本参数.Tag;
			}

			ppg_基本参数.Paint -= new PaintEventHandler(ppg_基本参数_Paint);

		}

		private void btn_电表通信串口_Click(object sender, EventArgs e)
		{
			if (btn_电表通信串口.Text == "打开串口")
			{
				if (string.IsNullOrEmpty(pmc[0].Value.ToString())) return;
				if (!int.TryParse(pmc[1].Value.ToString(), out int baudRate))
				{
					MessageBox.Show("打开串口失败：非法的波特率值！");
					return;
				}

				if (string.IsNullOrEmpty(pmc[2].Value.ToString())) return;
				if (!int.TryParse(pmc[3].Value.ToString(), out int dataBit))
				{
					MessageBox.Show("打开串口失败：非法的数据位！");
					return;
				}

				if (string.IsNullOrEmpty(pmc[4].Value.ToString())) return;
				if (!int.TryParse(pmc[5].Value.ToString(), out int overTime))
				{
					MessageBox.Show("打开串口失败：非法的通信超时时间！");
					return;
				}

				if (SpMeter.setSerialPort(pmc[0].Value.ToString(), baudRate, pmc[2].Value.ToString(), dataBit, pmc[4].Value.ToString(), overTime))
				{
					btn_电表通信串口.Text = "关闭串口";
					SpMeter.OverTime = overTime;
					ini.IniWriteValue("Main", "ComPortName", pmc[0].Value.ToString());
					ini.IniWriteValue("Main", "BaudRate", pmc[1].Value.ToString());
					ini.IniWriteValue("Main", "Parity", pmc[2].Value.ToString());
					ini.IniWriteValue("Main", "DataBits", pmc[3].Value.ToString());
					ini.IniWriteValue("Main", "stopBits", pmc[4].Value.ToString());
					ini.IniWriteValue("Main", "OverTime", pmc[5].Value.ToString());
					ini.IniWriteValue("Main", "PasswordLevel", pmc[7].Value.ToString());
					ini.IniWriteValue("Main", "Password", pmc[8].Value.ToString());
					ini.IniWriteValue("Main", "Mins", pmc[9].Value.ToString());
					ini.IniWriteValue("Main", "Period", pmc[10].Value.ToString());
					ini.IniWriteValue("Main", "SendThenDelay", pmc[11].Value.ToString());
				}
				else
				{
					MessageBox.Show("打开串口失败！");
				}
			}
			else
			{
				SpMeter.closePort();
				btn_电表通信串口.Text = "打开串口";
				cbMoniter.CheckState = CheckState.Unchecked;
			}
		}

		#region 用于显示
		int currentRows = 0; //行数
		int delIdx = 0; //删除开始的位置

		private delegate void delInfoList1(string text, bool warn, bool haveTime);

		/// <summary>
		/// AppearMessage&PrintLog
		/// </summary>
		/// <param name="log"></param>
		/// <param name="warn"></param>
		/// <param name="haveTime"></param>
		void output(string log, bool warn = false, bool haveTime = false)
		{
			if (rtb_log.InvokeRequired)
			{
				var d = new delInfoList1(output);
				try
				{
					rtb_log.Invoke(d, log, warn, haveTime);
				}
				catch (Exception)
				{
					throw;
				}
			}
			else
			{
				currentRows++; //显示行数，最多1000行
				if (currentRows == 1000)
				{
					delIdx = rtb_log.TextLength;
				}
				else
				{
					if (currentRows == 1000 + 20)
					{
						rtb_log.Select(0, delIdx);
						rtb_log.SelectedText = "";
						currentRows = 20;
					}
				}
				var len1 = rtb_log.TextLength;
				//rtb_log.Focus();                   
				//rtb_log.Select(rtb_log.TextLength, 0);                   
				rtb_log.ScrollToCaret();
				var text = "";
				if (haveTime)
					text = DateTime.Now.ToString("HH:mm:ss:fff ") + log + "\r\n";
				else
					text = "====================" + log + "====================" + "\r\n";
				rtb_log.AppendText(text);
				//WriteLog
				WriteLog.Write(text);
				if (warn)
				{
					rtb_log.Select(len1, text.Length);
					rtb_log.SelectionColor = Color.Red;
				}
			}
		}
		#endregion

		private void cb_loop_SelectedIndexChanged(object sender, EventArgs e)
		{
			loopSelIndex = cb_loop.SelectedIndex;
		}

		public string AddStr
		{
			get { return Helps.ByteToStringWithNoSpace(Add); }
		}

		byte[] Add
		{
			get
			{
				byte[] _Add = new byte[] { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };
				if (!Helps.TryStrToHexByte(Helps.fnAgainst(addr.Value.ToString()), out _Add)) { return _Add; }
				if (is多回路)
				{
					switch (loopSelIndex)
					{
						case 2:
						case 3:
						case 4:
							_Add[0] = (byte)(_Add[0] - _Add[0] % 0x10 + loopSelIndex);
							break;
						default:
							_Add[0] = (byte)(_Add[0] - _Add[0] % 0x10 + 1);
							break;
					}
				}
				else
				{

				}

				return _Add;
			}
			set
			{
				Array.Reverse(value);
				addr.Value = Helps.ByteToStringWithNoSpace(value);
			}
		}

		private void btn_读瞬时量_Click(object sender, EventArgs e)
		{
			if (!SpMeter.isOpen()) return;
			if (SpMeter.Working) return;
			byte[] cmd2 = new byte[100];
			byte[] Rec2 = new byte[100];
			int index2 = 0;
			Thread objThread = new Thread(new ThreadStart(delegate
			{
				if ((string)addr.Value == "AAAAAAAAAAAA")
				{
					#region 读表地址
					cmd2 = fnString2HEX("68 AA AA AA AA AA AA 68 11 04 34 37 33 37 B6 16");
					output("发送： " + SerialTool.ByteToString(cmd2), false, true);
					Rec2 = SpMeter.SendCommand(cmd2, "645");
					output("接收： " + SerialTool.ByteToString(Rec2), false, true);
					if (Rec2 == null)
					{
						output("收到报文为空，将重发1次", true, true);
						output("发送： " + SerialTool.ByteToString(cmd2), false, true);
						Rec2 = SpMeter.SendCommand(cmd2, "645");
						output("接收： " + SerialTool.ByteToString(Rec2), false, true);
						if (Rec2 == null)
						{
							output("收到报文为空，请检查", true, true);
							AutoSend = false;
							return;
						}
					}

					index2 = RecCutNum(Rec2, "645");

					if (index2 == -1)
					{
						output("收到的报文有误，请检查", true, true);
						AutoSend = false;
						return;
					}

					if (Rec2[8 + index2] != 0x91)
					{
						output("设置失败，请检查", true, true);
						AutoSend = false;
						return;
					}
					#endregion
				}

				//复制表号
				Array.Copy(Rec2, index2 + 1, Add, 0, 6);
				if (loopSelIndex != 0)//读取全部回路
				{
					#region 读参数
					cmd2 = fnString2HEX("68 01 00 00 00 00 00 68 11 08 0E 12 12 37 11 48 12 0E");
					Array.Copy(Add, 0, cmd2, 1, 6);
					switch (loopSelIndex)
					{
						case 1:
							cmd2[1] = (byte)(cmd2[1] - cmd2[1] % 0x10 + 1);
							break;
						case 2:
							cmd2[1] = (byte)(cmd2[1] - cmd2[1] % 0x10 + 2);
							break;
						case 3:
							cmd2[1] = (byte)(cmd2[1] - cmd2[1] % 0x10 + 3);
							break;
						case 4:
							cmd2[1] = (byte)(cmd2[1] - cmd2[1] % 0x10 + 4);
							break;
					}
					string cmd2_str = SerialTool.ByteToString(cmd2);
					cmd2_str = cmd2_str + DataConvert.fnGetCS(cmd2_str).ToString("X2") + " 16";
					output("发送： " + cmd2_str, false, true);
					cmd2 = fnString2HEX(cmd2_str);
					byte[] Rec3 = SpMeter.SendCommand(cmd2, "645");
					output("接收： " + SerialTool.ByteToString(Rec3), false, true);
					if (Rec3 == null)
					{
						output("收到报文为空，将重发1次", true, true);
						output("发送： " + SerialTool.ByteToString(cmd2), false, true);
						Rec3 = SpMeter.SendCommand(cmd2, "645");
						output("接收： " + SerialTool.ByteToString(Rec3), false, true);
						if (Rec3 == null)
						{
							output("收到报文为空，请检查", true, true);
							AutoSend = false;
							return;
						}
					}

					index2 = RecCutNum(Rec3, "645");

					if (index2 == -1)
					{
						output("收到的报文有误，请检查", true, true);
						AutoSend = false;
						return;
					}

					if (Rec3[8 + index2] != 0x91)
					{
						output("读取失败，请检查", true, true);
						AutoSend = false;
						return;
					}
					#endregion

					#region 处理数据
					int Datalenth = Rec3[9 + index2];
					byte[] Data = new byte[Datalenth];
					byte[] bData;
					int dwData;
					Array.Copy(Rec3, index2 + 10, Data, 0, Datalenth);
					//减33
					for (int i = 0; i < Datalenth; i++)
					{
						Data[i] -= 0x33;
					}

					string DataStr = SerialTool.byteToHexStr(Data);
					//去掉数据标识
					DataStr = DataStr.Substring(12);
					Invoke((EventHandler)delegate
					{
						电压.Text = Changelan(DataStr.Substring(0, 2 * 2)).Insert(3, ".") + ";" + Changelan(DataStr.Substring(4, 2 * 2)).Insert(3, ".") + ";" + Changelan(DataStr.Substring(8, 2 * 2)).Insert(3, ".");
					});

					DataStr = DataStr.Substring(12);

					#region 4组重复组合
					switch (loopSelIndex)
					{
						case 1:
							Invoke((EventHandler)delegate
							{
								电流1.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(8, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(16, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(24, 2 * 4)), 5);
								DataStr = DataStr.Substring(32);

								有功功率1.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 4);
								DataStr = DataStr.Substring(8);

								无功功率1.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 4);
								DataStr = DataStr.Substring(8);

								功率因数1.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 2)), 1);
								DataStr = DataStr.Substring(4);
							});
							if (isHex解析电能)
							{
								bData = DataConvert.HEXString2Bytes(Changelan(DataStr.Substring(0, 2 * 4)));
								dwData = (int)bData[0] * 0x1000000 + bData[1] * 0x10000 + bData[2] * 0x100 + bData[3];
								Invoke((EventHandler)delegate
								{
									组合有功电能1.Text = (dwData / 100).ToString() + "." + System.Math.Abs(dwData % 100);
								});
								DataStr = DataStr.Substring(8);

								bData = DataConvert.HEXString2Bytes(Changelan(DataStr.Substring(0, 2 * 4)));
								dwData = (int)bData[0] * 0x1000000 + bData[1] * 0x10000 + bData[2] * 0x100 + bData[3];
								Invoke((EventHandler)delegate
								{
									组合无功11.Text = (dwData / 100).ToString() + "." + System.Math.Abs(dwData % 100);
								});
								DataStr = DataStr.Substring(8);
							}
							else
							{
								Invoke((EventHandler)delegate
								{
									组合有功电能1.Text = Changelan(DataStr.Substring(0, 2 * 4)).Insert(6, ".");
								});
								DataStr = DataStr.Substring(8);
								Invoke((EventHandler)delegate
								{
									组合无功11.Text = Changelan(DataStr.Substring(0, 2 * 4)).Insert(6, ".");
								});
								DataStr = DataStr.Substring(8);
							}
							break;
						case 2:
							Invoke((EventHandler)delegate
							{
								电流2.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(8, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(16, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(24, 2 * 4)), 5);
							});
							DataStr = DataStr.Substring(32);
							Invoke((EventHandler)delegate
							{
								有功功率2.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 4);
							});
							DataStr = DataStr.Substring(8);
							Invoke((EventHandler)delegate
							{
								无功功率2.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 4);
							});
							DataStr = DataStr.Substring(8);
							Invoke((EventHandler)delegate
							{
								功率因数2.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 2)), 1);
							});
							DataStr = DataStr.Substring(4);

							if (isHex解析电能 == true)
							{
								bData = DataConvert.HEXString2Bytes(Changelan(DataStr.Substring(0, 2 * 4)));
								dwData = (int)bData[0] * 0x1000000 + bData[1] * 0x10000 + bData[2] * 0x100 + bData[3];
								Invoke((EventHandler)delegate
								{
									组合有功电能2.Text = (dwData / 100).ToString() + "." + System.Math.Abs(dwData % 100);
								});
								DataStr = DataStr.Substring(8);

								bData = DataConvert.HEXString2Bytes(Changelan(DataStr.Substring(0, 2 * 4)));
								dwData = (int)bData[0] * 0x1000000 + bData[1] * 0x10000 + bData[2] * 0x100 + bData[3];
								Invoke((EventHandler)delegate
								{
									组合无功12.Text = (dwData / 100).ToString() + "." + System.Math.Abs(dwData % 100);
								});
								DataStr = DataStr.Substring(8);
							}
							else
							{
								Invoke((EventHandler)delegate
								{
									组合有功电能2.Text = Changelan(DataStr.Substring(0, 2 * 4)).Insert(6, ".");
								});
								DataStr = DataStr.Substring(8);
								Invoke((EventHandler)delegate
								{
									组合无功12.Text = Changelan(DataStr.Substring(0, 2 * 4)).Insert(6, ".");
								});
								DataStr = DataStr.Substring(8);
							}
							break;
						case 3:
							Invoke((EventHandler)delegate
							{
								电流3.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(8, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(16, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(24, 2 * 4)), 5);
								DataStr = DataStr.Substring(32);

								有功功率3.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 4);
								DataStr = DataStr.Substring(8);

								无功功率3.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 4);
								DataStr = DataStr.Substring(8);

								功率因数3.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 2)), 1);
								DataStr = DataStr.Substring(4);

								if (HexCheckBox.Checked == true)
								{
									bData = DataConvert.HEXString2Bytes(Changelan(DataStr.Substring(0, 2 * 4)));
									dwData = (int)bData[0] * 0x1000000 + bData[1] * 0x10000 + bData[2] * 0x100 + bData[3];
									组合有功电能3.Text = (dwData / 100).ToString() + "." + System.Math.Abs(dwData % 100);
									DataStr = DataStr.Substring(8);

									bData = DataConvert.HEXString2Bytes(Changelan(DataStr.Substring(0, 2 * 4)));
									dwData = (int)bData[0] * 0x1000000 + bData[1] * 0x10000 + bData[2] * 0x100 + bData[3];
									组合无功13.Text = (dwData / 100).ToString() + "." + System.Math.Abs(dwData % 100);
									DataStr = DataStr.Substring(8);
								}
								else
								{
									组合有功电能3.Text = Changelan(DataStr.Substring(0, 2 * 4)).Insert(6, ".");
									DataStr = DataStr.Substring(8);

									组合无功13.Text = Changelan(DataStr.Substring(0, 2 * 4)).Insert(6, ".");
									DataStr = DataStr.Substring(8);
								}
							});
							break;
						case 4:
							Invoke((EventHandler)delegate
							{
								电流4.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(8, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(16, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(24, 2 * 4)), 5);
								DataStr = DataStr.Substring(32);

								有功功率4.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 4);
								DataStr = DataStr.Substring(8);

								无功功率4.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 4);
								DataStr = DataStr.Substring(8);

								功率因数4.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 2)), 1);
								DataStr = DataStr.Substring(4);

								if (isHex解析电能 == true)
								{
									bData = DataConvert.HEXString2Bytes(Changelan(DataStr.Substring(0, 2 * 4)));
									dwData = (int)bData[0] * 0x1000000 + bData[1] * 0x10000 + bData[2] * 0x100 + bData[3];
									组合有功电能4.Text = (dwData / 100).ToString() + "." + System.Math.Abs(dwData % 100);
									DataStr = DataStr.Substring(8);

									bData = DataConvert.HEXString2Bytes(Changelan(DataStr.Substring(0, 2 * 4)));
									dwData = (int)bData[0] * 0x1000000 + bData[1] * 0x10000 + bData[2] * 0x100 + bData[3];
									组合无功14.Text = (dwData / 100).ToString() + "." + System.Math.Abs(dwData % 100);
									DataStr = DataStr.Substring(8);
								}
								else
								{
									组合有功电能4.Text = Changelan(DataStr.Substring(0, 2 * 4)).Insert(6, ".");
									DataStr = DataStr.Substring(8);

									组合无功14.Text = Changelan(DataStr.Substring(0, 2 * 4)).Insert(6, ".");
									DataStr = DataStr.Substring(8);
								}
							});
							break;
					}
					#endregion

					#endregion

					this.Invoke((EventHandler)(delegate
					{
						互感器状态.Text = Changelan(DataStr.Substring(0, 2 * 1));
						DataStr = DataStr.Substring(2);

						遥信状态.Text = Changelan(DataStr.Substring(0, 2 * 1));
						DataStr = DataStr.Substring(2);

						停电断相状态.Text = Changelan(DataStr.Substring(0, 2 * 1));
						DataStr = DataStr.Substring(2);
					}));
				}
				else
				{
					#region 读参数
					cmd2 = fnString2HEX("68 01 00 00 00 00 00 68 11 08 0E 12 12 37 12 48 12 0E");
					Array.Copy(Add, 0, cmd2, 1, 6);
					string cmd2_str = SerialTool.ByteToString(cmd2);
					cmd2_str = cmd2_str + DataConvert.fnGetCS(cmd2_str).ToString("X2") + " 16";
					output("发送： " + cmd2_str, false, true);
					cmd2 = fnString2HEX(cmd2_str);
					byte[] Rec3 = SpMeter.SendCommand(cmd2, "645");
					output("接收： " + SerialTool.ByteToString(Rec3), false, true);
					if (Rec3 == null)
					{
						output("收到报文为空，将重发1次", true, true);
						output("发送： " + SerialTool.ByteToString(cmd2), false, true);
						Rec3 = SpMeter.SendCommand(cmd2, "645");
						output("接收： " + SerialTool.ByteToString(Rec3), false, true);
						if (Rec3 == null)
						{
							output("收到报文为空，请检查", true, true);
							AutoSend = false;
							return;
						}
					}
					index2 = RecCutNum(Rec3, "645");

					if (index2 == -1)
					{
						output("收到的报文有误，请检查", true, true);
						AutoSend = false;
						return;
					}

					if (Rec3[8 + index2] != 0x91)
					{
						output("读取失败，请检查", true, true);
						AutoSend = false;
						return;
					}
					#endregion

					#region 处理数据
					int Datalenth = Rec3[9 + index2];
					byte[] Data = new byte[Datalenth];
					byte[] bData;
					int dwData;
					Array.Copy(Rec3, index2 + 10, Data, 0, Datalenth);
					//减33
					for (int i = 0; i < Datalenth; i++)
					{
						Data[i] -= 0x33;
					}
					string DataStr = SerialTool.byteToHexStr(Data);
					//去掉数据标识
					DataStr = DataStr.Substring(12);
					this.Invoke((EventHandler)(delegate
					{
						电压.Text = Changelan(DataStr.Substring(0, 2 * 2)).Insert(3, ".") + ";" + Changelan(DataStr.Substring(4, 2 * 2)).Insert(3, ".") + ";" + Changelan(DataStr.Substring(8, 2 * 2)).Insert(3, ".");
						DataStr = DataStr.Substring(12);

						电流1.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(8, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(16, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(24, 2 * 4)), 5);
						DataStr = DataStr.Substring(32);

						有功功率1.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 4);
						DataStr = DataStr.Substring(8);

						无功功率1.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 4);
						DataStr = DataStr.Substring(8);

						功率因数1.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 2)), 1);
					}));
					DataStr = DataStr.Substring(4);

					if (isHex解析电能 == true)
					{
						bData = DataConvert.HEXString2Bytes(Changelan(DataStr.Substring(0, 2 * 4)));
						dwData = (int)bData[0] * 0x1000000 + bData[1] * 0x10000 + bData[2] * 0x100 + bData[3];
						this.Invoke((EventHandler)(delegate
						{
							组合有功电能1.Text = (dwData / 100).ToString() + "." + System.Math.Abs(dwData % 100);
						}));
						DataStr = DataStr.Substring(8);

						bData = DataConvert.HEXString2Bytes(Changelan(DataStr.Substring(0, 2 * 4)));
						dwData = (int)bData[0] * 0x1000000 + bData[1] * 0x10000 + bData[2] * 0x100 + bData[3];
						this.Invoke((EventHandler)(delegate
						{
							组合无功11.Text = (dwData / 100).ToString() + "." + System.Math.Abs(dwData % 100);
						}));
						DataStr = DataStr.Substring(8);
					}
					else
					{
						this.Invoke((EventHandler)(delegate
						{
							组合有功电能1.Text = Changelan(DataStr.Substring(0, 2 * 4)).Insert(6, ".");
						}));
						DataStr = DataStr.Substring(8);
						this.Invoke((EventHandler)(delegate
						{
							组合无功11.Text = Changelan(DataStr.Substring(0, 2 * 4)).Insert(6, ".");
						}));
						DataStr = DataStr.Substring(8);
					}
					this.Invoke((EventHandler)(delegate
					{
						电流2.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(8, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(16, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(24, 2 * 4)), 5);
					}));
					DataStr = DataStr.Substring(32);
					this.Invoke((EventHandler)(delegate
					{
						有功功率2.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 4);
					}));
					DataStr = DataStr.Substring(8);
					this.Invoke((EventHandler)(delegate
					{
						无功功率2.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 4);
					}));
					DataStr = DataStr.Substring(8);
					this.Invoke((EventHandler)(delegate
					{
						功率因数2.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 2)), 1);
					}));
					DataStr = DataStr.Substring(4);

					if (isHex解析电能 == true)
					{
						bData = DataConvert.HEXString2Bytes(Changelan(DataStr.Substring(0, 2 * 4)));
						dwData = (int)bData[0] * 0x1000000 + bData[1] * 0x10000 + bData[2] * 0x100 + bData[3];
						this.Invoke((EventHandler)(delegate
						{
							组合有功电能2.Text = (dwData / 100).ToString() + "." + System.Math.Abs(dwData % 100);
						}));
						DataStr = DataStr.Substring(8);

						bData = DataConvert.HEXString2Bytes(Changelan(DataStr.Substring(0, 2 * 4)));
						dwData = (int)bData[0] * 0x1000000 + bData[1] * 0x10000 + bData[2] * 0x100 + bData[3];
						this.Invoke((EventHandler)(delegate
						{
							组合无功12.Text = (dwData / 100).ToString() + "." + System.Math.Abs(dwData % 100);
						}));
						DataStr = DataStr.Substring(8);
					}
					else
					{
						this.Invoke((EventHandler)(delegate
						{
							组合有功电能2.Text = Changelan(DataStr.Substring(0, 2 * 4)).Insert(6, ".");
						}));
						DataStr = DataStr.Substring(8);
						this.Invoke((EventHandler)(delegate
						{
							组合无功12.Text = Changelan(DataStr.Substring(0, 2 * 4)).Insert(6, ".");
						}));
						DataStr = DataStr.Substring(8);
					}
					this.Invoke((EventHandler)(delegate
					{
						电流3.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(8, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(16, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(24, 2 * 4)), 5);
					}));
					DataStr = DataStr.Substring(32);
					this.Invoke((EventHandler)(delegate
					{
						有功功率3.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 4);
					}));
					DataStr = DataStr.Substring(8);
					this.Invoke((EventHandler)(delegate
					{
						无功功率3.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 4);
					}));
					DataStr = DataStr.Substring(8);
					this.Invoke((EventHandler)(delegate
					{
						功率因数3.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 2)), 1);
					}));
					DataStr = DataStr.Substring(4);

					if (isHex解析电能 == true)
					{
						bData = DataConvert.HEXString2Bytes(Changelan(DataStr.Substring(0, 2 * 4)));
						dwData = (int)bData[0] * 0x1000000 + bData[1] * 0x10000 + bData[2] * 0x100 + bData[3];
						this.Invoke((EventHandler)(delegate
						{
							组合有功电能3.Text = (dwData / 100).ToString() + "." + System.Math.Abs(dwData % 100);
						}));
						DataStr = DataStr.Substring(8);

						bData = DataConvert.HEXString2Bytes(Changelan(DataStr.Substring(0, 2 * 4)));
						dwData = (int)bData[0] * 0x1000000 + bData[1] * 0x10000 + bData[2] * 0x100 + bData[3];
						this.Invoke((EventHandler)(delegate
						{
							组合无功13.Text = (dwData / 100).ToString() + "." + System.Math.Abs(dwData % 100);
						}));
						DataStr = DataStr.Substring(8);
					}
					else
					{
						this.Invoke((EventHandler)(delegate
						{
							组合有功电能3.Text = Changelan(DataStr.Substring(0, 2 * 4)).Insert(6, ".");
						}));
						DataStr = DataStr.Substring(8);

						this.Invoke((EventHandler)(delegate
						{
							组合无功13.Text = Changelan(DataStr.Substring(0, 2 * 4)).Insert(6, ".");
						}));
						DataStr = DataStr.Substring(8);
					}
					this.Invoke((EventHandler)(delegate
					{
						电流4.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(8, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(16, 2 * 4)), 5) + ";" + CheckIfFushu(Changelan(DataStr.Substring(24, 2 * 4)), 5);
					}));
					DataStr = DataStr.Substring(32);
					this.Invoke((EventHandler)(delegate
					{
						有功功率4.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 4);
					}));
					DataStr = DataStr.Substring(8);
					this.Invoke((EventHandler)(delegate
					{
						无功功率4.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 4)), 4);
					}));
					DataStr = DataStr.Substring(8);
					this.Invoke((EventHandler)(delegate
					{
						功率因数4.Text = CheckIfFushu(Changelan(DataStr.Substring(0, 2 * 2)), 1);
					}));
					DataStr = DataStr.Substring(4);

					if (isHex解析电能 == true)
					{
						bData = DataConvert.HEXString2Bytes(Changelan(DataStr.Substring(0, 2 * 4)));
						dwData = (int)bData[0] * 0x1000000 + bData[1] * 0x10000 + bData[2] * 0x100 + bData[3];
						this.Invoke((EventHandler)(delegate
						{
							组合有功电能4.Text = (dwData / 100).ToString() + "." + System.Math.Abs(dwData % 100);
						}));
						DataStr = DataStr.Substring(8);

						bData = DataConvert.HEXString2Bytes(Changelan(DataStr.Substring(0, 2 * 4)));
						dwData = (int)bData[0] * 0x1000000 + bData[1] * 0x10000 + bData[2] * 0x100 + bData[3];
						this.Invoke((EventHandler)(delegate
						{
							组合无功14.Text = (dwData / 100).ToString() + "." + System.Math.Abs(dwData % 100);
						}));
						DataStr = DataStr.Substring(8);
					}
					else
					{
						this.Invoke((EventHandler)(delegate
						{
							组合有功电能4.Text = Changelan(DataStr.Substring(0, 2 * 4)).Insert(6, ".");
						}));
						DataStr = DataStr.Substring(8);
						this.Invoke((EventHandler)(delegate
						{
							组合无功14.Text = Changelan(DataStr.Substring(0, 2 * 4)).Insert(6, ".");
						}));
						DataStr = DataStr.Substring(8);
					}
					#endregion
					this.Invoke((EventHandler)(delegate
					{
						互感器状态.Text = Changelan(DataStr.Substring(0, 2 * 2));
						DataStr = DataStr.Substring(4);

						遥信状态.Text = Changelan(DataStr.Substring(0, 2 * 1));
						DataStr = DataStr.Substring(2);

						停电断相状态.Text = Changelan(DataStr.Substring(0, 2 * 1));
						DataStr = DataStr.Substring(2);
					}));
				}
			}));
			objThread.Start();

		}

		//负数加“-”和显示
		private string CheckIfFushu(string Data, int dot)
		{
			byte[] fth = fnString2HEX(Data);
			if ((fth[0] & 0x80) > 0)
			{
				Data = "-" + (fth[0] & 0x7f).ToString("X2") + Data.Substring(2, Data.Length - 2);
				Data = Data.Insert(dot + 1, ".");
			}
			else
			{
				Data = Data.Insert(dot, ".");
			}
			return Data;
		}

		//按位倒序数组顺序
		private string Changelan(string Data)
		{
			string temp = "";
			if (Data.Length == 2)
			{
				return Data;
			}
			if (Data.Length % 4 != 0)
			{
				return "";
			}
			for (int i = 0; i < Data.Length / 2; i++)
			{
				temp += Data.Substring(Data.Length - i * 2 - 2, 2);
			}

			return temp;
		}

		private bool isHex解析电能 = true;
		private void HexCheckBox_CheckedChanged(object sender, EventArgs e)
		{
			isHex解析电能 = HexCheckBox.Checked;
		}

		private void dgv_RW_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
		{
			if (e.RowIndex == -1)
			{
				return;
			}
			else if (e.ColumnIndex == -1)
			{
				return;
			}
			string dataDisplay = "";
			if (dgv_RW.Columns[e.ColumnIndex].DataPropertyName == "读取值")
			{
				dgv_RW.EndEdit();
				if (dt_RW.Rows[e.RowIndex]["读取值"] == null || dt_RW.Rows[e.RowIndex]["DataBlockType"] == null)
				{
					return;
				}
				string dataMark = dgv_RW.Rows[e.RowIndex].Cells["DataMark"].Value?.ToString();
				if (dataMark == "04000601")
				{
					ActComChaParser activeComCha = new ActComChaParser();
					string obj = dt_RW.Rows[e.RowIndex]["读取值"].ToString();

					activeComCha.parse(obj);

					if (activeComCha.ShowDialog() == DialogResult.OK)
					{
						dt_RW.Rows[e.RowIndex]["设置值"] = activeComCha.GenerateFeatureByte().ToString("X2");
						dgv_RW.Refresh();
					}
					else
					{
						timer.Stop();
					}

				}
			}
			if (dgv_RW.Columns[e.ColumnIndex].DataPropertyName == "设置值")
			{
				if (dt_RW.Rows[e.RowIndex]["DataBlockType"] == null)
				{
					return;
				}
				dataDisplay = dt_RW.Rows[e.RowIndex]["设置值"].ToString();
				string s = dgv_RW.Columns[4].DataPropertyName;
				if ((dgv_RW.Rows[e.RowIndex].Cells["DataMark"].Value.ToString() == "04020103") || (dgv_RW.Rows[e.RowIndex].Cells["DataMark"].Value.ToString() == "04020102") || (dgv_RW.Rows[iTimerRowIndex].Cells["datamark"].Value.ToString().Trim() == "0400010C"))
				{
					ClockForm cfClockForm = new ClockForm();
					iTimerRowIndex = e.RowIndex;
					if (cfClockForm.ShowDialog() == DialogResult.OK)
					{
						timer.Start();
					}
					else
					{
						timer.Stop();
					}
				}
			}

			string KuaiStr = dt_RW.Rows[e.RowIndex]["DataBlockType"].ToString();
			DataTable dtDisplay = new DataTable();
			if (KuaiStr.Contains("-"))
			{
				string[] startAndEndInt = KuaiStr.Split(new[] { "-" }, StringSplitOptions.RemoveEmptyEntries);
				if (startAndEndInt.Length != 2)
				{
					return;
				}
				DataRow[] drs = dtL2.Select(string.Format("ID >= {0} and ID <= {1}", startAndEndInt[0], startAndEndInt[1]));
				//DataRow[] drs = dtL2.Select(string.Format("ID <= {0} ", startAndEndInt[0]));
				if (drs.Length < 2)
				{
					return;
				}

				dtDisplay.Columns.Add("Name", typeof(string));
				dtDisplay.Columns.Add("DataLen", typeof(string));
				dtDisplay.Columns.Add("DataFormat", typeof(string));
				dtDisplay.Columns.Add("DecimalPlaces", typeof(string));//TODO;调整为int，需要调整数据库中
				dtDisplay.Columns.Add("DataUnit", typeof(string));
				dtDisplay.Columns.Add("Value", typeof(string));
				for (int i = 0; i < drs.Length; i++)
				{
					dtDisplay.Rows.Add(new object[] { drs[i]["Name"], drs[i]["DataLen"], drs[i]["DataFormat"], drs[i]["DecimalPlaces"], drs[i]["DataUnit"], "" });
				}
			}
			else
			{
				if (KuaiStr == "")
				{
					return;
				}
				dtDisplay = DataBank.Find(x => x.TableName == KuaiStr).Copy();
				if (dtDisplay == null)
				{
					return;
				}
			}
			Form_DisplayDataBlock displayDataBlock = new Form_DisplayDataBlock(dtDisplay, dataDisplay) { StartPosition = FormStartPosition.CenterScreen };
			if (displayDataBlock.ShowDialog() == DialogResult.OK)
			{
				dt_RW.Rows[e.RowIndex]["设置值"] = Form_DisplayDataBlock.setDataStr;
				dgv_RW.Refresh();
				displayDataBlock.Dispose();
			}
		}

		private void timer_Tick(object sender, EventArgs e)
		{
			if (dgv_RW.Rows.Count > 0)
			{
				if (iTimerRowIndex >= dgv_RW.Rows.Count)
				{
					timer.Stop();
					return;
				}

				if ((dgv_RW.Rows[iTimerRowIndex].Cells["datamark"].Value.ToString().Trim() == "04020103") || (dgv_RW.Rows[iTimerRowIndex].Cells["datamark"].Value.ToString().Trim() == "0400010C"))
				{
					dgv_RW.Rows[iTimerRowIndex].Cells["设置值"].Value =
						DateTime.Now.ToString("yyMMdd") + ((int)DateTime.Now.DayOfWeek).ToString("X2") + DateTime.Now.ToString("HHmmss");
				}
				else if (dgv_RW.Rows[iTimerRowIndex].Cells["DataMark"].Value.ToString() == "04020102")
				{
					dgv_RW.Rows[iTimerRowIndex].Cells["设置值"].Value = DateTime.Now.ToString("HHmmss");
				}
				else
				{
					timer.Enabled = false;
				}
			}
		}

		private void btn_抄读_Click(object sender, EventArgs e)
		{
			//double d1 = -0.217;
			//double d2 = Math.Truncate(100 * d1) / 100;


			if (txcbBLE.CheckState == CheckState.Unchecked)
			{
				if (btn_电表通信串口.Text == "打开串口")
				{
					MessageBox.Show("请先打开串口!");
					return;
				}

				if (SpMeter.Working)
				{
					return;
				}

				try
				{
					DataRow[] drs = dt_RW.Select(string.Format("选择 = '{0}' ", "true"), "");
					int rowCount = drs.GetLength(0);
					if (rowCount < 1)
					{
						return;
					}

					SpMeter.Working = true;
					//drs = drs.OrderBy(x => dt_RW.Rows.IndexOf(x)).ToArray();

					#region 抄读
					bool isReadingErr = false;
					DLT645 dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
					for (int i = 0; i < rowCount && SpMeter.Working; i++)
					{
						List<string> dataMarkList = new List<string>();
						byte seq = 0;
						string dataResultStr = "";

						if (drs[i]["DataMark"] == null || drs[i]["DataMark"].ToString().Length != 8)
						{
							drs[i]["读取值"] = "";
							drs[i]["操作结论"] = "抄读失败：数据标识非法！";
							continue;
						}

						string dataMark = drs[i]["DataMark"].ToString();
						dataMarkList.Add(dataMark);
						if (drs[i]["DataMarkFather"] != null && drs[i]["DataMarkFather"].ToString().Length == 8)
						{
							dataMarkList.Add(drs[i]["DataMarkFather"].ToString());
						}

						byte[] sendBytes = null;
						string errStr = "";
						if (!txcEncrypt.Checked)
						{
							if (!dlt645.Pack(DLT645.ControlType.Read, dataMarkList, "", 0, out sendBytes, out errStr))
							{
								drs[i]["读取值"] = "";
								drs[i]["操作结论"] = errStr;
								continue;
							}
						}
						else
						{
							if (!dlt645.Pack(DLT645.ControlType.SafetyReadLTU, dataMarkList, "", 0, out sendBytes, out errStr))
							{
								drs[i]["读取值"] = "";
								drs[i]["操作结论"] = errStr;
								continue;
							}
						}

						string strAPDU = "";
						byte[] baAPDU;
						byte[] baTemp;
						byte[] recBytes;
						if (cb698.Checked)
						{
							strAPDU = "090700f2090201060208010000640064" + sendBytes.Length.ToString("X2");
							Helps.TryStrToHexByte(strAPDU, out baAPDU);
							baTemp = new byte[sendBytes.Length];
							baTemp = sendBytes;
							sendBytes = new byte[baTemp.Length + baAPDU.Length + 1];
							baAPDU.CopyTo(sendBytes, 0);
							baTemp.CopyTo(sendBytes, baAPDU.Length);
							baTemp = new byte[1] { 00 };
							baTemp.CopyTo(sendBytes, sendBytes.Length - 1);

							strAPDU = "68" + Helps.fnAgainst((15 + sendBytes.Length).ToString("X4")) + "4345" + Helps.fnAgainst("AAAAAAAAAAAA") + "00";
							Helps.TryStrToHexByte(strAPDU, out baAPDU);
							ushort uCountFCS16 = WSDFCS16.CountFCS16(baAPDU, 1, baAPDU.Length - 1);
							baTemp = new byte[2];
							Helps.TryStrToHexByte(uCountFCS16.ToString("X4"), out baTemp);
							baAPDU = baAPDU.Concat(baTemp).ToArray();
							baAPDU = baAPDU.Concat(sendBytes).ToArray();

							uCountFCS16 = WSDFCS16.CountFCS16(baAPDU, 1, baAPDU.Length - 1);
							baTemp = new byte[2];
							Helps.TryStrToHexByte(uCountFCS16.ToString("X4"), out baTemp);
							baAPDU = baAPDU.Concat(baTemp).ToArray();
							sendBytes = new byte[baAPDU.Length + 1];
							baAPDU.CopyTo(sendBytes, 0);
							baTemp = new byte[1] { 0x16 };
							baTemp.CopyTo(sendBytes, sendBytes.Length - 1);
							output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
							recBytes = SpMeter.SendCommand(sendBytes, "698Pass");
						}
						else
						{
							output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
							recBytes = SpMeter.SendCommand(sendBytes, "645");
						}

						output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
						bool isHaveHouxu = false;
						do
						{
							if (!SpMeter.Working)
							{
								continue;
							}

							string dataPianDuan;
							DLT645.ControlType controlType = DLT645.ControlType.Read;
							if (txcEncrypt.Checked)
							{
								controlType = DLT645.ControlType.SafetyReadLTU;
							}

							if (isHaveHouxu)
							{
								controlType = DLT645.ControlType.ReadFollow;
							}

							if (!dlt645.UnPack(recBytes, controlType, dataMarkList, seq, out isHaveHouxu, out dataPianDuan, out errStr))
							{
								drs[i]["读取值"] = "";
								drs[i]["操作结论"] = errStr;
								isReadingErr = true;
								break;
							}

							if (!string.IsNullOrEmpty(drs[i]["DataBlockType"].ToString()))
							{
								dataPianDuan = Helps.fnAgainst(dataPianDuan);
							}

							dataResultStr += dataPianDuan.Trim();
							if (isHaveHouxu)
							{
								seq++;
								if (!dlt645.Pack(DLT645.ControlType.ReadFollow, dataMarkList, "", seq, out sendBytes, out errStr))
								{
									drs[i]["读取值"] = "";
									drs[i]["操作结论"] = errStr;
									isReadingErr = true;
									break;
								}

								output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
								recBytes = SpMeter.SendCommand(sendBytes, "645");
								output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
							}
						} while (isHaveHouxu);

						if (isReadingErr)
						{
							isReadingErr = false;
							continue;
						}

						int dataLen;
						if (drs[i]["DataLen"].ToString().Contains("*"))
						{
							GetData(drs[i], dataResultStr, dataResultStr.Length);
						}
						else
						{
							if (!int.TryParse(drs[i]["DataLen"].ToString(), out dataLen))
							{
								drs[i]["读取值"] = "";
								drs[i]["操作结论"] = "数据库中该数据项长度非法，请排查数据库信息！";
								continue;
							}

							if (dataResultStr.Length != dataLen * 2)
							{
								drs[i]["读取值"] = "";
								drs[i]["操作结论"] = "失败：返回数据长度非法";
								continue;
							}
							GetData(drs[i], dataResultStr, dataLen);
						}
						Thread.Sleep(int.Parse(pmc[11].Value.ToString()));
					}
					#endregion
				}
				catch (Exception exception)
				{
					output("抄读过程中出现错误：" + exception.Message, true, true);
				}
				finally
				{
					if (SpMeter.Working)
					{
						SpMeter.Working = false;
					}
				}
			}
			else
			{
				#region Thread
				object oTemp = null;
				Thread td = new Thread(new ParameterizedThreadStart(tReadBLE));
				td.IsBackground = true;
				td.Start(oTemp);
				#endregion
			}
		}

		/// <summary>
		/// ExplainReadData
		/// </summary>
		/// <param name="dr"></param>
		/// <param name="dataResultStr"></param>
		/// <param name="dataLen"></param>
		/// <returns></returns>
		bool GetData(DataRow dr, string dataResultStr, int dataLen)
		{
			int decimalPlaces = 0;
			if (dr["DecimalPlaces"] == null || (!int.TryParse(dr["DecimalPlaces"].ToString(), out decimalPlaces)))
			{
				decimalPlaces = 0;
			}
			if (dr["DataBlockType"] == null || string.IsNullOrEmpty(dr["DataBlockType"].ToString()))
			{
				switch (dr["DataFormat"])
				{
					case "BCD":
					case "YYMMDDhh":
					case "YYMMDDWW":
						string strSymble = "";
						if ((dr["Name"].ToString().Contains("功率因数")) || (dr["DataUnit"].ToString() == "A") || (dr["DataUnit"].ToString() == "kW") || (dr["DataUnit"].ToString() == "Kvar") || (dr["DataUnit"].ToString() == "kVA"))
						{
							int iTe = 0;

							if ((Convert.ToInt64("0x" + dataResultStr.Substring(0, 1), 16) & 0x08) == 8)
							{
								strSymble = "-";
							}
							iTe = (int)(Convert.ToInt64("0x" + dataResultStr.Substring(0, 1), 16) & 0x07);
							dataResultStr = strSymble + iTe.ToString() + dataResultStr.Substring(1);
						}
						if (decimalPlaces != 0)
						{
							dataResultStr = dataResultStr.Insert(dataResultStr.Length - decimalPlaces, ".");
						}
						dr["读取值"] = dataResultStr;
						dr["操作结论"] = "成功";
						break;
					case "ASCII":
						if (dr["DataUnit"].ToString() == "Positive")
						{
							dataResultStr = Helps.fnAgainst(dataResultStr);
						}
						System.Text.ASCIIEncoding asciiEncoding = new System.Text.ASCIIEncoding();
						dataResultStr = Helps.ASCII2Str(dataResultStr).PadRight(dataLen, asciiEncoding.GetChars(new byte[] { 0 })[0]);
						dr["读取值"] = dataResultStr;
						dr["操作结论"] = "成功";
						break;
					case "HEX10":
						UInt64 dataInt;
						if (!UInt64.TryParse(dataResultStr, NumberStyles.AllowHexSpecifier, null, out dataInt))
						{
							dr["读取值"] = dataResultStr;
							dr["操作结论"] = "失败:解析异常";
							break;
						}

						if (decimalPlaces == 0)
						{
							dr["读取值"] = dataInt;
						}
						else
						{
							dr["读取值"] = dataInt / Math.Pow(10, decimalPlaces);
						}
						dr["操作结论"] = "成功";
						break;
					case "Float":
						UInt32 x = Convert.ToUInt32((dataResultStr), 16);//字符串转16进制32位无符号整数
						float fy = BitConverter.ToSingle(BitConverter.GetBytes(x), 0);//IEEE754 字节转换float//得出数字 3236.725

						dr["读取值"] = fy.ToString();
						dr["操作结论"] = "成功";
						break;
					case "Complement"://补码
						UInt32 UnsignedValue = Convert.ToUInt32((dataResultStr), 16);//字符串转16进制32位无符号整数
						int bitLength = dataLen * 8; // 16
						UInt32 signBit = 1U << (bitLength - 1); // 1 << 15 = 0x8000 = 32768
						Int32 signedValue;
						if ((UnsignedValue & signBit) != 0)
						{
							// 是负数，补码转换
							signedValue = (Int32)(UnsignedValue | ~((1U << bitLength) - 1));
						}
						else
						{
							signedValue = (Int32)UnsignedValue;
						}
						if (decimalPlaces == 0)
						{
							dr["读取值"] = signedValue;
						}
						else
						{
							dr["读取值"] = signedValue / Math.Pow(10, decimalPlaces);
						}
						dr["操作结论"] = "成功";
						break;
					default://一律按hex16格式处理
						dr["读取值"] = dataResultStr;
						dr["操作结论"] = "成功";
						break;
				}
				strDataParse = dr["读取值"].ToString();
			}
			else
			{
				dr["读取值"] = dataResultStr;
				dr["操作结论"] = "成功";
			}
			if (txcbCheckData.Checked)
			{
				dr["操作结论"] = "参数错误!!!";
				if (dr["DataFormat"].ToString() == "ASCII")
				{
					dataResultStr = dataResultStr.Replace("\0", "");
					string strCheck = dr["设置值"].ToString().Replace("\0", "");
					if (dataResultStr == strCheck)
					{
						dr["操作结论"] = "参数正确";
					}
				}
				else if (dr["DataFormat"].ToString() == "BCD")
				{
					if (dataResultStr.Contains("."))
					{
						if (double.Parse(dataResultStr) == double.Parse(dr["设置值"].ToString()))
						{
							dr["操作结论"] = "参数正确";
						}
					}
					else if (Int64.Parse(dataResultStr) == Int64.Parse(dr["设置值"].ToString()))
					{
						dr["操作结论"] = "参数正确";
					}
				}
			}

			return true;
		}

		/// <summary>
		/// ExplainSetData
		/// </summary>
		/// <param name="dr"></param>
		/// <param name="setValue"></param>
		/// <param name="dataStr"></param>
		/// <param name="errStr"></param>
		/// <returns></returns>
		bool GetSetDataStr(DataRow dr, string setValue, out string dataStr, out string errStr)
		{
			dataStr = "";
			errStr = "设置值转换报文失败。";
			int len = 0;
			string strLen = dr["DataLen"].ToString();
			if (strLen.Contains("*"))
			{
				len = setValue.Length / 2;
			}
			else
			{
				if (!int.TryParse(dr["DataLen"].ToString(), out len))
				{
					errStr = "数据库中数据长度非法，请检查";
					return false;
				}
			}

			if (string.IsNullOrEmpty(setValue))
			{
				if ((len == 0))
				{
					return true;
				}
				errStr = "请输入设置值";
				return false;
			}
			try
			{

				if (string.IsNullOrEmpty(dr["DataBlockType"].ToString()))
				{
					int DecimalPlaces;
					if (!int.TryParse(dr["DecimalPlaces"].ToString(), out DecimalPlaces))
					{
						DecimalPlaces = 0;
					}
					switch (dr["DataFormat"])
					{
						case "BCD":
						case "YYMMDDhh":
						case "YYMMDDWW":
							Decimal dataDec;
							if (!Decimal.TryParse(setValue, out dataDec))
							{
								errStr = "设置值非法，请检查";
								return false;
							}
							Int64 iTemp = (Int64)Math.Pow(10, DecimalPlaces);
							dataStr = ((Int64)(dataDec * iTemp)).ToString(string.Format("D{0}", (2 * len)));
							/*
							double dData;
							if (!double.TryParse(setValue, out dData))
							{
								errStr = "设置值非法，请检查";
								return false;
							}

							double dDataDot = dData * iTemp;
							dataStr = ((Int64)(dDataDot)).ToString(string.Format("D{0}", (2 * len)));
							dDataDot *= 10;
							dataStr = ((Int64)(dDataDot )).ToString(string.Format("D{0}", (2 * len)));

							float flData;
							if (!float.TryParse(setValue, out flData))
							{
								errStr = "设置值非法，请检查";
								return false;
							}
							float fDataDot = flData * iTemp;
							dataStr = ((Int64)(fDataDot)).ToString(string.Format("D{0}", (2 * len)));
							fDataDot *= 10;
							dataStr = ((Int64)(fDataDot)).ToString(string.Format("D{0}", (2 * len)));

							Decimal decData;
							if (!Decimal.TryParse(setValue, out decData))
							{
								errStr = "设置值非法，请检查";
								return false;
							}
							Decimal decDataDot = decData * iTemp;
							dataStr = ((Int64)(decDataDot)).ToString(string.Format("D{0}", (2 * len)));
							decDataDot *= 10;
							dataStr = ((Int64)(decDataDot)).ToString(string.Format("D{0}", (2 * len)));
							*/
							//dataStr = DataConvert.fnGetStringFormat(setValue.PadLeft( 2 * len, '0' ) ); Math.Ceiling
							if (dataStr.Length > 2 * len)
							{
								errStr = "设置值非法，请检查";
								return false;
							}
							break;
						case "ASCII":
							if (setValue.Length > len)
							{
								errStr = "设置值长度非法，请检查";
								return false;
							}
							System.Text.ASCIIEncoding asciiEncoding = new System.Text.ASCIIEncoding();
							byte[] asciiByts = new byte[len];
							if (Regex.IsMatch(setValue, @"[\u4e00-\u9fa5]"))
							{
								byte[] ba = Helps.Str2AsciibBytes(setValue);
								ba.CopyTo(asciiByts, 0);
							}
							else
							{
								asciiByts = Helps.Str2AsciibBytes(setValue.PadRight(len, asciiEncoding.GetChars(new byte[] { 0 })[0]));
							}

							dataStr = SerialTool.byteToHexStr(asciiByts);
							break;
						case "HEX10":
							if (!double.TryParse(setValue, out double dataD))
							{
								errStr = "设置值非法，请检查";
								return false;
							}
							dataStr = ((Int64)(dataD * Math.Pow(10, DecimalPlaces))).ToString(string.Format("X{0}", (2 * len)));
							if (dataStr.Length > 2 * len)
							{
								errStr = "设置值非法，请检查";
								return false;
							}
							break;
						case "Float":
							float fData = float.Parse(setValue);
							byte[] ieee754Bytes = BitConverter.GetBytes(fData);
							dataStr = Helps.fnAgainst(DataConvert.fnGetStringFormat(DataConvert.Bytes2HexString(ieee754Bytes, 0, ieee754Bytes.Length)));
							break;
						default://一律按HEX处理
								//if (!Helps.IsIllegalHexadecimal1(setValue))
								//{
								//    errStr = "设置值非法，请检查";
								//    return false;
								//}
							if (setValue.Length != 2 * len)
							{
								errStr = "设置值非法，请检查";
								return false;
							}

							dataStr = setValue;
							break;
					}
					return true;
				}
				else
				{
					if (setValue.Length != 2 * len)
					{
						errStr = "设置值长度错误，请检查";
						return false;
					}
					if (!Helps.IsHexadecimal(setValue))
					{
						errStr = "设置值非法，请检查";
						return false;
					}
					dataStr = Helps.fnAgainst(setValue);
					return true;
				}
			}
			catch (Exception e)
			{
				errStr = "设置值组包时发生错误:" + e.Message;
				return false;
			}
		}

		private void btn_设置_Click(object sender, EventArgs e)
		{
			if (txcbBLE.CheckState == CheckState.Unchecked)
			{
				if (btn_电表通信串口.Text == "打开串口")
				{
					MessageBox.Show("请先打开串口!");
					return;
				}

				if (SpMeter.Working)
				{
					return;
				}
				try
				{
					DataRow[] drs = dt_RW.Select(string.Format("选择 = '{0}' ", "true"), "");
					int rowCount = drs.GetLength(0);
					if (rowCount < 1)
					{
						return;
					}
					SpMeter.Working = true;

					#region 设置
					DLT645 dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
					for (int i = 0; i < rowCount && SpMeter.Working; i++)
					{
						List<string> dataMarkList = new List<string>();
						dataMarkList.Clear();
						var dataResultStr = "";
						byte[] sendBytes = null;
						string errStr = "";
						string dataStr;
						string strRandom = "";
						if (txcEncrypt.Checked)
						{
							string strGetRandom = m_SCUProxy.HY_SendToSecModuleRandom("DFDDA010");
							dlt645 = new DLT645(Helps.fnAgainst(txtxbTXAddr.Text.ToString()));
							if (!_dlt645.Pack(DLT645.ControlType.SafetyCertificationLTU, lstrDataMark, strGetRandom, 0, out baSendBytes, out string strErr))
							{

							}
							output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
							byte[] recByte = SpMeter.SendCommand(baSendBytes, "645");
							output("RX:" + Helps.ByteToStringWithSpace(recByte), false, true);

							if (dlt645.UnPack(recByte, DLT645.ControlType.SafetyCertificationLTU, dataMarkList, 0, out bool isHave, out string strDataField, out errStr))
							{
								strRandom = strDataField.Substring(strDataField.Length - 8, 8);
								output("随机数:" + strRandom, false, true);
								strRandom = Helps.fnAgainst(strRandom);
								//drs[i]["操作结论"] = "设置成功";
							}
							else
							{
								drs[i]["操作结论"] = "设置失败：" + errStr;
							}
						}
						if (drs[i]["DataMark"] == null || drs[i]["DataMark"].ToString().Length != 8)
						{
							drs[i]["读取值"] = "";
							drs[i]["操作结论"] = "设置失败：数据标识非法！";
							continue;
						}

						string dataMark = drs[i]["DataMark"].ToString();
						dataMarkList.Add(dataMark);
						if (drs[i]["DataMarkFather"] != null && drs[i]["DataMarkFather"].ToString().Length == 8)
						{
							dataMarkList.Add(drs[i]["DataMarkFather"].ToString());
						}

						if (!GetSetDataStr(drs[i], drs[i]["设置值"].ToString(), out dataStr, out errStr))
						{
							drs[i]["操作结论"] = "设置失败：" + errStr;
							continue;
						}
						DLT645.ControlType ctSeType = DLT645.ControlType.Write;

						if (txcbSetControl.Text == "1C")
						{
							ctSeType = DLT645.ControlType.WriteSwitch;
						}
						else if (txcEncrypt.Checked)
						{
							ctSeType = DLT645.ControlType.SafetyWriteLTU;
						}
						dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
						if (!dlt645.Pack(ctSeType, dataMarkList, strRandom + dataStr, 0, out sendBytes, out errStr, pmc[7].Value.ToString(), pmc[8].Value.ToString(), pmc[9].Value.ToString()))
						{
							drs[i]["操作结论"] = errStr;
							continue;
						}

						output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
						byte[] recBytes = SpMeter.SendCommand(sendBytes, "645");
						output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
						string dataPianDuan;
						bool isHaveHouxu;

						if (dlt645.UnPack(recBytes, ctSeType, dataMarkList, 0, out isHaveHouxu, out dataPianDuan, out errStr))
						{
							drs[i]["操作结论"] = "设置成功";
						}
						else
						{
							drs[i]["操作结论"] = "设置失败：" + errStr;
						}
						Thread.Sleep(int.Parse(pmc[11].Value.ToString()));
					}
					#endregion
				}
				catch (Exception exception)
				{
					output("设置过程中出现错误：" + exception.Message, true, true);
				}
				finally
				{
					if (SpMeter.Working)
					{
						SpMeter.Working = false;
					}
				}
			}
			else
			{
				#region Thread
				object oTemp = null;
				Thread td = new Thread(new ParameterizedThreadStart(tSetBLE));
				td.IsBackground = true;
				td.Start(oTemp);
				#endregion
			}
		}

		#region 升级 6025&4G6025
		List<byte> listHex = new List<byte>();
		List<byte> blistHex = new List<byte>();
		private List<byte[]> lbaHexBytes = new List<byte[]>();
		private int iPartSize = 1344, iRepeatNum = 3;
		public static DLT645 _dlt645 = new DLT645("888888888888");
		List<string> lstrDataMark = new List<string>();
		public readonly int iFHBS_LENGHT = 128;
		private void button2_Click(object sender, EventArgs e)
		{
			listHex.Clear();
			if (openFileDialog1.ShowDialog() == DialogResult.OK)
			{
				textBox1.Text = openFileDialog1.FileName;
			}
			try
			{
				using (FileStream fs = new FileStream(openFileDialog1.FileName, FileMode.Open, FileAccess.Read))
				{
					while (true)
					{
						int tempB = 0;
						try
						{
							tempB = fs.ReadByte();
							if (tempB == -1)
							{
								break;
							}
							listHex.Add(Convert.ToByte(tempB));
						}
						catch (Exception exx)
						{
							MessageBox.Show(exx.Message);
							return;
						}
					}
				}
			}
			catch (Exception exx)
			{
				MessageBox.Show(exx.Message);
				return;
			}

			int hexLength = listHex.Count;
			byte[] TempBuf = new byte[240 * 1024];

			for (int i = 0; i < 240 * 1024; i++)
			{
				TempBuf[i] = 0xff;
			}
			listHex.CopyTo(0, TempBuf, 0, hexLength);

			uint ImageCrc1 = UpdataCRC32.CalcCRC32bySoftforUpgrade(0, TempBuf, 240 * 1024);//计算总文件校验 240k

			int num1 = hexLength % 1024;//将发送的固件进行1024补齐，方便电表处理

			if (num1 != 0)
			{
				int num2 = 1024 - num1;//计算需要补多少个FF
				for (int i = 0; i < num2; i++)
				{
					listHex.Add(0xff);
				}
			}
			uint ImageCrc = UpdataCRC32.CalcCRC32bySoftforUpgrade(0, TempBuf, listHex.Count);//计算发给电表的补齐1024的文件的校验，这个校验会随固件下发
			textBox2.Text = "FileLength= " + listHex.Count.ToString() + " bytes  " + "CRC= 0x" + ImageCrc.ToString("X8");
			listHex.Add((byte)(ImageCrc >> 24));
			listHex.Add((byte)(ImageCrc >> 16));
			listHex.Add((byte)(ImageCrc >> 8));
			listHex.Add((byte)(ImageCrc >> 0));//最后4个字节为校验
		}

		/// <summary>
		/// SendMessage
		/// </summary>
		/// <param name="sendBytes"></param>
		/// <param name="okByte"></param>
		/// <returns></returns>
		bool sendOK(byte[] sendBytes, byte okByte)
		{
			//if (SpMeter.Working) return false;
			try
			{
				if (sendBytes[0] == 0x68)
				{
					byte[] bSendBuf = sendBytes;
					sendBytes = new byte[sendBytes.Length + 4];
					byte[] bFE = { 0xFE, 0xFE, 0xFE, 0xFE, };
					bFE.CopyTo(sendBytes, 0);
					bSendBuf.CopyTo(sendBytes, bFE.Length);
				}

#if !ShortUpGrade
				output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
#endif

				byte[] recBytes = SpMeter.SendCommand(sendBytes, "645");

#if !ShortUpGrade
				output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
#endif

				int index = RecCutNum(recBytes, "645");
				if (index == -1)
				{
					if (okByte == 0xfe)
					{
						output("读取版本失败!", true, false);//Reading version error
					}

					return false;
				}

				if (okByte == 0xfe)
				{
					if (recBytes[8 + index] != 0x91)
					{
						output("读取版本失败!", true, false);//Reading version error
						return false;
					}
					string 校验码Str = (((recBytes[index + 30] - 0x33)) & 0xff).ToString("X2") + ((recBytes[index + 31] - 0x33) & 0xff).ToString("X2");
					string VersionNum = (((recBytes[index + 25] - 0x33)) & 0xff).ToString("X2") + ((recBytes[index + 24] - 0x33) & 0xff).ToString("X2");
					MessageBox.Show("版本号：" + VersionNum + "；校验码：" + 校验码Str);
					return true;
				}

				if (okByte != 0xff)
				{
					if (recBytes[8 + index] != okByte)
					{
						return false;
					}
				}
				return true;
			}
			catch (Exception e)
			{
				output(e.Message, true, true);
				return false;
			}
		}

		private void button3_Click(object sender, EventArgs e)
		{
			if (SpMeter.Working) return;
			int t = 0;

			//byte[] TxBufFactoryMode = { 0xfe, 0xfe, 0xfe, 0xfe, 0x68, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x68, 0x14, 0x15, 0x0E, 0x12, 0x12, 0x37, 0x35, 0x33, 0x33, 0x33, 0x37, 0x36, 0x35, 0x34, 0x34, 0x35, 0x12, 0xE, 0xBB, 0xBB, 0xBB, 0xBB, 0x34, 0xdf, 0x16 };
			//byte[] TxBufUpdate = { 0xfe, 0xfe, 0xfe, 0xfe, 0x68, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x68, 0x14, 0x29, 0x0e, 0x12, 0x12, 0x37, 0x35, 0x33, 0x33, 0x33, 0x37, 0x36, 0x35, 0x34, 0x35, 0x43, 0x12, 0x0e, 0xbb, 0xbb, 0xbb, 0xbb, 0x27, 0x34, 0xdd, 0x20, 0x34, 0xdd, 0x93, 0x8e, 0x36, 0x33, 0x83, 0xf6, 0x33, 0x33, 0x13, 0xfb, 0x43, 0x33, 0xdd, 0xdd, 0xdd, 0xbb, 0x16 };
			byte[] ReceBuf = null;

			byte[] TxBufUpdate = new byte[57];
			string strDataSend = "68" + Helps.fnAgainst(pmc[6].Value.ToString()) + "6814290E12123735333333373635343543120EBBBBBBBB2734DD2034DD938E363383F6333313FB4333DDDDDD";
			Helps.TryStrToHexByte(strDataSend, out TxBufUpdate);

			#region 校验和+16
			byte sum = Helps.Ssum(TxBufUpdate);
			byte[] endBytes = new[] { sum, (byte)0x16 };
			#endregion

			TxBufUpdate = TxBufUpdate.Concat(endBytes).ToArray();

			if (sendOK(TxBufUpdate, 0xff))
			{
				output("请按键进入编程状态，开始升级", true, true);
			}
			else
			{
				output("擦除成功", false, true);//Enter Erase Mode Success, Pleae wait for LCD display READY
			}
		}

		private void initial_Click(object sender, EventArgs e)
		{
			//byte[] TxBufInitial =				{ 0xfe, 0xfe, 0xfe, 0xfe, 0x68, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x68, 0x14, 0x14, 0x0E, 0x12, 0x12, 0x37, 0x35, 0x33, 0x33, 0x33, 0x37, 0x36, 0x35, 0x34, 0xE, 0x3A, 0x12, 0xE, 0xBB, 0xBB, 0xBB, 0xBB, 0x89, 0x16 };
			byte[] TxBufInitial = new byte[31];
			string strDataSend = "68" + Helps.fnAgainst(pmc[6].Value.ToString()) + "6814140E121237" + (DataConvert.fnEncodingData(pmc[8].Value.ToString() + pmc[7].Value.ToString())) + "373635340E3A120EBBBBBBBB";
			Helps.TryStrToHexByte(strDataSend, out TxBufInitial);

			#region 校验和+16
			byte sum = Helps.Ssum(TxBufInitial);
			byte[] endBytes = new[] { sum, (byte)0x16 };
			#endregion

			TxBufInitial = TxBufInitial.Concat(endBytes).ToArray();


			if (sendOK(TxBufInitial, 0x94))
			{
				output("初始化电表成功", false, true);//Initial Meter Success
			}
			else
			{
				output("通信异常,请检查", false, true);//Meter reply error,Check the connection please
			}
		}

		private void button4_Click(object sender, EventArgs e)
		{
			progressBar1.Maximum = listHex.Count / 1024;
			progressBar1.Value = 0;
            if (!int.TryParse(pmc[1].Value.ToString(), out int baudRate))
            {
                MessageBox.Show("打开串口失败：非法的波特率值！");
                return;
            }
            if (!int.TryParse(pmc[3].Value.ToString(), out int dataBit))
            {
                MessageBox.Show("打开串口失败：非法的数据位！");
                return;
            }
            if (!int.TryParse(pmc[5].Value.ToString(), out int overTime))
            {
                MessageBox.Show("打开串口失败：非法的通信超时时间！");
                return;
            }
			SpMeter.setSerialPort(pmc[0].Value.ToString(), baudRate, Parity.Even.ToString(), dataBit, pmc[4].Value.ToString(), overTime);

            Thread thr = new Thread(ThrFun);
			thr.IsBackground = true;
			thr.Start();
		}

		public delegate void OnPregss(object obj1, object obj2, object obj3, object obj4);
		OnPregss dlgSetControl;
		private object m_objectShow = new object();

		/// <summary>
		/// 6025UpdataDisplayThread委托
		/// </summary>
		/// <param name="obj1"></param>
		/// <param name="obj2"></param>
		/// <param name="obj3"></param>
		/// <param name="obj4"></param>
		public void Comm_ShowDataInRichTextBoxHandler(object obj1, object obj2, object obj3, object obj4)
		{
			try
			{
				if (dlgSetControl == null)
				{
					dlgSetControl = new OnPregss(_fnSetControlValue);
				}
				Invoke(dlgSetControl, new object[] { obj1, obj2, obj3, obj4 });
			}
			catch { }
		}

		/// <summary>
		/// 6025Updata4GDisplayThread委托
		/// </summary>
		/// <param name="obj1"></param>
		/// <param name="obj2"></param>
		/// <param name="obj3"></param>
		/// <param name="obj4"></param>
		public void Comm_ShowDataInrtbLogHandler(object obj1, object obj2, object obj3, object obj4)
		{
			try
			{
				if (dlgSetControl == null)
				{
					dlgSetControl = new OnPregss(_fnSetControlValue4G);
				}
				Invoke(dlgSetControl, new object[] { obj1, obj2, obj3, obj4 });
			}
			catch { }
		}

		/// <summary>
		/// 显示到RichText
		/// </summary>
		/// <param name="rtb"></param>
		/// <param name="s"></param>
		/// <param name="c"></param>
		public void RichTextAppend(RichTextBox rtb, string s, Color c)
		{
			int i;
			i = rtb.TextLength;
			rtb.AppendText(s);
			rtb.Select(i, s.Length);
			rtb.SelectionColor = c;
			rtb.SelectionStart = this.richTextBox1.TextLength;
			rtb.ScrollToCaret();
		}

		//ControlFlag 0 刷新发送到第几包  1 
		public void _fnSetControlValue(object obj1, object obj2, object obj3, object obj4)
		{
			lock (m_objectShow)
			{
				int ControlFlag = (int)obj1;

				if (ControlFlag == 0)
				{
					int count = (int)obj3;
					progressBar1.Value = count;
					RichTextAppend(richTextBox1, DateTime.Now.ToString() + "发送第 " + (count + 1).ToString() + "包 " + "(" + (listHex.Count / 1024).ToString() + ") \n", Color.Blue);//Sending Block
				}
				else if (ControlFlag == 1)
				{
					string TempStr = (string)(obj4);
					RichTextAppend(richTextBox1, DateTime.Now.ToString() + " " + TempStr + "\n", Color.Red);
				}
				else if (ControlFlag == 3)
				{
					int count = (int)obj3;
					RichTextAppend(richTextBox1, DateTime.Now.ToString() + " 软件版本: " + (count).ToString() + "\n", Color.Blue);//The firmware version is
				}
				else if (ControlFlag == 4)
				{
					string count = (string)obj3;
					RichTextAppend(richTextBox1, DateTime.Now.ToString() + "  软件版本: " + count + "\n", Color.Blue);
				}
				else if (ControlFlag == 5)
				{
					string count = (string)obj3;
					RichTextAppend(richTextBox1, DateTime.Now.ToString() + " " + count + "\n", Color.Blue);
				}
			}
		}

		/// <summary>
		/// 6025刷新发送到第几包
		/// </summary>
		/// <param name="obj1"></param>
		/// <param name="obj2"></param>
		/// <param name="obj3"></param>
		/// <param name="obj4"></param>
		public void _fnSetControlValue4G(object obj1, object obj2, object obj3, object obj4)
		{
			lock (m_objectShow)
			{
				int ControlFlag = (int)obj1;

				if (ControlFlag == 0)
				{
					int count = (int)obj3;
					pbProgress.Value = (int)(count * 100 / listHex.Count / 1024);
					RichTextAppend(rtbLog, DateTime.Now.ToString() + "发送第 " + (count + 1).ToString() + "包 " + "(" + (listHex.Count / 1024).ToString() + ") \n", Color.Blue);//Sending Block
				}
				else if (ControlFlag == 1)
				{
					string TempStr = (string)(obj4);
					RichTextAppend(rtbLog, DateTime.Now.ToString() + " " + TempStr + "\n", Color.Red);
				}
				else if (ControlFlag == 3)
				{
					int count = (int)obj3;
					RichTextAppend(rtbLog, DateTime.Now.ToString() + " 软件版本: " + (count).ToString() + "\n", Color.Blue);//The firmware version is
				}
				else if (ControlFlag == 4)
				{
					string count = (string)obj3;
					RichTextAppend(rtbLog, DateTime.Now.ToString() + "  软件版本: " + count + "\n", Color.Blue);
				}
				else if (ControlFlag == 5)
				{
					string count = (string)obj3;
					RichTextAppend(rtbLog, DateTime.Now.ToString() + obj4.ToString() + (count + 1) + "包 " + "\n", Color.Blue);
				}
			}
		}

		/// <summary>
		/// 升级6025
		/// </summary>
		public void ThrFun()
		{
			byte[] SendBuf = new byte[1024];
			byte[] ReceBuf = new byte[100];
			int TimerCounter = 0;
			int t = 0;
			int SendBlock = listHex.Count / 1024;

			for (int i = 0; i < SendBlock; i++)
			{
				listHex.CopyTo(i * 1024, SendBuf, 0, 1024);
				Comm_ShowDataInRichTextBoxHandler(0, 0, i, 0);
				// progressBar1.Value = i;
				//  RichTextAppend(richTextBox1, DateTime.Now.ToString() + " Sending Block " + (i + 1).ToString() + "(" + SendBlock.ToString() + ") \n", Color.Blue);
				System.Threading.Thread.Sleep(10);
				switch (SpMeter.sendUpdata(SendBuf))
				{
					case 0:

						break;
					case 1:
						RichTextAppend(richTextBox1, "打开失败！\n", Color.Red);//Open Error！
						return;
					case 2:
						Comm_ShowDataInRichTextBoxHandler(1, 0, 0, "发送失败,请检查!");//Sending error,Check the connection please
						return;
					case 3:
						Comm_ShowDataInRichTextBoxHandler(1, 0, 0, "接收失败,请检查!");//Meter reply error,Check the connection please
						return;
					default:
						return;
				}
			}
			System.Threading.Thread.Sleep(600);
			listHex.CopyTo(SendBlock * 1024, SendBuf, 0, 4);//校验
			SpMeter.sendWithNoAsk(SendBuf, 4);
			//RichTextAppend(richTextBox1, DateTime.Now.ToString() + " Sending Crc " + "\n", Color.Red);
			Comm_ShowDataInRichTextBoxHandler(1, 0, 0, "发送校验中");//Sending CRC......
			System.Threading.Thread.Sleep(2000);
			Comm_ShowDataInRichTextBoxHandler(1, 0, 0, "固件升级完成! 请检查版本信息");//Image Send Finished! Please check firmware version of the meter
			Comm_ShowDataInRichTextBoxHandler(1, 0, 0, "请等待15秒...");//Please wait about 15 seconds...
																	// MessageBox.Show("Image Send Finished");
			System.Threading.Thread.Sleep(15000);
            if (!int.TryParse(pmc[1].Value.ToString(), out int baudRate))
            {
                MessageBox.Show("打开串口失败：非法的波特率值！");
                return;
            }
            if (!int.TryParse(pmc[3].Value.ToString(), out int dataBit))
            {
                MessageBox.Show("打开串口失败：非法的数据位！");
                return;
            }
            if (!int.TryParse(pmc[5].Value.ToString(), out int overTime))
            {
                MessageBox.Show("打开串口失败：非法的通信超时时间！");
                return;
            }
            SpMeter.setSerialPort(pmc[0].Value.ToString(), baudRate, pmc[2].Value.ToString(), dataBit, pmc[4].Value.ToString(), overTime);

            GetMeterVersion();
		}

		/// <summary>
		/// 6025升级线程4G
		/// </summary>
		public void ThreadFun()
		{
			byte[] SendBuf = new byte[1024];
			byte[] ReceBuf = new byte[100];
			byte[] bBufFrameHeader = { 00, 00, 00, 00, 0x10, 0x24 };
			int TimerCounter = 0;
			int t = 0;
			int SendBlock = listHex.Count / 1024;

			for (int i = 0; i < SendBlock; i++)
			{
				Int64 iLength = i * 1024;
				string strLength = iLength.ToString("D8");
				byte[] bBuf = DataConvert.HEXString2Bytes(strLength);
				for (int j = 0; j < 4; j++)
				{
					bBufFrameHeader[j] = bBuf[j];
				}
				Console.WriteLine(DataConvert.Bytes2HexString(bBufFrameHeader, 0, bBufFrameHeader.Length));
				listHex.CopyTo(i * 1024, SendBuf, 0, 1024);
				Comm_ShowDataInrtbLogHandler(0, 0, i, 0);
				//Console.WriteLine(DataConvert.Bytes2HexString(SendBuf, 0, SendBuf.Length));
				byte[] bSendBuf = new byte[bBufFrameHeader.Length + SendBuf.Length + 2];
				bBufFrameHeader.CopyTo(bSendBuf, 0);
				//Console.WriteLine(DataConvert.Bytes2HexString(bSendBuf, 0, bSendBuf.Length));
				SendBuf.CopyTo(bSendBuf, bBufFrameHeader.Length);
				//Console.WriteLine(DataConvert.Bytes2HexString(bSendBuf, 0, bSendBuf.Length));
				uint u1kCRC = WSDFCS16.CheckCRC16(bSendBuf, bSendBuf.Length - 2);
				bBuf = DataConvert.HEXString2Bytes(u1kCRC.ToString("x4"));
				bBuf.CopyTo(bSendBuf, bBufFrameHeader.Length + SendBuf.Length);
				//Console.WriteLine(DataConvert.Bytes2HexString(bSendBuf, 0, bSendBuf.Length));
				System.Threading.Thread.Sleep(10);
				byte[] bRECBuf = SpMeter.SendCommands(bSendBuf, bSendBuf.Length, 400);
				if (bRECBuf != null)
				{
					if (bRECBuf[18] != 0x34)
					{
						MessageBox.Show("升级传输异常,请检查!");
						return;
					}
				}

				output(DataConvert.Bytes2HexString(bSendBuf, 0, bSendBuf.Length), false, true);
			}

			System.Threading.Thread.Sleep(600);
			bBufFrameHeader = new byte[] { 0x99, 0x99, 0x99, 0x99, 0x00, 0x04 };

			byte[] bSendBuff = new byte[12];
			byte[] bSendBufCRC = new byte[4];

			bBufFrameHeader.CopyTo(bSendBuff, 0);
			listHex.CopyTo(SendBlock * 1024, bSendBufCRC, 0, 4);//校验
			bSendBufCRC.CopyTo(bSendBuff, bBufFrameHeader.Length);
			uint uCRC = WSDFCS16.CheckCRC16(bSendBuff, bSendBuff.Length - 2);
			byte[] bBuff = DataConvert.HEXString2Bytes(uCRC.ToString("x4"));
			bBuff.CopyTo(bSendBuff, bBufFrameHeader.Length + bSendBufCRC.Length);
			//Console.WriteLine(DataConvert.Bytes2HexString(bSendBuff, 0, bSendBuff.Length));

			byte[] bRECBuff = SpMeter.SendCommands(bSendBuff, bSendBuff.Length, 4000);
			if (bRECBuff != null)
			{
				if (bRECBuff[18] != 0x34)
				{
					MessageBox.Show("升级传输异常,请检查!");
					return;
				}
			}

			Comm_ShowDataInrtbLogHandler(1, 0, 0, "发送校验中");//Sending CRC......
			System.Threading.Thread.Sleep(2000);
			Comm_ShowDataInrtbLogHandler(1, 0, 0, "固件升级完成! 请检查版本信息");//Image Send Finished! Please check firmware version of the meter
			Comm_ShowDataInrtbLogHandler(1, 0, 0, "请等待15秒...");//Please wait about 15 seconds...
															   // MessageBox.Show("Image Send Finished");
			System.Threading.Thread.Sleep(15000);
			GetMeterVersion();
		}

		/// <summary>
		/// 获取电表版本6025
		/// </summary>
		private void GetMeterVersion()
		{
			int t = 0;
			byte[] TxBufGetVersion = null;//{ 0xfe, 0xfe, 0xfe, 0xfe, 0x68, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x68, 0x11, 0x08, 0xe, 0x12, 0x12, 0x37, 0xe, 0x3b, 0x12, 0xe, 0xeb, 0x16 };

			string strDataSend = "68" + Helps.fnAgainst(pmc[6].Value.ToString()) + "6811080E1212370E3B120E";
			Helps.TryStrToHexByte(strDataSend, out TxBufGetVersion);

			#region 校验和+16
			byte sum = Helps.Ssum(TxBufGetVersion);
			byte[] endBytes = new[] { sum, (byte)0x16 };
			#endregion

			TxBufGetVersion = TxBufGetVersion.Concat(endBytes).ToArray();
			byte[] FVersion = new byte[2];
			try
			{
				sendOK(TxBufGetVersion, 0xfe);
			}
			catch (Exception exx)
			{
				MessageBox.Show(exx.Message);
				return;
			}
		}
		#endregion

		#region 通信测试
		List<string> addrcoms = new List<string>();
		private void btn_通信测试_Click(object sender, EventArgs e)
		{
			if (SpMeter.Working) return;
			SpMeter.Working = true;
			addrcoms.Clear();
			int num = 0;
			for (int i = 1; i < 31; i++)
			{
				string text = "txTextBox" + i.ToString();
				TXTextBox tb = (TXTextBox)this.findControl(text);
				addrcoms.Add(tb.Text);
				if (string.IsNullOrEmpty(tb.Text)) num++;
				text = "ucSwitch" + i.ToString();
				UCSwitch checkbox = (UCSwitch)this.findControl(text);
				checkbox.Checked = false;
			}
			if (num == 30) { SpMeter.Working = false; return; }
			this.backgroundWorker1.RunWorkerAsync();
		}
		private Control findControl(string controlName)
		{
			Control c1;
			foreach (Control c in groupBox1.Controls)
			{
				if (c.Name == controlName)
				{
					return c;
				}
				//else if (c.Controls.Count > 0)
				//{
				//    c1 = findControl(controlName);
				//    if (c1 != null)
				//    {
				//        return c1;
				//    }
				//}
			}
			return null;
		}
		private void backgroundWorker1_DoWork(object sender, DoWorkEventArgs e)
		{
			try
			{
				List<string> dataMarkList = new List<string>();
				dataMarkList.Add("04000101");
				for (int i = 1; i < 31; i++)
				{
					if (string.IsNullOrEmpty(addrcoms[i - 1]))
					{
						backgroundWorker1.ReportProgress(i, false);
						continue;
					}
					DLT645 dlt = new DLT645(Helps.fnAgainst(addrcoms[i - 1].PadLeft(12, '0')));
					byte[] sendBytes = null;
					string errStr = "";

					if (!dlt.Pack(DLT645.ControlType.Read, dataMarkList, "", 0, out sendBytes, out errStr))
					{
						backgroundWorker1.ReportProgress(i, false);
						continue;
					}
					backgroundWorker1.ReportProgress(i, sendOK(sendBytes, 0x91));
				}
			}
			catch (Exception e1)
			{
				output(e1.Message, true, true);
			}
			finally
			{
				SpMeter.Working = false;
			}
		}
		#endregion

		private void backgroundWorker1_ProgressChanged(object sender, ProgressChangedEventArgs e)
		{
			string text = "ucSwitch" + e.ProgressPercentage;
			UCSwitch checkbox = (UCSwitch)this.findControl(text);
			checkbox.Checked = (bool)e.UserState;
		}

		#region 厂内模式
		private void Btn_厂内_Click(object sender, EventArgs e)
		{
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}
			byte[] TxBufFactoryMode = new byte[32];
			string strDataSend = "68" + Helps.fnAgainst(pmc[6].Value.ToString()) + "6814150E121237" + (DataConvert.fnEncodingData(pmc[8].Value.ToString() + pmc[7].Value.ToString())) + "373635343435120EBBBBBBBB34";
			Helps.TryStrToHexByte(strDataSend, out TxBufFactoryMode);

			#region 校验和+16
			byte sum = Helps.Ssum(TxBufFactoryMode);
			byte[] endBytes = new[] { sum, (byte)0x16 };
			#endregion

			TxBufFactoryMode = TxBufFactoryMode.Concat(endBytes).ToArray();
			//byte[] TxBufFactoryMode = { 0xfe, 0xfe, 0xfe, 0xfe, 0x68, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x68, 0x14, 0x15, 0x0E, 0x12, 0x12, 0x37, 0x35, 0x33, 0x33, 0x33, 0x37, 0x36, 0x35, 0x34, 0x34, 0x35, 0x12, 0xE, 0xBB, 0xBB, 0xBB, 0xBB, 0x34, 0xdf, 0x16 };
			if (sendOK(TxBufFactoryMode, 0x94))
			{
				output(" 打开编程模式成功", false, true);
			}
			else
			{
				output(" 打开编程模式失败!", true, true);
			}
		}
		#endregion

		private void btn_导入_Click(object sender, EventArgs e)
		{
			OpenFileDialog openFileDialog = new OpenFileDialog();
			openFileDialog.InitialDirectory = ".\\";
			openFileDialog.Filter = "可导入文件类型|*.xlsx;*.xls";
			openFileDialog.FilterIndex = 0;
			openFileDialog.RestoreDirectory = false;
			string errStr = "";
			if (openFileDialog.ShowDialog() == DialogResult.OK)
			{
				if (!AsposeExcelTools.ExcelSheetToDataTable(openFileDialog.FileName, "读写参数", 0, 0, out dt_RW, out errStr))
				{
					MessageBox.Show("导入读写参数方案失败：" + errStr);
					return;
				}
				dgv_RW.DataSource = dt_RW;
			}
		}

		private void btn_导出_Click(object sender, EventArgs e)
		{
			SaveFileDialog saveFileDialog1 = new SaveFileDialog();
			saveFileDialog1.Filter = "Excel文件（*.xlsx）|*.xlsx|Excel文件（*.xls）|*.xls";
			saveFileDialog1.FilterIndex = 0;
			saveFileDialog1.RestoreDirectory = false;
			string errStr = "";
			if (saveFileDialog1.ShowDialog() == DialogResult.OK)
			{
				List<DataTable> dts = new List<DataTable>();
				dts.Add(dt_RW);

				if (!AsposeExcelTools.DataTableListToExcel(dts, saveFileDialog1.FileName, new string[] { "读写参数" }, out errStr))
				{
					MessageBox.Show("导出读写参数方案失败：" + errStr);
					return;
				}
			}
		}

		private void btn_清空全部_Click(object sender, EventArgs e)
		{
			dt_RW.Rows.Clear();
			dgv_RW.DataSource = dt_RW;
		}

		private void btn_读取版本_Click(object sender, EventArgs e)
		{
			GetMeterVersion();
		}

		private bool is多回路 = true;
		private void Channel_box_CheckedChanged(object sender, EventArgs e)
		{
			is多回路 = Channel_box.Checked;
		}

		private void ReceieveMinus33_Click(object sender, EventArgs e)
		{
			rtb_log.Focus();
			string strData = DataConvert.fnGetStringFormat(rtb_log.SelectedText.ToString());
			if (strData == "")
			{
				MessageBox.Show("选取的数据为空!");
				return;
			}
			strData = fnDecodingDataMinus33(strData);//minus 33
													 //strData = fnSetStringAddSpace(strData);//add space
			rtb_log.SelectedText = strData;
		}

		private void btnInput_Click(object sender, EventArgs e)
		{
			listHex.Clear();
			blistHex.Clear();
			if (openFileDialog1.ShowDialog() == DialogResult.OK)
			{
				tbPath.Text = openFileDialog1.FileName;
			}
			try
			{
				using (FileStream fs = new FileStream(openFileDialog1.FileName, FileMode.Open, FileAccess.Read))
				{
					while (true)
					{
						int tempB = 0;
						try
						{
							tempB = fs.ReadByte();
							if (tempB == -1)
							{
								break;
							}
							listHex.Add(Convert.ToByte(tempB));
							blistHex.Add(Convert.ToByte(tempB));
						}
						catch (Exception exx)
						{
							MessageBox.Show(exx.Message);
							return;
						}
					}
				}
			}
			catch (Exception exx)
			{
				MessageBox.Show(exx.Message);
				return;
			}

			int hexLength = listHex.Count;
			byte[] TempBuf = new byte[240 * 1024];

			for (int i = 0; i < 240 * 1024; i++)
			{
				TempBuf[i] = 0xff;
			}
			listHex.CopyTo(0, TempBuf, 0, hexLength);

			uint ImageCrc1 = UpdataCRC32.CalcCRC32bySoftforUpgrade(0, TempBuf, 240 * 1024);//计算总文件校验 240k

			int num1 = hexLength % 1024;//将发送的固件进行1024补齐，方便电表处理

			if (num1 != 0)
			{
				int num2 = 1024 - num1;//计算需要补多少个FF
				for (int i = 0; i < num2; i++)
				{
					listHex.Add(0xff);
				}
			}
			uint ImageCrc = UpdataCRC32.CalcCRC32bySoftforUpgrade(0, TempBuf, listHex.Count);//计算发给电表的补齐1024的文件的校验，这个校验会随固件下发

			tbInfo.Text = "FileLength= " + listHex.Count.ToString() + " bytes  " + "CRC= 0x" + ImageCrc.ToString("X8");
			listHex.Add((byte)(ImageCrc >> 24));
			listHex.Add((byte)(ImageCrc >> 16));
			listHex.Add((byte)(ImageCrc >> 8));
			listHex.Add((byte)(ImageCrc >> 0));//最后4个字节为校验
		}

		private void btnErase_Click(object sender, EventArgs e)
		{
			if (SpMeter.Working) return;
			int t = 0;
			byte[] TxBufFactoryMode = { 0xfe, 0xfe, 0xfe, 0xfe, 0x68, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x68, 0x14, 0x15, 0x0E, 0x12, 0x12, 0x37, 0x35, 0x33, 0x33, 0x33, 0x37, 0x36, 0x35, 0x34, 0x34, 0x35, 0x12, 0xE, 0xBB, 0xBB, 0xBB, 0xBB, 0x34, 0xdf, 0x16 };
			byte[] TxBufUpdate = { 0xfe, 0xfe, 0xfe, 0xfe, 0x68, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x68, 0x14, 0x29, 0x0e, 0x12, 0x12, 0x37, 0x35, 0x33, 0x33, 0x33, 0x37, 0x36, 0x35, 0x34, 0x35, 0x43, 0x12, 0x0e, 0xbb, 0xbb, 0xbb, 0xbb, 0x27, 0x34, 0xdd, 0x20, 0x34, 0xdd, 0x93, 0x8e, 0x36, 0x33, 0x83, 0xf6, 0x33, 0x33, 0x13, 0xfb, 0x43, 0x33, 0xdd, 0xdd, 0xdd, 0xbb, 0x16 };
			byte[] ReceBuf = null;

			if (sendOK(TxBufFactoryMode, 0x94))
			{
				output(" Open Factory Mode Success", false, true);
			}
			else
			{
				output(" Meter reply error,Check the connection please", false, true);
			}

			if (sendOK(TxBufUpdate, 0xff))
			{
				output("请按键进入编程状态，开始升级", true, true);
			}
			else
			{
				output("擦除成功", false, true);//Enter Erase Mode Success, Pleae wait for LCD display READY
			}
		}

		private void btnUpdate_Click(object sender, EventArgs e)
		{
			progressBar1.Maximum = listHex.Count / 1024;
			progressBar1.Value = 0;
			Thread thr = new Thread(ThreadFun);
			thr.IsBackground = true;
			thr.Start();
		}

		private void btnInitializing_Click(object sender, EventArgs e)
		{
			byte[] TxBufFactoryMode = { 0xfe, 0xfe, 0xfe, 0xfe, 0x68, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x68, 0x14, 0x15, 0x0E, 0x12, 0x12, 0x37, 0x35, 0x33, 0x33, 0x33, 0x37, 0x36, 0x35, 0x34, 0x34, 0x35, 0x12, 0xE, 0xBB, 0xBB, 0xBB, 0xBB, 0x34, 0xdf, 0x16 };
			byte[] TxBufInitial = { 0xfe, 0xfe, 0xfe, 0xfe, 0x68, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x68, 0x14, 0x14, 0x0E, 0x12, 0x12, 0x37, 0x35, 0x33, 0x33, 0x33, 0x37, 0x36, 0x35, 0x34, 0xE, 0x3A, 0x12, 0xE, 0xBB, 0xBB, 0xBB, 0xBB, 0x89, 0x16 };

			if (sendOK(TxBufInitial, 0x94))
			{
				output("初始化电表成功", false, true);//Initial Meter Success
			}
			else
			{
				output("通信异常,请检查", true, true);//Meter reply error,Check the connection please
			}
		}

		private void btnVersion_Click(object sender, EventArgs e)
		{
			GetMeterVersion();
		}

		private void btnOuput_Click(object sender, EventArgs e)
		{
			SaveFileDialog saveFileDialog1 = new SaveFileDialog();
			saveFileDialog1.Filter = "BIN文件（*.bin）|*.bin";
			saveFileDialog1.FilterIndex = 0;
			saveFileDialog1.RestoreDirectory = false;
			string errStr = "";
			string strLength = "";
			int hexLength = blistHex.Count;
			byte[] TempBuf = null;
			int iCount1K = (blistHex.Count / 1024);
			if (blistHex.Count % 1024 != 0)
			{
				iCount1K = ((blistHex.Count / 1024 + 1));
				strLength = ((blistHex.Count / 1024 + 1) * 1024 - 1).ToString("X");
			}

			TempBuf = new byte[iCount1K * 1024];
			for (int i = 0; i < iCount1K * 1024; i++)
			{
				TempBuf[i] = 0xff;
			}
			blistHex.CopyTo(0, TempBuf, 0, blistHex.Count);

			uint ImageCrc1 = UpdataCRC32.CalcCRC32bySoftforUpgrade(0, TempBuf, iCount1K * 1024);//计算总文件校验 240k
			ImageCrc1 = UInt32.Parse(Helps.fnAgainst(ImageCrc1.ToString("X")), NumberStyles.AllowHexSpecifier);
			if (saveFileDialog1.ShowDialog() == DialogResult.OK)
			{
				Helps.CombineMethod(saveFileDialog1.FileName, tbPath.Text.ToString(), strLength, ImageCrc1);
			}
		}

		private void btnMeterClear_Click(object sender, EventArgs e)
		{
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}
			//DateTime.Now
			DateTime dt = DateTime.Now.AddMonths(1);
			string strDateTime = dt.ToString("yyMMddHHmmss");
			//"FEFEFEFE" +
			byte[] bByteMeterClear = new byte[32];
			string strDataSend = "68" + "888888888888" + "68" + "1A10" + (DataConvert.fnEncodingData(pmc[8].Value.ToString() + pmc[7].Value.ToString())) + "22d222404d33" + DataConvert.fnEncodingData((strDateTime));
			Helps.TryStrToHexByte(strDataSend, out bByteMeterClear);
			#region 校验和+16
			byte sum = Helps.Ssum(bByteMeterClear);
			byte[] endBytes = new[] { sum, (byte)0x16 };
			#endregion
			bByteMeterClear = bByteMeterClear.Concat(endBytes).ToArray();

			if (sendOK(bByteMeterClear, 0x9A))
			{
				output(" 电表清零成功", false, true);
			}
			else
			{
				output(" 电表清零失败!,请检查电表时间是否正常", true, true);
			}
		}

		private void btReadTopoTime_Click(object sender, EventArgs e)
		{
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}
			DataTable DtSource = null;
			DtSource = DataBank.Find(x => x.TableName == "拓扑事件记录").Copy();
			if (DtSource == null)
			{
				return;
			}
			byte[] bSendBytes = new byte[32];//tbCount
			string strDataSend = "68" + Helps.fnAgainst(pmc[6].Value.ToString()) + "68110D4b333b3c" + DataConvert.fnEncodingData(Int16.Parse(tbCount.Text.ToString()).ToString("x2")) + DataConvert.fnEncodingData(Int16.Parse(tbNumber.Text.ToString()).ToString("X4").PadLeft(4, '0')) + DataConvert.fnEncodingData(Int16.Parse(tbSec.Text.ToString()).ToString("x2")) + DataConvert.fnEncodingData(Int16.Parse(tbMin.Text.ToString()).ToString("x2")) + DataConvert.fnEncodingData(Int16.Parse(tbHour.Text.ToString()).ToString("x2")) + DataConvert.fnEncodingData(Int16.Parse(tbDay.Text.ToString()).ToString("x2")) + DataConvert.fnEncodingData(Int16.Parse(tbMon.Text.ToString()).ToString("x2")) + DataConvert.fnEncodingData(Int16.Parse(tbYear.Text.ToString()).ToString("x2"));
			Helps.TryStrToHexByte(strDataSend, out bSendBytes);

			#region 校验和+16
			byte sum = Helps.Ssum(bSendBytes);
			byte[] endBytes = new[] { sum, (byte)0x16 };
			#endregion
			bSendBytes = bSendBytes.Concat(endBytes).ToArray();
			output("TX:" + Helps.ByteToStringWithSpace(bSendBytes), false, true);
			byte[] recBytes = SpMeter.SendCommand(bSendBytes, "645");
			output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
			DLT645 dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
			List<string> dataMarkList = new List<string>();
			string dataPianDuan, strErr;
			bool isHaveHouxu;
			dataMarkList.Add("09080018");
			if (!dlt645.UnPack(recBytes, DLT645.ControlType.Read, dataMarkList, 0, out isHaveHouxu, out dataPianDuan, out strErr))
			{
				output(strErr, true, true);
				return;
			}
			dataPianDuan = Helps.fnAgainst(dataPianDuan);
			output((dataPianDuan), false, false);
			int len1 = 0;
			if (dataPianDuan.Length > 24)
			{
				for (int i = 0; i < 10; i++)
				{
					if (!int.TryParse(DtSource.Rows[i]["DataLen"].ToString(), out len1))
					{
						//Text = "数据库存储的数据长度有误，请检查数据库！";
						return;
					}

					string dataStr = "";

					dataStr = dataPianDuan.Substring(0, 2 * len1);

					switch (DtSource.Rows[i]["DataFormat"])
					{
						case "BCD":
							if (!int.TryParse(DtSource.Rows[i]["DecimalPlaces"].ToString(), out int decimalPlaces))
							{
								decimalPlaces = 0;
							}

							dataStr = Helps.fnAgainst(dataStr);
							if (decimalPlaces != 0)
							{
								dataStr = dataStr.Insert(dataStr.Length - decimalPlaces, ".");
							}
							DtSource.Rows[i]["Value0"] = dataStr;
							break;
						case "HEX10":
							UInt64 dataStrHEX10;
							if (!int.TryParse(DtSource.Rows[i]["DecimalPlaces"].ToString(), out int decimalPlace))
							{
								decimalPlace = 0;
							}
							dataStr = Helps.fnAgainst(dataStr);
							dataStrHEX10 = UInt64.Parse(dataStr, System.Globalization.NumberStyles.HexNumber);

							if (dataStrHEX10 != 0)
							{
								dataStr = dataStrHEX10.ToString();
							}

							if (decimalPlace != 0)
							{
								if (dataStr.Length == decimalPlace)
								{
									dataStr = dataStr.Insert(dataStr.Length - decimalPlace, "0.");
								}
								else
								{
									if (dataStr.Length < decimalPlace)
									{
										dataStr = dataStr.PadLeft((decimalPlace + 1), '0');
									}
									dataStr = dataStr.Insert(dataStr.Length - decimalPlace, ".");
								}
							}

							DtSource.Rows[i]["Value0"] = dataStr;
							break;
						default://一律按hex16格式处理;
								//dataStr = Helps.fnAgainst(dataStr);
							DtSource.Rows[i]["Value0"] = dataStr;
							break;
					}
					dataPianDuan = dataPianDuan.Substring(2 * len1);
				}

				string[] stradataPart = new String[dataPianDuan.Length / 66];
				for (int ii = 0; ii < stradataPart.Length; ii++)
				{
					stradataPart[ii] = dataPianDuan.Substring(66 * ii, 66);

					dgvTopoShow.DataSource = DtSource;
					len1 = 0;
					for (int i = 10; i < DtSource.Rows.Count; i++)
					{
						if (!int.TryParse(DtSource.Rows[i]["DataLen"].ToString(), out len1))
						{
							//Text = "数据库存储的数据长度有误，请检查数据库！";
							return;
						}

						string dataStr = "";
						if (DtSource.Rows[i]["DataFormat"].ToString().Contains("bit"))
						{
							dataStr = stradataPart[ii].Substring(0, len1);
						}
						else
						{
							dataStr = stradataPart[ii].Substring(0, 2 * len1);
						}

						switch (DtSource.Rows[i]["DataFormat"])
						{
							case "BCD":
								if (!int.TryParse(DtSource.Rows[i]["DecimalPlaces"].ToString(), out int decimalPlaces))
								{
									decimalPlaces = 0;
								}

								dataStr = Helps.fnAgainst(dataStr);
								if (decimalPlaces != 0)
								{
									dataStr = dataStr.Insert(dataStr.Length - decimalPlaces, ".");
								}

								if (dataStr == "00")
								{
									DtSource.Rows[i]["Value" + ii.ToString()] = "未知";
								}
								else if (dataStr == "01")
								{
									DtSource.Rows[i]["Value" + ii.ToString()] = "A相";
								}
								else if (dataStr == "02")
								{
									DtSource.Rows[i]["Value" + ii.ToString()] = "B相";
								}
								else if (dataStr == "03")
								{
									DtSource.Rows[i]["Value" + ii.ToString()] = "C相";
								}
								else if (dataStr == "04")
								{
									DtSource.Rows[i]["Value" + ii.ToString()] = "三相";
								}
								break;
							case "HEX10":
								UInt64 dataStrHEX10;
								if (!int.TryParse(DtSource.Rows[i]["DecimalPlaces"].ToString(), out int decimalPlace))
								{
									decimalPlace = 0;
								}
								dataStr = Helps.fnAgainst(dataStr);
								dataStrHEX10 = UInt64.Parse(dataStr, System.Globalization.NumberStyles.HexNumber);

								if (dataStrHEX10 != 0)
								{
									dataStr = dataStrHEX10.ToString();
								}

								if (decimalPlace != 0)
								{
									if (dataStr.Length == decimalPlace)
									{
										dataStr = dataStr.Insert(dataStr.Length - decimalPlace, "0.");
									}
									else
									{
										if (dataStr.Length < decimalPlace)
										{
											dataStr = dataStr.PadLeft((decimalPlace + 1), '0');
										}
										dataStr = dataStr.Insert(dataStr.Length - decimalPlace, ".");
									}
								}

								DtSource.Rows[i]["Value" + ii.ToString()] = dataStr;
								break;
							case "ASCII":
								string strData = "";
								byte[] bData = DataConvert.HEXString2Bytes(dataStr);
								for (int j = (dataStr.Length / 2); j > 0; j--)
								{
									string strValue = Char.ConvertFromUtf32(bData[j - 1]);

									strData += strValue;
								}

								DtSource.Rows[i]["Value" + ii.ToString()] = strData;
								break; ;
							case "BIN":
								stradataPart[ii] = Helps.fnAgainst(stradataPart[ii]);
								stradataPart[ii] = DataConvert.HexStringToBinary(stradataPart[ii]);
								DtSource.Rows[i]["Value" + ii.ToString()] = dataStr;
								break;
							case "bit":
							case "bits":
								dataStr = new string(dataStr.ToCharArray().Reverse().ToArray());
								DtSource.Rows[i]["Value" + ii.ToString()] = dataStr;
								break;
							case "ssmmhhDDMMYY":
								DtSource.Rows[i]["Value" + ii.ToString()] = dataStr;
								break;
							case "Float":
								UInt32 x = Convert.ToUInt32(Helps.fnAgainst(dataStr), 16);//字符串转16进制32位无符号整数
								float fy = BitConverter.ToSingle(BitConverter.GetBytes(x), 0);//IEEE754 字节转换float//得出数字 3236.725
								DtSource.Rows[i]["Value" + ii.ToString()] = fy.ToString();
								break;
							default://一律按hex16格式处理;
									//dataStr = Helps.fnAgainst(dataStr);
								DtSource.Rows[i]["Value" + ii.ToString()] = dataStr;
								break;
						}
						if (DtSource.Rows[i]["DataFormat"].ToString().Contains("bit"))
						{
							stradataPart[ii] = stradataPart[ii].Substring(len1);
						}
						else
						{
							stradataPart[ii] = stradataPart[ii].Substring(2 * len1);
						}
					}
				}
			}
			else
			{
				output("抄读拓扑历史记录为空");
			}
		}

		#region SearchTab
		private void btnSarch_Click(object sender, EventArgs e)
		{
			if (string.IsNullOrEmpty(tbSearch.Text.Trim()))
				return;
			treeView1.CollapseAll();
			string searchStr = tbSearch.Text.Trim();
			DataTable dt = Helps.GetNewDataTable(dtL2, (string.Format("DataMark = '{0}' or Name like '%{0}%'", searchStr)));
			FindTreeNode(treeView1.Nodes, dt);
		}

		/// <summary>
		/// FindTreeNode
		/// </summary>
		/// <param name="node"></param>
		/// <param name="dt"></param>
		private void FindTreeNode(TreeNodeCollection node, DataTable dt)
		{
			foreach (TreeNode n in node)
			{
				if (n.Nodes.Count > 0)
				{
					FindTreeNode(n.Nodes, dt);
				}
				else if (n.Tag != null)
				{
					DataRow[] drs1 = dt.Select(string.Format("ID = '{0}'", n.Tag));
					if (drs1.Length > 0)
					{
						treeView1.SelectedNode = n;
						n.BackColor = Color.LightSkyBlue;
					}
					else
					{
						n.BackColor = SystemColors.Window;
					}
				}
			}
		}
		#endregion

		#region PasteAndExplain
		/// <summary>
		/// 字符串数据
		/// </summary>
		string strPasteData = "";
		byte[] baPasteData;
		List<string> resultList = new List<string>();
		List<string> strDataList = new List<string>();

		/// <summary>
		/// PasteAndExplain
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void btnPasteExplain_Click(object sender, EventArgs e)
		{
			//清空
			rtbExplain.Text = null;
			string strErr = "";
			dgvExplain.Rows.Clear();
			resultList = new List<string>();
			strDataList = new List<string>();

			// Declares an IDataObject to hold the data returned from the clipboard.
			// Retrieves the data from the clipboard.
			System.Windows.Forms.IDataObject iData = Clipboard.GetDataObject();

			// Determines whether the data is in a format you can use.
			if (iData.GetDataPresent(DataFormats.Text))
			{
				// Yes it is, so display it in a rtbExplain.
				strPasteData = (String)iData.GetData(DataFormats.Text);
				//去除前导字节
				if (strPasteData.Contains("FE FE FE FE ") || strPasteData.Contains("fe fe fe fe "))
				{
					strPasteData = strPasteData.Substring(12);
				}
				rtbExplain.Text = strPasteData;
			}

			strPasteData = DataConvert.fnGetStringFormat(strPasteData);

			if (strPasteData.Length % 2 != 0)
			{
				MessageBox.Show("粘贴报文格式长度错误! 请检查!");
				return;
			}

			Helps.TryStrToHexByte(strPasteData, out baPasteData);
			if (UnPackTable(baPasteData, ref resultList, ref strErr))
			{
				foreach (string strTemp in resultList)
				{
					object[] results = strTemp.Split('|');
					dgvExplain.Rows.Add(results);
				}
			}
			else
			{
				MessageBox.Show("解析失败：" + strErr);
			}
		}

		/// <summary>
		/// 判断执行是否成功，解析数据域-33
		/// </summary>
		/// <param name="recBytes">收到的报文</param>
		/// <param name="resultList"></param>
		/// <param name="errStr"></param>
		/// <returns></returns>
		public bool UnPackTable(byte[] recBytes, ref List<string> resultList, ref string errStr)
		{
			resultList.Clear();
			if (recBytes == null)
			{
				errStr += "报文为空！";
				return false;
			}

			#region Variable
			errStr = "失败!";
			string dataField = "";
			string[] dataMarks = new string[2];
			int iDataMarkCount = 1;
			string strControl = "";
			string strPassword = "";
			string strOperate = "";
			string strMins = "";
			#endregion

			#region JudgeLength
			int iHead = 0;
			while (iHead < recBytes.Length)
			{
				if (recBytes[iHead] != (byte)0x68)
				{
					iHead++;
				}
				else
				{
					break;
				}
			}

			if (iHead > recBytes.Length)
			{
				return false;
			}
			else if (recBytes.Length - iHead < 12)
			{
				return false;
			}

			int iEnd = recBytes.Length - 1;
			while (iEnd > iHead)
			{
				if (recBytes[iEnd] != (byte)0x16)
				{
					iEnd--;
				}
				else
				{
					break;
				}
			}

			if (iEnd <= iHead)
			{
				return false;
			}
			else if ((recBytes[iHead] != (byte)0x68) || (recBytes[iHead + 7] != (byte)0x68))
			{
				return false;
			}
			#endregion

			#region CheckCRC
			byte[] recByte = new byte[iEnd - iHead + 1];
			byte[] baSum = new byte[iEnd - iHead - 1];
			Array.Copy(recBytes, iHead, recByte, 0, recByte.Length);
			Array.Copy(recBytes, iHead, baSum, 0, baSum.Length);
			byte bSum = Helps.Ssum(baSum);
			if (bSum != recBytes[iEnd - 1])
			{
				errStr = "报文校验错误, 校验为:" + bSum.ToString("X2");
				return false;
			}
			#endregion

			string strAddr = DataConvert.ReverseBytesToHexStringWithoutSpace(recByte, 1, 6);
			resultList.Add("帧起始符|" + recByte[0].ToString("X2") + "|1" + "| " + "| ");
			resultList.Add("地址域|" + strAddr + "|6" + "| " + "| ");
			resultList.Add("帧起始符|" + recByte[7].ToString("X2") + "|1" + "| " + "| ");

			#region DataSubtract
			for (int i = 10; i < (recByte[9] + 10); i++)
			{
				recByte[i] -= 0x33;
			}
			#endregion

			#region DataMarkProcess
			if ((recByte[8] == 0x91 || (recByte[8] == 0x14) || (recByte[8] == 0x11)))
			{
				dataMarks[0] = DataConvert.ReverseBytesToHexStringWithoutSpace(recByte, 10, 4);
				if (dataMarks[0] == "04DFDFDB")
				{
					dataMarks[1] = "04DFDFDB";
					if (recByte[8] == 0x14)
					{
						dataMarks[0] = DataConvert.ReverseBytesToHexStringWithoutSpace(recByte, 22, 4);
					}
					else
					{
						dataMarks[0] = "DBDF" + recByte[15].ToString("X2") + recByte[14].ToString("X2");
					}
					iDataMarkCount = 2;
				}
			}
			else
			{
				iDataMarkCount = 0;
			}
			#endregion

			try
			{
				switch (recByte[8])
				{
					case 0x91:
						strControl = "正常读取";
						if (recByte[9] < 4)
						{
							errStr = "报文数据域长度非法！";
							return false;
						}

						switch (iDataMarkCount)
						{
							case 1:
								string sTemp = recByte[13].ToString("X2") + recByte[12].ToString("X2") + recByte[11].ToString("X2") + recByte[10].ToString("X2");
								if (dataMarks[0] != recByte[13].ToString("X2") + recByte[12].ToString("X2") + recByte[11].ToString("X2") + recByte[10].ToString("X2"))
								{
									errStr = "报文数据标识错误！";
									return false;
								}
								byte[] dataBytes = new byte[recByte[9] - 4];
								Array.Copy(recByte, 14, dataBytes, 0, dataBytes.Length);
								Array.Reverse(dataBytes);
								dataField = Helps.fnAgainst(Helps.ByteToStringWithNoSpace(dataBytes));
								break;
							case 2:
								if ((dataMarks[1] != recByte[13].ToString("X2") + recByte[12].ToString("X2") + recByte[11].ToString("X2") + recByte[10].ToString("X2")) || (dataMarks[0].Substring(4) != recByte[15].ToString("X2") + recByte[14].ToString("X2")))
								{
									errStr = "报文数据标识错误！";
									return false;
								}
								dataBytes = new byte[recByte[9] - 6];
								Array.Copy(recByte, 16, dataBytes, 0, dataBytes.Length);
								Array.Reverse(dataBytes);
								dataField = Helps.fnAgainst(Helps.ByteToStringWithNoSpace(dataBytes));
								break;
							default:
								errStr = "数据标识有误！";
								return false;
						}
						break;
					case 0x11:
						switch (iDataMarkCount)
						{
							case 1:
								byte[] dataBytes = new byte[recByte[9] - 4];
								Array.Copy(recByte, 14, dataBytes, 0, dataBytes.Length);
								Array.Reverse(dataBytes);
								dataField = Helps.fnAgainst(Helps.ByteToStringWithNoSpace(dataBytes));
								break;
							case 2:
								dataBytes = new byte[recByte[9] - 8];
								Array.Copy(recByte, 18, dataBytes, 0, dataBytes.Length);
								Array.Reverse(dataBytes);
								dataField = Helps.fnAgainst(Helps.ByteToStringWithNoSpace(dataBytes));
								break;
							default:
								errStr = "数据标识有误！";
								return false;
						}
						strControl = "读取请求";
						break;
					case 0xd1:
						//TODO:解析失败原因
						strControl = "异常读取";
						errStr = strControl;
						dataField = (recByte[10]).ToString();
						break;
					case 0x14:
						strControl = "设置请求";
						if (recByte[9] < 12)
						{
							errStr = "报文数据域长度非法！";
							return false;
						}

						switch (iDataMarkCount)
						{
							case 1:
								strPassword = recByte[14].ToString("X2") + recByte[17].ToString("X2") + recByte[16].ToString("X2") + recByte[15].ToString("X2");
								strOperate = recByte[21].ToString("X2") + recByte[20].ToString("X2") + recByte[19].ToString("X2") + recByte[18].ToString("X2");

								byte[] dataBytes = new byte[recByte[9] - 12];
								Array.Copy(recByte, 22, dataBytes, 0, dataBytes.Length);
								Array.Reverse(dataBytes);
								dataField = Helps.fnAgainst(Helps.ByteToStringWithNoSpace(dataBytes));
								break;
							case 2:
								strPassword = recByte[14].ToString("X2") + recByte[17].ToString("X2") + recByte[16].ToString("X2") + recByte[15].ToString("X2");
								strOperate = recByte[21].ToString("X2") + recByte[20].ToString("X2") + recByte[19].ToString("X2") + recByte[18].ToString("X2");
								strMins = recByte[29].ToString("X2") + recByte[28].ToString("X2") + recByte[27].ToString("X2") + recByte[26].ToString("X2");

								dataBytes = new byte[recByte[9] - 20];
								Array.Copy(recByte, 30, dataBytes, 0, dataBytes.Length);
								Array.Reverse(dataBytes);
								dataField = Helps.fnAgainst(Helps.ByteToStringWithNoSpace(dataBytes));
								break;
							default:
								errStr = "数据标识有误！";
								return false;
						}
						break;
					case 0x94:
						strControl = "正常设置";
						if (recByte[9] != 00)
						{
							errStr = "报文数据域非法！";
						}
						break;
					case 0xD4:
						//TODO:解析失败原因
						strControl = "异常设置";
						errStr = strControl;
						byte[] baData = new byte[recByte[9]];
						Array.Copy(recByte, 10, baData, 0, baData.Length);
						Array.Reverse(baData);
						dataField = Helps.fnAgainst(Helps.ByteToStringWithNoSpace(baData));
						break;
					default:
						baData = new byte[recByte.Length - 12];
						Array.Copy(recByte, 10, baData, 0, baData.Length);
						Array.Reverse(baData);
						dataField = Helps.fnAgainst(Helps.ByteToStringWithNoSpace(baData));
						break;
						//return true;
				}
				resultList.Add("控制码|" + recByte[8].ToString("X2") + "(" + strControl + ")" + "|1" + "| " + "| ");
				resultList.Add("数据域长度|" + recByte[9].ToString() + "|1" + "| " + "| ");

				if (iDataMarkCount != 0)
				{
					DataTable dtData = new DataTable();
					DataTable dt = DataBank.Find(x => x.TableName == "level2");
					DataRow[] drs = dt.Select(string.Format("DataMark= '{0}' ", dataMarks[0]));
					string strDatamarkName = "";
					if (drs.Length != 0)
					{
						strDatamarkName = drs[0]["Name"].ToString();
					}
					else
					{
						strDatamarkName = "Data中无<" + dataMarks[0] + ">数据标识";
					}

					if (iDataMarkCount == 2)
					{
						resultList.Add("扩展数据标识|" + dataMarks[1] + "|4" + "| " + "| ");
					}

					if (recByte[8] == 0x14)
					{
						if (iDataMarkCount == 1)
						{
							resultList.Add("数据标识(" + strDatamarkName + ")|" + dataMarks[0] + "|4" + "| " + "| ");
						}
						resultList.Add("密码|" + strPassword + "|4" + "| " + "| ");
						resultList.Add("操作者代码|" + strOperate + "|4" + "| " + "| ");

						if (iDataMarkCount == 2)
						{
							resultList.Add("数据标识(" + strDatamarkName + ")|" + dataMarks[0] + "|4" + "| " + "| ");
							resultList.Add("相对分钟数|" + strMins + "|4" + "| " + "| ");
						}
					}
					else
					{
						if ((iDataMarkCount == 2) && (recByte[8]) == 0x91)
						{
							resultList.Add("数据标识(" + strDatamarkName + ")|" + dataMarks[0].Substring(4) + "|2" + "| " + "| ");
						}
						else
						{
							resultList.Add("数据标识(" + strDatamarkName + ")|" + dataMarks[0] + "|4" + "| " + "| ");
						}
					}

					if (drs.Length == 0)
					{
						errStr = ("Data中无<" + dataMarks[0] + ">数据标识");
						if (dataField.Length > 0)
						{
							resultList.Add("数据|" + dataField + "| " + (dataField.Length / 2).ToString() + "| " + "| ");
						}
					}
					else
					{
						if (drs[0]["DataBlockType"].ToString() != "")
						{
							if (recByte[9] > 4)
							{
								if ((iDataMarkCount == 2) && (recByte[9] < 9))
								{

								}
								else
								{
									if (recByte[8] == 0x11)
									{
										dtData = DataBank.Find(x => x.TableName == ("读" + drs[0]["DataBlockType"].ToString()));
									}
									else
									{
										dtData = DataBank.Find(x => x.TableName == drs[0]["DataBlockType"].ToString());
									}

									if (dtData == null)
									{
										errStr = "未查找到 sheet: 读" + drs[0]["DataBlockType"].ToString();
										return false;
									}

									if (UnPackDataTable(dataField, dtData, ref strDataList, ref errStr))
									{
										foreach (string t in strDataList)
										{
											resultList.Add(t);
										}
									}
									else
									{
										return false;
									}
								}
							}
						}
						else
						{
							if (strControl != "读取请求")
							{
								if (GetExplainData(drs[0], Helps.fnAgainst(dataField), dataField.Length, ref strDataList, ref errStr))
								{
									foreach (string t in strDataList)
									{
										resultList.Add(t);
									}
								}
								else
								{
									return false;
								}
							}
						}
					}
				}
				else
				{
					if ((recByte[8] & 0x40) == 0x40)
					{
						resultList.Add("错误码|" + dataField + "|1" + "| " + "| ");
					}
					else
					{
						resultList.Add("数据|" + dataField + "|" + (dataField.Length / 2) + "| " + "| ");
					}
				}

				resultList.Add("校验码|" + bSum.ToString("X2") + "|1" + "| " + "| ");
				resultList.Add("结束符|" + "16" + "|1" + "| " + "| ");
				errStr = "";
				return true;
			}
			catch (Exception e)
			{
				errStr += e.Message;
				return false;
			}
		}

		/// <summary>
		/// 解析块
		/// </summary>
		/// <param name="_valueStr"></param>
		/// <param name="DtSource"></param>
		/// <param name="strDataList"></param>
		/// <param name="errStr"></param>
		/// <returns></returns>
		public bool UnPackDataTable(string _valueStr, DataTable DtSource, ref List<string> strDataList, ref string errStr)
		{
			int len1 = 0;
			string brakestatus = "";
			string strBitData = "";
			bool bBit = false;

			if (!string.IsNullOrEmpty(_valueStr))
			{
				int len = 0;
				int iJudgeRow = 0;

				for (int i = 0; i < DtSource.Rows.Count; i++)
				{
					if (DtSource.Rows[i]["DataFormat"].ToString().Contains("bit"))
					{
						//if (DtSource.Rows[i]["Name"].ToString().Contains("Bit7"))
						//{
						//	bBit = true;
						//	int iRows = 0;
						//	strBitData = DataConvert.HexStringToBinary(_valueStr.Substring(i * 2, 2));

						//	_valueStr = _valueStr.Substring(0, i * 2) + strBitData + _valueStr.Substring(i * 2 + 2);
						//	for (int j = i; j < DtSource.Rows.Count; j++)
						//	{
						//		if (DtSource.Rows[j]["DataFormat"].ToString().Contains("bit"))
						//		{
						//			if (!int.TryParse(DtSource.Rows[j]["DataLen"].ToString(), out len1))
						//			{

						//			}
						//			if (DtSource.Rows[j]["DecimalPlaces"].ToString() == ("判断"))
						//			{
						//				DataTable DtAnalyze = null;
						//				string strAnalyze = DtSource.Rows[j]["DataUnit"].ToString();
						//				DtAnalyze = winMain.DataBank.Find(x => x.TableName == strAnalyze);
						//				if (DtAnalyze == null)
						//				{
						//					return false;
						//				}
						//				string strTemp = (strBitData.Substring(0, len1));
						//				for (int k = 0; k < DtAnalyze.Rows.Count; k++)
						//				{
						//					if (strTemp == DtAnalyze.Rows[k]["Value"].ToString())
						//					{
						//						brakestatus = DtAnalyze.Rows[k]["Name"].ToString();
						//						iJudgeRow = j;
						//						break;
						//					}
						//				}
						//			}

						//			iRows += len1;
						//		}
						//		strBitData = strBitData.Substring(len1);
						//		if (iRows > 7)
						//		{
						//			i = j;
						//			len1 = 8;
						//			break;
						//		}
						//	}
						//}
						bBit = true;
						//if ( (DtSource.Rows[i]["Name"].ToString().Contains("Bit7") ) && (DtSource.Rows[i]["DataFormat"].ToString().Contains("bits") ) )
						//{
						//	int iRows = 0;

						//	strBitData = DataConvert.HexStringToBinary(_valueStr.Substring(i * 2, 2));

						//	_valueStr = _valueStr.Substring(0, i * 2) + strBitData + _valueStr.Substring(i * 2 + 2);
						//	for (int j = i; j < DtSource.Rows.Count; j++)
						//	{
						//		if (DtSource.Rows[j]["DataFormat"].ToString().Contains("bit"))
						//		{
						//			if (!int.TryParse(DtSource.Rows[j]["DataLen"].ToString(), out len1))
						//			{

						//			}
						//			if (DtSource.Rows[j]["DecimalPlaces"].ToString() == ("判断"))
						//			{
						//				DataTable DtAnalyze = null;
						//				string strAnalyze = DtSource.Rows[j]["DataUnit"].ToString();
						//				DtAnalyze = winMain.DataBank.Find(x => x.TableName == strAnalyze);
						//				if (DtAnalyze == null)
						//				{
						//					return;
						//				}
						//				string strTemp = (strBitData.Substring(0, len1));
						//				for (int k = 0; k < DtAnalyze.Rows.Count; k++)
						//				{
						//					if (strTemp == DtAnalyze.Rows[k]["Value"].ToString())
						//					{
						//						brakestatus = DtAnalyze.Rows[k]["Name"].ToString();
						//						iJudgeRow = j;
						//						break;
						//					}
						//				}
						//			}

						//			iRows += len1;
						//		}
						//		strBitData = strBitData.Substring(len1);
						//		if (iRows > 7)
						//		{
						//			i = j;
						//			len1 = 8;
						//			break;
						//		}
						//	}
						//}
						//else
						{
							if (i == 0)
							{
								if (DtSource.TableName == "主动上报状态字块")
								{
									_valueStr = Helps.fnAgainst(_valueStr);
								}
								strBitData = DataConvert.HexStringToBinary(_valueStr);
								_valueStr = strBitData;
							}
						}
						//_valueStr = new string(_valueStr.ToCharArray().Reverse().ToArray());
					}
					else
					{
						if (DtSource.Rows[i]["DataLen"].ToString() == "")
						{
							if (string.IsNullOrEmpty(DtSource.Rows[i]["DataLen"].ToString()))
							{

							}
							continue;
						}
						else
						{
							if (!int.TryParse(DtSource.Rows[i]["DataLen"].ToString(), out len1))
							{
								Text = "数据库存储的数据长度有误，请检查数据库！";
								return false;
							}
						}
					}

					len += len1;
				}

				for (int i = 0; i < DtSource.Rows.Count; i++)
				{
					if (_valueStr == "")
					{
						return true;
					}
					if (!int.TryParse(DtSource.Rows[i]["DataLen"].ToString(), out len1))
					{
						//Text = "数据库存储的数据长度有误，请检查数据库！";
						return false;
					}

					string dataStr = "";
					if (DtSource.Rows[i]["DataFormat"].ToString().Contains("bit"))
					{
						dataStr = _valueStr.Substring(0, len1);
					}
					else
					{
						dataStr = _valueStr.Substring(0, 2 * len1);
					}

					switch (DtSource.Rows[i]["DataFormat"])
					{
						case "BCD":
							if (!int.TryParse(DtSource.Rows[i]["DecimalPlaces"].ToString(), out int decimalPlaces))
							{
								decimalPlaces = 0;
							}
							dataStr = Helps.fnAgainst(dataStr);
							string strSymble = "";
							if ((DtSource.Rows[i]["Name"].ToString().Contains("功率因数")) || (DtSource.Rows[i]["DataUnit"].ToString() == "A") || (DtSource.Rows[i]["DataUnit"].ToString() == "kW") || (DtSource.Rows[i]["DataUnit"].ToString() == "Kvar") || (DtSource.Rows[i]["DataUnit"].ToString() == "kVA"))
							{
								int iTe = 0;

								if ((Convert.ToInt64("0x" + dataStr.Substring(0, 1), 16) & 0x08) == 8)
								{
									strSymble = "-";
								}
								iTe = (int)(Convert.ToInt64("0x" + dataStr.Substring(0, 1), 16) & 0x07);
								dataStr = strSymble + iTe.ToString() + dataStr.Substring(1);
							}

							if (decimalPlaces != 0)
							{
								dataStr = dataStr.Insert(dataStr.Length - decimalPlaces, ".");
							}
							strDataList.Add(DtSource.Rows[i]["Name"] + "|" + dataStr + "|" + DtSource.Rows[i]["DataLen"] + "| " + DtSource.Rows[i]["DecimalPlaces"] + "|" + DtSource.Rows[i]["DataUnit"]);
							//DtSource.Rows[i]["Value"] = dataStr;
							break;
						case "HEX10":
							UInt64 dataStrHEX10;
							if (!int.TryParse(DtSource.Rows[i]["DecimalPlaces"].ToString(), out int decimalPlace))
							{
								decimalPlace = 0;
							}
							dataStr = Helps.fnAgainst(dataStr);
							dataStrHEX10 = UInt64.Parse(dataStr, System.Globalization.NumberStyles.HexNumber);

							if (dataStrHEX10 != 0)
							{
								dataStr = dataStrHEX10.ToString();
							}

							if (decimalPlace != 0)
							{
								if (dataStr.Length == decimalPlace)
								{
									dataStr = dataStr.Insert(dataStr.Length - decimalPlace, "0.");
								}
								else
								{
									if (dataStr.Length < decimalPlace)
									{
										dataStr = dataStr.PadLeft((decimalPlace + 1), '0');
									}
									dataStr = dataStr.Insert(dataStr.Length - decimalPlace, ".");
								}

							}
							strDataList.Add(DtSource.Rows[i]["Name"] + "|" + dataStr + "|" + DtSource.Rows[i]["DataLen"] + "| " + DtSource.Rows[i]["DecimalPlaces"] + "|" + DtSource.Rows[i]["DataUnit"]);
							//DtSource.Rows[i]["Value"] = dataStr;
							break;
						case "ASCII":
							string strData = "";
							if (DtSource.Rows[i]["DataUnit"].ToString() == "Positive")
							{
								dataStr = Helps.fnAgainst(dataStr);
							}
							byte[] bData = DataConvert.HEXString2Bytes(dataStr);
							for (int j = (dataStr.Length / 2); j > 0; j--)
							{
								string strValue = Char.ConvertFromUtf32(bData[j - 1]);

								strData += strValue;
							}
							strDataList.Add(DtSource.Rows[i]["Name"] + "|" + strData + "|" + DtSource.Rows[i]["DataLen"] + "| " + DtSource.Rows[i]["DecimalPlaces"] + "|" + DtSource.Rows[i]["DataUnit"]);
							//DtSource.Rows[i]["Value"] = strData;
							break; ;
						case "BIN":
							dataStr = DataConvert.HexStringToBinary(dataStr);
							strDataList.Add(DtSource.Rows[i]["Name"] + "|" + dataStr + "|" + DtSource.Rows[i]["DataLen"] + "| " + DtSource.Rows[i]["DecimalPlaces"] + "|" + DtSource.Rows[i]["DataUnit"]);
							break;
						case "bit":
						case "bits":
							//dataStr = new string(dataStr.ToCharArray().Reverse().ToArray());
							if (brakestatus == "")//顺序解析
							{
								DataTable DtAnalyze = null;
								string strAnalyze = DtSource.Rows[i]["DataUnit"].ToString();
								DtAnalyze = winMain.DataBank.Find(x => x.TableName == strAnalyze);
								if (DtAnalyze == null)
								{
									strDataList.Add(DtSource.Rows[i]["Name"] + "|" + dataStr + "|" + DtSource.Rows[i]["DataLen"] + "| " + DtSource.Rows[i]["DecimalPlaces"] + "|" + DtSource.Rows[i]["DataUnit"]);
								}
								else
								{
									string strTemp = dataStr;
									for (int j = 0; j < DtAnalyze.Rows.Count; j++)
									{
										if (strTemp == DtAnalyze.Rows[j]["Value"].ToString())
										{
											strDataList.Add(DtSource.Rows[i]["Name"] + "|" + DtAnalyze.Rows[j]["Name"].ToString() + "|" + DtSource.Rows[i]["DataLen"] + "| " + DtSource.Rows[i]["DecimalPlaces"] + "|" + DtSource.Rows[i]["DataUnit"]);
											break;
										}
									}
								}
							}
							else
							{
								if (iJudgeRow == i)
								{
									strDataList.Add(DtSource.Rows[i]["Name"] + "|" + brakestatus + "|" + DtSource.Rows[i]["DataLen"] + "| " + DtSource.Rows[i]["DecimalPlaces"] + "|" + DtSource.Rows[i]["DataUnit"]);
								}
								else
								{
									DataTable DtAnalyze = null;
									string strAnalyze = brakestatus + "-" + DtSource.Rows[i]["DataUnit"].ToString();
									DtAnalyze = winMain.DataBank.Find(x => x.TableName == strAnalyze);
									if (DtAnalyze == null)
									{
										strDataList.Add(DtSource.Rows[i]["Name"] + "|" + dataStr + "|" + DtSource.Rows[i]["DataLen"] + "| " + DtSource.Rows[i]["DecimalPlaces"] + "|" + DtSource.Rows[i]["DataUnit"]);
									}
									else
									{
										string strTemp = dataStr;
										for (int j = 0; j < DtAnalyze.Rows.Count; j++)
										{
											if (strTemp == DtAnalyze.Rows[j]["Value"].ToString())
											{
												strDataList.Add(DtSource.Rows[i]["Name"] + "|" + DtAnalyze.Rows[j]["Name"].ToString() + "|" + DtSource.Rows[i]["DataLen"] + "| " + DtSource.Rows[i]["DecimalPlaces"] + "|" + DtSource.Rows[i]["DataUnit"]);
												break;
											}
										}
									}
								}
							}
							//DtSource.Rows[i]["Value"] = dataStr;
							break;
						case "ssmmhhDDMMYY":
							strDataList.Add(DtSource.Rows[i]["Name"] + "|" + dataStr + "|" + DtSource.Rows[i]["DataLen"] + "| " + DtSource.Rows[i]["DecimalPlaces"] + "|" + DtSource.Rows[i]["DataUnit"]);
							//DtSource.Rows[i]["Value"] = dataStr;
							break;
						case "Float":
							UInt32 ui32Temp = Convert.ToUInt32(Helps.fnAgainst(dataStr), 16);//字符串转16进制32位无符号整数
							float fData = BitConverter.ToSingle(BitConverter.GetBytes(ui32Temp), 0);//IEEE754 字节转换float
																									//得出数字 3236.725
							strDataList.Add(DtSource.Rows[i]["Name"] + "|" + fData.ToString() + "|" + DtSource.Rows[i]["DataLen"] + "| " + DtSource.Rows[i]["DecimalPlaces"] + "|" + DtSource.Rows[i]["DataUnit"]);
							//DtSource.Rows[i]["Value" + ii.ToString()] = fy.ToString();
							break;
						default://一律按hex16格式处理;
							dataStr = Helps.fnAgainst(dataStr);
							strDataList.Add(DtSource.Rows[i]["Name"] + "|" + dataStr + "|" + DtSource.Rows[i]["DataLen"] + "|" + DtSource.Rows[i]["DecimalPlaces"] + "|" + DtSource.Rows[i]["DataUnit"]);
							//DtSource.Rows[i]["Value"] = dataStr;
							break;
					}
					if (DtSource.Rows[i]["DataFormat"].ToString().Contains("bit"))
					{
						_valueStr = _valueStr.Substring(len1);
					}
					else
					{
						_valueStr = _valueStr.Substring(2 * len1);
					}
				}
			}
			return true;
		}

		/// <summary>
		/// 解析报文数据
		/// </summary>
		/// <param name="dr"></param>
		/// <param name="dataResultStr"></param>
		/// <param name="dataLen"></param>
		/// <param name="strDataList"></param>
		/// <returns></returns>
		bool GetExplainData(DataRow dr, string dataResultStr, int dataLen, ref List<string> strDataList, ref string strErr)
		{
			int decimalPlaces = 0;
			if (dr["DecimalPlaces"] == null || (!int.TryParse(dr["DecimalPlaces"].ToString(), out decimalPlaces)))
			{
				decimalPlaces = 0;
			}
			if (dr["DataBlockType"] == null || string.IsNullOrEmpty(dr["DataBlockType"].ToString()))
			{
				switch (dr["DataFormat"])
				{
					case "BCD":
					case "YYMMDDhh":
					case "YYMMDDWW":
						string strSymble = "";
						if ((dr["Name"].ToString().Contains("功率因数")) || (dr["DataUnit"].ToString() == "A") || (dr["DataUnit"].ToString() == "kW") || (dr["DataUnit"].ToString() == "Kvar") || (dr["DataUnit"].ToString() == "kVA"))
						{
							int iTe = 0;

							if ((Convert.ToInt64("0x" + dataResultStr.Substring(0, 1), 16) & 0x08) == 8)
							{
								strSymble = "-";
							}
							iTe = (int)(Convert.ToInt64("0x" + dataResultStr.Substring(0, 1), 16) & 0x07);
							dataResultStr = strSymble + iTe.ToString() + dataResultStr.Substring(1);
						}
						if (decimalPlaces != 0)
						{
							dataResultStr = dataResultStr.Insert(dataResultStr.Length - decimalPlaces, ".");
						}
						strDataList.Add(dr["Name"].ToString() + "|" + dataResultStr + "|" + dr["DataLen"].ToString() + "| " + dr["DecimalPlaces"].ToString() + "|" + dr["DataUnit"].ToString());
						break;
					case "ASCII":
						System.Text.ASCIIEncoding asciiEncoding = new System.Text.ASCIIEncoding();
						dataResultStr = Helps.ASCII2Str(dataResultStr).PadRight(dataLen, asciiEncoding.GetChars(new byte[] { 0 })[0]);
						//dataResultStr = Helps.Str2AsciiStr(dataResultStr).PadRight(dataLen, asciiEncoding.GetChars(new byte[] { 0 })[0]);
						strDataList.Add(dr["Name"].ToString() + "|" + dataResultStr + "|" + dr["DataLen"].ToString() + "| " + dr["DecimalPlaces"].ToString() + "|" + dr["DataUnit"].ToString());
						break;
					case "HEX10":
						UInt64 dataInt;
						if (!UInt64.TryParse(dataResultStr, NumberStyles.AllowHexSpecifier, null, out dataInt))
						{
							strErr = "失败:解析异常";
						}

						if (decimalPlaces == 0)
						{
							strDataList.Add(dr["Name"].ToString() + "|" + dataInt.ToString() + "|" + dr["DataLen"].ToString() + "| " + dr["DecimalPlaces"].ToString() + "|" + dr["DataUnit"].ToString());
						}
						else
						{
							strDataList.Add(dr["Name"].ToString() + "|" + (dataInt / Math.Pow(10, decimalPlaces)).ToString() + "|" + dr["DataLen"].ToString() + "| " + dr["DecimalPlaces"].ToString() + "|" + dr["DataUnit"].ToString());
						}
						break;
					default://一律按hex16格式处理
						strDataList.Add(dr["Name"].ToString() + "|" + dataResultStr + "|" + dr["DataLen"].ToString() + "| " + dr["DecimalPlaces"].ToString() + "|" + dr["DataUnit"].ToString());
						break;
				}
			}
			else
			{
				strDataList.Add(dr["Name"].ToString() + "|" + dataResultStr + "|" + dr["DataLen"].ToString() + "| " + dr["DecimalPlaces"].ToString() + "|" + dr["DataUnit"].ToString());
			}

			return true;
		}

		private void dgvExplain_CellClick(object sender, DataGridViewCellEventArgs e)
		{
			//当点击表clumn的列时，e.RowIndex==-1
			if (e.RowIndex > -1)
			{
				int iSelectionStart = 0;
				int iSelectionLength = 0;
				int iSelectionLengthBit = 0;

				for (int i = 0; i < e.RowIndex + 1; i++)
				{
					if (dgvExplain.Rows[i].Cells[0].Value.ToString().Contains("Bit"))
					{
						for (int j = i; j < e.RowIndex + 1; j++)
						{
							iSelectionLengthBit += Int32.Parse(dgvExplain.Rows[j].Cells[2].Value.ToString());
							if (iSelectionLengthBit % 8 == 0)
							{
								iSelectionLength = iSelectionLengthBit / 8;
								iSelectionStart += iSelectionLength;
								i = j;
								break;
							}
							else if (j == e.RowIndex)
							{
								iSelectionLength = iSelectionLengthBit / 8 + 1;
								iSelectionStart += iSelectionLength;
								i = j;
							}
						}
					}
					else
					{
						iSelectionLength = Int32.Parse(dgvExplain.Rows[i].Cells[2].Value.ToString());
						iSelectionStart += iSelectionLength;

					}
				}

				if (iSelectionLength != 0)
				{
					iSelectionStart -= iSelectionLength;
					iSelectionStart = iSelectionStart * 3;
					iSelectionLength = iSelectionLength * 3 - 1;
					rtbExplain.SelectionBackColor = Color.White;
					rtbExplain.SelectionStart = iSelectionStart;
					rtbExplain.SelectionLength = iSelectionLength;
					rtbExplain.SelectionBackColor = System.Drawing.SystemColors.Highlight;
				}
			}
		}
		#endregion

		#region MessageTest
		DataRow[] drs;
		int rowCount = 0;
		UInt64 uiSendCount = 0;
		UInt64 uiSendCorrentCount = 0;

		private static bool bMessageTest = false;

		private void btnTest_Click(object sender, EventArgs e)
		{
			if (txcbBLE.CheckState == CheckState.Unchecked)
			{
				if (btn_电表通信串口.Text == "打开串口")
				{
					MessageBox.Show("请先打开串口!");
					return;
				}
			}
			try
			{
				if (btnTest.Text == "报文测试")
				{
					if (!int.TryParse(pmc[5].Value.ToString(), out int overTime))
					{
						MessageBox.Show("非法的等待超时时间！");
						return;
					}
					SpMeter.OverTime = overTime;
					drs = dt_RW.Select(string.Format("选择 = '{0}' ", "true"), "");
					rowCount = drs.GetLength(0);
					if (rowCount < 1)
					{
						return;
					}
					SpMeter.Working = true;
					bMessageTest = true;
					btnTest.Text = "停止测试";
					uiSendCount = 0;
					uiSendCorrentCount = 0;
					#region Thread
					object oPassage = null;
					if (txcbBLE.CheckState == CheckState.Checked)
					{
						oPassage = "BlueTooth";
					}
					Thread td = new Thread(new ParameterizedThreadStart(tTestMessage));
					td.IsBackground = true;
					td.Start(oPassage);
					#endregion
				}
				else
				{
					SpMeter.Working = false;
					bMessageTest = false;
					btnTest.Text = "报文测试";
				}
			}
			catch (Exception exception)
			{
				output("设置过程中出现错误：" + exception.Message, true, true);
				SpMeter.Working = false;
				bMessageTest = false;
				btnTest.Text = "报文测试";
			}
			finally
			{
				//if (SpMeter.Working)
				//{
				//    SpMeter.Working = false;
				//}
				//btnTest.Text = "报文测试";
				//bMessageTest = false;
			}
		}

		/// <summary>
		/// 
		/// </summary>
		private void tTestMessage(Object oPassage)
		{
			try
			{
				while (bMessageTest)
				{
					#region 报文测试
					for (int i = 0; i < rowCount && SpMeter.Working; i++)
					{
						var dataResultStr = "";

						if (drs[i]["DataMark"].ToString() != "")
						{
							continue;
						}

						string dataStr = drs[i]["设置值"].ToString();
						string strType = drs[i]["DataUnit"].ToString();

						if (drs[i]["Name"].ToString() == "延时")
						{
							if (dataStr != "")
							{
								output("延时" + dataStr + "S", false, false);
								System.Threading.Thread.Sleep(Int32.Parse(dataStr) * 1000);
								drs[i]["操作结论"] = "延时成功";
							}
							continue;
						}
						string errStr = "";

						byte[] sendBytes = DataConvert.HEXString2Bytes(dataStr);
						for (int j = 0; j < 3; j++)
						{
							if ((Int64.Parse(pmc[10].Value.ToString()) > 0) && j > 0)
							{
								break;
							}
							output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
							DateTime dtTimeStart = DateTime.Now;
							byte[] recBytes = null;
							if (oPassage == null)
							{
								recBytes = SpMeter.SendCommand(sendBytes, strType);
								output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
							}
							else
							{
								RecBuf = null;
								var vBool = SendDataBLE(sendBytes);
								int iCount = 0;
								while (RecBuf == null)
								{
									iCount++;
									if (iCount > SpMeter.OverTime / 21)
									{
										break;
									}
									Thread.Sleep(20);
								}

								if (RecBuf == null)
								{
									continue;
								}
								recBytes = new byte[RecBuf.Length];
								Array.Copy(RecBuf, 0, recBytes, 0, RecBuf.Length);
							}

							uiSendCount++;
							if (recBytes == null)
							{
								drs[i]["操作结论"] = "无返回报文";
								output(drs[i]["Name"].ToString() + "无返回报文", false, false);
								if (tcbErrStop.Checked)
								{
									bMessageTest = false;
									FnStopTestHandler(1);
									break;
								}
								else
								{
									continue;
								}
							}
							else
							{
								if (Helps.JudgeMessageCorrect(recBytes, strType) == "")
								{
									uiSendCorrentCount++;
									drs[i]["操作结论"] = "成功";
								}
								else if (Helps.JudgeMessageCorrect(recBytes, strType) == "失败")
								{
									drs[i]["操作结论"] = "失败";
									output(drs[i]["Name"].ToString() + "失败", true, false);
								}
								else
								{
									drs[i]["操作结论"] = "校验错误";
									output(drs[i]["Name"].ToString() + "校验错误", true, false);
								}
							}
							DateTime dtTimeEnd = DateTime.Now;

							TimeSpan tsTime = dtTimeEnd - dtTimeStart;
							if ((Int64.Parse(pmc[10].Value.ToString()) > 0) && (tsTime.TotalMilliseconds - Int64.Parse(pmc[10].Value.ToString()) <= 0))
							{
								Console.WriteLine("0" + (Int64.Parse(pmc[10].Value.ToString()) - tsTime.Milliseconds - 5));
								if ((Int64.Parse(pmc[10].Value.ToString()) - tsTime.Milliseconds - 5) <= 0)
								{
									break;
								}
								fnWait(Int64.Parse(pmc[10].Value.ToString()) - tsTime.Milliseconds - 5);
								//Thread.Sleep(int.Parse(pmc[10].Value.ToString()) - tsTime.Milliseconds - 5);
							}
							else if (int.Parse(pmc[11].Value.ToString()) > 0)
							{
								Console.WriteLine("1" + (int.Parse(pmc[11].Value.ToString()) - 10));
								if ((int.Parse(pmc[11].Value.ToString()) - 10) <= 0)
								{
									break;
								}
								Thread.Sleep((int.Parse(pmc[11].Value.ToString()) - 10));
								break;
							}
						}
					}
					#endregion
				}

				Thread.Sleep(1000);
				output("累计发送报文" + uiSendCount.ToString() + "条," + "成功接收" + uiSendCorrentCount.ToString() + "条", true, false);
			}
			catch (Exception e)
			{
				Console.WriteLine(e);

				//throw;
			}
			btnTest.Text = "报文测试";
		}

		#region 停止测试
		public delegate void dStopTest(object objIndex);
		dStopTest stMessgage;

		private object m_objectShowUpdata = new object();
		public void FnStopTestHandler(object objIndex)
		{
			try
			{
				if (stMessgage == null)
				{
					stMessgage = new dStopTest(StopTest);
				}
				Invoke(stMessgage, new object[] { objIndex });
			}
			catch
			{

			}
		}

		/// <summary>
		/// btnTest
		/// </summary>
		/// <param name="objIndex"></param>
		public void StopTest(object objIndex)
		{
			lock (m_objectShow)
			{
				btnTest.Text = "报文测试";
				SpMeter.Working = false;
			}
		}
		#endregion
		#endregion

		#region F460&RN7326Updata
		ushort usCalcuCRC16 = 0;
		byte[] baSendBytes;
		string strErr = "";

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
				if (cbMessageType.Text == "短报文")
				{
					iPartSize = 192;
				}
				else if (cbMessageType.Text == "长报文")
				{
					iPartSize = 1344;
				}
				else if (cbMessageType.Text == "负荷辨识工装")
				{
					iPartSize = iFHBS_LENGHT;
				}

				int index = 0;
				lbaHexBytes.Clear();
				while (fs.Length > index)
				{
					int lenght = (int)fs.Length - index < iPartSize ? (int)(fs.Length - index) : iPartSize;
					byte[] buff = read.ReadBytes(lenght);
					if (cbMessageType.Text != "负荷辨识工装")
					{
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

				tbCRCUpGradeFile.Text = usCalcuCRC16.ToString("X4");
				//Console.WriteLine(DataConvert.Bytes2HexString(lbaHexBytes[lbaHexBytes.Count - 1 ], 0, lbaHexBytes[lbaHexBytes.Count - 1 ].Length));
			}
			catch (Exception e1)
			{
				//output("导入升级程序有误：" + e1.Message, true, true);
			}
		}

		private void btn_升级_Click(object sender, EventArgs e)
		{
			pbProgressF460.Value = 0;
			bool bVersionSupport = false;
			if (btn_升级.Text.ToString() == "取消升级")
			{
				btn_升级.Text = "开始升级";
				return;
			}

			#region Tips
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}

			if (tbFilePath.Text == "")
			{
				MessageBox.Show("请导入升级文件！");
				return;
			}
			#endregion
			byte[] baReceBuf = null;

			#region 1读取版本

			if (!txcbFHBS.Checked)
			{
				if (FunReadVersionF460(out string strVersionF460) != "")
				{
					MessageBox.Show("读版本失败!");
					return;
				}
				else
				{
					for (int i = 0; i < dgvVersionF460.RowCount; i++)
					{
						if (strVersionF460.Contains(dgvVersionF460[2, i].Value.ToString()))
						{
							if (dgvVersionF460[0, i].Value.ToString() == "TRUE")
							{
								bVersionSupport = true;
								//output("升级前校验码:" + dgvVersionF460[2, i].Value.ToString(), false, false);
								break;
							}
							else
							{
								MessageBox.Show("升级前校验码:" + dgvVersionF460[2, i].Value.ToString() + "不支持升级,请联系固件提供人!");
								return;
							}
						}
					}

					if (!bVersionSupport)
					{
#if !wsd
						MessageBox.Show("升级前校验码:" + strVersionF460 + "不支持升级,请联系固件提供人!");
						return;
#endif
					}
				}
			}
			#endregion

			#region 进行升级
			btn_升级.Text = "取消升级";
			pbProgressF460.Maximum = lbaHexBytes.Count;
			pbProgressF460.Value = 0;
			Thread tThreadUpdata = new Thread(FuncUpdata) { IsBackground = true };
			tThreadUpdata.Start("F460");
			#endregion
		}

		private void btnInputRN7326_Click(object sender, EventArgs e)
		{
			pbProgress7326.Value = 0;
			btnUpdata7326.Text = "开始升级";
			if (tbFilePath7326.Text == "")
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
			tbFilePath7326.Text = openFileDialog1.FileName;
			if (txcbChipType.Text.ToString() == "RN7326")
			{
				if (!(tbFilePath7326.Text.ToString().Contains("7326")))
				{
					MessageBox.Show("导入的文件不包含7326,请检查固件是否正确!");
					tbFilePath7326.Text = "";
					return;
				}
			}
			FileStream fs = new FileStream(openFileDialog1.FileName, FileMode.Open, FileAccess.Read);
			BinaryReader read = new BinaryReader(fs);

			try
			{
				if (cbMessageType7326.Text != "长报文")
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
							if (txcbChipType.Text == "RN7326")
							{
								listData.Add(0x00);
							}
							else
							{
								listData.Add(0xFF);
							}
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

				//Console.WriteLine(DataConvert.Bytes2HexString(lbaHexBytes[lbaHexBytes.Count - 1 ], 0, lbaHexBytes[lbaHexBytes.Count - 1 ].Length));
			}
			catch (Exception e1)
			{
				//output("导入升级程序有误：" + e1.Message, true, true);
			}
		}

		private void btnUpdata7326_Click(object sender, EventArgs e)
		{
			pbProgress7326.Value = 0;
			bool bVersionSupport = false;
			if (btnUpdata7326.Text.ToString() == "取消升级")
			{
				btnUpdata7326.Text = "开始升级";
				return;
			}

			#region Tips
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}

			if (tbFilePath7326.Text == "")
			{
				MessageBox.Show("请导入升级文件！");
				return;
			}
			#endregion

			byte[] baReceBuf = null;

			#region 1读取版本
			//         output("1读取版本及校验码", false, false);
			if (txcbChipType.Text == "RN7326")
			{
				if (FunReadVersion7326(out string strVersion7326) != "")
				{
					if (!txcbCommunicationmodule.Checked)
					{
						MessageBox.Show("读版本失败!");
						return;
					}
				}
				else
				{
					for (int i = 0; i < dgvVersion7326.RowCount; i++)
					{
						if (strVersion7326.Contains(dgvVersion7326[2, i].Value.ToString()))
						{
							if (dgvVersion7326[0, i].Value.ToString() == "TRUE")
							{
								output("升级前校验码:" + dgvVersion7326[2, i].Value.ToString(), false, false);
								bVersionSupport = true;
								break;
							}
							else
							{
#if !wsd
								MessageBox.Show("升级前校验码:" + dgvVersion7326[2, i].Value.ToString() + "不支持升级,请联系固件提供人!");
								return;
#endif
							}
						}
					}

#if !wsd
					if (!bVersionSupport)
					{
						MessageBox.Show("升级前校验码:" + strVersion7326 + "不支持升级,请联系固件提供人!");
						return;
					}
#endif
				}
				//FunReadVersion7326(out string strVersion7326, true);
			}
			else if (txcbChipType.Text == "GD32F415")
			{
				if (FunReadVersionF415(out string strVersion7326) != "")
				{
					if (!txcbCommunicationmodule.Checked)
					{
						MessageBox.Show("读版本失败!");
						return;
					}
				}

				//FunReadVersionF415(out string strVersion7326, true);
			}

			btnUpdata7326.Text = "取消升级";
			#endregion

			#region 进行升级
			pbProgress7326.Maximum = lbaHexBytes.Count;
			pbProgress7326.Value = 0;
			Thread tThreadUpdata = new Thread(FuncUpdata) { IsBackground = true };
			if (txcbChipType.Text == "RN7326")
			{
				tThreadUpdata.Start("RN7326");
			}
			else if (txcbChipType.Text == "GD32F415")
			{
				tThreadUpdata.Start("GD32F415");
			}
			#endregion
		}

		private void cbMessageType7326_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (cbMessageType7326.Text != "长报文")
			{
				iPartSize = 192;
			}
			else
			{
				iPartSize = 1344;
			}
		}

		private void cbMessageType_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (cbMessageType.Text == "短报文")
			{
				iPartSize = 192;
			}
			else if (cbMessageType.Text == "长报文")
			{
				iPartSize = 1344;
			}
			else if (cbMessageType.Text == "负荷辨识工装")
			{
				iPartSize = iFHBS_LENGHT;
			}
		}

		private void tbtnReadVesion7326_Click(object sender, EventArgs e)
		{
			if (txcbChipType.Text == "RN7326")
			{
				FunReadVersion7326(out string strVersion7326, true);
			}
			else if (txcbChipType.Text == "GD32F415")
			{
				FunReadVersionF415(out string strVersion7326, true);
			}
		}

		/// <summary>
		/// 读取升级标志函数
		/// </summary>
		/// <returns></returns>
		private string FunReadUpdataFlag()
		{
			string strErr = "";
			byte[] baReceBuf = null;
			strDataList.Clear();

			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return "请先打开串口!";
			}
			lstrDataMark.Clear();
			lstrDataMark.Add("DBDF21D1");
			lstrDataMark.Add("04DFDFDB");
			_dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
			if (!_dlt645.Pack(DLT645.ControlType.Read, lstrDataMark, "", 0, out baSendBytes, out strErr, pmc[7].Value.ToString(), pmc[8].Value.ToString()))
			{
				return strErr;
			}
			output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
			baReceBuf = SpMeter.SendCommand(baSendBytes, "645");
			output("RX:" + Helps.ByteToStringWithSpace(baReceBuf), false, true);
			if (!_dlt645.UnPack(baReceBuf, DLT645.ControlType.Read, lstrDataMark, 0, out bool isHaveHouxu, out string strData, out strErr))
			{

			}
			strData = strData.Substring(4, 4);
			if (strData != "5AA5")
			{
				strErr = "升级失败!";
			}
			return strErr;
		}

		/// <summary>
		/// 读取415版本函数
		/// </summary>
		/// <param name="strVersion7326">返回版本号及校验码 </param>
		/// <param name="bMessageBox">显示弹窗版本,返回版本号为空</param>
		/// <returns></returns>
		private string FunReadVersionF415(out string strVersion7326, bool bMessageBox = false)
		{
			string strErr = "";
			byte[] baReceBuf = null;
			strVersion7326 = "";
			strDataList.Clear();

			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return "请先打开串口!";
			}
			lstrDataMark.Clear();
			lstrDataMark.Add("04000F04");

			_dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
			if (!_dlt645.Pack(DLT645.ControlType.Read, lstrDataMark, "", 0, out baSendBytes, out strErr, pmc[7].Value.ToString(), pmc[8].Value.ToString()))
			{
				return strErr;
			}
			output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
			baReceBuf = SpMeter.SendCommand(baSendBytes, "645");
			output("RX:" + Helps.ByteToStringWithSpace(baReceBuf), false, true);
			if (!_dlt645.UnPack(baReceBuf, DLT645.ControlType.Read, lstrDataMark, 0, out bool isHaveHouxu, out string strData, out strErr))
			{

			}
			DataTable dtData = Helps.FunCreateTableBlockColumns();
			List<string> lsList = new List<string>();
			lsList.Add("软件版本-2-HEX---");
			lsList.Add("校验码-2-HEX---");
			lsList.Add("BOOT版本-2-HEX---");
			lsList.Add("BOOT校验码-2-HEX---");

			foreach (var VARIABLE in lsList)
			{
				object[] oRow = VARIABLE.Split('-');
				dtData.Rows.Add(oRow);
			}
			string s1 = strData.Substring(12, 4);
			string s2 = strData.Substring(8, 4);
			string s3 = strData.Substring(4, 4);
			string s4 = strData.Substring(0, 4);
			//strData = strData.Substring(6, 2) + strData.Substring(4, 2) + strData.Substring(2, 2) + strData.Substring(2, 2);
			strData = s1 + s2 + s3 + s4;
			if (UnPackDataTable((strData), dtData, ref strDataList, ref strErr))
			{
				foreach (var VARIABLE in strDataList)
				{
					string[] straData = VARIABLE.Split('|');
					strVersion7326 += straData[0] + ":" + Helps.fnAgainst(straData[1]) + "\n";
				}

				if (bMessageBox)
				{
					MessageBox.Show(strVersion7326);
				}
			}
			else
			{
				return strErr;
			}
			return strErr;
		}

		/// <summary>
		/// 读取7326版本函数
		/// </summary>
		/// <param name="strVersion7326">返回版本号及校验码 </param>
		/// <param name="bMessageBox">显示弹窗版本,返回版本号为空</param>
		/// <returns></returns>
		private string FunReadVersion7326(out string strVersion7326, bool bMessageBox = false)
		{
			string strErr = "";
			byte[] baReceBuf = null;
			strVersion7326 = "";
			strDataList.Clear();

			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return "请先打开串口!";
			}
			lstrDataMark.Clear();
			lstrDataMark.Add("DBDF21DB");
			lstrDataMark.Add("04DFDFDB");
			_dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
			if (!_dlt645.Pack(DLT645.ControlType.Read, lstrDataMark, "", 0, out baSendBytes, out strErr, pmc[7].Value.ToString(), pmc[8].Value.ToString()))
			{
				return strErr;
			}
			output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
			baReceBuf = SpMeter.SendCommand(baSendBytes, "645");
			output("RX:" + Helps.ByteToStringWithSpace(baReceBuf), false, true);
			if (!_dlt645.UnPack(baReceBuf, DLT645.ControlType.Read, lstrDataMark, 0, out bool isHaveHouxu, out string strData, out strErr))
			{

			}
			DataTable dtData = Helps.FunCreateTableBlockColumns();
			List<string> lsList = new List<string>();
			lsList.Add("软件版本-2-HEX---");
			lsList.Add("校验码-2-HEX---");
			foreach (var VARIABLE in lsList)
			{
				object[] oRow = VARIABLE.Split('-');
				dtData.Rows.Add(oRow);
			}

			if (UnPackDataTable(Helps.fnAgainst(strData), dtData, ref strDataList, ref strErr))
			{
				foreach (var VARIABLE in strDataList)
				{
					string[] straData = VARIABLE.Split('|');
					strVersion7326 += straData[0] + ":" + Helps.fnAgainst(straData[1]) + "\n";
				}

				if (bMessageBox)
				{
					MessageBox.Show(strVersion7326);
				}
			}
			else
			{
				return strErr;
			}
			return strErr;
		}

		/// <summary>
		/// 读取7326版本函数研发用
		/// </summary>
		/// <returns></returns>
		private string FunReadVersion7326APP()
		{
			string strErr = "";
			byte[] baReceBuf = null;
			strDataList.Clear();

			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return "请先打开串口!";
			}
			lstrDataMark.Clear();
			lstrDataMark.Add("DBDF21DC");
			lstrDataMark.Add("04DFDFDB");
			_dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
			if (!_dlt645.Pack(DLT645.ControlType.Read, lstrDataMark, "", 0, out baSendBytes, out strErr, pmc[7].Value.ToString(), pmc[8].Value.ToString()))
			{
				return strErr;
			}
			output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
			baReceBuf = SpMeter.SendCommand(baSendBytes, "645");
			output("RX:" + Helps.ByteToStringWithSpace(baReceBuf), false, true);
			if (_dlt645.UnPack(baReceBuf, DLT645.ControlType.Read, lstrDataMark, 0, out bool isHaveHouxu, out string strData, out strErr))
			{
				DataTable dtData = Helps.FunCreateTableBlockColumns();

				List<string> lsList = new List<string>();
				lsList.Add("APP1-2-HEX---");
				lsList.Add("APP2-2-HEX---");
				lsList.Add("版本号-2-HEX---");
				lsList.Add("App运行区域-2-HEX---");
				foreach (var VARIABLE in lsList)
				{
					object[] oRow = VARIABLE.Split('-');
					dtData.Rows.Add(oRow);
				}

				if (UnPackDataTable(Helps.fnAgainst(strData), dtData, ref strDataList, ref strErr))
				{
					string[] stradata = new string[] { };
					string strMessage = "";
					foreach (var VARIABLE in strDataList)
					{
						stradata = VARIABLE.Split('|');
						if (stradata[0] == "App运行区域")
						{
							if (stradata[1] == "0000")
							{
								strMessage += stradata[0] + ": APP1";
							}
							else
							{
								strMessage += stradata[0] + ": APP2";
							}
						}
						else
						{
							strMessage += stradata[0] + ":" + Helps.fnAgainst(stradata[1]) + "\n";
						}
					}
					MessageBox.Show(strMessage);
				}
			}

			return strErr;
		}

		/// <summary>
		/// 读取F460版本函数
		/// </summary>
		/// <param name="strVersionF460">返回版本号及校验码 </param>
		/// <param name="bMessageBox">显示弹窗版本,返回版本号为空</param>
		/// <returns></returns>
		private string FunReadVersionF460(out string strVersionF460, bool bMessageBox = false)
		{
			string strErr = "";
			byte[] baReceBuf = null;
			strVersionF460 = "";
			strDataList.Clear();
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return "请先打开串口!";
			}
			lstrDataMark.Clear();
			lstrDataMark.Add("DBDF08DB");
			lstrDataMark.Add("04DFDFDB");
			_dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
			if (!_dlt645.Pack(DLT645.ControlType.Read, lstrDataMark, "", 0, out baSendBytes, out strErr, pmc[7].Value.ToString(), pmc[8].Value.ToString()))
			{
				return strErr;
			}
#if !ShortUpGrade
			output("1-读取并检查升级前的版本及校验码", false, false);
			output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
#endif
			baReceBuf = SpMeter.SendCommand(baSendBytes, "645");
#if !ShortUpGrade
			output("RX:" + Helps.ByteToStringWithSpace(baReceBuf), false, true);
#endif
			//if (!_dlt645.UnPack(baReceBuf, DLT645.ControlType.Read, lstrDataMark, 0, out bool isHaveHouxu, out string strData, out strErr))
			//{
			//	return strErr;
			//}

			//DataRow[] drs = dtL2.Select(string.Format("DataMark= '{0}' ", lstrDataMark[0]));
			//if (drs.Length == 0)
			//{
			//	return "未查找到 数据标识:" + lstrDataMark[0].ToString();
			//}
			//DataTable dtData = DataBank.Find(x => x.TableName == drs[0]["DataBlockType"].ToString());

			//if (dtData == null)
			//{
			//	return "未查找到 sheet: " + drs[0]["DataBlockType"].ToString();
			//}

			//if (UnPackDataTable(Helps.fnAgainst(strData), dtData, ref strDataList, ref strErr))
			//{
			int index = RecCutNum(baReceBuf, "645");
			if (index == -1)
			{
#if !ShortUpGrade
				MessageBox.Show("读取版本失败!");
#endif
				return "读取版本失败!";
			}
			string 校验码Str = (((baReceBuf[index + 30] - 0x33)) & 0xff).ToString("X2") + ((baReceBuf[index + 31] - 0x33) & 0xff).ToString("X2");
			string VersionNum = (((baReceBuf[index + 25] - 0x33)) & 0xff).ToString("X2") + ((baReceBuf[index + 24] - 0x33) & 0xff).ToString("X2");
			if (bMessageBox)
			{

				MessageBox.Show("软件版本:" + VersionNum);

				//MessageBox.Show((strDataList[1].Replace("软件版本|", "").Replace("|2|0|", "")) + "(" + Helps.fnAgainst(strDataList[3].Replace("校验码|", "").Replace("|2|0|", "")) + ")");
			}
			else
			{
				strVersionF460 = VersionNum + 校验码Str;
				//(strDataList[1].Replace("软件版本|", "").Replace("|2|0|", "")) + "(" + Helps.fnAgainst(strDataList[3].Replace("校验码|", "").Replace("|2|0|", "")) + ")";
#if !ShortUpGrade
				output("软件版本及校验码:" + strVersionF460, false, false);
#endif
			}
			//}
			//else
			//{
			//	return strErr;
			//}
			return strErr;
		}

		private void tbtnReadVersionF460_Click(object sender, EventArgs e)
		{
			GetMeterVersion();
		}

		#region 进度条
		public delegate void OnPregssUpdata(object objIndex, object objType);
		OnPregssUpdata dlgSetControlUpdata;

		private object m_objectStopTest = new object();
		public void FnShowpbProgressHandler(object objIndex, object objType)
		{
			try
			{
				if (dlgSetControlUpdata == null)
				{
					dlgSetControlUpdata = new OnPregssUpdata(ShowpbProgress);
				}
				Invoke(dlgSetControlUpdata, new object[] { objIndex, objType });
			}
			catch
			{

			}
		}

		/// <summary>
		/// 刷新发送到第几包
		/// </summary>
		/// <param name="objIndex"></param>
		/// <param name="objType"></param>
		public void ShowpbProgress(object objIndex, object objType)
		{
			lock (m_objectShow)
			{
				int count = (int)objIndex;
				if (objType == "F460")
				{
					pbProgressF460.Value = count;
					if (count == 0)
					{
						SpMeter.Working = false;
						btn_升级.Text = "开始升级";
					}
				}
				else if (objType == "RN7326" || (objType == "GD32F415"))
				{
					pbProgress7326.Value = count;
					if (count == 0)
					{
						SpMeter.Working = false;
						btnUpdata7326.Text = "开始升级";
					}
				}
				else if (objType == "ZX")
				{
					pbProgressZX.Value = count;
					if (count == 0)
					{
						SpMeter.Working = false;
						tbUpdataZX.Text = "开始升级";
					}
				}
			}
		}
		#endregion

		#region 升级线程
		/// <summary>
		/// 线程
		/// </summary>
		private void FuncUpdata(object oType)
		{
			string strPartData = "";
			SpMeter.Working = true;
			#region 2-升级请求
			output("2-升级请求", false, false);
			if (iPartSize == 192)
			{
				strPartData = Helps.fnAgainst(Helps.fnAgainst((((lbaHexBytes.Count - 1) * iPartSize + lbaHexBytes[lbaHexBytes.Count - 1].Length).ToString("X8"))) + Helps.fnAgainst(lbaHexBytes.Count.ToString("X4")) + Helps.fnAgainst((usCalcuCRC16.ToString("X4"))));
			}
			else if (iPartSize == iFHBS_LENGHT)
			{
				strPartData = Helps.fnAgainst(Helps.fnAgainst((((lbaHexBytes.Count - 1) * iPartSize + lbaHexBytes[lbaHexBytes.Count - 1].Length).ToString("X8"))) + Helps.fnAgainst(lbaHexBytes.Count.ToString("X4")) + Helps.fnAgainst((usCalcuCRC16.ToString("X4"))));
			}
			else if (iPartSize == 1344)
			{
				strPartData = Helps.fnAgainst(Helps.fnAgainst((((lbaHexBytes.Count - 1) * iPartSize + lbaHexBytes[lbaHexBytes.Count - 1].Length).ToString("X8"))) + Helps.fnAgainst(((lbaHexBytes.Count - 1) * 7 + (lbaHexBytes[lbaHexBytes.Count - 1].Length / 192)).ToString("X4")) + Helps.fnAgainst((usCalcuCRC16.ToString("X4"))));
			}

			lstrDataMark.Clear();
			if (oType == "F460")
			{
				lstrDataMark.Add("0F0F0F01");
			}
			else if (oType == "ZX")
			{
				lstrDataMark.Add("0F0F0F0A");
			}
			else if (oType == "RN7326" || (oType == "GD32F415"))
			{
				lstrDataMark.Add("0F0F0F04");
			}
			else
			{
				MessageBox.Show("升级失败!");
				FnShowpbProgressHandler(0, oType);
				return;
			}

			if (!_dlt645.Pack(DLT645.ControlType.Updata, lstrDataMark, strPartData, 0, out baSendBytes, out strErr, pmc[7].Value.ToString(), pmc[8].Value.ToString()))
			{

			}

			Console.WriteLine(DataConvert.Bytes2HexString(baSendBytes, 0, baSendBytes.Length));

			if (!sendOK(baSendBytes, 0x9d))
			{
				MessageBox.Show("升级请求失败!");
				FnShowpbProgressHandler(0, oType);
				return;
			}
			#endregion

			if (oType == "F460")
			{
				if (btn_升级.Text.ToString() != "取消升级")
				{
					FnShowpbProgressHandler(0, oType);
					return;
				}
			}
			else if (oType == "ZX")
			{
				if (tbUpdataZX.Text.ToString() != "取消升级")
				{
					FnShowpbProgressHandler(0, oType);
					return;
				}
			}
			else if (oType == "RN7326" || (oType == "GD32F415"))
			{
				if (btnUpdata7326.Text.ToString() != "取消升级")
				{
					FnShowpbProgressHandler(0, oType);
					return;
				}
			}
			else
			{
				MessageBox.Show("升级失败!");
				return;
			}

			#region 3-传输文件
			output("3-传输文件", false, false);

			#region 3.1执行文件传输
			for (int m = 0; m < lbaHexBytes.Count; m++)
			{
				if (oType == "F460")
				{
					if (btn_升级.Text.ToString() != "取消升级")
					{
						FnShowpbProgressHandler(0, oType);
						return;
					}
				}
				else if (oType == "ZX")
				{
					if (tbUpdataZX.Text.ToString() != "取消升级")
					{
						FnShowpbProgressHandler(0, oType);
						return;
					}
				}
				else if (oType == "RN7326" || (oType == "GD32F415"))
				{
					if (btnUpdata7326.Text.ToString() != "取消升级")
					{
						FnShowpbProgressHandler(0, oType);
						return;
					}
				}
				else
				{
					MessageBox.Show("升级失败!");
					return;
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
				else if (oType == "RN7326" || (oType == "GD32F415"))
				{
					lstrDataMark.Add("0F0F0F05");
				}
				else if (oType == "ZX")
				{
					lstrDataMark.Add("0F0F0F0B");
				}

				int i = 0;

				if (iPartSize == 1344)
				{
					if (!_dlt645.PackUpdata(lstrDataMark[0].Replace("0F0F0F", ""), strPartData, out baSendBytes, out strErr))
					{

					}
					for (i = 0; i < iRepeatNum; i++)
					{
						output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
						byte[] recBytes = SpMeter.SendCommand(baSendBytes, "Updata");
						output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
						if ((recBytes == null) || ((recBytes[1] == 0x85) || (recBytes[1] == 0x82)))
						{
							Thread.Sleep(1000);
							continue;
						}
						else
						{
							FnShowpbProgressHandler((m + 1), oType);
							break;
						}
					}
				}
				else
				{
					if (!_dlt645.Pack(DLT645.ControlType.Updata, lstrDataMark, strPartData, 0, out baSendBytes, out strErr, pmc[7].Value.ToString(), pmc[8].Value.ToString()))
					{

					}
					for (i = 0; i < iRepeatNum; i++)
					{
						if (!sendOK(baSendBytes, 0x9d))
						{

							continue;
						}
						Thread.Sleep((int.Parse(pmc[11].Value.ToString())));
						FnShowpbProgressHandler((m + 1), oType);
						break;
					}
				}

				if (i >= iRepeatNum)
				{
					MessageBox.Show("重试次数3次,取消升级!");
					FnShowpbProgressHandler(0, oType);
					return;
				}
			}
			#endregion

			#endregion
			SpMeter.Working = false;
			#region 4-启动升级
			output("4-启动升级", false, false);
			lstrDataMark.Clear();

			if (oType == "F460")
			{
				lstrDataMark.Add("0F0F0F03");
			}
			else if (oType == "RN7326" || (oType == "GD32F415"))
			{
				lstrDataMark.Add("0F0F0F06");
			}
			else if (oType == "ZX")
			{
				lstrDataMark.Add("0F0F0F0C");
			}

			if (!_dlt645.Pack(DLT645.ControlType.Updata, lstrDataMark, "55", 0, out baSendBytes, out strErr, pmc[7].Value.ToString(), pmc[8].Value.ToString()))
			{

			}

			if (!sendOK(baSendBytes, 0x9d))
			{
				if (txcbCommunicationmodule.Checked)
				{
					MessageBox.Show("启动升级失败!");
					return;
				}
				//MessageBox.Show("升级失败!");
				//FnShowpbProgressHandler(0, oType);
				//return;
			}
			#endregion
			if ((!txcbCommunicationmodule.Checked && iPartSize != 128) || !txcbFHBS.Checked)
			{
				//循环等待10秒电表复位,更新程序
				#region 5-读取升级后的校验码
				output("5-读取升级后的校验码");

				//for (int i = 0; i < iRepeatNum; i++)
				{
					#region 1读取版本
					string strVersion = "";
					output("延时60S后,读校验码", false, true);
					Thread.Sleep(60000);
					if (oType == "F460")
					{
						if (FunReadVersionF460(out strVersion) != "")
						{
							MessageBox.Show("读版本失败!");
							return;
						}
					}
					else if (oType == "RN7326")
					{
						if (FunReadVersion7326(out strVersion) != "")
						{
							MessageBox.Show("读版本失败!");
							return;
						}
						if (FunReadUpdataFlag() != "")
						{
							MessageBox.Show("升级失败!");
							return;
						}
					}
					else if (oType == "GD32F415")
					{
						if (FunReadVersionF415(out strVersion) != "")
						{
							MessageBox.Show("读版本失败!");
							return;
						}
						//if (FunReadUpdataFlag() != "")
						//{
						//	MessageBox.Show("升级失败!");
						//	return;
						//}
					}
					else if (oType == "ZX")
					{
						if (FunReadCheckSumZX(out strVersion) != "")
						{
							MessageBox.Show("读版本失败!");
							return;
						}
					}
					MessageBox.Show("升级后校验码:\n" + strVersion + "\n 升级成功!");
					#endregion
				}
				#endregion
			}
			FnShowpbProgressHandler(0, oType);
		}
		#endregion
		#endregion

		/// <summary>
		/// FromMessage
		/// </summary>
		/// <param name="baByte"></param>
		/// <param name="ControlType"></param>
		/// <param name="lstrDataMark"></param>
		/// <param name="strData"></param>
		/// <returns></returns>
		public string FunMessage(byte[] baByte, DLT645.ControlType ControlType, List<string> lstrDataMark, out string strData)
		{
			strData = "";
			if (btn_电表通信串口.Text == "打开串口")
			{
				this.Info("请先打开串口！");
				//MessageBox.Show("请先打开串口!");
				return "请先打开串口!";
			}

			if (baByte != null)
			{
				if (baByte[0] == 0x68)
				{
					byte[] bSendBuf = baByte;
					baByte = new byte[baByte.Length + 4];
					byte[] bFE = { 0xFE, 0xFE, 0xFE, 0xFE, };
					bFE.CopyTo(baByte, 0);
					bSendBuf.CopyTo(baByte, bFE.Length);
				}
				output("TX:" + Helps.ByteToStringWithSpace(baByte), false, true);
				if (ControlType == DLT645.ControlType.Radio)
				{
					SpMeter.sendWithNoAsk(baByte, baByte.Length);
					strErr = "广播校时发送成功,请检查!";
				}
				else
				{
					byte[] recBytes = SpMeter.SendCommand(baByte, "645");
					output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
					bool ReadFollow = false;
					byte seq = 0;
					do
					{
						string dataPianDuan;

						if (!_dlt645.UnPack(recBytes, ControlType, lstrDataMark, seq, out ReadFollow, out dataPianDuan, out strErr))
						{
							return strErr;
						}

						strData += Helps.fnAgainst(dataPianDuan.Trim());
						if (ReadFollow)
						{
							ControlType = DLT645.ControlType.ReadFollow;
							seq++;
							if (!_dlt645.Pack(ControlType, lstrDataMark, "", seq, out byte[] sendBytes, out strErr))
							{
								return strErr;
							}

							output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
							recBytes = SpMeter.SendCommand(sendBytes, "645");
							output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
						}
					} while (ReadFollow);
				}
			}
			return strErr;
		}

		/// <summary>
		/// FunDealMessage
		/// </summary>
		/// <param name="data"></param>
		/// <param name="ControlType"></param>
		/// <param name="lstrDataMark"></param>
		/// <param name="strData"></param>
		/// <returns></returns>
		public string FunDealMessage(string data, DLT645.ControlType ControlType, List<string> lstrDataMark, out string strData, string strAdr = "")
		{
			strData = "";
			byte[] baByte = null;
			if (strAdr == "")
			{
				_dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
			}
			else
			{
				_dlt645 = new DLT645(Helps.fnAgainst(strAdr));
			}
			if (!txcEncrypt.Checked)
			{
				if (!_dlt645.Pack(ControlType, lstrDataMark, data, 0, out baByte, out strErr))
				{
					MessageBox.Show("组包失败!");
					return "组包失败!";
				}
			}
			else
			{
				if (ControlType == DLT645.ControlType.Read)
				{
					ControlType = DLT645.ControlType.SafetyReadLTU;
				}
				else if (ControlType == DLT645.ControlType.ReadLoad)
				{
					ControlType = DLT645.ControlType.SafetyReadLoadLTU;
				}
				if (!_dlt645.Pack(ControlType, lstrDataMark, data, 0, out baByte, out strErr))
				{
					MessageBox.Show("组包失败!");
					return "组包失败!";
				}
			}
			//if (!winMain._dlt645.Pack(ControlType, lstrDataMark, data, 0, out byte[] baByte, out strErr))
			//{
			//	MessageBox.Show("组包失败!");
			//	return "组包失败!";
			//}
			if (baByte != null)
			{
				if (baByte[0] == 0x68)
				{
					byte[] bSendBuf = baByte;
					baByte = new byte[baByte.Length + 4];
					byte[] bFE = { 0xFE, 0xFE, 0xFE, 0xFE, };
					bFE.CopyTo(baByte, 0);
					bSendBuf.CopyTo(baByte, bFE.Length);
				}
				if (!txcbBLE.Checked)
				{
					if (btn_电表通信串口.Text == "打开串口")
					{
						this.Info("请先打开串口！");
						//MessageBox.Show("请先打开串口!");
						return "请先打开串口!";
					}
					output("TX:" + Helps.ByteToStringWithSpace(baByte), false, true);
					if (ControlType == DLT645.ControlType.Radio)
					{
						SpMeter.sendWithNoAsk(baByte, baByte.Length);
						strErr = "广播校时发送成功,请检查!";
					}
					else
					{
						byte[] recBytes = SpMeter.SendCommand(baByte, "645");
						output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);

						bool ReadFollow = false;
						byte seq = 0;
						do
						{
							string dataPianDuan;

							if (!_dlt645.UnPack(recBytes, ControlType, lstrDataMark, seq, out ReadFollow, out dataPianDuan, out strErr))
							{
								return strErr;
							}

							strData += Helps.fnAgainst(dataPianDuan.Trim());
							if (ReadFollow)
							{
								ControlType = DLT645.ControlType.ReadFollow;
								if (txcEncrypt.Checked)
								{
									ControlType = DLT645.ControlType.SafetyReadFollowLTU;
								}
								seq++;
								if (!_dlt645.Pack(ControlType, lstrDataMark, "", seq, out byte[] sendBytes, out strErr))
								{
									return strErr;
								}

								output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
								recBytes = SpMeter.SendCommand(sendBytes, "645");
								output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
							}
						} while (ReadFollow);
					}
				}
				else
				{
					#region Thread
					//object oTemp = baByte;
					//RecBuf = null;

					//               Thread td = new Thread(new ParameterizedThreadStart(tFunDealMessageBLE));
					//               //td.IsBackground = true;
					//               td.Start(oTemp);

					Method1(baByte);
					#endregion
				}
			}
			return strErr;
		}

		/// <summary>
		/// FunDealMessage
		/// </summary>
		/// <param name="data"></param>
		/// <param name="ControlType"></param>
		/// <param name="lstrDataMark"></param>
		/// <param name="strData"></param>
		/// <returns></returns>
		public async Task<string> asyncFunDealMessage(string data, DLT645.ControlType ControlType, List<string> lstrDataMark)
		{
			string strData = ";";

			byte[] baByte = null;
			_dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
			if (!txcEncrypt.Checked)
			{
				if (!_dlt645.Pack(ControlType, lstrDataMark, data, 0, out baByte, out strErr))
				{
					MessageBox.Show("组包失败!");
					strData = "组包失败!" + ";";
					return strData;
				}
			}
			else
			{
				if (ControlType == DLT645.ControlType.Read)
				{
					ControlType = DLT645.ControlType.SafetyReadLTU;
				}
				else if (ControlType == DLT645.ControlType.ReadLoad)
				{
					ControlType = DLT645.ControlType.SafetyReadLoadLTU;
				}
				if (!_dlt645.Pack(ControlType, lstrDataMark, data, 0, out baByte, out strErr))
				{
					MessageBox.Show("组包失败!");
					strData = "组包失败!" + ";";
					return strData;
				}
			}
			//if (!winMain._dlt645.Pack(ControlType, lstrDataMark, data, 0, out byte[] baByte, out strErr))
			//{
			//	MessageBox.Show("组包失败!");
			//	return "组包失败!";
			//}
			if (baByte != null)
			{
				if (baByte[0] == 0x68)
				{
					byte[] bSendBuf = baByte;
					baByte = new byte[baByte.Length + 4];
					byte[] bFE = { 0xFE, 0xFE, 0xFE, 0xFE, };
					bFE.CopyTo(baByte, 0);
					bSendBuf.CopyTo(baByte, bFE.Length);
				}

				{
					#region Thread
					//object oTemp = baByte;
					//RecBuf = null;

					//               Thread td = new Thread(new ParameterizedThreadStart(tFunDealMessageBLE));
					//               //td.IsBackground = true;
					//               td.Start(oTemp);
					//            Task.Run(()=>
					//                   {
					//		object oTemp = baByte;
					//		tFunDealMessageBLE(oTemp);
					//                   }
					//                   );
					//Task.WaitAll();
					await Method1(baByte);
					if (RecBuf != null)
					{
						byte[] recBytes = new byte[RecBuf.Length];
						Array.Copy(RecBuf, 0, recBytes, 0, RecBuf.Length);
						bool ReadFollow = false;
						byte seq = 0;
						do
						{
							string dataPianDuan;

							if (!_dlt645.UnPack(recBytes, ControlType, lstrDataMark, seq, out ReadFollow, out dataPianDuan, out strErr))
							{
								return strErr;
							}

							strData += Helps.fnAgainst(dataPianDuan.Trim());
							if (ReadFollow)
							{
								ControlType = DLT645.ControlType.ReadFollow;
								if (txcEncrypt.Checked)
								{
									ControlType = DLT645.ControlType.SafetyReadFollowLTU;
								}
								seq++;
								if (!_dlt645.Pack(ControlType, lstrDataMark, "", seq, out byte[] sendBytes, out strErr))
								{
									return strErr;
								}

								output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
								recBytes = SpMeter.SendCommand(sendBytes, "645");
								output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
							}
						} while (ReadFollow);
					}
					#endregion
				}
			}

			return strData;
		}


		public async Task Method1(byte[] baByte)
		{
			await Task.Run(() =>
			{
				object oTemp = baByte;
				tFunDealMessageBLE(oTemp);
			}
					);

		}

		/// <summary>
		/// ReadBLE
		/// </summary>
		private void tFunDealMessageBLE(Object oData)
		{
			try
			{
				{
					byte[] recBytes = null;
					{

						var vBool = SendDataBLE((byte[])oData);
						output("TX:" + Helps.ByteToStringWithSpace((byte[])oData), false, true);
						int iCount = 0;
						while (RecBuf == null)
						{
							iCount++;
							if (iCount > SpMeter.OverTime / 21)
							{
								break;
							}
							Thread.Sleep(20);
						}

						if (RecBuf == null)
						{
							//return "蓝牙返回为空";
						}
						recBytes = new byte[RecBuf.Length];
						Array.Copy(RecBuf, 0, recBytes, 0, RecBuf.Length);
					}
					//               bool ReadFollow = false;
					//               byte seq = 0;
					//string strData = "";
					//               do
					//               {
					//                   string dataPianDuan;

					//                   if (!_dlt645.UnPack(recBytes, ControlType, lstrDataMark, seq, out ReadFollow, out dataPianDuan, out strErr))
					//                   {
					//                       //return strErr;
					//                   }

					//                   strData += Helps.fnAgainst(dataPianDuan.Trim());
					//                   if (ReadFollow)
					//                   {
					//                       ControlType = DLT645.ControlType.ReadFollow;
					//                       seq++;
					//                       if (!_dlt645.Pack(ControlType, lstrDataMark, "", seq, out byte[] sendBytes, out strErr))
					//                       {
					//                           //return strErr;
					//                       }

					//                       output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
					//                       recBytes = SpMeter.SendCommand(sendBytes, "645");
					//                       output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
					//                   }
					//               } while (ReadFollow);
				}
			}
			catch (Exception exception)
			{
				output("抄读过程中出现错误：" + exception.Message, true, true);
			}
			finally
			{

			}
		}

		private void tsmiAdjustMeter_Click(object sender, EventArgs e)
		{
			FormAdjustMeter famFrom = new FormAdjustMeter(Helps.fnAgainst(pmc[6].Value.ToString())) { StartPosition = FormStartPosition.CenterScreen };
			famFrom.Show(this);
		}

		private void Factory_Click(object sender, EventArgs e)
		{

		}

		private void tsmiAdjustTime_Click(object sender, EventArgs e)
		{
			FormAdjustTime fatFrom = new FormAdjustTime(Helps.fnAgainst(pmc[6].Value.ToString())) { StartPosition = FormStartPosition.CenterScreen };
			fatFrom.Show(this);
		}

		private void tsmiControlClear_Click(object sender, EventArgs e)
		{
			FormControlClear fatFrom = new FormControlClear(Helps.fnAgainst(pmc[6].Value.ToString())) { StartPosition = FormStartPosition.CenterScreen };
			fatFrom.Show(this);
		}

		private void txTabControl1Changed(object sender, EventArgs e)
		{
			_dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
			timerSystem.Enabled = true;
			if (txTabControl1.SelectedTab.Name == "tpTopoZX")
			{
				if (txcbTopoType.Text.ToString() == "智芯")
				{
					lReadCounts.Text = "读取条数(<=20)";
					txtbCount.Text = "20";
				}
				else
				{
					lReadCounts.Text = "读取条数(<=10)";
					txtbCount.Text = "10";
				}
			}
			else if (txTabControl1.SelectedTab.Name == "tpReadData")
			{
				//ReadCommonData();
				ReadSpecialTest();
			}
			else if (txTabControl1.SelectedTab.Name == "tabPage2")
			{
#if !wsd
				#region dtVersionF460
				dtVersionF460 = new DataTable(null);
				dtVersionF460.Columns.Add("是否支持升级", typeof(bool));
				dtVersionF460.Columns.Add("软件版本", typeof(string));
				dtVersionF460.Columns.Add("校验码", typeof(string));
#if wsd
				if (DataBank.Find(x => x.TableName == "F460支持升级的版本") == null)
				{

					MessageBox.Show("不存在'F460支持升级的版本'sheet");
					return;

				}
				dtVersionF460 = DataBank.Find(x => x.TableName == "F460支持升级的版本").Copy();
				dgvVersionF460.DataSource = dtVersionF460;
#endif
				#endregion
#endif
			}
			else if (txTabControl1.SelectedTab.Name == "tpFunction")
			{
				InitTeleindicationTest();
			}
		}

		private void tabUpdata_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (TXtabUpdata.SelectedTab.Name == "tpF460Updata")
			{
				//#if! lr
				//#region dtVersionF460
				//dtVersionF460 = new DataTable(null);
				//dtVersionF460.Columns.Add("是否支持升级", typeof(bool));
				//dtVersionF460.Columns.Add("软件版本", typeof(string));
				//dtVersionF460.Columns.Add("校验码", typeof(string));
				//if (DataBank.Find(x => x.TableName == "F460支持升级的版本") == null)
				//{
				//	MessageBox.Show("不存在'F460支持升级的版本'sheet");
				//	return;
				//}
				//dtVersionF460 = DataBank.Find(x => x.TableName == "F460支持升级的版本").Copy();
				//dgvVersionF460.DataSource = dtVersionF460;
				//#endregion
				//#endif
			}
			else if (TXtabUpdata.SelectedTab.Name == "tp7326Updata")
			{
				#region dtVersion7326
				dtVersion7326 = new DataTable(null);
				dtVersion7326.Columns.Add("是否支持升级", typeof(bool));
				dtVersion7326.Columns.Add("软件版本", typeof(string));
				dtVersion7326.Columns.Add("校验码", typeof(string));
#if !ShortUpGrade
				if (DataBank.Find(x => x.TableName == "RN7326支持升级的版本") == null)
				{
					MessageBox.Show("不存在'RN7326支持升级的版本'sheet");
					return;
				}
				dtVersion7326 = DataBank.Find(x => x.TableName == "RN7326支持升级的版本").Copy();
				dgvVersion7326.DataSource = dtVersion7326;
#endif
				#endregion
			}
			else if (TXtabUpdata.SelectedTab.Name == "PartUpgrade")
			{
#if ShortUpGrade
                tbBeforeUpGrade.Text = Environment.CurrentDirectory + "\\" + "ParaO.bin";
                tbAfterUpGrade.Text = Environment.CurrentDirectory + "\\" + "ParaN.bin";
                txbBeforeUpGrade.Visible = false;
                txbAfterUpGrade.Visible = false;
                string strSafeFirst = ini.IniReadValue("Main", "UpGradeShortReserve", "") ;
                if (strSafeFirst == "true")
                {
                    cbSafeFirst.Checked = true;
                }
                else
                {
                    cbSafeFirst.Checked = false;
                }
					string strApp = ini.IniReadValue("Main", "UpGradeShortApp", "");
                if (strApp == "true")
                {
                    cbApp.Checked = true;
                }
                else
                {
                    cbApp.Checked = false;
                }
					cbSafeFirst.Enabled = false;
                cbApp.Enabled = false;
#endif
			}

		}

		private void tsmiChipRegisters_Click(object sender, EventArgs e)
		{
			FormChipRegisters fcrFrom = new FormChipRegisters(Helps.fnAgainst(pmc[6].Value.ToString())) { StartPosition = FormStartPosition.CenterScreen };
			fcrFrom.Show(this);
		}

		/// <summary>
		/// 是否在自动循环
		/// </summary>
		bool AutoMoniter = false;
		private void cbMoniter_CheckedChanged(object sender, EventArgs e)
		{
			if (cbMoniter.CheckState == CheckState.Checked)
			{
				if (btn_电表通信串口.Text == "打开串口")
				{
					this.Info("请先打开串口！");
					//MessageBox.Show("请先打开串口!");
					cbMoniter.CheckState = CheckState.Unchecked;
					return;
				}
				AutoMoniter = true;
				SpMeter.Working = true;
				Thread ThTestL = new Thread(new ParameterizedThreadStart(TAutoMoniter));
				ThTestL.IsBackground = true;
				ThTestL.Start();
			}
			else
			{
				StopMoniter();
			}
		}

		/// <summary>
		/// 自动线程
		/// </summary>
		private void TAutoMoniter(object Interval)
		{
			try
			{
				while (AutoMoniter)
				{
					if (AutoMoniter && SpMeter.Working)
					{
						byte[] baReceBuf = SpMeter.MoniterSerial();
						if (baReceBuf != null)
						{
							if (baReceBuf.Length > 0)
							{
								AutoMoniter = false;
								List<string> resultList = new List<string>();
								output("RX:" + Helps.ByteToStringWithSpace(baReceBuf), false, true);
								bool bTrue = UnPackTable(baReceBuf, ref resultList, ref strErr);

								if ((strErr == ""))
								{
									if (resultList[1].Contains("AAAAAAAAAA"))
									{
										string[] straAddr = resultList[1].Split('|');
										for (int i = 0; i < dtMoniter.Rows.Count; i++)
										{
											if (((bool)dtMoniter.Rows[i][0] == false) && ((dtMoniter.Rows[i][1].ToString().Substring(10, 2) == straAddr[1].Substring(10, 2)) || straAddr[1] == "AAAAAAAAAAAA"))
											{
												string[] straDataMark = resultList[5].Split('|');
												DLT645 dlt645 = new DLT645(Helps.fnAgainst(dtMoniter.Rows[i][1].ToString()));
												dlt645.PackMoniter(DLT645.ControlType.SlaveReplyAddr, straDataMark[1], ("00000000"), out baSendBytes, out strErr);
												if (SpMeter.sendWithNoAsk(baSendBytes, baSendBytes.Length) == true)
												{
													output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
													dtMoniter.Rows[i][0] = true;
												}

												AutoMoniter = true;
												break;
											}
										}
									}
									else
									{
										string[] straAddr = resultList[1].Split('|');
										string[] straConTrolType = resultList[3].Split('|');
										int iConTrolType = (int)(Convert.ToInt64("0x" + straConTrolType[1].Substring(0, 2), 16)) + 0x80;

										for (int i = 0; i < dtMoniter.Rows.Count; i++)
										{
											if (dtMoniter.Rows[i][1].ToString().Substring(10, 2) == straAddr[1].Substring(10, 2))
											{
												string[] straDataMark = resultList[5].Split('|');
												DLT645 dlt645 = new DLT645(Helps.fnAgainst(dtMoniter.Rows[i][1].ToString()));
												DLT645.ControlType ctType = DLT645.ControlType.SlaveReplyAddr;
												string strData = "";
												if (iConTrolType == 0x9e)
												{
													ctType = DLT645.ControlType.SlaveReplyTime;
													string[] straData = resultList[5].Split('|');
													strData = straData[1];
													straDataMark[1] = "";
												}
												else if (iConTrolType == 0x91)
												{
													strData = "000000000000000000000000";
												}

												dlt645.PackMoniter(ctType, straDataMark[1], strData, out baSendBytes, out strErr);
												if (SpMeter.sendWithNoAsk(baSendBytes, baSendBytes.Length) == true)
												{
													output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
												}

												AutoMoniter = true;
												break;
											}
										}
									}
									AutoMoniter = true;
								}
								else
								{
									AutoMoniter = true;
									continue;
								}
							}
						}
					}
					else
					{
						break;
					}
				}
			}
			catch { }
		}

		/// <summary>
		/// 停止
		/// </summary>
		private void StopMoniter()
		{
			AutoMoniter = false;
			SpMeter.Working = false;
			cbMoniter.CheckState = CheckState.Unchecked;
		}

		#region MoniterAddr
		/// <summary>
		/// Moniter
		/// </summary>
		private void MoniterAddr()
		{
			#region ReadData
			dgvMoniter.AutoGenerateColumns = false;
			dtMoniter = new DataTable(null);
			dtMoniter.Columns.Add("选择", typeof(bool));
			dtMoniter.Columns.Add("通讯地址", typeof(string));
			List<string> lsList = new List<string>();
			for (int i = 0; i < 64; i++)
			{
				lsList.Add("FALSE-2022020103" + (i + 1).ToString("d2"));
			}

			foreach (var VARIABLE in lsList)
			{
				object[] oRow = VARIABLE.Split('-');
				dtMoniter.Rows.Add(oRow);
			}

			dgvMoniter.DataSource = dtMoniter;
			#endregion
		}
		#endregion

		private void tsmiLoadProFile_Click(object sender, EventArgs e)
		{
			FromLoadProFile flpfFrom = new FromLoadProFile(Helps.fnAgainst(pmc[6].Value.ToString())) { StartPosition = FormStartPosition.CenterScreen };
			flpfFrom.Show(this);
		}

		private void txbClearTopo_Click(object sender, EventArgs e)
		{
			byte[] baBytes = null;
			string strData = "";
			lstrDataMark.Clear();
			if (txcbTopoType.Text.ToString() == "智芯")
			{
				lstrDataMark.Add("09080304");
			}
			else
			{
				lstrDataMark.Add("08C0C000");
			}
			_dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
			if (!_dlt645.Pack(DLT645.ControlType.Write, lstrDataMark, strData, 0, out baBytes, out string strErr))
			{
				MessageBox.Show("组包失败!");
				return;
			}

			if (sendOK(baBytes, 0x94))
			{
				output(" 清零成功", false, true);
			}
			else
			{
				output(" 清零失败!,请检查", true, true);
			}
		}

		private void txbSendTopo_Click(object sender, EventArgs e)
		{
			byte[] baBytes = null;
			string strData = "";
			lstrDataMark.Clear();
			_dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
			if (txcbTopoType.Text.ToString() == "智芯")
			{
				strData = "00FFFFFFFFFFFF010002AAE990012003";
				lstrDataMark.Add("09080106");
				if (!_dlt645.Pack(DLT645.ControlType.WriteTopo, lstrDataMark, strData, 0, out baBytes, out string strErr))
				{
					MessageBox.Show("组包失败!");
					return;
				}
			}
			else
			{
				strData = "FFFFFFFFFFFF";
				lstrDataMark.Add("08C0C001");
				if (!_dlt645.Pack(DLT645.ControlType.Write, lstrDataMark, strData, 0, out baBytes, out string strErr))
				{
					MessageBox.Show("组包失败!");
					return;
				}
			}

			if (sendOK(baBytes, 0x94))
			{
				output(" 拓扑发送成功", false, true);
			}
			else
			{
				output(" 拓扑发送失败!,请检查", true, true);
			}
		}

		private void txbAdjustTime_Click(object sender, EventArgs e)
		{
			byte[] baBytes = null;
			string strDateTime = DateTime.Now.ToString("yyMMdd") + ((int)DateTime.Now.DayOfWeek).ToString("X2") + DateTime.Now.ToString("HHmmss");
			lstrDataMark.Clear();
			if (txcbTopoType.Text.ToString() == "智芯")
			{
				lstrDataMark.Add("04400003");
			}
			else
			{
				lstrDataMark.Add("04020103");
			}
			_dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
			if (!_dlt645.Pack(DLT645.ControlType.Write, lstrDataMark, strDateTime, 0, out baBytes, out string strErr))
			{
				MessageBox.Show("组包失败!");
				return;
			}

			if (sendOK(baBytes, 0x94))
			{
				output(" 校时成功", false, true);
			}
			else
			{
				output(" 校时失败!,请检查", true, true);
			}
		}

		private void txbReadTopo_Click(object sender, EventArgs e)
		{
			byte[] baBytes = null;
			string strData = "";
			int len1 = 0, iRows = 3, iBlockLength = 0;
			lstrDataMark.Clear();

			if (txcbTopoType.Text.ToString() == "智芯")
			{
				strData = UInt32.Parse(txtbCount.Text).ToString("X2") + Helps.fnAgainst(UInt32.Parse(txtbStart.Text).ToString("X4"));
				lstrDataMark.Add("09080306");
			}
			else if (txcbTopoType.Text.ToString() == "浙江导轨表")
			{
				strData = UInt32.Parse(txtbCount.Text).ToString("X2") + Helps.fnAgainst(UInt32.Parse(txtbStart.Text).ToString("X4"));
				lstrDataMark.Add("09080306");
			}
			else
			{
				strData = Helps.fnAgainst(UInt32.Parse(txtbCount.Text).ToString("X4")) + Helps.fnAgainst(UInt32.Parse(txtbStart.Text).ToString("X4"));
				lstrDataMark.Add("08C0C002");
			}
			byte seq = 0;


			string dataPianDuan;
			bool isHaveHouxu = false;

			_dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
			if (!_dlt645.Pack(DLT645.ControlType.ReadTopo, lstrDataMark, strData, seq, out baBytes, out string strErr))
			{
				MessageBox.Show("组包失败!");
				return;
			}
			output("TX:" + Helps.ByteToStringWithSpace(baBytes), false, true);
			byte[] recBytes = SpMeter.SendCommand(baBytes, "645");
			output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
			if (recBytes != null)
			{
				DataTable DtSource = null;
				if (DtSource != null)
				{
					DtSource.Rows.Clear();
					dgvTopoZX.DataSource = DtSource;
					dgvTopoZX.Rows.Clear();
				}
				string dataResultStr = "";
				do
				{
					DLT645.ControlType controlType = DLT645.ControlType.Read;
					if (isHaveHouxu)
					{
						controlType = DLT645.ControlType.ReadFollow;
					}
					if (!_dlt645.UnPack(recBytes, controlType, lstrDataMark, seq, out isHaveHouxu, out dataPianDuan, out strErr))
					{
						output(strErr, true, true);
						return;
					}
					dataPianDuan = Helps.fnAgainst(dataPianDuan);
					dataResultStr += dataPianDuan.Trim();
					if (isHaveHouxu)
					{
						seq++;
						if (!_dlt645.Pack(DLT645.ControlType.ReadFollow, lstrDataMark, "", seq, out baBytes, out strErr))
						{
							break;
						}

						output("TX:" + Helps.ByteToStringWithSpace(baBytes), false, true);
						recBytes = SpMeter.SendCommand(baBytes, "645");
						output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
					}
				} while (isHaveHouxu);

				output((dataResultStr), false, false);

				if (txcbTopoType.Text.ToString() == "智芯")
				{
					DtSource = DataBank.Find(x => x.TableName == "读取识别结果记录").Copy();
					iRows = 3;

					//iBlockLength = 22;
				}
				else
				{
					DtSource = DataBank.Find(x => x.TableName == "查询拓扑信息").Copy();
					iRows = 2;
					//iBlockLength = 40;
				}

				for (int i = iRows; i < DtSource.Rows.Count; i++)
				{
					if (!int.TryParse(DtSource.Rows[i]["DataLen"].ToString(), out len1))
					{
						//Text = "数据库存储的数据长度有误，请检查数据库！";
						return;
					}
					iBlockLength += len1 * 2;
				}

				if (DtSource == null)
				{
					return;
				}

				if (dataResultStr.Length > 24)
				{
					for (int i = 0; i < iRows; i++)
					{
						if (!int.TryParse(DtSource.Rows[i]["DataLen"].ToString(), out len1))
						{
							//Text = "数据库存储的数据长度有误，请检查数据库！";
							return;
						}

						string dataStr = "";

						dataStr = dataResultStr.Substring(0, 2 * len1);

						switch (DtSource.Rows[i]["DataFormat"])
						{
							case "BCD":
								if (!int.TryParse(DtSource.Rows[i]["DecimalPlaces"].ToString(), out int decimalPlaces))
								{
									decimalPlaces = 0;
								}

								dataStr = Helps.fnAgainst(dataStr);
								if (decimalPlaces != 0)
								{
									dataStr = dataStr.Insert(dataStr.Length - decimalPlaces, ".");
								}
								DtSource.Rows[i]["Value0"] = dataStr;
								break;
							case "HEX10":
								UInt64 dataStrHEX10;
								if (!int.TryParse(DtSource.Rows[i]["DecimalPlaces"].ToString(), out int decimalPlace))
								{
									decimalPlace = 0;
								}
								dataStr = Helps.fnAgainst(dataStr);
								dataStrHEX10 = UInt64.Parse(dataStr, System.Globalization.NumberStyles.HexNumber);

								if (dataStrHEX10 != 0)
								{
									dataStr = dataStrHEX10.ToString();
								}

								if (decimalPlace != 0)
								{
									if (dataStr.Length == decimalPlace)
									{
										dataStr = dataStr.Insert(dataStr.Length - decimalPlace, "0.");
									}
									else
									{
										if (dataStr.Length < decimalPlace)
										{
											dataStr = dataStr.PadLeft((decimalPlace + 1), '0');
										}
										dataStr = dataStr.Insert(dataStr.Length - decimalPlace, ".");
									}
								}

								DtSource.Rows[i]["Value0"] = dataStr;
								break;
							default://一律按hex16格式处理;
									//dataStr = Helps.fnAgainst(dataStr);
								DtSource.Rows[i]["Value0"] = dataStr;
								break;
						}
						dataResultStr = dataResultStr.Substring(2 * len1);
					}

					string[] stradataPart = new String[dataResultStr.Length / iBlockLength];
					for (int ii = 0; ii < stradataPart.Length; ii++)
					{
						stradataPart[ii] = dataResultStr.Substring(iBlockLength * ii, iBlockLength);

						dgvTopoZX.DataSource = DtSource;
						len1 = 0;
						for (int i = iRows; i < DtSource.Rows.Count; i++)
						{
							if (!int.TryParse(DtSource.Rows[i]["DataLen"].ToString(), out len1))
							{
								//Text = "数据库存储的数据长度有误，请检查数据库！";
								return;
							}

							string dataStr = "";
							if (DtSource.Rows[i]["DataFormat"].ToString().Contains("bit"))
							{
								dataStr = stradataPart[ii].Substring(0, len1);
							}
							else
							{
								dataStr = stradataPart[ii].Substring(0, 2 * len1);
							}

							switch (DtSource.Rows[i]["DataFormat"])
							{
								case "BCD":
									if (!int.TryParse(DtSource.Rows[i]["DecimalPlaces"].ToString(), out int decimalPlaces))
									{
										decimalPlaces = 0;
									}

									dataStr = Helps.fnAgainst(dataStr);
									if (decimalPlaces != 0)
									{
										dataStr = dataStr.Insert(dataStr.Length - decimalPlaces, ".");
									}

									if (DtSource.Rows[i]["Name"].ToString().Contains("相位"))
									{
										if (dataStr == "00")
										{
											DtSource.Rows[i]["Value" + (ii + 1).ToString()] = "未知";
										}
										else if (dataStr == "01")
										{
											DtSource.Rows[i]["Value" + (ii + 1).ToString()] = "A相";
										}
										else if (dataStr == "02")
										{
											DtSource.Rows[i]["Value" + (ii + 1).ToString()] = "B相";
										}
										else if (dataStr == "03")
										{
											DtSource.Rows[i]["Value" + (ii + 1).ToString()] = "C相";
										}
										else if (dataStr == "04")
										{
											DtSource.Rows[i]["Value" + (ii + 1).ToString()] = "三相";
										}
									}
									else
									{
										DtSource.Rows[i]["Value" + (ii + 1).ToString()] = dataStr;
									}
									break;
								case "HEX10":
									UInt64 dataStrHEX10;
									if (!int.TryParse(DtSource.Rows[i]["DecimalPlaces"].ToString(), out int decimalPlace))
									{
										decimalPlace = 0;
									}
									dataStr = Helps.fnAgainst(dataStr);
									dataStrHEX10 = UInt64.Parse(dataStr, System.Globalization.NumberStyles.HexNumber);

									if (dataStrHEX10 != 0)
									{
										dataStr = dataStrHEX10.ToString();
									}

									if (decimalPlace != 0)
									{
										if (dataStr.Length == decimalPlace)
										{
											dataStr = dataStr.Insert(dataStr.Length - decimalPlace, "0.");
										}
										else
										{
											if (dataStr.Length < decimalPlace)
											{
												dataStr = dataStr.PadLeft((decimalPlace + 1), '0');
											}
											dataStr = dataStr.Insert(dataStr.Length - decimalPlace, ".");
										}
									}

									DtSource.Rows[i]["Value" + (ii + 1).ToString()] = dataStr;
									break;

								case "BIN":
									stradataPart[ii] = Helps.fnAgainst(stradataPart[ii]);
									stradataPart[ii] = DataConvert.HexStringToBinary(stradataPart[ii]);
									DtSource.Rows[i]["Value" + (ii + 1).ToString()] = dataStr;
									break;
								case "bit":
								case "bits":
									dataStr = new string(dataStr.ToCharArray().Reverse().ToArray());
									DtSource.Rows[i]["Value" + (ii + 1).ToString()] = dataStr;
									break;
								case "ssmmhhDDMMYY":
									DtSource.Rows[i]["Value" + (ii + 1).ToString()] = dataStr;
									break;
								case "Float":
									UInt32 x = Convert.ToUInt32(Helps.fnAgainst(dataStr), 16);//字符串转16进制32位无符号整数
									float fy = BitConverter.ToSingle(BitConverter.GetBytes(x), 0);//IEEE754 字节转换float//得出数字 3236.725
									DtSource.Rows[i]["Value" + (ii + 1).ToString()] = fy.ToString();
									break;
								default://一律按hex16格式处理;
										//dataStr = Helps.fnAgainst(dataStr);
									DtSource.Rows[i]["Value" + (ii + 1).ToString()] = dataStr;
									break;
							}
							if (DtSource.Rows[i]["DataFormat"].ToString().Contains("bit"))
							{
								stradataPart[ii] = stradataPart[ii].Substring(len1);
							}
							else
							{
								stradataPart[ii] = stradataPart[ii].Substring(2 * len1);
							}
						}
					}
				}
				else
				{
					output("抄读拓扑历史记录为空");
				}
			}
		}

		private void txcbTopoType_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (txcbTopoType.Text.ToString() == "智芯")
			{
				lReadCounts.Text = "读取条数(<=20)";
				txtbCount.Text = "20";
			}
			else
			{
				lReadCounts.Text = "读取条数(<=10)";
				txtbCount.Text = "10";
			}
		}

		private void tsmiContinuedFreezeTest_Click(object sender, EventArgs e)
		{
			FromContinuedFreezeTest fcftFrom = new FromContinuedFreezeTest(Helps.fnAgainst(pmc[6].Value.ToString())) { StartPosition = FormStartPosition.CenterScreen };
			fcftFrom.Show(this);
		}

		private void tbtnReadVesion7326APP_Click(object sender, EventArgs e)
		{
			FunReadVersion7326APP();
		}

		private void txtcReadDatas_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (txtcReadDatas.SelectedTab.Name == "tpCommonRead")
			{
				ReadCommonData();
			}
			else if (txtcReadDatas.SelectedTab.Name == "tpMoniter")
			{
				MoniterAddr();
			}
			else if (txtcReadDatas.SelectedTab.Name == "tpSpecialTest")
			{
				ReadSpecialTest();
			}
			else if (txtcReadDatas.SelectedTab.Name == "tpXL1140")
			{
				ReadCheckErrorTest();
			}
			else if (txtcReadDatas.SelectedTab.Name == "tpTripping")
			{
				ReadTrippingData();
			}
			else if (txtcReadDatas.SelectedTab.Name == "tpReadDataExcel")
			{
				dtReadDataExcel = new DataTable(null);
				//dtVersionF460.Columns.Add("是否支持升级", typeof(bool));
				//dtVersionF460.Columns.Add("软件版本", typeof(string));
				//dtVersionF460.Columns.Add("校验码", typeof(string));
				if (DataBank.Find(x => x.TableName == "抄读测试") == null)
				{
					MessageBox.Show("不存在'抄读测试'sheet");
					return;
				}
				dtReadDataExcel = DataBank.Find(x => x.TableName == "抄读测试").Copy();
				dgvReadDataExcel.DataSource = dtReadDataExcel;
				for (int i = 0; i < dgvReadDataExcel.Rows.Count; i++)
				{
					dgvReadDataExcel.Rows[i].Cells[0].Value = true;
				}
			}
		}

		/// <summary>
		/// 读取常用
		/// </summary>
		// 主函数
		private void ReadCommonData()
		{
			string type = txcbType.Text.ToString();
			dtReaDataTable = dgvCommon.DataSource as DataTable;
			// ① 保存当前表格状态
			if ((dgvCommon.DataSource != null) && (dtReaDataTable != null))
			{
				string currentType = dgvCommon.Tag as string;
				if (!string.IsNullOrEmpty(currentType))
				{
					// 克隆表结构并复制数据（确保独立保存）
					savedTables[currentType] = dtReaDataTable.Copy();
				}
			}

			// ② 如果有保存过的表格，直接恢复
			if (savedTables.ContainsKey(type))
			{
				dtReaDataTable = savedTables[type];
				dgvCommon.DataSource = dtReaDataTable;
				dgvCommon.Tag = type;
				return;
			}
			// ③ 否则创建新的表格
			dgvCommon.AutoGenerateColumns = false;
			dtReaDataTable = new DataTable(type);
			dtReaDataTable.Columns.Add("选择", typeof(bool));
			dtReaDataTable.Columns.Add("Name", typeof(string));
			dtReaDataTable.Columns.Add("DataMark", typeof(string));
			dtReaDataTable.Columns.Add("DataLen", typeof(string));
			dtReaDataTable.Columns.Add("DataBlockType", typeof(string));
			dtReaDataTable.Columns.Add("DataFormat", typeof(string));
			dtReaDataTable.Columns.Add("DecimalPlaces", typeof(string));
			dtReaDataTable.Columns.Add("DataUnit", typeof(string));
			dtReaDataTable.Columns.Add("读取值", typeof(string));
			dtReaDataTable.Columns.Add("操作结论", typeof(string));
			List<string> lsList = new List<string>();
			// ====== 根据类型添加数据 ======
			if (type == "瞬时量")
			{
				lsList.Add("TRUE-A相电压-02010100-2--BCD-1-V--");
				lsList.Add("TRUE-B相电压-02010200-2--BCD-1-V--");
				lsList.Add("TRUE-C相电压-02010300-2--BCD-1-V--");
				lsList.Add("TRUE-A相电流-02020100-3--BCD-3-A--");
				lsList.Add("TRUE-B相电流-02020200-3--BCD-3-A--");
				lsList.Add("TRUE-C相电流-02020300-3--BCD-3-A--");
				lsList.Add("TRUE-瞬时总有功功率-02030000-3--BCD-4-kW--");
				lsList.Add("TRUE-A相有功功率-02030100-3--BCD-4-kW--");
				lsList.Add("TRUE-B相有功功率-02030200-3--BCD-4-kW--");
				lsList.Add("TRUE-C相有功功率-02030300-3--BCD-4-kW--");
				lsList.Add("TRUE-瞬时总无功功率-02040000-3--BCD-4-Kvar--");
				lsList.Add("TRUE-瞬时A无功功率-02040100-3--BCD-4-Kvar--");
				lsList.Add("TRUE-瞬时B无功功率-02040200-3--BCD-4-Kvar--");
				lsList.Add("TRUE-瞬时C无功功率-02040300-3--BCD-4-Kvar--");
				lsList.Add("TRUE-瞬时总视在功率-02050000-3--BCD-4-kVA--");
				lsList.Add("TRUE-瞬时A视在功率-02050100-3--BCD-4-kVA--");
				lsList.Add("TRUE-瞬时B视在功率-02050200-3--BCD-4-kVA--");
				lsList.Add("TRUE-瞬时C视在功率-02050300-3--BCD-4-kVA--");
				lsList.Add("TRUE-瞬时总功率因数-02060000-2--BCD-3---");
				lsList.Add("TRUE-瞬时A功率因数-02060100-2--BCD-3---");
				lsList.Add("TRUE-瞬时B功率因数-02060200-2--BCD-3---");
				lsList.Add("TRUE-瞬时C功率因数-02060300-2--BCD-3---");
			}
			else if (type == "电能")
			{
				lsList.Add("TRUE-当前组合有功电能总-00000000-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前组合有功电能尖-00000100-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前组合有功电能峰-00000200-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前组合有功电能平-00000300-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前组合有功电能谷-00000400-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前正向有功电能总-00010000-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前正向有功电能尖-00010100-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前正向有功电能峰-00010200-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前正向有功电能平-00010300-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前正向有功电能谷-00010400-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前反向有功电能总-00020000-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前反向有功电能尖-00020100-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前反向有功电能峰-00020200-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前反向有功电能平-00020300-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前反向有功电能谷-00020400-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前A相正向有功电能-00150000-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前B相正向有功电能-00290000-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前C相正向有功电能-003D0000-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前A相反向有功电能-00160000-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前B相反向有功电能-002A0000-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前C相反向有功电能-003E0000-4--BCD-2-kWh--");
				lsList.Add("TRUE-当前组合无功1电能总-00030000-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前组合无功1电能尖-00030100-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前组合无功1电能峰-00030200-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前组合无功1电能平-00030300-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前组合无功1电能谷-00030400-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前组合无功2电能总-00040000-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前组合无功2电能尖-00040100-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前组合无功2电能峰-00040200-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前组合无功2电能平-00040300-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前组合无功2电能谷-00040400-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前一象限无功电能总-00050000-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前一象限无功电能尖-00050100-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前一象限无功电能峰-00050200-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前一象限无功电能平-00050300-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前一象限无功电能谷-00050400-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前二象限无功电能总-00060000-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前二象限无功电能尖-00060100-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前二象限无功电能峰-00060200-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前二象限无功电能平-00060300-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前二象限无功电能谷-00060400-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前三象限无功电能总-00070000-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前三象限无功电能尖-00070100-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前三象限无功电能峰-00070200-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前三象限无功电能平-00070300-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前三象限无功电能谷-00070400-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前四象限无功电能总-00080000-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前四象限无功电能尖-00080100-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前四象限无功电能峰-00080200-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前四象限无功电能平-00080300-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前四象限无功电能谷-00080400-4--BCD-2-kvarh--");
				lsList.Add("TRUE-当前正向视在电能总-00090000-4--BCD-2-kVAh--");
				lsList.Add("TRUE-当前正向视在电能尖-00090100-4--BCD-2-kVAh--");
				lsList.Add("TRUE-当前正向视在电能峰-00090200-4--BCD-2-kVAh--");
				lsList.Add("TRUE-当前正向视在电能平-00090300-4--BCD-2-kVAh--");
				lsList.Add("TRUE-当前正向视在电能谷-00090400-4--BCD-2-kVAh--");
				lsList.Add("TRUE-当前反向视在电能总-000A0000-4--BCD-2-kVAh--");
				lsList.Add("TRUE-当前反向视在电能尖-000A0100-4--BCD-2-kVAh--");
				lsList.Add("TRUE-当前反向视在电能峰-000A0200-4--BCD-2-kVAh--");
				lsList.Add("TRUE-当前反向视在电能平-000A0300-4--BCD-2-kVAh--");
				lsList.Add("TRUE-当前反向视在电能谷-000A0400-4--BCD-2-kVAh--");
			}
			else if (type == "高精度瞬时量")
			{
				lsList.Add("TRUE-高精度瞬时总有功功率-020C0000-4--BCD-4-kW--");
				lsList.Add("TRUE-高精度A相有功功率-020C0100-4--BCD-4-kW--");
				lsList.Add("TRUE-高精度B相有功功率-020C0200-4--BCD-4-kW--");
				lsList.Add("TRUE-高精度C相有功功率-020C0300-4--BCD-4-kW--");
				lsList.Add("TRUE-高精度瞬时总无功功率-020D0000-4--BCD-4-Kvar--");
				lsList.Add("TRUE-高精度瞬时A无功功率-020D0100-4--BCD-4-Kvar--");
				lsList.Add("TRUE-高精度瞬时B无功功率-020D0200-4--BCD-4-Kvar--");
				lsList.Add("TRUE-高精度瞬时C无功功率-020D0300-4--BCD-4-Kvar--");
				lsList.Add("TRUE-高精度瞬时总视在功率-020E0000-4--BCD-4-kVA--");
				lsList.Add("TRUE-高精度瞬时A视在功率-020E0100-4--BCD-4-kVA--");
				lsList.Add("TRUE-高精度瞬时B视在功率-020E0200-4--BCD-4-kVA--");
				lsList.Add("TRUE-高精度瞬时C视在功率-020E0300-4--BCD-4-kVA--");
			}

			// ====== 添加数据到表格 ======
			foreach (var VARIABLE in lsList)
			{
				object[] oRow = VARIABLE.Split('-');
				dtReaDataTable.Rows.Add(oRow);
			}
			if (type == "瞬时量")
			{
				AllReadDataTables.Add("瞬时量", dtReaDataTable.Copy());
			}
			else if (type == "电能")
			{
				AllReadDataTables.Add("电能", dtReaDataTable.Copy());
			}
			else if (type == "高精度瞬时量")
			{
				AllReadDataTables.Add("高精度瞬时量", dtReaDataTable.Copy());
			}

			// ④ 绑定新表格
			dgvCommon.DataSource = dtReaDataTable;
			dgvCommon.Tag = type; // 保存当前类型标记
		}
		/// <summary>
		/// 读取特殊处理
		/// </summary>
		private void ReadSpecialTest()
		{
			#region ReadData
			dgvSpecialTest.AutoGenerateColumns = false;
			dtSpecialReaDataTable = new DataTable(null);
			dtSpecialReaDataTable.Columns.Add("选择", typeof(bool));
			dtSpecialReaDataTable.Columns.Add("Name", typeof(string));
			dtSpecialReaDataTable.Columns.Add("DataMark", typeof(string));
			dtSpecialReaDataTable.Columns.Add("DataLen", typeof(string));
			dtSpecialReaDataTable.Columns.Add("DataBlockType", typeof(string));
			dtSpecialReaDataTable.Columns.Add("DataFormat", typeof(string));
			dtSpecialReaDataTable.Columns.Add("DecimalPlaces", typeof(string));
			dtSpecialReaDataTable.Columns.Add("DataUnit", typeof(string));
			dtSpecialReaDataTable.Columns.Add("读取值", typeof(string));
			dtSpecialReaDataTable.Columns.Add("相对误差%", typeof(string));
			List<string> lsList = new List<string>();
			{
				lsList.Add("TRUE-A相电压-02010100-2--BCD-1-V--");
				lsList.Add("TRUE-B相电压-02010200-2--BCD-1-V--");
				lsList.Add("TRUE-C相电压-02010300-2--BCD-1-V--");
				lsList.Add("TRUE-A相电流-02020100-3--BCD-3-A--");
				lsList.Add("TRUE-B相电流-02020200-3--BCD-3-A--");
				lsList.Add("TRUE-C相电流-02020300-3--BCD-3-A--");
				lsList.Add("TRUE-高精度瞬时总有功功率-020C0000-4--BCD-4-kW--");
				//lsList.Add("TRUE-高精度A相有功功率-020C0100-4--BCD-4-kW--");
				//lsList.Add("TRUE-高精度B相有功功率-020C0200-4--BCD-4-kW--");
				//lsList.Add("TRUE-高精度C相有功功率-020C0300-4--BCD-4-kW--");
				lsList.Add("TRUE-高精度瞬时总无功功率-020D0000-4--BCD-4-Kvar--");
				//lsList.Add("TRUE-高精度瞬时A无功功率-020D0100-4--BCD-4-Kvar--");
				//lsList.Add("TRUE-高精度瞬时B无功功率-020D0200-4--BCD-4-Kvar--");
				//lsList.Add("TRUE-高精度瞬时C无功功率-020D0300-4--BCD-4-Kvar--");
				lsList.Add("TRUE-高精度瞬时总视在功率-020E0000-4--BCD-4-kVA--");
				//lsList.Add("TRUE-高精度瞬时A视在功率-020E0100-4--BCD-4-kVA--");
				//lsList.Add("TRUE-高精度瞬时B视在功率-020E0200-4--BCD-4-kVA--");
				//lsList.Add("TRUE-高精度瞬时C视在功率-020E0300-4--BCD-4-kVA--");
				lsList.Add("TRUE-瞬时总功率因数-02060000-2--BCD-3---");
				//lsList.Add("TRUE-瞬时A功率因数-02060100-2--BCD-3---");
				//lsList.Add("TRUE-瞬时B功率因数-02060200-2--BCD-3---");
				//lsList.Add("TRUE-瞬时C功率因数-02060300-2--BCD-3---");
			}


			foreach (var VARIABLE in lsList)
			{
				object[] oRow = VARIABLE.Split('-');
				dtSpecialReaDataTable.Rows.Add(oRow);
			}

			dgvSpecialTest.DataSource = dtSpecialReaDataTable;
			#endregion
		}

		private void txcbType_SelectedIndexChanged(object sender, EventArgs e)
		{
			ReadCommonData();
		}

		private void txbReadDatas_Click(object sender, EventArgs e)
		{
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}

			if (SpMeter.Working)
			{
				return;
			}
			try
			{
				DataRow[] drs = dtReaDataTable.Select(string.Format("选择 = '{0}' ", "true"), "");
				int rowCount = drs.GetLength(0);
				if (rowCount < 1)
				{
					return;
				}

				SpMeter.Working = true;

				#region 抄读
				bool isReadingErr = false;
				DLT645 dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
				for (int i = 0; i < rowCount && SpMeter.Working; i++)
				{
					List<string> dataMarkList = new List<string>();
					byte seq = 0;
					string dataResultStr = "";

					if (drs[i]["DataMark"] == null || drs[i]["DataMark"].ToString().Length != 8)
					{
						drs[i]["读取值"] = "";
						drs[i]["操作结论"] = "抄读失败：数据标识非法！";
						continue;
					}

					string dataMark = drs[i]["DataMark"].ToString();
					dataMarkList.Add(dataMark);

					byte[] sendBytes = null;
					string errStr = "";
					if (!dlt645.Pack(DLT645.ControlType.Read, dataMarkList, "", 0, out sendBytes, out errStr))
					{
						drs[i]["读取值"] = "";
						drs[i]["操作结论"] = errStr;
						continue;
					}

					string strAPDU = "";
					byte[] baAPDU;
					byte[] baTemp;
					byte[] recBytes;
					{
						output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
						recBytes = SpMeter.SendCommand(sendBytes, "645");
					}

					output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
					bool isHaveHouxu = false;
					if (!SpMeter.Working)
					{
						continue;
					}

					string dataPianDuan;
					DLT645.ControlType controlType = DLT645.ControlType.Read;

					if (!dlt645.UnPack(recBytes, controlType, dataMarkList, seq, out isHaveHouxu, out dataPianDuan, out errStr))
					{
						drs[i]["读取值"] = "";
						drs[i]["操作结论"] = errStr;
						isReadingErr = true;
						continue;
					}

					dataResultStr = dataPianDuan.Trim();

					int dataLen;

					if (!int.TryParse(drs[i]["DataLen"].ToString(), out dataLen))
					{
						drs[i]["读取值"] = "";
						drs[i]["操作结论"] = "数据库中该数据项长度非法，请排查数据库信息！";
						continue;
					}

					if (dataResultStr.Length != dataLen * 2)
					{
						drs[i]["读取值"] = "";
						drs[i]["操作结论"] = "失败：返回数据长度非法";
						continue;
					}
					GetData(drs[i], dataResultStr, dataLen);
					Thread.Sleep(int.Parse(pmc[11].Value.ToString()));
				}
				#endregion
			}
			catch (Exception exception)
			{
				output("抄读过程中出现错误：" + exception.Message, true, true);
			}
			finally
			{
				if (SpMeter.Working)
				{
					SpMeter.Working = false;
				}
			}
		}

		private void txbOutputData_Click(object sender, EventArgs e)
		{
			SaveFileDialog saveFileDialog1 = new SaveFileDialog();
			saveFileDialog1.Filter = "Excel文件（*.xlsx）|*.xlsx|Excel文件（*.xls）|*.xls";
			saveFileDialog1.FilterIndex = 0;
			saveFileDialog1.RestoreDirectory = false;
			string errStr = "";
			if (saveFileDialog1.ShowDialog() == DialogResult.OK)
			{
				List<DataTable> dts = new List<DataTable>();
				dts.Add(dtReaDataTable);

				if (!AsposeExcelTools.DataTableListToExcel(dts, saveFileDialog1.FileName, new string[] { "常用数据" }, out errStr))
				{
					MessageBox.Show("导出常用数据失败：" + errStr);
					return;
				}
				MessageBox.Show("导出成功!");
			}
		}

		private void btnInputZX_Click(object sender, EventArgs e)
		{
			pbProgressZX.Value = 0;
			tbUpdataZX.Text = "开始升级";
			if (tbFilePathZX.Text == "")
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
			tbFilePathZX.Text = openFileDialog1.FileName;
			if (!(tbFilePathZX.Text.ToString().Contains("ZX")))
			{
				MessageBox.Show("导入的文件不包含ZX,请检查固件是否正确!");
				tbFilePathZX.Text = "";
				return;
			}

			FileStream fs = new FileStream(openFileDialog1.FileName, FileMode.Open, FileAccess.Read);
			BinaryReader read = new BinaryReader(fs);

			try
			{
				if (cbMessageTypeZX.Text != "长报文")
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

				//Console.WriteLine(DataConvert.Bytes2HexString(lbaHexBytes[lbaHexBytes.Count - 1 ], 0, lbaHexBytes[lbaHexBytes.Count - 1 ].Length));
			}
			catch (Exception e1)
			{
				//output("导入升级程序有误：" + e1.Message, true, true);
			}
		}

		private void cbMessageTypeZX_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (cbMessageTypeZX.Text != "长报文")
			{
				iPartSize = 192;
			}
			else
			{
				iPartSize = 1344;
			}
		}

		private void tbUpdataZX_Click(object sender, EventArgs e)
		{
			pbProgressZX.Value = 0;
			bool bVersionSupport = false;
			if (tbUpdataZX.Text.ToString() == "取消升级")
			{
				tbUpdataZX.Text = "开始升级";
				return;
			}

			#region Tips
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}

			if (tbFilePathZX.Text == "")
			{
				MessageBox.Show("请导入升级文件！");
				return;
			}
			#endregion

			byte[] baReceBuf = null;

			#region 1读取版本
			//         output("1读取版本及校验码", false, false);
			if (FunReadCheckSumZX(out string strCheckSumZX) != "")
			{
				MessageBox.Show("读版本失败!");
				return;
			}
			else
			{
				//for (int i = 0; i < dgvVersion7326.RowCount; i++)
				//{
				//	if (strVersion7326.Contains(dgvVersion7326[2, i].Value.ToString()))
				//	{
				//		if (dgvVersion7326[0, i].Value.ToString() == "TRUE")
				//		{
				//			output("升级前校验码:" + dgvVersion7326[2, i].Value.ToString(), false, false);
				//			bVersionSupport = true;
				//			break;
				//		}
				//		else
				//		{
				//			MessageBox.Show("升级前校验码:" + dgvVersion7326[2, i].Value.ToString() + "不支持升级,请联系固件提供人!");
				//			return;
				//		}
				//	}
				//}

				//if (!bVersionSupport)
				//{
				//	MessageBox.Show("升级前校验码:" + strVersion7326 + "不支持升级,请联系固件提供人!");
				//	return;
				//}
			}
			#endregion

			tbUpdataZX.Text = "取消升级";

			#region 进行升级
			pbProgressZX.Maximum = lbaHexBytes.Count;
			pbProgressZX.Value = 0;
			Thread tThreadUpdata = new Thread(FuncUpdata) { IsBackground = true };
			tThreadUpdata.Start("ZX");
			#endregion
		}

		/// <summary>
		/// 读取ZX算法版校验码函数
		/// </summary>
		/// <param name="strCheckSumZX">返回版本号及校验码 </param>
		/// <param name="bMessageBox">显示弹窗版本,返回版本号为空</param>
		/// <returns></returns>
		private string FunReadCheckSumZX(out string strCheckSumZX, bool bMessageBox = false)
		{
			string strErr = "";
			byte[] baReceBuf = null;
			strCheckSumZX = "";
			strDataList.Clear();

			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return "请先打开串口!";
			}
			lstrDataMark.Clear();
			lstrDataMark.Add("DBDF21DF");
			lstrDataMark.Add("04DFDFDB");
			_dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
			if (!_dlt645.Pack(DLT645.ControlType.Read, lstrDataMark, "", 0, out baSendBytes, out strErr, pmc[7].Value.ToString(), pmc[8].Value.ToString()))
			{
				return strErr;
			}
			output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
			baReceBuf = SpMeter.SendCommand(baSendBytes, "645");
			output("RX:" + Helps.ByteToStringWithSpace(baReceBuf), false, true);
			if (!_dlt645.UnPack(baReceBuf, DLT645.ControlType.Read, lstrDataMark, 0, out bool isHaveHouxu, out string strData, out strErr))
			{

			}
			DataTable dtData = Helps.FunCreateTableBlockColumns();
			List<string> lsList = new List<string>();
			lsList.Add("校验码-2-HEX---");
			foreach (var VARIABLE in lsList)
			{
				object[] oRow = VARIABLE.Split('-');
				dtData.Rows.Add(oRow);
			}

			if (UnPackDataTable(Helps.fnAgainst(strData), dtData, ref strDataList, ref strErr))
			{
				foreach (var VARIABLE in strDataList)
				{
					string[] straData = VARIABLE.Split('|');
					strCheckSumZX += straData[0] + ":" + Helps.fnAgainst(straData[1]) + "\n";
				}

				if (bMessageBox)
				{
					MessageBox.Show(strCheckSumZX);
				}
			}
			else
			{
				return strErr;
			}
			return strErr;
		}

		private void tbtnReadCheckSumZX_Click(object sender, EventArgs e)
		{
			FunReadCheckSumZX(out string strCheckSumZX, true);
		}

		private void tbSendTopo_Click(object sender, EventArgs e)
		{
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}
			byte[] TxBufFactoryMode = new byte[32];
			string strtbSendTopoTime = tbSendTopoTime.Text.ToString().PadLeft(12, '0');
			string strtbSendTopoTimeS = "";
			if (strtbSendTopoTime == "FFFFFFFFFFFF")
			{
				strtbSendTopoTimeS = "FFFFFFFFFFFF";
			}
			else
			{
				for (int i = 0; i < 6; i++)
				{
					string strData = strtbSendTopoTime.Substring(i * 2, 2);
					strtbSendTopoTimeS += int.Parse(strData).ToString("X2");
				}
			}

			string strDataSend = "68" + Helps.fnAgainst(pmc[6].Value.ToString()) + "6814233C343B3C" + DataConvert.fnEncodingData(tbTopoTypeSend.Text.ToString().PadLeft(2, '0')) + DataConvert.fnEncodingData((strtbSendTopoTimeS)) + "8B35B8783435DD1CC3345336333333333333333333333333";
			Helps.TryStrToHexByte(strDataSend, out TxBufFactoryMode);

			#region 校验和+16
			byte sum = Helps.Ssum(TxBufFactoryMode);
			byte[] endBytes = new[] { sum, (byte)0x16 };
			#endregion

			TxBufFactoryMode = TxBufFactoryMode.Concat(endBytes).ToArray();

			if (sendOK(TxBufFactoryMode, 0x94))
			{
				output(pmc[6].Value.ToString() + "立即执行一次拓扑发送成功", false, true);
			}
			else
			{
				output(pmc[6].Value.ToString() + " 立即执行一次拓扑发送失败!", true, true);
			}
		}

		private void tbAdjustTimeMS_Click(object sender, EventArgs e)
		{
			byte[] baBytes = null;
			string strRandom = "";
			DLT645.ControlType ctSeType = DLT645.ControlType.msAdjustTime;
			//string strDateTime = DateTime.Now.ToString("yyMMdd") + DateTime.Now.ToString("HHmmss") + DateTime.Now.Millisecond.ToString().PadLeft(4, '0');
			string strDateTime = txt_SetMeterTime.Value.ToString("yyMMdd") + txt_SetMeterTime.Value.ToString("HHmmss") + DateTime.Now.Millisecond.ToString().PadLeft(4, '0');
			lstrDataMark.Clear();
			_dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
			if (txcEncrypt.Checked)
			{
				ctSeType = DLT645.ControlType.SafetyMsAdjustTimeLTU;
				if (pmc[6].Value.ToString() == "999999999999")
				{
					Helps.TryStrToHexByte(Helps.fnAgainst(strDateTime), out baBytes, out strErr);
					baSendBytes = m_SCUProxy.HY_SendToProxyBuf(01, 0x14, 0x02, baBytes);
					output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
					byte[] baRecBytes = SpWuCha.SendCommand(baSendBytes, "EncryptSCU");
					if (baRecBytes == null)
					{
						MessageBox.Show("返回报文为空!");
						return;
					}
					output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);
					byte[] baDataBytes = new byte[16];
					Array.Copy(baRecBytes, (baRecBytes.Length - 17), baDataBytes, 0, baDataBytes.Length);

					if (!_dlt645.Pack(ctSeType, lstrDataMark, Helps.fnAgainst(Helps.ByteToStringWithNoSpace(baDataBytes)), 0, out baSendBytes, out strErr, pmc[7].Value.ToString(), pmc[8].Value.ToString(), pmc[9].Value.ToString()))
					{
						output("点对点加密组包失败!");
					}

					output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
					SpMeter.sendWithNoAsk(baBytes, baBytes.Length);
					output("密文广播校时发送成功", false, true);
				}
				else
				{
					string strGetRandom = m_SCUProxy.HY_SendToSecModuleRandom("DFDDA010");
					_dlt645 = new DLT645(Helps.fnAgainst(txtxbTXAddr.Text.ToString()));
					if (!_dlt645.Pack(DLT645.ControlType.SafetyCertificationLTU, lstrDataMark, strGetRandom, 0, out baSendBytes, out string strErr))
					{

					}
					output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
					byte[] recByte = SpMeter.SendCommand(baSendBytes, "645");
					output("RX:" + Helps.ByteToStringWithSpace(recByte), false, true);

					if (_dlt645.UnPack(recByte, DLT645.ControlType.SafetyCertificationLTU, lstrDataMark, 0, out bool isHave, out string strDataField, out strErr))
					{
						strRandom = strDataField.Substring(strDataField.Length - 8, 8);
						output("随机数:" + strRandom, false, true);
						strRandom = Helps.fnAgainst(strRandom);
					}
					else
					{
						MessageBox.Show("获取随机数失败!");
						return;
					}

					Helps.TryStrToHexByte(Helps.fnAgainst(strRandom + strDateTime), out recByte, out strErr);
					baSendBytes = m_SCUProxy.HY_SendToProxyBuf(01, 0x14, 0x01, recByte);
					output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
					byte[] baRecBytes = SpWuCha.SendCommand(baSendBytes, "EncryptSCU");
					if (baRecBytes == null)
					{
						MessageBox.Show("返回报文为空!");
						return;
					}
					output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);
					byte[] baDataBytes = new byte[4];
					Array.Copy(baRecBytes, (baRecBytes.Length - 5), baDataBytes, 0, baDataBytes.Length);
					_dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
					if (!_dlt645.Pack(ctSeType, lstrDataMark, Helps.fnAgainst(Helps.ByteToStringWithNoSpace(baDataBytes)) + strDateTime, 0, out baSendBytes, out strErr, pmc[7].Value.ToString(), pmc[8].Value.ToString(), pmc[9].Value.ToString()))
					{
						output("点对点加密组包失败!");
					}

					output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
					byte[] recBytes = SpMeter.SendCommand(baSendBytes, "645");
					output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
					if (_dlt645.UnPack(recBytes, ctSeType, lstrDataMark, 0, out isHave, out strDataField, out strErr))
					{
						output("点对点加密校时成功!");
					}
					else
					{
						MessageBox.Show("点对点加密校时失败!");
						return;
					}
				}
			}
			else
			{
				if (!_dlt645.Pack(DLT645.ControlType.msAdjustTime, lstrDataMark, strDateTime, 0, out baBytes, out string strErr))
				{
					MessageBox.Show("组包失败!");
					return;
				}

				if (pmc[6].Value.ToString() == "999999999999")
				{
					output("TX:" + Helps.ByteToStringWithSpace(baBytes), false, true);
					SpMeter.sendWithNoAsk(baBytes, baBytes.Length);
					output(" 广播校时发送成功", false, true);
				}
				else
				{
					if (sendOK(baBytes, 0x9E))
					{
						output(" 校时成功", false, true);
					}
					else
					{
						output(" 校时失败!,请检查", true, true);
					}
				}
			}
		}

		Point cellPosition = new Point(0, 0);
		private void 地址加1ToolStripMenuItem_Click(object sender, EventArgs e)
		{
			dgvMoniter.EndEdit();
			if (cellPosition.Y == dgvMoniter.Columns["通讯地址"].Index)
			{
				if (dgvMoniter.Rows[cellPosition.X].Cells[cellPosition.Y].Value != null)
				{
					string strNum = dgvMoniter.Rows[cellPosition.X].Cells[cellPosition.Y].Value.ToString();
					try
					{
						double.Parse(strNum);
						double number = Convert.ToDouble(strNum);
						for (int i = cellPosition.X + 1; i < 64; i++)
						{
							dgvMoniter.CurrentCell = dgvMoniter.Rows[i].Cells[cellPosition.Y];
							dgvMoniter.Rows[i].Cells[cellPosition.Y].Value = (number + i - cellPosition.X).ToString().PadLeft(12, '0');
						}
					}
					catch (System.Exception ex)
					{
						MessageBox.Show("数值非纯数字信息,请修改");
					}
				}
			}
		}

		private void dgvMoniter_MouseMove(object sender, MouseEventArgs e)
		{
			cellPosition.X = dgvMoniter.HitTest(e.X, e.Y).RowIndex;
			cellPosition.Y = dgvMoniter.HitTest(e.X, e.Y).ColumnIndex;
		}

		private void 取消勾选状态ToolStripMenuItem_Click(object sender, EventArgs e)
		{
			dgvMoniter.EndEdit();
			for (int i = 0; i < this.dgvMoniter.Rows.Count; i++)
			{
				DataGridViewCheckBoxCell checkBox = (DataGridViewCheckBoxCell)this.dgvMoniter.Rows[i].Cells[0];
				checkBox.Value = 0; //设置为0 取消全选
			}
		}

		private void txbCertificateExchange_Click(object sender, EventArgs e)
		{
			txcEncrypt.CheckState = CheckState.Unchecked;
			SpWuCha.setSerialPort(txcbEncrypt.Text.ToString(), 115200, "None", 8, "1", 3000);
			byte[] baSendBytes = null;
			List<string> lstrDataMark = new List<string>();
			for (byte i = 1; i < 4; i++)
			{
				baSendBytes = m_SCUProxy.HY_SendToProxyBuf(0x01, 0x13, i);
				output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
				byte[] baRecBytes = SpWuCha.SendCommand(baSendBytes, "EncryptSCU");
				if (baRecBytes == null)
				{
					MessageBox.Show("返回报文为空!");
					return;
				}
				output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);
				byte[] baDataBytes = new byte[baRecBytes.Length - 9];
				Array.Copy(baRecBytes, 8, baDataBytes, 0, baDataBytes.Length);

				if (!_dlt645.Pack(DLT645.ControlType.SafetyCertificationLTU, lstrDataMark, Helps.ByteToStringWithNoSpace(baDataBytes), 0, out baSendBytes, out string strErr))
				{

				}
				output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
				baRecBytes = SpMeter.SendCommand(baSendBytes, "645");
				if (baRecBytes == null)
				{
					MessageBox.Show("返回报文为空!");
					return;
				}
				output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);
			}

			for (byte i = 4; i < 10; i++)
			{
				baSendBytes = m_SCUProxy.HY_SendToProxyBuf(0x01, 0x13, i);
				output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
				byte[] baRecBytes = SpWuCha.SendCommand(baSendBytes, "EncryptSCU");
				if (baRecBytes == null)
				{
					MessageBox.Show("返回报文为空!");
					return;
				}
				output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);
				byte[] baDataBytes = new byte[baRecBytes.Length - 9];
				Array.Copy(baRecBytes, 8, baDataBytes, 0, baDataBytes.Length);

				if (!_dlt645.Pack(DLT645.ControlType.SafetyCertificationLTU, lstrDataMark, Helps.ByteToStringWithNoSpace(baDataBytes), 0, out baSendBytes, out string strErr))
				{

				}
				output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
				baRecBytes = SpMeter.SendCommand(baSendBytes, "645");
				if (baRecBytes == null)
				{
					MessageBox.Show("返回报文为空!");
					return;
				}
				output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);
				if (!_dlt645.UnPack(baRecBytes, DLT645.ControlType.SafetyCertificationLTU, lstrDataMark, 0, out bool isHaveHouxu, out string strDataField, out strErr))
				{

				}
				Helps.TryStrToHexByte(strDataField, out baDataBytes, out strErr);
				i++;
				baSendBytes = m_SCUProxy.HY_SendToProxyBuf(0x01, 0x13, i, baDataBytes);
				output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
				baRecBytes = SpWuCha.SendCommand(baSendBytes, "EncryptSCU");
				if (baRecBytes == null)
				{
					MessageBox.Show("返回报文为空!");
					return;
				}
				output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);
				//output("第" + i + "帧证书交换成功" );
			}
			output("全部证书交换成功");
		}

		private void txbAuthenticationNegotiation_Click(object sender, EventArgs e)
		{
			SpWuCha.setSerialPort(txcbEncrypt.Text.ToString(), 115200, "None", 8, "1", 3000);
			byte[] baSendBytes = null;
			List<string> lstrDataMark = new List<string>();
			//for (byte i = 10; i < 14; i++)
			{
				baSendBytes = m_SCUProxy.HY_SendToProxyBuf(0x01, 0x13, 10);
				output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
				byte[] baRecBytes = SpWuCha.SendCommand(baSendBytes, "EncryptSCU");
				if (baRecBytes == null)
				{
					MessageBox.Show("返回报文为空!");
					return;
				}
				output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);
				byte[] baDataBytes = new byte[baRecBytes.Length - 9];
				Array.Copy(baRecBytes, 8, baDataBytes, 0, baDataBytes.Length);

				if (!_dlt645.Pack(DLT645.ControlType.SafetyCertificationLTU, lstrDataMark, Helps.ByteToStringWithNoSpace(baDataBytes), 0, out baSendBytes, out string strErr))
				{

				}
				output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
				baRecBytes = SpMeter.SendCommand(baSendBytes, "645");
				if (baRecBytes == null)
				{
					MessageBox.Show("返回报文为空!");
					return;
				}
				output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);
				if (!_dlt645.UnPack(baRecBytes, DLT645.ControlType.SafetyCertificationLTU, lstrDataMark, 0, out bool isHaveHouxu, out string strDataField, out strErr))
				{

				}
				Helps.TryStrToHexByte(strDataField, out baDataBytes, out strErr);
				baSendBytes = m_SCUProxy.HY_SendToProxyBuf(0x01, 0x13, 11, baDataBytes);
				output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
				baRecBytes = SpWuCha.SendCommand(baSendBytes, "EncryptSCU");
				if (baRecBytes == null)
				{
					MessageBox.Show("返回报文为空!");
					return;
				}
				output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);

				//Helps.TryStrToHexByte(strDataField, out baDataBytes, out strErr);
				baSendBytes = m_SCUProxy.HY_SendToProxyBuf(0x01, 0x13, 12, baDataBytes);
				output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
				baRecBytes = SpWuCha.SendCommand(baSendBytes, "EncryptSCU");
				if (baRecBytes == null)
				{
					MessageBox.Show("返回报文为空!");
					return;
				}
				output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);

				baDataBytes = new byte[baRecBytes.Length - 9];
				Array.Copy(baRecBytes, 8, baDataBytes, 0, baDataBytes.Length);

				if (!_dlt645.Pack(DLT645.ControlType.SafetyCertificationLTU, lstrDataMark, Helps.ByteToStringWithNoSpace(baDataBytes), 0, out baSendBytes, out strErr))
				{

				}
				output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
				baRecBytes = SpMeter.SendCommand(baSendBytes, "645");
				if (baRecBytes == null)
				{
					MessageBox.Show("返回报文为空!");
					return;
				}
				output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);
				if (!_dlt645.UnPack(baRecBytes, DLT645.ControlType.SafetyCertificationLTU, lstrDataMark, 0, out isHaveHouxu, out strDataField, out strErr))
				{

				}
				Helps.TryStrToHexByte(strDataField, out baDataBytes, out strErr);
				baSendBytes = m_SCUProxy.HY_SendToProxyBuf(0x01, 0x13, 13, baDataBytes);
				output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
				baRecBytes = SpWuCha.SendCommand(baSendBytes, "EncryptSCU");
				if (baRecBytes == null)
				{
					MessageBox.Show("返回报文为空!");
					return;
				}
				output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);
				output("随机数:" + strDataField.Substring(strDataField.Length - 8), false, true);
				output("认证协商成功");
				txcEncrypt.CheckState = CheckState.Checked;
				txtxbTXAddr.Text = pmc[6].Value.ToString();
			}
		}

		private void txbSpecialTest_Click(object sender, EventArgs e)
		{
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}

			if ((txtbPercentU.Text == "") || (txtbPercentI.Text == "") || (txtbAngle.Text == ""))
			{
				MessageBox.Show("请先填写电压及电流百分比!");
				return;
			}
			if (SpMeter.Working)
			{
				return;
			}
			try
			{
				DataRow[] drs = dtSpecialReaDataTable.Select(string.Format("选择 = '{0}' ", "true"), "");
				int rowCount = drs.GetLength(0);
				if (rowCount < 1)
				{
					return;
				}

				SpMeter.Working = true;

				#region 抄读
				bool isReadingErr = false;
				DLT645 dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
				for (int i = 0; i < rowCount && SpMeter.Working; i++)
				{
					List<string> dataMarkList = new List<string>();
					byte seq = 0;
					string dataResultStr = "";

					if (drs[i]["DataMark"] == null || drs[i]["DataMark"].ToString().Length != 8)
					{
						drs[i]["读取值"] = "";
						drs[i]["相对误差%"] = "抄读失败：数据标识非法！";
						continue;
					}

					string dataMark = drs[i]["DataMark"].ToString();
					dataMarkList.Add(dataMark);

					byte[] sendBytes = null;
					string errStr = "";
					if (!dlt645.Pack(DLT645.ControlType.Read, dataMarkList, "", 0, out sendBytes, out errStr))
					{
						drs[i]["读取值"] = "";
						drs[i]["相对误差%"] = errStr;
						continue;
					}

					string strAPDU = "";
					byte[] baAPDU;
					byte[] baTemp;
					byte[] recBytes;
					{
						output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
						recBytes = SpMeter.SendCommand(sendBytes, "645");
					}

					output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
					bool isHaveHouxu = false;
					if (!SpMeter.Working)
					{
						continue;
					}

					string dataPianDuan;
					DLT645.ControlType controlType = DLT645.ControlType.Read;

					if (!dlt645.UnPack(recBytes, controlType, dataMarkList, seq, out isHaveHouxu, out dataPianDuan, out errStr))
					{
						drs[i]["读取值"] = "";
						drs[i]["相对误差%"] = errStr;
						isReadingErr = true;
						continue;
					}

					dataResultStr = dataPianDuan.Trim();

					int dataLen;

					if (!int.TryParse(drs[i]["DataLen"].ToString(), out dataLen))
					{
						drs[i]["读取值"] = "";
						drs[i]["相对误差%"] = "数据库中该数据项长度非法，请排查数据库信息！";
						continue;
					}

					if (dataResultStr.Length != dataLen * 2)
					{
						drs[i]["读取值"] = "";
						drs[i]["相对误差%"] = "失败：返回数据长度非法";
						continue;
					}
					int decimalPlaces = 0;
					if (drs[i]["DecimalPlaces"] == null || (!int.TryParse(drs[i]["DecimalPlaces"].ToString(), out decimalPlaces)))
					{
						decimalPlaces = 0;
					}
					string strSymble = "";
					if ((drs[i]["Name"].ToString().Contains("功率因数")) || (drs[i]["DataUnit"].ToString() == "A") || (drs[i]["DataUnit"].ToString() == "kW") || (drs[i]["DataUnit"].ToString() == "Kvar") || (drs[i]["DataUnit"].ToString() == "kVA"))
					{
						int iTe = 0;

						if ((Convert.ToInt64("0x" + dataResultStr.Substring(0, 1), 16) & 0x08) == 8)
						{
							strSymble = "-";
						}
						iTe = (int)(Convert.ToInt64("0x" + dataResultStr.Substring(0, 1), 16) & 0x07);
						dataResultStr = strSymble + iTe.ToString() + dataResultStr.Substring(1);
					}
					if (decimalPlaces != 0)
					{
						dataResultStr = dataResultStr.Insert(dataResultStr.Length - decimalPlaces, ".");
					}
					drs[i]["读取值"] = dataResultStr;
					//GetData(drs[i], dataResultStr, dataLen);
					if ((drs[i]["Name"].ToString().Contains("电压")))
					{
						double[] daU = { 0.1, 0, -0.1 };
						double dUe = GetRandomNumber(daU);
						drs[i]["读取值"] = (double.Parse(txtbU.Text.ToString()) * double.Parse(txtbPercentU.Text.ToString()) / 100 + dUe).ToString("F1");
						drs[i]["相对误差%"] = (dUe * 100 / double.Parse(txtbU.Text.ToString())).ToString("F2");
					}
					else if ((drs[i]["Name"].ToString().Contains("电流")))
					{
						double[] daII = { 0.011, 0.010, 0.009, 0.008, 0.007, 0.006, 0.005, 0.004, 0.003, 0.002, 0.001, 0, -0.011, -0.010, -0.009, -0.008, -0.007, -0.006, -0.005, -0.004, -0.003, -0.002, -0.001 };
						double[] daI = { 0.011, 0.010, 0.009, 0.008, 0.007, 0.006, 0.005, 0.004, 0.003, 0.002, 0.001, 0, -0.011, -0.010, -0.009, -0.008, -0.007, -0.006, -0.005, -0.004, -0.003, -0.002, -0.001 };
						double dUI = GetRandomNumber(daI) + GetRandomNumber(daII);
						drs[i]["读取值"] = (double.Parse(txtbI.Text.ToString()) * double.Parse(txtbPercentI.Text.ToString()) / 100 + dUI).ToString("F3");
						drs[i]["相对误差%"] = (dUI * 100 / double.Parse(txtbI.Text.ToString())).ToString("F2");
					}
					else if ((drs[i]["Name"].ToString().Contains("功率因数")))
					{
						double[] daCOS = { 0, -0.001 };
						double dCOS = GetRandomNumber(daCOS);
						drs[i]["读取值"] = (Math.Cos(double.Parse(txtbAngle.Text.ToString()) * (Math.PI) / 180) + dCOS).ToString("F3");
						drs[i]["相对误差%"] = (dCOS * 100).ToString("F2");
					}
					else if ((drs[i]["Name"].ToString().Contains("视在功率")))
					{
						double[] daApparentPowper = { 0.0101, 0.0102, 0.0103, 0.0124, 0.0115, 0.0106, 0.0097, 0.0086, 0.0075, 0.0064, 0.0053, 0.0042, 0.0031, 0.0021, 0.0011, 0, -0.0101, -0.0102, -0.0103, -0.0104, -0.0115, -0.0104, -0.0093, -0.0082, -0.0071, -0.0063, -0.0054, -0.0045, -0.0036, -0.0022, -0.0011 };
						double[] daApparentPowperH = { 0.003, 0.002, 0.001, 0, -0.003, -0.002, -0.001 };
						double dApparentPowper = GetRandomNumber(daApparentPowper) + GetRandomNumber(daApparentPowperH);
						drs[i]["读取值"] = ((double.Parse(txtbI.Text.ToString()) * double.Parse(txtbPercentI.Text.ToString()) / 100) * (double.Parse(txtbU.Text.ToString()) * double.Parse(txtbPercentU.Text.ToString()) / 100) * 3 / 1000 + dApparentPowper).ToString("F4");
						drs[i]["相对误差%"] = (dApparentPowper * 100 / (double.Parse(txtbI.Text.ToString()) * (double.Parse(txtbU.Text.ToString())) * 3 / 1000)).ToString("F2");
					}
					else if ((drs[i]["Name"].ToString().Contains("有功功率")))
					{
						double[] daApparentPowper = { 0.0101, 0.0102, 0.0103, 0.0104, 0.0115, 0.0106, 0.0097, 0.0086, 0.0075, 0.0064, 0.0053, 0.0042, 0.0031, 0.0021, 0.0011, 0, -0.0101, -0.0102, -0.0103, -0.0104, -0.0115, -0.0104, -0.0093, -0.0082, -0.0071, -0.0063, -0.0054, -0.0045, -0.0036, -0.0022, -0.0011 };
						double[] daApparentPowperH = { 0.003, 0.002, 0.001, 0, -0.003, -0.002, -0.001 };
						double dApparentPowper = (GetRandomNumber(daApparentPowper) + GetRandomNumber(daApparentPowperH));
						if (txtbAngle.Text.ToString() != "90" && txtbAngle.Text.ToString() != "270")
						{
							dApparentPowper = (GetRandomNumber(daApparentPowper) + GetRandomNumber(daApparentPowperH)) * Math.Cos(double.Parse(txtbAngle.Text.ToString()) * (Math.PI) / 180);
						}

						drs[i]["读取值"] = ((double.Parse(txtbI.Text.ToString()) * double.Parse(txtbPercentI.Text.ToString()) / 100) * (double.Parse(txtbU.Text.ToString()) * double.Parse(txtbPercentU.Text.ToString()) / 100) * 3 * Math.Cos(double.Parse(txtbAngle.Text.ToString()) * (Math.PI) / 180) / 1000 + dApparentPowper).ToString("F4");
						if (txtbAngle.Text.ToString() != "90" && txtbAngle.Text.ToString() != "270")
						{
							drs[i]["相对误差%"] = (dApparentPowper * 100 / (double.Parse(txtbI.Text.ToString()) * (double.Parse(txtbU.Text.ToString()) * 3 * Math.Cos(double.Parse(txtbAngle.Text.ToString()) * (Math.PI) / 180) / 1000))).ToString("F2");
						}
						else
						{
							drs[i]["相对误差%"] = "";
						}
					}
					else if ((drs[i]["Name"].ToString().Contains("无功功率")))
					{
						double[] daApparentPowper = { 0.0151, 0.0142, 0.0133, 0.0124, 0.0115, 0.0106, 0.0097, 0.0086, 0.0075, 0.0064, 0.0053, 0.0042, 0.0031, 0.0021, 0.0011, 0, -0.0151, -0.0142, -0.0133, -0.0124, -0.0115, -0.0104, -0.0093, -0.0082, -0.0071, -0.0063, -0.0054, -0.0045, -0.0036, -0.0022, -0.0011 };
						double[] daApparentPowperH = { 0.5, 0.6, 0.7, 0.8 };
						double dApparentPowper = GetRandomNumber(daApparentPowper) + GetRandomNumber(daApparentPowperH);
						drs[i]["读取值"] = ((double.Parse(txtbI.Text.ToString()) * double.Parse(txtbPercentI.Text.ToString()) / 100) * (double.Parse(txtbU.Text.ToString()) * double.Parse(txtbPercentU.Text.ToString()) / 100) * 3 * Math.Sin(double.Parse(txtbAngle.Text.ToString()) * (Math.PI) / 180) / 1000 + dApparentPowper).ToString("F4");
						if (txtbAngle.Text.ToString() != "0" && txtbAngle.Text.ToString() != "180")
						{
							drs[i]["相对误差%"] = (dApparentPowper * 100 / (double.Parse(txtbI.Text.ToString()) * (double.Parse(txtbU.Text.ToString()) * 3 * Math.Sin(double.Parse(txtbAngle.Text.ToString()) * (Math.PI) / 180) / 1000))).ToString("F2");
						}
						else
						{
							drs[i]["相对误差%"] = "";
						}
					}
					Thread.Sleep(16);
				}
				#endregion
			}
			catch (Exception exception)
			{
				output("抄读过程中出现错误：" + exception.Message, true, true);
			}
			finally
			{
				if (SpMeter.Working)
				{
					SpMeter.Working = false;
				}
			}
		}

		// 随机抽取数组中的数据
		static double GetRandomNumber(double[] a)
		{
			Random rnd = new Random();
			int index = rnd.Next(a.Length);
			return a[index];
		}

		private void txbReadVersion_Click(object sender, EventArgs e)
		{
			GetMeterVersion();
		}

		private void txbBase64En_Click(object sender, EventArgs e)
		{
			byte[] bytes = DataConvert.HEXString2Bytes(txtbInputText.Text);
			txtbOutputText.Text = System.Convert.ToBase64String(bytes);
		}

		private void txbBase64De_Click(object sender, EventArgs e)
		{
			byte[] bytes = System.Convert.FromBase64String(txtbInputText.Text);
			txtbOutputText.Text = DataConvert.Bytes2HexString(bytes, 0, bytes.Length);
		}

		private void txcbEncrypt_DropDown(object sender, EventArgs e)
		{
			int i = 0;
			this.txcbEncrypt.Items.Clear(); // 清除串口
			string[] gCOM = System.IO.Ports.SerialPort.GetPortNames(); // 获取设备的所有可用串口
			int j = gCOM.Length; // 得到所有可用串口数目
			for (i = 0; i < j; i++)
			{
				this.txcbEncrypt.Items.Add(gCOM[i]); // 依次添加到下拉框中
			}
		}

		bool bTeleindicationTest = false;
		int iTeleindicationTestCount = 0;

		/// <summary>
		/// 初始化
		/// </summary>
		private void InitTeleindicationTest()
		{
			#region InitTeleindicationTest
			dgvTeleindication.AutoGenerateColumns = false;
			dtTeleindication = new DataTable(null);
			dtTeleindication.Columns.Add("选择", typeof(bool));
			dtTeleindication.Columns.Add("Name", typeof(string));
			dtTeleindication.Columns.Add("DataMark", typeof(string));
			dtTeleindication.Columns.Add("DataMarkFather", typeof(string));
			dtTeleindication.Columns.Add("DataLen", typeof(string));
			dtTeleindication.Columns.Add("DataBlockType", typeof(string));
			dtTeleindication.Columns.Add("DataFormat", typeof(string));
			dtTeleindication.Columns.Add("DecimalPlaces", typeof(string));
			dtTeleindication.Columns.Add("DataUnit", typeof(string));
			dtTeleindication.Columns.Add("读取值", typeof(string));
			dtTeleindication.Columns.Add("操作结论", typeof(string));
			List<string> lsList = new List<string>();
			{
				lsList.Add("TRUE-刷新时间-DBDF113B-04DFDFDB-4--HEX10-0---");
				lsList.Add("TRUE-遥信状态-DBDF113E-04DFDFDB-1--HEX10-0---");
				lsList.Add("TRUE-数值-DBDF113F-04DFDFDB-4--HEX10-3---");
				lsList.Add("TRUE-滤波延时-DBDF1142-04DFDFDB-4--HEX10-0---");
			}

			foreach (var VARIABLE in lsList)
			{
				object[] oRow = VARIABLE.Split('-');
				dtTeleindication.Rows.Add(oRow);
			}
			#endregion
			dgvTeleindication.DataSource = dtTeleindication;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void txbStartTeleindication_Click(object sender, EventArgs e)
		{
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}

			try
			{
				if (txbStartTeleindication.Text == "开始测试")
				{
					if (!int.TryParse(pmc[5].Value.ToString(), out int overTime))
					{
						MessageBox.Show("非法的等待超时时间！");
						return;
					}
					SpMeter.OverTime = overTime;
					drs = dtTeleindication.Select(string.Format("选择 = '{0}' ", "true"), "");
					iTeleindicationTestCount = int.Parse(txtbTeleindicationNum.Text);
					SpMeter.Working = true;
					bTeleindicationTest = true;
					txbStartTeleindication.Text = "停止测试";
					uiSendCount = 0;
					uiSendCorrentCount = 0;
					#region Thread
					Thread td = new Thread(new ThreadStart(tTestTeleindication));
					td.IsBackground = true;
					td.Start();
					#endregion
				}
				else
				{
					SpMeter.Working = false;
					bTeleindicationTest = false;
					txbStartTeleindication.Text = "开始测试";
				}
			}
			catch (Exception exception)
			{
				output("设置过程中出现错误：" + exception.Message, true, true);
				SpMeter.Working = false;
				bTeleindicationTest = false;
				txbStartTeleindication.Text = "开始测试";
			}
			finally
			{

			}
		}

		/// <summary>
		/// 
		/// </summary>
		private void tTestTeleindication()
		{
			try
			{
				DataTable dtDatas = new DataTable();
				for (int m = 0; m < iTeleindicationTestCount && SpMeter.Working && bTeleindicationTest; m++)
				{
					lstrDataMark.Clear();
					lstrDataMark.Add("DBDF0201");
					lstrDataMark.Add("04DFDFDB");
					string strErr = FunDealMessage("01", DLT645.ControlType.Write, lstrDataMark, out string strData);
					if (strErr != "")
					{
						if (strErr != "请先打开串口!")
						{
							MessageBox.Show(strErr);
						}
						bTeleindicationTest = false;
						break;
					}
					Thread.Sleep(1000);
					lstrDataMark.Clear();
					lstrDataMark.Add("DBDF113A");
					lstrDataMark.Add("04DFDFDB");
					strErr = FunDealMessage("01", DLT645.ControlType.Write, lstrDataMark, out strData);
					if (strErr != "")
					{
						if (strErr != "请先打开串口!")
						{
							MessageBox.Show(strErr);
						}
						bTeleindicationTest = false;
						break;
					}
					Thread.Sleep(1000);
					DLT645 dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));

					for (int i = 0; i < dgvTeleindication.RowCount && SpMeter.Working && bTeleindicationTest; i++)
					{
						if (dtDatas.Columns.Count < i + 1)
						{
							dtDatas.Columns.Add(dtTeleindication.Rows[i][1].ToString(), typeof(string));
						}
						if (dtDatas.Rows.Count < m + 1)
						{
							dtDatas.Rows.Add();
						}
						#region 抄读
						bool isReadingErr = false;

						List<string> dataMarkList = new List<string>();
						byte seq = 0;
						string dataResultStr = "";

						if (drs[i]["DataMark"] == null || drs[i]["DataMark"].ToString().Length != 8)
						{
							strDataParse = "数据标识非法！";
							drs[i]["读取值"] = "";
							drs[i]["操作结论"] = "抄读失败：数据标识非法！";
							continue;
						}

						string dataMark = drs[i]["DataMark"].ToString();
						dataMarkList.Add(dataMark);
						if (drs[i]["DataMarkFather"] != null && drs[i]["DataMarkFather"].ToString().Length == 8)
						{
							dataMarkList.Add(drs[i]["DataMarkFather"].ToString());
						}

						byte[] sendBytes = null;
						string errStr = "";

						if (!dlt645.Pack(DLT645.ControlType.Read, dataMarkList, "", 0, out sendBytes, out errStr))
						{
							strDataParse = errStr;
							drs[i]["读取值"] = "";
							drs[i]["操作结论"] = errStr;
							continue;
						}

						string strAPDU = "";
						byte[] recBytes;

						output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
						recBytes = SpMeter.SendCommand(sendBytes, "645");
						output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
						bool isHaveHouxu = false;
						do
						{
							if (!SpMeter.Working)
							{
								continue;
							}

							string dataPianDuan;
							DLT645.ControlType controlType = DLT645.ControlType.Read;
							if (txcEncrypt.Checked)
							{
								controlType = DLT645.ControlType.SafetyReadLTU;
							}

							if (isHaveHouxu)
							{
								controlType = DLT645.ControlType.ReadFollow;
							}

							if (!dlt645.UnPack(recBytes, controlType, dataMarkList, seq, out isHaveHouxu, out dataPianDuan, out errStr))
							{
								strDataParse = errStr;
								drs[i]["读取值"] = "";
								drs[i]["操作结论"] = errStr;
								isReadingErr = true;
								break;
							}

							if (!string.IsNullOrEmpty(drs[i]["DataBlockType"].ToString()))
							{
								dataPianDuan = Helps.fnAgainst(dataPianDuan);
							}

							dataResultStr += dataPianDuan.Trim();
							if (isHaveHouxu)
							{
								seq++;
								if (!dlt645.Pack(DLT645.ControlType.ReadFollow, dataMarkList, "", seq, out sendBytes, out errStr))
								{
									strDataParse = errStr;
									drs[i]["读取值"] = "";
									drs[i]["操作结论"] = errStr;
									isReadingErr = true;
									break;
								}

								output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
								recBytes = SpMeter.SendCommand(sendBytes, "645");
								output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
							}
						} while (isHaveHouxu);

						if (isReadingErr)
						{
							isReadingErr = false;
							continue;
						}

						int dataLen;
						if (drs[i]["DataLen"].ToString().Contains("*"))
						{
							GetData(drs[i], dataResultStr, dataResultStr.Length);
						}
						else
						{
							if (!int.TryParse(drs[i]["DataLen"].ToString(), out dataLen))
							{
								strDataParse = "返回数据长度非法";
								drs[i]["读取值"] = "";
								drs[i]["操作结论"] = "数据库中该数据项长度非法，请排查数据库信息！";
								continue;
							}

							if (dataResultStr.Length != dataLen * 2)
							{
								strDataParse = "返回数据长度非法";
								drs[i]["读取值"] = "";
								drs[i]["操作结论"] = "失败：返回数据长度非法";
								continue;
							}
							GetData(drs[i], dataResultStr, dataLen);

							dtDatas.Rows[m][i] = strDataParse;
						}
						#endregion
					}
					lstrDataMark.Clear();
					lstrDataMark.Add("DBDF113c");
					lstrDataMark.Add("04DFDFDB");
					strErr = FunDealMessage("01", DLT645.ControlType.Write, lstrDataMark, out strData);
					if (strErr != "")
					{
						if (strErr != "请先打开串口!")
						{
							MessageBox.Show(strErr);
						}
						bTeleindicationTest = false;
						break;
					}
				}

				Thread.Sleep(1000);
				AsposeExcelTools.DataTableToExcel(dtDatas, string.Format("{0}", System.Environment.CurrentDirectory) + string.Format("\\LOG\\{0:yyyyMMdd HHmmss}" + ".xlsx", DateTime.Now), out string strErrs);
				//output("累计发送报文" + uiSendCount.ToString() + "条," + "成功接收" + uiSendCorrentCount.ToString() + "条", true, false);
			}
			catch (Exception e)
			{
				Console.WriteLine(e);
			}
		}

		#region 停止测试
		public delegate void dStopTeleindication(object objIndex);
		dStopTeleindication stTeleindication;

		public void FnStopTeleindicationHandler(object objIndex)
		{
			try
			{
				if (stTeleindication == null)
				{
					stTeleindication = new dStopTeleindication(StopTeleindication);
				}
				Invoke(stTeleindication, new object[] { objIndex });
			}
			catch
			{

			}
		}

		/// <summary>
		/// btnTest
		/// </summary>
		/// <param name="objIndex"></param>
		public void StopTeleindication(object objIndex)
		{
			lock (m_objectShow)
			{
				bTeleindicationTest = false;
				txbStartTeleindication.Text = "开始测试";
				SpMeter.Working = false;
			}
		}
		#endregion

		private void txbOneKey_Click(object sender, EventArgs e)
		{
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}
			byte[] TxBufFactoryMode = new byte[32];
			string strDataSend = "68" + Helps.fnAgainst(pmc[6].Value.ToString()) + "6814150E121237" + (DataConvert.fnEncodingData(pmc[8].Value.ToString() + pmc[7].Value.ToString())) + "373635343435120EBBBBBBBB34";
			Helps.TryStrToHexByte(strDataSend, out TxBufFactoryMode);

			#region 校验和+16
			byte sum = Helps.Ssum(TxBufFactoryMode);
			byte[] endBytes = new[] { sum, (byte)0x16 };
			#endregion

			TxBufFactoryMode = TxBufFactoryMode.Concat(endBytes).ToArray();
			//byte[] TxBufFactoryMode = { 0xfe, 0xfe, 0xfe, 0xfe, 0x68, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x68, 0x14, 0x15, 0x0E, 0x12, 0x12, 0x37, 0x35, 0x33, 0x33, 0x33, 0x37, 0x36, 0x35, 0x34, 0x34, 0x35, 0x12, 0xE, 0xBB, 0xBB, 0xBB, 0xBB, 0x34, 0xdf, 0x16 };
			if (sendOK(TxBufFactoryMode, 0x94))
			{
				output(" 打开编程模式成功", false, true);
			}
			else
			{
				output(" 打开编程模式失败!", true, true);
			}

			Thread.Sleep(10);
			byte[] TxBufInitial = new byte[31];
			strDataSend = "68" + Helps.fnAgainst(pmc[6].Value.ToString()) + "6814140E121237" + (DataConvert.fnEncodingData(pmc[8].Value.ToString() + pmc[7].Value.ToString())) + "373635340E3A120EBBBBBBBB";
			Helps.TryStrToHexByte(strDataSend, out TxBufInitial);

			#region 校验和+16
			sum = Helps.Ssum(TxBufInitial);
			endBytes = new[] { sum, (byte)0x16 };
			#endregion

			TxBufInitial = TxBufInitial.Concat(endBytes).ToArray();

			if (sendOK(TxBufInitial, 0x94))
			{
				output("初始化电表成功", false, true);//Initial Meter Success
			}
			else
			{
				output("通信异常,请检查", false, true);//Meter reply error,Check the connection please
			}
			Thread.Sleep(13000);

			string strDateTime = DateTime.Now.ToString("yyMMdd") + ((int)DateTime.Now.DayOfWeek).ToString("X2") + DateTime.Now.ToString("HHmmss");

			string strErr = "";
			lstrDataMark.Clear();
			lstrDataMark.Add("0400010C");
			strErr = FunDealMessage(strDateTime, DLT645.ControlType.Write, lstrDataMark, out string strData);
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

			//byte[] baBytes = null;
			//string strDateTime = DateTime.Now.ToString("yyMMdd") + DateTime.Now.ToString("HHmmss") + DateTime.Now.Millisecond.ToString().PadLeft(4, '0');
			//lstrDataMark.Clear();
			//_dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
			//if (!_dlt645.Pack(DLT645.ControlType.msAdjustTime, lstrDataMark, strDateTime, 0, out baBytes, out string strErr))
			//{
			//	MessageBox.Show("组包失败!");
			//	return;
			//}

			//if (sendOK(baBytes, 0x9E))
			//{
			//	output(" 校时成功", false, true);
			//}
			//else
			//{
			//	output(" 校时失败!,请检查", true, true);
			//}
		}

		#region BLE
		private void btnSearch_Click(object sender, EventArgs e)
		{
			//清除设备列表
			devices.Clear();
			lv_device.Items.Clear();
			output("正在搜索设备...");
			//启动设备搜索
			StartBleDeviceWatcher();
		}

		/// <summary>
		/// 查询附近设备
		/// </summary>
		private void StartBleDeviceWatcher()
		{
			//BLE device watch
			// Query for extra properties you want returned
			string[] requestedProperties = { "System.Devices.Aep.DeviceAddress", "System.Devices.Aep.IsConnected", "System.Devices.Aep.Bluetooth.Le.IsConnectable" };
			string aqsAllBluetoothLEDevices = "(System.Devices.Aep.ProtocolId:=\"{bb7bb05e-5972-42b5-94fc-76eaa7084d49}\")";

			//aqsAllBluetoothLEDevices,
			deviceWatcher = DeviceInformation.CreateWatcher(BluetoothLEDevice.GetDeviceSelectorFromPairingState(false), requestedProperties, DeviceInformationKind.AssociationEndpoint);

			// Register event handlers before starting the watcher.
			// Added, Updated and Removed are required to get all nearby devices
			deviceWatcher.Added += DeviceWatcher_Added;
			deviceWatcher.Updated += DeviceWatcher_Updated;
			deviceWatcher.Removed += DeviceWatcher_Removed;
			// EnumerationCompleted and Stopped are optional to implement.
			deviceWatcher.EnumerationCompleted += DeviceWatcher_EnumerationCompleted;
			deviceWatcher.Stopped += DeviceWatcher_Stopped;
			// Start the watcher.
			deviceWatcher.Start();
		}

		private int Strchr(string originalString, char charToSearch)
		{
			int found = originalString.IndexOf(charToSearch);
			return found > -1 ? found : -1;
		}

		private string Strchr(string originalString, char charToSearch, bool pastIt)
		{
			int found = originalString.IndexOf(charToSearch);
			return found > -1 ? originalString.Substring(found + (pastIt ? 1 : 0)) : "";
		}

		/// <summary>
		/// 发现新设备
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="deviceInfo"></param>
		private void DeviceWatcher_Added(DeviceWatcher sender, DeviceInformation deviceInfo)
		{
			try
			{
				this.Invoke(new Action(() =>
				{
					if (sender == deviceWatcher)
					{
						// Make sure device isn't already present in the list.
						if (!devices.ContainsKey(deviceInfo.Id))
						{
							if (deviceInfo.Name != "")
							{
								string id = deviceInfo.Id;

								devices.Add(deviceInfo.Id, deviceInfo);
								//添加到列表
								ListViewItem lvi = new ListViewItem();
								lvi.Text = deviceInfo.Name;
								lvi.Name = id;

								string Mac = "";
								if (Strchr(id, '-') > 0)
								{
									Mac = Strchr(id, '-', true);
								}
								lvi.SubItems.Add(Mac);
								lvi.SubItems.Add(id);
								//lvi.SubItems.Add(deviceInfo.Pairing.CanPair.ToString());
								//lvi.SubItems.Add(deviceInfo.Pairing.IsPaired.ToString());
								//lvi.SubItems.Add("");
								lv_device.Items.Add(lvi);
							}
						}
					}
				}));
			}
			catch
			{

			}
		}

		/// <summary>
		/// 设备信息被更新
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="deviceInfoUpdate"></param>
		private void DeviceWatcher_Updated(DeviceWatcher sender, DeviceInformationUpdate deviceInfoUpdate)
		{
			try
			{
				this.Invoke(new Action(() =>
				{
					// Protect against race condition if the task runs after the app stopped the deviceWatcher.
					if (sender == deviceWatcher)
					{
						//存在该设备
						if (devices.Keys.Contains(deviceInfoUpdate.Id) && devices[deviceInfoUpdate.Id] != null)
						{
							string id = deviceInfoUpdate.Id;
							devices[deviceInfoUpdate.Id].Update(deviceInfoUpdate);
							lv_device.Items[id].SubItems[0].Text = devices[deviceInfoUpdate.Id].Name;
							//lv_device.Items[id].SubItems[2].Text = devices[deviceInfoUpdate.Id].Pairing.CanPair.ToString();
							//lv_device.Items[id].SubItems[3].Text = devices[deviceInfoUpdate.Id].Pairing.IsPaired.ToString();
						}
					}
				}));
			}
			catch
			{

			}
		}

		/// <summary>
		/// 从设备列表移除设备
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="deviceInfoUpdate"></param>
		private void DeviceWatcher_Removed(DeviceWatcher sender, DeviceInformationUpdate deviceInfoUpdate)
		{
			this.Invoke(new Action(() =>
			{
				if (sender == deviceWatcher)
				{
					string id = deviceInfoUpdate.Id;
					//if (chb_ShowUnconnectableDevice.Checked != true)
					//{
					devices.Remove(deviceInfoUpdate.Id);
					lv_device.Items.RemoveByKey(id);
					//}
				}
			}));
		}

		/// <summary>
		/// 设备枚举完成
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void DeviceWatcher_EnumerationCompleted(DeviceWatcher sender, object e)
		{
			this?.Invoke(new Action(() =>
			{
				//log("设备搜索完毕", Color.Green);
			}));
		}

		/// <summary>
		/// 终止设备搜索
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void DeviceWatcher_Stopped(DeviceWatcher sender, object e)
		{
			this?.Invoke(new Action(() =>
			{
				//log("搜索被终止", Color.Red);
			}));
		}

		public void AddDataInfoData(string str)
		{
			try
			{
				output(str);
			}
			catch
			{

			}
		}

		private void CurrentDevice_ConnectionStatusChanged(BluetoothLEDevice sender, object args)
		{
			if (sender.ConnectionStatus == BluetoothConnectionStatus.Disconnected && ble_Device.DeviceId != null)
			{
				this.Invoke(new Action(() =>
				{
					AddDataInfoData(Dev_Name_Str + " 已断链！");
					Dev_ID_Str = "";
				}));
			}
			else
			{
				this.Invoke(new Action(() =>
				{
					AddDataInfoData(Dev_Name_Str + " 已连接！");
					ble_Device = sender;
				}));
			}
		}

		/// <summary>
		/// 为蓝牙的characteristic设置回调使能和回调函数
		/// </summary>
		private async void UnhookNotify()
		{
			if (last_ble_Characteristic_0xFFF1 == null) return;
			GattCharacteristicProperties last_properties = last_ble_Characteristic_0xFFF1.CharacteristicProperties;
			//特征支持Nodify或者Indicate属性
			if (last_properties.HasFlag(GattCharacteristicProperties.Notify) || last_properties.HasFlag(GattCharacteristicProperties.Indicate))
			{
				//解除上一个属性的Notify和Indicate
				try
				{
					// BT_Code: Must write the CCCD in order for server to send notifications.
					// We receive them in the ValueChanged event handler.
					// Note that this sample configures either Indicate or Notify, but not both.
					var result = await last_ble_Characteristic_0xFFF1.WriteClientCharacteristicConfigurationDescriptorAsync(GattClientCharacteristicConfigurationDescriptorValue.None);
					if (result == GattCommunicationStatus.Success)
					{
						//设置回调函数
						last_ble_Characteristic_0xFFF1.ValueChanged -= Characteristic_ValueChanged;
					}
				}
				catch (Exception ex)
				{
					// This usually happens when a device reports that it support notify, but it actually doesn't.
					//log(ex.Message);
				}
			}
		}

		private async void UnhookNotifySubscribe()
		{
			if (last_ble_CharacteristicSubscribe == null) return;
			GattCharacteristicProperties last_properties = last_ble_CharacteristicSubscribe.CharacteristicProperties;
			//特征支持Nodify或者Indicate属性
			if (last_properties.HasFlag(GattCharacteristicProperties.Notify) || last_properties.HasFlag(GattCharacteristicProperties.Indicate))
			{
				//解除上一个属性的Notify和Indicate
				try
				{
					// BT_Code: Must write the CCCD in order for server to send notifications.
					// We receive them in the ValueChanged event handler.
					// Note that this sample configures either Indicate or Notify, but not both.
					var result = await last_ble_CharacteristicSubscribe.WriteClientCharacteristicConfigurationDescriptorAsync(GattClientCharacteristicConfigurationDescriptorValue.None);
					if (result == GattCommunicationStatus.Success)
					{
						//设置回调函数
						last_ble_CharacteristicSubscribe.ValueChanged -= Characteristic_ValueChanged;
					}
				}
				catch (Exception ex)
				{
					// This usually happens when a device reports that it support notify, but it actually doesn't.
					//log(ex.Message);
				}
			}
		}

		//为蓝牙的characteristic设置回调使能和回调函数
		private async void HookNotify()
		{
			GattCharacteristicProperties properties = ble_Characteristic_0xFFF1.CharacteristicProperties;
			//特征支持Nodify或者Indicate属性
			if (properties.HasFlag(GattCharacteristicProperties.Notify) || properties.HasFlag(GattCharacteristicProperties.Indicate))
			{
				// initialize status
				GattCommunicationStatus status = GattCommunicationStatus.Unreachable;
				var cccdValue = GattClientCharacteristicConfigurationDescriptorValue.None;
				if (properties.HasFlag(GattCharacteristicProperties.Indicate))
				{
					cccdValue = GattClientCharacteristicConfigurationDescriptorValue.Indicate;
				}

				else if (properties.HasFlag(GattCharacteristicProperties.Notify))
				{
					cccdValue = GattClientCharacteristicConfigurationDescriptorValue.Notify;
				}

				try
				{
					//设置回调使能
					// BT_Code: Must write the CCCD in order for server to send indications.
					status = await ble_Characteristic_0xFFF1.WriteClientCharacteristicConfigurationDescriptorAsync(cccdValue);

					if (status == GattCommunicationStatus.Success)
					{
						//设置回调函数
						ble_Characteristic_0xFFF1.ValueChanged += Characteristic_ValueChanged;
						output("成功订阅了服务特征变化的通知");
					}
					else
					{
						AddDataInfoData($"0xFFF1未成功订阅服务特征变化的通知: {status}");
					}
				}
				catch (Exception ex)
				{
					// This usually happens when a device reports that it support indicate, but it actually doesn't.
					//log(ex.Message);
				}
			}
		}

		//为蓝牙的characteristic设置回调使能和回调函数
		private async void HookNotifySubscribe()
		{
			GattCharacteristicProperties properties = ble_CharacteristicSubscribe.CharacteristicProperties;
			//特征支持Nodify或者Indicate属性
			if (properties.HasFlag(GattCharacteristicProperties.Notify) || properties.HasFlag(GattCharacteristicProperties.Indicate))
			{
				// initialize status
				GattCommunicationStatus status = GattCommunicationStatus.Unreachable;
				var cccdValue = GattClientCharacteristicConfigurationDescriptorValue.None;
				if (properties.HasFlag(GattCharacteristicProperties.Indicate))
				{
					cccdValue = GattClientCharacteristicConfigurationDescriptorValue.Indicate;
				}

				else if (properties.HasFlag(GattCharacteristicProperties.Notify))
				{
					cccdValue = GattClientCharacteristicConfigurationDescriptorValue.Notify;
				}

				try
				{
					//设置回调使能
					// BT_Code: Must write the CCCD in order for server to send indications.
					status = await ble_CharacteristicSubscribe.WriteClientCharacteristicConfigurationDescriptorAsync(cccdValue);

					if (status == GattCommunicationStatus.Success)
					{
						//设置回调函数
						ble_CharacteristicSubscribe.ValueChanged += Characteristic_ValueChanged;
						output("成功订阅了服务特征变化的通知");
					}
					else
					{
						AddDataInfoData($"0xFFF1未成功订阅服务特征变化的通知: {status}");
					}
				}
				catch (Exception ex)
				{
					// This usually happens when a device reports that it support indicate, but it actually doesn't.
					//log(ex.Message);
				}
			}
		}

		public void AddRevData(byte[] buf)
		{
			try
			{
				string str = "";
				for (int i = 0; i < buf.Length; i++)
				{
					str = str + " " + buf[i].ToString("X2");
				}
				output("蓝牙通道recv:" + str, false, true);
			}
			catch
			{
			}
		}

		byte Ble_Process_GetSum(byte[] data, int len)
		{
			byte i = 0;
			int j = 0;
			for (j = 0; j < len; j++)
			{
				i += data[j];
			}
			return i;
		}

		void Ble_Process_Less_33H(ref byte[] data, int len)
		{
			for (int i = 0; i < len; i++)
			{
				data[i] = (byte)(data[i] - 0x33);
			}
		}

		uint Ble_Process_Security_GetTagValue(byte[] data, int index)
		{
			return (uint)((data[index + 0] << 24) | (data[index + 1] << 16) | (data[index + 2] << 8) | data[index + 3]);
		}

		uint Ble_Process_Security_GetLenValue2(byte[] data, int index)
		{
			return (uint)((data[index + 0] << 8) | data[index + 1]);
		}

		int Ble_Process_Security_ModuleInfo(byte[] s_data, int s_len)
		{
			try
			{
				int index = 4;

				uint a_len = Ble_Process_Security_GetLenValue2(s_data, index);//总长度
				index = index + 2;

				uint mac_len = s_data[index];
				index = index + 1;
				if (mac_len != 6)
				{
					return -1;
				}
				AddDataInfoData("收到MAC地址：" + s_data[index + 5].ToString("X2") + "-" + s_data[index + 4].ToString("X2") + "-" + s_data[index + 3].ToString("X2") + "-" + s_data[index + 2].ToString("X2") + "-" + s_data[index + 1].ToString("X2") + "-" + s_data[index].ToString("X2"));
				index = index + 6;

				int version_len = s_data[index];
				index = index + 1;

				byte[] bb = new byte[version_len];
				Array.Copy(s_data, index, bb, 0, version_len);
				string version_str = System.Text.Encoding.UTF8.GetString(bb);
				AddDataInfoData("软件版本:" + version_str);
				index = index + version_len;

				int chip_len = s_data[index];
				index = index + 1;

				AddDataInfoData("芯片序列号:" + s_data[index].ToString("X2") + s_data[index + 1].ToString("X2") + s_data[index + 2].ToString("X2") + s_data[index + 3].ToString("X2") + s_data[index + 4].ToString("X2") + s_data[index + 5].ToString("X2") + s_data[index + 6].ToString("X2") + s_data[index + 7].ToString("X2"));
				index = index + 8;

				int name_len = s_data[index];
				index = index + 1;
				byte[] b_name = new byte[name_len];
				Array.Copy(s_data, index, b_name, 0, name_len);
				string name_str = System.Text.Encoding.UTF8.GetString(b_name);
				AddDataInfoData("模组名称:" + name_str);
				index = index + name_len;
			}
			catch
			{

			}
			return 0;
		}

		byte Ble_Process_Security_Chip_Auth(byte[] data, int data_len)
		{
			int ret = 0;
			if (data_len < 4)
			{
				AddDataInfoData("接收数据长度错误:" + data_len.ToString());
				return 0;
			}

			switch (Ble_Process_Security_GetTagValue(data, 0))
			{
				case 0xDFDDA026://模组信息
					ret = Ble_Process_Security_ModuleInfo(data, data_len);
					break;
				case 0xDFDDA100://返回没有认证进行了数据转发 错误
					AddDataInfoData("数据转发错误，没有进行认证!错误码：0x" + Ble_Process_Security_GetLenValue2(data, 6).ToString("X"));
					break;
				default:
					AddDataInfoData("数据解析错误！");
					break;
			}
			return 0;
		}

		private void Ble_Process_TypeParse()
		{
			byte contral_flag = ListBuffer[8];
			byte data_len = ListBuffer[9];
			if (contral_flag == 0x80)
			{
				byte[] data = new byte[data_len];
				ListBuffer.CopyTo(10, data, 0, data_len);

				Ble_Process_Less_33H(ref data, data_len);

				Ble_Process_Security_Chip_Auth(data, data_len);
			}
			else
			{

			}
		}

		public delegate void RecvDataDelegate(byte[] data);

		/// <summary>
		///  RecvDataBLE
		/// </summary>
		/// <param name="data"></param>
		public void RecvData(byte[] data)
		{
			AddRevData(data); //显示接收的数据
			if (txrb485BLE.Checked)
			{
				string str = Helps.AsciiByt2Str(data);
				output("TX:" + str, false, true);
			}

			if (txcTransshipment.Checked)
			{
				if (SpMeter.sendWithNoAsk(data, data.Length) == true)
				{
					output("TX:" + Helps.ByteToStringWithSpace(data), false, true);
				}
				else
				{
					output("发送失败!", true, true);
				}

				Thread.Sleep(data.Length / 8);
				TransshipmentMoniter = true;
				ThTestL = new Thread(new ParameterizedThreadStart(tTransshipmentMoniter));
				ThTestL.IsBackground = true;
				ThTestL.Start();
			}

			if ((ListBuffer.Count + data.Length) > (10 * 1024))
			{
				ListBuffer.Clear();
			}

			lock (rev_obj)
			{
				if (txcbBLE.Checked)
				{
					ListBuffer.Clear();
					ListBuffer.AddRange(data);
					//Console.WriteLine(DataConvert.Bytes2HexString(data, 0, data.Length));
					while (ListBuffer.Count >= 12)
					{
						//帧校验
						if ((ListBuffer[0] == 0x68) && (ListBuffer[7] == 0x68))
						{
							int len = ListBuffer[9];
							if (ListBuffer.Count >= (len + 12)) //数据长度满足
							{
								byte[] tmp = new byte[len + 10];
								ListBuffer.CopyTo(0, tmp, 0, len + 10);
								byte cs = Ble_Process_GetSum(tmp, len + 10);

								if ((ListBuffer[len + 11] == 0x16) && (ListBuffer[len + 10] == cs))
								{
									Ble_Process_TypeParse(); //帧处理
								}
								else
								{
									AddDataInfoData("帧校验错误");
								}

								lock (rev_obj)
								{
									ListBuffer.RemoveRange(0, len + 12);
								}
							}
							else
							{
								break; //继续接收
							}
						}
						else
						{
							lock (rev_obj)
							{
								ListBuffer.RemoveAt(0);
							}
						}
					}
				}
				else
				{
					ListBuffer.AddRange(data);
				}
			}
		}
		private void Characteristic_ValueChanged(GattCharacteristic sender, GattValueChangedEventArgs args)
		{
			// BT_Code: An Indicate or Notify reported that the value has changed.
			// Display the new value with a timestamp.
			GattCharacteristic BLE_Characteristicubscribe = ble_CharacteristicSubscribe;

			GattCharacteristicProperties properties = BLE_Characteristicubscribe.CharacteristicProperties;
			if (properties.HasFlag(GattCharacteristicProperties.Indicate))
			{
				AddDataInfoData("收到Indicate通知");
			}
			if (properties.HasFlag(GattCharacteristicProperties.Notify))
			{

			}
			if (BLE_Characteristicubscribe == null) return;
			this.Invoke(new Action(() =>
			{
				IBuffer result = args.CharacteristicValue;
				try
				{
					//byte[] data;
					RecBuf = null;
					//CryptographicBuffer.CopyToByteArray(result, out data);
					CryptographicBuffer.CopyToByteArray(result, out RecBuf);
					//Console.WriteLine(DataConvert.Bytes2HexString(RecBuf, 0, RecBuf.Length));
					//用委托进行数据传出
					RecvDataDelegate nc1 = new RecvDataDelegate(RecvData);
					//nc1(data);
					nc1(RecBuf);
				}
				catch
				{

				}
			}));
		}

		private async void getData_fromCharacteristic_0xFFF1()
		{
			AddDataInfoData("正在读取特征的数据类别 0xFFF1");

			if (ble_Characteristic_0xFFF1 != null)
			{
				//解除上一个设备的回调
				UnhookNotify();
				try
				{
					// Get all the child descriptors of a characteristics. 
					var result = await ble_Characteristic_0xFFF1.GetDescriptorsAsync(BluetoothCacheMode.Uncached);
					if (result.Status != GattCommunicationStatus.Success)
					{
						AddDataInfoData("Descriptor read failure: " + result.Status.ToString());
					}
					// BT_Code: There's no need to access presentation format unless there's at least one. 
					presentationFormat = null;
					if (ble_Characteristic_0xFFF1.PresentationFormats.Count > 0)
					{
						if (ble_Characteristic_0xFFF1.PresentationFormats.Count.Equals(1))
						{
							// Get the presentation format since there's only one way of presenting it
							presentationFormat = ble_Characteristic_0xFFF1.PresentationFormats[0];
						}
						else
						{
							// It's difficult to figure out how to split up a characteristic and encode its different parts properly.
							// In this case, we'll just encode the whole thing to a string to make it easy to print out.
						}
					}
					AddDataInfoData("0xFFF1特征类别获取成功");
					GattCharacteristicProperties properties = ble_Characteristic_0xFFF1.CharacteristicProperties;
					if (properties.HasFlag(GattCharacteristicProperties.Notify) == false)
					{
						AddDataInfoData("0xFFF1特征类别内容错误");
						return;
					}
					HookNotify();
				}
				catch (Exception ee)
				{
					AddDataInfoData("设备暂时不可达");
				}
			}
		}

		private async void getData_fromCharacteristic_0xFFF2()
		{
			AddDataInfoData("正在读取特征的数据类别 0xFFF2");

			if (ble_Characteristic_0xFFF2 != null)
			{
				//解除上一个设备的回调
				//UnhookNotify();
				try
				{
					// Get all the child descriptors of a characteristics. 
					var result = await ble_Characteristic_0xFFF2.GetDescriptorsAsync(BluetoothCacheMode.Uncached);
					if (result.Status != GattCommunicationStatus.Success)
					{
						AddDataInfoData("Descriptor read failure: " + result.Status.ToString());
					}
					// BT_Code: There's no need to access presentation format unless there's at least one. 
					presentationFormat = null;
					if (ble_Characteristic_0xFFF2.PresentationFormats.Count > 0)
					{
						if (ble_Characteristic_0xFFF2.PresentationFormats.Count.Equals(1))
						{
							// Get the presentation format since there's only one way of presenting it
							presentationFormat = ble_Characteristic_0xFFF2.PresentationFormats[0];
						}
						else
						{
							// It's difficult to figure out how to split up a characteristic and encode its different parts properly.
							// In this case, we'll just encode the whole thing to a string to make it easy to print out.
						}
					}
					AddDataInfoData("0xFFF2特征类别获取成功");
					GattCharacteristicProperties properties = ble_Characteristic_0xFFF2.CharacteristicProperties;
					if (properties.HasFlag(GattCharacteristicProperties.Write) == false)
					{
						AddDataInfoData("0xFFF2特征类别内容错误");
						return;
					}
				}
				catch (Exception ee)
				{
					AddDataInfoData("设备暂时不可达");
				}
			}
		}

		private async void getData_fromCharacteristicSubscribe()
		{
			AddDataInfoData("正在读取特征的数据类别" + iCharacteristicSubscribeBLE.ToString("X4"));

			if (ble_CharacteristicSubscribe != null)
			{
				//解除上一个设备的回调
				UnhookNotifySubscribe();
				try
				{
					// Get all the child descriptors of a characteristics. 
					//var result = await ble_CharacteristicSubscribe.GetDescriptorsAsync(BluetoothCacheMode.Uncached);
					var result = await ble_CharacteristicSubscribe.GetDescriptorsAsync();
					if (result.Status != GattCommunicationStatus.Success)
					{
						AddDataInfoData("Descriptor read failure: " + result.Status.ToString());
					}
					// BT_Code: There's no need to access presentation format unless there's at least one. 
					presentationFormat = null;
					if (ble_CharacteristicSubscribe.PresentationFormats.Count > 0)
					{
						if (ble_CharacteristicSubscribe.PresentationFormats.Count.Equals(1))
						{
							// Get the presentation format since there's only one way of presenting it
							presentationFormat = ble_CharacteristicSubscribe.PresentationFormats[0];
						}
						else
						{
							// It's difficult to figure out how to split up a characteristic and encode its different parts properly.
							// In this case, we'll just encode the whole thing to a string to make it easy to print out.
						}
					}
					AddDataInfoData(iCharacteristicSubscribeBLE.ToString("X4") + "特征类别获取成功");
					GattCharacteristicProperties properties = ble_CharacteristicSubscribe.CharacteristicProperties;
					if (properties.HasFlag(GattCharacteristicProperties.Notify) == false)
					{
						AddDataInfoData(iCharacteristicSubscribeBLE.ToString("X4") + "特征类别内容错误");
						return;
					}
					HookNotifySubscribe();
				}
				catch (Exception ee)
				{
					AddDataInfoData("设备暂时不可达");
				}
			}
		}

		private async void getData_fromCharacteristicWrite()
		{
			AddDataInfoData("正在读取特征的数据类别" + iCharacteristicWriteBLE.ToString("X4"));

			if (ble_CharacteristicWrite != null)
			{
				//解除上一个设备的回调
				//UnhookNotify();
				try
				{
					// Get all the child descriptors of a characteristics. 
					//var result = await ble_CharacteristicWrite.GetDescriptorsAsync(BluetoothCacheMode.Uncached);
					var result = await ble_CharacteristicWrite.GetDescriptorsAsync();
					if (result.Status != GattCommunicationStatus.Success)
					{
						AddDataInfoData("Descriptor read failure: " + result.Status.ToString());
					}
					// BT_Code: There's no need to access presentation format unless there's at least one. 
					presentationFormat = null;
					if (ble_CharacteristicWrite.PresentationFormats.Count > 0)
					{
						if (ble_CharacteristicWrite.PresentationFormats.Count.Equals(1))
						{
							// Get the presentation format since there's only one way of presenting it
							presentationFormat = ble_CharacteristicWrite.PresentationFormats[0];
						}
						else
						{
							// It's difficult to figure out how to split up a characteristic and encode its different parts properly.
							// In this case, we'll just encode the whole thing to a string to make it easy to print out.
						}
					}
					AddDataInfoData(iCharacteristicWriteBLE.ToString("X4") + "特征类别获取成功");
					GattCharacteristicProperties properties = ble_CharacteristicWrite.CharacteristicProperties;
					//if (properties.HasFlag(GattCharacteristicProperties.Write) == false || properties.HasFlag(GattCharacteristicProperties.WriteWithoutResponse) == false)
					if (properties.HasFlag(GattCharacteristicProperties.WriteWithoutResponse) == false)
					{
						AddDataInfoData(iCharacteristicWriteBLE.ToString("X4") + "特征类别内容错误");
						return;
					}
				}
				catch (Exception ee)
				{
					AddDataInfoData("设备暂时不可达");
				}
			}
		}

		private async void getCharacteristic()
		{
			AddDataInfoData("获取当前特征值");
			ble_Characteristics = null;
			IReadOnlyList<GattCharacteristic> characteristics = null;
			try
			{
				var accessStatus = await ble_Service.RequestAccessAsync();
				if (accessStatus == DeviceAccessStatus.Allowed)
				{
					//var result = await ble_Service.GetCharacteristicsAsync(BluetoothCacheMode.Cached);
					//var result = await ble_Service.GetCharacteristicsAsync(BluetoothCacheMode.Uncached);
					var result = await ble_Service.GetCharacteristicsAsync();
					if (result.Status == GattCommunicationStatus.Success)
					{
						characteristics = result.Characteristics;
						AddDataInfoData(String.Format("找到 {0} 个特征", characteristics.Count));
						//if (characteristics.Count != 2)
						//{
						//    AddDataInfoData("特征个数为" + characteristics.Count.ToString() + " 错误！");
						//    btnDisconnected_Click(null, null);//断开连接
						//    return;
						//}

						for (int i = 0; i < characteristics.Count; i++)
						{
							ushort characteristicname = BLE_Info.GetCharacteristicName_INT(characteristics[i]);
							if ((characteristicname != iCharacteristicSubscribeBLE) && (characteristicname != iCharacteristicWriteBLE))
							{
								//AddDataInfoData("特征值" + characteristicname.ToString("X") + " 错误！");
								ble_Characteristics = null;
								//btnDisconnected_Click(null, null);//断开连接
								continue;
							}
							AddDataInfoData("特征值" + characteristicname.ToString("X4"));
							if (characteristicname == iCharacteristicSubscribeBLE)//接收数据 订阅
							{
								last_ble_CharacteristicSubscribe = ble_CharacteristicSubscribe;
								ble_CharacteristicSubscribe = characteristics[i];
								if (characteristics.Count == 1)
								{
									ble_CharacteristicWrite = characteristics[i];
								}
							}
							else if (characteristicname == iCharacteristicWriteBLE)//发送数据
							{
								ble_CharacteristicWrite = characteristics[i];
							}
						}

						ble_Characteristics = characteristics;

						getData_fromCharacteristicSubscribe();
						getData_fromCharacteristicWrite();
					}
					else
					{

					}
				}
				else
				{
					// Not granted access
					AddDataInfoData("服务访问失败");
				}
			}
			catch (Exception ex)
			{
				AddDataInfoData("服务特征禁止操作");
				//log("服务特征禁止操作: " + ex.Message);
			}
		}

		/// <summary>
		/// 
		/// </summary>
		private async void getService()
		{
			GattDeviceServicesResult result;
			ble_Service = null;
			try
			{
				//result = await ble_Device.GetGattServicesAsync(BluetoothCacheMode.Cached);//返回具有指定缓存模式的此蓝牙 LowEnergy 设备的 GattDeviceServices。
				result = await ble_Device.GetGattServicesAsync();//返回具有指定缓存模式的此蓝牙 LowEnergy 设备的 GattDeviceServices。
																 //result = await ble_Device.GetGattServicesAsync(BluetoothCacheMode.Uncached);//返回具有指定缓存模式的此蓝牙 LowEnergy 设备的 GattDeviceServices。
				ble_Device.GetGattServicesAsync().Completed = async (AsyncInfo, AsyncStatus) =>
				{
					if (AsyncStatus == AsyncStatus.Completed)
					{
						var Services = AsyncInfo.GetResults().Services;
						if (Services.Count > 0)
						{
							foreach (var Gatt in Services)
							{

							}
						}
					}
				};
			}
			catch (Exception ex)
			{
				AddDataInfoData("获取服务失败!");
				//log("获取服务失败!" + ex.Message, Color.Red);
				btnDisconnected_Click(null, null);//断开连接
				return;
			}
			if (result.Status == GattCommunicationStatus.Success)
			{
				var services = result.Services;
				output(String.Format("找到 {0} 个服务", services.Count));
				ble_Services = services;

				if (services.Count == 0)
				{
					AddDataInfoData("服务个数为0!");
					btnDisconnected_Click(null, null);//断开连接
					return;
				}
				ble_Service = null;
				foreach (var service in services)
				{
					ushort sname = BLE_Info.GetServiceName_INT(service);

					if (sname == iServiceBLE)//找到需要的服务了
					{
						ble_Service = service;
						//AddDataInfoData("通信服务0xFFF0存在");
						AddDataInfoData("通信服务" + iServiceBLE.ToString("X4") + "存在");
						getCharacteristic();//获取特征值
					}
				}
			}
			else
			{
				AddDataInfoData("设备不可访问!");
				btnDisconnected_Click(null, null);//断开连接
			}
		}

		private async void GetDeviceServices()
		{
			output("设备连接...");

			BluetoothLEDevice bledevice = null;
			try
			{
				if (lv_device.Items.Count <= 0 || lv_device.SelectedItems.Count <= 0) return;

				string id = lv_device.SelectedItems[0].Name;
				string name = lv_device.SelectedItems[0].Text;
				if (Dev_ID_Str == id)
				{
					AddDataInfoData("设备 " + name + " 已经在连接状态！");
					return;
				}
				Dev_ID_Str = lv_device.SelectedItems[0].Name;
				Dev_Name_Str = lv_device.SelectedItems[0].Text;

				if (id != null)
				{
					bledevice = await BluetoothLEDevice.FromIdAsync(id);
					if (bledevice == null)
					{
						AddDataInfoData("设备连接失败");
					}
				}
			}
			catch (Exception ex) when (ex.HResult == E_DEVICE_NOT_AVAILABLE)
			{
				AddDataInfoData("蓝牙设备不可用");
				Dev_ID_Str = "";
			}
			if (bledevice != null)
			{
				ble_Device = bledevice;
				ble_Device.ConnectionStatusChanged += CurrentDevice_ConnectionStatusChanged;
				AddDataInfoData("设备连接成功");
				System.Threading.Thread.Sleep(100);
				getService();//获取服务
			}
		}

		private void lv_device_DoubleClick(object sender, EventArgs e)
		{
			btnDisconnected_Click(null, null);
			if (ble_Device != null)
			{
				ble_Device.ConnectionStatusChanged -= CurrentDevice_ConnectionStatusChanged;
			}
			GetDeviceServices();
		}

		private void StopBleDeviceWatcher()
		{
			if (deviceWatcher != null)
			{
				deviceWatcher.Stop();
				// Unregister the event handlers.
				deviceWatcher.Added -= DeviceWatcher_Added;
				deviceWatcher.Updated -= DeviceWatcher_Updated;
				deviceWatcher.Removed -= DeviceWatcher_Removed;
				deviceWatcher.EnumerationCompleted -= DeviceWatcher_EnumerationCompleted;
				deviceWatcher.Stopped -= DeviceWatcher_Stopped;
				deviceWatcher = null;
			}
		}

		private void btnStop_Click(object sender, EventArgs e)
		{
			StopBleDeviceWatcher();
		}

		private void btnDisconnected_Click(object sender, EventArgs e)
		{
			try
			{
				Dev_ID_Str = "";
				//UnhookNotify();
				UnhookNotifySubscribe();
				try
				{
					if (ble_CharacteristicSubscribe != null)
					{
						if (ble_CharacteristicSubscribe.Service.Session.SessionStatus == GattSessionStatus.Active)
						{
							ble_CharacteristicSubscribe.Service.Dispose();
						}
						ble_CharacteristicSubscribe = null;
					}
				}
				catch
				{

				}

				try
				{
					if (ble_CharacteristicWrite != null)
					{
						if (ble_CharacteristicWrite.Service.Session.SessionStatus == GattSessionStatus.Active)
						{
							ble_CharacteristicWrite.Service.Dispose();
						}
						ble_CharacteristicWrite = null;
					}
				}
				catch
				{

				}

				try
				{
					if (ble_CharacteristicSubscribe != null)
					{
						if (ble_CharacteristicSubscribe.Service.Session.SessionStatus == GattSessionStatus.Active)
						{
							ble_CharacteristicSubscribe.Service.Dispose();
						}
						ble_CharacteristicSubscribe = null;
					}
				}
				catch
				{

				}

				try
				{
					if (ble_Service != null)
					{
						if (ble_Service.Session.SessionStatus == GattSessionStatus.Active)
						{
							ble_Service.Dispose();
						}
						ble_Service = null;
					}
				}
				catch
				{

				}

				if (ble_Device != null)
				{
					ble_Device.ConnectionStatusChanged -= CurrentDevice_ConnectionStatusChanged;
					ble_Device.Dispose();
					ble_Device = null;
				}
				//log("设备断链", Color.Red);
			}
			catch (Exception ex)
			{
				//log("设备断链错误！", Color.Red);
			}
		}

		void Ble_Process_MakeFrameHead_645()
		{
			SendPointer = 0;
			SendBuf[SendPointer++] = 0x68;     //启动1
			SendBuf[SendPointer++] = 0;        //帧长1
			SendBuf[SendPointer++] = 0;        //帧长2
			SendBuf[SendPointer++] = 0;
			SendBuf[SendPointer++] = 0;
			SendBuf[SendPointer++] = 0;
			SendBuf[SendPointer++] = 0;
			SendBuf[SendPointer++] = 0x68;     //启动
			SendBuf[SendPointer++] = 0x00;
			SendBuf[SendPointer++] = 0;
		}

		private async Task<bool> WriteBufferToCharacteristicAsync(IBuffer buffer)
		{
			try
			{
				GattCharacteristic ble_CharacteristicWriteBuffer = ble_CharacteristicWrite;
				if (ble_CharacteristicWriteBuffer == null)
				{
					AddDataInfoData("写特征不存在");
					return false;
				}
				// BT_Code: Writes the value from the buffer to the characteristic.

				if (iCharacteristicWriteBLE == 0x0000ffe1)
				{
					var Status = await ble_CharacteristicWriteBuffer.WriteValueAsync(buffer, GattWriteOption.WriteWithoutResponse);
					if (Status == GattCommunicationStatus.Success)
					{
						await Task.Delay(TimeSpan.FromSeconds(3));
						return true;
					}
					else
					{
						AddDataInfoData($"写入失败: {Status}");
						return false;
					}
				}
				else
				{
					var result = await ble_CharacteristicWriteBuffer.WriteValueWithResultAsync(buffer);
					if (result.Status == GattCommunicationStatus.Success)
					{
						await Task.Delay(TimeSpan.FromSeconds(3));
						return true;
					}
					else
					{
						AddDataInfoData($"写入失败: {result.Status}");
						return false;
					}
				}
			}
			catch (Exception ex) when (ex.HResult == E_BLUETOOTH_ATT_INVALID_PDU)
			{
				AddDataInfoData("系统错误 " + ex.Message);
				return false;
			}
			catch (Exception ex) when (ex.HResult == E_BLUETOOTH_ATT_WRITE_NOT_PERMITTED || ex.HResult == E_ACCESSDENIED)
			{
				// This usually happens when a device reports that it support writing, but it actually doesn't.
				AddDataInfoData("系统错误 " + ex.Message);
				return false;
			}
		}

		public void AddSendData(byte[] buf, int len)
		{
			try
			{
				string str = "";
				for (int i = 0; i < len; i++)
				{
					str = str + " " + buf[i].ToString("X2");
				}
				output("蓝牙通道send:" + str, false, true);
			}
			catch
			{
			}
		}

		/// <summary>
		/// 蓝牙发送
		/// </summary>
		private async void SendData()
		{
			try
			{
				if (SendPointer > 0)
				{
					AddSendData(SendBuf, SendPointer);
					await WriteBufferToCharacteristicAsync(WindowsRuntimeBufferExtensions.AsBuffer(SendBuf, 0, SendPointer));
					SendPointer = 0;
				}
			}
			catch
			{

			}
		}

		/// <summary>
		/// 蓝牙发送
		/// </summary>
		private async Task<bool> SendDataBLE(byte[] SendBuf)
		{
			try
			{
				RecBuf = null;
				return await WriteBufferToCharacteristicAsync(WindowsRuntimeBufferExtensions.AsBuffer(SendBuf, 0, SendBuf.Length));
			}
			catch
			{
				return false;
			}
		}

		void Ble_Process_Add_33H(ref byte[] data, int len)
		{
			for (int i = 0; i < len; i++)
			{
				data[i] = (byte)(data[i] + 0x33);
			}
		}

		void Ble_Process_MakeFrameEnd_645()
		{
			byte len = (byte)((SendPointer - 10) & 0xFF);//预留部分为0
			SendBuf[9] = len;

			byte[] tmp = new byte[len];
			Array.Copy(SendBuf, 10, tmp, 0, len);
			Ble_Process_Add_33H(ref tmp, len);
			Array.Copy(tmp, 0, SendBuf, 10, len);
			byte cs = Ble_Process_GetSum(SendBuf, SendPointer);
			SendBuf[SendPointer++] = cs;
			SendBuf[SendPointer++] = 0x16;
			SendData();
		}

		private void btnModuleInfo_Click(object sender, EventArgs e)
		{
			try
			{
				Ble_Process_MakeFrameHead_645();
				SendBuf[SendPointer++] = (byte)((0xDFDDA025 >> 24) & 0xff);
				SendBuf[SendPointer++] = (byte)((0xDFDDA025 >> 16) & 0xff);
				SendBuf[SendPointer++] = (byte)((0xDFDDA025 >> 8) & 0xff);
				SendBuf[SendPointer++] = (byte)(0xDFDDA025 & 0xff);
				Ble_Process_MakeFrameEnd_645();
			}
			catch
			{

			}
		}

		private void btnSendTest_Click(object sender, EventArgs e)
		{
			byte[] TxBuftest = null;
			string strDataSend = "68" + Helps.fnAgainst(pmc[6].Value.ToString()) + "6811043436D337";
			//string strDataSend = "68" + Helps.fnAgainst(pmc[6].Value.ToString()) + "6800041210D35E" ;
			Helps.TryStrToHexByte(strDataSend, out TxBuftest);

			#region 校验和+16
			byte sum = Helps.Ssum(TxBuftest);
			byte[] endBytes = new[] { sum, (byte)0x16 };
			#endregion

			TxBuftest = TxBuftest.Concat(endBytes).ToArray();
			output("蓝牙TX:" + Helps.ByteToStringWithSpace(TxBuftest), false, true);

			RecBuf = null;
			#region Thread
			Thread td = new Thread(new ParameterizedThreadStart(tTestBLE));
			td.IsBackground = true;
			td.Start(TxBuftest);
			#endregion
			//output("蓝牙RX:" + Helps.ByteToStringWithSpace(RecBuf), false, true);
		}

		/// <summary>
		/// 
		/// </summary>
		private void tTestBLE(Object TxBufGetVersion)
		{
			try
			{
				var vBool = SendDataBLE((byte[])TxBufGetVersion);
				int iCount = 0;
				while (RecBuf == null)
				{
					iCount++;
					if (iCount > SpMeter.OverTime / 21)
					{
						break;
					}
					Thread.Sleep(20);
				}
				//output("蓝牙RX:" + Helps.ByteToStringWithSpace(RecBuf), false, true);

			}
			catch (Exception e)
			{
				Console.WriteLine(e);
			}
		}

		/// <summary>
		/// ReadBLE
		/// </summary>
		private void tReadBLE(Object TxBufGetVersion)
		{
			try
			{
				DataRow[] drs = dt_RW.Select(string.Format("选择 = '{0}' ", "true"), "");
				int rowCount = drs.GetLength(0);
				if (rowCount < 1)
				{
					return;
				}

				#region 抄读
				bool isReadingErr = false;
				DLT645 dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
				for (int i = 0; i < rowCount; i++)
				{
					List<string> dataMarkList = new List<string>();
					byte seq = 0;
					string dataResultStr = "";

					if (drs[i]["DataMark"] == null || drs[i]["DataMark"].ToString().Length != 8)
					{
						drs[i]["读取值"] = "";
						drs[i]["操作结论"] = "抄读失败：数据标识非法！";
						continue;
					}

					string dataMark = drs[i]["DataMark"].ToString();
					dataMarkList.Add(dataMark);
					if (drs[i]["DataMarkFather"] != null && drs[i]["DataMarkFather"].ToString().Length == 8)
					{
						dataMarkList.Add(drs[i]["DataMarkFather"].ToString());
					}

					byte[] sendBytes = null;
					string errStr = "";
					if (!txcEncrypt.Checked)
					{
						if (!dlt645.Pack(DLT645.ControlType.Read, dataMarkList, "", 0, out sendBytes, out errStr))
						{
							drs[i]["读取值"] = "";
							drs[i]["操作结论"] = errStr;
							continue;
						}
					}
					else
					{
						if (!dlt645.Pack(DLT645.ControlType.SafetyReadLTU, dataMarkList, "", 0, out sendBytes, out errStr))
						{
							drs[i]["读取值"] = "";
							drs[i]["操作结论"] = errStr;
							continue;
						}
					}

					string strAPDU = "";
					byte[] baAPDU;
					byte[] baTemp;
					byte[] recBytes;
					if (cb698.Checked)
					{
						strAPDU = "090700f2090201060208010000640064" + sendBytes.Length.ToString("X2");
						Helps.TryStrToHexByte(strAPDU, out baAPDU);
						baTemp = new byte[sendBytes.Length];
						baTemp = sendBytes;
						sendBytes = new byte[baTemp.Length + baAPDU.Length + 1];
						baAPDU.CopyTo(sendBytes, 0);
						baTemp.CopyTo(sendBytes, baAPDU.Length);
						baTemp = new byte[1] { 00 };
						baTemp.CopyTo(sendBytes, sendBytes.Length - 1);

						strAPDU = "68" + Helps.fnAgainst((15 + sendBytes.Length).ToString("X4")) + "4345" + Helps.fnAgainst("AAAAAAAAAAAA") + "00";
						Helps.TryStrToHexByte(strAPDU, out baAPDU);
						ushort uCountFCS16 = WSDFCS16.CountFCS16(baAPDU, 1, baAPDU.Length - 1);
						baTemp = new byte[2];
						Helps.TryStrToHexByte(uCountFCS16.ToString("X4"), out baTemp);
						baAPDU = baAPDU.Concat(baTemp).ToArray();
						baAPDU = baAPDU.Concat(sendBytes).ToArray();

						uCountFCS16 = WSDFCS16.CountFCS16(baAPDU, 1, baAPDU.Length - 1);
						baTemp = new byte[2];
						Helps.TryStrToHexByte(uCountFCS16.ToString("X4"), out baTemp);
						baAPDU = baAPDU.Concat(baTemp).ToArray();
						sendBytes = new byte[baAPDU.Length + 1];
						baAPDU.CopyTo(sendBytes, 0);
						baTemp = new byte[1] { 0x16 };
						baTemp.CopyTo(sendBytes, sendBytes.Length - 1);
						output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
						recBytes = SpMeter.SendCommand(sendBytes, "698Pass");
					}
					else
					{
						//string strTemp = "232330303837514E3D32303233313231313135333534393938353B53543D34343B434E3D313031313B50573D3132333435363B4D4E3D3132333435363738313233343536373831323334353637383B466C61673D353B43503D26262626313243300D0A";
						//Helps.TryStrToHexByte(strTemp, out sendBytes);
						output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
						RecBuf = null;
						var vBool = SendDataBLE(sendBytes);
						int iCount = 0;
						while (RecBuf == null)
						{
							iCount++;
							if (iCount > SpMeter.OverTime / 31)
							{
								break;
							}
							Thread.Sleep(30);
						}

						if (RecBuf == null)
						{
							return;
						}
						recBytes = new byte[RecBuf.Length];
						Array.Copy(RecBuf, 0, recBytes, 0, RecBuf.Length);
					}

					//output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
					bool isHaveHouxu = false;
					do
					{
						string dataPianDuan;
						DLT645.ControlType controlType = DLT645.ControlType.Read;
						if (txcEncrypt.Checked)
						{
							controlType = DLT645.ControlType.SafetyReadLTU;
						}

						if (isHaveHouxu)
						{
							controlType = DLT645.ControlType.ReadFollow;
						}

						if (!dlt645.UnPack(recBytes, controlType, dataMarkList, seq, out isHaveHouxu, out dataPianDuan, out errStr))
						{
							drs[i]["读取值"] = "";
							drs[i]["操作结论"] = errStr;
							isReadingErr = true;
							break;
						}

						if (!string.IsNullOrEmpty(drs[i]["DataBlockType"].ToString()))
						{
							dataPianDuan = Helps.fnAgainst(dataPianDuan);
						}

						dataResultStr += dataPianDuan.Trim();
						if (isHaveHouxu)
						{
							seq++;
							if (!dlt645.Pack(DLT645.ControlType.ReadFollow, dataMarkList, "", seq, out sendBytes, out errStr))
							{
								drs[i]["读取值"] = "";
								drs[i]["操作结论"] = errStr;
								isReadingErr = true;
								break;
							}

							output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
							RecBuf = null;
							var vBool = SendDataBLE(sendBytes);
							int iCount = 0;
							while (RecBuf == null)
							{
								iCount++;
								if (iCount > SpMeter.OverTime / 21)
								{
									break;
								}
								Thread.Sleep(20);
							}

							if (RecBuf == null)
							{
								return;
							}
							recBytes = new byte[RecBuf.Length];
							Array.Copy(RecBuf, 0, recBytes, 0, RecBuf.Length);
							//recBytes = SpMeter.SendCommand(sendBytes, "645");
							output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
						}
					} while (isHaveHouxu);

					if (isReadingErr)
					{
						isReadingErr = false;
						continue;
					}

					int dataLen;
					if (drs[i]["DataLen"].ToString().Contains("*"))
					{
						GetData(drs[i], dataResultStr, dataResultStr.Length);
					}
					else
					{
						if (!int.TryParse(drs[i]["DataLen"].ToString(), out dataLen))
						{
							drs[i]["读取值"] = "";
							drs[i]["操作结论"] = "数据库中该数据项长度非法，请排查数据库信息！";
							continue;
						}

						if (dataResultStr.Length != dataLen * 2)
						{
							drs[i]["读取值"] = "";
							drs[i]["操作结论"] = "失败：返回数据长度非法";
							continue;
						}
						GetData(drs[i], dataResultStr, dataLen);
					}
					Thread.Sleep(int.Parse(pmc[11].Value.ToString()));
				}
				#endregion
			}
			catch (Exception exception)
			{
				output("抄读过程中出现错误：" + exception.Message, true, true);
			}
			finally
			{
				//if (SpMeter.Working)
				//{
				//	SpMeter.Working = false;
				//}
			}

			//try
			//{
			//	var vBool = SendDataBLE((byte[])TxBufGetVersion);
			//	while (RecBuf == null)
			//	{
			//		Thread.Sleep(20);
			//	}
			//	output("蓝牙RX:" + Helps.ByteToStringWithSpace(RecBuf), false, true);

			//}
			//catch (Exception e)
			//{
			//	Console.WriteLine(e);
			//}
		}

		/// <summary>
		/// SetBLE
		/// </summary>
		private void tSetBLE(Object TxBufGetVersion)
		{
			try
			{
				DataRow[] drs = dt_RW.Select(string.Format("选择 = '{0}' ", "true"), "");
				int rowCount = drs.GetLength(0);
				if (rowCount < 1)
				{
					return;
				}
				SpMeter.Working = true;

				#region 设置
				DLT645 dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
				for (int i = 0; i < rowCount && SpMeter.Working; i++)
				{
					List<string> dataMarkList = new List<string>();
					dataMarkList.Clear();
					var dataResultStr = "";
					byte[] sendBytes = null;
					string errStr = "";
					string dataStr;
					string strRandom = "";
					if (txcEncrypt.Checked)
					{
						string strGetRandom = m_SCUProxy.HY_SendToSecModuleRandom("DFDDA010");
						dlt645 = new DLT645(Helps.fnAgainst(txtxbTXAddr.Text.ToString()));
						if (!_dlt645.Pack(DLT645.ControlType.SafetyCertificationLTU, lstrDataMark, strGetRandom, 0, out baSendBytes, out string strErr))
						{

						}
						output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
						byte[] recByte = SpMeter.SendCommand(baSendBytes, "645");
						output("RX:" + Helps.ByteToStringWithSpace(recByte), false, true);

						if (dlt645.UnPack(recByte, DLT645.ControlType.SafetyCertificationLTU, dataMarkList, 0, out bool isHave, out string strDataField, out errStr))
						{
							strRandom = strDataField.Substring(strDataField.Length - 8, 8);
							output("随机数:" + strRandom, false, true);
							strRandom = Helps.fnAgainst(strRandom);
							//drs[i]["操作结论"] = "设置成功";
						}
						else
						{
							drs[i]["操作结论"] = "设置失败：" + errStr;
						}
					}
					if (drs[i]["DataMark"] == null || drs[i]["DataMark"].ToString().Length != 8)
					{
						drs[i]["读取值"] = "";
						drs[i]["操作结论"] = "设置失败：数据标识非法！";
						continue;
					}

					string dataMark = drs[i]["DataMark"].ToString();
					dataMarkList.Add(dataMark);
					if (drs[i]["DataMarkFather"] != null && drs[i]["DataMarkFather"].ToString().Length == 8)
					{
						dataMarkList.Add(drs[i]["DataMarkFather"].ToString());
					}

					if (!GetSetDataStr(drs[i], drs[i]["设置值"].ToString(), out dataStr, out errStr))
					{
						drs[i]["操作结论"] = "设置失败：" + errStr;
						continue;
					}
					DLT645.ControlType ctSeType = DLT645.ControlType.Write;

					//if (txcbSetControl.Text == "1C")
					//{
					//	ctSeType = DLT645.ControlType.WriteSwitch;
					//}
					//else 
					if (txcEncrypt.Checked)
					{
						ctSeType = DLT645.ControlType.SafetyWriteLTU;
					}
					dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
					if (!dlt645.Pack(ctSeType, dataMarkList, strRandom + dataStr, 0, out sendBytes, out errStr, pmc[7].Value.ToString(), pmc[8].Value.ToString(), pmc[9].Value.ToString()))
					{
						drs[i]["操作结论"] = errStr;
						continue;
					}

					output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
					RecBuf = null;
					var vBool = SendDataBLE(sendBytes);
					int iCount = 0;
					while (RecBuf == null)
					{
						iCount++;
						if (iCount > SpMeter.OverTime / 21)
						{
							break;
						}
						Thread.Sleep(20);
					}

					if (RecBuf == null)
					{
						return;
					}
					byte[] recBytes = new byte[RecBuf.Length];
					Array.Copy(RecBuf, 0, recBytes, 0, RecBuf.Length);
					//output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
					string dataPianDuan;
					bool isHaveHouxu;

					if (dlt645.UnPack(recBytes, ctSeType, dataMarkList, 0, out isHaveHouxu, out dataPianDuan, out errStr))
					{
						drs[i]["操作结论"] = "设置成功";
					}
					else
					{
						drs[i]["操作结论"] = "设置失败：" + errStr;
					}
					Thread.Sleep(int.Parse(pmc[11].Value.ToString()));
				}
				#endregion
			}
			catch (Exception exception)
			{
				output("设置过程中出现错误：" + exception.Message, true, true);
			}
			finally
			{
				if (SpMeter.Working)
				{
					SpMeter.Working = false;
				}
			}
		}

		private void txbModuleInfo_Click(object sender, EventArgs e)
		{
			byte[] obaSend = null;
			FnGroupMessagesBLE("DFDDA025", out obaSend);
			if (obaSend == null)
			{
				MessageBox.Show("组包文失败!");
				return;
			}
			output("TX:" + Helps.ByteToStringWithSpace(obaSend), false, true);
			byte[] recBytes = SpMeter.SendCommand(obaSend, "645");
			if (recBytes != null)
			{
				if (recBytes[8] == 0xC0)
				{
					MessageBox.Show("模组返回异常!");
					return;
				}
				output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
				byte[] baData = new byte[recBytes[9] - 3];
				Array.Copy(recBytes, 13, baData, 0, baData.Length);
				Ble_Process_Less_33H(ref baData, baData.Length);
				Ble_Process_Security_ModuleInfo(baData, baData.Length);
			}
		}

		private bool FnGroupMessagesBLE(string strTag, out byte[] obaBytes)
		{
			obaBytes = null;
			bool bResult = false;
			string strDataSend = "68" + Helps.fnAgainst(pmc[6].Value.ToString()) + "6800" + (strTag.Length / 2 + 3).ToString("X2") + DataConvert.fnEncodingData("1B" + (strTag.Length / 2).ToString("X4") + strTag, false);
			Helps.TryStrToHexByte(strDataSend, out obaBytes);

			#region 校验和+16
			byte sum = Helps.Ssum(obaBytes);
			byte[] endBytes = new[] { sum, (byte)0x16 };
			#endregion

			obaBytes = obaBytes.Concat(endBytes).ToArray();
			return bResult;
		}

		private void txbOpenBLE_Click(object sender, EventArgs e)
		{
			byte[] obaSend = null;
			FnGroupMessagesBLE("DFDDA02B", out obaSend);
			if (obaSend == null)
			{
				MessageBox.Show("组包文失败!");
				return;
			}
			output("TX:" + Helps.ByteToStringWithSpace(obaSend), false, true);
			byte[] recBytes = SpMeter.SendCommand(obaSend, "645");
			if (recBytes != null)
			{
				if (recBytes[8] == 0xC0)
				{
					MessageBox.Show("模组返回异常!");
					return;
				}
				output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
				if (recBytes[16] == 0x5f)
				{
					output("蓝牙开启成功!");
				}
			}
		}

		private void txbReadN58_Click(object sender, EventArgs e)
		{
			//string strTemp = "68 31 00 43 05 63 26 13 50 03 00 00 00 6C 09 07 00 F2 01 02 01 03 02 08 01 00 00 1E 00 64 10 68 45 56 64 45 56 64 68 11 04 34 33 39 38 BB 16 00 52 4A 16";
			string strTemp = "68 31 00 43 45 aa aa aa aa aa aa 00 a6 58 09 07 00 f2 01 02 01 03 02 08 01 00 00 1e 00 64 10 68 45 56 64 45 56 64 68 11 04 34 33 39 38 bb 16 00 52 4a 16";
			byte[] sendBytes = new byte[0];
			byte[] recBytes = new byte[0];
			Helps.TryStrToHexByte(strTemp, out sendBytes);
			output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
			RecBuf = null;
			var vBool = SendDataBLE(sendBytes);
			int iCount = 0;
			while (RecBuf == null)
			{
				iCount++;
				if (iCount > SpMeter.OverTime / 21)
				{
					break;
				}
				Thread.Sleep(20);
			}

			if (RecBuf == null)
			{
				return;
			}
			recBytes = new byte[RecBuf.Length];
			Array.Copy(RecBuf, 0, recBytes, 0, RecBuf.Length);
			System.Text.ASCIIEncoding asciiEncoding = new System.Text.ASCIIEncoding();
			string dataResultStr = Helps.ByteToStringWithSpace(RecBuf);
			dataResultStr = Helps.ASCII2Str(dataResultStr);
			output("TX:" + dataResultStr, false, true);
		}
		#endregion

		private void txbCloseBLE_Click(object sender, EventArgs e)
		{
			byte[] obaSend = null;
			FnGroupMessagesBLE("DFDDA02D", out obaSend);
			if (obaSend == null)
			{
				MessageBox.Show("组包文失败!");
				return;
			}
			output("TX:" + Helps.ByteToStringWithSpace(obaSend), false, true);
			byte[] recBytes = SpMeter.SendCommand(obaSend, "645");
			if (recBytes != null)
			{
				if (recBytes[8] == 0xC0)
				{
					MessageBox.Show("模组返回异常!");
					return;
				}
				output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
				if (recBytes[16] == 0x61)
				{
					output("蓝牙关闭成功!");
				}
			}
		}

		private void txbSetNameBLE_Click(object sender, EventArgs e)
		{
			byte[] obaSend = null;
			string strData = "";

			System.Text.ASCIIEncoding asciiEncoding = new System.Text.ASCIIEncoding();
			byte[] asciiByts = Helps.Str2AsciibBytes(txtbSetNameBLE.Text.ToString().PadRight(txtbSetNameBLE.Text.Length, asciiEncoding.GetChars(new byte[] { 0 })[0]));
			strData = SerialTool.byteToHexStr(asciiByts);
			FnGroupMessagesBLE("DFDDA029" + (strData.Length / 2).ToString("X4") + strData, out obaSend);
			if (obaSend == null)
			{
				MessageBox.Show("组包文失败!");
				return;
			}
			output("TX:" + Helps.ByteToStringWithSpace(obaSend), false, true);
			byte[] recBytes = SpMeter.SendCommand(obaSend, "645");
			if (recBytes != null)
			{
				output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
				if (recBytes[8] == 0xC0)
				{
					output("设置模组名称:" + txtbSetNameBLE.Text.ToString() + " 失败！");
					return;
				}
				else
				{
					output("设置模组名称:" + txtbSetNameBLE.Text.ToString() + " 成功！");
				}
			}
		}

		private void txbGetAuthenticationStatus_Click(object sender, EventArgs e)
		{
			baSendBytes = m_SCUProxy.HY_SendToProxyBuf(0x01, 0x13, 0x0E);
			output("TX:" + Helps.ByteToStringWithSpace(baSendBytes), false, true);
			byte[] baRecBytes = SpWuCha.SendCommand(baSendBytes, "EncryptSCU");
			if (baRecBytes == null)
			{
				MessageBox.Show("返回报文为空!");
				return;
			}
			output("RX:" + Helps.ByteToStringWithSpace(baRecBytes), false, true);
			if (baRecBytes[7] == 0)
			{
				output("未认证!", true);
			}
			else
			{
				output("已认证!");
			}

		}

		private void rb_CustomTime_CheckedChanged(object sender, EventArgs e)
		{
			timerSystem.Enabled = false;
		}

		private void rb_SysTime_CheckedChanged(object sender, EventArgs e)
		{
			timerSystem.Enabled = true;
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
		#endregion

		private void timerSystem_Tick(object sender, EventArgs e)
		{
			string strTmp = DateTime.Now.ToString("yy-MM-dd") + " " + DateTime.Now.ToString("HH:mm:ss");
			SetSysTime(strTmp);
		}

		private void txrbSwitch_CheckedChanged(object sender, EventArgs e)
		{
			iServiceBLE = 0x0001;
			iCharacteristicSubscribeBLE = 0x0003;
			iCharacteristicWriteBLE = 0x0002;
			btnSendTest.Visible = false;
			btnModuleInfo.Visible = false;
			groupBox9.Visible = false;
			txbReadN58.Visible = false;
			gb485BLE.Visible = false;
		}

		private void txrbLTU2_CheckedChanged(object sender, EventArgs e)
		{
			iServiceBLE = 0xFFF0;
			iCharacteristicSubscribeBLE = 0xFFF1;
			iCharacteristicWriteBLE = 0xFFF2;

			//iServiceBLE = 0xFFE0;
			//iCharacteristicSubscribeBLE = 0xFFE2;
			//iCharacteristicWriteBLE = 0xFFE1;
			btnSendTest.Visible = true;
			btnModuleInfo.Visible = true;
			txbReadN58.Visible = false;
			groupBox9.Visible = true;
			gb485BLE.Visible = false;
		}

		private void txrbN58_CheckedChanged(object sender, EventArgs e)
		{
			iServiceBLE = 0xFFE0;
			iCharacteristicSubscribeBLE = 0xFFE2;
			iCharacteristicWriteBLE = 0xFFE1;
			btnSendTest.Visible = false;
			btnModuleInfo.Visible = false;
			groupBox9.Visible = false;
			txbReadN58.Visible = true;
			gb485BLE.Visible = false;
		}

		private void txrbLTU1_CheckedChanged(object sender, EventArgs e)
		{
			//iServiceBLE = 0x6287;
			//iCharacteristicSubscribeBLE = 0x6487;
			//iCharacteristicWriteBLE = 0x6387;
			iServiceBLE = 0xFFE0;

			//iCharacteristicSubscribeBLE = 0xFFE2;
			//LTU
			iCharacteristicSubscribeBLE = 0xFFE1;
			iCharacteristicWriteBLE = 0xFFE1;
			btnSendTest.Visible = false;
			btnModuleInfo.Visible = false;
			groupBox9.Visible = false;
			txbReadN58.Visible = false;
			gb485BLE.Visible = false;
		}
		private void txrb485BLE_CheckedChanged(object sender, EventArgs e)
		{
			iServiceBLE = 0xEEF3;
			iCharacteristicSubscribeBLE = 0xE0F5;
			iCharacteristicWriteBLE = 0xEFF4;
			//iServiceBLE = 0x0001;
			//iCharacteristicSubscribeBLE = 0x0003;
			//iCharacteristicWriteBLE = 0x0002;
			btnSendTest.Visible = false;
			btnModuleInfo.Visible = false;
			groupBox9.Visible = false;
			txbReadN58.Visible = false;
			txcTransshipment.Visible = false;
			gb485BLE.Visible = true;
		}
		private void btnInputF460Info_Click(object sender, EventArgs e)
		{
			if (tbFilePathInfo.Text == "")
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
			if (openFileDialog1.ShowDialog() != DialogResult.OK)
			{
				return;
			}

			tbFilePathInfo.Text = openFileDialog1.FileName;
			//if (!(tbFilePathInfo.Text.ToString().Contains("F460")))
			//{
			//	MessageBox.Show("导入的文件不包含F460,请检查固件是否正确!");
			//	tbFilePathInfo.Text = "";
			//	return;
			//}

			// 创建一个文件流对象，用于打开指定文件
			FileStream fs = new FileStream(openFileDialog1.FileName, FileMode.Open, FileAccess.Read);
			// 创建一个二进制读取器对象，用于从文件流中读取数据
			BinaryReader read = new BinaryReader(fs);


			try
			{
				iPartSize = Int32.Parse(tbByteCounts.Text.ToString());
				int index = 0;
				lbaHexBytes.Clear();
				int iAllCount = 0, iDefenderPartCount = 0, iAddLength = 0;
				if ((txcbMeterType.SelectedIndex == 2))
				{
					#region LTU升级信息
					byte[] baInfo = DataConvert.HEXString2Bytes(rtbBinInfo.Text.ToString());
					//List<byte> lbData = new List<byte>();


					//lbData.AddRange(baInfo);
					//for (int i = 0; i < iPartSize - (baInfo.Length % iPartSize); i++)
					//{
					//	lbData.Add(0xFF);
					//}
					//baInfo = new byte[iPartSize];
					//baInfo = lbData.ToArray();
					//lbaHexBytes.Add(baInfo);
					#endregion

					while (fs.Length > index)
					{
						if (index == 0)
						{
							int lenght = iPartSize - baInfo.Length;
							byte[] buff = read.ReadBytes(lenght);
							//if (buff.Length % iPartSize != 0)
							{
								List<byte> listData = new List<byte>();
								listData.AddRange(baInfo);
								listData.AddRange(buff);
								buff = new byte[iPartSize];
								buff = listData.ToArray();
							}
							lbaHexBytes.Add(buff);
							index += lenght;
						}
						else
						{
							int lenght = (int)fs.Length - index < iPartSize ? (int)(fs.Length - index) : iPartSize;
							byte[] buff = read.ReadBytes(lenght);
							if (buff.Length % iPartSize != 0)
							{
								iAddLength = iPartSize - (buff.Length % iPartSize);
								List<byte> listData = new List<byte>();

								listData.AddRange(buff);
								for (int i = 0; i < iPartSize - (buff.Length % iPartSize); i++)
								{
									listData.Add(0xFF);
								}
								buff = new byte[iPartSize];
								buff = listData.ToArray();
							}
							lbaHexBytes.Add(buff);
							index += lenght;
						}
					}
				}
				else
				{
					#region LTU升级信息
					byte[] baInfo = DataConvert.HEXString2Bytes(rtbBinInfo.Text.ToString());
					List<byte> lbData = new List<byte>();
					//int iAllCount = 0, iDefenderPartCount = 0, iAddLength = 0;

					lbData.AddRange(baInfo);
					for (int i = 0; i < iPartSize - (baInfo.Length % iPartSize); i++)
					{
						lbData.Add(0xFF);
					}
					baInfo = new byte[iPartSize];
					baInfo = lbData.ToArray();
					lbaHexBytes.Add(baInfo);
					if (cbDoubleBlock.Checked)
					{
						lbData = new List<byte>();
						for (int i = 0; i < iPartSize; i++)
						{
							lbData.Add(0xFF);
						}
						lbaHexBytes.Add(lbData.ToArray());
					}
					#endregion

					while (fs.Length > index)
					{
						int lenght = (int)fs.Length - index < iPartSize ? (int)(fs.Length - index) : iPartSize;
						byte[] buff = read.ReadBytes(lenght);
						if (buff.Length % iPartSize != 0)
						{
							iAddLength = iPartSize - (buff.Length % iPartSize);
							List<byte> listData = new List<byte>();

							listData.AddRange(buff);
							for (int i = 0; i < iPartSize - (buff.Length % iPartSize); i++)
							{
								listData.Add(0xFF);
							}
							buff = new byte[iPartSize];
							buff = listData.ToArray();
						}
						lbaHexBytes.Add(buff);
						index += lenght;
					}
				}
				//释放Bin文件
				fs.Close();
				read.Close();

				#region NoDefender
				iAllCount = lbaHexBytes.Count * iPartSize;
				iDefenderPartCount = (8192 + iAddLength) / iPartSize;
				if ((8192 + iAddLength) % iPartSize != 0)
				{
					iDefenderPartCount += 1;
				}
				//if ( (!cbAllUpgrade.Checked) && (lbaHexBytes.Count > 2459))
				if ((!cbAllUpgrade.Checked))
				{
					lbaHexBytes.RemoveRange((lbaHexBytes.Count - iDefenderPartCount), iDefenderPartCount);
					while (lbaHexBytes[lbaHexBytes.Count - 1].SequenceEqual(lbaHexBytes[1]))
					{
						lbaHexBytes.RemoveAt(lbaHexBytes.Count - 1);
					}
				}
				#endregion

				#region Save
				if (Helps.ByteToFile(lbaHexBytes, tbFilePathInfo.Text.Replace(".bin", "UpGrade.bin")))
				{
					MessageBox.Show("导出成功!");
				}
				else
				{
					MessageBox.Show("导出失败!");
				}
				#endregion
			}
			catch (Exception e1)
			{
				//output("导入升级程序有误：" + e1.Message, true, true);
			}
		}

		private void txBOutputBin_Click(object sender, EventArgs e)
		{

		}

		#region CAN
		public bool fnCAN_AT(string strData, string strCMD = "CAN_FRAMEFORMAT", bool bFlag = false)
		{
			if (!stCAN.isOpen())
			{
				stCAN.Working = false;
				return false;
			}
			bool bSuccess = true;
			//stCAN.
			output("+++", false, true);
			Thread.Sleep(100);
			byte[] baRec = stCAN.SendCommand("+++");
			if (baRec == null)
			{
				Thread.Sleep(10);
				return false;
			}
			output(DataConvert.Byte2String(baRec, 0, baRec.Length), false, true);
			//string strText = "AT+" + strCMD + "=" + (SG != null ? SG() : (string) null) + "\r\n";
			string strText = "AT+" + strCMD + "=" + strData + "\r\n";
			if (bFlag)
			{
				strText = "AT+" + strCMD + "=?\r\n";
			}

			output(strText, false, true);
			baRec = stCAN.SendCommand(strText);

			if (baRec == null)
			{
				return false;
			}
			output(DataConvert.Byte2String(baRec, 0, baRec.Length), false, true);
			output("ATO", false, true);
			baRec = stCAN.SendCommand("ATO\r\n");
			if (baRec == null)
			{
				Thread.Sleep(10);
				return false;
			}
			output(DataConvert.Byte2String(baRec, 0, baRec.Length), false, true);

			return bSuccess;
		}

		private void txbInputCan_Click(object sender, EventArgs e)
		{
			OpenFileDialog openFileDialog = new OpenFileDialog();
			openFileDialog.InitialDirectory = ".\\";
			openFileDialog.Filter = "可导入文件类型|*.xlsx;*.xls";
			openFileDialog.FilterIndex = 0;
			openFileDialog.RestoreDirectory = false;
			string errStr = "";
			dtCAN = new DataTable(null);
			dtCAN.Columns.Add("选择", typeof(bool));
			if (openFileDialog.ShowDialog() == DialogResult.OK)
			{
				if (!AsposeExcelTools.ExcelSheetToDataTable(openFileDialog.FileName, "CAN", 0, 0, out dtCAN, out errStr))
				{
					MessageBox.Show("导入CAN方案失败：" + errStr);
					return;
				}
				dgvCAN.DataSource = dtCAN;
			}
			for (int i = 0; i < dtCAN.Rows.Count; i++)
			{
				DataGridViewCell aa = dgvCAN.Rows[i].Cells[8];
				aa.Style.BackColor = Color.LightCyan;
			}
		}

		string fnSplitCAN(string strDataIn)
		{
			string strTemp = "";
			string strDataOut = "";
			int iLength = ((strDataIn.Length / 2) - 2) / 7;
			if ((((strDataIn.Length / 2) - 2) % 7) != 0)
			{
				iLength += 1;
			}
			string strSum = Helps.funSum((iLength + 1).ToString("X2") + Helps.fnAgainst((strDataIn.Length / 2).ToString("X4")) + strDataIn);

			strTemp = strDataIn.Substring(0, 8);

			strDataOut = "01" + (iLength + 1).ToString("X2") + Helps.fnAgainst((strDataIn.Length / 2).ToString("X4")) + strTemp;
			strDataIn = strDataIn.Substring(8);
			strDataIn = (strDataIn + strSum).PadRight(16 * iLength, '0');
			for (int i = 2; i < (iLength + 2); i++)
			{
				strDataOut += i.ToString("X2") + strDataIn.Substring(0, 14);
				strDataIn = strDataIn.Substring(14);
			}
			//strTemp = (iLength + 2).ToString("X2") + strDataIn.Substring(0, strDataIn.Length);

			return strDataOut;
		}

		/// <summary>
		/// 
		/// </summary>
		private void tTestCAN()
		{
			try
			{
				string strLastGPN = "";
				string strLastSA = "";
				string strLastPS = "";
				while (stCAN.isOpen())
				{
					for (int i = 0; i < dtCAN.Rows.Count; i++)
					{
						if (dtCAN.Rows[i][0].ToString() == null || dtCAN.Rows[i][0].ToString() == "" || dtCAN.Rows[i][0].ToString() == "False")
						{
							continue;
						}
						string strGPN = dtCAN.Rows[i]["PF"].ToString();
						string strPS = dtCAN.Rows[i]["PS"].ToString();
						string strSA = dtCAN.Rows[i]["SA"].ToString();
						if ((strGPN != strLastGPN) || strSA != strLastSA || strPS != strLastPS)
						{
							string strDataIn = "1,1,0," + int.Parse((strGPN + strPS + strSA), NumberStyles.HexNumber).ToString();
							Thread.Sleep(5);
							if (!fnCAN_AT(strDataIn))
							{
								FnStopTestCANHandler();
								return;
							}

							if (!stCAN.isOpen())
							{
								return;
							}
							output("PF:" + strGPN + ";PS:" + strPS + ";SA:" + strSA);
							strLastGPN = strGPN;
							strLastSA = strSA;
							strLastSA = strSA;
						}
						string strData = dtCAN.Rows[i]["数据值"].ToString();
						if (strData.Length > 16)
						{
							//string strOut = fnSplitCAN(strData);
							string strOut = (strData);
							int iMax = strOut.Length / 16;
							for (int j = 0; j < iMax; j++)
							{
								Thread.Sleep(50);
								string strSend = strOut.Substring(0, 16);
								output(strSend, false, true);
								stCAN.sendWithNoAsk(DataConvert.HEXString2Bytes(strSend), DataConvert.HEXString2Bytes(strSend).Length);
								strOut = strOut.Substring(16);
							}
						}
						else
						{
							strData = strData.PadRight(16, '0');
							output(strData, false, true);
							stCAN.sendWithNoAsk(DataConvert.HEXString2Bytes(strData), DataConvert.HEXString2Bytes(strData).Length);
						}
					}

					if (!txcbLoop.Checked)
					{
						stCAN.Working = false;
						stCAN.closePort();
						FnStopTestCANHandler();
					}
				}
			}
			catch (Exception e)
			{
				Console.WriteLine(e);
			}
		}

		#region 停止测试
		public delegate void dStopTestCAN();
		dStopTestCAN dstcStopTestCAN;

		public void FnStopTestCANHandler()
		{
			try
			{
				if (dstcStopTestCAN == null)
				{
					dstcStopTestCAN = new dStopTestCAN(StopTestCAN);
				}
				Invoke(dstcStopTestCAN);
			}
			catch
			{

			}
		}

		/// <summary>
		/// btnTest
		/// </summary>
		/// <param name="objIndex"></param>
		public void StopTestCAN()
		{
			lock (m_objectShow)
			{
				txbTestCAN.Text = "开始测试";
				stCAN.Working = false;
			}
		}
		#endregion

		private void txbTestCAN_Click(object sender, EventArgs e)
		{
			try
			{
				stCAN.setSerialPort(txcbCANSerial.Text.ToString(), 9600, "None", 8, "1", 50);
				if (txbTestCAN.Text == "开始测试")
				{
					if (!int.TryParse(pmc[5].Value.ToString(), out int overTime))
					{
						MessageBox.Show("非法的等待超时时间！");
						return;
					}
					stCAN.OverTime = overTime;
					if (dtCAN == null)
					{
						MessageBox.Show("请先导入CAN方案");
						return;
					}
					drs = dtCAN.Select(string.Format("选择 = '{0}' ", "true"), "");
					stCAN.Working = true;
					txbTestCAN.Text = "停止测试";
					#region Thread
					Thread td = new Thread(new ThreadStart(tTestCAN));
					td.IsBackground = true;
					td.Start();
					#endregion
				}
				else
				{
					stCAN.Working = false;
					stCAN.closePort();
					txbTestCAN.Text = "开始测试";
				}
			}
			catch (Exception exception)
			{
				output("设置过程中出现错误：" + exception.Message, true, true);
				stCAN.Working = false;
				bTeleindicationTest = false;
				txbTestCAN.Text = "开始测试";
			}
			finally
			{

			}
		}


		private void txcbCANSerial_DropDown(object sender, EventArgs e)
		{
			int i = 0;
			this.txcbCANSerial.Items.Clear(); // 清除串口
			string[] gCOM = System.IO.Ports.SerialPort.GetPortNames(); // 获取设备的所有可用串口
			int j = gCOM.Length; // 得到所有可用串口数目
			for (i = 0; i < j; i++)
			{
				this.txcbCANSerial.Items.Add(gCOM[i]); // 依次添加到下拉框中
			}
		}

		private void 全选ToolStripMenuItem_Click(object sender, EventArgs e)
		{
			if (txTabControl1.SelectedTab.Name == "tpCAN")
			{
				dgvCAN.EndEdit();
				for (int i = 0; i < this.dgvCAN.Rows.Count; i++)
				{
					DataGridViewCheckBoxCell checkBox = (DataGridViewCheckBoxCell)this.dgvCAN.Rows[i].Cells[0];
					checkBox.Value = true; //设置为0 取消全选
				}
			}
			else if (txtcReadDatas.SelectedTab.Name == "tpCommonRead")
			{
				dgvCommon.EndEdit();
				string type = dgvCommon.Tag as string;
				if (!string.IsNullOrEmpty(type))
				{
					DataTable dt = AllReadDataTables[type].Copy();
					if (dt != null)
					{
						for (int i = 0; i < dt.Rows.Count; i++)
						{
							dt.Rows[i]["选择"] = true;
						}
						dgvCommon.DataSource = null;
						dgvCommon.DataSource = dt;
						dgvCommon.Tag = type; // 保持tag一致
					}
				}
			}
		}

		private void 取消全选ToolStripMenuItem_Click(object sender, EventArgs e)
		{
			if (txTabControl1.SelectedTab.Name == "tpCAN")
			{
				dgvCAN.EndEdit();
				for (int i = 0; i < this.dgvCAN.Rows.Count; i++)
				{
					DataGridViewCheckBoxCell checkBox = (DataGridViewCheckBoxCell)this.dgvCAN.Rows[i].Cells[0];
					checkBox.Value = false; //设置为0 取消全选
				}
			}
			else if (txtcReadDatas.SelectedTab.Name == "tpCommonRead")
			{
				dgvCommon.EndEdit();
				for (int i = 0; i < this.dgvCommon.Rows.Count; i++)
				{
					DataGridViewCheckBoxCell checkBox = (DataGridViewCheckBoxCell)this.dgvCommon.Rows[i].Cells[0];
					checkBox.Value = false; //设置为0 取消全选
				}
			}
		}

		private void SelectRowsByColumnContainsChar(string colHeader, char containChar)
		{
			dgvCommon.EndEdit();

			// 查找列索引
			int nameColIndex = -1;
			foreach (DataGridViewColumn col in dgvCommon.Columns)
			{
				if (col.HeaderText.Trim() == colHeader)
				{
					nameColIndex = col.Index;
					break;
				}
			}

			if (nameColIndex == -1)
			{
				MessageBox.Show("未找到列：" + colHeader);
				return;
			}
			foreach (DataGridViewRow row in dgvCommon.Rows)
			{
				if (row.IsNewRow) continue;
				var chkCell = row.Cells[0] as DataGridViewCheckBoxCell;
				object value = row.Cells[nameColIndex].Value;
				bool contains = false;

				if (value != null)
				{
					string text = value.ToString();
					// 忽略大小写搜索
					if (text.IndexOf(containChar.ToString(), StringComparison.OrdinalIgnoreCase) >= 0)
					{
						// 若已勾选，则反选；未勾选则勾选
						chkCell = row.Cells[0] as DataGridViewCheckBoxCell;
						if (chkCell != null)
						{
							bool checkedNow = chkCell.Value is bool b && b;
							contains = !checkedNow;
						}
						else
						{
                            contains = true;
                        }
                    }
                }


                if (chkCell != null)
                {
                    chkCell.Value = contains;
                }
            }
			// 删除不匹配的行
            for (int j = dgvCommon.Rows.Count - 1; j >= 0; j--)
            {
                DataGridViewRow row = dgvCommon.Rows[j];
                object value = row.Cells[nameColIndex].Value;
                string text = value.ToString();
                if (text.IndexOf(containChar.ToString(), StringComparison.OrdinalIgnoreCase) < 0)
                {
                    dgvCommon.Rows.RemoveAt(j);
                }
            }
            dgvCommon.Refresh();
		}

		private void 全选_取消选AtoolStripMenuItem_Click(object sender, EventArgs e)
		{
			SelectRowsByColumnContainsChar("数据项名称", 'a');
		}
		private void 全选_取消选BtoolStripMenuItem_Click(object sender, EventArgs e)
		{
			SelectRowsByColumnContainsChar("数据项名称", 'b');
		}
		private void 全选_取消选CtoolStripMenuItem_Click(object sender, EventArgs e)
		{
			SelectRowsByColumnContainsChar("数据项名称", 'c');
		}


		private void dgvCAN_CellMouseDoubleClick(object sender, DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex == -1)
			{
				return;
			}
			else if (e.ColumnIndex == -1)
			{
				return;
			}
			string dataDisplay = "";
			string FP = "";
			if (dgvCAN.Columns[e.ColumnIndex].DataPropertyName == "数据值")
			{
				if (dtCAN.Rows[e.RowIndex]["数据值"] == null)
				{
					return;
				}

				dataDisplay = dtCAN.Rows[e.RowIndex]["数据值"].ToString();
				FP = dtCAN.Rows[e.RowIndex]["PF"].ToString();
			}

			DataTable dtDisplay = new DataTable();

			{
				dtDisplay = DataBank.Find(x => x.TableName == "CAN-" + FP);
				if (dtDisplay == null)
				{
					MessageBox.Show("未查找到 Sheet" + "CAN-" + FP);
					return;
				}
				dtDisplay = DataBank.Find(x => x.TableName == "CAN-" + FP).Copy();
			}
			Form_DisplayDataBlock displayDataBlock = new Form_DisplayDataBlock(dtDisplay, dataDisplay) { StartPosition = FormStartPosition.CenterScreen };
			if (displayDataBlock.ShowDialog() == DialogResult.OK)
			{
				dtCAN.Rows[e.RowIndex]["数据值"] = Helps.fnAgainst(Form_DisplayDataBlock.setDataStr);
				dgvCAN.Refresh();
				displayDataBlock.Dispose();
			}
		}

		private void txbInputMockCAN_Click(object sender, EventArgs e)
		{
			OpenFileDialog openFileDialog = new OpenFileDialog();
			openFileDialog.InitialDirectory = ".\\";
			openFileDialog.Filter = "可导入文件类型|*.log;*.LOG";
			openFileDialog.FilterIndex = 0;
			openFileDialog.RestoreDirectory = false;
			string errStr = "";
			dtMockCAN = new DataTable(null);
			dtMockCAN.Columns.Add("选择", typeof(bool));
			dtMockCAN.Columns.Add("DataName", typeof(string));
			dtMockCAN.Columns.Add("ID", typeof(string));
			dtMockCAN.Columns.Add("PF", typeof(string));
			dtMockCAN.Columns.Add("PS", typeof(string));
			dtMockCAN.Columns.Add("SA", typeof(string));
			dtMockCAN.Columns.Add("DataLen", typeof(string));
			dtMockCAN.Columns.Add("DataFormat", typeof(string));
			dtMockCAN.Columns.Add("数据值", typeof(string));
			dtMockCAN.Columns.Add("操作结论", typeof(string));
			DataRow drDataRow = dtMockCAN.NewRow();
			if (openFileDialog.ShowDialog() == DialogResult.OK)
			{
				string strLine = "";
				string[] straLine;
				try
				{
					//Pass the file path and file name to the StreamReader constructor Encoding gb2312
					StreamReader sr = new StreamReader(openFileDialog.FileName, Encoding.GetEncoding("gb2312"));
					//Read the first line of text
					strLine = sr.ReadLine();
					//Continue to read until you reach end of file
					while (strLine != null)
					{
						drDataRow = dtMockCAN.NewRow();
						if (strLine == "")
						{
							//Read the next line
							strLine = sr.ReadLine();
						}
						//write the line to console window
						//Console.WriteLine(strLine);
						straLine = strLine.Split(' ');
						drDataRow["DataName"] = straLine[0];
						drDataRow["ID"] = straLine[2];
						drDataRow["PF"] = straLine[2].Substring(2, 2);
						drDataRow["PS"] = straLine[2].Substring(4, 2);
						drDataRow["SA"] = straLine[2].Substring(6, 2);
						drDataRow["数据值"] = straLine[3];
						dtMockCAN.Rows.Add(drDataRow);
						//Read the next line
						strLine = sr.ReadLine();
					}
					//close the file
					sr.Close();
					Console.ReadLine();
				}
				catch (Exception ee)
				{
					Console.WriteLine("Exception: " + ee.Message);
				}
				finally
				{
					Console.WriteLine("Executing finally block.");
				}

				//排序加删除重复项
				dtMockCAN.DefaultView.Sort = "ID";
				dtMockCAN = dtMockCAN.DefaultView.ToTable(true);//, "ID", "数据值" 筛选指定列
				DgvMockCAN.DataSource = dtMockCAN;
			}
		}

		private void OutOutCAN_Click(object sender, EventArgs e)
		{
			SaveFileDialog saveFileDialog1 = new SaveFileDialog();
			saveFileDialog1.Filter = "Excel文件（*.xlsx）|*.xlsx|Excel文件（*.xls）|*.xls";
			saveFileDialog1.FilterIndex = 0;
			saveFileDialog1.RestoreDirectory = false;
			string errStr = "";
			if (saveFileDialog1.ShowDialog() == DialogResult.OK)
			{
				List<DataTable> dts = new List<DataTable>();
				dts.Add(dtMockCAN);

				if (!AsposeExcelTools.DataTableListToExcel(dts, saveFileDialog1.FileName, new string[] { "CAN" }, out errStr))
				{
					MessageBox.Show("导出CAN失败：" + errStr);
					return;
				}
			}
		}

		private void txtbSeachCAN_Click(object sender, EventArgs e)
		{
			if (string.IsNullOrEmpty(tbSeachCAN.Text.Trim()))
				return;
			for (int i = 0; i < dtCAN.Rows.Count; i++)
			{
				DataGridViewCell aa = dgvCAN.Rows[i].Cells[8];
				aa.Style.BackColor = Color.LightCyan;
			}
			string searchStr = tbSeachCAN.Text.Trim();
			DataRow[] dr = dtCAN.Select((string.Format("'数据值' like '%{0}%'", searchStr)));
			//DataTable dt = Helps.GetNewDataTable(dtCAN, (string.Format( "'数据值' like '%{0}%'", searchStr)));
			for (int i = 0; i < dtCAN.Rows.Count; i++)
			{
				if (dtCAN.Rows[i]["数据值"].ToString() == searchStr)
				{
					DataGridViewCell aa = dgvCAN.Rows[i].Cells[8];
					aa.Style.BackColor = Color.Green;
				}
			}
		}
		#endregion

		private void dgv_RW_CellMouseClick(object sender, DataGridViewCellMouseEventArgs e)
		{
			if (e.Button == MouseButtons.Right)
			{
				ContextMenuStrip cc = new ContextMenuStrip();
				cc.Items.Add("全选");
				cc.Items.Add("全不选");
				//cc.Items.Add("全部删除");
				//cc.Items.Add("删除选中");
				//cc.Items.Add("向上");
				//cc.Items.Add("向下");
				//cc.Items.Add("置顶");
				//cc.Items.Add("置尾");

				cc.Items[0].Tag = 0;
				cc.Items[1].Tag = 1;
				//cc.Items[2].Tag = 2;
				//cc.Items[3].Tag = 3;
				//cc.Items[4].Tag = 4;
				//cc.Items[5].Tag = 5;
				//cc.Items[6].Tag = 6;
				//cc.Items[7].Tag = 7;
				cc.ItemClicked += new ToolStripItemClickedEventHandler(cc_ItemClicked);
				cc.Show(Control.MousePosition);
			}
		}

		void cc_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
		{
			try
			{
				bool ret = false;
				if ((int)e.ClickedItem.Tag == 0)
				{//quanxuan
					ret = true;
				}
				else if ((int)e.ClickedItem.Tag == 1)
				{//quanbuxuan
					ret = false;
				}
				//else if ((int)e.ClickedItem.Tag == 2)
				//{//deleteAll
				//    dataGridView1.Rows.Clear();
				//    return;
				//}
				//else if ((int)e.ClickedItem.Tag == 3)
				//{//deleteAchecked
				//    int tempcheck = 0;
				//    while (true)
				//    {
				//        //if ((bool)dataGridView1.Rows[tempcheck].Cells[0].Value)
				//        //if (Convert.ToBoolean(dataGridView1.Rows[tempcheck].Cells[0].Value))
				//        if ((bool)dataGridView1.Rows[tempcheck].Cells[0].EditedFormattedValue)
				//        {
				//            #region  删除集合对应的数据
				//            while (true)
				//            {
				//                if (tempcheck + 1 < dataGridView1.Rows.Count)
				//                {
				//                    if (dataGridView1.Rows[tempcheck + 1].Cells[0].Tag == null || !(dataGridView1.Rows[tempcheck + 1].Cells[0].Tag.ToString() == "FreezeeList" || dataGridView1.Rows[tempcheck + 1].Cells[0].Tag.ToString() == "NormalList"))
				//                    {
				//                        break;
				//                    }
				//                    else
				//                    {
				//                        dataGridView1.Rows.RemoveAt(tempcheck + 1);
				//                    }
				//                }
				//                else
				//                {
				//                    break;
				//                }
				//            }
				//            #endregion
				//            dataGridView1.Rows.RemoveAt(tempcheck);
				//        }
				//        else
				//        {
				//            tempcheck++;
				//        }
				//        if (tempcheck == dataGridView1.Rows.Count)
				//        {
				//            break;
				//        }
				//    }
				//    return;
				//}
				//else if ((int)e.ClickedItem.Tag == 4)
				//{//向上
				//    UpDataGridView(dataGridView1);
				//    return;
				//}
				//else if ((int)e.ClickedItem.Tag == 5)
				//{//向下
				//    DownDataGridView(dataGridView1);
				//    return;
				//}
				//else if ((int)e.ClickedItem.Tag == 6)
				//{//置顶
				//    TopDataGridView(dataGridView1);
				//    return;
				//}
				//else if ((int)e.ClickedItem.Tag == 7)
				//{//置底
				//    BottomDataGridView(dataGridView1);
				//    return;
				//}
				//else
				//{
				//    return;
				//}
				for (int i = 0; i < dgv_RW.Rows.Count; i++)
				{
					dgv_RW.Rows[i].Cells[0].Value = ret;
				}
			}
			catch
			{

			}
		}

		#region RailFunction
		private void tbRailFunction_Click(object sender, EventArgs e)
		{
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}
			byte[] TxBufFactoryMode = new byte[32];
			string strDataSend = "68" + Helps.fnAgainst(pmc[6].Value.ToString()) + "6814150E121237" + (DataConvert.fnEncodingData(pmc[8].Value.ToString() + pmc[7].Value.ToString())) + "373635343435120EBBBBBBBB34";
			Helps.TryStrToHexByte(strDataSend, out TxBufFactoryMode);

			#region 校验和+16
			byte sum = Helps.Ssum(TxBufFactoryMode);
			byte[] endBytes = new[] { sum, (byte)0x16 };
			#endregion

			TxBufFactoryMode = TxBufFactoryMode.Concat(endBytes).ToArray();

			if (sendOK(TxBufFactoryMode, 0x94))
			{
				output(" 打开编程模式成功", false, true);
			}
			else
			{
				output(" 打开编程模式失败!", true, true);
			}

			Thread.Sleep(10);

			byte[] baBytes = null;
			string strDateTime = DateTime.Now.ToString("yyMMdd") + ((int)DateTime.Now.DayOfWeek).ToString("X2") + DateTime.Now.ToString("HHmmss");
			lstrDataMark.Clear();
			lstrDataMark.Add("0400010C");
			_dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
			if (!_dlt645.Pack(DLT645.ControlType.Write, lstrDataMark, strDateTime, 0, out baBytes, out string strErr))
			{
				MessageBox.Show("组包失败!");
				return;
			}

			if (sendOK(baBytes, 0x94))
			{
				output(" 校时成功", false, true);
			}
			else
			{
				output(" 校时失败!,请检查", true, true);
			}

			string strData = "";
			//lstrDataMark.Clear();
			//lstrDataMark.Add("DBDF1151");
			//lstrDataMark.Add("04DFDFDB");
			//FunDealMessage( "", DLT645.ControlType.Read, lstrDataMark, out string strData);
			//if(strData != "00")
			//{
			//	MessageBox.Show(lstrDataMark[0].ToString() + "状态字错误!");
			//}

			lstrDataMark.Clear();
			lstrDataMark.Add("DBDF1152");
			lstrDataMark.Add("04DFDFDB");
			FunDealMessage("", DLT645.ControlType.Read, lstrDataMark, out strData);
			if (strData != "00")
			{
				MessageBox.Show(lstrDataMark[0].ToString() + "状态字错误!");
			}

			if (txrbCPU.Checked)
			{
				byte[] TxBufInitial = new byte[31];
				strDataSend = "68" + Helps.fnAgainst(pmc[6].Value.ToString()) + "6814140E121237" + (DataConvert.fnEncodingData(pmc[8].Value.ToString() + pmc[7].Value.ToString())) + "373635340E3A120EBBBBBBBB";
				Helps.TryStrToHexByte(strDataSend, out TxBufInitial);

				#region 校验和+16
				sum = Helps.Ssum(TxBufInitial);
				endBytes = new[] { sum, (byte)0x16 };
				#endregion

				TxBufInitial = TxBufInitial.Concat(endBytes).ToArray();

				if (sendOK(TxBufInitial, 0x94))
				{
					output("初始化电表成功", false, true);//Initial Meter Success
				}
				else
				{
					output("通信异常,请检查", false, true);//Meter reply error,Check the connection please
				}
				Thread.Sleep(12000);
			}
		}

		private void tbReadRemoteSignalingClose_Click(object sender, EventArgs e)
		{
			lstrDataMark.Clear();
			lstrDataMark.Add("DBDF1123");
			lstrDataMark.Add("04DFDFDB");
			FunDealMessage("", DLT645.ControlType.Read, lstrDataMark, out string strData);
			if (strData != "01")
			{
				MessageBox.Show(lstrDataMark[0].ToString() + "遥信状态错误!");
			}

			byte[] baTxRead698 = new byte[32];
			byte[] baAPDU = new byte[2];
			byte[] baCRC = new byte[2];
			byte[] recBytes = null;

			string strDataSend = "6817004305" + Helps.fnAgainst(pmc[6].Value.ToString()) + "A0";
			Helps.TryStrToHexByte(strDataSend, out baTxRead698);

			#region 校验和1
			ushort uCountFCS16 = WSDFCS16.CountFCS16(baTxRead698, 1, baTxRead698.Length - 1);
			Helps.TryStrToHexByte(uCountFCS16.ToString("X4"), out baCRC);
			baTxRead698 = baTxRead698.Concat(baCRC).ToArray();
			#endregion

			strDataSend = "0501004033020000";
			Helps.TryStrToHexByte(strDataSend, out baAPDU);
			baTxRead698 = baTxRead698.Concat(baAPDU).ToArray();

			#region 校验和2
			uCountFCS16 = WSDFCS16.CountFCS16(baTxRead698, 1, baTxRead698.Length - 1);
			Helps.TryStrToHexByte(uCountFCS16.ToString("X4"), out baCRC);
			baTxRead698 = baTxRead698.Concat(baCRC).ToArray();
			#endregion

			baCRC = new byte[1];
			baCRC[0] = 0x16;
			baTxRead698 = baTxRead698.Concat(baCRC).ToArray();

			output("TX:" + Helps.ByteToStringWithSpace(baTxRead698), false, true);
			recBytes = SpMeter.SendCommand(baTxRead698, "698");
			if (recBytes == null)
			{
				MessageBox.Show("返回报文为空!");
				return;
			}
			output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);

			if (recBytes.Length == 32)
			{
				MessageBox.Show("错误码:" + recBytes[26].ToString());
			}
			else if (recBytes.Length == 33 && (recBytes[27] != 1))
			{
				if (recBytes[27] == 0)
				{
					MessageBox.Show("罗氏线圈未接入");
				}
				else
				{
					MessageBox.Show("罗氏线圈类型为:" + recBytes[27].ToString());
				}
			}

		}

		private void tbReadRemoteSignalingOpen_Click(object sender, EventArgs e)
		{
			lstrDataMark.Clear();
			lstrDataMark.Add("DBDF1123");
			lstrDataMark.Add("04DFDFDB");
			FunDealMessage("", DLT645.ControlType.Read, lstrDataMark, out string strData);
			if (strData != "00")
			{
				MessageBox.Show(lstrDataMark[0].ToString() + "遥信状态错误!");
			}

			byte[] baTxRead698 = new byte[32];
			byte[] baAPDU = new byte[2];
			byte[] baCRC = new byte[2];
			byte[] recBytes = null;

			string strDataSend = "6817004305" + Helps.fnAgainst(pmc[6].Value.ToString()) + "A0";
			Helps.TryStrToHexByte(strDataSend, out baTxRead698);

			#region 校验和1
			ushort uCountFCS16 = WSDFCS16.CountFCS16(baTxRead698, 1, baTxRead698.Length - 1);
			Helps.TryStrToHexByte(uCountFCS16.ToString("X4"), out baCRC);
			baTxRead698 = baTxRead698.Concat(baCRC).ToArray();
			#endregion

			strDataSend = "0501004033020000";
			Helps.TryStrToHexByte(strDataSend, out baAPDU);
			baTxRead698 = baTxRead698.Concat(baAPDU).ToArray();

			#region 校验和2
			uCountFCS16 = WSDFCS16.CountFCS16(baTxRead698, 1, baTxRead698.Length - 1);
			Helps.TryStrToHexByte(uCountFCS16.ToString("X4"), out baCRC);
			baTxRead698 = baTxRead698.Concat(baCRC).ToArray();
			#endregion

			baCRC = new byte[1];
			baCRC[0] = 0x16;
			baTxRead698 = baTxRead698.Concat(baCRC).ToArray();

			output("TX:" + Helps.ByteToStringWithSpace(baTxRead698), false, true);
			recBytes = SpMeter.SendCommand(baTxRead698, "698");
			if (recBytes == null)
			{
				MessageBox.Show("返回报文为空!");
				return;
			}
			output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);

			if (recBytes.Length == 32)
			{
				MessageBox.Show("错误码:" + recBytes[26].ToString());
			}
			else if (recBytes.Length == 33 && recBytes[27] != 0)
			{
				MessageBox.Show("罗氏线圈识别结果为:" + recBytes[27].ToString());
			}
		}

		private void tbSetAddrs_Click(object sender, EventArgs e)
		{
			lstrDataMark.Clear();
			lstrDataMark.Add("04000401");
			string strErr = FunDealMessage((txtxbAddrS.Text.ToString()), DLT645.ControlType.Write, lstrDataMark, out string strData);
			if (strErr != "" && strErr != "收到的报文通信地址错误！")
			{
				MessageBox.Show(strErr);
			}
		}

		private void txrbMachine_CheckedChanged(object sender, EventArgs e)
		{
			tbRailFunction.Text = "开场内-校时-读故障";
			tbReadRemoteSignalingClose.Visible = false;
			tbReadRemoteSignalingOpen.Visible = false;
			tbReadESAMSerial.Visible = true;
		}

		private void txrbCPU_CheckedChanged(object sender, EventArgs e)
		{
			tbRailFunction.Text = "开场内-校时-读故障-初始化";
			tbReadRemoteSignalingClose.Visible = true;
			tbReadRemoteSignalingOpen.Visible = true;
			tbReadESAMSerial.Visible = false;
		}

		private void tbReadESAMSerial_Click(object sender, EventArgs e)
		{
			byte[] baTxRead698 = new byte[32];
			byte[] baAPDU = new byte[2];
			byte[] baCRC = new byte[2];
			byte[] recBytes = null;

			string strDataSend = "6817004305" + Helps.fnAgainst(pmc[6].Value.ToString()) + "A0";
			Helps.TryStrToHexByte(strDataSend, out baTxRead698);

			#region 校验和1
			ushort uCountFCS16 = WSDFCS16.CountFCS16(baTxRead698, 1, baTxRead698.Length - 1);
			Helps.TryStrToHexByte(uCountFCS16.ToString("X4"), out baCRC);
			baTxRead698 = baTxRead698.Concat(baCRC).ToArray();
			#endregion

			strDataSend = "050100F100020000";
			Helps.TryStrToHexByte(strDataSend, out baAPDU);
			baTxRead698 = baTxRead698.Concat(baAPDU).ToArray();

			#region 校验和2
			uCountFCS16 = WSDFCS16.CountFCS16(baTxRead698, 1, baTxRead698.Length - 1);
			Helps.TryStrToHexByte(uCountFCS16.ToString("X4"), out baCRC);
			baTxRead698 = baTxRead698.Concat(baCRC).ToArray();
			#endregion

			baCRC = new byte[1];
			baCRC[0] = 0x16;
			baTxRead698 = baTxRead698.Concat(baCRC).ToArray();

			output("TX:" + Helps.ByteToStringWithSpace(baTxRead698), false, true);
			recBytes = SpMeter.SendCommand(baTxRead698, "698");
			if (recBytes == null)
			{
				MessageBox.Show("返回报文为空!");
				return;
			}
			output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);

			if (recBytes.Length == 32)
			{
				MessageBox.Show("错误码:" + recBytes[26].ToString());
			}
			else if (recBytes.Length == 41 && recBytes[28] != 128)
			{
				MessageBox.Show("ESAM序列号错误");
			}
		}
		#endregion

		#region FuncPartUpdata

		#region 升级线程
		/// <summary>
		/// 线程
		/// </summary>
		private void FuncPartUpdata()
		{
			string strStartAddr = "";
			string strInData = "";
			string strUpGradeData = "";
			string strPartUpGradeData = "";
			string strErr = "";
			int iCountMessage = 0;
			Btn_厂内_Click(null, null);

			for (int i = 0; (i < lbaUpGrade.Count) && bPartUpGrade; i++)
			//for (int i = lbaUpGrade.Count - 1; i > 0; i--)
			{
				if (cbApp.Checked)
				{
					//AppStartAddr 32K
					strStartAddr = Helps.fnAgainst((32768 + liUpGrade[i] * 8192).ToString("X8"));
				}
				else
				{
					///boot
					strStartAddr = Helps.fnAgainst((0 + liUpGrade[i] * 8192).ToString("X8"));
				}

				iCountMessage = lbaUpGrade[i].Length / 128;
				strInData = strStartAddr + Helps.fnAgainst(iCountMessage.ToString("x4")) + "0100DBDF";
				strInData = Helps.fnAgainst(strInData);
				lstrDataMark.Clear();
				lstrDataMark.Add("DBDF0A00");
				lstrDataMark.Add("04DFDFDB");
				strErr = FunDealMessage(strInData, DLT645.ControlType.Write, lstrDataMark, out string strData);
				if (strErr != "")
				{
					output(strErr, true);
					MessageBox.Show(strErr);
					return;
				}

				lstrDataMark.Clear();
				lstrDataMark.Add("DBDF0A01");
				lstrDataMark.Add("04DFDFDB");
				strUpGradeData =
					DataConvert.fnGetStringFormat(DataConvert.Bytes2HexString(lbaUpGrade[i], 0, lbaUpGrade[i].Length));
				for (int j = 0; (j < iCountMessage) && bPartUpGrade; j++)
				{
					strPartUpGradeData = strUpGradeData.Substring(j * 256, 256);
					strInData = Helps.fnAgainst(Helps.fnAgainst((j).ToString("x4")) + strPartUpGradeData);
					strErr = FunDealMessage(strInData, DLT645.ControlType.Write, lstrDataMark, out strData);
					if (strErr != "")
					{
						output(strErr, true);
						//MessageBox.Show(strErr);
						return;
					}
				}

				lstrDataMark.Clear();
				lstrDataMark.Add("DBDF0A02");
				lstrDataMark.Add("04DFDFDB");
				strInData = Helps.fnAgainst(strStartAddr + Helps.fnAgainst(iCountMessage.ToString("x4")) + "A55A5AA5");
				strErr = FunDealMessage(strInData, DLT645.ControlType.Write, lstrDataMark, out strData);
				if (strErr != "")
				{
					output(strErr, true);
					MessageBox.Show(strErr);
					return;
				}

				Thread.Sleep(1000);
			}
		}
		#endregion


		private void txbBeforeUpGrade_Click(object sender, EventArgs e)
		{
			if (tbBeforeUpGrade.Text == "")
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
			if (openFileDialog1.ShowDialog() != DialogResult.OK)
			{
				return;
			}

			tbBeforeUpGrade.Text = openFileDialog1.FileName;

			FileStream fs = new FileStream(openFileDialog1.FileName, FileMode.Open, FileAccess.Read);
			BinaryReader read = new BinaryReader(fs);

			try
			{
				int index = 0;
				int iPartSize = 8192;
				lbaBeforeUpGrade.Clear();

				while (fs.Length > index)
				{
					int lenght = (int)fs.Length - index < iPartSize ? (int)(fs.Length - index) : iPartSize;
					byte[] buff = read.ReadBytes(lenght);
					if (buff.Length % iPartSize != 0)
					{
						List<byte> listData = new List<byte>();

						listData.AddRange(buff);
						for (int i = 0; i < iPartSize - (buff.Length % iPartSize); i++)
						{
							listData.Add(0xFF);
						}
						buff = new byte[iPartSize];
						buff = listData.ToArray();
					}
					//Console.WriteLine(DataConvert.Bytes2HexString(buff, 0, buff.Length));
					lbaBeforeUpGrade.Add(buff);
					index += lenght;
				}
				//释放Bin文件
				fs.Close();
				read.Close();
			}
			catch (Exception e1)
			{
				//output("导入升级程序有误：" + e1.Message, true, true);
			}
		}

		private void txbAfterUpGrade_Click(object sender, EventArgs e)
		{
			if (tbAfterUpGrade.Text == "")
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
			if (openFileDialog1.ShowDialog() != DialogResult.OK)
			{
				return;
			}

			tbAfterUpGrade.Text = openFileDialog1.FileName;

			FileStream fs = new FileStream(openFileDialog1.FileName, FileMode.Open, FileAccess.Read);
			BinaryReader read = new BinaryReader(fs);

			try
			{
				int index = 0;
				int iPartSize = 8192;
				lbaAfterUpGrade.Clear();

				while (fs.Length > index)
				{
					int lenght = (int)fs.Length - index < iPartSize ? (int)(fs.Length - index) : iPartSize;
					byte[] buff = read.ReadBytes(lenght);
					if (buff.Length % iPartSize != 0)
					{
						List<byte> listData = new List<byte>();

						listData.AddRange(buff);
						for (int i = 0; i < iPartSize - (buff.Length % iPartSize); i++)
						{
							listData.Add(0xFF);
						}
						buff = new byte[iPartSize];
						buff = listData.ToArray();
					}
					lbaAfterUpGrade.Add(buff);
					index += lenght;
				}
				//释放Bin文件
				fs.Close();
				read.Close();
			}
			catch (Exception e1)
			{
				//output("导入升级程序有误：" + e1.Message, true, true);
			}
		}

		private void txbStartUpgrade_Click(object sender, EventArgs e)
		{
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}

			#region Tips
			if (tbBeforeUpGrade.Text == "" || tbAfterUpGrade.Text == "")
			{
				MessageBox.Show("请导入升级文件！");
				return;
			}
			#endregion

			#region Save
			lbaUpGrade = new List<byte[]>();
			liUpGrade = new List<int>();
			for (int i = 0; i < lbaAfterUpGrade.Count; i++)
			{
				if (!lbaAfterUpGrade[i].SequenceEqual(lbaBeforeUpGrade[i]))
				{
					liUpGrade.Add(i);
					lbaUpGrade.Add(lbaAfterUpGrade[i]);
				}
			}

			if (cbSafeFirst.Checked)
			{
				liUpGrade.Reverse();
				lbaUpGrade.Reverse();
			}

			#endregion

			try
			{
				if (txbStartUpgrade.Text == "开始升级")
				{
					txbStartUpgrade.Text = "取消升级";
					bPartUpGrade = true;
					#region Thread
					Thread td = new Thread(new ThreadStart(FuncPartUpdata));
					td.IsBackground = true;
					td.Start();
					#endregion
				}
				else
				{
					SpMeter.Working = false;
					bPartUpGrade = false;
					txbStartUpgrade.Text = "开始升级";
				}
			}
			catch (Exception exception)
			{
				output("升级过程中出现错误：" + exception.Message, true, true);
				SpMeter.Working = false;
				bPartUpGrade = false;
				txbStartUpgrade.Text = "开始升级";
			}
			finally
			{

			}
		}
		#endregion

		private void txcbMeterType_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (txcbMeterType.SelectedIndex == 0)
			{
				cbAllUpgrade.Visible = false;
				cbDoubleBlock.Visible = false;
				cbDoubleBlock.Checked = false;
				tbByteCounts.Text = "192";
				rtbBinInfo.Text = "";
			}
			else
			{
				cbAllUpgrade.Visible = true;
				cbAllUpgrade.Checked = false;
				if (txcbMeterType.SelectedIndex == 1)
				{
					cbDoubleBlock.Visible = true;
					cbDoubleBlock.Checked = true;
				}
				else
				{
					cbDoubleBlock.Visible = false;
					cbDoubleBlock.Checked = false;
				}

				tbByteCounts.Text = "200";
				rtbBinInfo.Text = "576973646F6D5261696C4D6574657273";
			}
		}

		private void txbSendmessages_Click(object sender, EventArgs e)
		{
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}

			if (rtbExplain.Text.ToString() == "")
			{
				MessageBox.Show("请先粘贴报文!");
				return;
			}
			string strDataSend = DataConvert.fnGetStringFormat(rtbExplain.Text.ToString());
			Helps.TryStrToHexByte(strDataSend, out byte[] TxBuf);
			output(DataConvert.Bytes2HexString(TxBuf, 0, TxBuf.Length), false, true);
			byte[] recBytes = SpMeter.SendCommand(TxBuf, "645");

			if (recBytes != null)
			{
				output(DataConvert.Bytes2HexString(recBytes, 0, recBytes.Length), false, true);
			}
			else
			{
				output("返回报文为空", true, false);
			}
		}

		private void tbSearch_KeyDown(object sender, KeyEventArgs e)
		{
			// 当按下回车键时，触发搜索按钮的点击事件
			if (e.KeyValue == 13)
			{
				btnSarch_Click(sender, e);
			}
		}


		private void tbSearch_Press(object sender, KeyPressEventArgs e)
		{
			if (e.KeyChar == '\r')
			{
				btnSarch_Click(null, null);
			}
		}

		private void 读取存储区ToolStripMenuItem_Click(object sender, EventArgs e)
		{
			FormReadStorage frsFrom = new FormReadStorage(Helps.fnAgainst(pmc[6].Value.ToString())) { StartPosition = FormStartPosition.CenterScreen };
			frsFrom.Show(this);
		}

		#region Transshipment
		/// <summary>
		/// 是否在Transshipment
		/// </summary>
		bool TransshipmentMoniter = false;
		Thread ThTestL = null;
		private void txcTransshipment_CheckedChanged(object sender, EventArgs e)
		{
			SpMeter.DiscardInBuffer();
			if (txcTransshipment.CheckState == CheckState.Checked)
			{
				if (btn_电表通信串口.Text == "打开串口")
				{
					this.Info("请先打开串口！");
					//MessageBox.Show("请先打开串口!");
					txcTransshipment.CheckState = CheckState.Unchecked;
					return;
				}
				TransshipmentMoniter = true;
				SpMeter.Working = true;
				ThTestL = new Thread(new ParameterizedThreadStart(tTransshipmentMoniter));
				ThTestL.IsBackground = true;
				ThTestL.Start();
			}
			else
			{
				StopTransshipmentMoniter();
			}
		}

		/// <summary>
		/// 自动线程
		/// </summary>
		private void tTransshipmentMoniter(object Interval)
		{
			try
			{
				while (TransshipmentMoniter)
				{
					if (TransshipmentMoniter)//&& SpMeter.Working
					{
						byte[] baReceBuf = SpMeter.MoniterSerial();
						if (baReceBuf != null)
						{
							if (baReceBuf.Length > 0)
							{
								lock (rev_obj)
								{
									if (((baReceBuf[1] == 00 && baReceBuf[0] == 0xFE) || (baReceBuf[0] == 0x68 && baReceBuf[1] == 00 && baReceBuf[2] == 00 && baReceBuf[3] == 00 && baReceBuf[4] == 00 && baReceBuf[5] == 00 && baReceBuf[6] == 00)))
									{
										byte[] baReceBufT = new byte[1];
										baReceBufT[0] = baReceBuf[0];
										baReceBuf = new byte[1];
										baReceBuf[0] = baReceBufT[0];
									}
									TransshipmentMoniter = false;
									List<string> resultList = new List<string>();
									output("RX:" + Helps.ByteToStringWithSpace(baReceBuf), false, true);

									RecBuf = null;

									var vBool = SendDataBLE(baReceBuf);
									Thread.Sleep(10);
								}

								TransshipmentMoniter = true;
								output("蓝牙发送:" + Helps.ByteToStringWithSpace(baReceBuf), false, true);
							}
						}
						Thread.Sleep(50);
					}
					else
					{
						break;
					}
				}
			}
			catch
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
			txcTransshipment.CheckState = CheckState.Unchecked;
		}

		#endregion

		private void txbFactory_Click(object sender, EventArgs e)
		{
			Btn_厂内_Click(null, null);
		}

		private void txbFactoryEn_Click(object sender, EventArgs e)
		{
			lstrDataMark.Clear();
			lstrDataMark.Add("DBDF1166");
			lstrDataMark.Add("04DFDFDB");
			FunDealMessage("", DLT645.ControlType.Read, lstrDataMark, out string strData);
			byte bFirst = byte.Parse(strData.Substring(0, 2), NumberStyles.HexNumber);
			byte[] baBase = { 0x44, 0x59, 0x54, 0x51 };
			string strInData = "";
			for (int i = 0; i < baBase.Length; i++)
			{
				byte bData = (byte)(byte.Parse(strData.Substring(i * 2, 2), NumberStyles.HexNumber) + bFirst - baBase[i]);
				strInData += bData.ToString("X2");
			}
			strInData = "01" + strInData;
			strInData = Helps.fnAgainst(strInData);
			lstrDataMark.Clear();
			lstrDataMark.Add("DBDF0201");
			lstrDataMark.Add("04DFDFDB");

			strErr = FunDealMessage(strInData, DLT645.ControlType.Write, lstrDataMark, out strData);
			if (strErr != "")
			{
				output(strErr, true);
				MessageBox.Show(strErr);
				return;
			}
		}

		private void txTreeComboBox4_SelectedIndexChanged(object sender, EventArgs e)
		{

		}

		private void txbPort_Click(object sender, EventArgs e)
		{
			if (txbPort.Text == "打开串口")
			{
				//stSource.setSerialPort(txcbPortSource.Text.ToString(), 38400, "None", 8, "1", 500);
				if (stSource.setSerialPort(txcbPortSource.Text.ToString(), 38400, "None", 8, "1", 1500))
				{
					txbPort.Text = "关闭串口";
				}
				else
				{
					MessageBox.Show("打开串口失败！");
				}
			}
			else
			{
				stSource.closePort();
				txbPort.Text = "打开串口";
			}
		}

		/// <summary>
		/// 把一个浮点数转化成16进制字符串
		/// </summary>
		/// <param name="paraFloat">要转化的浮点数</param>
		/// <returns>返回的字符串</returns>
		public static string FloatToHex(float paraFloat)
		{
			StringBuilder sb = new StringBuilder();
			byte[] bytes = BitConverter.GetBytes(paraFloat);

			foreach (var item in bytes)
			{
				sb.Insert(0, item.ToString("X2"));
			}
			return sb.ToString();
		}

		private void txbUpSource_Click(object sender, EventArgs e)
		{
			if (txbPort.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}
			float fUa = float.Parse(txtbUa.Text.ToString());
			float fUb = float.Parse(txtbUb.Text.ToString());
			float fUc = float.Parse(txtbUc.Text.ToString());
			string strUa = "01" + Helps.fnAgainst(FloatToHex(float.Parse(txtbUa.Text.ToString())));
			string strUb = "03" + Helps.fnAgainst(FloatToHex(float.Parse(txtbUb.Text.ToString())));
			string strUc = "05" + Helps.fnAgainst(FloatToHex(float.Parse(txtbUc.Text.ToString())));
			string strAngleUa = "02" + Helps.fnAgainst(FloatToHex(float.Parse(txtbAngleUa.Text.ToString())));
			string strAngleUb = "04" + Helps.fnAgainst(FloatToHex(float.Parse(txtbAngleUb.Text.ToString())));
			string strAngleUc = "06" + Helps.fnAgainst(FloatToHex(float.Parse(txtbAngleUc.Text.ToString())));
			//stSource.setSerialPort(txcbPortSource.Text.ToString(), 38400, "None", 8, "1", 500);
			string strIa = "07" + Helps.fnAgainst(FloatToHex(float.Parse(txtbIa.Text.ToString())));
			string strIb = "09" + Helps.fnAgainst(FloatToHex(float.Parse(txtbIb.Text.ToString())));
			string strIc = "0B" + Helps.fnAgainst(FloatToHex(float.Parse(txtbIc.Text.ToString())));
			string strAngleIa = "08" + Helps.fnAgainst(FloatToHex(float.Parse(txtbAngleIa.Text.ToString())));
			string strAngleIb = "0A" + Helps.fnAgainst(FloatToHex(float.Parse(txtbAngleIb.Text.ToString())));
			string strAngleIc = "0C" + Helps.fnAgainst(FloatToHex(float.Parse(txtbAngleIc.Text.ToString())));
			string strHzAB = "0E00004842";
			string strHzC = "0F00004842";
			string strMode = "2D04000000";
			if (txcbMode.Text.ToString() == "三相四线")
			{
				string strData = strUa + strAngleUa + strUb + strAngleUb + strUc + strAngleUc + strIa + strAngleIa + strIb + strAngleIb + strIc + strAngleIc + strHzAB + strHzC + strMode;
				string strLen = Helps.fnAgainst(((strData.Length / 2) + 8).ToString("X4"));
				string Message = "68" + strLen + "68";
				strData = "0092" + strData;
				byte[] baBufStrat = null;
				byte[] baBuf = null;
				Helps.TryStrToHexByte(Message, out baBufStrat);
				Helps.TryStrToHexByte(strData, out baBuf);

				#region 校验和+16
				byte sum = Helps.Ssum(baBuf);
				byte[] endBytes = new[] { sum, (byte)0x16 };
				baBuf = baBuf.Concat(endBytes).ToArray();
				baBuf = baBufStrat.Concat(baBuf).ToArray();
				output("TX:" + Helps.ByteToStringWithSpace(baBuf), false, true);
				stSource.Working = true;
				byte[] recBytes = stSource.SendCommand(baBuf, baBuf.Length, 3000);
				if (recBytes != null)
				{
					output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
				}
				Helps.TryStrToHexByte("681a006800031801001901001a01001b01001c01001d0100a816", out baBufStrat);
				output("TX:" + Helps.ByteToStringWithSpace(baBufStrat), false, true);
				stSource.Working = true;
				recBytes = stSource.SendCommand(baBufStrat, baBufStrat.Length, 3000);
				if (recBytes != null)
				{
					output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
				}
				#endregion
			}
			else if (txcbMode.Text.ToString() == "三相三线")
			{
				fUa = (float)(double.Parse(txtbUa.Text.ToString()) / Math.Sqrt(3));
				fUb = (float)(double.Parse(txtbUb.Text.ToString()) / Math.Sqrt(3));
				fUc = (float)(double.Parse(txtbUc.Text.ToString()) / Math.Sqrt(3));
				strUa = "01" + Helps.fnAgainst(FloatToHex(fUa));
				strUb = "03" + Helps.fnAgainst(FloatToHex(fUb));
				strUc = "05" + Helps.fnAgainst(FloatToHex(fUc));
				strMode = "2D03000000";
				string strData = strUa + strAngleUa + strUb + strAngleUb + strUc + strAngleUc + strIa + strAngleIa + strIb + strAngleIb + strIc + strAngleIc + strHzAB + strHzC + strMode;
				string strLen = Helps.fnAgainst(((strData.Length / 2) + 8).ToString("X4"));
				string Message = "68" + strLen + "68";
				strData = "0092" + strData;
				byte[] baBufStrat = null;
				byte[] baBuf = null;
				Helps.TryStrToHexByte(Message, out baBufStrat);
				Helps.TryStrToHexByte(strData, out baBuf);

				#region 校验和+16
				byte sum = Helps.Ssum(baBuf);
				byte[] endBytes = new[] { sum, (byte)0x16 };
				baBuf = baBuf.Concat(endBytes).ToArray();
				baBuf = baBufStrat.Concat(baBuf).ToArray();
				output("TX:" + Helps.ByteToStringWithSpace(baBuf), false, true);
				stSource.Working = true;
				byte[] recBytes = stSource.SendCommand(baBuf, baBuf.Length, 3000);
				if (recBytes != null)
				{
					output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
				}
				Helps.TryStrToHexByte("6817006800031801001901001a01001b01001d01008b16", out baBufStrat);
				output("TX:" + Helps.ByteToStringWithSpace(baBufStrat), false, true);
				stSource.Working = true;
				recBytes = stSource.SendCommand(baBufStrat, baBufStrat.Length, 3000);
				if (recBytes != null)
				{
					output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
				}
				#endregion
			}
		}

		private void txbDownSource_Click(object sender, EventArgs e)
		{
			if (txbPort.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}
			byte[] baBufStrat = null;
			Helps.TryStrToHexByte("681a006800041f0100200100210100220100230100240100d316", out baBufStrat);
			output("TX:" + Helps.ByteToStringWithSpace(baBufStrat), false, true);
			stSource.Working = true;
			byte[] recBytes = stSource.SendCommand(baBufStrat, baBufStrat.Length, 3000);
			if (recBytes != null)
			{
				output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
			}
		}

		private void txbCloseFactory_Click(object sender, EventArgs e)
		{
			lstrDataMark.Clear();
			lstrDataMark.Add("DBDF0201");
			lstrDataMark.Add("04DFDFDB");
			string strErr = FunDealMessage("00", DLT645.ControlType.Write, lstrDataMark, out string strData);
			if (strErr != "")
			{
				if (strErr != "请先打开串口!")
				{
					MessageBox.Show(strErr);
				}
			}
		}

		private void txcbBLE_CheckedChanged(object sender, EventArgs e)
		{
			if (txcbBLE.Checked)
			{
				bBLE = true;
			}
			else
			{
				bBLE = false;
			}
		}

		private void txbUpGradeResult_Click(object sender, EventArgs e)
		{
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}
			byte[] TxBufFactoryMode = new byte[32];
			string strDataSend = "68" + Helps.fnAgainst(pmc[6].Value.ToString()) + "6814150E121237" + (DataConvert.fnEncodingData(pmc[8].Value.ToString() + pmc[7].Value.ToString())) + "373635343435120EBBBBBBBB34";
			Helps.TryStrToHexByte(strDataSend, out TxBufFactoryMode);

			#region 校验和+16
			byte sum = Helps.Ssum(TxBufFactoryMode);
			byte[] endBytes = new[] { sum, (byte)0x16 };
			#endregion

			TxBufFactoryMode = TxBufFactoryMode.Concat(endBytes).ToArray();
			//byte[] TxBufFactoryMode = { 0xfe, 0xfe, 0xfe, 0xfe, 0x68, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x68, 0x14, 0x15, 0x0E, 0x12, 0x12, 0x37, 0x35, 0x33, 0x33, 0x33, 0x37, 0x36, 0x35, 0x34, 0x34, 0x35, 0x12, 0xE, 0xBB, 0xBB, 0xBB, 0xBB, 0x34, 0xdf, 0x16 };
			if (sendOK(TxBufFactoryMode, 0x94))
			{
				output(" 通信成功", false, true);
			}
			else
			{
				output(" 通信失败!", true, true);
			}

			Thread.Sleep(10);
			byte[] TxBufInitial = new byte[31];
			strDataSend = "68" + Helps.fnAgainst(pmc[6].Value.ToString()) + "6814140E121237" + (DataConvert.fnEncodingData(pmc[8].Value.ToString() + pmc[7].Value.ToString())) + "373635340E3A120EBBBBBBBB";
			Helps.TryStrToHexByte(strDataSend, out TxBufInitial);

			#region 校验和+16
			sum = Helps.Ssum(TxBufInitial);
			endBytes = new[] { sum, (byte)0x16 };
			#endregion

			TxBufInitial = TxBufInitial.Concat(endBytes).ToArray();

			if (sendOK(TxBufInitial, 0x94))
			{
				output("电表通信成功", false, true);//Initial Meter Success
			}
			else
			{
				output("通信异常,请检查", false, true);//Meter reply error,Check the connection please
			}
			Thread.Sleep(13000);

			#region 1读取版本
			IniClass ini = new IniClass(Application.StartupPath + @"\Config.ini");
			if (FunReadVersionF460(out string strVersionF460) != "")
			{
				MessageBox.Show("读版本失败!");
				return;
			}
			else
			{
				string strVerUpGrade = ini.IniReadValue("Main", "UpGradeResult", "");

				if (strVersionF460.Substring(4, 4) == strVerUpGrade)
				{
					MessageBox.Show("升级成功！");
				}
				else
				{
					MessageBox.Show("升级失败！，请重试！");
				}
			}
			#endregion
		}

		private void txbRailVersion_Click(object sender, EventArgs e)
		{
			lstrDataMark.Clear();
			lstrDataMark.Add("DBDF1119");
			lstrDataMark.Add("04DFDFDB");
			FunDealMessage("", DLT645.ControlType.Read, lstrDataMark, out string strData);
			string[] strVer = new string[4];
			for (int i = 0; i < 4; i++)
			{
				strVer[i] = (strData.Substring(i * 4, 4));
			}
			MessageBox.Show("Boot校验:		" + strVer[0] + "\n" + "App校验:		" + strVer[1] + "\n" + "SafePara校验:	" + strVer[2] + "\n" + "总校验:		" + strVer[3] + "\n");
			byte bFirst = byte.Parse(strData.Substring(0, 2), NumberStyles.HexNumber);
			//byte [] baBase = {0x44,0x59,0x54,0x51};
			//string strInData = "";
			//for (int i = 0; i < baBase.Length; i++)
			//{
			//	byte bData =(byte) ( byte.Parse(strData.Substring(i * 2, 2), NumberStyles.HexNumber ) + bFirst - baBase[i] );
			//	strInData += bData.ToString("X2");
			//}
			//strInData = "01" + strInData;
			//strInData = Helps.fnAgainst(strInData);
		}
		/// <summary>
		/// 误差
		/// </summary>
		private void ReadCheckErrorTest()
		{
			#region CheckError
			dgvCheckErr.AutoGenerateColumns = false;
			dtCheckErrDataTable = new DataTable(null);
			dtCheckErrDataTable.Columns.Add("选择", typeof(bool));
			dtCheckErrDataTable.Columns.Add("Name", typeof(string));
			dtCheckErrDataTable.Columns.Add("DataMark", typeof(string));
			dtCheckErrDataTable.Columns.Add("DataLen", typeof(string));
			dtCheckErrDataTable.Columns.Add("DataBlockType", typeof(string));
			dtCheckErrDataTable.Columns.Add("DataFormat", typeof(string));
			dtCheckErrDataTable.Columns.Add("DecimalPlaces", typeof(string));
			dtCheckErrDataTable.Columns.Add("DataUnit", typeof(string));
			dtCheckErrDataTable.Columns.Add("读取值", typeof(string));
			dtCheckErrDataTable.Columns.Add("相对误差%", typeof(string));
			List<string> lsList = new List<string>();
			{
				//lsList.Add("TRUE-高精度相电压A-021F0100-3--BCD-2-V--");
				//lsList.Add("TRUE-高精度相电压B-021F0200-3--BCD-2-V--");
				//lsList.Add("TRUE-高精度相电压C-021F0300-3--BCD-2-V--");
				lsList.Add("TRUE-高精度线电压AB-02200100-3--BCD-2-V--");
				lsList.Add("TRUE-高精度线电压AC-02200200-3--BCD-2-V--");
				lsList.Add("TRUE-高精度线电压BC-02200300-3--BCD-2-V--");
				lsList.Add("TRUE-高精度电流A-021B0100-4--BCD-4-A--");
				lsList.Add("TRUE-高精度电流B-021B0200-4--BCD-4-A--");
				lsList.Add("TRUE-高精度电流C-021B0300-4--BCD-4-A--");
				lsList.Add("TRUE-高精度瞬时总有功功率-020C0000-4--BCD-4-kW--");
				lsList.Add("TRUE-高精度A相有功功率-020C0100-4--BCD-4-kW--");
				lsList.Add("TRUE-高精度B相有功功率-020C0200-4--BCD-4-kW--");
				lsList.Add("TRUE-高精度C相有功功率-020C0300-4--BCD-4-kW--");
			}


			foreach (var VARIABLE in lsList)
			{
				object[] oRow = VARIABLE.Split('-');
				dtCheckErrDataTable.Rows.Add(oRow);
			}

			dgvCheckErr.DataSource = dtCheckErrDataTable;
			#endregion
		}
		private void txbCheckError_Click(object sender, EventArgs e)
		{
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}

			if (SpMeter.Working)
			{
				return;
			}
			try
			{
				DataRow[] drs = dtCheckErrDataTable.Select(string.Format("选择 = '{0}' ", "true"), "");
				int rowCount = drs.GetLength(0);
				if (rowCount < 1)
				{
					return;
				}

				SpMeter.Working = true;

				#region 抄读
				bool isReadingErr = false;
				DLT645 dlt645 = new DLT645(Helps.fnAgainst(pmc[6].Value.ToString()));
				for (int i = 0; i < rowCount && SpMeter.Working; i++)
				{
					List<string> dataMarkList = new List<string>();
					byte seq = 0;
					string dataResultStr = "";

					if (drs[i]["DataMark"] == null || drs[i]["DataMark"].ToString().Length != 8)
					{
						drs[i]["读取值"] = "";
						drs[i]["相对误差%"] = "抄读失败：数据标识非法！";
						continue;
					}

					string dataMark = drs[i]["DataMark"].ToString();
					dataMarkList.Add(dataMark);

					byte[] sendBytes = null;
					string errStr = "";
					if (!dlt645.Pack(DLT645.ControlType.Read, dataMarkList, "", 0, out sendBytes, out errStr))
					{
						drs[i]["读取值"] = "";
						drs[i]["相对误差%"] = errStr;
						continue;
					}

					string strAPDU = "";
					byte[] baAPDU;
					byte[] baTemp;
					byte[] recBytes;
					{
						output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
						recBytes = SpMeter.SendCommand(sendBytes, "645");
					}

					output("RX:" + Helps.ByteToStringWithSpace(recBytes), false, true);
					bool isHaveHouxu = false;
					if (!SpMeter.Working)
					{
						continue;
					}

					string dataPianDuan;
					DLT645.ControlType controlType = DLT645.ControlType.Read;

					if (!dlt645.UnPack(recBytes, controlType, dataMarkList, seq, out isHaveHouxu, out dataPianDuan, out errStr))
					{
						drs[i]["读取值"] = "";
						drs[i]["相对误差%"] = errStr;
						isReadingErr = true;
						continue;
					}

					dataResultStr = dataPianDuan.Trim();

					int dataLen;

					if (!int.TryParse(drs[i]["DataLen"].ToString(), out dataLen))
					{
						drs[i]["读取值"] = "";
						drs[i]["相对误差%"] = "数据库中该数据项长度非法，请排查数据库信息！";
						continue;
					}

					if (dataResultStr.Length != dataLen * 2)
					{
						drs[i]["读取值"] = "";
						drs[i]["相对误差%"] = "失败：返回数据长度非法";
						continue;
					}
					int decimalPlaces = 0;
					if (drs[i]["DecimalPlaces"] == null || (!int.TryParse(drs[i]["DecimalPlaces"].ToString(), out decimalPlaces)))
					{
						decimalPlaces = 0;
					}
					string strSymble = "";
					if ((drs[i]["Name"].ToString().Contains("功率因数")) || (drs[i]["DataUnit"].ToString() == "A") || (drs[i]["DataUnit"].ToString() == "kW") || (drs[i]["DataUnit"].ToString() == "Kvar") || (drs[i]["DataUnit"].ToString() == "kVA"))
					{
						int iTe = 0;

						if ((Convert.ToInt64("0x" + dataResultStr.Substring(0, 1), 16) & 0x08) == 8)
						{
							strSymble = "-";
						}
						iTe = (int)(Convert.ToInt64("0x" + dataResultStr.Substring(0, 1), 16) & 0x07);
						dataResultStr = strSymble + iTe.ToString() + dataResultStr.Substring(1);
					}
					if (decimalPlaces != 0)
					{
						dataResultStr = dataResultStr.Insert(dataResultStr.Length - decimalPlaces, ".");
					}
					drs[i]["读取值"] = dataResultStr;
					//GetData(drs[i], dataResultStr, dataLen);
					if ((drs[i]["Name"].ToString().Contains("电压")))
					{
						double dU = double.Parse(dataResultStr);
						double dErr = ((dU - 1140) * 100 / 1140);
						string strPass = "";
						if (Math.Abs(dErr) > 0.2)
						{
							strPass = "--超差!!";
						}
						else
						{
							strPass = "--合格";
						}
						drs[i]["相对误差%"] = (dErr).ToString("F2") + "%" + strPass;
					}
					else if ((drs[i]["Name"].ToString().Contains("电流")))
					{
						double dI = double.Parse(dataResultStr);
						double dStandard = 0;
						double dLimit = 0.2;
						if (txrbIb.Checked)
						{
							if (drs[i]["Name"].ToString() == "高精度电流B")
							{
								dStandard = 0;
								if (dI != 0)
								{
									drs[i]["相对误差%"] = "高精度电流B不为 0 --超差!!";
								}
								else
								{
									drs[i]["相对误差%"] = "合格";
								}
							}
							else
							{
								dStandard = 1.5;
								dLimit = 0.2;
								double dErr = ((dI - dStandard) * 100 / dStandard);
								string strPass = "";
								if (Math.Abs(dErr) > dLimit)
								{
									strPass = "--超差!!";
								}
								else
								{
									strPass = "--合格";
								}
								drs[i]["相对误差%"] = (dErr).ToString("F2") + "%" + strPass;
							}
						}
						else if (txrbStart.Checked)
						{
							if (drs[i]["Name"].ToString() == "高精度电流B")
							{
								dStandard = 0;
								if (dI != 0)
								{
									drs[i]["相对误差%"] = "高精度电流B不为 0 --超差!!";
								}
								else
								{
									drs[i]["相对误差%"] = "合格";
								}
							}
							else
							{
								if (dI == 0)
								{
									drs[i]["相对误差%"] = "高精度电流为 0 --超差!!";
								}
								else
								{
									drs[i]["相对误差%"] = "合格";
								}
							}
						}
						else if (txrbSneak.Checked)
						{
							dStandard = 0;
							if (dI != 0)
							{
								drs[i]["相对误差%"] = "高精度电流不为 0 --超差!!";
							}
							else
							{
								drs[i]["相对误差%"] = "合格";
							}
						}
					}
					else if ((drs[i]["Name"].ToString().Contains("功率")))
					{
						double dP = double.Parse(dataResultStr);
						double dStandard = 0;
						double dLimit = 0.2;
						if (txrbIb.Checked)
						{
							if (drs[i]["Name"].ToString() == "高精度B相有功功率")
							{
								dStandard = 0;
								if (dP != 0)
								{
									drs[i]["相对误差%"] = "高精度有功功率B不为 0 --超差!!";
								}
								else
								{
									drs[i]["相对误差%"] = "合格";
								}
							}
							else if (drs[i]["Name"].ToString() == "高精度瞬时总有功功率")
							{
								dStandard = 1.5 * 1140 * Math.Sqrt(3) / 1000;
								dLimit = 0.2;
								double dErr = ((dP - dStandard) * 100 / dStandard);
								string strPass = "";
								if (Math.Abs(dErr) > dLimit)
								{
									strPass = "--超差!!";
								}
								else
								{
									strPass = "--合格";
								}
								drs[i]["相对误差%"] = (dErr).ToString("F2") + "%" + strPass;
							}
							else
							{
								dStandard = 1.5 * 1140 * Math.Sqrt(3) / 2 / 1000;
								dLimit = 0.2;
								double dErr = ((dP - dStandard) * 100 / dStandard);
								string strPass = "";
								if (Math.Abs(dErr) > dLimit)
								{
									strPass = "--超差!!";
								}
								else
								{
									strPass = "--合格";
								}
								drs[i]["相对误差%"] = (dErr).ToString("F2") + "%" + strPass;
							}
						}
						else if (txrbStart.Checked)
						{
							if (drs[i]["Name"].ToString() == "高精度B相有功功率")
							{
								dStandard = 0;
								if (dP != 0)
								{
									drs[i]["相对误差%"] = "高精度有功功率B不为 0 --超差!!";
								}
								else
								{
									drs[i]["相对误差%"] = "合格";
								}
							}
							else if (drs[i]["Name"].ToString() == "高精度瞬时总有功功率")
							{
								dStandard = dStandard = 1.5 * 1140 * Math.Sqrt(3) * 0.001 / 1000;
								dLimit = 30;
								double dErr = ((dP - dStandard) * 100 / dStandard);
								string strPass = "";
								if (Math.Abs(dErr) > dLimit)
								{
									strPass = "--超差!!";
								}
								else
								{
									strPass = "--合格";
								}
								drs[i]["相对误差%"] = (dErr).ToString("F2") + "%" + strPass;
							}
							else
							{
								dStandard = 1.5 * 1140 * Math.Sqrt(3) / 2 * 0.001 / 1000;
								dLimit = 30;
								double dErr = ((dP - dStandard) * 100 / dStandard);
								string strPass = "";
								if (Math.Abs(dErr) > dLimit)
								{
									strPass = "--超差!!";
								}
								else
								{
									strPass = "--合格";
								}
								drs[i]["相对误差%"] = (dErr).ToString("F2") + "%" + strPass;
							}
						}
						else if (txrbSneak.Checked)
						{
							dStandard = 0;
							if (dP != 0)
							{
								drs[i]["相对误差%"] = "有功功率不为 0 --超差!!";
							}
							else
							{
								drs[i]["相对误差%"] = "合格";
							}
						}
					}
					//else if ((drs[i]["Name"].ToString().Contains("电流")))
					//{  
					//	double [] daII = {0.011, 0.010, 0.009, 0.008, 0.007, 0.006, 0.005, 0.004, 0.003, 0.002, 0.001, 0, -0.011, -0.010, -0.009, -0.008, -0.007, -0.006, -0.005, -0.004, -0.003, -0.002, -0.001};
					//	double [] daI = {  0.011, 0.010, 0.009, 0.008, 0.007, 0.006, 0.005, 0.004, 0.003, 0.002, 0.001, 0, -0.011, -0.010, -0.009, -0.008, -0.007, -0.006, -0.005, -0.004, -0.003, -0.002, -0.001};
					//	double dUI = GetRandomNumber(daI) +  GetRandomNumber(daII);
					//	drs[i]["读取值"] = (double.Parse(txtbI.Text.ToString() ) * double.Parse(txtbPercentI.Text.ToString() ) / 100+ dUI).ToString("F3");
					//	drs[i]["相对误差%"] = (dUI * 100 / double.Parse(txtbI.Text.ToString() ) ).ToString("F2");
					//}
					//else if ((drs[i]["Name"].ToString().Contains("功率因数")))
					//{  
					//	double [] daCOS = { 0, -0.001};
					//	double dCOS = GetRandomNumber(daCOS);
					//	drs[i]["读取值"] = (Math.Cos(double.Parse(txtbAngle.Text.ToString() )  * (Math.PI) / 180 ) + dCOS).ToString("F3");
					//	drs[i]["相对误差%"] = (dCOS * 100 ).ToString("F2");
					//}
					//else if ((drs[i]["Name"].ToString().Contains("视在功率")))
					//{  
					//	double [] daApparentPowper = {0.0101, 0.0102, 0.0103, 0.0124, 0.0115, 0.0106, 0.0097, 0.0086, 0.0075, 0.0064, 0.0053, 0.0042, 0.0031, 0.0021, 0.0011, 0, -0.0101, -0.0102, -0.0103, -0.0104, -0.0115, -0.0104, -0.0093, -0.0082, -0.0071, -0.0063, -0.0054, -0.0045, -0.0036, -0.0022, -0.0011};
					//	double [] daApparentPowperH = {0.003, 0.002, 0.001, 0, -0.003, -0.002, -0.001};
					//	double dApparentPowper = GetRandomNumber(daApparentPowper) + GetRandomNumber(daApparentPowperH);
					//	drs[i]["读取值"] = ( (double.Parse(txtbI.Text.ToString() ) * double.Parse(txtbPercentI.Text.ToString() ) / 100) * (double.Parse(txtbU.Text.ToString() ) * double.Parse(txtbPercentU.Text.ToString() ) / 100) * 3 / 1000+ dApparentPowper).ToString("F4");
					//	drs[i]["相对误差%"] = (dApparentPowper * 100 / (double.Parse(txtbI.Text.ToString() ) * (double.Parse(txtbU.Text.ToString() ) ) * 3 / 1000) ).ToString("F2");
					//}
					//else if ((drs[i]["Name"].ToString().Contains("有功功率")))
					//{  
					//	double [] daApparentPowper = {0.0101, 0.0102, 0.0103, 0.0104, 0.0115, 0.0106, 0.0097, 0.0086, 0.0075, 0.0064, 0.0053, 0.0042, 0.0031, 0.0021, 0.0011, 0, -0.0101, -0.0102, -0.0103, -0.0104, -0.0115, -0.0104, -0.0093, -0.0082, -0.0071, -0.0063, -0.0054, -0.0045, -0.0036, -0.0022, -0.0011};
					//	double [] daApparentPowperH = {0.003, 0.002, 0.001, 0, -0.003, -0.002, -0.001};
					//	double dApparentPowper = (GetRandomNumber(daApparentPowper) + GetRandomNumber(daApparentPowperH) ) ;
					//	if (txtbAngle.Text.ToString() != "90" && txtbAngle.Text.ToString() != "270")
					//	{
					//		dApparentPowper = (GetRandomNumber(daApparentPowper) + GetRandomNumber(daApparentPowperH) ) * Math.Cos(double.Parse(txtbAngle.Text.ToString() )  * (Math.PI) / 180 );
					//	}

					//	drs[i]["读取值"] = ( (double.Parse(txtbI.Text.ToString() ) * double.Parse(txtbPercentI.Text.ToString() ) / 100) * (double.Parse(txtbU.Text.ToString() ) * double.Parse(txtbPercentU.Text.ToString() ) / 100) * 3 * Math.Cos(double.Parse(txtbAngle.Text.ToString() )  * (Math.PI) / 180 ) / 1000+ dApparentPowper).ToString("F4");
					//	if (txtbAngle.Text.ToString() != "90" && txtbAngle.Text.ToString() != "270")
					//	{
					//		drs[i]["相对误差%"] = (dApparentPowper * 100 / (double.Parse(txtbI.Text.ToString() ) * (double.Parse(txtbU.Text.ToString() ) * 3 * Math.Cos(double.Parse(txtbAngle.Text.ToString() )  * (Math.PI) / 180 ) / 1000) ) ).ToString("F2");
					//	}
					//	else
					//	{
					//		drs[i]["相对误差%"] = "";
					//	}
					//}
					//else if ((drs[i]["Name"].ToString().Contains("无功功率")))
					//{  
					//	double [] daApparentPowper = {0.0151, 0.0142, 0.0133, 0.0124, 0.0115, 0.0106, 0.0097, 0.0086, 0.0075, 0.0064, 0.0053, 0.0042, 0.0031, 0.0021, 0.0011, 0, -0.0151, -0.0142, -0.0133, -0.0124, -0.0115, -0.0104, -0.0093, -0.0082, -0.0071, -0.0063, -0.0054, -0.0045, -0.0036, -0.0022, -0.0011};
					//	double [] daApparentPowperH = {0.5, 0.6,0.7, 0.8};
					//	double dApparentPowper = GetRandomNumber(daApparentPowper) + GetRandomNumber(daApparentPowperH);
					//	drs[i]["读取值"] = ( (double.Parse(txtbI.Text.ToString() ) * double.Parse(txtbPercentI.Text.ToString() ) / 100) * (double.Parse(txtbU.Text.ToString() ) * double.Parse(txtbPercentU.Text.ToString() ) / 100) * 3 * Math.Sin(double.Parse(txtbAngle.Text.ToString() )  * (Math.PI) / 180 ) / 1000+ dApparentPowper).ToString("F4");
					//	if (txtbAngle.Text.ToString() != "0" && txtbAngle.Text.ToString() != "180")
					//	{
					//		drs[i]["相对误差%"] = (dApparentPowper * 100 / (double.Parse(txtbI.Text.ToString() ) * (double.Parse(txtbU.Text.ToString() ) * 3 * Math.Sin(double.Parse(txtbAngle.Text.ToString() )  * (Math.PI) / 180 )/ 1000) ) ).ToString("F2");
					//	}
					//	else
					//	{
					//		drs[i]["相对误差%"] = "";
					//	}
					//}
					Thread.Sleep(16);
				}
				#endregion
			}
			catch (Exception exception)
			{
				output("抄读过程中出现错误：" + exception.Message, true, true);
			}
			finally
			{
				if (SpMeter.Working)
				{
					SpMeter.Working = false;
				}
			}
		}

		private void tpReadData_Click(object sender, EventArgs e)
		{

		}

		private void txrbIb_CheckedChanged(object sender, EventArgs e)
		{

		}

		private void label84_Click(object sender, EventArgs e)
		{

		}

		private void txbTripping_Click(object sender, EventArgs e)
		{
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}

			try
			{
				if (txbTripping.Text == "开始测试")
				{
					if (!int.TryParse(pmc[5].Value.ToString(), out int overTime))
					{
						MessageBox.Show("非法的等待超时时间！");
						return;
					}
					SpMeter.OverTime = overTime;
					drs = dtTripDataTable.Select(string.Format("选择 = '{0}' ", "true"), "");
					//iTeleindicationTestCount = int.Parse(txtbTeleindicationNum.Text);
					SpMeter.Working = true;
					bTripTest = true;
					txbTripping.Text = "停止测试";
					uiSendCount = 0;
					uiSendCorrentCount = 0;
					#region Thread
					Thread td = new Thread(new ThreadStart(tTestTrippiing));
					td.IsBackground = true;
					td.Start();
					#endregion
				}
				else
				{
					SpMeter.Working = false;
					bTripTest = false;
					txbTripping.Text = "开始测试";
				}
			}
			catch (Exception exception)
			{
				output("设置过程中出现错误：" + exception.Message, true, true);
				SpMeter.Working = false;
				bTripTest = false;
				txbTripping.Text = "开始测试";
			}
			finally
			{

			}
		}

		[DllImport("kernel32")]
		private static extern uint GetTickCount();
		/// <summary>
		/// 延时函数，单位ms毫秒
		/// </summary>
		/// <param name="iMSWait"></param>
		protected void fnWait(Int64 iMSWait)
		{
			Int64 time_cal, outtime_cal;
			time_cal = (Int64)GetTickCount();

			while (true)
			{
				outtime_cal = (Int64)GetTickCount() - time_cal;
				if (outtime_cal > iMSWait)
				{
					break;
				}
				Application.DoEvents();
				Thread.Sleep(1);
			}
		}

		/// <summary>
		/// 
		/// </summary>
		private void tTestTrippiing()
		{
			try
			{
				while (bTripTest)
				{
					uiSendCount++;
					#region 报文测试
					for (int i = 0; i < dtTripDataTable.Rows.Count && bTripTest; i++)
					{
						if (dtTripDataTable.Rows[i]["选择"].ToString() == "false")
						{
							continue;
						}
						byte bCycle = 00;
						var dataResultStr = "";
						DateTime dtSeTime = DateTime.Now.AddYears(1);
						eumRemoteControlType erctCommand = eumRemoteControlType.eumWindDown;
						if (i < 2)
						{
							if (i == 1)
							{
								bCycle = 0x40;
							}

							erctCommand = eumRemoteControlType.eumWindDown;
						}
						else
						{
							if (i == 3)
							{
								bCycle = 0x40;
							}
							erctCommand = eumRemoteControlType.eumWindUp;
						}
						string strData = (erctCommand | (eumRemoteControlType)bCycle).ToString("X") + "00" + Helps.fnAgainst(dtSeTime.ToString("yyMMddHHmmss"));

						string strErr = FunDealMessage(strData, DLT645.ControlType.Control, lstrDataMark, out strData);
						if (strErr != "")
						{
							MessageBox.Show(strErr);
							bTripTest = false;
							FnStopTrippiingHandler(1);
							break;
						}
						fnWait(Int64.Parse(pmc[10].Value.ToString()));
						//Int64 it = Int64.Parse(pmc[10].Value.ToString());
						//Thread.Sleep(Int64.Parse(pmc[10].Value.ToString()));
						lstrDataMark.Clear();

						string errStr = "";
						string strDataMark = dtTripDataTable.Rows[i]["DataMark"].ToString();
						lstrDataMark.Add(strDataMark);
						strErr = "";
						strErr = FunDealMessage("", DLT645.ControlType.Read, lstrDataMark, out strData);
						if (strErr != "")
						{
							MessageBox.Show(strErr);
							bTripTest = false;
							FnStopTrippiingHandler(1);
							break;
						}
						else
						{
							drs[i]["读取值"] = strData;
							if (i == 0 || i == 1)
							{
								if (strData != "0000")
								{
									drs[i]["操作结论"] = "合闸状态字错误!!";
									FnStopTrippiingHandler(1);
									bTripTest = false;
									break;
								}
								else
								{
									drs[i]["操作结论"] = "合闸状态字正确!!";
								}
							}
							else if (i == 2 || i == 3)
							{
								if (strData != "5000")
								{
									drs[i]["操作结论"] = "跳闸状态字错误!!";
									bTripTest = false;
									FnStopTrippiingHandler(1);
									break;
								}
								else
								{
									drs[i]["操作结论"] = "跳闸状态字正确!!";

								}
							}
						}

						if (i == 3)
						{
							uiSendCorrentCount++;
							output("累计测试" + uiSendCount.ToString() + "轮," + "成功" + uiSendCorrentCount.ToString() + "轮", true, false);
						}
					}
					#endregion

				}

				Thread.Sleep(1000);

			}
			catch (Exception e)
			{
				Console.WriteLine(e);
			}
		}

		#region 停止测试
		public delegate void dStopTrippiing(object objIndex);
		dStopTrippiing dstMessgage;

		private object m_objectTrippiing = new object();
		public void FnStopTrippiingHandler(object objIndex)
		{
			try
			{
				if (dstMessgage == null)
				{
					dstMessgage = new dStopTrippiing(StopTrippiing);
				}
				Invoke(dstMessgage, new object[] { objIndex });
			}
			catch
			{

			}
		}

		/// <summary>
		/// btnTrippiing
		/// </summary>
		/// <param name="objIndex"></param>
		public void StopTrippiing(object objIndex)
		{
			lock (m_objectTrippiing)
			{
				txbTripping.Text = "报文测试";
				SpMeter.Working = false;
				bTripTest = false;
				txbInputHWDCTest.Text = "开始测试";
			}
		}
		#endregion
		/// <summary>
		/// 读取Tripping
		/// </summary>
		private void ReadTrippingData()
		{
			#region ReadData
			dgvCommon.AutoGenerateColumns = false;
			dtTripDataTable = new DataTable(null);
			dtTripDataTable.Columns.Add("选择", typeof(bool));
			dtTripDataTable.Columns.Add("Name", typeof(string));
			dtTripDataTable.Columns.Add("DataMark", typeof(string));
			dtTripDataTable.Columns.Add("DataLen", typeof(string));
			dtTripDataTable.Columns.Add("DataBlockType", typeof(string));
			dtTripDataTable.Columns.Add("DataFormat", typeof(string));
			dtTripDataTable.Columns.Add("DecimalPlaces", typeof(string));
			dtTripDataTable.Columns.Add("DataUnit", typeof(string));
			dtTripDataTable.Columns.Add("读取值", typeof(string));
			dtTripDataTable.Columns.Add("操作结论", typeof(string));
			List<string> lsList = new List<string>();
			{
				lsList.Add("TRUE-检查第一回路电表运行状态字为合闸状态-04000503-2--BCD-0---");
				lsList.Add("TRUE-检查第二回路电表运行状态字为合闸状态-04000509-2--BCD-0---");
				lsList.Add("TRUE-检查第一回路电表运行状态字为跳闸状态-04000503-2--BCD-0---");
				lsList.Add("TRUE-检查第二回路电表运行状态字为跳闸状态-04000509-2--BCD-0---");
			}

			foreach (var VARIABLE in lsList)
			{
				object[] oRow = VARIABLE.Split('-');
				dtTripDataTable.Rows.Add(oRow);
			}

			dgvTripping.DataSource = dtTripDataTable;
			#endregion
		}

		private async Task HandleClickAsync()
		{
			// Can use ConfigureAwait here.
			await Task.Delay(1000).ConfigureAwait(continueOnCapturedContext: false);
		}

		private async void txBReadSensor_Click(object sender, EventArgs e)
		{
			int iCount, iCountAll, iCircle = 0;

			if (!int.TryParse(tb_ModulNum.Text, out iCountAll))
			{
				return;
			}
			DateTime dtTimeRead = DateTime.ParseExact(dtp_LoadTime.Text.ToString(), "yy-MM-dd HH:mm", System.Globalization.CultureInfo.CurrentCulture);
			string dataMark = "06011900";
			int iLenData = 0;//字节数
			int iPartLen = 0;
			string strData = "", strErr = "";
			lstrDataMark.Clear();
			lstrDataMark.Add(dataMark);
			dtLoadRecord = new DataTable();
			dgv_LoadCurve.DataSource = dtLoadRecord;
			string timeStr = dtTimeRead.ToString("yyMMddHHmm");
			while (dtLoadRecord.Rows.Count > 0)
			{
				dgv_LoadCurve.Invoke(new Action(() => dtLoadRecord.Rows.RemoveAt(0)));
			}
			//Colums
			dtLoadRecord.Columns.Add(new DataColumn("序号"));
			dtLoadRecord.Columns.Add(new DataColumn("曲线时标"));
			dtLoadRecord.Columns.Add(new DataColumn("参量类型编码M+数据内容"));
			//dtLoadRecord.Columns.Add("当前回复历史块数");
			//for (int i = 0; i < 5; i++)
			//{
			//	dtLoadRecord.Columns.Add("参量类型-编码" + (i + 1).ToString("X2"));
			//	dtLoadRecord.Columns.Add("数据内容-长度" + (i + 1).ToString("X2"));
			//	dtLoadRecord.Columns.Add("数据内容" + (i + 1).ToString("X2"));
			//}
			DataRow drData = dtLoadRecord.NewRow();
			for (int i = 0; i < iCountAll; i++)
			{
				timeStr = dtTimeRead.ToString("yyMMddHHmm");
				strData = "01" + Helps.fnAgainst(timeStr);
				strErr = FunDealMessage(strData, DLT645.ControlType.ReadLoad, lstrDataMark, out strData);
				drData = dtLoadRecord.NewRow();
				drData["序号"] = (i + 1).ToString();
				dtTimeRead = dtTimeRead.AddMinutes(15);
				if (strErr == "")
				{
					if (strData == "")
					{
						drData["曲线时标"] = "没有满足条件的负荷记录!";
						dtLoadRecord.Rows.Add(drData);
						continue;
					}
					else
					{
						strData = strData.Substring(2);
						DateTime dtTimeReadRec = DateTime.ParseExact(Helps.fnAgainst(strData.Substring(0, 12)), "yyMMddHHmmss", System.Globalization.CultureInfo.CurrentCulture);
						strData = strData.Substring(12);

						string strTime = dtTimeReadRec.ToString("yy-MM-dd HH:mm:ss");
						drData["曲线时标"] = strTime;
						drData["参量类型编码M+数据内容"] = strData;


						//while (strData != "")
						//{
						//	string strPart1 = strData.Substring(0, 2);
						//	string strPart2 = strData.Substring(2, 2);
						//	int iLen = int.Parse(strPart2);
						//	string strPart3 = strData.Substring(4, iLen * 2);
						//	drData["参量类型-编码" + strPart1] = strPart1;
						//	drData["数据内容-长度" + strPart1] = strPart2;
						//	drData["数据内容" + strPart1] = strPart3;
						//	strData = strData.Substring(4 + iLen * 2);
						//}

						dtLoadRecord.Rows.Add(drData);
					}
				}
				else
				{
					drData["曲线时标"] = strErr;
					dtLoadRecord.Rows.Add(drData);
					continue;
				}
			}
			dgv_LoadCurve.DataSource = dtLoadRecord;
			dgv_LoadCurve.Refresh();
		}

		private void txbParity_Click(object sender, EventArgs e)
		{
			string strAT = "AT+PARITY=" + txcbParity.Text.ToString();
			byte[] sendBytes = Helps.Str2AsciibBytes(strAT);
			byte[] endBytes = { 0x0D, 0x0A };
			sendBytes = sendBytes.Concat(endBytes).ToArray();
			byte[] recBytes = new byte[0];
			output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
			output("TX:" + strAT, false, true);
			RecBuf = null;
			var vBool = SendDataBLE(sendBytes);
			//int iCount = 0;
			//while (RecBuf == null)
			//{
			//	iCount++;
			//	if (iCount > SpMeter.OverTime / 21)
			//	{
			//		break;
			//	}
			//	Thread.Sleep(20);
			//}

			if (RecBuf == null)
			{
				return;
			}
			recBytes = new byte[RecBuf.Length];
			Array.Copy(RecBuf, 0, recBytes, 0, RecBuf.Length);
			System.Text.ASCIIEncoding asciiEncoding = new System.Text.ASCIIEncoding();
			string dataResultStr = Helps.ByteToStringWithSpace(RecBuf);
			dataResultStr = Helps.ASCII2Str(dataResultStr);
			output("TX:" + dataResultStr, false, true);
		}

		private void txcbChipType_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (txcbChipType.Text.ToString() == "RN7326")
			{
				txcbCommunicationmodule.Visible = true;
			}
			else
			{
				txcbCommunicationmodule.Visible = false;
			}
		}

		private void txbSetBLE485_Click(object sender, EventArgs e)
		{
			string strAT = "AT+BAUD=" + txtbBaudBLE.Text.ToString();
			byte[] sendBytes = Helps.Str2AsciibBytes(strAT);
			byte[] endBytes = { 0x0D, 0x0A };
			sendBytes = sendBytes.Concat(endBytes).ToArray();
			byte[] recBytes = new byte[0];
			output("TX:" + Helps.ByteToStringWithSpace(sendBytes), false, true);
			output("TX:" + strAT, false, true);
			RecBuf = null;
			var vBool = SendDataBLE(sendBytes);
			//int iCount = 0;
			//while (RecBuf == null)
			//{
			//	iCount++;
			//	if (iCount > SpMeter.OverTime / 21)
			//	{
			//		break;
			//	}
			//	Thread.Sleep(20);
			//}

			if (RecBuf == null)
			{
				return;
			}
			recBytes = new byte[RecBuf.Length];
			Array.Copy(RecBuf, 0, recBytes, 0, RecBuf.Length);
			System.Text.ASCIIEncoding asciiEncoding = new System.Text.ASCIIEncoding();
			string dataResultStr = Helps.ByteToStringWithSpace(RecBuf);
			dataResultStr = Helps.ASCII2Str(dataResultStr);
			output("TX:" + dataResultStr, false, true);
		}

		/// <summary>
		/// 报文测试线程
		/// </summary>
		private void tTestExcelMessage()
		{
			try
			{
				DataTable adtData = new DataTable();

                for (int j = 0; j < i64MessageTestCount && bMessageTestExcel; j++)
                {
                    int colunmnIndex = 0;
                    adtData.Rows.Add();
                    for (int i = 0; i < dgvReadDataExcel.Rows.Count && bMessageTestExcel; i++)
                    {
                        if ((bool)dgvReadDataExcel.Rows[i].Cells[0].Value == true)
                        {
                            
                            string colName = dgvReadDataExcel.Rows[i].Cells[6].Value?.ToString() ?? $"Col{i}";
                            if (!adtData.Columns.Contains(colName))
							{
								adtData.Columns.Add(colName, typeof(string));
							}

                            // 处理数据
                            lstrDataMark.Clear();
                            if (!string.IsNullOrEmpty(dgvReadDataExcel.Rows[i].Cells[7].Value?.ToString()))
                                lstrDataMark.Add(dgvReadDataExcel.Rows[i].Cells[7].Value.ToString());
                            if (!string.IsNullOrEmpty(dgvReadDataExcel.Rows[i].Cells[8].Value?.ToString()))
                                lstrDataMark.Add(dgvReadDataExcel.Rows[i].Cells[8].Value.ToString());

                            strErr = FunDealMessage("", DLT645.ControlType.Read, lstrDataMark, out string strData);

                            if (strErr == "")
                            {
                                strData = Helps.fnAgainst(strData);
                                dgvReadDataExcel.Rows[i].Cells[1].Value = strData;
                                dgvReadDataExcel.Rows[i].Cells[3].Value = "成功";
                                adtData.Rows[j][colunmnIndex] = strData;
                                colunmnIndex++;
                            }
                            else
                            {
                                adtData.Rows[j][i] = strErr;
                                dgvReadDataExcel.Rows[i].Cells[3].Value = "失败!";
                            }
                            fnWait(Int64.Parse(pmc[10].Value.ToString()));
                        }

                        
                    }
				}

				StopTestExcelHandler(null);
				AsposeExcelTools.DataTableToExcel(adtData, string.Format("{0}", System.Environment.CurrentDirectory) + string.Format("\\Log\\{0:yyyyMMdd HHmmss}" + ".xlsx", DateTime.Now), out string strErrs);
			}
			catch (Exception e)
			{
				//output(e.Message.ToString(), true, false, true);
				Console.WriteLine(e);
				//throw;
				StopTestExcelHandler(null);
			}
		}

		#region 停止
		public delegate void dStopTestExcel(object objIndex);
		dStopTestExcel dstStopTestExcel;
		public void StopTestExcelHandler(object objIndex)
		{
			try
			{
				if (dstStopTestExcel == null)
				{
					dstStopTestExcel = new dStopTestExcel(StopTestExcel);
				}
				Invoke(dstStopTestExcel, new object[] { objIndex });
			}
			catch
			{

			}
		}

		/// <summary>
		/// 停止
		/// </summary>
		/// <param name="objIndex"></param>
		public void StopTestExcel(object objIndex)
		{
			bMessageTestExcel = false;
			txbReadDataExcel.Text = "开始测试";
		}
		#endregion


		private void txbReadDataExcel_Click(object sender, EventArgs e)
		{
			try
			{
				i64MessageTestCount = long.Parse(txtbReadDataExcel.Text);
				if (txbReadDataExcel.Text == "开始测试")
				{
					if (btn_电表通信串口.Text == "打开串口")
					{
						MessageBox.Show("请先打开串口!");
						return;
					}
					txbReadDataExcel.Text = "停止测试";
					bMessageTestExcel = true;
					#region Thread
					Thread td = new Thread(new ThreadStart(tTestExcelMessage));
					td.IsBackground = true;
					td.Start();
					//InitChart();
					//this.timerTest.Start();
					#endregion
				}
				else
				{
					//this.timerTest.Stop();
					StopTestExcel(null);
				}
			}
			catch (Exception exception)
			{
				output("测试过程中出现错误：" + exception.Message, true, true);
				bMessageTestExcel = false;
				txbReadDataExcel.Text = "开始测试";
			}
			finally
			{
			}
		}

		private void txcbPortSource_SelectedIndexChanged(object sender, EventArgs e)
		{

		}

		private void ppg_基本参数_Click(object sender, EventArgs e)
		{

		}

		private void cbApp_CheckedChanged(object sender, EventArgs e)
		{

		}

		private void txbInputHWDCAddr_Click(object sender, EventArgs e)
		{
			OpenFileDialog openFileDialog = new OpenFileDialog();
			openFileDialog.InitialDirectory = ".\\";
			openFileDialog.Filter = "可导入文件类型|*.xlsx;*.xls";
			openFileDialog.FilterIndex = 0;
			openFileDialog.RestoreDirectory = false;
			string errStr = "";
			dtHWDC = new DataTable(null);
			dtHWDC.Columns.Add("选择", typeof(bool));
			if (openFileDialog.ShowDialog() == DialogResult.OK)
			{
				if (!AsposeExcelTools.ExcelSheetToDataTable(openFileDialog.FileName, "地址", 0, 0, out dtHWDC, out errStr))
				{
					MessageBox.Show("导入地址失败：" + errStr);
					return;
				}
				dgvHWDC.DataSource = dtHWDC;
			}
			//for (int i = 0; i < dtHWDC.Rows.Count; i++)
			//{
			//    DataGridViewCell aa = dgvHWDC.Rows[i].Cells[8];
			//    aa.Style.BackColor = Color.LightCyan;
			//}
		}

		private void txbOutput_Click(object sender, EventArgs e)
		{
			SaveFileDialog saveFileDialog1 = new SaveFileDialog();
			saveFileDialog1.Filter = "Excel文件（*.xlsx）|*.xlsx|Excel文件（*.xls）|*.xls";
			saveFileDialog1.FilterIndex = 0;
			saveFileDialog1.RestoreDirectory = false;
			string errStr = "";
			if (saveFileDialog1.ShowDialog() == DialogResult.OK)
			{
				List<DataTable> dts = new List<DataTable>();
				dts.Add(dtHWDC);

				if (!AsposeExcelTools.DataTableListToExcel(dts, saveFileDialog1.FileName, new string[] { "数据" }, out errStr))
				{
					MessageBox.Show("导出数据失败：" + errStr);
					return;
				}
				MessageBox.Show("导出成功!");
			}
		}

		private void txbInputHWDCTest_Click(object sender, EventArgs e)
		{
			if (btn_电表通信串口.Text == "打开串口")
			{
				MessageBox.Show("请先打开串口!");
				return;
			}

			try
			{
				if (txbInputHWDCTest.Text == "开始测试")
				{
					if (!int.TryParse(pmc[5].Value.ToString(), out int overTime))
					{
						MessageBox.Show("非法的等待超时时间！");
						return;
					}
					SpMeter.OverTime = overTime;
					drs = dtHWDC.Select(string.Format("选择 = '{0}' ", "true"), "");
					//iTeleindicationTestCount = int.Parse(txtbTeleindicationNum.Text);
					SpMeter.Working = true;
					bTripTest = true;
					txbInputHWDCTest.Text = "停止测试";
					uiSendCount = 0;
					uiSendCorrentCount = 0;
					fHighTemperatureI = float.Parse(txtbHighTemperatureI.Text.ToString());
					fHighTemperatureU = float.Parse(txtbHighTemperatureU.Text.ToString());
					fEffectiveValueJudgeI = float.Parse(txtxbEffectiveValueJudgeI.Text.ToString());
					fEffectiveValueJudgeU = float.Parse(txtxbEffectiveValueJudgeU.Text.ToString());
					#region Thread
					Thread td = new Thread(new ThreadStart(tTestHWDC));
					td.IsBackground = true;
					td.Start();
					#endregion
				}
				else
				{
					SpMeter.Working = false;
					bTripTest = false;
					txbInputHWDCTest.Text = "开始测试";
				}
			}
			catch (Exception exception)
			{
				output("测试过程中出现错误：" + exception.Message, true, true);
				SpMeter.Working = false;
				bTripTest = false;
				txbInputHWDCTest.Text = "开始测试";
			}
			finally
			{

			}
		}

		private void tTestHWDC()
		{
			try
			{
				for (int i = 0; i < dtHWDC.Rows.Count && bTripTest; i++)
				{
					bool bPass = true;
					if (dtHWDC.Rows[i]["选择"].ToString() == "false" || dtHWDC.Rows[i]["选择"].ToString() == "")
					{
						continue;
					}
					_dlt645 = _dlt645 = new DLT645(dtHWDC.Rows[i]["地址"].ToString());
					Thread.Sleep(1500);
					lstrDataMark.Clear();
					lstrDataMark.Add("DBDF0201");
					lstrDataMark.Add("04DFDFDB");
					string strInData = "01";
					strInData = Helps.fnAgainst(strInData);
					string strRes = FunDealMessage(strInData, DLT645.ControlType.Write, lstrDataMark, out string strData, dtHWDC.Rows[i]["地址"].ToString());
					if (strRes != "")
					{
						dtHWDC.Rows[i]["操作结论"] = strRes;
						continue;
					}
					lstrDataMark.Clear();
					lstrDataMark.Add("DBDF1310");
					lstrDataMark.Add("04DFDFDB");

					strRes = FunDealMessage("", DLT645.ControlType.Read, lstrDataMark, out strData, dtHWDC.Rows[i]["地址"].ToString());
					if (strRes != "")
					{
						dtHWDC.Rows[i]["操作结论"] = strRes;
						continue;
					}

					string strHighTemperature = Helps.fnAgainst(strData.Substring(36, 8));
					UInt32 x = Convert.ToUInt32((strHighTemperature), 16);//字符串转16进制32位无符号整数
					float fy = BitConverter.ToSingle(BitConverter.GetBytes(x), 0);//IEEE754 字节转换float//得出数字 3236.725
					dtHWDC.Rows[i]["电流高温偏置斜率"] = fy.ToString();
					if (fy > fHighTemperatureI)
					{
						dtHWDC.Rows[i]["操作结论"] = "电流高温偏置斜率 超差！";
						bPass = false;
						//continue;
					}

					strHighTemperature = Helps.fnAgainst(strData.Substring(44, 8));
					x = Convert.ToUInt32((strHighTemperature), 16);//字符串转16进制32位无符号整数
					fy = BitConverter.ToSingle(BitConverter.GetBytes(x), 0);//IEEE754 字节转换float//得出数字 3236.725
					dtHWDC.Rows[i]["电压高温偏置斜率"] = fy.ToString();
					if (fy > fHighTemperatureU)
					{
						dtHWDC.Rows[i]["操作结论"] = "电压高温偏置斜率 超差！";
						bPass = false;
						//continue;
					}
					lstrDataMark.Clear();
					lstrDataMark.Add("DBDF0601");
					lstrDataMark.Add("04DFDFDB");
					UInt64 dataInt = 0;
					UInt64 dataIntSum = 0;
					decimal dData = 0;
					for (int j = 0; j < 5; j++)
					{
						FunDealMessage("06000300", DLT645.ControlType.ReadFactory, lstrDataMark, out strData, dtHWDC.Rows[i]["地址"].ToString());

						strData = Helps.fnAgainst(strData);

						if (!UInt64.TryParse(strData, NumberStyles.AllowHexSpecifier, null, out dataInt))
						{

						}
						dataIntSum += dataInt;
					}
					dData = (decimal)dataIntSum / 5;
					dtHWDC.Rows[i]["电流有效值寄存器"] = dData.ToString();
					dataIntSum = 0;
					for (int j = 0; j < 5; j++)
					{
						FunDealMessage("08000300", DLT645.ControlType.ReadFactory, lstrDataMark, out strData, dtHWDC.Rows[i]["地址"].ToString());
						strData = Helps.fnAgainst(strData);
						if (!UInt64.TryParse(strData, NumberStyles.AllowHexSpecifier, null, out dataInt))
						{

						}
						dataIntSum += dataInt;
					}
					dData = (decimal)dataIntSum / 5;
					dtHWDC.Rows[i]["电压有效值寄存器"] = dData.ToString();
					Int64 dataInt64Sum = 0;
					Int64 dataInt64 = 0;
					for (int j = 0; j < 5; j++)
					{
						FunDealMessage("03000300", DLT645.ControlType.ReadFactory, lstrDataMark, out strData, dtHWDC.Rows[i]["地址"].ToString());
						strData = Helps.fnAgainst(strData);
						// 转换为字节数组
						byte[] bytes = new byte[3];
						bytes[0] = Convert.ToByte(strData.Substring(0, 2), 16);  // FC
						bytes[1] = Convert.ToByte(strData.Substring(2, 2), 16);  // 70
						bytes[2] = Convert.ToByte(strData.Substring(4, 2), 16);  // F4

						// 按大端序组合成整数 (注意字节顺序)
						int value = (bytes[0] << 16) | (bytes[1] << 8) | bytes[2];

						// 处理符号位 (最高位为1表示负数)
						if ((value & 0x800000) != 0)
						{
							// 如果是负数，进行二补数转换
							value = value - 0x1000000;
						}
						dataInt64Sum += value;
					}
					dData = (decimal)dataInt64Sum / 5;
					dtHWDC.Rows[i]["电流均值寄存器"] = dData.ToString();
					dataInt64Sum = 0;
					for (int j = 0; j < 5; j++)
					{
						FunDealMessage("04000300", DLT645.ControlType.ReadFactory, lstrDataMark, out strData, dtHWDC.Rows[i]["地址"].ToString());
						strData = Helps.fnAgainst(strData);
						// 转换为字节数组
						byte[] bytes = new byte[3];
						bytes[0] = Convert.ToByte(strData.Substring(0, 2), 16);  // FC
						bytes[1] = Convert.ToByte(strData.Substring(2, 2), 16);  // 70
						bytes[2] = Convert.ToByte(strData.Substring(4, 2), 16);  // F4

						// 按大端序组合成整数 (注意字节顺序)
						int value = (bytes[0] << 16) | (bytes[1] << 8) | bytes[2];

						// 处理符号位 (最高位为1表示负数)
						if ((value & 0x800000) != 0)
						{
							// 如果是负数，进行二补数转换
							value = value - 0x1000000;
						}
						dataInt64Sum += value;
					}
					dData = (decimal)dataInt64Sum / 5;
					dtHWDC.Rows[i]["电压均值寄存器"] = dData.ToString();
					lstrDataMark.Clear();
					lstrDataMark.Add("DBDF0400");
					lstrDataMark.Add("04DFDFDB");
					FunDealMessage("E83900200400", DLT645.ControlType.ReadFactory, lstrDataMark, out strData, dtHWDC.Rows[i]["地址"].ToString());
					strHighTemperature = Helps.fnAgainst(strData);
					if (!UInt64.TryParse(strHighTemperature, NumberStyles.AllowHexSpecifier, null, out dataInt))
					{

					}
					dtHWDC.Rows[i]["电流增益"] = dataInt.ToString();
					dData = decimal.Parse(dtHWDC.Rows[i]["电流有效值寄存器"].ToString()) * 10 / decimal.Parse(dataInt.ToString());
					dtHWDC.Rows[i]["电流有效值"] = dData.ToString();
					if (dData > (decimal)fEffectiveValueJudgeI)
					{
						dtHWDC.Rows[i]["操作结论"] = "电流有效值 超差！";
						bPass = false;
						//continue;
					}

					dData = Math.Abs(decimal.Parse(dtHWDC.Rows[i]["电流均值寄存器"].ToString()) * 10 / decimal.Parse(dataInt.ToString()));
					dtHWDC.Rows[i]["电流均值"] = dData.ToString();
					if (dData > (decimal)fEffectiveValueJudgeI)
					{
						dtHWDC.Rows[i]["操作结论"] = "电流均值 超差！";
						bPass = false;
						//continue;
					}

					FunDealMessage("E43900200400", DLT645.ControlType.ReadFactory, lstrDataMark, out strData, dtHWDC.Rows[i]["地址"].ToString());
					strHighTemperature = Helps.fnAgainst(strData);
					if (!UInt64.TryParse(strHighTemperature, NumberStyles.AllowHexSpecifier, null, out dataInt))
					{

					}
					dtHWDC.Rows[i]["电压增益"] = dataInt.ToString();
					dData = decimal.Parse(dtHWDC.Rows[i]["电压有效值寄存器"].ToString()) * 10 / decimal.Parse(dataInt.ToString());
					dtHWDC.Rows[i]["电压有效值"] = dData.ToString();
					if (dData > (decimal)fEffectiveValueJudgeU)
					{
						dtHWDC.Rows[i]["操作结论"] = "电压有效值 超差！";
						bPass = false;
						//continue;
					}

					dData = Math.Abs(decimal.Parse(dtHWDC.Rows[i]["电压均值寄存器"].ToString()) * 10 / decimal.Parse(dataInt.ToString()));
					dtHWDC.Rows[i]["电压均值"] = dData.ToString();
					if (dData > (decimal)fEffectiveValueJudgeU)
					{
						dtHWDC.Rows[i]["操作结论"] = "电压均值 超差！";
						bPass = false;
						//continue;
					}

					//strHighTemperature = Helps.fnAgainst(strData.Substring(24, 8));
					//if (!UInt64.TryParse(strHighTemperature, NumberStyles.AllowHexSpecifier, null, out dataInt))
					//{

					//}
					//dtHWDC.Rows[i]["电压增益"] = dataInt64.ToString();

					//dData = decimal.Parse(dtHWDC.Rows[i]["电压有效值寄存器"].ToString()) * 10 / decimal.Parse(dataInt64.ToString());
					//dtHWDC.Rows[i]["电压有效值"] = dData.ToString();
					//if (dData > (decimal)fEffectiveValueJudgeU)
					//{
					//    dtHWDC.Rows[i]["操作结论"] = "电压有效值 超差！";
					//    continue;
					//}
					if (bPass)
					{
						dtHWDC.Rows[i]["操作结论"] = "合格";
					}

				}
				FnStopTrippiingHandler(1);
			}
			catch (Exception e)
			{
				FnStopTrippiingHandler(1);
				//Console.WriteLine(e);
				//throw;
			}
		}

		private void contextMenuStripReadPara_Opening(object sender, CancelEventArgs e)
		{

		}

		private void tsmiSystemEvent_Click(object sender, EventArgs e)
		{
			FormReadMeterSystemEvent frmseFrom = new FormReadMeterSystemEvent(Helps.fnAgainst(pmc[6].Value.ToString())) { StartPosition = FormStartPosition.CenterScreen };
			frmseFrom.Show(this);
		}
	}
	public struct LogInfo
	{
		public bool DisplayBaowen;
		public string Name;
		public DateTime DateTime;
		public bool IsSend;
		public byte[] BaoWen;
		public bool Warn;
		public bool End;
	}
}
