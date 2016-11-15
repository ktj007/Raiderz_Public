#include "stdafx.h"
#include "M_PathFinder.h"
#include "RDevice.h"
#include "RRenderHelper.h"
#include "RMeshUtil.h"
#include "RCollisionTreeBuilder.h"
#include "RCollisionTree.h"

CPathFinderTest g_PathfinderTest;

using namespace rs3;
using namespace std;

CPathFinderTest::CPathFinderTest()
: m_bRenderNavigationMesh(false)
{
}

CPathFinderTest::~CPathFinderTest()
{
	Destroy();
}

void CPathFinderTest::SetRenderableNavigationMesh(bool bFlag)
{
	m_bRenderNavigationMesh = bFlag;
}

void CPathFinderTest::Init()
{
	m_PathFinder.Create();
}

void CPathFinderTest::Destroy()
{
	m_arrayNavVertices.clear();

	m_PathFinder.Destroy();
}

void CPathFinderTest::fillConnectTriangleFlag(iMesh *mesh, int faceIndex, int value, std::vector<int> &vecFaceIndices)
{
	_ASSERT(mesh!=NULL);

	list<int> indicies;
	indicies.push_back( faceIndex );

	_ASSERT(vecFaceIndices[faceIndex]==0);
	vecFaceIndices[faceIndex] = value;

	while( !indicies.empty() )
	{
		int nIndex = indicies.front();

		indicies.pop_front();

		for (int i = 0; i<3; ++i)
		{
			int adjIndex = mesh->get3DFaceConnection(nIndex, i);
			if (adjIndex == -1)
				continue;
			if (vecFaceIndices[adjIndex] != 0)
				continue;

			_ASSERT(vecFaceIndices[adjIndex]==0);
			vecFaceIndices[adjIndex] = value;
			indicies.push_back( adjIndex );
		}
	}
}

bool CPathFinderTest::LoadZone(const string &strPath, RHeightField *pHeightField)
{
	m_PathFinder.ClearZone();
	
	std::vector<string> vShapeNameArray;
	vShapeNameArray.resize(1);
	vShapeNameArray[0] = "small";

	bool bResult = m_PathFinder.AddZone(0, strPath, vShapeNameArray, pHeightField, true);
	if ( !bResult )
	{
		//AfxMessageBox("Path 파일을 찾을 수 없거나, Shape 개체가 없습니다", MB_OK | MB_ICONSTOP);
		return false;
	}

	return true;
}

// 유효하지 않은 Ex_를 그리는 정보를 담는다.
int CPathFinderTest::MakeInvalid(const std::vector<MVector3> &invalidArray)
{
	m_arrayInvalidPosition.clear();
	m_arrayInvalidPosition.resize(invalidArray.size());

	for ( size_t si = 0; si<invalidArray.size(); ++si )
		m_arrayInvalidPosition[si] = invalidArray[si];

	return 1;
}

int CPathFinderTest::MakeNavMesh()
{
	if ( m_PathFinder.GetMesh(0) == NULL )
		return 0;

	iMesh *mesh = m_PathFinder.m_vZoneList[0].m_pMesh;
	_ASSERT(mesh!=NULL);

	m_PathFinder.GetStaticObstacleDebugMesh(0, m_arrayObstacle, 100.f);
	// Navigation Mesh 보여주는 버텍스들 생성
	long FaceNumber = mesh->getNumberOf3DFaces();
	if ( FaceNumber == 0 )
		return 0;

	/**
	 navigation mesh의 connection 여부를 판단하여, connection 된 mesh들간을 칠한다.

	*/

	std::vector<int> vecFaceIndices;
	vecFaceIndices.resize(FaceNumber);

	::fill(vecFaceIndices.begin(), vecFaceIndices.end(), 0);

	int nColorIndex = 0;
	while ( true )
	{
		int nStartFace = -1;
		for ( int i = 0; i<FaceNumber; ++i )
		{
			if ( vecFaceIndices[i] == 0 )
			{
				nStartFace = i;
				break;
			}
		}

		if ( nStartFace == -1 )
			break;

		++nColorIndex;
		fillConnectTriangleFlag(mesh, nStartFace, nColorIndex, vecFaceIndices);
	}

	int TriangleNumber = FaceNumber*3;

	m_arrayNavVertices.clear();
	m_arrayNavVertices.resize(TriangleNumber);

	// COLOR TABLE
	const int COLORTABLE_NUM = 5;
	DWORD colorTable[COLORTABLE_NUM] = 
	{
		D3DCOLOR_ARGB(171, 72, 0, 255),
		D3DCOLOR_ARGB(171, 0, 216, 255),
		D3DCOLOR_ARGB(171, 144, 0, 144),
		D3DCOLOR_ARGB(171, 255, 216, 0),
		D3DCOLOR_ARGB(171, 128, 0, 0)
	};

	for ( int i = 0; i<FaceNumber; ++i )
	{
		DWORD dwFaceColor = D3DCOLOR_XRGB(250, 250, 250);
		if ( vecFaceIndices[i] != 0 )
		{
			int nColorIndex = min(vecFaceIndices[i]-1, COLORTABLE_NUM-1);
			dwFaceColor = colorTable[nColorIndex];
		}

		RVector v[3];

		for ( int k = 0; k<3; ++k )
		{
			tSigned32 x, y;
			float z;
			mesh->get3DFaceVertex(i, k, x, y, z);
			//if ( mesh->get3DFaceAttribute(i, PE_FaceAttribute_SectionID) == 0 )
			//	dwFaceColor |= D3DCOLOR_ARGB(0, 250, 0, 0);

			v[k].Set( (float)x, (float)y, (float)z );

			int nIndex = i*3+k;
			m_arrayNavVertices[nIndex].pos.Set((float)x, (float)y, (float)z);
			m_arrayNavVertices[nIndex].color = dwFaceColor;
		}
	}

	return FaceNumber;
}

void CPathFinderTest::ReleasePath()
{
	m_currentPath.Release();
}

void CPathFinderTest::Release()
{
	m_currentPath.Release();
	m_arrayNavVertices.clear();
}

void CPathFinderTest::FindPath(const RVector &vStart, const RVector &vEnd)
{
	ReleasePath();
	int nSize = m_PathFinder.GetShapeRange("small");
	
	RVector st(vStart);
	RVector en(vEnd);

	if ( PATH_SUCCESS!= m_PathFinder.IsValidPosition(NULL, 0, NULL, vStart))
	{
		MessageBox(NULL,"유효하지 않은 시작점!", "Error", MB_OK);
		return;
	}

	if ( PATH_SUCCESS!= m_PathFinder.IsValidPosition(NULL, 0, NULL, vEnd))
	{
		MessageBox(NULL,"유효하지 않은 끝!", "Error", MB_OK);
		return;
	}

	bool bResult = m_PathFinder.FindPath(0, st, en, m_currentPath, NULL);
	if ( bResult == false )
	{
		MessageBox(NULL,"길찾기 실패!", "Error", MB_OK);
	}

	mlog("* start pos(%f, %f, %f)\n* end pos(%f, %f, %f)\n", st.x, st.y, st.z, en.x, en.y, en.z);
}

bool CPathFinderTest::IsVailid3DPostiion(const RVector &vPosition, float fZOffset)
{
	iMesh *mesh = m_PathFinder.GetIMesh(0);
	if ( mesh == NULL )
		return false;

	float fHeight = vPosition.z;
	bool bValidNavimesh = m_PathFinder.IsValidPosition(&fHeight, 0, NULL, vPosition)==PATH_SUCCESS;
	bool bValidPosition = ((vPosition.z - fHeight) < fZOffset && (vPosition.z - fHeight) >=0.0f);	// 높이 차 100 이내

	return (bValidNavimesh && bValidPosition);
}

bool CPathFinderTest::GetValid3DPosition(RVector *pOut, const RVector &vPosition, float fZOffset , float fZUpOffset)
{
	iMesh *mesh = m_PathFinder.GetIMesh(0);
	if ( mesh == NULL )
		return false;

	RVector vAbovePos = vPosition + RVector(0, 0, fZUpOffset-0.9f);
	float fHeight;
	if ( m_PathFinder.IsValidPosition(&fHeight, 0, NULL, vAbovePos)==PATH_SUCCESS)
	{
		if ( pOut )
		{
			pOut->x = vAbovePos.x;
			pOut->y = vAbovePos.y;
			pOut->z = fHeight + 1.0f;
		}

		if (vPosition.z - fZOffset < fHeight)
			return true;

	}

	return false;
}

void CPathFinderTest::SetRenderState(RDevice *pDevice)
{
	pDevice->SetTexture(0, R_NONE);
	pDevice->SetTexture(1, R_NONE);
	pDevice->SetBlending(RBLEND_NONE);
	pDevice->SetLighting(false);
	pDevice->ShaderOff();

	pDevice->SetAlphaTestEnable(false);
	pDevice->SetDepthEnable(true, false);

	pDevice->SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, RTSS_COLORARG1, RTA_DIFFUSE);
	pDevice->SetTextureStageState(1, RTSS_COLOROP, RTOP_SELECTARG1);
	pDevice->SetTextureStageState(1, RTSS_COLORARG1, RTA_CURRENT);

	pDevice->SetTextureStageState(0, RTSS_ALPHAOP, RTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, RTSS_ALPHAARG1, RTA_DIFFUSE);
	pDevice->SetTextureStageState(1, RTSS_ALPHAOP, RTOP_DISABLE);
	pDevice->SetCullMode(RCULL_CCW);
}

void CPathFinderTest::ResetRenderState(RDevice *pDevice)
{
	pDevice->SetTexture(0, R_NONE);
	pDevice->SetTexture(1, R_NONE);
	pDevice->SetBlending(RBLEND_NONE);
	pDevice->SetDepthEnable(true);

	pDevice->SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, RTSS_COLORARG1, RTA_DIFFUSE);
	pDevice->SetTextureStageState(1, RTSS_COLOROP, RTOP_SELECTARG1);
	pDevice->SetTextureStageState(1, RTSS_COLORARG1, RTA_CURRENT);

	pDevice->SetTextureStageState(0, RTSS_ALPHAOP, RTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, RTSS_ALPHAARG1, RTA_DIFFUSE);
	pDevice->SetTextureStageState(1, RTSS_ALPHAOP, RTOP_DISABLE);
	pDevice->SetCullMode(RCULL_CCW);
}

void CPathFinderTest::RenderCurrentPath(rs3::RDevice *pDevice)
{
	std::vector<MVector3> vecArray;
	if ( m_currentPath.IsValid())
	{
		RRenderHelper::SetRenderState();

		vecArray.clear();
		m_PathFinder.PathTo3DPoints(vecArray, 0, m_currentPath, 0);

		pDevice->SetFvF(D3DFVF_XYZ);
		pDevice->SetTextureFactor(D3DCOLOR_XRGB(255, 255, 255));
		pDevice->SetDepthEnable(false);
		pDevice->DrawPrimitiveUP(RPT_LINESTRIP, vecArray.size()-1, &(vecArray[0]), sizeof(RVector));	
		pDevice->SetDepthEnable(true);
		RRenderHelper::EndRenderState();
	}
}

void CPathFinderTest::RenderNavMesh(rs3::RDevice *pDevice)
{
	if ( m_arrayNavVertices.empty())
		return;

	if (!m_bRenderNavigationMesh )
		return;

	// Navigation mesh 렌더링
	SetRenderState(pDevice);
	pDevice->SetTextureStageState(0, RTSS_COLOROP, RTOP_MODULATE);
	pDevice->SetTextureStageState(0, RTSS_COLORARG1, RTA_DIFFUSE);
	pDevice->SetTextureStageState(0, RTSS_COLORARG2, RTA_TFACTOR);

	pDevice->SetDepthEnable(true, false);
	pDevice->SetDepthBias(0,-0.006f);

	pDevice->SetFvF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	pDevice->SetTextureFactor(D3DCOLOR_XRGB(255,255,255));

	// 검은색 WIRE
	if ( GetAsyncKeyState(VK_SPACE))
	{
		RMatrix matZup100;
		matZup100.MakeIdentity();
		matZup100.SetTranslation(RVector(0, 0, 99));
		pDevice->SetTransform(RST_WORLD, matZup100);
	}
	else
	{
		pDevice->SetTransform(RST_WORLD, RMatrix::IDENTITY);
	}

	pDevice->SetFillMode(RFILL_WIRE);
	pDevice->DrawPrimitiveUP(RPT_TRIANGLELIST, m_arrayNavVertices.size()/3, &(m_arrayNavVertices[0]), sizeof(GVertex));

	// 색상 폴리곤 
	pDevice->SetBlending(RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
	pDevice->SetFillMode(RFILL_SOLID);
	pDevice->DrawPrimitiveUP(RPT_TRIANGLELIST, m_arrayNavVertices.size()/3, &(m_arrayNavVertices[0]), sizeof(GVertex));

	pDevice->SetDepthBias(0, 0);
	pDevice->SetBlending(RBLEND_NONE);

	if ( !m_arrayObstacle.empty())
	{
		pDevice->SetTextureFactor(D3DCOLOR_XRGB(255, 0, 0));
		pDevice->SetTextureStageState(0, RTSS_COLORARG1, RTA_TFACTOR);

		pDevice->SetFvF(D3DFVF_XYZ);
		pDevice->SetCullMode(RCULL_NONE);
		pDevice->DrawPrimitiveUP(RPT_TRIANGLELIST, m_arrayObstacle.size()/3,&(m_arrayObstacle[0]), sizeof(MVector3));
		pDevice->SetCullMode(RCULL_CCW);
	}

	if ( !m_arrayInvalidPosition.empty())
	{
		pDevice->SetTextureFactor(D3DCOLOR_XRGB(0, 1, 1));
		pDevice->SetTextureStageState(0, RTSS_COLORARG1, RTA_TFACTOR);

		for ( size_t si = 0; si<m_arrayInvalidPosition.size(); ++si )
			RRenderHelper::RenderCylinder(m_arrayInvalidPosition[si], RVector::AXISZ, 50.f, 100.f, D3DCOLOR_XRGB(0, 0, 1));
	}
	// Obstacle rendering

	ResetRenderState(pDevice);
}
