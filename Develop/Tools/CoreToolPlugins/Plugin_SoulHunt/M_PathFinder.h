#pragma once

#include "MPathFinder.h"
#include "MPathEngineExporter.h"
#include "MPath.h"

namespace rs3
{
	class RDevice;
	class RCollisionTree;
}

class RHeightField;
// 길찾기를 테스트 하는 코드 클래스
class CORETOOL_SOULHUNT_API CPathFinderTest
{
private:
	struct GVertex
	{
		MVector3 pos;
		DWORD color;
	};

	MPath m_currentPath;
	
	// Obstacle
	vector<MVector3> m_arrayObstacle;

	// Navigation Mesh (충돌/렌더링)
	std::vector<GVertex> m_arrayNavVertices;

	bool m_bRenderNavigationMesh;

	void SetRenderState(RDevice *pDevice);
	void ResetRenderState(RDevice *pDevice);
	
	void fillConnectTriangleFlag(iMesh *mesh, int faceIndex, int value, std::vector<int> &vecFaceIndices);

public:
	std::vector<MVector3> m_arrayInvalidPosition;

public:
	MPathFinder m_PathFinder;

	CPathFinderTest();
	virtual ~CPathFinderTest();

	void Init();
	void Destroy();
	
	bool LoadZone(const std::string &strPath, RHeightField *pHeightField);
	int MakeInvalid(const std::vector<MVector3> &invalidArray);
	int MakeNavMesh();

	void Release();
	void ReleasePath();

	void GetPathVertices(std::vector<RVector> *pPathArrayOut);
	void FindPath(const RVector &vStart, const RVector &vEnd);

	// 현재 3DPosition이 유효한지, 그리고 fZOffset 범위 내에 있는지 검사
	bool IsVailid3DPostiion(const RVector &vPosition, float fZOffset = 100.f);
	// 현재 3DPosition에서 fZUpOffset 만큼 위(Z방향)쪽 위치 사이의 Navigation mesh 기준의 3DPosition 값을 얻어낸다.
	bool GetValid3DPosition(RVector *pOut, const RVector &vPosition, float fZOffset = 100.f, float fZUpOffset = 100.f);

	void SetRenderableNavigationMesh(bool bFlag);

	void RenderNavMesh(RDevice *pDevice);
	void RenderCurrentPath(RDevice *pDevice);
};

extern CORETOOL_SOULHUNT_API CPathFinderTest g_PathfinderTest;