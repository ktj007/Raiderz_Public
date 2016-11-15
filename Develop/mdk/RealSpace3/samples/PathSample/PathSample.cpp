#include <windows.h>
#define _CRT_SECURE_NO_DEPRECATE
#include <crtdbg.h>

#include "../sampleApp.h"

#include "MPathFinder.h"
#include "MPathEngineExporter.h"
#include "MPath.h"
#include "MAgent.h"
#include "RCollisionTreeWithHeightField.h"
#include "RRenderHelper.h"
#include "RVType.h"

#include <vector>

/**
 이전 버전에서는 .server.cl2를 사용하였는데 지금은 .cl2와 _dis.dds / .desc 를 같이 참조하여 HeightField Picking을 별도로 처리한다.
 이 샘플의 목적은 Picking 루틴과 PathEngine의 Navigation mesh의 맞물림을 테스트 하는 것이다.

 2007/12/12
*/

using namespace rs3;
using namespace std;

float RandF(float from, float to)
{
	float fLen = to - from;
	if ( fLen == 0.0f )
		return to;

	float fV = (float)::rand()/(float)RAND_MAX;
	return from + (fV*fLen);
}

class PathApplication : public sampleApp
{
public:
	MPathFinder m_pathFinder;
	RCollisionTreeWithHeightField m_collision;

	PathApplication();
	virtual ~PathApplication();

	//
	vector<MVector3> m_vPickingPoints;
	vector<RVt_pos_clr> m_vResultLine;

	char *GetName()
	{
		return "PATHENGINE/CollisionTree testing";
	}

	void OnUpdate();
	bool OnCreate();
	void OnRender();
	void OnDestroy();

	virtual bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);
};

bool PathApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
	return sampleApp::OnWndMessage(hWnd, message, wParam, lParam, pReturn);
}

PathApplication g_App;

PathApplication::PathApplication()
{
	m_pathFinder.Create();
}

PathApplication::~PathApplication()
{
}


//const char* CL2_FILENAME = "../Data/Zone/Pure_Indoor/Pure_Indoor.cl2";
//const char* NAVMESH_FILENAME = "../Data/Zone/Pure_Indoor/Pure_Indoor";

const char *pcCl2Filename = "../Data/Zone/Rengot_Village/Rengot_Village.cl2";
//const char *pcCl2Filename = "../Data/Zone/Ritz_Sewer/Ritz_Sewer.cl2";
const char *pcDisFilename = "../Data/Zone/Rengot_Village/Rengot_Village_dis.dds";
const char *pcNavmeshFilename = "../Data/Zone/Rengot_Village/Rengot_Village";
const char *pcZoneFilename = "../Data/Zone/Rengot_Village/Rengot_Village.zone.xml";

vector<RVector> gvTriBuffer;

static void RenderPoly(MVector3 &v1, MVector3 &v2, MVector3 &v3)
{
	gvTriBuffer.push_back(v1);
	gvTriBuffer.push_back(v2);
	gvTriBuffer.push_back(v3);
}

bool PathApplication::OnCreate()
{
	if ( !GetSceneManager()->LoadRootSceneFromXML(pcZoneFilename))
		return false;
	
	GetSceneManager()->GetPrimaryCamera()->SetNearFarZ(10, 100000.f);

	// 
	if ( !m_collision.LoadCl2File(pcCl2Filename))
		return false;

	if ( !m_collision.LoadHeightField(pcDisFilename, MVector2(60000, 60000)))
		return false;

	if ( !m_pathFinder.AddZone(0, pcNavmeshFilename))
		return false;

	m_collision.CallbackTreeCl2(NULL, RenderPoly);

	// .cl2 에 Picking 되는 점을 찾아서 +100.f 높이에서 저장한다.
	const int RANDOM_COUNT = 80000;
	m_collision.UpdateAABB();

	RBoundingBox aabb = m_collision.GetBoundingBox();
	m_vPickingPoints.reserve(RANDOM_COUNT);
	for ( int i = 0; i<RANDOM_COUNT; )
	{
		RVector from(RandF(aabb.vmin.x, aabb.vmax.x), RandF(aabb.vmin.y, aabb.vmax.y), RandF(aabb.vmin.z,aabb.vmax.z));
		RVector to(from.x, from.y, aabb.vmin.z);

		RCollisionPickInfo info;
		info.m_inPickOrigin = from;
		info.m_inPickTo = to;

		if ( m_collision.Pick(info))
		{
			info.m_outPosition.z += 1.f;
			m_vPickingPoints.push_back(info.m_outPosition);

			RVt_pos_clr vStart;
			vStart.vPos = info.m_outPosition;
			vStart.dwColor = D3DCOLOR_XRGB(255, 255, 255);

			RVt_pos_clr vEnd;
			vEnd.vPos = info.m_outPosition + info.m_outNormal * 200.f;
			vEnd.dwColor = D3DCOLOR_XRGB(255, 0, 0);

			m_vResultLine.push_back(vStart);
			m_vResultLine.push_back(vEnd);
			++i;
		}
	}

	// 유효도 테스트
#if 0 
	m_vResultLine.reserve(RANDOM_COUNT*2);
	for ( int i = 0; i<RANDOM_COUNT; ++i )
	{
		float fHeight;
		if ( m_pathFinder.IsValidPosition(&fHeight, 0, NULL, m_vPickingPoints[i])==PATH_SUCCESS)
		{
			if ( m_vPickingPoints[i].z - fHeight < 200 )
			{
				// 유효한 것만 Line picking
				RVector vPos = m_vPickingPoints[i];
				m_vResultLine.push_back(vPos);
				vPos.z = fHeight;
				m_vResultLine.push_back(vPos);
			}
		}
	}

#endif

	m_pDevice->SetFillMode(RFILL_WIRE);
	
	return true;
}

void PathApplication::OnRender()
{
	RRenderHelper::SetRenderState();

	RBoundingBox aabb = m_collision.GetBoundingBox();

	m_pDevice->SetFvF(D3DFVF_XYZ);
	RRenderHelper::RenderBox(RMatrix::IDENTITY, aabb, D3DCOLOR_XRGB(255, 255, 0));

	// .cl2 직접 렌더링
	{
		m_pDevice->SetFillMode(RFILL_WIRE);
		m_pDevice->SetTextureFactor(D3DCOLOR_XRGB(0, 0, 0));

		const int DRAW_VERTEX_COUNT = 3*1500;
		size_t nCount = 0;
		while ( nCount < gvTriBuffer.size())
		{
			size_t nTricount;
			nTricount = (gvTriBuffer.size()-nCount);
			if ( nTricount > DRAW_VERTEX_COUNT )	nTricount = DRAW_VERTEX_COUNT;
			m_pDevice->DrawPrimitiveUP(RPT_TRIANGLELIST, (int)(nTricount)/3,&gvTriBuffer[nCount], sizeof(RVector));
			nCount += nTricount;
		}
	}

	if ( m_vPickingPoints.empty() == false && 0)
	{
		m_pDevice->SetFvF(D3DFVF_XYZ);
		m_pDevice->SetTextureFactor(D3DCOLOR_XRGB(255, 255, 255));
		m_pDevice->DrawPrimitiveUP(RPT_POINTLIST, (int)m_vPickingPoints.size(),&m_vPickingPoints[0], sizeof(MVector3));
	}

	if ( m_vResultLine.empty() == false)
	{
		m_pDevice->SetFvF(RVt_pos_clr::FVF);

		m_pDevice->SetTextureStageState(0, RTSS_COLORARG1, RTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, RTSS_ALPHAARG1, RTA_DIFFUSE);

		m_pDevice->DrawPrimitiveUP(RPT_LINELIST, (int)m_vResultLine.size()/2,&m_vResultLine[0], sizeof(RVt_pos_clr));
	}

	RRenderHelper::EndRenderState();
}

void PathApplication::OnUpdate()
{
	sampleApp::OnUpdate();
}

void PathApplication::OnDestroy()
{
	sampleApp::OnDestroy();
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	return g_App.Run();
}