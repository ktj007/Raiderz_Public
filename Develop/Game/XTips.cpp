#include "stdafx.h"
#include "XTips.h"
#include "XDef.h"
#include "XStrings.h"


// XTipCategory
XTipCategory::XTipCategory( int nParam1, int nParam2)
{
	m_nParam1 = nParam1;
	m_nParam2 = nParam2;
}


// ~XTipCategory
XTipCategory::~XTipCategory()
{
	m_Tips.clear();
}


// AddTip
void XTipCategory::AddTip( const wchar_t* szTip)
{
	m_Tips.push_back( szTip);
}


// GetRandomTip
const wchar_t* XTipCategory::GetRandomTip()
{
	if ( m_Tips.empty() == true)		return L"";

	XStringResMgr::GetInstance()->TransKeyStr(m_strBuffer, m_Tips[ rand() % m_Tips.size()].c_str());	
	return m_strBuffer.c_str();
}


// SetDescription
void XTipCategory::SetDescription( const wchar_t* szDesc)
{
	m_strDescription = szDesc;
}


// GetDescription
const wchar_t* XTipCategory::GetDescription()
{
	XStringResMgr::GetInstance()->TransKeyStr(m_strBuffer, m_strDescription.c_str());	
	return m_strBuffer.c_str();	
}


// XTips
XTips::XTips()
{
}


// ~XTips
XTips::~XTips()
{
	if ( m_Categories.empty() == false)
	{
		for ( multimap< wstring, XTipCategory*>::iterator itr = m_Categories.begin();  itr != m_Categories.end();  itr++)
			delete (*itr).second;

		m_Categories.clear();
	}
}


// Load
bool XTips::Load()
{
	MXml xml;
	if ( xml.LoadFile( MLocale::ConvUTF16ToAnsi(FILENAME_TIPS).c_str()) == false)		return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild( "TIPS").FirstChildElement().Element();

	for ( pElement;  pElement != NULL;  pElement = pElement->NextSiblingElement())
	{
		if ( _stricmp( pElement->Value(), "CATEGORY") != 0)		continue;

		wstring name;
		_Attribute( name, pElement, "name", "");

		int param1, param2;
		_Attribute( &param1, pElement, "param1", 0);
		_Attribute( &param2, pElement, "param2", 0);


		XTipCategory* pCategory = new XTipCategory( param1, param2);
		m_Categories.insert( multimap< wstring, XTipCategory*>::value_type( name, pCategory));


		for ( MXmlElement* pChild = pElement->FirstChildElement();  pChild != NULL;  pChild = pChild->NextSiblingElement())
		{
			if ( _stricmp( pChild->Value(), "TIP") == 0)
			{
				string tip;
				_Contents( tip, pChild, "");

				pCategory->AddTip(MLocale::ConvAnsiToUTF16(tip.c_str()).c_str());
			}
			else if ( _stricmp( pChild->Value(), "DESC") == 0)
			{
				string desc;
				_Contents( desc, pChild, "");

				pCategory->SetDescription( MLocale::ConvAnsiToUTF16(desc.c_str()).c_str());
			}
		}
	}

	return true;
}


// GetRandomTip
const wstring& XTips::GetRandomTip()
{
	m_strString.clear();

	while ( 1)
	{
		// Roll dice
		switch ( rand() % 3)
		{
		case 0 :	m_strString = GetRandomCommonTip();		break;
		case 1 :	m_strString = GetRandomLevelTip();		break;
		case 2 :	m_strString = GetRandomFieldTip();		break;
		}
			
		return m_strString;
	}

	return m_strString;
}


// GetFieldDesc
const wstring& XTips::GetFieldDesc()
{
	m_strString.clear();

	int nFieldID = gvar.World.GetFieldID();
	pair< multimap< wstring, XTipCategory*>::iterator, multimap< wstring, XTipCategory*>::iterator> itr_pair;
	itr_pair = m_Categories.equal_range( wstring( L"field"));

	for ( multimap< wstring, XTipCategory*>::iterator itr = itr_pair.first;  itr != itr_pair.second;  itr++)
	{
		if ( nFieldID != (*itr).second->m_nParam1)		continue;

		m_strString = (*itr).second->GetDescription();
		break;
	}

	return m_strString;
}


// GetRandomCommonTip
const wchar_t* XTips::GetRandomCommonTip()
{
	multimap< wstring, XTipCategory*>::iterator itr = m_Categories.find( wstring( L"common"));
	if ( itr == m_Categories.end())			return L"";
	return (*itr).second->GetRandomTip();
}


// GetRandomLevelTip
const wchar_t* XTips::GetRandomLevelTip()
{
	int nLevel = gvar.MyInfo.nLevel;

	pair< multimap< wstring, XTipCategory*>::iterator, multimap< wstring, XTipCategory*>::iterator> itr_pair;
	itr_pair = m_Categories.equal_range( wstring( L"level"));

	int i = 0;

	for ( multimap< wstring, XTipCategory*>::iterator itr = itr_pair.first;  itr != itr_pair.second;  itr++)
	{
		if (i > 3) return L"";
		if ( nLevel < (*itr).second->m_nParam1  ||  nLevel > (*itr).second->m_nParam2)		i++; continue;

		return (*itr).second->GetRandomTip();
	}
	return L"";
}


// GetRandomFieldTip
const wchar_t* XTips::GetRandomFieldTip()
{
	int nFieldID = gvar.World.GetFieldID();

	pair< multimap< wstring, XTipCategory*>::iterator, multimap< wstring, XTipCategory*>::iterator> itr_pair;
	itr_pair = m_Categories.equal_range( wstring( L"field"));

	for ( multimap< wstring, XTipCategory*>::iterator itr = itr_pair.first;  itr != itr_pair.second;  itr++)
	{
		if ( nFieldID != (*itr).second->m_nParam1)		continue;

		return (*itr).second->GetRandomTip();
	}
	return L"";
}

void XTipInGameUpdater::Update( float fDeltaTime, XTips* pTips, XUISystem* pUISystem )
{
	if (gvar.MyInfo.nLevel > XCONST::INGAME_TIP_MAX_LEVEL) return;

	if (m_RegUpdater.IsReady(fDeltaTime))
	{
		if (wcslen(pTips->GetRandomFieldTip()) <= 0) return;

		wstring strTip = L"[Tip] " + wstring(pTips->GetRandomFieldTip());

		pUISystem->OnPresentationUpper(strTip);
	}
}

XTipInGameUpdater::XTipInGameUpdater()
{
	m_RegUpdater.SetTickTime(float(XCONST::INGAME_TIP_TICK));
}
