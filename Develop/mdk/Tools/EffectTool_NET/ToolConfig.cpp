#include "StdAfx.h"
#include "ToolConfig.h"
#include "REngine.h"
#include "RConfiguration.h"

///////////////////////////////////////////////////////////////////////////////

void CMostRecentlyUsedList::Add( const string& str )
{
	// 이미 목록에 있으면 목록에 있는 아이템 제거
	for( vector<string>::iterator it = m_list.begin(); it != m_list.end(); ++it )
	{
		if( (*it) == str )
		{
			m_list.erase(it);
			break;
		}
	}

	_ASSERT( m_list.size() <= MAX_COUNT );
	if( m_list.size() >= MAX_COUNT )
		m_list.erase(m_list.begin());

	m_list.insert( m_list.begin(), str );
}

string CMostRecentlyUsedList::Get( size_t index )
{
	_ASSERT( m_list.size() > index );

	if( m_list.size() <= index )
		return string("");

	return m_list[index];
}

bool CMostRecentlyUsedList::Save( MXmlElement* pElement )
{
	_ASSERT( pElement );

	if( m_list.size() == 0 )
		return false;

	MXmlElement *pMRUElement = new MXmlElement("MRULIST");
	pElement->LinkEndChild( pMRUElement );

	for( size_t i = 0; i < m_list.size(); ++i )
	{
		MXmlElement* pMRUItemElement = new MXmlElement("MRUITEM");
		pMRUElement->LinkEndChild( pMRUItemElement );
		_SetAttribute( pMRUItemElement, "name", m_list[i] );
	}

	return true;
}

bool CMostRecentlyUsedList::Load( MXmlElement* pElement )
{
	_ASSERT( pElement );

	MXmlElement* pMRUElement = pElement->FirstChildElement( "MRULIST" );
	if( NULL == pMRUElement )
		return false;

	m_list.clear();

	MXmlElement* pMRUItemElement = pMRUElement->FirstChildElement( "MRUITEM" );
	for( pMRUItemElement; NULL != pMRUItemElement; pMRUItemElement = pMRUItemElement->NextSiblingElement() )
	{
		string str;
		_Attribute( str, pMRUItemElement, "name" );
		m_list.push_back( str );
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////

CToolConfig::CToolConfig(void)
{
	CAMERA_FOV		= 70;
	start_X			= 0;
	start_Y			= 0;
	maximize		= false;
	WinForm_width	= 1092;
	WinForm_height	= 864;

	split2			= 676;
	split3			= 210;

	ZeroMemory( m_customColors, sizeof(m_customColors) );

	InitPath();
}

CToolConfig::~CToolConfig(void)
{
}

bool CToolConfig::LoadConfig()
{
	using namespace rs3;

	MXml xml;
	if (xml.LoadFile("config.xml") == false)
	{
		if(xml.LoadFile("config_rel.xml") == false)
			return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pRootElement = docHandle.FirstChild("XML").Element();
	_ASSERT( pRootElement );
	if( NULL == pRootElement )
		return false;

	MXmlElement* pWinForm = pRootElement->FirstChildElement("WINFORM");
	if(pWinForm)
	{
		_Attribute(start_X, pWinForm, "startX");
		_Attribute(start_Y, pWinForm, "startY");
		_Attribute(&maximize, pWinForm, "maximize");
		_Attribute(WinForm_width, pWinForm, "width");
		_Attribute(WinForm_height, pWinForm, "height");

		_Attribute(split2, pWinForm, "split2");
		_Attribute(split3, pWinForm, "split3");
	}

	// 듀얼모니터에서 쓰레기값이 들어가서 초기화 실패하는 경우가 있어서 추가.
	if(WinForm_width <= 0)
		WinForm_width = 800;
	if(WinForm_height <= 0)
		WinForm_height = 600;
	if(start_X <= 0)
		start_X = 0;
	if(start_Y <= 0)
		start_Y = 0;

	if( MXmlElement* pEluFileMRUListElement = pRootElement->FirstChildElement("MRU_ELU") )
		m_eluFileMRUList.Load( pEluFileMRUListElement );

	MXmlElement* pCustomColors = pRootElement->FirstChildElement("CUSTOMCOLORS");
	if(pCustomColors)
	{
		MXmlElement* pCustomColor = pCustomColors->FirstChildElement("CUSTOMCOLOR");
		for(int i = 0; pCustomColor != NULL; pCustomColor = pCustomColor->NextSiblingElement("CUSTOMCOLOR"), ++i)
		{
			if( i >= 16 )
				break;

			_Contents( &m_customColors[i], pCustomColor );
		}
	}

	return true;
}

bool CToolConfig::SaveConfig()
{
	MXml xml;
	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild(pDec);

	MXmlElement* pRootElement = new MXmlElement("XML");
	xml.Doc()->LinkEndChild(pRootElement);

	{
		MXmlElement* pWinForm = new MXmlElement("WINFORM");
		pRootElement->LinkEndChild(pWinForm);

		_SetAttribute(pWinForm, "startX",	start_X);
		_SetAttribute(pWinForm, "startY",	start_Y);
		_SetAttribute(pWinForm, "maximize",	maximize);
		_SetAttribute(pWinForm, "width", WinForm_width);
		_SetAttribute(pWinForm, "height", WinForm_height);

		_SetAttribute(pWinForm, "split2", split2);
		_SetAttribute(pWinForm, "split3", split3);
	}

	{
		MXmlElement* pCustomColors = new MXmlElement("CUSTOMCOLORS");
		pRootElement->LinkEndChild( pCustomColors );

		for(int i = 0; i < MAX_CUSTOMCOLOR_COUNT; ++i)
		{
			MXmlElement* pCustomColor = new MXmlElement("CUSTOMCOLOR");
			pCustomColors->LinkEndChild( pCustomColor );
			_SetContents( pCustomColor, m_customColors[i] );
		}
	}

	{
		MXmlElement* pEluFileMRUListElement = new MXmlElement("MRU_ELU");
		pRootElement->LinkEndChild( pEluFileMRUListElement );
		m_eluFileMRUList.Save( pEluFileMRUListElement );
	}

	return xml.SaveFile("config.xml");
}

void CToolConfig::AddEluFileToMRUList( const char* pFileName )
{
	string fileName(pFileName);
	m_eluFileMRUList.Add( fileName );
}

void CToolConfig::InitPath()
{
	// 실행 파일 경로를 Current Directory로 설정
	char szModuleFilePath[_MAX_DIR] = {0,};
	GetModuleFileName( NULL, szModuleFilePath, _MAX_DIR );
	PathRemoveFileSpec( szModuleFilePath );
	BOOL result = SetCurrentDirectory( szModuleFilePath );
	_ASSERT( result );

	if( rs3::REngine::IsDevelopVersion() )
		m_strEngineResourcePath = "..\\..\\EngineResDev\\";
	else
		m_strEngineResourcePath = "..\\..\\EngineRes\\";

	m_strDataPath = "..\\..\\Data\\";

	// 디버깅용 경로 하드코딩
	//m_strEngineResourcePath = "..\\..\\EngineRes\\";
	//m_strDataPath = "..\\..\\..\\..\\..\\Game\\Runtime\\Data\\";

	// 파일 시스템 초기화 경로
	m_strFileSystemPath = m_strEngineResourcePath + string(";") + m_strDataPath;

	// 데이터 경로를 CurrentDirectory로 지정하고 FullPath를 얻는다.
	result = SetCurrentDirectory( m_strDataPath.c_str() );
	_ASSERT( result );

	char szDataFullPath[MAX_PATH] = {0,};
	DWORD dwResult = GetCurrentDirectory( MAX_PATH, szDataFullPath );
	_ASSERT( dwResult > 0 );
	string strDataFullPath(szDataFullPath);

	// 실행 파일 경로로 CurrentDirectory 다시 복구.
	result = SetCurrentDirectory( szModuleFilePath );
	_ASSERT( result );

	// 데이터 FullPath 경로 설정
	m_strEluPath = strDataFullPath + string("\\Model\\");
	m_strEffectPath = strDataFullPath + string("\\SFX\\");
	m_strEffectTexturePath = strDataFullPath + string("\\Texture\\SFX\\");
}