using System;
using System.Collections;
using System.IO;
using System.Xml;
using System.Windows.Forms;

namespace Genie
{
	/// <summary>
	/// IniData에 대한 요약 설명입니다.
	/// </summary>
	public class IniData
	{
		private Hashtable	m_ConstItems = new Hashtable();

		public IniData()
		{

		}

		public void Init()
		{
			m_ConstItems.Clear();

			// const 구조 읽기 ----
			try
			{
				string sKey;

				XmlTextReader xtr = new XmlTextReader(Application.StartupPath + @"\" + Global.szIniFileName);
				while(xtr.Read())
				{
					if(xtr.NodeType == XmlNodeType.Element)
					{
						if (xtr.LocalName == "CONST_ITEM")
						{
							sKey = xtr.LocalName;

							ConstIniNode new_node = new ConstIniNode();

							if (xtr.AttributeCount>0)
							{
								for (int i = 0; i < xtr.AttributeCount; i++)
								{
									xtr.MoveToAttribute(i);

									if (xtr.Name == "key")
									{
										new_node.szKey = xtr.Value;
									}
									else if (xtr.Name == "type")
									{
										if (xtr.Value == "int") new_node.nValueType = ValueType.VT_INTEGER;
										else if (xtr.Value == "float") new_node.nValueType = ValueType.VT_FLOAT;
										else if (xtr.Value == "str") new_node.nValueType = ValueType.VT_STRING;
										else if (xtr.Value == "path") new_node.nValueType = ValueType.VT_PATH;
										else if (xtr.Value == "bool") new_node.nValueType = ValueType.VT_BOOLEAN;
										else if (xtr.Value == "vec") new_node.nValueType = ValueType.VT_VECTOR;
										else if (xtr.Value == "color") new_node.nValueType = ValueType.VT_COLOR;
									}
									else if (xtr.Name == "default")
									{
										new_node.szDefaultValue = xtr.Value;
									}
									else if (xtr.Name == "desc")
									{
										new_node.szDesc = xtr.Value;
									}
								}

								m_ConstItems.Add(new_node.szKey, new_node);
							}
						}
						
						xtr.Read();
					}               
				}//while
				xtr.Close();
			}
			catch(FileNotFoundException e)
			{

			}
		}

		public ValueType GetValueTypeFromConstKey(string szKey)
		{
			ConstIniNode node = (ConstIniNode)m_ConstItems[szKey];
			if (node != null)
			{
				return node.nValueType;
			}
			return ValueType.VT_STRING;
		}
		public string GetStringFromConstKey(string szKey)
		{
			ConstIniNode node = (ConstIniNode)m_ConstItems[szKey];
			if (node != null)
			{
				return node.szDesc;
			}
			return "";
		}			

		public Hashtable ConstItems
		{
			get { return m_ConstItems; }
		}
	}

	public class ConstIniNode
	{
		public string		szKey;
		public ValueType	nValueType;
		public string		szDefaultValue;
		public string		szDesc;
		public ConstIniNode()
		{
			szKey = "";
			nValueType = ValueType.VT_STRING;
			szDefaultValue = "";
			szDesc = "";
		}
	}
}
