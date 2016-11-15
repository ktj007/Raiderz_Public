#pragma  once
#include "CSMeshInfo.h"

class MockMeshInfoMgr : public CSMeshInfoMgr
{
public:
	bool SetMeshColInfo(const std::wstring& strMeshName, const std::wstring& _strXmlData);
};
