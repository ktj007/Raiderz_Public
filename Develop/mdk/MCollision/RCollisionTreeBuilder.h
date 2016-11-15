#pragma once

#include "RLib.h"
#include "RCollisionTree.h"

namespace rs3 {

// 충돌용 AABB 트리 builder

////////////////////////////////////////////////////////////////////////////////////////////////
// RCollisionTreeBuilder

class RCollisionTreeBuilder;
class RCollisionTreeBuilderNode
{
	friend RCollisionTreeBuilder;
	RCollisionTreeBuilder* m_pBuilder;

	MBox m_bb;
	RCollisionTreeBuilderNode *m_pLeft,*m_pRight;
	vector<RCollisionTriangle*> m_polygons;
	int m_nIndex;
	static int m_nMaxPolygonPerNode;

	RCollisionTreeBuilderNode();
	RCollisionTreeBuilderNode(RCollisionTreeBuilder* pBuilder);
	~RCollisionTreeBuilderNode();

	void Build();

	int	GetNodeCount();
	int GetPolygonCount();

	bool IsLeafNode() { return m_pLeft==NULL; }
};

const int POLYGON_COUNT_BOUND = 10;

class RCollisionTreeBuilder
{
public:
	int m_nTotalPoly;
	int m_nCurrentPoly;

protected:
	bool	m_bIncludeTerrain;
	int m_nSavingTriangleIndex;
	RCollisionTreeBuilderNode* m_rootNode;

	bool Save(FILE *file, RCollisionTreeBuilderNode* pNode);
	bool SaveXml(const char *szFilename);

	void CopyToCollisionTree(RCollisionTree *pCollisionTree, RCollisionTreeNode *pCNode, RCollisionTreeBuilderNode *pBNode);

public:
	// material table
	std::vector<std::string> m_vecMaterialList;
	// 이름과 매치되는 Material Table의 인덱스를 돌려준다.
	DWORD GetZoneMaterialID(const std::string &strMaterialName);
	// 실패한 파일들의 이름을 수집한다.
	std::vector<std::string> m_vecFailedList;

public:
	RCollisionTreeBuilder(void);
	~RCollisionTreeBuilder(void);

	// Builder 로 만들어진 RCollisionTree를 생성한다.
	RCollisionTree* CreateCollisionTree();

	// .cl2 와 .cl2.xml (material table)을 저장한다.
	bool Save(const char* szFileName);
//	void SetIncludeTerrain(bool bInclude);

	// 수동으로 AddPolygon 후 BuildTree 할수있고,
	void AddPolygon(const MVector3& v0, const MVector3& v1, const MVector3& v2, WORD wNodeFlag, WORD wUserFlag, DWORD dwMaterial);
	
	void BuildTree(int nMaxPolygonPerNode = POLYGON_COUNT_BOUND);
	int  GetNumOfTreeToBuild(){ return (int)m_rootNode->m_polygons.size(); }

	const MBox& GetRootAABB();

	// MaxPlugin Export 시 작업 진행률을 알려주기 위한 함수
	virtual void OnUpdateProgress(int, int);
};

}
