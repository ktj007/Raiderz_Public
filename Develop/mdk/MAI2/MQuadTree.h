#pragma once

#include <list>
#include <vector>
#include "MVector3.h"
#include "MAStar.h"

using namespace std;

////////////////////////////////////////////////////////////////////
// MPathWalkableMap : 쿼드트리를 생성하는데 필요한 정보를 담고있다
class MPathWalkableMap {
	int m_nSize;
	float m_fScaleX,m_fScaleY;
	bool *m_pWalkable;
	float *m_pHeightMap;
	bool *m_pVisited;	// EraseIsolatedWalkable등 전처리시 필요
	list< pair<int,int> > m_visitNode;

	int *m_pAreaID;
	int m_nAreaIDs;


public:
	MPathWalkableMap();
	~MPathWalkableMap();

	void CreateNew(int nSize,float fScaleX,float fScaleY);
	bool CreateFromFile(const char* szFileName);
	bool SaveFile(const char* szFileName);

	int GetSize() { return m_nSize; }
	float GetScaleX() { return m_fScaleX; }
	float GetScaleY() { return m_fScaleY; }

	bool IsWalkable(int x,int y) { return m_pWalkable[y*m_nSize+x]; }
	void SetWalkable(int x,int y,bool bWalkable) { m_pWalkable[y*m_nSize+x] = bWalkable; }
	
	float GetHeight(int x,int y) { return m_pHeightMap[y*m_nSize+x]; }
	void SetHeight(int x,int y,float fHeight) { m_pHeightMap[y*m_nSize+x] = fHeight; }

	int GetAreaID(int x,int y) { return m_pAreaID[y*m_nSize+x]; }
	void SetAreaID(int x,int y,int nAreaID) { m_pAreaID[y*m_nSize+x] = nAreaID; }

	int GetLinkedCount(int x,int y);
	int EraseIsolatedWalkable(int nLimit);	// nLimit 이하의 고립된 갈수있는 영역을 없앤다. 리턴값은 없앤 갯수

	void CreateVisit()	{ m_pVisited = new bool[m_nSize*m_nSize]; }
	void DestroyVisit() { delete m_pVisited; }
	bool IsVisited(int x,int y) { return m_pVisited[y*m_nSize+x]; }
	void SetVisited(int x,int y,bool bVisit) { m_pVisited[y*m_nSize+x] = bVisit; }

	void Dump(char* szFileName);	// 그림파일로 덤프
	void DumpArea(char* szFileName);	// 영역 정보를 그림으로 덤프
};

class MQuadTree;

class MQuadTreeNode : public MNodeModel{
	friend MQuadTree;

	int m_nAreaID;
	MQuadTreeNode* m_pChildren[4];	

public:
	MQuadTreeNode() { m_pChildren[0]=m_pChildren[1]=m_pChildren[2]=m_pChildren[3]=NULL; }	// 이거 이쁘게 하는거 아시는분 ?
	virtual ~MQuadTreeNode();

	MVector3 m_position;
	vector<MQuadTreeNode*> m_successors;

	int GetAreaID() { return m_nAreaID; }
	MQuadTreeNode* GetChildren(int i) { return m_pChildren[i]; }

	virtual int GetSuccessorCount(void);
	virtual MNodeModel* GetSuccessor(int i);

	// 이웃 Node들의 Cost 얻기
	virtual float GetSuccessorCost(MNodeModel* pSuccessor);
	// Heuristic Estimation에 의한 pNode까지의 Cost 얻기
	virtual float GetHeuristicCost(MNodeModel* pNode);
};

enum QTCREATERESULT {
	WALKABLE,
	UNWALKABLE,
	MIXED
};

enum QTLINKTYPE {
	QTOP = 0,
	QRIGHT,
	QLEFT,
	QBOTTOM,
	QMAX
};
// QMAX - linkType 하면 반대방향


class MQuadTree {
	int m_nSize;
	int m_nNodeCount;
	float m_fScaleX;
	float m_fScaleY;
	MQuadTreeNode* m_pRootNode;
	MPathWalkableMap* m_pWalkableMap;

	MQuadTreeNode* CreateRecurse(QTCREATERESULT *pResult,int x,int y,int nCellSize);

	void FillNode(DWORD *dwBuffer,int nBufferPitch, MQuadTreeNode* pNode, int x,int y,int nCellSize,int nDepth);
	MQuadTreeNode* GetNodeRecurse(float px,float py,int x,int y,MQuadTreeNode* pNode,int nCellSize);

public:
	MQuadTree();
	virtual ~MQuadTree();

	void Create(MPathWalkableMap* pWalkableMap,int x,int y,int nCellSize,float fScaleX,float fScaleY);

	void DumpQuadTree(char* szFileName);
	int GetNodeCount() { return m_nNodeCount; }

	int GetSize()	{ return m_nSize; }

	float GetScaleX() { return m_fScaleX; }
	float GetScaleY() { return m_fScaleY; }

	void LinkNode(MQuadTreeNode *pStartNode, MQuadTreeNode *pToNode, QTLINKTYPE linkType);
	void CreateSuccessor(MQuadTreeNode *pNode);

	MQuadTreeNode* GetNode(float x,float y);
	MQuadTreeNode* GetRootNode() { return m_pRootNode; }
};