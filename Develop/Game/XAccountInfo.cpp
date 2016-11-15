
#include "stdafx.h"
#include "XAccountInfo.h"
#include "XPath.h"
#include "MLocale.h"
#include "XGlobalVar_Char.h"



// XAccountParam
XAccountParam::~XAccountParam()
{
	m_Param.clear();
}


// Set
bool XAccountParam::Set( wstring& _type, wstring& _val)
{
	if ( _type.empty() == true)		return false;

	map< wstring, wstring>::iterator itr = m_Param.find( _type);
	if ( itr != m_Param.end())
	{
		(*itr).second = _val;
		return true;
	}

	m_Param.insert( map< wstring, wstring>::value_type( _type, _val));
	return false;
}


// Get
const wchar_t* XAccountParam::Get( wstring& _type)
{
	if ( _type.empty() == true)		return NULL;

	map< wstring, wstring>::iterator itr = m_Param.find( _type);
	if ( itr != m_Param.end())
		return (*itr).second.c_str();

	return NULL;
}















// XAccountData
XAccountData::XAccountData()
{
}


// ~XAccountData
XAccountData::~XAccountData()
{
	Destroy();
}


// Destroy
void XAccountData::Destroy()
{
	ClearParameters();
	ClearJournalIndicator();
}


// SetParameter
bool XAccountData::SetParameter( wstring& strName, wstring& strType, wstring& strParam)
{
	map< wstring, XAccountParam*>::iterator itr = m_Parameters.find( strName);
	if ( itr != m_Parameters.end())
		return (*itr).second->Set( strType, strParam);


	XAccountParam* _param = new XAccountParam();
	_param->Set( strType, strParam);

	m_Parameters.insert( map< wstring, XAccountParam*>::value_type( strName, _param));

	return false;
}


// GetParameter
const wchar_t* XAccountData::GetParameter( wstring& strName, wstring& strType)
{
	map< wstring, XAccountParam*>::iterator itr = m_Parameters.find( strName);
	if ( itr != m_Parameters.end())
		return (*itr).second->Get( strType);

	return NULL;
}


// DeleteParameter
bool XAccountData::DeleteParameter( wstring& strName)
{
	map< wstring, XAccountParam*>::iterator itr = m_Parameters.find( strName);
	if ( itr != m_Parameters.end())
	{
		XAccountParam* pParam = (*itr).second;

		m_Parameters.erase( itr);
		delete pParam;

		return true;
	}

	return false;
}


// ClearParameters
void XAccountData::ClearParameters()
{
	for ( map< wstring, XAccountParam*>::iterator itr = m_Parameters.begin();  itr != m_Parameters.end();  itr++)
		delete (*itr).second;

	m_Parameters.clear();
}


// AddJournal
bool XAccountData::AddJournal( int nType, int nID)
{
	for ( vector< pair< int, int>>::iterator itr = m_JournalIndicator.begin();  itr != m_JournalIndicator.end();  itr++)
	{
		if ( (*itr).first == nType  &&  (*itr).second == nID)
			return false;
	}

	m_JournalIndicator.push_back( pair< int, int>( nType, nID));
	return true;
}


// RemoveJournal
bool XAccountData::RemoveJournal( int nType, int nID)
{
	for ( vector< pair< int, int>>::iterator itr = m_JournalIndicator.begin();  itr != m_JournalIndicator.end();  itr++)
	{
		if ( (*itr).first != nType  ||  (*itr).second != nID)	continue;

		m_JournalIndicator.erase( itr);
		return true;
	}

	return false;
}


// IsExistJournal
bool XAccountData::IsExistJournal( int nType, int nID)
{
	for ( vector< pair< int, int>>::iterator itr = m_JournalIndicator.begin();  itr != m_JournalIndicator.end();  itr++)
	{
		if ( (*itr).first == nType  &&  (*itr).second == nID)
			return true;
	}

	return false;
}


// ClearJournalIndicator
void XAccountData::ClearJournalIndicator()
{
	m_JournalIndicator.clear();
}












// XAccountInfo
XAccountInfo::XAccountInfo()
{
	m_bLoaded = false;
	m_pAccountInfo = NULL;
}


// ~XAccountInfo
XAccountInfo::~XAccountInfo()
{
	Clear();
}


// Clear
void XAccountInfo::Clear()
{
	if ( m_pAccountInfo != NULL)
	{
		delete m_pAccountInfo;
		m_pAccountInfo = NULL;
	}
}


// GetAccountInfo
XAccountData* XAccountInfo::GetAccountInfo()
{
	if ( m_bLoaded == false)
		return NULL;

	if ( m_pAccountInfo == NULL)
		m_pAccountInfo = new XAccountData();

	return m_pAccountInfo;
}


// Save
bool XAccountInfo::Save()
{
	if ( m_bLoaded == false  ||  m_pAccountInfo == NULL)
		return false;


	// Gather account save data
	if ( GatherAccountSaveData() == false)
		_ASSERT( 0);


	// Load XML file
	global.filesys->SetAllowAccessUncompFile( true);

	MXml xml;
	if ( xml.LoadFile(MLocale::ConvUTF16ToAnsi(XPath::GetAccountFileFullPath().c_str()).c_str()) == false)
		xml.Doc()->LinkEndChild( xml.CreateDeclaration( "1.0", "UTF-8", ""));

	global.filesys->SetAllowAccessUncompFile( false);


	// Remove my account info
	MXmlElement* pData = xml.DocHandle().FirstChild( "Data").Element();
	if ( pData == NULL)
	{
		pData = new MXmlElement( "Data");
		xml.Doc()->LinkEndChild( pData);
	}

	MXmlElement* pElement = pData->FirstChildElement();
	for ( pElement;  pElement != NULL;  pElement = pElement->NextSiblingElement())
	{
		if ( strcmp( pElement->Value(), "Account") != 0)
			continue;

		wstring strName;
		_Attribute( strName, pElement, "name", "", &xml);
		if ( strName.empty() == true  ||  strName != gvar.MyInfo.ChrInfo.szName)
			continue;

		pData->RemoveChild( pElement);
		break;
	}


	if ( m_pAccountInfo == NULL)	return true;


	// Write my account info
	MXmlElement* _account = new MXmlElement( "Account");
	_SetAttribute( _account, "name", MLocale::ConvUTF16ToUTF8(  gvar.MyInfo.ChrInfo.szName));
	pData->LinkEndChild( _account);


	// Parameter info
	MXmlElement* _params = new MXmlElement( "Parameters");
	_account->LinkEndChild( _params);

	map< wstring, XAccountParam*>& vParams = m_pAccountInfo->m_Parameters;
	for ( map< wstring, XAccountParam*>::iterator itr_params = vParams.begin();  itr_params != vParams.end();  itr_params++)
	{
		// Parameter
		MXmlElement* _param = new MXmlElement( "Param");
		_SetAttribute( _param, "name", MLocale::ConvUTF16ToAnsi( (*itr_params).first));
		_params->LinkEndChild( _param);

		map< wstring, wstring>& vParamVal = (*itr_params).second->m_Param;
		for ( map< wstring, wstring>::iterator itr_paramval = vParamVal.begin();  itr_paramval != vParamVal.end();  itr_paramval++)
		{
			if ( (*itr_paramval).first.empty() == true  ||  (*itr_paramval).second.empty() == true)
				continue;

			// Parameter value
			MXmlElement* _paramval = new MXmlElement( "Value");
			_SetAttribute( _paramval, "type", MLocale::ConvUTF16ToAnsi( (*itr_paramval).first));
			_SetContents( _paramval, MLocale::ConvUTF16ToUTF8( (*itr_paramval).second.c_str()).c_str());
			_param->LinkEndChild( _paramval);
		}
	}


	// Journal indicator
	MXmlElement* _journal = new MXmlElement( "JournalIndicator");
	_account->LinkEndChild( _journal);

	vector< pair< int, int>>& vJournal = m_pAccountInfo->m_JournalIndicator;
	for ( vector< pair< int, int>>::iterator itr = vJournal.begin();  itr != vJournal.end();  itr++)
	{
		if ( (*itr).first == 1)			// Quest
		{
			if ( gvar.MyInfo.Quests.IsPlayerQuest( (*itr).second) == false)
				continue;
		}
		else if ( (*itr).first == 2)	// Recipe
		{
			if ( gvar.MyInfo.RecipeInfo.IsExist( (*itr).second) == false)
				continue;
		}
		else
			continue;

		MXmlElement* _obj = new MXmlElement( "Object");
		_SetAttribute( _obj, "type", (*itr).first);
		_SetContents( _obj, (*itr).second);
		_journal->LinkEndChild( _obj);
	}

	return xml.SaveFile( MLocale::ConvUTF16ToAnsi(XPath::GetAccountFileFullPath().c_str()).c_str());
}


// Load
bool XAccountInfo::Load()
{
	// Create account data
	if ( m_pAccountInfo != NULL)	delete m_pAccountInfo;
	m_pAccountInfo = new XAccountData();

	m_bLoaded = true;


	// Load XML file
	global.filesys->SetAllowAccessUncompFile( true);

	MXml xml;
	if ( xml.LoadFile(MLocale::ConvUTF16ToAnsi(XPath::GetAccountFileFullPath().c_str()).c_str()) == false)
	{
		global.filesys->SetAllowAccessUncompFile( false);
		return false;
	}

	global.filesys->SetAllowAccessUncompFile( false);


	MXmlElement* pElement = xml.DocHandle().FirstChild( "Data").FirstChildElement().Element();
	for ( pElement;  pElement != NULL;  pElement = pElement->NextSiblingElement())
	{
		if ( strcmp( pElement->Value(), "Account") != 0)
			continue;

		wstring strName;
		_Attribute( strName, pElement, "name", "", &xml);
		if ( strName.empty() == true  ||  strName != gvar.MyInfo.ChrInfo.szName)
			continue;


		// Account
		for ( MXmlElement* _account = pElement->FirstChildElement();  _account != NULL;  _account = _account->NextSiblingElement())
		{
			// Parameter data
			if ( strcmp( _account->Value(), "Parameters") == 0)
			{
				for ( MXmlElement* _params = _account->FirstChildElement();  _params != NULL;  _params = _params->NextSiblingElement())
				{
					// Parameter
					if ( strcmp( _params->Value(), "Param") != 0)
						continue;

					wstring _name;
					_Attribute( _name, _params, "name", &xml);

					if ( _name.empty() == true)
						continue;


					// Value data
					for ( MXmlElement* _paramval = _params->FirstChildElement();  _paramval != NULL;  _paramval = _paramval->NextSiblingElement())
					{
						// Value
						if ( strcmp( _paramval->Value(), "Value") != 0)
							continue;

						wstring _type, _val;
						_Attribute( _type, _paramval, "type", &xml);
						_Contents( _val, _paramval, &xml);

						if ( _type.empty() == true  ||  _val.empty() == true)
							continue;

						m_pAccountInfo->SetParameter( _name, _type, _val);
					}
				}
			}


			// Journal indicator
			else if ( strcmp( _account->Value(), "JournalIndicator") == 0)
			{
				for ( MXmlElement* _obj= _account->FirstChildElement();  _obj != NULL;  _obj = _obj->NextSiblingElement())
				{
					// Fixed
					if ( strcmp( _obj->Value(), "Object") == 0)
					{
						int nType;
						_Attribute( &nType, _obj, "type", 0);

						int id;
						_Contents( &id, _obj, 0);

						if ( nType > 0  &&  id > 0)
							m_pAccountInfo->m_JournalIndicator.push_back( pair< int, int>( nType, id));
					}
				}
			}
		}

		break;
	}

	return true;
}


// GatherAccountSaveData
bool XAccountInfo::GatherAccountSaveData()
{
/*	// 유효한 계정의 정보만 남긴다
	for ( map< wstring, XAccountData*>::iterator itr = m_AccountInfo.begin();  itr != m_AccountInfo.end(); )
	{
		bool bValidate = false;

		for ( vector<XAccountCharInfo>::iterator _i = gvar.Char.m_CharInfos.begin();  _i != gvar.Char.m_CharInfos.end();  _i++)
		{
			if ( (*itr).first == wstring( (*_i).szName))
			{
				bValidate = true;
				break;
			}
		}


		// 해당 캐릭터 정보 삭제
		if ( bValidate == false)
		{
			XAccountData* pAccount = (*itr).second;
			delete pAccount;

			itr = m_AccountInfo.erase( itr);
			continue;
		}

		itr++;
	}



	// 내 캐릭터 정보를 리셋한다
	XAccountData* pAccount = NULL;
	for ( map< wstring, XAccountData*>::iterator itr = m_AccountInfo.begin();  itr != m_AccountInfo.end();  itr++)
	{
		if ( (*itr).first == wstring( gvar.MyInfo.ChrInfo.szName))
		{
			pAccount = (*itr).second;
			if ( pAccount == NULL)		continue;

			break;
		}
	}

	if ( pAccount == NULL)		return true;
*/

	/** 정보 수집 **/
	
	return true;
}


// ArrangeJournalIndicator
void XAccountInfo::ArrangeJournalIndicator()
{
	if ( m_bLoaded == false  ||  m_pAccountInfo == NULL)
		return;

	for ( vector< pair< int, int>>::iterator itr = m_pAccountInfo->m_JournalIndicator.begin();
		itr != m_pAccountInfo->m_JournalIndicator.end();)
	{
		if ( (*itr).first == 1)
		{
			if(gvar.MyInfo.Quests.IsPlayerQuest((*itr).second) == false)
			{
				itr = m_pAccountInfo->m_JournalIndicator.erase( itr);
				continue;
			}
		}

		itr++;
	}
}