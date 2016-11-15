#pragma once

#include "RLib.h"
#include "MFileBuffer.h"


namespace rs3 {

class RCollision;

// 충돌용 AABB 트리
struct RCollisionTriangle
{
	enum ATTRIBUTE	// Bit flag, 따라서 31개 까지 가능
	{
		NONE	= 0,
		WATER	= 0x01,
		MESH	= 0x02,
		TREE	= 0x04,
		TERRAIN = 0x08,
		// ...
		ALL		= 0xffff
	};
	RCollisionTriangle()
		: dwNodeAttribute(NONE)
		, dwUserMaterial(0)
	{ }

	MVector3 v[3];
	DWORD dwNodeAttribute;	// 비트플래그를 사용하는 속성
	DWORD dwUserMaterial;	// 사용자 재질(unsigned 32 비트 ID)
};

////////////////////////////////////////////////////////////////////////////////////////////////
// RCollisionTree
class RCollisionTree;
class RCollisionTreeBuilder;

class RCollisionTreeImpl;
class RCollisionTreeImpl_v0;
class RCollisionTreeImpl_v1;

class RCollisionTreeNode
{
	friend RCollisionTree;
	friend RCollisionTreeBuilder;
	friend RCollisionTreeImpl;
	friend RCollisionTreeImpl_v0;
	friend RCollisionTreeImpl_v1;

protected:
	MBox m_bb;
	RCollisionTreeNode *m_pLeft,*m_pRight;
	int m_nPolygonCount;
	RCollisionTriangle *m_pTriangles;

public:
	const RCollisionTriangle&	GetTriangles(int nIndex) const	{ return m_pTriangles[nIndex];}
	const MBox&			GetBoundBox() const				{ return m_bb; }

	int							GetPolygonCount() const	{ return m_nPolygonCount; }
	bool						IsLeafNode() const		{ return m_pLeft==NULL; }

	const RCollisionTreeNode*	GetLeftNode() const		{ return m_pLeft; }
	const RCollisionTreeNode*	GetRightNode() const	{ return m_pRight; }

};

class RCollisionPickInfo	// picking을 호출할때 사용되는 구조체
{
private:
	friend RCollisionTree;
	MVector3 m_tempPickDir;		// 내부에서 사용됨
	float	m_tempToDistance;

public:
	MVector3	m_inPickOrigin;		// 입력 : pick의 시작점.		-> 위의 두개를 채워주고 호출
	MVector3 m_inPickTo;			// 입력 : pick의 끝점.
	MVector3 m_outPosition;		// 반환 : pick된 지점			-> 리턴값은 아래 두개
	MVector3 m_outNormal;		// 반환 : pick된 지점의 normal
	float	m_outDistanceSq;	// 반환 : PickOrigin에서의 거리의 제곱
	WORD	m_outNodeAttr;		// 반환 : Picking Face의 속성 (WORD , RCollisionTriangle의 enum 으로 정의) - LoWord
	WORD	m_outUserAttr;		// 반환 : User가 지정한 속성 (WORD) - HiWord
	DWORD	m_outMaterial;		// 반환 : Picking Face의 재질
	const char*	m_outMaterialName;	// 반환 : Material의 재질 이름, 찾을 수 없으면 NULL

	float GetPickDistance() { return m_tempToDistance; }	// pick 시작~ 끝거리
};

// 읽기 전용
class RCollisionTree
{
	friend RCollisionTreeBuilder;
	friend RCollisionTreeImpl;
	friend RCollisionTreeImpl_v0;
	friend RCollisionTreeImpl_v1;

private:
	// 로딩을 위해 임시로 필요한 멤버
	RCollisionTriangle *m_pLoadingTriangle;
	RCollisionTreeNode *m_pLoadingNode;

	int					m_nTriangles;
	RCollisionTriangle *m_pTriangles;
	int					m_nNodeCount;
	RCollisionTreeNode *m_pRootNode;

	std::string		m_strCurrentFile;

	// 재질 이름/ID 테이블 ( .cl2.xml 에서 로드 )
	std::vector<std::string> m_vecMaterialTable;

	/// Picking 내부 함수, 삼각형 감기는 방향을 인지하느냐 무시하느냐 ...
	void PickTriangle(RCollisionTreeNode *pNode, RCollisionPickInfo &pickInfo, DWORD dwAttributeMask
		, bool (*IntersectFunc)(const MVector3&, const MVector3&, MVector3&, MVector3&, MVector3&, MVector3*) );

public:
	// callback interface
	class ICallback
	{
	public:
		virtual void OnNode(const RCollisionTreeNode *pNode) = 0;
		virtual void OnTriangle(const RCollisionTriangle *pTriangle) = 0;
	};

public:
	RCollisionTree(void);
	~RCollisionTree(void);

	void Destroy();	// 초기화

	bool Load(const char* szFileName);

	/// 월드의 바운딩을 구합니다. ( load되어있어야만 합니다 )
	const MBox& GetBoundingBox();

	/// 충돌을 검사한다. pickInfo.origin - pickInfo.to 로 가는길에 충돌하면 true를 리턴.
	virtual bool Pick(RCollisionPickInfo& pickInfo, DWORD dwAttributeMask , bool bNoCull);

	/// collision 이 this에 잠재적으로 충돌가능한 폴리곤을 모읍니다
	virtual void GetCollidablePolygons(RCollision &collision, DWORD dwAttributeMask);

	/// 디버그용 AABB 콜백 함수
	void CallbackTree(ICallback *piCallback);

	RCollisionTriangle* GetTriagles(){return m_pTriangles;}
	int					GetTriagleCount(){return m_nTriangles;}

	// root 관련 정보
	const RCollisionTreeNode*	GetRootNode(){ return m_pRootNode; }
	const MBox&			GetRootAABB();

	const std::string&			GetMaterialName(DWORD dwID);

protected:
	void CallBackTreeNode(RCollisionTreeNode *pNode, ICallback *piCallback);
	void GetCollidablePolygons(RCollisionTreeNode *pNode,RCollision &collision, DWORD dwAttributeMask);
};

inline 	const MBox& RCollisionTree::GetBoundingBox() { return m_pRootNode->m_bb; }
inline	const std::string& RCollisionTree::GetMaterialName(DWORD dwID)
{
	return m_vecMaterialTable[dwID];
}

}
