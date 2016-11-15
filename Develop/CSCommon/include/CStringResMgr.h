#pragma once

#include "CSCommonLib.h"
#include "CStringRes.h"
#include "MTstring.h"

struct CLangNode;

/// 문자열 리소스 관리자
class CSCOMMON_API CStringResMgr
{
public:
	typedef map<tstring, CStringRes<tstring>*>	lang_map;

public:
	CStringResMgr();
	virtual ~CStringResMgr();

	bool Init(const CLangNode& node, bool bIgnoteNonTranslationMark=false);											///< 해당 폴더의 xml을 모두 읽는다.
	bool Init(const CLangNode& node, const vector< std::tstring>& vFileList, bool bIgnoteNonTranslationMark=false);	///< 지정한 파일 목록을 모두 읽는다

	virtual	void Clear(void);

	virtual const TCHAR* GetString(tstring& key);
	virtual const TCHAR* GetString(const TCHAR* key);
	virtual wstring Plural(wstring key);
	bool Exist(tstring& key);
	bool Exist(const TCHAR* key);

	bool InsertString(const tstring& key, const tstring& value);
	bool UpdateString(const tstring& key, const tstring& value);
	void DeleteString(tstring& key);

	bool SetLocale(tstring strLocale);
	void AddLocale(tstring strLocale);
	tstring GetLocale(void) const;

protected:
	virtual bool OnInit(const CLangNode& node)		{ return true;	}	

private:
	bool LoadStrTable(const CLangNode& node, const TCHAR* szFileName, bool bIgnoteNonTranslationMark);

protected:
	lang_map				m_mapStringTables;		///< 다국어 테이블	
	tstring					m_strLocale;
};
