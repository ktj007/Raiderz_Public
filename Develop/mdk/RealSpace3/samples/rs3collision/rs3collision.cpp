// HelloRS3.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>

#include "crtdbg.h"
#include "mmsystem.h"

#include "MDebug.h"

#include "RD3DApplication.h"
#include "RDeviceD3D.h"
#include "RSceneManager.h"
#include "RStaticMeshSceneNode.h"
#include "RSimpleRotationSceneNode.h"
#include "RMultiPassLightSceneNode.h"
#include "RSimpleSphereSceneNode.h"
#include "RSimpleBoxSceneNode.h"
#include "RMeshMgr.h"
#include "RActor.h"
#include "../sampleApp.h"
#include "RCollisionTree.h"
#include "RRenderHelper.h"

#include "RCollision.h"
#include "RUtil.h"
//#include "RLightMapRenderer.h"

#include <algorithm>

using namespace std;
using namespace rs3;

#include "d3dx9.h"

class myApplication : public sampleApp 
{
	bool m_bCheckCollision;

	RStaticMeshSceneNode *m_pTree;
	RSimpleSphereSceneNode *m_pCollisionSphere;
	RCollisionSphere *m_pCollision;
	RCollisionTree *m_pCollisionTree;

	RActor			*m_pActor;

public:
	myApplication() : sampleApp(), m_pCollision(NULL), m_bCheckCollision(false) { }

	char *GetName() 
	{ 
		return "rs3 collision sample application"; 
	}
	bool OnCreate();
	void OnDestroy();

	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);

	void OnUpdate();
	void OnRender();

protected:
	void OnPickTest();
} g_App;

#define		__RANDOM ((float)rand()/(float)RAND_MAX)

void myApplication::OnPickTest()
{
	RECT rt;
	GetClientRect(GetWindowHandle(),&rt);

	RVector p, d, q;

	RGetScreenLine(600, 300, rt, m_pCamera->GetViewMatrix(), m_pCamera->GetProjectionMatrix(),&d);

	RVector pickOrigin = m_pCamera->GetPosition();
	RVector pickTo = m_pCamera->GetPosition() + 10000.f*d;
}

bool myApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
    switch (message)
    {
	case WM_KEYDOWN:
	break;
	}
	return sampleApp::OnWndMessage(hWnd,message,wParam,lParam,pReturn);
}

vector<RVector> vPointList;
vector<RVector> vLineList;

const int RANDOM_COUNT = 10000;

float RandF(float from, float to)
{
	float fLen = to - from;
	if ( fLen == 0.0f )
		return to;

	float fV = (float)::rand()/(float)RAND_MAX;
	return from + (fV*fLen);
}

bool myApplication::OnCreate()
{
	m_pCamera->SetNearFarZ(10.f,100000.f);

	m_pCollisionTree = new RCollisionTree;
	if ( !m_pCollisionTree->Load("Teress_Plain.cl2")) 
		return false;

	RBoundingBox aabb = m_pCollisionTree->GetRootAABB();
	vPointList.clear();
	vPointList.reserve(RANDOM_COUNT);

	for ( int i = 0; i<RANDOM_COUNT; ++i )
	{
		RVector from(RandF(aabb.vmin.x, aabb.vmax.x), RandF(aabb.vmin.y, aabb.vmax.y), aabb.vmax.z);
		RVector to(from.x, from.y, aabb.vmin.z);

		RCollisionPickInfo info;
		info.m_inPickOrigin = from;
		info.m_inPickTo = to;

		if ( m_pCollisionTree->Pick(info, RCollisionTriangle::WATER, true))
		{
			const RVector &vPos = info.m_outPosition;
			const RVector &vDir = info.m_outNormal;

			vPointList.push_back(vPos);
			vLineList.push_back(vPos);
			vLineList.push_back(vPos + RVector::AXISZ*100.f);
		}
	}

	return true;
}

void myApplication::OnDestroy()
{
	SAFE_DELETE(m_pCollisionTree);
}

void myApplication::OnUpdate()
{
	sampleApp::OnUpdate();
}

void RenderAABB(const RBoundingBox *pAABB)
{
	RBoundingBox bbox = *pAABB;
	RRenderHelper::RenderBox(RMatrix::IDENTITY, bbox);
}

void RenderPoly(RVector &v1, RVector &v2, RVector &v3)
{
	RVector Tri[3];
	Tri[0] = v1;
	Tri[1] = v2;
	Tri[2] = v3;

	RRenderHelper::RenderTriangles(RMatrix::IDENTITY, Tri, 3, D3DCOLOR_XRGB(0, 0, 0));
}

void myApplication::OnRender()
{
	RRenderHelper::SetRenderState();
	m_pDevice->SetFillMode(RFILL_WIRE);

	m_pDevice->SetCullMode(RCULL_CW);

// 	if ( m_pCollisionTree )
// 		m_pCollisionTree->CallbackTree(NULL, RenderPoly);

	m_pDevice->SetFillMode(RFILL_SOLID);
	RRenderHelper::EndRenderState();

	m_pDevice->SetDepthBias(0.01f);
	m_pDevice->SetFvF(D3DFVF_XYZ);

	if (!vPointList.empty())
		m_pDevice->DrawPrimitiveUP(RPT_POINTLIST, vPointList.size(), vPointList.front(), sizeof(RVector));

	if (!vLineList.empty())
		m_pDevice->DrawPrimitiveUP(RPT_LINELIST, vLineList.size()/2, vLineList.front(), sizeof(RVector));

	m_pDevice->SetDepthBias(0.0f);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	srand(GetTickCount());
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	MInitLog();

	return g_App.Run();
}

