using System;
using System.ComponentModel;
using System.Windows.Forms;

namespace WindowsTest
{
	public class Property
	{
		private string _name = string.Empty;
		private object _value = null;
		private bool _readonly = false;
		private bool _visible = true;
		private string _category = string.Empty;
		TypeConverter _converter = null;
		object _editor = null;
		private string _displayname = string.Empty;
		public winMain _winMain = null;
		public Property(string sName, object sValue)
		{
			this._name = sName;
			this._value = sValue;
		}

		public Property(string sName, object sValue, bool sReadonly, bool sVisible)
		{
			this._name = sName;
			this._value = sValue;
			this._readonly = sReadonly;
			this._visible = sVisible;
		}

		public string Name  //获得属性名   
		{
			get
			{
				return _name;
			}
			set
			{
				_name = value;
			}
		}
		public string DisplayName   //属性显示名称   
		{
			get
			{
				return _displayname;
			}
			set
			{
				_displayname = value;
			}
		}
		public TypeConverter Converter  //类型转换器，我们在制作下拉列表时需要用到   
		{
			get
			{
				return _converter;
			}
			set
			{
				_converter = value;
			}
		}
		public string Category  //属性所属类别   
		{
			get
			{
				return _category;
			}
			set
			{
				_category = value;
			}
		}
		public object Value  //属性值   
		{
			get
			{
				return _value;
			}
			set
			{
				_value = value;
			}
		}
		public bool ReadOnly  //是否为只读属性   
		{
			get
			{
				return _readonly;
			}
			set
			{
				_readonly = value;
			}
		}
		public bool Visible  //是否可见   
		{
			get
			{
				return _visible;
			}
			set
			{
				_visible = value;
			}
		}
		public virtual object Editor   //属性编辑器   
		{
			get
			{
				return _editor;
			}
			set
			{
				_editor = value;
			}
		}

		private string[] _PortNames = null;

		//[EditorAttribute(typeof(PropertyGridAddr), typeof(System.Drawing.Design.UITypeEditor))]
		public string[] PortNames
		{
			get
			{
				return _PortNames;
			}
			set
			{
				_PortNames = value;
			}
		}


	}

	[DefaultProperty("Name")]
	public class CustomPropertyDescriptor : PropertyDescriptor
	{
		Property m_Property;
		public CustomPropertyDescriptor(ref Property myProperty, Attribute[] attrs)
			: base(myProperty.Name, attrs)
		{
			m_Property = myProperty;
		}
		#region PropertyDescriptor 重写方法   
		public override bool CanResetValue(object component)
		{
			return false;
		}
		public override Type ComponentType
		{
			get
			{
				return null;
			}
		}
		public override object GetValue(object component)
		{
			return m_Property.Value;
		}

		public override string Description
		{
			get
			{
				return m_Property.Name;
			}
		}
		public override string Category
		{
			get
			{
				return m_Property.Category;
			}
		}
		public override string DisplayName
		{
			get
			{
				return m_Property.DisplayName != "" ? m_Property.DisplayName : m_Property.Name;
			}
		}
		public override bool IsReadOnly
		{
			get
			{
				return m_Property.ReadOnly;
			}
		}
		public override void ResetValue(object component)
		{
			//Have to implement   
		}
		public override bool ShouldSerializeValue(object component)
		{
			return false;
		}
		IniClass ini = new IniClass(Application.StartupPath + @"\Config.ini");

		public override void SetValue(object component, object value)
		{
			if (value.ToString().Contains(".xlsx"))
			{
				string strErr = "";

				if (!AsposeExcelTools.ExcelAllSheetToDataTableList(Environment.CurrentDirectory + "\\" + value.ToString(), out winMain.DataBank, out strErr))
				{
					MessageBox.Show("加载数据库失败：" + strErr);
					return;
				}

				if (winMain.RefreshTree())
				{
					m_Property.Value = value;
					ini.IniWriteValue("Main", "CurrentDirectory", value.ToString());
				}
			}
			else
			{
				m_Property.Value = value;
			}
		}
		public override TypeConverter Converter
		{
			get
			{
				return m_Property.Converter;
			}
		}
		public override Type PropertyType
		{
			get { return m_Property.Value.GetType(); }
		}
		public override object GetEditor(Type editorBaseType)
		{
			return m_Property.Editor == null ? base.GetEditor(editorBaseType) : m_Property.Editor;
		}
		#endregion
	}
}
