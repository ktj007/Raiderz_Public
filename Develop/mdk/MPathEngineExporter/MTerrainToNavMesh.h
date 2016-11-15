#pragma once

#include "MVector3.h"
#include "RTerrain.h"
#include "../../../sdk/pathengine/i_pathengine.h"

using namespace rs3;
// pathengine용으로 지형을 뽑아내는 클래스

// 지형의 일부를 표현하는 직각 이등변 삼각형
class MTerrainTriangle
{
protected:
		MTerrainTriangle*	m_pChild1;		// p1-p2-(빗변중점)
		MTerrainTriangle*	m_pChild2;		// p1-p3-(빗변중점)
		MTerrainTriangle*	m_pParent;		// 부모
		float				m_fError;		// 정확한 Z값과 비교해서 얼마나 오차가 있는가

		MTerrainTriangle*	m_pLeft;		// p1-p2에 접하는 삼각형
		MTerrainTriangle*	m_pRight;		// p1-p3에 접하는 삼각형
		MTerrainTriangle*	m_pDiagonal;	// p2-p3 (빗변) 에 접하는 삼각형

		bool				m_bSteep;		// 경사져서 삭제되어야 하는가
		bool				m_bSplited;		// 분할되었는가?
		int					m_nLevel;		// 더 분할할 수 있는 횟수

		MVector3			p1, p2, p3;		// p1의 꼭지각 = 90도	p2-p3 : 빗변

		
		// 중간에 어느 삼각형이 분할되었다면 그 부모들까지 다 분할될 필요가 있다.
		void				SplitParent();	

public:
		MTerrainTriangle();
		virtual ~MTerrainTriangle();

		// 삼각형을 세팅한다. 제일 큰 삼각형 두개를 세팅할때 사용.
		void				Set(const MVector3& _p1, const MVector3& _p2, const MVector3& _p3, MTerrainTriangle* pDiag, int nLevel);

		// 이 삼각형 밑으로 최대 분할 횟수 제한까지 쭉 분할한 삼각형 트리를 만든다.
		void				MakeFullTree(RTerrain* pTerrain);

		// 이 삼각형 밑의 삼각형들에 좌, 우, 대각선 방향의 삼각형들을 제대로 세팅한다.
		void				SetLeftRightDiagonalNode();

		// 이 삼각형 밑의 삼각형을 모든 삼각형이 오차 fThreshold 안에 들어갈 때까지 분할한다.
		void				Split(float fThreshold);

		// 이 삼각형 밑의 모든 노드를 파괴한다.
		void				Destroy();
		void				GetTriangleList(vector<MVector3>& vec);

		// 경사진 노드 체크 & 속성 체크
		void				ClipSteepPlane(RTerrain *pTerrain, float fLimitDot);

		// 평평하게 만든다
		int					Flatten();
};


// 지형을 적당히 삼각형 메쉬로 근사하는 클래스
class MExportedTerrain
{
		friend class RTerrain;
protected:
		MTerrainTriangle	tri1;
		MTerrainTriangle	tri2;

		vector<MVector3>	trivec;
		vector<int>			indexVertsRef;

		vector< vector<int> > adjData;

		vector<int>			faceID;
		vector<int>			visited;
		vector<int>			faceGroupSize;

		void				SetInitialTriangles(RTerrain* pTerrain);
		void				Build(RTerrain* pTerrain, float fErrorThreshold);

		void				BuildIndexedForm();
		void				FlipCCWFaces();
		void				BuildAdjFaceList();
		void				FloodFillFaces();
		void				RemoveDisconnect(RTerrain *pTerrain, vector<MVector3> &seeds);
		void				ClipTooSmallChunk(int nLimitSize);
		void				RemoveNoneIndex();
public:
		vector<MVector3>	indexVerts;
		vector<int>			indices;

		void				BuildAll(RTerrain* pTerrain, vector<MVector3> &seeds, float fErrorThreshold, float fLimitDot, int nLimitSize);
		void				DestroyBuffers();

		MExportedTerrain();
		virtual ~MExportedTerrain();
};



class MExportedTerrainMesh : public iFaceVertexMesh
{
public:
		MExportedTerrain eTerrain;

		long faces() const
		{
			return (long)eTerrain.indices.size() / 3;
		}
		long vertices() const
		{
			return (long)eTerrain.indexVerts.size();
		}

		long vertexIndex(long face, long vertexInFace) const
		{
			return (long)eTerrain.indices[face * 3 + (2 - vertexInFace)];
		}
		long vertexX(long vertex) const
		{
			return (long)eTerrain.indexVerts[vertex].x;
		}
		long vertexY(long vertex) const
		{
			return (long)eTerrain.indexVerts[vertex].y;
		}
		float vertexZ(long vertex) const
		{
			return eTerrain.indexVerts[vertex].z;
		}
		long faceAttribute(long face, long attributeIndex) const
		{
			switch ( attributeIndex )
			{
			case PE_FaceAttribute_SectionID:	// 지형 Section ID 는 0으로 지정
				return 0;
			}

			return -1;
		}

		MExportedTerrainMesh();
		virtual ~MExportedTerrainMesh();
};