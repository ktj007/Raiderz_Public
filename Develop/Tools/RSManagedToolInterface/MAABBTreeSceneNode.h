#pragma once

#include "MFramework.h"
#include "MSceneNode.h"

using namespace System;
using namespace rs3;

namespace RSManaged
{
	public __gc class MAABBTreeSceneNode : public MSceneNode
	{
	public:
		MAABBTreeSceneNode();
		MAABBTreeSceneNode(RSceneNode* pRealSceneNode);
		~MAABBTreeSceneNode();

		bool Open();
	protected:

	};
}
