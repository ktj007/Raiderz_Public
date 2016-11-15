using System;
using System.Collections;
using System.IO;
using System.Xml;
using System.Windows.Forms;
using System.Diagnostics;
using System.Text;

namespace Genie
{
	public class BaseModel
	{
		public string szName;
		public string szFileName;
		public BaseModel()
		{
			szName="";szFileName="";
		}
	}

	/// <summary>
	/// ModelData에 대한 요약 설명입니다.
	/// </summary>
	public class ModelData
	{
		public enum FileType		{ FT_XML, FT_ELU };

		private bool				m_bEdited;
		public string				m_szName;
		public string				m_szPath;
		public FileType				m_nFileType;
		public	ModelCategory		m_nCategory;
		public BaseModel			m_BaseModel = new BaseModel();
		public ArrayList			m_Animations = new ArrayList();
		public ArrayList			m_WeaponAnimationNames = new ArrayList();

		public ModelData()
		{
			m_bEdited = false;
			m_nFileType = FileType.FT_XML;
		}

		public void SetupCategory()
		{
			m_nCategory = ModelCategory.MC_PLAYER;

			string str = m_szPath.ToLower();
			if (str.IndexOf("player/") > 0)
			{
				m_nCategory = ModelCategory.MC_PLAYER;
			}
			else if (str.IndexOf("monster/") > 0)
			{
				m_nCategory = ModelCategory.MC_MONSTER;
			}
			else if (str.IndexOf("npc/") > 0)
			{
				m_nCategory = ModelCategory.MC_NPC;
			}
			else if (str.IndexOf("weapons/") > 0)
			{
				m_nCategory = ModelCategory.MC_WEAPON;
			}
			else if (str.IndexOf("sfx/") > 0)
			{
				m_nCategory = ModelCategory.MC_SFX;
			}
			else if (str.IndexOf("mapobject/") > 0)
			{
				m_nCategory = ModelCategory.MC_MAP_OBJECT;
			}

		}

		public void Load()
		{
			if (m_nFileType != FileType.FT_XML) return;

			try
			{
				string path = Global.szRootPath + m_szPath;

				int len = path.Length;
				if (path.Substring(len-3, 3).ToLower() != XML_TOKEN.TAG_XML.ToLower()) return;

				XmlTextReader xtr = new XmlTextReader(path);
				while(xtr.Read())
				{
					if (xtr.NodeType == XmlNodeType.Element) 
					{
						if (xtr.LocalName.ToLower() == XML_TOKEN.TAG_ADD_BASE_MODEL.ToLower())
						{
							string name="";
							string filename="";

							if (xtr.AttributeCount>0)
							{
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
									m_BaseModel.szName = name;
									m_BaseModel.szFileName = filename;
								}
							}
						}
						else if (xtr.LocalName.ToLower() == XML_TOKEN.TAG_ADD_ANIMATION.ToLower())
						{
							string name="";
							string filename="";
							string motion_loop_type="";

							if (xtr.AttributeCount>0)
							{
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
									else if (xtr.Name.ToLower() == XML_TOKEN.ATTR_MOTION_LOOP_TYPE.ToLower())
									{
										motion_loop_type = xtr.Value;
									}
								}

								if ((name != "") && (filename!=""))
								{

									ModelAnimation new_ani = new ModelAnimation();
									new_ani.InitFromXml(name, filename, motion_loop_type);


									m_Animations.Add(new_ani);
								}
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

			
			SetupWeaponAniNames();
			m_bEdited=false;
		}


		public void Save()
		{
			if (m_nFileType != FileType.FT_XML) return;

			try
			{
				string path = Global.szRootPath + m_szPath;
				Encoding encoding = Encoding.UTF8;

				XmlTextWriter tw = new XmlTextWriter(path, encoding);
				tw.Formatting = Formatting.Indented;
				tw.WriteStartDocument();
				tw.WriteStartElement(XML_TOKEN.TAG_XML);

				// AddBaseModel
				tw.WriteComment(" \n    BaseModel \n  ");
				tw.WriteStartElement(XML_TOKEN.TAG_ADD_BASE_MODEL);
				tw.WriteAttributeString(XML_TOKEN.ATTR_NAME, m_BaseModel.szName);
				tw.WriteAttributeString(XML_TOKEN.ATTR_FILENAME, m_BaseModel.szFileName);
				tw.WriteEndElement();		// XML_TOKEN.TAG_ADD_BASE_MODEL

				tw.WriteComment(" \n    Normal Animation \n  ");
				// AddAnimation (normal)
				for (int i = 0; i < m_Animations.Count; i++)
				{
					ModelAnimation ani = (ModelAnimation)m_Animations[i];
					if (ani.nWeaponType == WeaponType.WT_NONE)
					{
						tw.WriteStartElement(XML_TOKEN.TAG_ADD_ANIMATION);
						tw.WriteAttributeString(XML_TOKEN.ATTR_NAME, ani.RawName);
						tw.WriteAttributeString(XML_TOKEN.ATTR_FILENAME, ani.szFileName);
						tw.WriteAttributeString(XML_TOKEN.ATTR_MOTION_LOOP_TYPE, ConvertString.GetAnimationLoopTypeStr(ani.nLoopType));
						tw.WriteEndElement();	// XML_TOKEN.TAG_ADD_ANIMATION
					}
				}

				// AddAnimation (weapon)
				tw.WriteComment(" \n    Weapon Animation \n  ");
				for (int i = 0; i < m_Animations.Count; i++)
				{
					ModelAnimation ani = (ModelAnimation)m_Animations[i];
					if (ani.nWeaponType != WeaponType.WT_NONE)
					{
						tw.WriteStartElement(XML_TOKEN.TAG_ADD_ANIMATION);
						tw.WriteAttributeString(XML_TOKEN.ATTR_NAME, ani.RawName);
						tw.WriteAttributeString(XML_TOKEN.ATTR_FILENAME, ani.szFileName);
						tw.WriteAttributeString(XML_TOKEN.ATTR_MOTION_LOOP_TYPE, ConvertString.GetAnimationLoopTypeStr(ani.nLoopType));
						tw.WriteEndElement();	// XML_TOKEN.TAG_ADD_ANIMATION
					}
				}

				tw.WriteEndElement(); // XML_TOKEN.TAG_XML
				tw.Flush();
				tw.Close();
			}
			catch(FileNotFoundException e)
			{

			}

			m_bEdited=false;
		}

		private void SetupWeaponAniNames()
		{
			m_WeaponAnimationNames.Clear();

			for (int i = 0; i < m_Animations.Count; i++)
			{
				ModelAnimation ani = (ModelAnimation)m_Animations[i];

				if (ani.nWeaponType != WeaponType.WT_NONE)
				{
					if (!ExistWeaponAniName(ani.Name))
					{
						m_WeaponAnimationNames.Add(ani.Name);
					}
				}
			}

		}

		private bool ExistWeaponAniName(string szName)
		{
			for (int i = 0; i < m_WeaponAnimationNames.Count; i++)
			{
				string str = (string)m_WeaponAnimationNames[i];
				if (str.Equals(szName)) return true;
			}

			return false;
		}


		public ModelAnimation FindAnimation(WeaponType nWeaponType, string szName)
		{
			string szRawName = ConvertString.GetRawAnimationName(nWeaponType, szName);

			ModelAnimation ani = null;
			for (int i = 0; i < m_Animations.Count; i++)
			{
				ani = (ModelAnimation)m_Animations[i];

				if (ani.RawName == szRawName) return ani;
			}
			return null;
		}

		public bool EditAnimation(WeaponType nWeaponType, string szOldName, string szName, string szFileName, AnimationLoopType nLoopType)
		{
			ModelAnimation ani = FindAnimation(nWeaponType, szOldName);
			if (ani == null) return false;

			if ((szOldName == szName) && (szFileName == ani.szFileName) && (nLoopType == ani.nLoopType)) return false;

			ani.Set(nWeaponType, szName, szFileName, nLoopType);

			m_bEdited = true;
			return true;
		}

		public bool InsertAnimation(WeaponType nWeaponType, string szName, string szFileName, AnimationLoopType nLoopType)
		{
			ModelAnimation ani = FindAnimation(nWeaponType, szName);
			if (ani != null) return false;

			ani = new ModelAnimation();
			ani.Set(nWeaponType, szName, szFileName, nLoopType);
			m_Animations.Add(ani);

			if (nWeaponType != WeaponType.WT_NONE) SetupWeaponAniNames();
			m_bEdited = true;
			return true;
		}

		public bool RemoveAnimation(WeaponType nWeaponType, string szName)
		{
			string szRawName = ConvertString.GetRawAnimationName(nWeaponType, szName);

			ModelAnimation ani = null;
			for (int i = 0; i < m_Animations.Count; i++)
			{
				ani = (ModelAnimation)m_Animations[i];

				if (ani.RawName == szRawName)
				{
					m_Animations.RemoveAt(i);

					SetupWeaponAniNames();		// 다시 m_WeaponAnimationNames를 재설정한다.
					m_bEdited=true;
					return true;
				}
			}

		return false;
		}

		public bool Edited
		{
			get { return m_bEdited; }
		}
	}
}
