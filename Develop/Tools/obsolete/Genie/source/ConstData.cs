using System;
using System.Collections;
using System.IO;
using System.Xml;
using System.Text;
using System.Windows.Forms;

namespace Genie
{
	public class ConstDataNode
	{
		public string		szKey;
		public string		szValue;
		public ValueType	nValueType;
		public ConstDataNode()
		{
			szKey = "";
			szValue = "";
			nValueType = ValueType.VT_STRING;
		}

		public string GetTypeString()
		{
			switch (nValueType)
			{
				case ValueType.VT_INTEGER: return "INT";
				case ValueType.VT_FLOAT: return "FLOAT";
				case ValueType.VT_STRING: return "STR";
				case ValueType.VT_PATH: return "PATH";
				case ValueType.VT_BOOLEAN: return "BOOL";
				case ValueType.VT_VECTOR: return "VECTOR";
				case ValueType.VT_COLOR: return "COLOR";
			};

			return "";
		}
	};

	/// <summary>
	/// ConstData에 대한 요약 설명입니다.
	/// </summary>
	public class ConstData
	{
		private ArrayList	m_Nodes = new ArrayList();
		private bool		m_bEdited;

		public ConstData()
		{
			m_bEdited = false;

		}

		public void Load(string szFileName)
		{
			m_Nodes.Clear();

			try
			{
				string sKey;;
				//XmlTextReader xtr = new XmlTextReader(Application.StartupPath + @"\ini.xml");
				XmlTextReader xtr = new XmlTextReader(szFileName);
				while(xtr.Read())
				{
					if(xtr.NodeType == XmlNodeType.Element)
					{
						sKey = xtr.LocalName;

						if (xtr.AttributeCount>0)
						{
							for (int i = 0; i < xtr.AttributeCount; i++)
							{
								xtr.MoveToAttribute(i);

								if (xtr.Name.ToLower() == XML_TOKEN.ATTR_VALUE.ToLower())
								{
									ConstDataNode new_node = new ConstDataNode();
									new_node.szKey = sKey;
									new_node.szValue = xtr.Value;

									ConstIniNode ini_node = (ConstIniNode)Global.ini_data.ConstItems[sKey];
									if (ini_node != null)
									{
										new_node.nValueType = ini_node.nValueType;
									}

									m_Nodes.Add(new_node);
								}
							}
						}
						xtr.Read();

					}               
				}//while
				xtr.Close();
			}
			catch(Exception e)
			{

			}
		}

		public void Save(string szFileName)
		{
			try
			{
				
				Encoding encoding = Encoding.UTF8;

				XmlTextWriter tw = new XmlTextWriter(szFileName, encoding);
				tw.Formatting = Formatting.Indented;
				tw.WriteStartDocument();
				tw.WriteStartElement(XML_TOKEN.TAG_MAIET);
				tw.WriteStartElement(XML_TOKEN.TAG_CONST);


				foreach(ConstDataNode node in m_Nodes)
				{
					tw.WriteStartElement(node.szKey);
					tw.WriteAttributeString(XML_TOKEN.ATTR_VALUE, node.szValue);
					tw.WriteEndElement();		// node.szKey
				}

				tw.WriteEndElement(); // CONST
				tw.WriteEndElement(); // maiet
				tw.Flush();
				tw.Close();


				m_bEdited = false;
			}
			catch(Exception e)
			{
				MessageBox.Show("저장하지 못했습니다.");
			}
		}

		public void Serialize(bool bToForm, System.Windows.Forms.ListView listview)
		{
			listview.Items.Clear();

			foreach(ConstDataNode node in m_Nodes)
			{
				System.Windows.Forms.ListViewItem new_item = new System.Windows.Forms.ListViewItem();

				new_item.Text = node.szKey;
				new_item.SubItems.Add(node.szValue);
				new_item.SubItems.Add(node.GetTypeString());

				listview.Items.Add(new_item);
			}
		}

		public void SetValue(string szKey, string szValue)
		{
			foreach(ConstDataNode node in m_Nodes)
			{
				if (node.szKey == szKey)
				{
					if (node.szValue != szValue)
					{
						node.szValue = szValue;
						m_bEdited = true;
						break;
					}
				}
			}
		}

		public bool Edited
		{
			get { return m_bEdited; }
		}

	}
}
