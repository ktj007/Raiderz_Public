//////////////////////////////////////////////////////////////////////////
// 미친 d3d 샘플입니다.
// d3d runtime 을 retail 로 하면 흰색이 나오고 debug 로 하면 안나옵니다.
// -> 뎁스스텐실 버퍼 셋팅 이후 clear()하여 정상. 해결.

#include <windows.h>

#include "../sampleApp.h"

#include "RDeviceD3D.h"

using namespace rs3;

class myApplication : public sampleApp
{
public:
	myApplication() : sampleApp() { }

	char *GetName() { return "rs3 bullshit sample application"; }

	void OnRender();
} g_App;



#define NEW_DEPTH_STENCIL

void myApplication::OnRender()
{
	RDevice *pDevice = REngine::GetDevicePtr();

	int m_nWidth = 800;
	int m_nHeight = 600;

#ifdef NEW_DEPTH_STENCIL
	RTexture* hDepthStencil = pDevice->CreateRenderTargetTexture(m_nWidth, m_nHeight, RFMT_D16);
	RTexture* hBeforeDepth = pDevice->SetDepthStencilBuffer(hDepthStencil);
#endif

	//pDevice->Clear(true,true,false,0xff0000ff);

#ifdef NEW_DEPTH_STENCIL
	RTexture* m_hBeforeDepthBuffer = pDevice->GetDepthStencilBuffer();
	REngine::GetDevice().SetDepthStencilBuffer( NULL );
	REngine::GetDevice().SetDepthStencilBuffer(m_hBeforeDepthBuffer);
#endif

	pDevice->Clear(true,true,false,0xff0000ff);

	REngine::GetDevice().SetCullMode(RCULL_NONE);

	RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
	
	//pCamera->SetPosition(RVector3(11652.0f, 11185.0f, -44.22f)); // 걍 임의의 포지션
	//pCamera->SetDirection(RVector3(0.9829f, -0.1610f, -0.088f));
	pCamera->SetPosition(RVector3(0, 0, 0)); // 걍 임의의 포지션
	pCamera->SetDirection(RVector3(0, -1, 0));


	pDevice->SetBlending( RBLEND_NONE, RBLEND_NONE);
	pDevice->SetAlphaTestEnable( false);

	pCamera->UpdateViewFrustrum();

	pDevice->SetTransform( RST_WORLD, RMatrix::IDENTITY );
	pDevice->SetTransform( RST_VIEW, pCamera->GetViewMatrix() );
	pDevice->SetTransform( RST_PROJECTION, pCamera->GetProjectionMatrix() );

	struct screencoord
	{
		D3DXVECTOR3	Pos;
	};

	static DWORD scfvf = D3DFVF_XYZ;

	REngine::GetDevice().SetFvF(scfvf);

	screencoord	coord[4] = { 
// 		D3DXVECTOR3( 0, 0 , -65),
// 		D3DXVECTOR3( 14306, 8870, -65), 
// 		D3DXVECTOR3( 14306, 9370, -65),
// 		D3DXVECTOR3( 20000, 20000,-65)
		D3DXVECTOR3( -50, -100, -50),
		D3DXVECTOR3(  50, -100, -50), 
		D3DXVECTOR3( -50, -100,  50),
		D3DXVECTOR3(  50, -100,  50)
	};
//	bool ch = REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLEFAN, 2, coord, sizeof(screencoord));
	bool ch = REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLESTRIP, 2, coord, sizeof(screencoord));

#ifdef NEW_DEPTH_STENCIL
	pDevice->SetDepthStencilBuffer(hBeforeDepth);
	pDevice->DeleteTexture(hDepthStencil);
#endif
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	g_App.Run();

	PFC_FINALANALYSIS("rs3actor.txt");
}