#include "stdafx.h"
#include "MLightSceneNode.h"

using namespace RSManaged;
using namespace System;


MLightSceneNode::MLightSceneNode() : MSceneNode()
{
	m_pSN = new rs3::RLightSceneNode();
}

MLightSceneNode::MLightSceneNode(RSceneNode* pRealSceneNode) : MSceneNode(pRealSceneNode)
{

}

MLightSceneNode::~MLightSceneNode()
{

}
