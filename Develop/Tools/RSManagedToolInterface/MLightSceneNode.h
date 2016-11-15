#pragma once

#include "MFramework.h"
#include "MSceneNode.h"

using namespace System;
using namespace rs3;

namespace RSManaged
{
	public __gc class MLightSceneNode : public MSceneNode
	{
	public:
		MLightSceneNode();
		MLightSceneNode(RSceneNode* pRealSceneNode);
		~MLightSceneNode();

	protected:

	};
}
