#include "stdafx.h"
#include "RActor.h"
#include "RSceneManager.h"
#include "RRenderHelper.h"
#include "RCameraSceneNode.h"
#include "RProjectionShadow.h"
#include "RShaderFX.h"
#include "RDirectionalLightSceneNode.h"
#include "RStaticMeshSceneNode.h"

#include "../sampleApp.h"
#include "RDepthPassRenderer.h"
#include "RRenderHelper2.h"
#include "Shadow.h"
#include "RActor.h"

using namespace rs3;
using namespace std;

const std::string ZONEFILENAME = "../data/Zone/ShadowTestMap/ShadowTestMap.zone.xml";

class myApplication : public sampleApp 
{
private:
	RDirectionalLightSceneNode *m_pLightNode;

	bool m_bRotateLightMap;
	bool m_bUniformShadow;
	
	RActor *m_pActor;

public:
	myApplication() : sampleApp()
	{

		m_bRotateLightMap = false;
		m_bUniformShadow = true;
		m_pLightNode = NULL;
	}

	char *GetName() 
	{ 
		return "hello rs3 sample application"; 
	}

	bool OnCreate()
	{
		m_dwClearColor = D3DCOLOR_XRGB(255,255,255);

		RConfiguration &config = REngine::GetConfig();

		config.m_nWidth = 1440;
		config.m_nHeight = 900;
		config.depthFormat = RFMT_D24S8;
		config.pixelFormat = RFMT_ARGB32;
		
		config.SetPath(RP_MESH, "../Data/Model/MapObject;.");
		config.SetPath(RP_EFFECT,"../Data/SFX");
		config.SetPath(RP_SHADER,"../Data/Shader");

		RSceneManager* pSceneManager = GetSceneManager();
		_ASSERT( pSceneManager );

		// 액터 생성
		m_pActor = new RActor;
		if ( !pSceneManager->LoadSceneNode(m_pActor,"outdoor.elu"))
			return false;

		_ASSERT(m_pActor);

		m_pActor->SetDynamicShadowCaster(true);
		m_pActor->SetDynamicShadowReceiver(true);

		m_pActor->SetPosition(RVector::ZERO);
		pSceneManager->AddSceneNode(m_pActor);

		m_pActor->UpdateTransform();
		
		// 카메라 설정
		m_fDistToCamera = 2000.f;

		RVector eye = RVector(0,-0,0);
		RVector dir = RVector(-0, 1,-0);
		dir.Normalize();

		const float NEAR_Z = 10.f;
		const float FAR_Z = 15000.f;

		m_pCamera->Set(eye, dir);
		m_pCamera->SetNearFarZ(NEAR_Z, FAR_Z);
		m_pCamera->SetFogEnable(true);
		m_pCamera->SetFogColor(D3DCOLOR_XRGB(0, 0, 0));
		m_pCamera->SetFogFar(FAR_Z);
		m_pCamera->SetFogNear(FAR_Z-10.0f);
		m_pCamera->SetFarCullingZ(FAR_Z);
		m_pCamera->Update();

		// 렌더타겟 카메라 만들기
		m_pLightNode = new RDirectionalLightSceneNode;
		
		RVector vLightDirection;
		vLightDirection = -RVector(1.0f, 1.0f, 1.2f);
		//vLightDirection = -RVector(0, 0, 1);
		vLightDirection.Normalize();
		
		m_pLightNode->SetLightDirection(vLightDirection);
		m_pSceneManager->AddSceneNode((RSceneNode*)m_pLightNode);

		RPassRenderer *pPassRenderer = m_pSceneManager->GetPassRenderer(RRP_RENDERDEPTH);
		RDepthPassRenderer *pDepthPassRenderer = static_cast<RDepthPassRenderer*>(pPassRenderer);
		pDepthPassRenderer->Init();

		return true;
	}

	void OnDestroy()
	{
		RSceneManager* pSceneManager = GetSceneManager();
		_ASSERT( pSceneManager );

		RPassRenderer *pPassRenderer = m_pSceneManager->GetPassRenderer(RRP_RENDERDEPTH);
		RDepthPassRenderer *pDepthPassRenderer = static_cast<RDepthPassRenderer*>(pPassRenderer);
		pDepthPassRenderer->Destroy();

		if ( m_pActor )
		{
			m_pActor->RemoveFromParent();
			delete m_pActor;
			m_pActor = NULL;
		}

		if ( m_pLightNode )
		{
			m_pLightNode->RemoveFromParent();
			delete m_pLightNode;
			m_pLightNode = NULL;
		}
	}

	void OnRender()
	{
		//m_pActor->SetDynamicShadowCaster(false);
		//m_pActor->RenderSingle(RActor::DETAIL_NONE_TEXTURE_COMMON_POLYGON);
		//m_pActor->SetDynamicShadowCaster(true);
		//RRenderHelper::SetRenderState();
		//RRenderHelper::RenderAxis(RMatrix::IDENTITY, 5000.f);
		
		//RPassRenderer *pPassRenderer = m_pSceneManager->GetPassRenderer(RRP_RENDERDEPTH);
		//RDepthPassRenderer *pDepthPassRenderer = static_cast<RDepthPassRenderer*>(pPassRenderer);
		//RenderHelper::RenderViewFrustumLine(m_pDevice,pDepthPassRenderer->m_camShadow, D3DCOLOR_XRGB(255, 0, 0));
#if 0

		RShadowPrj proj(m_pCameraShadow->GetPosition(), m_pCameraShadow->GetDirection(), m_pLightNode->GetDirection());
		proj.UpdateViewMatrix();

		RShadowPrj::VEC3LIST vecList;
		vecList.resize(8);
		RShadowPrj::FrustumToPoints(&vecList[0],m_pCameraShadow->GetViewProjectionMatrix());
		proj.UpdateUniformProjection(vecList, 100);

		RenderHelper::RenderViewFrustumLine(m_pDevice,*m_pCameraShadow, D3DCOLOR_XRGB(255, 255, 255));
		RenderHelper::RenderCameraAxis(m_pDevice, proj.GetLightViewMatrix());
		RenderHelper::RenderViewFrustumLine(m_pDevice, proj.GetLightViewMatrix() * proj.GetLightProjectionMatrix(), D3DCOLOR_XRGB(255, 0, 0));
#endif
		RRenderHelper::EndRenderState();
	}

	void OnUpdate()
	{	
		sampleApp::OnUpdate();
#if 0
		RMatrix matRotationZ;
		matRotationZ.MakeIdentity();
		matRotationZ.SetRotationZ(DegreeToRadian(0.04f));

		RVector vDirection = m_pLightNode->GetDirection();
		matRotationZ.TransformNormal(vDirection);
		m_pLightNode->SetDirection(vDirection);
		m_pLightNode->Update();
#endif
		::Sleep(0);

		if ( NULL == m_pActor )
			return;
	}

	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
	{
		switch ( message )
		{
		case WM_KEYDOWN:
			switch (LOWORD(wParam))
			{
			case 'R':
				m_bRotateLightMap = !m_bRotateLightMap;
			break;
			case 'T':
				m_bUniformShadow = !m_bUniformShadow;
			break;
			//case 'N':
			//	OnLoadShader();
			//break;
			}

		return false;

		};
		return sampleApp::OnWndMessage(hWnd,message,wParam,lParam,pReturn);
	}

} g_App;

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	return g_App.Run();
}