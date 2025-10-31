using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace WindowsTest
{
	public class IniClass
	{
		//public string inipath = System.Windows.Forms.Application.StartupPath + @"\Config.ini";
		public string inipath;
		[System.Runtime.InteropServices.DllImport("kernel32")]
		private static extern bool WritePrivateProfileString(string section, string key, string val, string filePath);
		[System.Runtime.InteropServices.DllImport("kernel32")]
		private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);
		[System.Runtime.InteropServices.DllImport("kernel32")]
		private static extern uint GetPrivateProfileStringA(string section, string key, string def, Byte[] retVal, int size, string filePath);
		/// <summary>
		/// 读取所有Sections
		/// </summary>
		/// <returns></returns>
		public List<string> ReadSections()
		{
			return ReadSections(inipath);
		}

		public List<string> ReadSections(string iniFilename)
		{
			List<string> result = new List<string>();
			Byte[] buf = new Byte[65536];
			uint len = GetPrivateProfileStringA(null, null, null, buf, buf.Length, iniFilename);
			int j = 0;
			for (int i = 0; i < len; i++)
				if (buf[i] == 0)
				{
					result.Add(Encoding.Default.GetString(buf, j, i - j));
					j = i + 1;
				}
			return result;
		}

		/// <summary>
		/// 读取Section下所有key
		/// </summary>
		/// <param name="SectionName"></param>
		/// <returns></returns>
		public List<string> ReadKeys(String SectionName)
		{
			return ReadKeys(SectionName, inipath);
		}

		public List<string> ReadKeys(string SectionName, string iniFilename)
		{
			List<string> result = new List<string>();
			Byte[] buf = new Byte[65536];
			uint len = GetPrivateProfileStringA(SectionName, null, null, buf, buf.Length, iniFilename);
			int j = 0;
			for (int i = 0; i < len; i++)
				if (buf[i] == 0)
				{
					result.Add(Encoding.Default.GetString(buf, j, i - j));
					j = i + 1;
				}
			return result;
		}
		/// <summary> 
		/// 构造方法 
		/// </summary> 
		/// <param name="INIPath">文件路径</param> 
		public IniClass(string INIPath)
		{
			inipath = INIPath;
		}
		/// <summary> 
		/// 写入INI文件 
		/// </summary> 
		/// <param name="Section">项目名称(如 [TypeName] )</param> 
		/// <param name="Key">键</param> 
		/// <param name="Value">值</param> 
		public void IniWriteValue(string Section, string Key, string Value)
		{
			WritePrivateProfileString(Section, Key, Value, this.inipath);
		}
		// 删除ini文件下所有段落
		public void ClearAllSection()
		{
			FileInfo fi = new FileInfo(this.inipath);
			fi.Delete();
		}
		/// <summary> 
		/// 读出INI文件 
		/// </summary> 
		/// <param name="Section">项目名称(如 [TypeName] )</param> 
		/// <param name="Key">键</param> 
		public string IniReadValue(string Section, string Key)
		{
			StringBuilder temp = new StringBuilder(500);
			int i = GetPrivateProfileString(Section, Key, "", temp, 500, this.inipath);
			return temp.ToString();
		}

		/// <summary> 
		/// 读出INI文件 
		/// </summary> 
		/// <param name="Section">项目名称(如 [TypeName] )</param> 
		/// <param name="Key">键</param> 
		/// <param name="Default">默认值</param> 
		public string IniReadValue(string Section, string Key, string Default)
		{
			StringBuilder temp = new StringBuilder(500);
			int i = GetPrivateProfileString(Section, Key, Default, temp, 500, this.inipath);
			return temp.ToString();
		}

		/// <summary> 
		/// 验证文件是否存在 
		/// </summary> 
		/// <returns>布尔值</returns> 
		public bool ExistINIFile()
		{
			return File.Exists(inipath);
		}
	}
}
