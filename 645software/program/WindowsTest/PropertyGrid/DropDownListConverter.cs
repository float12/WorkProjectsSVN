using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing.Design;
using System.Globalization;
using System.Windows.Forms;
using System.Windows.Forms.Design;

namespace WindowsTest
{
	public class DropDownListConverter : StringConverter
	{
		object[] m_Objects;
		public DropDownListConverter(object[] objects)
		{
			m_Objects = objects;
		}
		public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
		{
			return true;
		}
		public override bool GetStandardValuesExclusive(ITypeDescriptorContext context)
		{
			return true;
		}
		public override System.ComponentModel.TypeConverter.StandardValuesCollection GetStandardValues(ITypeDescriptorContext context)
		{
			return new StandardValuesCollection(m_Objects);//我们可以直接在内部定义一个数组，但并不建议这样做，这样对于下拉框的灵活             //性有很大影响   
		}
	}
	public class DropDownListConverterData : StringConverter
	{
		object[] m_Objects;
		public DropDownListConverterData(object[] objects)
		{
			m_Objects = objects;
		}
		public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
		{
			return true;
		}
		public override bool GetStandardValuesExclusive(ITypeDescriptorContext context)
		{
			return true;
		}
		public override System.ComponentModel.TypeConverter.StandardValuesCollection GetStandardValues(ITypeDescriptorContext context)
		{
			return new StandardValuesCollection(m_Objects);//我们可以直接在内部定义一个数组，但并不建议这样做，这样对于下拉框的灵活             //性有很大影响   
		}
		public override object ConvertFrom(
	  ITypeDescriptorContext context,
	  CultureInfo culture,
	  object value)
		{
			if (value is string)
				return (object)(string)value;
			return value == null ? (object)"" : base.ConvertFrom(context, culture, value);
		}
	}

	public class DropDownListCom : StringConverter
	{
		public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
		{
			return true;
		}
		public override bool GetStandardValuesExclusive(ITypeDescriptorContext context)
		{
			return true;
		}

		public override System.ComponentModel.TypeConverter.StandardValuesCollection GetStandardValues(ITypeDescriptorContext context)
		{
			return new StandardValuesCollection(winMain.SpMeter.getSerials());
		}
	}

	public class PropertyGridAddr : UITypeEditor
	{

		public override UITypeEditorEditStyle GetEditStyle(System.ComponentModel.ITypeDescriptorContext context)
		{
			return UITypeEditorEditStyle.Modal;

		}
		public override object EditValue(System.ComponentModel.ITypeDescriptorContext context, System.IServiceProvider provider, object value)
		{
			IWindowsFormsEditorService edSvc = (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));
			value = "AAAAAAAAAAAA";
			if (edSvc != null)
			{
				if (!winMain.SpMeter.isOpen())
				{
					MessageBox.Show("请先打开串口！");
					return value;
				}
				if (winMain.SpMeter.Working)
				{
					MessageBox.Show("串口占用中！");
					return value;
				}
				DLT645 dlt645 = new DLT645("AAAAAAAAAAAA");
				List<string> readAddr = new List<string>();
				readAddr.Add("04000401");
				if (!dlt645.Pack(DLT645.ControlType.Read, readAddr, "", 0, out byte[] sendB, out string errStr))
				{
					MessageBox.Show(errStr);
					return value;
				}
				LogInfo logInfo = new LogInfo { BaoWen = sendB, Name = "探测通信地址", DateTime = DateTime.Now, DisplayBaowen = true, End = false, IsSend = true, Warn = false };
				byte[] recBytes = winMain.SpMeter.SendCommand(sendB, "645");
				logInfo = new LogInfo { BaoWen = recBytes, Name = null, DateTime = DateTime.Now, DisplayBaowen = true, End = false, IsSend = false, Warn = false };
				if (!dlt645.UnPack(recBytes, DLT645.ControlType.Read, readAddr, 0, out bool isHaveHouxu, out string data, out errStr))
				{
					MessageBox.Show("探测通信地址失败：" + errStr);
					return value;
				}
				if (isHaveHouxu)
				{
					MessageBox.Show("探测通信地址失败：返回报文非法");
					return value;
				}
				if (data.Length != 12)
				{
					MessageBox.Show("探测通信地址失败：返回报文非法");
					return value;
				}
				value = data;
			}
			return value;
		}
	}
}
