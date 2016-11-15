#include "stdafx.h"
#include "MCommandDesc.h"

namespace minet {

MCommandDesc::MCommandDesc(int nID, int nFlag, MCommandFlow nFlow, const char* szName, const char* szDescription)
{
	m_nID = nID;
	strcpy_s(m_szName, szName);
	strcpy_s(m_szDescription, szDescription);
	m_nFlag = nFlag;
	m_nFlow = nFlow;
	m_pHandler = NULL;
	m_pFunc = NULL;
}

MCommandDesc::~MCommandDesc(void)
{
	for(int i=0; i<(int)m_ParamDescs.size(); i++)
	{
		SAFE_DELETE(m_ParamDescs[i]);
	}
	m_ParamDescs.clear();
}

void MCommandDesc::AddParamDesc(MCommandParameterDesc* pParamDesc)
{
	m_ParamDescs.push_back(pParamDesc);
}

bool MCommandDesc::IsFlag(int nFlag) const
{
	return ((m_nFlag&nFlag)==nFlag);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandDescMap //////////////////////////////////////////////////////////////////////////////////
MCommandDescMap::MCommandDescMap()
{

}

MCommandDescMap::~MCommandDescMap()
{

}

MCommandDescMap* MCommandDescMap::GetInstance()
{
	static MCommandDescMap stCommandDescMap;
	return &stCommandDescMap;
}

void MCommandDescMap::Clear()
{
	for(MDescMap::iterator i = m_DescMap.begin(); i != m_DescMap.end(); i++)
	{
		MCommandDesc* pCommandDesc = (*i).second;
		SAFE_DELETE(pCommandDesc);
	}
	m_DescMap.clear();
}

MCommandDesc* MCommandDescMap::GetCommandDesc(int i)
{
	if(i<0 || i>=(int)m_DescMap.size()) return NULL;

	MDescMap::iterator itor = m_DescMap.begin();

	for (int t=0; t < i; t++)
	{
		itor++;
		if (itor == m_DescMap.end()) return NULL;
	}

	return (*itor).second;
}

MCommandDesc* MCommandDescMap::GetCommandDescByID(int nID)
{
	MDescMap::iterator itor = m_DescMap.find(nID);
	if (itor != m_DescMap.end())
	{
		return (*itor).second;
	}
	
	return NULL;
}

void MCommandDescMap::AddCommandDesc(MCommandDesc* pCD)
{
	_ASSERT(m_DescMap.find(pCD->GetID()) == m_DescMap.end());	// 커맨드는 중복되면 안된다
	m_DescMap.insert(MDescMap::value_type(pCD->GetID(), pCD));
}

MCommandDesc* MCommandDescMap::GetCommandDescByName( const char* chName )
{
	MDescMap::iterator itor = m_DescMap.begin();

	while(itor != m_DescMap.end())
	{
		if(strcmp((*itor).second->GetName(), chName) == 0)
		{
			return (*itor).second;
		}

		itor++;
	}

	return NULL;
}



} // namespace minet