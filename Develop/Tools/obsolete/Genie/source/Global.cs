using System;
using System.Collections;
using System.IO;
using System.Xml;
using System.Text;

namespace Genie
{
	public enum ValueType
	{
		VT_INTEGER,
		VT_FLOAT,
		VT_STRING,
		VT_PATH,
		VT_BOOLEAN,
		VT_VECTOR,
		VT_COLOR
	}

	public enum ModelCategory
	{
		MC_PLAYER = 0,
		MC_MONSTER,
		MC_NPC,
		MC_WEAPON,
		MC_SFX,
		MC_MAP_OBJECT,
		MC_MAX
	}

	public enum WeaponType
	{
		WT_NONE=0,
		WT_1H_SLASH,
		WT_2H_SLASH,
		WT_1H_BLUNT,
		WT_2H_BLUNT,
		WT_1H_PIERCE,
		WT_2H_PIERCE,
		WT_ARCHERY,
		WT_GUNPOWDER
	}

	/// <summary>
	/// Global에 대한 요약 설명입니다.
	/// </summary>
	public class Global
	{
		// 상수값
		public static string		szFeedbackUrl = "http://iworks.maiet.net/mdn/wiki.php/%BC%D2%BF%EF%C7%E5%C6%AE/Genie%C7%C7%B5%E5%B9%E9";
		public static string		szHelpUrl = "http://iworks.maiet.net/mdn/wiki.php/%BC%D2%BF%EF%C7%E5%C6%AE/Genie%B5%B5%BF%F2%B8%BB";

		public static string		szConstFileName = "const.xml";
		public static string		szIniFileName = "ini.xml";
		public static string		szOptionFileName = "option.xml";

		public static string		szRootPath;
		public static string		szConstFileNameFullPath;
		public static string		szModelListFileNameFullPath;

		public static MainForm		mainform;
		public static ConstData		const_data = new ConstData();
		public static ModelListData	modellist_data = new ModelListData();
		public static OptionData	option = new OptionData();
		public static IniData		ini_data = new IniData();


		public Global()
		{

		}

		static private void LoadOption()
		{
			try
			{
				string sKey;
				XmlTextReader xtr = new XmlTextReader(Const.szOptionFileName);
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
									if (sKey.ToUpper() == XML_TOKEN.TAG_ROOTPATH.ToUpper())
									{
										szRootPath = xtr.Value;
									}
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
		}

		static public void SaveOption()
		{
			try
			{
				Encoding encoding = Encoding.UTF8;

				XmlTextWriter tw = new XmlTextWriter(Const.szOptionFileName, encoding);
				tw.Formatting = Formatting.Indented;
				tw.WriteStartDocument();
				tw.WriteStartElement(XML_TOKEN.TAG_MAIET);
				tw.WriteStartElement(XML_TOKEN.TAG_OPTION);


				tw.WriteStartElement(XML_TOKEN.TAG_ROOTPATH);
				tw.WriteAttributeString(XML_TOKEN.ATTR_VALUE, szRootPath);
				tw.WriteEndElement();

				tw.WriteEndElement(); // OPTION
				tw.WriteEndElement(); // maiet
				tw.Flush();
				tw.Close();
			}
			catch(Exception e)
			{

			}
		}

		static public void Init(MainForm a_mainform)
		{
			mainform = a_mainform;
			szRootPath = "../../";	// test

			LoadOption();
			szRootPath = Path.GetFullPath(szRootPath);

			LoadAllData();
		}

		static public void LoadAllData()
		{
			szConstFileNameFullPath = szRootPath + "data/system/" + szConstFileName;
			szModelListFileNameFullPath = szRootPath + "data/Model/" + Const.szModelListFileName;

			// ini 로딩
			ini_data.Init();		


			// modellist 로딩
			modellist_data.Load(szModelListFileNameFullPath);
			modellist_data.Serialize(true, mainform.tvModel);


			// 상수값 로딩
			const_data.Load(szConstFileNameFullPath);
			const_data.Serialize(true, mainform.lvConst);
		}
	}

	public class Const
	{
		// 프로그램에서 사용하는 상수값
		public static float			fVersion = 0.11f;
		public static string		szTabNameModel = "모델";
		public static string		szTabNameConst = "상수값";

		public static string		szOptionFileName = "option.xml";
		public static string		szModelListFileName = "ModelList.xml";
	}

	public class XML_TOKEN
	{
		public static string		TAG_MAIET				= "maiet";
		public static string		TAG_ADD_BASE_MODEL		= "AddBaseModel";
		public static string		TAG_ADD_ANIMATION		= "AddAnimation";
		public static string		TAG_XML					= "xml";
		public static string		TAG_ADD_XML				= "AddXml";
		public static string		TAG_ADD_ELU				= "AddElu";
		public static string		TAG_CONST				= "CONST";
		public static string		TAG_OPTION				= "OPTION";
		public static string		TAG_ROOTPATH			= "ROOTPATH";

		public static string		ATTR_NAME				= "name";
		public static string		ATTR_FILENAME			= "filename";
		public static string		ATTR_MOTION_LOOP_TYPE	= "motion_loop_type";
		public static string		ATTR_VALUE				= "value";
	}
}
