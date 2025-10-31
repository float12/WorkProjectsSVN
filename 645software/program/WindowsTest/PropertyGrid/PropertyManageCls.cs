using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;

namespace WindowsTest
{
	public class PropertyManageCls : CollectionBase, ICustomTypeDescriptor
	{
		public void Add(Property value)
		{
			int flag = -1;
			if (value != null)
			{
				if (base.List.Count > 0)
				{
					//IList mList = new List();
					List<Property> mList = new List<Property>();
					for (int i = 0; i < base.List.Count; i++)
					{
						Property p = base.List[i] as Property;
						if (value.Name == p.Name)
						{
							flag = i;
						}
						mList.Add(p);
					}
					if (flag == -1)
					{
						mList.Add(value);
					}
					base.List.Clear();
					foreach (Property p in mList)
					{
						base.List.Add(p);
					}
				}
				else
				{
					base.List.Add(value);
				}
			}
		}
		public void Remove(Property value)
		{
			if (value != null && base.List.Count > 0)
				base.List.Remove(value);
		}
		public Property this[int index]
		{
			get
			{
				return (Property)base.List[index];
			}
			set
			{
				base.List[index] = (Property)value;
			}
		}
		#region ICustomTypeDescriptor 成员   
		public AttributeCollection GetAttributes()
		{
			return TypeDescriptor.GetAttributes(this, true);
		}
		public string GetClassName()
		{
			return TypeDescriptor.GetClassName(this, true);
		}
		public string GetComponentName()
		{
			return TypeDescriptor.GetComponentName(this, true);
		}
		public TypeConverter GetConverter()
		{
			return TypeDescriptor.GetConverter(this, true);
		}
		public EventDescriptor GetDefaultEvent()
		{
			return TypeDescriptor.GetDefaultEvent(this, true);
		}
		public PropertyDescriptor GetDefaultProperty()
		{
			return TypeDescriptor.GetDefaultProperty(this, true);
		}
		public object GetEditor(Type editorBaseType)
		{
			return TypeDescriptor.GetEditor(this, editorBaseType, true);
		}
		public EventDescriptorCollection GetEvents(Attribute[] attributes)
		{
			return TypeDescriptor.GetEvents(this, attributes, true);
		}
		public EventDescriptorCollection GetEvents()
		{
			return TypeDescriptor.GetEvents(this, true);
		}
		public PropertyDescriptorCollection GetProperties(Attribute[] attributes)
		{
			PropertyDescriptor[] newProps = new PropertyDescriptor[this.Count];
			for (int i = 0; i < this.Count; i++)
			{
				Property prop = (Property)this[i];
				newProps[i] = new CustomPropertyDescriptor(ref prop, attributes);
			}
			return new PropertyDescriptorCollection(newProps);
		}
		public PropertyDescriptorCollection GetProperties()
		{
			return TypeDescriptor.GetProperties(this, true);
		}
		public object GetPropertyOwner(PropertyDescriptor pd)
		{
			return this;
		}
		#endregion
	}
}
