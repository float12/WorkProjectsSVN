using System;
using System.ComponentModel;
using System.Linq;
using System.Reflection;

namespace WindowsTest
{
	public static class EnumHelper
	{

		/// <summary>
		/// 获取枚举值的Description
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="value"></param>
		/// <returns></returns>
		public static string GetDescription<T>(this T value) where T : struct
		{
			string result = value.ToString();
			Type type = typeof(T);
			FieldInfo info = type.GetField(value.ToString());
			var attributes = info.GetCustomAttributes(typeof(DescriptionAttribute), true);
			if (attributes != null && attributes.FirstOrDefault() != null)
			{
				result = (attributes.First() as DescriptionAttribute).Description;
			}

			return result;
		}

		/// <summary>
		/// 根据Description获取枚举值
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="value"></param>
		/// <returns></returns>
		public static T GetValueByDescription<T>(this string description) where T : struct
		{
			Type type = typeof(T);
			foreach (var field in type.GetFields())
			{
				if (field.Name == description)
				{
					return (T)field.GetValue(null);
				}

				var attributes = (DescriptionAttribute[])field.GetCustomAttributes(typeof(DescriptionAttribute), true);
				if (attributes != null && attributes.FirstOrDefault() != null)
				{
					if (attributes.First().Description == description)
					{
						return (T)field.GetValue(null);
					}
				}
			}

			throw new ArgumentException(string.Format("{0} 未能找到对应的枚举.", description), "Description");
		}

		/// <summary>
		/// 获取string获取枚举值
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="value"></param>
		/// <returns></returns>
		public static T GetValue<T>(this string value) where T : struct
		{
			T result;
			if (Enum.TryParse(value, true, out result))
			{
				return result;
			}

			throw new ArgumentException(string.Format("{0} 未能找到对应的枚举.", value), "Value");
		}
	}

	/// <summary>
	/// 远程控制类型
	/// </summary>
	public enum eumRemoteControlType : byte
	{
		/// <summary>
		/// 拉闸
		/// </summary>
		eumWindUp = (byte)0x1a,
		/// <summary>
		/// 合闸允许
		/// </summary>
		eumAllowWindDown = (byte)0x1b,
		/// <summary>
		/// 合闸
		/// </summary>
		eumWindDown = (byte)0x1c,
		/// <summary>
		/// 预跳闸(延时时间到直接合闸)
		/// </summary>
		eumReadyWindUpToDown = (byte)0x1d,
		/// <summary>
		/// 预跳闸(延时时间到合闸允许)
		/// </summary>
		eumReadyWindUpToAllow = (byte)0x1e,
		/// <summary>
		/// 报警
		/// </summary>
		eumAlarm = (byte)0x2a,
		/// <summary>
		/// 解除报警
		/// </summary>
		eumLiftAlarm = (byte)0x2b,
		/// <summary>
		/// 保电
		/// </summary>
		eumPaulPower = (byte)0x3a,
		/// <summary>
		/// 保电解除
		/// </summary>
		eumLiftPaulPower = (byte)0x3b,
	}
}
