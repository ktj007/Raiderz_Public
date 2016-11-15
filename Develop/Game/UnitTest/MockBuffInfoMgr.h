#pragma  once

#include "XBuffInfo.h"

class MockBuffInfoMgr : public XBuffInfoMgr
{
public:
	MockBuffInfoMgr();
	virtual ~MockBuffInfoMgr();

	void CreateBuffInfo( const std::wstring& _strXmlData);
	
protected:	
	bool SetBuffInfo( const std::wstring& _strXmlData);
};
