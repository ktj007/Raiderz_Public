#pragma once

#include "RSceneNode.h"

namespace rs3
{
	class RCullingList
	{
	public:
		RCullingList(void);
		~RCullingList(void);

	public:
		void		AddSceneNode(RSceneNode *pNode);
		void		RemoveSceneNode(RSceneNode* pNode);
		bool		ExistSceneNode(RSceneNode* pNode);
		void		Clear(void);	

	public:
		SCENE_SET*		GetCulledList(RRENDERPASS pass)	{ return &m_CulledList[pass]; }

	private:
		SCENE_SET			m_CulledList[RRP_MAX];
	};
}