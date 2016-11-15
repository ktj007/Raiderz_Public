//#include <vld.h>
#include <windows.h>

#define _CRT_SECURE_NO_DEPRECATE

#include "crtdbg.h"
#include "RDeviceD3D.h"
#include "RSceneManager.h"
#include "../sampleApp.h"
//#include "RSingleTree.h"
#include "RGrassSystem.h"

#include "RWaterResource.h"
#include "RTreePassRenderer.h"
#include "RWaterPassRenderer.h"

//#pragma comment(lib, "../../../RealSpace3/lib/RealSpace3_d.lib")
//#pragma comment(lib, "../../../cml2/lib/cml2d.lib")

using namespace rs3;

#include "d3dx9.h"

const char* ZONE_NAME = "Zone/Login/Login.zone.xml";

class myApplication : public sampleApp 
{
public:
	int					ppp;
	RTextSceneNode*		status;
	RWaterPlane*		m_pWaterPlane;

	myApplication() : sampleApp(), m_pWaterPlane(NULL) {}

	char *GetName() { return "rs3 water application"; }
	void OnUpdate();
	bool OnCreate();
	void OnRender();
	void OnDestroy();

	float& NowWaterParam();
	float GetInc();

	virtual bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);

} g_App;

bool myApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
	switch (message)
	{
	case WM_CHAR:
		if(wParam==VK_ESCAPE) PostMessage(hWnd,WM_CLOSE,0,0);
 		switch (wParam)
 		{
			case 'u':
				ppp--;
				if (ppp < 0) ppp = 14;
				break;
			case 'j':
				ppp++;
				if (ppp >= 15) ppp = 0;
				break;
			case 'h':
				NowWaterParam() -= GetInc();
				break;
			case 'k':
				NowWaterParam() += GetInc();
				break;
			case '1':
				GetSceneManager()->GetPassRenderer< RWaterPassRenderer >()->m_nWaterEffectLevel = 0;
				break;
			case '2':
				GetSceneManager()->GetPassRenderer< RWaterPassRenderer >()->m_nWaterEffectLevel = 1;
				break;
			case '3':
				GetSceneManager()->GetPassRenderer< RWaterPassRenderer >()->m_nWaterEffectLevel = 2;
				break;
		}
		break;
	case WM_KEYDOWN:
		break;
	};

	return sampleApp::OnWndMessage(hWnd, message, wParam, lParam, pReturn);
}



bool myApplication::OnCreate()
{	
	RCapsule c;
	c = RCapsule(RVector(0, 0, 0), RVector(0, 0, 10), 5);
	RVector d;
	d.Set(1, 0 , 0);
	d.Normalize();
	float dist;
	bool i = c.IntersectRay(RVector(-10, -1, 13), d, dist);

	GetSceneManager()->LoadRootSceneFromXML(ZONE_NAME);

	std::vector< RSceneNode* > vWaterPlane;
	struct SSerializeTest
	{
		static bool TreeSerializeTest( RSceneNode* pSceneNode )
		{
			return MIsExactlyClass( RWaterPlane, pSceneNode );
		}
	};

	GetSceneManager()->GetRootSceneNode().SerializeToVector( SSerializeTest::TreeSerializeTest, vWaterPlane );
	_ASSERT( vWaterPlane.empty() == false );
	m_pWaterPlane = static_cast<RWaterPlane*>(vWaterPlane[0]);

	m_pCamera->SetNearFarZ(500, 60000);
	m_pCamera->SetPosition(10000, 15000, 15000);
	SetClearColor(0x000000ff);

	ppp = 0;

	status = new RTextSceneNode;
	status->SetFont(m_pFont);
	status->SetPosition(10, 20);
	status->SetText("");
	status->SetColor(0xffff0000);
	GetSceneManager()->AddSceneNode(status);
	return true;
}

void myApplication::OnRender()
{
	sampleApp::OnRender();
}


void myApplication::OnUpdate()
{	
	sampleApp::OnUpdate();

	static char* ostr[] = 
	{
		"UVScale 1 : ",
		"UVScale 2 : ",
		"UVScale 3 : ",
		"UVScale 4 : ",
		"AdjustFactor : ",
		"Weight 1 : ",
		"Weight 2 : ",
		"Weight 3 : ",
		"Weight 4 : ",
		"Depth Limit : ",
		"Red Absorb Half-life : ",
		"Green Absorb Half-life : ",
		"Blue Absorb Half-life : ",
		"Fresnel Constant : ",
		"Reflect Adjust : "
	};
	char buf[200] = {0, };
	sprintf(buf, "%s%f", ostr[ppp], NowWaterParam());
	status->SetText(buf);
	
}


void myApplication::OnDestroy()
{
	status->RemoveFromParent();
	delete status;
}


float& myApplication::NowWaterParam()
{
	RWaterResource* pWaterResource = m_pWaterPlane->GetWaterResource();
	RWaterResource::SWaterPlaneProperty* pWaterProperty = pWaterResource->GetWaterPlaneProperty();
	switch(ppp)
	{
	case 0:
		return pWaterProperty->m_fBumpUVScale1;
	case 1:
		return pWaterProperty->m_fBumpUVScale2;
	case 2:
		return pWaterProperty->m_fBumpUVScale3;
	case 3:
		return pWaterProperty->m_fBumpUVScale4;
	case 4:
		return pWaterProperty->m_fBumpAdjustFactor;
	case 5:
		return pWaterProperty->m_fBumpWeight1;
	case 6:
		return pWaterProperty->m_fBumpWeight2;
	case 7:
		return pWaterProperty->m_fBumpWeight3;
	case 8:
		return pWaterProperty->m_fBumpWeight4;
	case 9:
		return pWaterProperty->m_fDepthLimit;
	case 10:
		return pWaterProperty->m_vWaterRGBHalflife.x;
	case 11:
		return pWaterProperty->m_vWaterRGBHalflife.y;
	case 12:
		return pWaterProperty->m_vWaterRGBHalflife.z;
	case 13:
		return pWaterProperty->m_fFresnelConstant;
	case 14:
		return pWaterProperty->m_fReflectAdjust;
	default:
		return pWaterProperty->m_fBumpUVScale1;
	}
}

float myApplication::GetInc()
{
	switch(ppp)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		return 20.0f;
	case 4:
		return 0.1f;
	case 5:
	case 6:
	case 7:
	case 8:
		return 0.05f;
	case 9:
		return 20.0f;
	case 10:
	case 11:
	case 12:
		return 50.0f;
	case 13:
	case 14:
		return 0.01f;
	default:
		return 1.0f;
	}

}


int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
// 	rs3::config.SetShaderPath("../../Shaders");
// 	rs3::config.SetTreePath("./TreeModels");
// 	rs3::config.SetTexturePath("./;./TestMap/");
// 	rs3::config.SetModelPath("../Data/Zone/Water_Test_2/;../Data/Zone/Water_Test_Map/");

	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	return g_App.Run();
}