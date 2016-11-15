// HelloRS3.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include "crtdbg.h"
#include "RDeviceD3D.h"
#include "RSceneManager.h"
#include "../sampleApp.h"
#include "d3dx9.h"
#include <string>
#include "RTerrain.h"
#include "RHeightField.h"
#include "RSceneNodeID.h"
#include "RWaterSceneNode.h"
#include "REnvironmentSet.h"

#include <MXml.h>

using namespace rs3;
using namespace std;

const char* ZONE_NAME = "Zone/Ingen_Freeport/Ingen_Freeport.zone.xml";

class MyApplication : public sampleApp 
{
public:
	string m_strZoneFilename;
	RVector m_cameraPos;
	REnvironmentSet* m_pEnvironmentSet;

	MyApplication() 
		: sampleApp()
		, m_strZoneFilename(ZONE_NAME)
		, m_pEnvironmentSet(NULL)
	{ 
	}

	char *GetName() 
	{
		return _T("Terrain TestCode");
	}

	void UpdateDescriptMapUnderWater(RTerrain *pTerrain);

	bool OnCreate();
	void OnDestroy();
	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);
	void OnUpdate();
	void OnRender();

} g_App;


bool MyApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
	switch (message)
	{
	case WM_KEYDOWN:
		switch(wParam)
		{
		case 'Q' :
			PostQuitMessage(0);
		break;
		case VK_F3 :
			if ( m_pCamera )
			{
				FILE *fp = fopen("camera.txt","wt");
				if ( fp )
				{
					const RVector& position = m_pCamera->GetPosition();
					const RVector& direction = m_pCamera->GetDirection();

					fprintf(fp,"%f %f %f\n", position.x, position.y, position.z);
					fprintf(fp,"%f %f %f\n", direction.x, direction.y, direction.z);
					fclose(fp);
				}
			}

		break;
		};
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

bool GetAllWaterPlane(RSceneNode *pSceneNode)
{
	if ( _stricmp(pSceneNode->GetNodeIDString(), RSID_WATERPLANE) == 0 )
		return true;
	else
		return false;
}

bool MyApplication::OnCreate()
{
	REngine::GetConfig().SetPath(RP_MESH,	"../Data/Model;../Data/Model/MapObject;../Data/Model/Sky;../Data/Model/Player;../Data/Model/weapons;../Data/SFX");
	REngine::GetConfig().SetPath(RP_TEXTURE,"../../../Game/Runtime/Data/Texture/Map;../../../Game/Runtime/Data/Texture/Tile");
	REngine::GetConfig().SetPath(RP_EFFECT,	"../Data/SFX");
	REngine::GetConfig().SetDataPath("../Data/");


	if ( !GetSceneManager()->LoadRootSceneFromXML(m_strZoneFilename.c_str(), false))
		return false;

	RTerrain *pTerrain = GetSceneManager()->GetCurrentTerrain();

	_ASSERT(m_pCamera);
	m_pCamera->SetNearFarZ(10, 50000);

	m_pCamera->SetPosition(RVector3(52764.0f, 42830.0f, 7818.0f));
	m_pCamera->SetDirection(RVector3(0.0f, 1.0f, -0.5f));

	m_cameraPos = m_pCamera->GetPosition();
	m_cameraDir = m_pCamera->GetDirection();

	const char *ZONE_ENV_FILE_NAME = "../Data/Zone/Ingen_Freeport/Ingen_Freeport.zone.env.xml";
	// 환경 씬노드 생성
	m_pEnvironmentSet = new rs3::REnvironmentSet;
	// 파일로부터 만들기
	bool bRet = m_pEnvironmentSet->CreateFromFile(ZONE_ENV_FILE_NAME, false);
	// 씬매니저에 더하기
	REngine::GetSceneManager().AddSceneNode(m_pEnvironmentSet);
	// 환경 설정
	bRet = m_pEnvironmentSet->SetCurrentEnvSet(REnvironment::DAWN, REnvironment::SUNNY);

	return true;
}

void MyApplication::OnDestroy()
{
	if(m_pEnvironmentSet)
	{
		m_pEnvironmentSet->RemoveFromParent();
		SAFE_DELETE(m_pEnvironmentSet);
	}

	sampleApp::OnDestroy();
}

void MyApplication::OnUpdate()
{
#define IsKeyDown(key) ((GetAsyncKeyState(key) & 0x8000)!=0)

	DWORD elapsed = GetDevice()->GetLastElapsedTime() * (IsKeyDown(VK_SHIFT) ? 15 : 1);

	float fRotateStep = elapsed*0.001f;
	float fMoveStep = elapsed*0.5f;

	if(IsActive())
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(GetWindowHandle(),&pt);
		// mouse control
		int iDeltaX, iDeltaY;
		iDeltaX = pt.x-GetDevice()->GetScreenWidth()/2;
		iDeltaY = pt.y-GetDevice()->GetScreenHeight()/2;

		float RotateAxisZ = -iDeltaX*0.01f;
		float RotateAxisRight = -iDeltaY*0.01f;

		// update direction
		RMatrix mat;
		mat.MakeIdentity();
		mat.SetRotationZ(RotateAxisZ);

		m_cameraDir =  m_cameraDir * mat;

		RVector vRight = m_pCamera->GetRight();

		mat.MakeIdentity();
		mat.SetRotationMatrixAxis(vRight, RotateAxisRight);

		m_cameraDir = m_cameraDir * mat;

		ResetCursor();
	}

	RVector posDiff(0,0,0);
	if(IsActive()){
		RVector dir = m_cameraDir;
		RVector right = RVector(0,0,1).CrossProduct(dir);
		right.Normalize();

		if(IsKeyDown('W')) posDiff+=fMoveStep*dir;
		if(IsKeyDown('S')) posDiff-=fMoveStep*dir;
		if(IsKeyDown('A')) posDiff+=fMoveStep*right;
		if(IsKeyDown('D')) posDiff-=fMoveStep*right;
		if(IsKeyDown(VK_SPACE)) posDiff+=fMoveStep*RVector(0,0,1);
	} // if(IsActive())

	if(posDiff.Length()>0) 
	{
		m_cameraPos += posDiff;
	}
	
	m_pCamera->Set(m_cameraPos, m_cameraDir);

	char szBuffer[256];
	sprintf(szBuffer,"FPS %3.1f camera ( %3.3f, %3.3f, %3.3f )",GetDevice()->GetFrameRate(),
		m_cameraPos.x,m_cameraPos.y,m_cameraPos.z);

	SetText(szBuffer);
}

void MyApplication::OnRender()
{
	sampleApp::OnRender();

	m_pDevice->SetDepthBias(0.01f);
	m_pDevice->SetFvF(D3DFVF_XYZ);
	m_pDevice->SetDepthBias(0.0f);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	if ( lpCmdLine != NULL && lpCmdLine[0] != 0 )
		g_App.m_strZoneFilename = lpCmdLine;

	return g_App.Run();
}

