#include "StdAfx.h"
#include "C_XMLResourceList.h"
#include "C_XMLConfig.h"
#include "MXml.h"

CXMLResourceList::CXMLResourceList(void)
{
}

CXMLResourceList::~CXMLResourceList(void)
{
}

void CXMLResourceList::InitResourcePath()
{
	char szfull[256]= "";
	_fullpath( szfull, CXMLConfig::GetInstance()->pcDataPath, 256 );
	std::string fullPath(szfull);

	//set zone,field list path
	m_fieldListPath_ = fullPath + "system\\fieldlist.xml";
	m_npcListPath_ = fullPath + "system\\npc.xml";
	//C:\workspace\hunt\Develop\Game\runtime\Data\Model\Monster
	m_npcModelFolderPath_ = fullPath + "model\\";

	m_zoneListPath_ = fullPath + "system\\zonelist.xml";
	m_zoneUtilsPath_ = fullPath + "zone\\Zone_Utils\\";
	m_zoneFolderPath_ = fullPath + "zone\\";
	//m_propertyListPath_ = fullPath + "system\\property.xml";
	m_propertyListPath_ = "property.xml";

	_fullpath( szfull, CXMLConfig::GetInstance()->pcTilepath, 256);
	m_tileFolderPath_ = szfull;
}

bool CXMLResourceList::LoadZoneList()
{
	m_zoneList_.clear();

	MXml kXml;
	if ( !kXml.LoadFile(m_zoneListPath_.c_str()))
		return false;

	if(kXml.Doc()->FirstChildElement("maiet") == NULL)
		return false;

	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("maiet")->FirstChildElement("ZONELIST");
	if ( pRootElement == NULL )
		return false;

	std::string sB("(zone:");
	std::string eB(") ");
	std::string split(" //");
	std::string id;
	std::string name;
	std::string file;

	for ( MXmlElement *pChildElement = pRootElement->FirstChildElement();
		pChildElement; pChildElement = pChildElement->NextSiblingElement())
	{
		if ( _stricmp( pChildElement->Value(), "ZONE") == 0)
		{
			_Attribute(id, pChildElement, "id", &kXml);
			_Attribute(name, pChildElement, "name", &kXml);
			_Attribute(file, pChildElement, "file", &kXml);
			m_zoneList_.push_back(sB + id + eB + name + split + file );
		}
	}
	return true;
}

bool CXMLResourceList::LoadFieldList()
{
	m_fieldList_.clear();

	MXml kXml;
	if ( !kXml.LoadFile(m_fieldListPath_.c_str()))
		return false;

	if(kXml.Doc()->FirstChildElement("maiet") == NULL)
		return false;

	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("maiet")->FirstChildElement("FIELDLIST");
	if ( pRootElement == NULL )
		return false;

	std::string sB("(field:");
	std::string sE(") ");
	std::string id;
	std::string name;

	std::string sGB("(");
	std::string sGDot(":");
	std::string strGroupId;

	for ( MXmlElement *pChildElement = pRootElement->FirstChildElement();
		pChildElement; pChildElement = pChildElement->NextSiblingElement())
	{
		if ( _stricmp( pChildElement->Value(), "FIELD") == 0 )
		{
			_Attribute(id, pChildElement, "id", &kXml);
			_Attribute(name, pChildElement, "name", &kXml);
			m_fieldList_.push_back(sB + id + sE + name );
		}
		else if ( _stricmp( pChildElement->Value(), "FIELDGROUP") == 0 )
		{
			for (MXmlElement* pFieldGroupChild = pChildElement->FirstChildElement();
				pFieldGroupChild; pFieldGroupChild = pFieldGroupChild->NextSiblingElement())
			{
				_Attribute(strGroupId, pChildElement, "id");
				if ( _stricmp( pFieldGroupChild->Value(), "FIELD") == 0 )
				{
					_Attribute(id, pFieldGroupChild, "id", &kXml);
					_Attribute(name, pFieldGroupChild, "name", &kXml);
					m_fieldList_.push_back(sGB+ strGroupId + sGDot + id + sE + name);
				}
			}
		}
	}
	return true;
}

void CXMLResourceList::PutComboBox(CComboBox& _refComboBox, LIST_TYPE _type)
{
	_refComboBox.ResetContent();
	std::vector<std::string>::iterator itr;

	if(_type == ZONE_LIST)
	{
		for(itr = m_zoneList_.begin(); itr != m_zoneList_.end(); itr++)
		{
			_refComboBox.AddString(itr->c_str());
		}
		_refComboBox.AddString("테스트존");
	}
	else if(_type == FIELD_LIST)
	{
		for(itr = m_fieldList_.begin(); itr != m_fieldList_.end(); itr++)
		{
			_refComboBox.AddString(itr->c_str());
		}
	}
}

void CXMLResourceList::PutListCtrl(CListCtrl& _refListCtrl,
	std::map<int, std::pair<std::string, std::string> >& _refFileName,
	LIST_TYPE _type)
{
	//reset
	_refListCtrl.DeleteAllItems();
	_refFileName.clear();

	std::vector<std::string>::iterator itr;
	int i = 0;

	char szfull[256]= "";
	_fullpath( szfull, CXMLConfig::GetInstance()->pcDataPath, 256 );

	if(_type == ZONE_LIST)
	{
		for(itr = m_zoneList_.begin(); itr != m_zoneList_.end(); ++itr, ++i)
		{
			CString str_list(itr->c_str());
			_refListCtrl.InsertItem(i, str_list);

			//파일이름 확인
			int a = str_list.Find("//");
			int b = str_list.GetLength();
			CString newZoneId = str_list.Right(b-a-2);
			CString fullPath(szfull);
			fullPath += "zone\\";
			CString zoneFile = fullPath + newZoneId + "\\" + newZoneId + ".zone.xml";
			//파일이 존재하면
			if(GetFileAttributes(zoneFile) != INVALID_FILE_ATTRIBUTES)
			{
				_refFileName[i].first = zoneFile.GetString();
				_refFileName[i].second = (LPTSTR)(LPCTSTR)(fullPath + "Zone_Utils\\" + newZoneId + ".bmp");
			}
		}
	}
	else if(_type == FIELD_LIST)
	{
		for(itr = m_fieldList_.begin(); itr != m_fieldList_.end(); ++itr, ++i)
		{
			_refListCtrl.InsertItem(i, itr->c_str());
		}
	}
}

bool CXMLResourceList::LoadGrassList(const char *filename, std::vector<std::string>& _refVecFile)
{
	//초기화
	_refVecFile.clear();

	MXml kXml;
	if ( !kXml.LoadFile(filename))
		return false;
   
	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("ROOT");
	if ( pRootElement == NULL )
		return false;

	for ( MXmlElement *pChildElement = pRootElement->FirstChildElement();
		pChildElement; pChildElement = pChildElement->NextSiblingElement())
	{
		if ( _stricmp( pChildElement->Value(), "GRASSTEXTURE") == 0)
		{
			std::string pieceName;
			_Contents(pieceName, pChildElement);
			_refVecFile.push_back(pieceName);
		}
	}
	return true;
}

bool CXMLResourceList::MakeZoneIdFileNameMap()
{
	m_vecZoneIdFileNameMap.clear();

	if (!LoadZoneList())
		return false;

	char szfull[256]= "";
	_fullpath( szfull, CXMLConfig::GetInstance()->pcDataPath, 256 );
	std::string fullPath(szfull);

	const std::vector<std::string>&  zoneList = GetZoneList();
	for(std::vector<std::string>::const_iterator it = zoneList.begin(); it != zoneList.end(); ++it)
	{
		// id
		int start_id = it->find(":");
		int end_id = it->find(")");
		std::string str_id(it->substr(start_id+1, end_id - start_id-1));
		int zone_id = atoi(str_id.c_str());

		// file name
		int start_zone_name = it->find("//");
		int end_zone_name = it->find("(");
		std::string zoneFileName(it->substr( start_zone_name + 2, end_zone_name - start_zone_name -2));
		zoneFileName = fullPath + "zone\\" + zoneFileName + "\\" + zoneFileName + ".zone.xml";

		m_vecZoneIdFileNameMap.push_back( std::pair<int, std::string>(zone_id, zoneFileName) );
	}

	if (m_vecZoneIdFileNameMap.empty())
		return false;

	return true;
}

int CXMLResourceList::GetZoneIdFromFileName( const char* _pSzZoneName )
{
	for (std::vector< std::pair<int, std::string> >::iterator itr = m_vecZoneIdFileNameMap.begin(); itr != m_vecZoneIdFileNameMap.end(); ++itr)
	{
		CString strInMapData(itr->second.c_str());
		strInMapData.MakeLower();

		CString strRequested(_pSzZoneName);
		strRequested.MakeLower();

		if (strInMapData == strRequested)
		{
			return itr->first;
		}
	}

	return -1;
}

bool CXMLResourceList::GetZoneFileNameFromId( int _nId, std::string& _refZoneFileName )
{
	for (std::vector< std::pair<int, std::string> >::iterator itr = m_vecZoneIdFileNameMap.begin(); itr != m_vecZoneIdFileNameMap.end(); ++itr)
	{
		if (itr->first == _nId)
		{
			_refZoneFileName = itr->second;
			return true;
		}
	}

	return false;
}
