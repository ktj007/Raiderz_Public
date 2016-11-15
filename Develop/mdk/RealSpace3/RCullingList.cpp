#include "stdafx.h"
#include "RCullingList.h"

namespace rs3
{

	RCullingList::RCullingList( void )
	{
	}

	RCullingList::~RCullingList( void )
	{
	}

	void RCullingList::AddSceneNode( RSceneNode *pNode )
	{
		m_CulledList[pNode->GetUsingPass()].insert(pNode);
	}

	void RCullingList::RemoveSceneNode( RSceneNode* pNode )
	{
		m_CulledList[pNode->GetUsingPass()].erase(pNode);
	}

	bool RCullingList::ExistSceneNode( RSceneNode* pNode )
	{
		SCENE_SET::iterator iter = m_CulledList[pNode->GetUsingPass()].find(pNode);
		if( iter == m_CulledList[pNode->GetUsingPass()].end() )	return	false;
		return true;
	}

	void RCullingList::Clear( void )
	{
		for(int i=0;i<RRP_MAX;i++)
			m_CulledList[i].clear();
	}


}