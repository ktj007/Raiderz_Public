#pragma once


#include "MQuadTree.h"
#include "MAStar.h"

class MQuadTreePathFinder
{
	MPathWalkableMap m_walkableMap;
	MQuadTree	m_quadTree;
	MAStar		m_AStar;

public:
	MQuadTreePathFinder(void);
	virtual ~MQuadTreePathFinder(void);

	MQuadTree* GetQuadTree() { return &m_quadTree; }


	bool Create(const char* szWalkableMap);

	bool FindPath(const MVector3& startPos, const MVector3& goalPos);

	int GetPathCount(void);
	MQuadTreeNode* GetPath(int i);

	bool IsWalkable(const MVector3& startPos);
};