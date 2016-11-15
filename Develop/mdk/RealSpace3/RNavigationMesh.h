#ifndef _RNAVIGATIONMESH_H
#define _RNAVIGATIONMESH_H

#include "RTypes.h"
#include "RLine.h"
#include "RAStar.h"
#include <vector>
#include <list>
using namespace std;

namespace rs3 {

class RNavigationNode;

// 네비게이션 메쉬 페이스
struct RNavFace
{
	unsigned short	v1, v2, v3;
};


class RNavigationPath
{
public:
	RNavigationPath() {}
	virtual ~RNavigationPath() { }
};

class RNavigationMesh
{
public:
	// --- 타입 정의 -------------
	typedef	std::vector<RNavigationNode*> RNodeArray;
private:
	// --- data ------------------
	RNodeArray			m_NodeArray;
	list<RVector>		m_WaypointList;

	RAStar				m_AStar;
	RNavigationNode*	m_pStartNode, *m_pGoalNode;
	int					m_nVertCount;
	int					m_nFaceCount;
	RVector*			m_vertices;
	RNavFace*			m_faces;


	void AddNode(int nID, const RVector& PointA, const RVector& PointB, const RVector& PointC);
	void LinkNodes();
	void MakeNodes();
	void ClearNodes();
public:
	// --- func ------------------
	RNavigationMesh();
	virtual ~RNavigationMesh();
	void Clear();
	bool Open(const char* szFileName);
	bool Save(const char* szFileName);
	void ClearAllNodeWeight();

	RNavigationNode* FindClosestNode(const RVector& point) const;
	bool BuildNavigationPath(RNavigationNode* pStartNode, 
							 const RVector& StartPos, 
							 RNavigationNode* pEndNode, 
							 const RVector& EndPos);

	// Path Finding
	RVector SnapPointToNode(RNavigationNode* pNode, const RVector& Point);
	RVector SnapPointToMesh(RNavigationNode** NodeOut, const RVector& Point);
	bool LineOfSightTest(RNavigationNode* pStartNode, const RVector& StartPos, RNavigationNode* pGoalNode, const RVector& EndPos);
	bool BuildNavigationPath(const RVector& vStartPos, const RVector& vGoalPos);



	RNodeArray* GetNodes()		{ return &m_NodeArray; }
	int GetNodeCount()			{ return (int)m_NodeArray.size(); }
	list<RVector>&	GetWaypointList() { return m_WaypointList; }

	// 테스트용 ------------------
//	void Render();
//	void RenderLinks();

	// exporter에서 사용 ---------
	inline void InitVertices(int vert_count);
	inline void InitFaces(int face_count);
	inline void SetVertex(int index, RVector& v);
	inline void SetFace(int index, RNavFace& f);
};




// inline /////////////////////////////////////////////////////////////////////
inline void RNavigationMesh::InitVertices(int vert_count)
{
	if (m_vertices != NULL) delete [] m_vertices;
	m_nVertCount = vert_count;
	m_vertices = new RVector[vert_count];
}
inline void RNavigationMesh::InitFaces(int face_count)
{
	if (m_faces != NULL) delete [] m_faces;
	m_nFaceCount = face_count;
	m_faces = new RNavFace[face_count];
}

inline void RNavigationMesh::SetVertex(int index, RVector& v)
{
	m_vertices[index] = v;
}

inline void RNavigationMesh::SetFace(int index, RNavFace& f)
{
	m_faces[index] = f;
}

}

#endif
