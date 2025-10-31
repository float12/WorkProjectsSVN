using System;
using System.Runtime.InteropServices;
using System.Text;

namespace WindowsTest
{
	class Hscom
	{
		#region 调用hscom.dll函数

		/// <summary>
		/// 误差读取
		/// </summary>
		/// <param name="MError">误差读取返回误差数据</param>
		/// <param name="Meter_No">挂表位,1..96</param>
		/// <param name="Dev_Port">装置通讯口 如：COM1 则为1,COM2 为 2</param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool Error_Read(ref StringBuilder MError, int Meter_No, int Dev_Port);

		/// <summary>
		/// 负载点调整(可任意设定,四线状态)
		/// </summary>
		/// <param name="Phase"></param>
		/// <param name="Rated_Fraq"></param>
		/// <param name="Volt1"></param>
		/// <param name="Volt2"></param>
		/// <param name="Volt3"></param>
		/// <param name="Curr1"></param>
		/// <param name="Curr2"></param>
		/// <param name="Curr3"></param>
		/// <param name="Uab"></param>
		/// <param name="Uac"></param>
		/// <param name="Ang1"></param>
		/// <param name="Ang2"></param>
		/// <param name="Ang3"></param>
		/// <param name="SModel"></param>
		/// <param name="Dev_port"></param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool Adjust_CUST(int Phase, double Rated_Fraq, double Volt1, double Volt2, double Volt3, double Curr1, double Curr2, double Curr3, double Uab, double Uac, double Ang1, double Ang2, double Ang3, string SModel, int Dev_port);

		/// <summary>
		/// 负载点调整(可以直接赋角度值)
		/// </summary>
		/// <param name="Phase"></param>
		/// <param name="Rated_Fraq"></param>
		/// <param name="PhaseSequence"></param>
		/// <param name="Revers"></param>
		/// <param name="Volt"></param>
		/// <param name="Curr"></param>
		/// <param name="IABC"></param>
		/// <param name="COSP"></param>
		/// <param name="SModel"></param>
		/// <param name="Dev_Port">装置通讯口 如：COM1 则为1,COM2 为 2</param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool Adjust_UI3(int Phase, double Rated_Fraq, int PhaseSequence, int Revers, double Volt,
			double Curr, string IABC, string COSP, string SModel, int Dev_Port);

		/// <summary>
		/// 负载点调整(三相电压电流可分别设置)
		/// </summary>
		/// <param name="Phase"></param>
		/// <param name="Rated_Volt"></param>
		/// <param name="Rated_Curr"></param>
		/// <param name="Rated_Fraq"></param>
		/// <param name="PhaseSequence"></param>
		/// <param name="Revers"></param>
		/// <param name="Volt_Per1"></param>
		/// <param name="Volt_Per2"></param>
		/// <param name="Volt_Per3"></param>
		/// <param name="Curr_Per1"></param>
		/// <param name="Curr_Per2"></param>
		/// <param name="Curr_Per3"></param>
		/// <param name="IABC"></param>
		/// <param name="COSP"></param>
		/// <param name="SModel"></param>
		/// <param name="Dev_Port">装置通讯口 如：COM1 则为1,COM2 为 2</param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool Adjust_UI4(int Phase, double Rated_Volt, double Rated_Curr, double Rated_Fraq, int PhaseSequence, int Revers, double Volt_Per1, double Volt_Per2, double Volt_Per3, double Curr_Per1, double Curr_Per2, double Curr_Per3, string IABC, double COSP, string SModel, int Dev_Port);

		/// <summary>
		/// 负载点调整
		/// </summary>
		/// <param name="Phase"></param>
		/// <param name="Rated_Volt"></param>
		/// <param name="Rated_Curr"></param>
		/// <param name="Rated_Freq"></param>
		/// <param name="PhaseSequence"></param>
		/// <param name="Revers"></param>
		/// <param name="Volt_Per"></param>
		/// <param name="Curr_Per"></param>
		/// <param name="IABC"></param>
		/// <param name="CosP"></param>
		/// <param name="SModel"></param>
		/// <param name="Dev_Port">装置通讯口 如：COM1 则为1,COM2 为 2</param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool Adjust_UI(int Phase, double Rated_Volt, double Rated_Curr, double Rated_Freq, int PhaseSequence, int Revers, double Volt_Per, double Curr_Per, string IABC, string CosP, string SModel, int Dev_Port);

		/// <summary>
		/// 485 通道切换(只单个通道吸合)
		/// </summary>
		/// <param name="Meter_No">挂表位,1..96</param>
		/// <param name="Open_Flag"></param>
		/// <param name="Dev_Port">装置通讯口 如：COM1 则为1,COM2 为 2</param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool Set_485_Channel(int Meter_No, int Open_Flag, int Dev_Port);

		/// <summary>
		/// 485 通道切换(可多个通道吸合)
		/// </summary>
		/// <param name="Meter_No">挂表位,1..96</param>
		/// <param name="Open_Flag"></param>
		/// <param name="Dev_Port">装置通讯口 如：COM1 则为1,COM2 为 2</param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool Set_485_ChannelS(int Meter_No, int Open_Flag, int Dev_Port);

		/// <summary>
		/// 指示仪表读取
		/// </summary>
		/// <param name="SDataint"></param>
		/// <param name="SModel"></param>
		/// <param name="Dev_Port">装置通讯口 如：COM1 则为1,COM2 为 2</param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool StdMeter_Read(ref StringBuilder SDataint, string SModel, int Dev_Port);

		/// <summary>
		/// 打开 485 通讯口
		/// </summary>
		/// <param name="LngHdcComm"></param>
		/// <param name="BaudRate"></param>
		/// <param name="Meter_Port"></param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool Open485CommPort(ref IntPtr LngHdcComm, StringBuilder BaudRate, byte Meter_Port);

		/// <summary>
		/// 发送 485 数据(16 进制)
		/// </summary>
		/// <param name="LngHdcComm"></param>
		/// <param name="MeterData"></param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool Send485Data(IntPtr LngHdcComm, string MeterData);

		/// <summary>
		/// 接收 485 数据(16 进制)
		/// </summary>
		/// <param name="LngHdcComm"></param>
		/// <param name="MeterDatarecint"></param>
		/// <param name="recv_len"></param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool Recv485Data(IntPtr LngHdcComm, out IntPtr MeterDatarecint, int recv_len);

		/// <summary>
		/// 关闭 485 通讯口
		/// </summary>
		/// <param name="LngHdcComm"></param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool Close485CommPort(IntPtr LngHdcComm);

		/// <summary>
		/// 脉冲采样通道切换
		/// </summary>
		/// <param name="Meter_No">挂表位,1..96</param>
		/// <param name="Channel_Flag"></param>
		/// <param name="Dev_Port">装置通讯口 如：COM1 则为1,COM2 为 2</param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool Set_Pulse_Channel(int Meter_No, int Channel_Flag, int Dev_Port);

		/// <summary>
		/// 开始测试误差
		/// </summary>
		/// <param name="Meter_No">挂表位,1..96</param>
		/// <param name="Constant"></param>
		/// <param name="Pulse"></param>
		/// <param name="Dev_Port">装置通讯口 如：COM1 则为1,COM2 为 2</param>
		/// <returns></returns>
		//[DllImport("hscom.dll", CallingConvention = CallingConvention.Cdecl)]
		[DllImport("hscom.dll")]
		public static extern bool Error_Start(int Meter_No, double Constant, int Pulse, int Dev_Port);

		/// <summary>
		/// 降电压电流
		/// </summary>
		/// <param name="Dev_Port">装置通讯口 如：COM1 则为1,COM2 为 2</param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool Power_Off(int Dev_Port);

		/// <summary>
		/// 小电流修正方式
		/// </summary>
		/// <param name="ReviseMode"></param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool Set_ReviseMode(int ReviseMode);

		/// <summary>
		/// 源修正等待时间(不调用时默认 5s)
		/// </summary>
		/// <param name="ReviseTime"></param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool Set_ReviseTime(double ReviseTime);

		/// <summary>
		/// 误差仪总清
		/// </summary>
		/// <param name="Dev_Port">装置通讯口 如：COM1 则为1,COM2 为 2</param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool Error_Clear(int Dev_Port);

		/// <summary>
		/// 关闭与装置通讯的串口
		/// </summary>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool Dll_Port_Close();

		/// <summary>
		/// 开始潜动或起动
		/// </summary>
		/// <param name="Meter_No">挂表位,1..96</param>
		/// <param name="Dev_Port">装置通讯口 如：COM1 则为1,COM2 为 2</param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool CRPSTA_start(int Meter_No, int Dev_Port);

		/// <summary>
		/// 潜动或起动脉冲读取
		/// </summary>
		/// <param name="Meter_No">挂表位,1..96</param>
		/// <param name="Dev_Port">装置通讯口 如：COM1 则为1,COM2 为 2</param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool CRPSTA_Result(int Meter_No, int Dev_Port);

		/// <summary>
		/// 潜动或起动状态复位
		/// </summary>
		/// <param name="Meter_No">挂表位,1..96</param>
		/// <param name="Dev_Port">装置通讯口 如：COM1 则为1,COM2 为 2</param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool CRPSTA_clear(int Meter_No, int Dev_Port);

		/// <summary>
		/// 开始常数校核
		/// </summary>
		/// <param name="Meter_No">挂表位,1..96</param>
		/// <param name="Dev_Port">装置通讯口 如：COM1 则为1,COM2 为 2</param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool ConstTest_Start(int Meter_No, int Dev_Port);

		/// <summary>
		/// 读取常数校核电量
		/// </summary>
		/// <param name="Meter_No">挂表位,1..96</param>
		/// <param name="Dev_Port">装置通讯口 如：COM1 则为1,COM2 为 2</param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool ConstPulse_Read(int Meter_No, int Dev_Port);

		/// <summary>
		/// V 型输出
		/// </summary>
		/// <param name="Vout"></param>
		/// <returns></returns>
		[DllImport("hscom.dll", EntryPoint = "Set_OutPutMode")]
		private static extern Boolean Set_OutPutMode(bool Vout);

		/// <summary>
		/// 自动短接复位
		/// </summary>
		/// <param name="Meter_No">挂表位,1..96</param>
		/// <param name="Dev_Port">装置通讯口 如：COM1 则为1,COM2 为 2</param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool AutoSub_Reset(int Meter_No, int Dev_Port);

		/// <summary>
		/// 获取当前标准表常数
		/// </summary>
		/// <returns></returns>
		[DllImport("hscom.dll", EntryPoint = "GetStdConst")]
		private static extern double GetStdConst();
		#endregion

		#region 调用动态库函数时需要设置的参数
		/// <summary>
		/// 相线  0 单相 1 三相四线有功 2 三相三线有功 3 90度无功 4 60度无功 5 四线正弦无功 6 三线正弦无功 7 单相无功
		/// </summary>
		public static int Phase = 0;
		/// <summary>
		/// 装置通讯口 即232口 1代表COM1
		/// </summary>
		public static int Dev_Port = 1;
		/// <summary>
		/// 装置配置的标准表型号
		/// </summary>
		public static string SModel = "HS5100H";
		/// <summary>
		/// 被校表额定电压 如220V则为220
		/// </summary>
		public static double Rated_Volt = 220;
		/// <summary>
		/// 被校表额定电流 如1.5(6)A则为1.5
		/// </summary>
		public static double Rated_Curr = 1.5;
		/// <summary>
		/// 被校表额定频率，范围45~65
		/// </summary>
		public static double Rated_Freq = 50;
		/// <summary>
		/// 相序 0-正相序 1-逆相序
		/// </summary>
		public static int PhaseSequence = 0;
		/// <summary>
		/// 电流方向 0-正相 1-反相
		/// </summary>
		public static int Revers = 0;
		/// <summary>
		/// 被校表常数
		/// </summary>
		public static double Constant = 1200;
		/// <summary>
		/// 小电流修正方式   0--不修正 1--正常修正 2--精确修正(带扩展CT装置) 3--精确修正(不带扩展CT的非便携式装置)
		/// </summary>
		public static int ReviseMode = 3;
		#endregion
		/// <summary>
		/// 控制动态库函数失败重发次数
		/// </summary>
		public static int RepeatHscomCount = 3;

		#region 中转软件用到的
		//public static double COSP = 0;
		//public static double Volt_Per = 100;
		//public static double Curr_Per = 100;
		//public static string IABC = "H";
		//public static double Volt_PerB = 100;
		//public static double Curr_PerB = 100;
		//public static double Volt_PerC = 100;
		//public static double Curr_PerC = 100;
		#endregion

		private static readonly object _lockErrorRead = 0;

		/// <summary>
		/// 设置动态库用到的基本参数
		/// </summary>
		/// <param name="phase">相线  0 单相 1 三相四线有功 2 三相三线有功 3 90度无功 4 60度无功 5 四线正弦无功 6 三线正弦无功 7 单相无功</param>
		/// <param name="dev_Port">装置通讯口 即232口 1代表COM1</param>
		/// <param name="sModel">装置配置的标准表型号</param>
		/// <param name="rated_Volt">被校表额定电压 如220V则为220</param>
		/// <param name="rated_Curr"> 被校表额定电流 如1.5(6)A则为1.5</param>
		/// <param name="rated_Freq">被校表额定频率，范围45~65</param>
		/// <param name="phaseSequence">相序 0-正相序 1-逆相序</param>
		/// <param name="revers"> 电流方向 0-正相 1-反相</param>
		/// <param name="constant">被校表常数</param>
		/// <param name="reviseMode">小电流修正方式   0--不修正 1--正常修正 2--精确修正(带扩展CT装置) 3--精确修正(不带扩展CT的非便携式装置)</param>
		/// <param name="repeatHscomCount"></param>
		/// <returns></returns>
		public static bool SetParameter(int phase, int dev_Port, string sModel, double rated_Volt, double rated_Curr,
			double rated_Freq, int phaseSequence, int revers, double constant, int reviseMode, int repeatHscomCount)
		{
			Phase = phase;
			Dev_Port = dev_Port;
			SModel = sModel;
			Rated_Volt = rated_Volt;
			Rated_Curr = rated_Curr;
			Rated_Freq = rated_Freq;
			PhaseSequence = phaseSequence;
			Revers = revers;
			Constant = constant;
			ReviseMode = reviseMode;
			RepeatHscomCount = repeatHscomCount;
			return true;
		}

		/// <summary>
		/// 关闭与装置通讯的串口
		/// </summary>
		/// <param name="strRet">系统调用动态库失败返回的错误信息</param>
		/// <returns></returns>
		public static bool fnDll_Port_Close(out string strRet)
		{
			bool flag = false;
			strRet = "关闭与装置通讯的串口失败";
			try
			{
				for (int i = 0; i < RepeatHscomCount + 1; i++)
				{
					flag = Dll_Port_Close();
					if (flag)
						break;
					System.Threading.Thread.Sleep(100);
				}
			}
			catch (Exception exception)
			{
				strRet += "：" + exception.Message;
			}
			return flag;
		}

		/// <summary>
		/// 负载点调整(三相电压电流可分别设置)
		/// </summary>
		/// <param name="Volt_Per">负载点A相电压百分数 100表示100%</param>
		/// <param name="Volt_PerB">负载点B相电压百分数 100表示100%</param>
		/// <param name="Volt_PerC">负载点C相电压百分数 100表示100%</param>
		/// <param name="Curr_Per">负载点A相电流百分数 100表示100%</param>
		/// <param name="Curr_PerB">负载点B相电流百分数 100表示100%</param>
		/// <param name="Curr_PerC">负载点C相电流百分数 100表示100%</param>
		/// <param name="IABC">负载点合分元 H-合元 A-分A B-分B C-分C</param>
		/// <param name="COSP">功因对应的角度，如0,30,60</param>
		/// <param name="strRet">系统调用动态库失败返回的错误信息</param>
		/// <returns></returns>
		public static bool fnAdjust_UI4(double Volt_Per, double Volt_PerB, double Volt_PerC, double Curr_Per, double Curr_PerB, double Curr_PerC, string IABC, double COSP, out string strRet)
		{
			bool flag = false;
			strRet = "负载点调整失败";
			try
			{
				if (!fnSet_ReviseMode(out strRet)) //设置小电流修正方式
				{
					return flag;
				}
				for (int i = 0; i < RepeatHscomCount + 1; i++)
				{
					flag = Adjust_UI4(Phase, Math.Round(Rated_Volt, 1, MidpointRounding.AwayFromZero), Rated_Curr, Rated_Freq, PhaseSequence, Revers, Volt_Per, Volt_PerB, Volt_PerC, Curr_Per, Curr_PerB, Curr_PerC, IABC, COSP, SModel, Dev_Port);
					if (flag)
						break;
					System.Threading.Thread.Sleep(100);
				}
			}
			catch (Exception exception)
			{
				strRet += "：" + exception.Message;
			}
			return flag;
		}

		/// <summary>
		/// 开始测试误差
		/// </summary>
		/// <param name="Meter_No">挂表位,1..96挂表位,1..96</param>
		/// <param name="Pulse">校验时采样圈数</param>
		/// <param name="strRet">系统调用动态库失败返回的错误信息</param>
		/// <returns></returns>
		public static bool fnError_Start(int Meter_No, int Pulse, out string strRet)
		{
			bool flag = false;
			strRet = "开始测试误差失败";
			try
			{
				for (int i = 0; i < RepeatHscomCount + 1; i++)
				{
					flag = Error_Start(Meter_No, Constant, Pulse, Dev_Port);
					if (flag)
						break;
					System.Threading.Thread.Sleep(100);
				}
			}
			catch (Exception exception)
			{
				strRet += "：" + exception.Message;
			}
			return flag;
		}

		/// <summary>
		/// 误差读取
		/// </summary>
		/// <param name="MError">误差读取返回误差数据误差读取返回误差数据</param>
		/// <param name="Meter_No">挂表位,1..96挂表位,1..96</param>
		/// <param name="strRet">系统调用动态库失败返回的错误信息</param>
		/// <returns></returns>
		public static bool fnError_Read(ref string MError, int Meter_No, out string strRet)
		{
			bool flag = false;
			strRet = "【表位" + Meter_No + "】读取误差失败";
			StringBuilder mError = new StringBuilder();
			try
			{
				lock (_lockErrorRead)
				{
					for (int i = 0; i < RepeatHscomCount + 1; i++)
					{
						flag = Error_Read(ref mError, Meter_No, Dev_Port);
						if (!flag)
						{
							System.Threading.Thread.Sleep(100);
							continue;
						}
						MError = mError.ToString(); break;
					}
				}
			}
			catch (Exception exception)
			{
				strRet += "：" + exception.Message;
			}
			return flag;
		}

		/// <summary>
		/// 误差仪总清
		/// </summary>
		/// <param name="strRet"></param>
		/// <returns></returns>
		public static bool fnError_Clear(out string strRet)
		{
			bool flag = false;
			strRet = "误差仪总清失败";
			try
			{
				for (int i = 0; i < RepeatHscomCount + 1; i++)
				{
					flag = Error_Clear(Dev_Port);
					if (flag)
						break;
					System.Threading.Thread.Sleep(100);
				}
			}
			catch (Exception exception)
			{
				strRet += "：" + exception.Message;
			}
			return flag;
		}

		/// <summary>
		/// 485通道切换（可多个通道吸合）
		/// </summary>
		/// <param name="Meter_No">挂表位,1..96挂表位,1..96</param>
		/// <param name="Open_Flag">485通道切换状态 1-吸合 0-断开</param>
		/// <param name="strRet">系统调用动态库失败返回的错误信息</param>
		/// <returns></returns>
		public static bool fnSet_485_ChannelS(int Meter_No, int Open_Flag, out string strRet)
		{
			bool flag = false;
			string state = "";
			state = Open_Flag == 0 ? "断开" : "吸合";
			strRet = "【表位" + Meter_No + "】485多通道" + state + "失败";
			try
			{
				for (int i = 0; i < RepeatHscomCount + 1; i++)
				{
					flag = Set_485_ChannelS(Meter_No, Open_Flag, Dev_Port);
					if (flag)
						break;
					System.Threading.Thread.Sleep(100);
				}
			}
			catch (Exception exception)
			{
				strRet += "：" + exception.Message;
			}
			return flag;
		}

		/// <summary>
		/// 485通道切换（只单个通道吸合）
		/// </summary>
		/// <param name="Meter_No">挂表位,1..96</param>
		/// <param name="Open_Flag"></param>
		/// <param name="strRet"></param>
		/// <returns></returns>
		public static bool fnSet_485_Channel(int Meter_No, int Open_Flag, out string strRet)
		{
			bool flag = false;
			string state = "";
			state = Open_Flag == 0 ? "断开" : "吸合";
			strRet = "【表位" + Meter_No + "】485单通道" + state + "失败";
			try
			{
				for (int i = 0; i < RepeatHscomCount + 1; i++)
				{
					flag = Set_485_Channel(Meter_No, Open_Flag, Dev_Port);
					if (flag)
						break;
					System.Threading.Thread.Sleep(100);
				}
			}
			catch (Exception exception)
			{
				strRet += "：" + exception.Message;
			}
			return flag;
		}

		private static readonly object LockStdMeterRead = 0;
		/// <summary>
		/// 指示仪表读取
		/// </summary>
		/// <param name="SData">指示仪表返回数据</param>
		/// <param name="strRet">系统调用动态库失败返回的错误信息</param>
		/// <returns></returns>
		public static bool fnStdMeter_Read(ref string SData, out string strRet)
		{
			bool flag = false;
			strRet = "读取指示仪表失败";
			StringBuilder sData = new StringBuilder(512);
			try
			{
				lock (LockStdMeterRead)
				{
					for (int i = 0; i < RepeatHscomCount + 1; i++)
					{
						flag = StdMeter_Read(ref sData, SModel, Dev_Port);
						if (flag)
							break;
						System.Threading.Thread.Sleep(100);
					}
					if (flag)
						SData = sData.ToString();
				}
			}
			catch (Exception exception)
			{
				strRet += "：" + exception.Message;
			}
			return flag;
		}

		/// <summary>
		/// 脉冲采样通道切换
		/// </summary>
		/// <param name="Meter_No">挂表位,1..96挂表位,1..96</param>
		/// <param name="Channel_Flag">脉冲采样通道 0-有功正向/反向,1-有功反向,2-无功正向/反向,3-无功反向,4-时钟,5-投切,6-需量.单相表误差脉冲通道一般发0即可，正反向都在同一个端子输出。</param>
		/// <param name="strRet">系统调用动态库失败返回的错误信息</param>
		/// <returns></returns>
		public static bool fnSet_Pulse_Channel(int Meter_No, int Channel_Flag, out string strRet)
		{
			bool flag = false;
			strRet = "脉冲采样通道切换失败";
			try
			{
				for (int i = 0; i < RepeatHscomCount + 1; i++)
				{
					flag = Set_Pulse_Channel(Meter_No, Channel_Flag, Dev_Port);
					if (flag)
						break;
					System.Threading.Thread.Sleep(100);
				}
			}
			catch (Exception exception)
			{
				strRet += "：" + exception.Message;
			}
			return flag;
		}

		/// <summary>
		/// 降电压电流
		/// </summary>
		/// <param name="strRet">系统调用动态库失败返回的错误信息</param>
		/// <returns></returns>
		public static bool fnPower_Off(out string strRet)
		{
			bool flag = false;
			strRet = "降电压电流失败";
			try
			{
				for (int i = 0; i < RepeatHscomCount + 1; i++)
				{
					flag = Power_Off(Dev_Port);
					if (flag)
						break;
					System.Threading.Thread.Sleep(100);
				}
			}
			catch (Exception exception)
			{
				strRet += "：" + exception.Message;
			}
			return flag;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="Dev_Port"></param>
		/// <returns></returns>
		[DllImport("hscom.dll")]
		public static extern bool Power_Pause(int Dev_Port);

		/// <summary>
		/// 设置小电流修正方式
		/// </summary>
		/// <param name="strRet">系统调用动态库失败返回的错误信息</param>
		/// <returns></returns>
		public static bool fnSet_ReviseMode(out string strRet)
		{
			strRet = "设置小电流修正方式失败";
			bool flag = false;
			try
			{
				for (int i = 0; i < RepeatHscomCount + 1; i++)
				{
					flag = Set_ReviseMode(ReviseMode);
					if (flag)
						break;
				}
			}
			catch (Exception exception)
			{
				strRet += "：" + exception.Message;
			}
			return flag;
		}
	}
}
