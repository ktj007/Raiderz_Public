// HelloRS3.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>

#include "crtdbg.h"
#include "mmsystem.h"

#include "MDebug.h"

#include "RD3DApplication.h"
#include "RDeviceD3D.h"
#include "RSingletonSceneManager.h"
#include "../sampleApp.h"
#include "RRenderHelper.h"
#include "RHeightField.h"
#include "RTerrain.h"
#include "RUtil.h"

#include <algorithm>
#include <vector>

using namespace std;
using namespace rs3;

#include "d3dx9.h"

class myApplication : public sampleApp 
{
public:
	string m_strZoneFilename;
	typedef pair<RBoundingBox, DWORD> PAIR_BBOXCOLOR;
	vector<PAIR_BBOXCOLOR > coloredAABBs;

	myApplication() 
		: sampleApp()
		, m_strZoneFilename("../data/Zone/Teress_Plain/Teress_Plain.zone.xml")
	{
	}

	char *GetName() 
	{ 
		return "rs3 collision sample2 application"; 
	}
	bool OnCreate();
	void OnDestroy();

	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);

	void OnUpdate();
	void OnRender();

protected:
	void OnPickTest();
} g_App;

bool myApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
	switch (message)
	{
	case WM_KEYDOWN:
		break;
	}
	return sampleApp::OnWndMessage(hWnd,message,wParam,lParam,pReturn);
}

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
	if ( !GetSceneManager()->LoadRootSceneFromXML(m_strZoneFilename.c_str()))
		return false;

	RTerrain *pTerrain = GetSceneManager()->GetCurrentTerrain();
	RHeightField *pHeightField = pTerrain->GetHeightFieldPtr();

	const RBoundingBox &AABB = pTerrain->GetAABB();

	m_pCamera->SetNearFarZ(10.f,100000.f);

	const int RANDOM_COUNT = 500;
	for (int i = 0; i<RANDOM_COUNT; ++i)
	{
		RBoundingBox bbox;
		bbox.vmin = RVector(RandF(AABB.vmin.x, AABB.vmax.x), RandF(AABB.vmin.y, AABB.vmax.y), RandF(AABB.vmin.z, AABB.vmax.z));
		bbox.vmax = bbox.vmin + RVector(RandF(10, 1000), RandF(10, 1000), RandF(10, 1000));

		DWORD dwColor = D3DCOLOR_XRGB(0xff,0xff,0xff);
		if ( pHeightField->IsIntersect(bbox))
			dwColor = D3DCOLOR_XRGB(0xff,0x00,0x00);
		
		coloredAABBs.push_back(PAIR_BBOXCOLOR(bbox, dwColor));
	}

	return true;
}

void myApplication::OnDestroy()
{
	sampleApp::OnDestroy();
}

void myApplication::OnUpdate()
{
	sampleApp::OnUpdate();
}

void myApplication::OnRender()
{
	if ( !coloredAABBs.empty())
	{
		RRenderHelper::SetRenderState(false);
		for ( vector<PAIR_BBOXCOLOR >::iterator it = coloredAABBs.begin(); it != coloredAABBs.end(); ++it )
		{
			RRenderHelper::RenderBox(RMatrix::IDENTITY, it->first, it->second);
		}
		RRenderHelper::EndRenderState();
	}
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

