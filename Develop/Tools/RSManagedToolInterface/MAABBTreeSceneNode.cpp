#include "stdafx.h"
#include "MAABBTreeSceneNode.h"

using namespace RSManaged;
using namespace System;


MAABBTreeSceneNode::MAABBTreeSceneNode() : MSceneNode()
{
	m_pSN = new rs3::RAABBTreeSceneNode();
}

MAABBTreeSceneNode::MAABBTreeSceneNode(RSceneNode* pRealSceneNode) : MSceneNode(pRealSceneNode)
{

}

MAABBTreeSceneNode::~MAABBTreeSceneNode()
{

}

bool MAABBTreeSceneNode::Open()
{
	rs3::RAABBTreeSceneNode* node = (rs3::RAABBTreeSceneNode*)m_pSN;
	return node->Open("D:/My Projects/Maiet/SoulHunt/Develop/RealSpace3/samples/runtime/Forbidden Temple/Forbidden Temple.RS");
}