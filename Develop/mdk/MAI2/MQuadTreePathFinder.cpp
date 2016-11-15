#include "StdAfx.h"
#include "MQuadTreePathFinder.h"

MQuadTreePathFinder::MQuadTreePathFinder(void)
{
}

MQuadTreePathFinder::~MQuadTreePathFinder(void)
{
}

bool MQuadTreePathFinder::Create(const char* szWalkableMap)
{

	if(!m_walkableMap.CreateFromFile(szWalkableMap)) 
		return false;

	m_quadTree.Create(&m_walkableMap,0,0,m_walkableMap.GetSize(),
						m_walkableMap.GetScaleX(),m_walkableMap.GetScaleY());

	return true;
}

bool MQuadTreePathFinder::FindPath(const MVector3& startPos, const MVector3& goalPos)
{
	MQuadTreeNode* pStartNode = m_quadTree.GetNode(startPos.x,startPos.y);
	MQuadTreeNode* pGoalNode = m_quadTree.GetNode(goalPos.x,goalPos.y);

	if(!pStartNode || !pGoalNode) return false;

	if(pStartNode->GetAreaID()==0 || pGoalNode->GetAreaID()==0) 
		return false;
	if(pStartNode->GetAreaID()!=pGoalNode->GetAreaID())
		return false;

	return m_AStar.FindPath(pStartNode,pGoalNode);
//	return true;
}

int MQuadTreePathFinder::GetPathCount(void)
{
	return m_AStar.GetShortestPathCount();
}

MQuadTreeNode* MQuadTreePathFinder::GetPath(int i)
{
	return (MQuadTreeNode*)m_AStar.GetShortestPath(i);
}

bool MQuadTreePathFinder::IsWalkable(const MVector3& pos)
{
	// 노드가 존재하지 않으면 갈수 없는 곳이다.
	return m_quadTree.GetNode(pos.x,pos.y)!=NULL;
}