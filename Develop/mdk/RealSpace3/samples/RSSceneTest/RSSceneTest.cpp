#include <windows.h>

#define _CRT_SECURE_NO_DEPRECATE

#include "crtdbg.h"
#include "RDeviceD3D.h"
#include "RSceneManager.h"
#include "../sampleApp.h"

#include "RDirectionalLightSceneNode.h"
#include "RLightManager.h"

#include <string>
#include <cstdlib>

using namespace std;
using namespace rs3;

class myApplication : public sampleApp 
{
private:
	string m_strZoneFilename;
	RDirectionalLightSceneNode *m_pDirectionalLight;

	bool m_bCameraAccess;
	RVector m_vCameraPos;
	RVector m_vCameraDir;
	RVector m_vCameraUp;
	
	float m_fAverageFPS;
	int m_nFrameCount;
public:
	myApplication(const char *szZoneFilename)
		: m_strZoneFilename(szZoneFilename)
		, m_pDirectionalLight(NULL)
		, m_bCameraAccess(true)
		, m_fAverageFPS(0.0f)
		, m_nFrameCount(0)
	{
		const char *pStr = strchr(szZoneFilename,' ');
		m_vCameraPos = RVector(100, 100, 100);
		m_vCameraDir = RVector(0, 1, 0);
		m_vCameraUp = RVector(0, 0, 1);

		SetCameraDirection( RVector(1,1,-1) );
		SetCameraPos(RVector(3000, 3000, 12000));

		if ( pStr )
		{
			char szBuffer[MAX_PATH];
			szBuffer[0] = '\0';

			sscanf(szZoneFilename,"%s", szBuffer);
			m_strZoneFilename = szBuffer;
			sscanf(pStr+1,"%f %f %f %f %f %f %f %f %f"
				,&m_vCameraPos.x,&m_vCameraPos.y,&m_vCameraPos.z
				,&m_vCameraDir.x,&m_vCameraDir.y,&m_vCameraDir.z
				,&m_vCameraUp.x,&m_vCameraUp.y,&m_vCameraUp.z);
			
			m_bCameraAccess = false;
		}

	}

	virtual ~myApplication()
	{
	}

	void OnInit() 
	{	
		sampleApp::OnInit(); 
		REngine::GetConfig().SetModelPath("../Data/Model/MapObject;../Data/Model;../Data/sfx");
		REngine::GetConfig().SetEffectPath("../Data/sfx");
	}

	virtual void Destroy()
	{
		RCameraSceneNode *pCamera = GetSceneManager()->GetPrimaryCamera();
		RVector vPos = pCamera->GetPosition();
		RVector vDir = pCamera->GetDirection();
		RVector vUp = pCamera->GetUp();

		mlog("Camera Position	: %f %f %f\n", vPos.x, vPos.y, vPos.z);
		mlog("Camera Direction	: %f %f %f\n", vDir.x, vDir.y, vDir.z);
		mlog("Camera Up			: %f %f %f\n", vUp.x, vUp.y, vUp.z);
		mlog("FPS Average : %f\n", m_fAverageFPS/m_nFrameCount);

		if ( m_pDirectionalLight )
		{
			m_pDirectionalLight->RemoveFromParent();
			delete m_pDirectionalLight;
			m_pDirectionalLight = NULL;
		}

		sampleApp::Destroy();
	}

	virtual bool Create()
	{
		if ( !sampleApp::Create())
			return false;

		RSceneManager *pSceneManager = GetSceneManager();

		string strPath = "../../../Game/Runtime/Data/Zone/";
		string strFilename = strPath + m_strZoneFilename + "/";
		strFilename += m_strZoneFilename + ".zone.xml";

		bool bLoadZone = pSceneManager->LoadRootSceneFromXML( strFilename.c_str());
		if ( bLoadZone )
		{
			RCameraSceneNode *pCamera = pSceneManager->GetPrimaryCamera();
			if ( pCamera )
			{
				pCamera->SetNearFarZ(10.f,50000.f);
				pCamera->SetFarCullingZ(50000.f);
				pCamera->SetTransform(m_vCameraPos, m_vCameraDir, m_vCameraUp);
				pCamera->Update();
			}
		}

		m_dwClearColor = D3DCOLOR_XRGB(0, 0, 250);

		RDirectionalLightSceneNode* pDefaultLight = pSceneManager->GetLightManager()->GetDefaultLight();
		if(pDefaultLight)
		{
			pDefaultLight->SetLightAmbient(0.458824f, 0.466667f, 0.462745f, 1);
			pDefaultLight->SetLightDiffuse(1, 1, 1, 1);
			pDefaultLight->SetPower(1.0f);
		}

		return true;
	}

	virtual void OnUpdate()
	{
		if ( m_bCameraAccess )
		{
			sampleApp::OnUpdate();
		}
		else
		{
			char szBuffer[256];
			float fps = GetDevice()->GetFrameRate();
			sprintf(szBuffer,"FPS %5.2f", fps);
			SetText(szBuffer);

			m_fAverageFPS += fps;
			++m_nFrameCount;
		}
	}
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	// 기본파라미터
	char szBuffer[256] = "Rengot_Village";

	if(lpCmdLine && strlen(lpCmdLine))
		strcpy(szBuffer,lpCmdLine);

	myApplication App(szBuffer);
	return App.Run();
}