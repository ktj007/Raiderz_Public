#include "stdafx.h"
#include "CStringResMgr.h"
#include "MDebug.h"
#include <io.h>
#include "Shlwapi.h"
#include "MLocale.h"
#include "CLangInfo.h"

CStringResMgr::CStringResMgr()
{
}

CStringResMgr::~CStringResMgr()
{
	Clear();
}

bool CStringResMgr::Init(const CLangNode& node, bool bIgnoteNonTranslationMark)
{
	struct _tfinddata_t c_file;
	intptr_t hFile;
	TCHAR szName[_MAX_PATH];

	TCHAR szFilter[_MAX_PATH];
	_stprintf_s(szFilter, _T("%s*.xml"), MLocale::ConvUTF16ToTCHAR(node.strPath.c_str()).c_str());

	hFile = _tfindfirst( szFilter, &c_file );

	if( hFile != -1 )
	{
		do
		{
			size_t len = _tcslen(c_file.name);
			_tcscpy_s(szName, c_file.name);			
			LoadStrTable(node, szName, bIgnoteNonTranslationMark);

		} while( _tfindnext( hFile, &c_file ) == 0 );

		_findclose(hFile);
	}

	bool ret = OnInit(node);
	return ret;
}

bool CStringResMgr::Init(const CLangNode& node, const vector< std::tstring>& vFileList, bool bIgnoteNonTranslationMark)
{
	for each(tstring strFileName in vFileList)
	{
		LoadStrTable(node, strFileName.c_str(), bIgnoteNonTranslationMark);		
	}

	bool ret = OnInit(node);
	return ret;
}

void CStringResMgr::Clear(void)
{
	for each(const lang_map::value_type& lang in m_mapStringTables)
	{
		delete lang.second;
	}
	m_mapStringTables.clear();

	m_strLocale.clear();
}

const TCHAR* CStringResMgr::GetString(tstring& key)
{
	lang_map::iterator it = m_mapStringTables.find(m_strLocale);

	if (it == m_mapStringTables.end())
	{
		return _T("");
	}

	CStringRes<tstring>* pTable = it->second;
	return pTable->GetStr(key);
}

const TCHAR* CStringResMgr::GetString(const TCHAR* key)
{
	lang_map::iterator it = m_mapStringTables.find(m_strLocale);

	if (it == m_mapStringTables.end())
	{
		return _T("");
	}

	CStringRes<tstring>* pTable = it->second;
	return pTable->GetStr(key);
}

bool CStringResMgr::Exist(tstring& key)
{
	lang_map::iterator it = m_mapStringTables.find(m_strLocale);

	if (it == m_mapStringTables.end())
	{
		return false;
	}

	CStringRes<tstring>* pTable = it->second;
	return pTable->Exist(key);
}
bool CStringResMgr::Exist(const TCHAR* key)
{
	lang_map::iterator it = m_mapStringTables.find(m_strLocale);

	if (it == m_mapStringTables.end())
	{
		return false;
	}

	CStringRes<tstring>* pTable = it->second;
	return pTable->Exist(key);
}


wstring CStringResMgr::Plural(wstring key)
{
	lang_map::iterator it = m_mapStringTables.find(m_strLocale);

	if (it == m_mapStringTables.end())
	{
		return L"";
	}
	
	CStringRes<tstring>* pTable = it->second;	
	tstring strKey = MLocale::ConvUTF16ToTCHAR(key.c_str());
	return MLocale::ConvTCHARToUTF16(pTable->GetStr(strKey, CStringNode::PLURAL));
}

bool CStringResMgr::InsertString(const tstring& key, const tstring& value)
{
	lang_map::iterator it = m_mapStringTables.find(m_strLocale);

	if (it == m_mapStringTables.end())
	{
		return false;
	}

	CStringRes<tstring>* pTable = it->second;	
	bool bInserted = pTable->InsertStr(key, value);
	return bInserted;
}

bool CStringResMgr::UpdateString(const tstring& key, const tstring& value)
{
	lang_map::iterator it = m_mapStringTables.find(m_strLocale);

	if (it == m_mapStringTables.end())
	{
		return false;
	}

	CStringRes<tstring>* pTable = it->second;	
	bool bSeted = pTable->SetStr(key, value);
	return bSeted;
}

void CStringResMgr::DeleteString(tstring& key)
{
	lang_map::iterator it = m_mapStringTables.find(m_strLocale);

	if (it == m_mapStringTables.end())
	{
		return;
	}

	CStringRes<tstring>* pTable = it->second;	
	pTable->DelStr(key);
}

bool CStringResMgr::LoadStrTable(const CLangNode& node, const TCHAR* szFileName, bool bIgnoteNonTranslationMark )
{
	// 언어 테이블 찾기
	lang_map::iterator it = m_mapStringTables.find(MLocale::ConvUTF16ToTCHAR(node.strLocale.c_str()));

	CStringRes<tstring>* pTable;

	if (it == m_mapStringTables.end())
	{
		// 새 언어 추가
		pTable = new CStringRes<tstring>();
		m_mapStringTables.insert(make_pair(MLocale::ConvUTF16ToTCHAR(node.strLocale.c_str()), pTable));

		if (m_strLocale.empty())
		{
			m_strLocale = MLocale::ConvUTF16ToTCHAR(node.strLocale.c_str());
		}
	}
	else
	{
		// 기존 언어 선택
		pTable = it->second;
	}


	// 문자열 테이블 읽기
	tstring strFullPath = MLocale::ConvUTF16ToTCHAR(node.strPath.c_str()) + szFileName;
	if (!pTable->Load(strFullPath.c_str(), bIgnoteNonTranslationMark))
	{
		mlog("Error!! - %s Load Failed(locale=%s).\n", MLocale::ConvTCHARToAnsi(szFileName).c_str(), MLocale::ConvUTF16ToAnsi(node.strLocale.c_str()).c_str());
		return false;
	}

	return true;
}


bool CStringResMgr::SetLocale(tstring strLocale)
{
	lang_map::iterator it = m_mapStringTables.find(strLocale);

	if (it == m_mapStringTables.end())
	{
		return false;
	}

	m_strLocale = strLocale;
	
	return true;
}

void CStringResMgr::AddLocale(tstring strLocale)
{
	lang_map::iterator it = m_mapStringTables.find(strLocale);

	if (it == m_mapStringTables.end())
	{
		CStringRes<tstring>* pTable = new CStringRes<tstring>();
		m_mapStringTables.insert(make_pair(strLocale, pTable ));

		if (m_strLocale.empty())
		{
			m_strLocale = strLocale;
		}
	}
}

tstring CStringResMgr::GetLocale(void) const
{
	return m_strLocale;
}
