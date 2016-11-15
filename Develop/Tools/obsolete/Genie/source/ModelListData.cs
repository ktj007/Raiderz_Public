using System;
using System.Collections;
using System.IO;
using System.Xml;
using System.Windows.Forms;


namespace Genie
{
	/// <summary>
	/// ModelListData에 대한 요약 설명입니다.
	/// </summary>
	public class ModelListData
	{
		private Hashtable	m_Models = new Hashtable();

		public ModelListData()
		{

		}

		public void Load(string szFileName)
		{
			m_Models.Clear();

			try
			{
				XmlTextReader xtr = new XmlTextReader(szFileName);
				while(xtr.Read())
				{
					if ((xtr.NodeType == XmlNodeType.Element) && 
						((xtr.LocalName.ToLower() == XML_TOKEN.TAG_ADD_XML.ToLower()) || 
						(xtr.LocalName.ToLower() == XML_TOKEN.TAG_ADD_ELU.ToLower())) )
					{
						ModelData.FileType file_type;
						if (xtr.LocalName.ToLower() == XML_TOKEN.TAG_ADD_XML.ToLower()) file_type = ModelData.FileType.FT_XML;
						else file_type = ModelData.FileType.FT_ELU;

						if (xtr.AttributeCount>0)
						{
							string name="";
							string filename="";

							for (int i = 0; i < xtr.AttributeCount; i++)
							{
								xtr.MoveToAttribute(i);

								if (xtr.Name.ToLower() == XML_TOKEN.ATTR_NAME.ToLower())
								{
									name = xtr.Value;
								}
								else if (xtr.Name.ToLower() == XML_TOKEN.ATTR_FILENAME.ToLower())
								{
									filename = xtr.Value;
								}
							}

							if ((name != "") && (filename!=""))
							{
								ModelData new_data = new ModelData();
								new_data.m_szName = name;
								new_data.m_szPath = filename;
								new_data.m_nFileType = file_type;
								new_data.SetupCategory();
								



								new_data.Load();		// xml을 읽는다.

								m_Models.Add(name, new_data);
							}
						}
						xtr.Read();

					}               
				}//while
				xtr.Close();
			}
			catch(Exception e)
			//catch(FileNotFoundException e)
			{

			}
		}


		public void Serialize(bool bToForm, System.Windows.Forms.TreeView treeview)
		{
			// 트리구성
			treeview.Nodes.Clear();

			TreeNode playerNode = treeview.Nodes.Add(ConvertString.GetModelCategoryStr(ModelCategory.MC_PLAYER));
			TreeNode monsterNode = treeview.Nodes.Add(ConvertString.GetModelCategoryStr(ModelCategory.MC_MONSTER));
			TreeNode npcNode = treeview.Nodes.Add(ConvertString.GetModelCategoryStr(ModelCategory.MC_NPC));
			TreeNode weaponNode = treeview.Nodes.Add(ConvertString.GetModelCategoryStr(ModelCategory.MC_WEAPON));
			TreeNode sfxNode = treeview.Nodes.Add(ConvertString.GetModelCategoryStr(ModelCategory.MC_SFX));
			TreeNode MapObjectNode = treeview.Nodes.Add(ConvertString.GetModelCategoryStr(ModelCategory.MC_MAP_OBJECT));

			IDictionaryEnumerator htEnum = m_Models.GetEnumerator();
			while(htEnum.MoveNext())
			{
				ModelData data = (ModelData)htEnum.Value;

				int image_index = 1;
				if (data.Edited) image_index = 2;

				if (data.m_nCategory == ModelCategory.MC_PLAYER)
				{
					TreeNode new_node = new TreeNode(data.m_szName, image_index, image_index);
					playerNode.Nodes.Add(new_node);
				}
				else if (data.m_nCategory == ModelCategory.MC_MONSTER)
				{
					TreeNode new_node = new TreeNode(data.m_szName, image_index, image_index);
					monsterNode.Nodes.Add(new_node);
				}
				else if (data.m_nCategory == ModelCategory.MC_NPC)
				{
					TreeNode new_node = new TreeNode(data.m_szName, image_index, image_index);
					npcNode.Nodes.Add(new_node);
				}
				else if (data.m_nCategory == ModelCategory.MC_WEAPON)
				{
					TreeNode new_node = new TreeNode(data.m_szName, image_index, image_index);
					weaponNode.Nodes.Add(new_node);
				}
				else if (data.m_nCategory == ModelCategory.MC_SFX)
				{
					TreeNode new_node = new TreeNode(data.m_szName, image_index, image_index);
					sfxNode.Nodes.Add(new_node);
				}
				else if (data.m_nCategory == ModelCategory.MC_MAP_OBJECT)
				{
					TreeNode new_node = new TreeNode(data.m_szName, image_index, image_index);
					MapObjectNode.Nodes.Add(new_node);
				}

			}
		}

		public ModelData FindModelData(string szName)
		{
			ModelData node = (ModelData)m_Models[szName];
			return node;
		}
	}
}
