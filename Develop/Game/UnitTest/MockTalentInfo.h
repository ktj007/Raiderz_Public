#pragma  once

#include "XTalentInfoMgr.h"

class MockTalentInfoMgr : public XTalentInfoMgr
{
public:
	bool CreateTalent( const std::wstring& _strXmlData );
	bool SetTalent( XTalentInfo* pTalentInfo, const std::wstring& _strXmlData );
	bool SetTalentExt( XTalentInfo* pTalentInfo, const std::wstring& _strXmlData );
};
