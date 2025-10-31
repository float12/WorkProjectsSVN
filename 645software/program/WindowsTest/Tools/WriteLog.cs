using System;
using System.IO;
using System.Text;

namespace WindowsTest
{
	class WriteLog
	{
		public static bool isTesting = false;
		private static StreamWriter streamWriter; //写文件  
		public static string path = string.Format("{0}", System.Environment.CurrentDirectory) + string.Format("\\LOG\\{0:yyyyMMdd}log.txt", DateTime.Now.Date);
		public static string factoryName = "未知厂家";
		public static string itemName = "未命名的测试项";
		public static string meterNO = "";
		public static string meter型号 = "";
		public static string meter备注 = "";
		public static void Write(string message)
		{
			try
			{
				path = string.Format("{0}", System.Environment.CurrentDirectory) + string.Format("\\LOG\\{0:yyyyMMdd}log.txt", DateTime.Now.Date);
				if (!System.IO.Directory.Exists(string.Format("{0}", System.Environment.CurrentDirectory) + "\\LOG"))
				{
					System.IO.Directory.CreateDirectory(string.Format("{0}", System.Environment.CurrentDirectory) + "\\LOG"); //不存在就创建目录   
				}

				if (!File.Exists(path))
				{
					FileStream fs = new FileStream(path, FileMode.CreateNew);
					fs.Close();
				}
				streamWriter = new StreamWriter(path, true, System.Text.Encoding.Default);
				if (message != null)
				{
					streamWriter.WriteLine(message);
				}
			}
			finally
			{
				if (streamWriter != null)
				{
					streamWriter.Flush();
					streamWriter.Dispose();
					streamWriter = null;
				}
			}
		}
		public static void Write(string message, string pathChoice, bool isAppend)
		{
			try
			{
				if (!File.Exists(pathChoice))
				{
					FileStream fs = new FileStream(pathChoice, FileMode.CreateNew);
					fs.Close();
				}
				streamWriter = new StreamWriter(pathChoice, isAppend, Encoding.Default);
				if (message != null)
				{
					streamWriter.WriteLine(message);
				}
			}
			finally
			{
				if (streamWriter != null)
				{
					streamWriter.Flush();
					streamWriter.Dispose();
					streamWriter = null;
				}
			}
		}
	}
	public class fileEncode
	{
		//获得文件编码格式的类
		public static Encoding GetFileEncodeType(string filename)
		{
			System.IO.FileStream fs = new System.IO.FileStream(filename, System.IO.FileMode.Open, System.IO.FileAccess.Read);
			System.IO.BinaryReader br = new System.IO.BinaryReader(fs);
			Byte[] buffer = br.ReadBytes(2);
			br.Close();
			fs.Close();

			if (buffer[0] >= 0xEF)
			{
				if (buffer[0] == 0xEF && buffer[1] == 0xBB)
				{
					return System.Text.Encoding.UTF8;
				}
				else if (buffer[0] == 0xFE && buffer[1] == 0xFF)
				{
					return System.Text.Encoding.BigEndianUnicode;
				}
				else if (buffer[0] == 0xFF && buffer[1] == 0xFE)
				{
					return System.Text.Encoding.Unicode;

				}
				else
				{
					return System.Text.Encoding.Default;
				}
			}
			else
			{
				return System.Text.Encoding.Default;
			}
		}
	}
}
