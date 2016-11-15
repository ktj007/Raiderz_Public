//#include <vld.h>
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include "crtdbg.h"
#include "../sampleApp.h"

#include "MPathFinder.h"
#include "MPathEngineExporter.h"
#include "MPath.h"
#include "MAgent.h"
#include "RCollisionTree.h"
#include "RRenderHelper.h"
#include "RCollisionTreeBuilder.h"
#include "MPathCollision.h"

using namespace rs3;

/*
* PureIndoor 등의 navigation mesh에서 점진적으로 이동하는 것을 보여주는 예제 
* 내부적인 API는 (\sdk\pathengine\release05_12\samples\tutorial3) 를 참고하면 될 것임.
*/

class PathApplication : public sampleApp
{
public:
	MPathFinder m_pathFinder;
	RCollisionTree *m_pcollision;

	MContext m_context;
	
	RVector *m_pVertices;
	int m_nFaceCount;

	// 점진적 이동 위한 필드
	MPath m_path;
	MAgent m_agent;
	MAgent m_obstacle;

	MVector3 m_vstart;
	MVector3 m_vend;

	PathApplication();
	virtual ~PathApplication();

	char *GetName()
	{
		return "PATHENGINE Test Sample2";
	}

	void OnUpdate();
	bool OnCreate();
	void OnRender();
	void OnDestroy();

	void RenderAgent(MAgent &agent, DWORD color);
	void RenderPath(MPath &path);

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
	m_pVertices = NULL;
}

PathApplication::~PathApplication()
{
	SAFE_DELETE(m_pcollision);
	SAFE_DELETE_ARRAY(m_pVertices);
}

const int ZONE_ID = 0;

bool PathApplication::OnCreate()
{
	if (!m_pathFinder.AddZone(ZONE_ID, "Plane"))
		return false;

	int nFaceCount = m_pathFinder.GetNavigationMeshFaceCount(ZONE_ID);
	if ( nFaceCount == 0 )
		return false;

	RVector *pVertices = new MVector3[nFaceCount*3];
	m_pathFinder.GetNavigationMeshFaceList(ZONE_ID, pVertices,nFaceCount);

	RCollisionTreeBuilder collisionBuilder;
	for ( int i = 0; i<nFaceCount; ++i )
	{
		RVector v0 = pVertices[i*3];
		RVector v1 = pVertices[i*3+1];
		RVector v2 = pVertices[i*3+2];

		collisionBuilder.AddPolygon(v0, v1, v2);
	}

	collisionBuilder.BuildTree();
	m_pcollision = collisionBuilder.CreateCollsitionTree();
	
	m_pVertices = pVertices;
	m_nFaceCount = nFaceCount;

	pVertices = NULL;

	// Agent를 올려놓음 
	RVector vStartPosition(-1000, 0, 0);
	RVector vEndPosition(1000, 0, 0);

	if ( !m_pathFinder.NewPlaceAgent(m_agent, vStartPosition, ZONE_ID))
		return false;

	if ( !m_pathFinder.NewPlaceAgent(m_obstacle, MVector3::ZERO, ZONE_ID))
		return false;

	// context 를 만든다.
	m_pathFinder.NewContext(m_context, ZONE_ID);

	// 두개의 agent 를 context 에 추가한다.
	m_context.AddAgent(m_agent);
	m_context.AddAgent(m_obstacle);

	// path를 만든다.
	if ( !m_pathFinder.FindPath(m_path, ZONE_ID, m_agent, vEndPosition))
		return false;

	m_vstart = vStartPosition;
	m_vend = vEndPosition;

	return true;
}

void PathApplication::RenderAgent(MAgent &agent, DWORD color)
{
	if ( agent.IsValid())
	{
		RVector vOut;
		agent.GetPosition(&vOut);

		RMatrix matWorld;
		matWorld.MakeIdentity();

		int nSize = m_pathFinder.GetShapeRange("small");
		RSphere sphere;
		sphere.center = vOut;
		sphere.radius = nSize;

		RRenderHelper::RenderSphere(sphere, color);
	}
}

void PathApplication::RenderPath(MPath &path)
{
	if ( path.GetSize() >= 2 )
	{
		std::vector<MVector3> vertices;
		m_pathFinder.PathTo3DPoints(vertices, ZONE_ID, path, 0);

		m_pDevice->SetFvF(D3DFVF_XYZ);
		m_pDevice->SetTextureFactor(D3DCOLOR_XRGB(0, 0, 0));
		m_pDevice->SetDepthEnable(false);
		m_pDevice->DrawPrimitiveUP(RPT_LINESTRIP, vertices.size()-1,&vertices[0], sizeof(MVector3));
		m_pDevice->SetDepthEnable(true);
	}
}

void PathApplication::OnRender()
{
	RRenderHelper::SetRenderState();

	m_pDevice->SetFvF(D3DFVF_XYZ);
	m_pDevice->SetTextureFactor(D3DCOLOR_XRGB(0, 128, 0));
	if ( m_pVertices )
		m_pDevice->DrawPrimitiveUP(RPT_TRIANGLELIST, m_nFaceCount,(void*)m_pVertices, sizeof(MVector3));

	RenderAgent(m_agent, D3DCOLOR_XRGB(0,0,255));
	RenderAgent(m_obstacle, D3DCOLOR_XRGB(255, 0, 0));
	RenderPath(m_path);

	RRenderHelper::RenderArrow(m_vstart, RVector::AXISZ, 10, 10, 10, 10, D3DCOLOR_XRGB(255, 255, 255));
	RRenderHelper::RenderArrow(m_vend, RVector::AXISZ, 10, 10, 10, 10, D3DCOLOR_XRGB(255, 255, 255));

	RRenderHelper::EndRenderState();
}

void PathApplication::OnUpdate()
{
	float fElapsedTime = static_cast<float>(GetDevice()->GetLastElapsedTime())/1000.f;
	const float SPEED = 100.13f;

	sampleApp::OnUpdate();

	if ( m_path.GetSize() >= 2 && m_agent.IsValid() )
		m_pathFinder.AdvancedAlongPath(NULL, ZONE_ID, m_agent, m_path, SPEED*fElapsedTime);
}

void PathApplication::OnDestroy()
{
	//m_context.Release();
	//m_agent.Release();
	//m_obstacle.Release();
	//m_path.Release();

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