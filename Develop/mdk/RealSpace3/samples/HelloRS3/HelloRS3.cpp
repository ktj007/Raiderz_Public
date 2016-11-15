// HelloRS3.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include <windows.h>
#include "crtdbg.h"
#include "mmsystem.h"
#include "HelloRS3.h"
#include "RD3DApplication.h"
#include "RFont.h"
#include "RSceneManager.h"
#include "RCameraSceneNode.h"
#include "RTextSceneNode.h"
//#include "RMeshSceneNode.h"
#include "RStaticMeshSceneNode.h"
#include "RDeviceD3D.h"
#include "RLightSceneNode.h"
#include "RLightManager.h"
#include "RSpotLightSceneNode.h"
//#include "RParticleSystem.h"

#include "MDebug.h"
#include "RMeshMgr.h"
#include "RActor.h"
#include "../sampleApp.h"

using namespace rs3;

#include "d3dx9.h"

class mySceneNode : public RSceneNode {
	HVERTEXFORMAT	m_vf;
	HVERTEXBUFFER	m_vb;
	struct CUSTOMVERTEX2
	{
		RVector position;
		RVector	normal;
	};
		CUSTOMVERTEX2 vertices[50*2];

public:
	bool Create() 
	{
		RVertexAttribute attribs[] = { 
			TYPE_VERTEX,	FORMAT_FLOAT, 3,
			TYPE_NORMAL,	FORMAT_FLOAT, 3,
		};
		if ((m_vf= REngine::GetDevice().CreateVertexFormat(attribs, elementsOf(attribs))) == R_NONE) return false;

		for( int i=0; i<50; i++ )
		{
#define RADIUS	20.f
			float theta = (2*MMath::PI*i)/(50-1);
			vertices[2*i+0].position = RADIUS*RVector( sinf(theta),-1.0f, cosf(theta) );
			vertices[2*i+0].normal   = RVector( sinf(theta), 0.0f, cosf(theta) );
			vertices[2*i+1].position = RADIUS*RVector( sinf(theta), 1.0f, cosf(theta) );
			vertices[2*i+1].normal   = RVector( sinf(theta), 0.0f, cosf(theta) );
		}

		if ((m_vb= REngine::GetDevice().CreateVertexBuffer(sizeof(vertices),sizeof(CUSTOMVERTEX2),vertices)) == R_NONE) return false;

		RMatrix matWorld;
		matWorld.SetTranslationMatrix(RVector(200,0,100));
		SetTransform(matWorld);
		return true;
	}

protected:
	void OnRender() 
	{

		REngine::GetDevice().SetCullMode(RCULL_NONE);
		REngine::GetDevice().SetTexture(1,R_NONE);
		REngine::GetDevice().SetTextureStageState(0,RTSS_COLOROP	,RTOP_SELECTARG1 );
		REngine::GetDevice().SetTextureStageState(0,RTSS_COLORARG1 ,RTA_DIFFUSE );

		RLightSceneNode *pLight = REngine::GetSceneManager().GetLightManager()->GetLightRecommended(RVector(0,0,0),10);
	//	LPDIRECT3DDEVICE9 pd3dDevice = ((RDeviceD3D*)m_pDevice)->GetD3DDevice();

		// Assume d3dDevice is a valid pointer to an IDirect3DDevice9 interface.
		D3DLIGHT9 d3dLight;
		ZeroMemory(&d3dLight, sizeof(d3dLight));
		// Set up a white point light.
		d3dLight.Type = D3DLIGHT_POINT;
		d3dLight.Diffuse.r  = 1.0f;
		d3dLight.Diffuse.g  = 1.0f;
		d3dLight.Diffuse.b  = 1.0f;
		d3dLight.Position.x = pLight->GetPosition().x;
		d3dLight.Position.y = pLight->GetPosition().y;
		d3dLight.Position.z = pLight->GetPosition().z;
		// Don't attenuate.
		d3dLight.Attenuation0 = 1.0f; 
		d3dLight.Range        = 2000.0f;

		REngine::GetDevice().SetLight(0,(void*)&d3dLight);
		REngine::GetDevice().SetLightEnable(0,true);
		REngine::GetDevice().SetLighting(true);
		REngine::GetDevice().SetNormalizeNormals(true);

	//	pd3dDevice->SetLight( 0, &d3dLight );
	//	pd3dDevice->LightEnable( 0, TRUE );
	//	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	//	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

		// Finally, turn on some ambient light.
		REngine::GetDevice().SetAmbient(0x00202020);


		RMatrix matWorld = GetWorldTransform();
	//	matWorld.SetTranslationMatrix(RVector(200,0,100));
		REngine::GetDevice().SetTransform( RST_WORLD, matWorld);

		REngine::GetDevice().SetVertexFormat(m_vf);
		REngine::GetDevice().SetVertexBuffer(m_vb,0,0);

	//		REngine::GetDevice().SetVertexFormat(R_NONE);
	//	g_pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL );

	//		REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLESTRIP, 2*50-2, vertices, sizeof(CUSTOMVERTEX2));
		REngine::GetDevice().DrawPrimitive(RPT_TRIANGLESTRIP, 0, 2*50-2);

		REngine::GetDevice().SetCullMode(RCULL_CW);
	}
} testNode;

class mySceneNode2 : public RSceneNode {
	HVERTEXFORMAT	m_vf;
	HVERTEXBUFFER	m_vb;
	struct CUSTOMVERTEX2
	{
		RVector position;
		RVector	normal;
	};
		CUSTOMVERTEX2 vertices[50*2];

public:
	bool Create() 
	{
		RVertexAttribute attribs[] = { 
			TYPE_VERTEX,	FORMAT_FLOAT, 3,
			TYPE_NORMAL,	FORMAT_FLOAT, 3,
		};
		if ((m_vf= REngine::GetDevice().CreateVertexFormat(attribs, elementsOf(attribs))) == R_NONE) return false;

		for( int i=0; i<50; i++ )
		{
#define RADIUS	20.f
			float theta = (2*MMath::PI*i)/(50-1);
			vertices[2*i+0].position = RADIUS*RVector( sinf(theta),-1.0f, cosf(theta) );
			vertices[2*i+0].normal   = RVector( sinf(theta), 0.0f, cosf(theta) );
			vertices[2*i+1].position = RADIUS*RVector( sinf(theta), 1.0f, cosf(theta) );
			vertices[2*i+1].normal   = RVector( sinf(theta), 0.0f, cosf(theta) );
		}

		if ((m_vb= REngine::GetDevice().CreateVertexBuffer(sizeof(vertices),sizeof(CUSTOMVERTEX2),vertices)) == R_NONE) return false;

		RMatrix matWorld;
		matWorld.SetTranslationMatrix(RVector(200,0,100));
		SetTransform(matWorld);
		return true;
	}

protected:
	void OnRender() 
	{

		REngine::GetDevice().SetCullMode(RCULL_NONE);
		REngine::GetDevice().SetTexture(1,R_NONE);
	//	const RTextureInfo *pInfo = REngine::GetDevice().GetTextureInfo(1);
		REngine::GetDevice().SetTextureStageState(0,RTSS_COLOROP	,RTOP_SELECTARG1 );
		REngine::GetDevice().SetTextureStageState(0,RTSS_COLORARG1 ,RTA_DIFFUSE );

		RLightSceneNode *pLight = REngine::GetSceneManager().GetLightManager()->GetLightRecommended(RVector(0,0,0),10);
	//	LPDIRECT3DDEVICE9 pd3dDevice = ((RDeviceD3D*)m_pDevice)->GetD3DDevice();

		if(!pLight) return;

		// Assume d3dDevice is a valid pointer to an IDirect3DDevice9 interface.
		D3DLIGHT9 d3dLight;
		ZeroMemory(&d3dLight, sizeof(d3dLight));
		// Set up a white point light.
		d3dLight.Type = D3DLIGHT_POINT;
		d3dLight.Diffuse.r  = 1.0f;
		d3dLight.Diffuse.g  = 1.0f;
		d3dLight.Diffuse.b  = 1.0f;
		d3dLight.Position.x = pLight->GetPosition().x;
		d3dLight.Position.y = pLight->GetPosition().y;
		d3dLight.Position.z = pLight->GetPosition().z;
		// Don't attenuate.
		d3dLight.Attenuation0 = 1.0f; 
		d3dLight.Range        = 2000.0f;

		REngine::GetDevice().SetLight(0,(void*)&d3dLight);
		REngine::GetDevice().SetLightEnable(0,true);
		REngine::GetDevice().SetLighting(true);
		REngine::GetDevice().SetNormalizeNormals(true);

		// Finally, turn on some ambient light.

		REngine::GetDevice().SetAmbient(0x00202020);

		RMatrix matWorld = GetWorldTransform();
		REngine::GetDevice().SetTransform( RST_WORLD, matWorld);

		REngine::GetDevice().SetShader(R_NONE);

		REngine::GetDevice().SetVertexFormat(m_vf);
		REngine::GetDevice().SetVertexBuffer(m_vb,0,0);
		REngine::GetDevice().DrawPrimitive(RPT_TRIANGLESTRIP, 0, 2*50-2);
		REngine::GetDevice().SetCullMode(RCULL_CW);

	}
} testNode2;

class mySceneNode3 : public RSceneNode 
{
	struct CUSTOMVERTEX
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex
		DWORD color;        // The vertex color
		float u,v;
	};

public :
	RTexture* m_hTexture;

	void Create() 
	{
		m_hTexture = R_NONE;
	}

	void OnRender() 
	{

		static HVERTEXFORMAT vf = R_NONE;
		if(vf==R_NONE) {
			RVertexAttribute attribs[] = { 
				TYPE_POSITIONT, FORMAT_FLOAT, 4,
				TYPE_COLOR, FORMAT_D3DCOLOR, 4,
				TYPE_TEXCOORD, FORMAT_FLOAT, 2 
			};
			if ((vf= REngine::GetDevice().CreateVertexFormat(attribs, elementsOf(attribs))) == R_NONE) return ;
		}

		// Initialize three vertices for rendering a triangle
		CUSTOMVERTEX vertices[] =
		{
			{ 250.0f, 50.0f, 0.5f, 1.0f, 0xff00ff00, 1,0, },
			{ 50.0f,  50.0f, 0.5f, 1.0f, 0xffff0000, 0,0, }, // x, y, z, rhw, color
			{ 250.0f, 250.0f, 0.5f, 1.0f, 0xff00ffff, 1,1, },
			{  50.0f, 250.0f, 0.5f, 1.0f, 0xffffffff, 0,1, }, 
		};

		REngine::GetDevice().SetTexture(0,m_hTexture);
		REngine::GetDevice().SetBlending(RBLEND_ONE,RBLEND_NONE);
		REngine::GetDevice().SetTextureStageState(0,RTSS_COLOROP	,RTOP_MODULATE );
		REngine::GetDevice().SetTextureStageState(0,RTSS_COLORARG1 ,RTA_DIFFUSE );
		REngine::GetDevice().SetTextureStageState(0,RTSS_COLORARG2 ,RTA_TEXTURE );

		REngine::GetDevice().SetVertexFormat(vf);

		REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLESTRIP, 2, vertices, sizeof(CUSTOMVERTEX));
        

	}
};

class myApplication : public sampleApp 
{
	RLightSceneNode *m_pLightNode;
	mySceneNode2	m_lightModelNode;
	RActor			m_actor;

public:
	myApplication() : sampleApp(), m_pLightNode(NULL) {}

	char *GetName() { return "hello rs3 sample application"; }
	bool OnCreate();
	void OnDestroy();
	void OnUpdate();
	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);

//	void OnTextureLoadingComplete(RBACKGOUNDWORKTICKET nTicket, HTEXTURE hTexture) {
//		_ASSERT(m_nTicket == nTicket);
//		m_testNode3.m_hTexture = m_hBackgroundLoadingTexture = hTexture;
//	}


} g_App;

bool myApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
    switch (message)
    {
		case WM_CHAR:
			if(wParam==VK_ESCAPE) PostMessage(hWnd,WM_CLOSE,0,0);
			break;
	};

	return false;
}

bool myApplication::OnCreate()
{
//	REngine::GetSceneManager().LoadXML("Particle.RS.xml");

	// by pok, 포비든 템플이 없습니다. 새로운 샘플 미디어폴더를 만드는게 좋을것 같습니다.
	//if(!REngine::GetSceneManager().LoadRootSceneFromXML("../Data/Zone/Teress_Plain/Teress_Plain.zone.xml")) return false;

	// light 테스트
	m_pLightNode = new RLightSceneNode;
	REngine::GetSceneManager().AddSceneNode(m_pLightNode);
//	REngine::GetSceneManager().AddLight(m_pLightNode);

	// light 메쉬
	m_lightModelNode.Create();
	REngine::GetSceneManager().AddSceneNode(&m_lightModelNode);


	
	REngine::GetSceneManager().AddSceneNode(&m_actor);
	m_actor.Create("hm.elu");
/*
	RAnimation * pAni = new RAnimation();
	pAni->LoadAni("hm.elu.animation.xml");
	m_actor.SetAnimation(pAni);
	m_actor.Play();
*/

	// 광원의 영향을 받을 노드
	//m_pLightNode->AddTargetNode(m_pTree);

	// custom 테스트 노드
	//testNode.Create();
	//REngine::GetSceneManager().AddSceneNode(&testNode);

	testNode2.Create();
	REngine::GetSceneManager().AddSceneNode(&testNode2);

	GetDevice()->SetCullMode(RCULL_CW);

	// background loading 테스트
	//m_hBackgroundLoadingTexture = R_NONE;
	//m_nTicket = GetDevice()->CreateTextureBackground("gunzlogo.tga",this);

	REngine::GetDevice().SetTextureFilter(0,RTF_LINEAR);
	REngine::GetDevice().SetTextureFilter(1,RTF_LINEAR);

	REngine::GetDevice().SetLighting(false);
	REngine::GetDevice().SetDepthBias(0);

//	RParticleSystem::GetInstance()->Start();
	//*/
	return true;
}

void myApplication::OnDestroy()
{
//	SAFE_DELETE(m_ActorScene[0]);
	m_actor.RemoveFromParent();
	m_actor.Destroy();
	m_pLightNode->RemoveFromParent();
	SAFE_DELETE(m_pLightNode);
}

void myApplication::OnUpdate()
{
	sampleApp::OnUpdate();
//	 mesh 돌리기
	//{
	//RMatrix matWorld;
	//matWorld.SetTranslationMatrix(RVector(0,500,200));
	//RMatrix matRot;
	//matRot.SetRotationMatrix(RVector(0,(timeGetTime()*.0001f),0));
	//matWorld = matRot * matWorld;
	//m_pMesh->SetTransform(matWorld);
	//}

	// 광원 테스트중
	const float fRounding = 500.f;
	static float fRad = 0;
	fRad += (float)(GetDevice()->GetLastElapsedTime())*0.001f;
	RVector lightPosition = m_actor.GetPosition() + RVector( fRounding*cosf(fRad) , fRounding*sinf(fRad) , 300.0f );
	m_pLightNode->SetPosition(lightPosition);
	RMatrix tm;
	tm.SetTranslationMatrix(m_pLightNode->GetPosition());
	m_lightModelNode.SetTransform(tm);

	char szBuffer[256];
	sprintf_s(szBuffer,"FPS %3.3f",GetDevice()->GetFrameRate());
	m_pTextNode->SetText(szBuffer);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
//	_CrtSetBreakAlloc(3083);

	return g_App.Run();
}

