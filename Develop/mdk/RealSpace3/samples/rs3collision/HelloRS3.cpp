// HelloRS3.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "crtdbg.h"
#include "mmsystem.h"
#include "HelloRS3.h"
#include "RD3DApplication.h"
#include "RFont.h"
#include "RSceneManager.h"
#include "RCameraSceneNode.h"
#include "RTextSceneNode.h"
#include "RMeshSceneNode.h"
#include "RAABBTreeSceneNode.h"
#include "RDeviceD3D.h"
#include "RLightSceneNode.h"
#include "RSpotLightSceneNode.h"
#include "RParticleSystem.h"

#include "RMeshMgr.h"
#include "RActor.h"

#include "RCollision.h"

_USING_NAMESPACE_REALSPACE3

#include "d3dx9.h"

//RSpotLightSceneNode testNode;

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
	bool Create() {
		RVertexAttribute attribs[] = { 
			TYPE_VERTEX,	FORMAT_FLOAT, 3,
			TYPE_NORMAL,	FORMAT_FLOAT, 3,
		};
		if ((m_vf= m_pDevice->CreateVertexFormat(attribs, elementsOf(attribs))) == R_NONE) return false;

		for( int i=0; i<50; i++ )
		{
#define RADIUS	20.f
			float theta = (2*PI*i)/(50-1);
			vertices[2*i+0].position = RADIUS*RVector( sinf(theta),-1.0f, cosf(theta) );
			vertices[2*i+0].normal   = RVector( sinf(theta), 0.0f, cosf(theta) );
			vertices[2*i+1].position = RADIUS*RVector( sinf(theta), 1.0f, cosf(theta) );
			vertices[2*i+1].normal   = RVector( sinf(theta), 0.0f, cosf(theta) );
		}

		if ((m_vb= m_pDevice->CreateVertexBuffer(sizeof(vertices),sizeof(CUSTOMVERTEX2),vertices)) == R_NONE) return false;

		RMatrix matWorld;
		matWorld.SetTranslationMatrix(RVector(200,0,100));
		SetTransform(matWorld);
		return true;
	}

protected:
	void OnRender() {

	m_pDevice->SetCullMode(RCULL_NONE);
	m_pDevice->SetTexture(1,R_NONE);
	m_pDevice->SetTextureStageState(0,RTSS_COLOROP	,RTOP_SELECTARG1 );
	m_pDevice->SetTextureStageState(0,RTSS_COLORARG1 ,RTA_DIFFUSE );

	RLightSceneNode *pLight = GetSceneManager()->GetLight();
//	LPDIRECT3DDEVICE9 pd3dDevice = ((RDeviceD3D*)m_pDevice)->GetD3DDevice();

	// Assume d3dDevice is a valid pointer to an IDirect3DDevice9 interface.
	D3DLIGHT9 d3dLight;
	ZeroMemory(&d3dLight, sizeof(d3dLight));
	// Set up a white point light.
	d3dLight.Type = D3DLIGHT_POINT;
	d3dLight.Diffuse.r  = 1.0f;
	d3dLight.Diffuse.g  = 1.0f;
	d3dLight.Diffuse.b  = 1.0f;
	d3dLight.Position.x = pLight->m_position.x;
	d3dLight.Position.y = pLight->m_position.y;
	d3dLight.Position.z = pLight->m_position.z;
	// Don't attenuate.
	d3dLight.Attenuation0 = 1.0f; 
	d3dLight.Range        = 2000.0f;

	m_pDevice->SetLight(0,(void*)&d3dLight);
	m_pDevice->SetLightEnable(0,true);
	m_pDevice->SetLighting(true);
	m_pDevice->SetNormalizeNormals(true);

//	pd3dDevice->SetLight( 0, &d3dLight );
//	pd3dDevice->LightEnable( 0, TRUE );
//	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
//	pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

    // Finally, turn on some ambient light.
	m_pDevice->SetAmbient(0x00202020);


	RMatrix matWorld = GetTransform();
//	matWorld.SetTranslationMatrix(RVector(200,0,100));
	m_pDevice->SetTransform( RST_WORLD, matWorld);

		m_pDevice->SetVertexFormat(m_vf);
		m_pDevice->SetVertexBuffer(m_vb,0,0);

//		m_pDevice->SetVertexFormat(R_NONE);
//	g_pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL );

//		m_pDevice->DrawPrimitiveUP(RPT_TRIANGLESTRIP, 2*50-2, vertices, sizeof(CUSTOMVERTEX2));
		m_pDevice->DrawPrimitive(RPT_TRIANGLESTRIP, 0, 2*50-2);

	GetDevice()->SetCullMode(RCULL_CW);
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
	bool Create() {
		RVertexAttribute attribs[] = { 
			TYPE_VERTEX,	FORMAT_FLOAT, 3,
			TYPE_NORMAL,	FORMAT_FLOAT, 3,
		};
		if ((m_vf= m_pDevice->CreateVertexFormat(attribs, elementsOf(attribs))) == R_NONE) return false;

		for( int i=0; i<50; i++ )
		{
#define RADIUS	20.f
			float theta = (2*PI*i)/(50-1);
			vertices[2*i+0].position = RADIUS*RVector( sinf(theta),-1.0f, cosf(theta) );
			vertices[2*i+0].normal   = RVector( sinf(theta), 0.0f, cosf(theta) );
			vertices[2*i+1].position = RADIUS*RVector( sinf(theta), 1.0f, cosf(theta) );
			vertices[2*i+1].normal   = RVector( sinf(theta), 0.0f, cosf(theta) );
		}

		if ((m_vb= m_pDevice->CreateVertexBuffer(sizeof(vertices),sizeof(CUSTOMVERTEX2),vertices)) == R_NONE) return false;

		RMatrix matWorld;
		matWorld.SetTranslationMatrix(RVector(200,0,100));
		SetTransform(matWorld);
		return true;
	}

protected:
	void OnRender() {

	m_pDevice->SetCullMode(RCULL_NONE);
	m_pDevice->SetTexture(1,R_NONE);
//	const RTextureInfo *pInfo = m_pDevice->GetTextureInfo(1);
	m_pDevice->SetTextureStageState(0,RTSS_COLOROP	,RTOP_SELECTARG1 );
	m_pDevice->SetTextureStageState(0,RTSS_COLORARG1 ,RTA_DIFFUSE );

	RLightSceneNode *pLight = GetSceneManager()->GetLight();
//	LPDIRECT3DDEVICE9 pd3dDevice = ((RDeviceD3D*)m_pDevice)->GetD3DDevice();

	// Assume d3dDevice is a valid pointer to an IDirect3DDevice9 interface.
	D3DLIGHT9 d3dLight;
	ZeroMemory(&d3dLight, sizeof(d3dLight));
	// Set up a white point light.
	d3dLight.Type = D3DLIGHT_POINT;
	d3dLight.Diffuse.r  = 1.0f;
	d3dLight.Diffuse.g  = 1.0f;
	d3dLight.Diffuse.b  = 1.0f;
	d3dLight.Position.x = pLight->m_position.x;
	d3dLight.Position.y = pLight->m_position.y;
	d3dLight.Position.z = pLight->m_position.z;
	// Don't attenuate.
	d3dLight.Attenuation0 = 1.0f; 
	d3dLight.Range        = 2000.0f;

	m_pDevice->SetLight(0,(void*)&d3dLight);
	m_pDevice->SetLightEnable(0,true);
	m_pDevice->SetLighting(true);
	m_pDevice->SetNormalizeNormals(true);

	// Finally, turn on some ambient light.

	m_pDevice->SetAmbient(0x00202020);

	RMatrix matWorld = GetTransform();
//	matWorld.SetTranslationMatrix(RVector(200,0,100));
	m_pDevice->SetTransform( RST_WORLD, matWorld);

		m_pDevice->SetVertexFormat(m_vf);
		m_pDevice->SetVertexBuffer(m_vb,0,0);

//		m_pDevice->SetVertexFormat(R_NONE);
//	g_pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL );

//		m_pDevice->DrawPrimitiveUP(RPT_TRIANGLESTRIP, 2*50-2, vertices, sizeof(CUSTOMVERTEX2));
		m_pDevice->DrawPrimitive(RPT_TRIANGLESTRIP, 0, 2*50-2);

	GetDevice()->SetCullMode(RCULL_CW);
	}
} testNode2;

class mySceneNode3 : public RSceneNode {
	struct CUSTOMVERTEX
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex
		DWORD color;        // The vertex color
		float u,v;
	};

public :
	HTEXTURE m_hTexture;

	void Create() {
		m_hTexture = R_NONE;
	}

	void OnRender() {

		static HVERTEXFORMAT vf = R_NONE;
		if(vf==R_NONE) {
			RVertexAttribute attribs[] = { 
				TYPE_POSITIONT, FORMAT_FLOAT, 4,
				TYPE_COLOR, FORMAT_D3DCOLOR, 4,
				TYPE_TEXCOORD, FORMAT_FLOAT, 2 
			};
			if ((vf= m_pDevice->CreateVertexFormat(attribs, elementsOf(attribs))) == R_NONE) return ;
		}

		// Initialize three vertices for rendering a triangle
		CUSTOMVERTEX vertices[] =
		{
			{ 250.0f, 50.0f, 0.5f, 1.0f, 0xff00ff00, 1,0, },
			{ 50.0f,  50.0f, 0.5f, 1.0f, 0xffff0000, 0,0, }, // x, y, z, rhw, color
			{ 250.0f, 250.0f, 0.5f, 1.0f, 0xff00ffff, 1,1, },
			{  50.0f, 250.0f, 0.5f, 1.0f, 0xffffffff, 0,1, }, 
		};

		m_pDevice->SetTexture(0,m_hTexture);
		m_pDevice->SetBlending(RBLEND_ONE,RBLEND_NONE);
		m_pDevice->SetTextureStageState(0,RTSS_COLOROP	,RTOP_MODULATE );
		m_pDevice->SetTextureStageState(0,RTSS_COLORARG1 ,RTA_DIFFUSE );
		m_pDevice->SetTextureStageState(0,RTSS_COLORARG2 ,RTA_TEXTURE );

		m_pDevice->SetVertexFormat(vf);

		m_pDevice->DrawPrimitiveUP(RPT_TRIANGLESTRIP, 2, vertices, sizeof(CUSTOMVERTEX));
        

	}
};

#include "d3dx9.h"

class myApplication : public RD3DApplication {
	RCameraSceneNode *m_pCamera;
	RTextSceneNode *m_pTextNode;
	RTextSceneNode *m_pTextNode2;
	RAABBTreeSceneNode *m_pTree;
	RFont			*m_pFont;
	RLightSceneNode *m_pLightNode;
	RMeshSceneNode	*m_pMesh;
	RActorScene		*m_pActorScene;
	mySceneNode2	m_lightModelNode;
//	mySceneNode3	m_testNode3;
	RMeshMgr		m_MeshMgr;
	RActor			m_Actor[1000];
	RActorScene*	m_ActorScene[1000];

	RCollisionSphere *m_pCollision;

	//RBACKGOUNDWORKTICKET m_nTicket;
	//HTEXTURE		m_hBackgroundLoadingTexture;

public:
	myApplication() : RD3DApplication() , m_pTextNode(NULL), m_pTextNode2(NULL), m_pFont(NULL), m_pLightNode(NULL), m_pMesh(NULL), m_pCollision(NULL) {}

	char *GetName() { return "rs3 sample application"; }
	bool OnCreate();
	void OnDestroy();
	void OnUpdate();
	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);

	void OnTextureLoadingComplete(RBACKGOUNDWORKTICKET nTicket, HTEXTURE hTexture) {
//		_ASSERT(m_nTicket == nTicket);
//		m_testNode3.m_hTexture = m_hBackgroundLoadingTexture = hTexture;
	}


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
//	GetSceneManager()->LoadXML("Particle.RS.xml");

	// font 등록
	m_pFont = new RFont;
	m_pFont->Create(g_App.GetDevice(),"굴림",12);
	
	// 카메라
	m_pCamera = new RCameraSceneNode;
	GetSceneManager()->AddChild(m_pCamera);
	GetSceneManager()->SetActiveCamera(m_pCamera);
	
	// light 테스트
	m_pLightNode = new RLightSceneNode;
	GetSceneManager()->AddChild(m_pLightNode);
	GetSceneManager()->AddLight(m_pLightNode);

	// light 메쉬
	m_lightModelNode.Create();
	GetSceneManager()->AddChild(&m_lightModelNode);

	// mesh node 테스트
	m_pMesh = new RMeshSceneNode;
	m_pMesh->LoadElu("zzz.elu");
	RMatrix tm;
	tm.SetTranslationMatrix(RVector(200,0,200));
	m_pMesh->SetTransform(tm);

//	GetSceneManager()->AddChild(m_pMesh);

	// aabb tree 테스트 노드
	m_pTree = new RAABBTreeSceneNode;
	m_pTree->Open("mansion.RS");
	GetSceneManager()->AddChild(m_pTree);

	// collision 테스트
	m_pCollision = new RCollisionSphere(50.f);
	GetSceneManager()->AddChild(m_pCollision);


	// 광원의 영향을 받을 노드
	//m_pLightNode->AddTargetNode(m_pTree);

	// custom 테스트 노드
	//testNode.Create();
	//GetSceneManager()->AddChild(&testNode);

	/*
	testNode2.Create();
	GetSceneManager()->AddChild(&testNode2);
	*/

	//m_testNode3.Create();
	//GetSceneManager()->AddChild(&m_testNode3);

	// text 테스트 노드
	m_pTextNode = new RTextSceneNode();
	m_pTextNode->SetFont(m_pFont);
	m_pTextNode->SetText("test");
	m_pTextNode->SetPosition(0,0);
	GetSceneManager()->AddChild(m_pTextNode);

	//m_pTextNode2 = new RTextSceneNode(m_pFont);
	//m_pTextNode2->SetPosition(0,16);
	//GetSceneManager()->AddChild(m_pTextNode2);

	GetDevice()->SetCullMode(RCULL_CW);

	// background loading 테스트
	//m_hBackgroundLoadingTexture = R_NONE;
	//m_nTicket = GetDevice()->CreateTextureBackground("gunzlogo.tga",this);

	// 로딩이 느려서 테스트중 잠시 봉인
//	/*

//	RMesh* pMesh = m_MeshMgr.Add("nyahbit_male.elu");
	RMesh* pMesh = m_MeshMgr.Add("human_male_01.elu");

//	pMesh->m_AniMgr.Add("run","nyahbit_male_run.elu.ani",-1);
//	pMesh->m_AniMgr.Add("run","human_male_run.elu.ani",-1);
//	pMesh->m_AniMgr.Add("run","human_male_walk.elu.ani",-1);
//	pMesh->m_AniMgr.Add("run","human_male_idle.elu.ani",-1);
		
	// mesh 로 부터 scene node tree 를 생성한다.

#define _MAX_UNIT_X 1
#define _MAX_UNIT_Y 1

	RActorScene* pActorScene;

	int index;

	for(int i=0;i<_MAX_UNIT_Y;i++) {
		for(int j=0;j<_MAX_UNIT_X;j++) {

			index = i*_MAX_UNIT_Y+j;

			RMatrix m;		
//			m.MakeIdentity();
			m.SetTranslationMatrix(50*i,50*j,300);

			m_ActorScene[index] = pActorScene = m_Actor[index].Create( pMesh );

			m_Actor[index].SetAnimation(AMode_All,"run");
			m_Actor[index].SetWorldMatrix(m);
			m_Actor[index].SetSpeed(AMode_All,4.8f);
//			m_Actor[index].SetBlendAnimation(AMode_All,"attack",0.6f);//마지막 인자는 몇초동안 블랜딩할것인지..

			GetSceneManager()->AddChild(pActorScene);

		}
	}

	/*
	RMatrix world;
	world.MakeIdentity();
	GetDevice()->SetTransform(RST_WORLD,world);
	*/

	m_pDevice->SetTextureFilter(0,RTF_LINEAR);
	m_pDevice->SetTextureFilter(1,RTF_LINEAR);

	m_pDevice->SetLighting(false);
	m_pDevice->SetDepthBias(0);

	RParticleSystem::GetInstance()->Start();
	//*/
	return true;
}

void myApplication::OnDestroy()
{
	GetSceneManager()->Clear();

	SAFE_DELETE(m_ActorScene[0]);
	m_Actor[0].Destroy();
	SAFE_DELETE(m_pCamera);
	SAFE_DELETE(m_pTextNode);
	SAFE_DELETE(m_pTextNode2);
	SAFE_DELETE(m_pTree);
	SAFE_DELETE(m_pLightNode);
	SAFE_DELETE(m_pMesh);

	SAFE_DELETE(m_pFont);
// TODO: spotlight destroy
//	RSpotLightSceneNode::Destroy();
}

void myApplication::OnUpdate()
{
#define IsKeyDown(key) ((GetAsyncKeyState(key) & 0x8000)!=0)

	static RVector cameraPos=RVector(100,50,100),cameraDir;
	static float	fCameraZ = .5f*PI , fCameraX = -.5f*PI;

	DWORD elapsed = GetDevice()->GetLastElapsedTime() * (IsKeyDown(VK_SHIFT) ? 5 : 1);

	float fRotateStep=elapsed*0.001f;
	float fMoveStep=elapsed*0.5f;

	if(IsKeyDown(VK_LEFT)) fCameraZ+=fRotateStep;
	if(IsKeyDown(VK_RIGHT)) fCameraZ-=fRotateStep;
	if(IsKeyDown(VK_UP)) fCameraX-=fRotateStep;
	if(IsKeyDown(VK_DOWN)) fCameraX+=fRotateStep;

	if(IsActive()){
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(GetWindowHandle(),&pt);
		int iDeltaX, iDeltaY;
		iDeltaX = pt.x-GetDevice()->GetScreenWidth()/2;
		iDeltaY = pt.y-GetDevice()->GetScreenHeight()/2;

		fCameraZ+=-iDeltaX*0.01f;
		fCameraX+=iDeltaY*0.01f;
		fCameraX=max(0.01f,min(PI-0.01f,fCameraX));

		{
			POINT pt={GetDevice()->GetScreenWidth()/2,GetDevice()->GetScreenHeight()/2};
			ClientToScreen(GetWindowHandle(),&pt);
			SetCursorPos(pt.x,pt.y);
		}
	}

	cameraDir = RVector(cosf(fCameraZ)*sinf(fCameraX),
						sinf(fCameraZ)*sinf(fCameraX),
						cosf(fCameraX));


	RVector posDiff(0,0,0);

	if(IsActive()){
		RVector dir = cameraDir;
		RVector right = RVector(0,0,1).CrossProduct(dir);
		right.Normalize();

		if(IsKeyDown('W')) posDiff+=fMoveStep*dir;
		if(IsKeyDown('S')) posDiff-=fMoveStep*dir;
		if(IsKeyDown('A')) posDiff+=fMoveStep*right;
		if(IsKeyDown('D')) posDiff-=fMoveStep*right;
		if(IsKeyDown(VK_SPACE)) posDiff+=fMoveStep*RVector(0,0,1);

		cameraPos += posDiff;
	}

	GetSceneManager()->GetActiveCamera()->Set(cameraPos, cameraDir);

	if(posDiff.Length()>0.01f) {

		m_pCollision->Clear();
		m_pCollision->RCollision::SetPosition( cameraPos + cameraDir*500.f );
		m_pTree->GetCollidablePlanes(*m_pCollision,m_pCollision->RCollision::GetPosition(),m_pCollision->RCollision::GetPosition()+posDiff);

	}

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
	fRad += (float)(elapsed)*0.001f;
	RVector lightPosition = m_pMesh->GetPosition() + RVector( fRounding*cosf(fRad) , fRounding*sinf(fRad) , 300.0f );
	m_pLightNode->m_position = lightPosition;
	RMatrix tm;
	tm.SetTranslationMatrix(m_pLightNode->m_position);
	m_lightModelNode.SetTransform(tm);

	char szBuffer[256];
	sprintf(szBuffer,"FPS %3.3f",GetDevice()->GetFrameRate());
	m_pTextNode->SetText(szBuffer);

	//sprintf(szBuffer,"AABBTree %d tris, %d nodes, %d sectors",
	//	m_pTree->GetRenderedTriangleCount(),
	//	m_pTree->GetRenderedNodeCount(),
	//	m_pTree->GetRenderedSectorCount());
	//m_pTextNode2->SetText(szBuffer);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
//	_CrtSetBreakAlloc(9211);

	return g_App.Run();
}

