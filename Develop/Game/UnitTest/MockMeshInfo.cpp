#include "stdafx.h"
#include "MockMeshInfo.h"
#include "CSMeshInfoLoader.h"

bool MockMeshInfoMgr::SetMeshColInfo( const std::wstring& strMeshName, const std::wstring& _strXmlData )
{
	USES_CONVERSION_EX;

	MXmlDocument doc;
	doc.Parse(W2A_EX(_strXmlData.c_str(), 0));
	MXmlElement* pRootElement = doc.FirstChildElement();
	if(pRootElement == NULL)
		return false;

	std::wstring strLowerMeshName = CSStrings::StringToLower(strMeshName);

	CSMeshInfoLoader meshInfoLoader;
	map<wstring, CSMeshInfo>::iterator meshInfoFind = m_mapMeshInfo.find(strLowerMeshName);
	if(meshInfoFind != m_mapMeshInfo.end())
	{
		meshInfoLoader.ParseMeshInfo(&((*meshInfoFind).second), NULL, pRootElement);
	}
	else 
	{
		CSMeshInfo meshInfo;
		meshInfoLoader.ParseMeshInfo(&meshInfo, NULL, pRootElement);
		meshInfo.m_strMeshName = strLowerMeshName;
		m_mapMeshInfo.insert(map<wstring, CSMeshInfo>::value_type(meshInfo.m_strMeshName, meshInfo));
	}

	return true;
}