#pragma once

#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include "shlwapi.h"
#include "RD3DApplication.h"
#include "RFont.h"
#include "RTextSceneNode.h"
#include "RCameraSceneNode.h"
#include "RConfiguration.h"
#include "MDebug.h"
#include "MProfiler.h"
#include "MMemoryLeak.h"

// #ifndef _NO_PRAGMA_COMMENT_LIB
// #ifdef _DEBUG
// #pragma comment (lib, "../../../cml2/lib/cml2d.lib")
// #pragma comment (lib, "../../../mcollision/lib/mcollision_d.lib")
// #pragma comment (lib, "realspace3d.lib")
// #else
// #pragma comment (lib, "../../../cml2/lib/cml2.lib")
// #pragma comment (lib, "../../../MCollision/lib/mcollision.lib")
// #pragma comment (lib, "realspace3.lib")
// #endif
// #endif

//#include "../../../../sdk/bandi_capture_library/include/bandicap.h"

// class sampleCapturer
// {
// private:
// 	HWND					m_hHandle;
// 	rs3::RDeviceD3D*		m_pDevice;
// 	bool					m_bInited;
// 	CBandiCaptureLibrary	m_bandiCaptureLibrary;
// 
// 	BCAP_CONFIG GetConfig()
// 	{
// 		BCAP_CONFIG cfg;
// 		return cfg;
// 	}
// 
// 
// 	void Final()
// 	{
// 		if (m_bInited)
// 		{
// 			m_bandiCaptureLibrary.Destory();
// 			m_bInited = false;
// 		}
// 	}
// 
// 	bool CheckInit()
// 	{
// 		if (m_bInited)
// 			return true;
// 
// 		if (0 == m_hHandle || NULL == m_pDevice)
// 			return false;
// 
// 		return m_bInited;
// 	}
// 
// public:
// 	sampleCapturer() : m_bInited(false), m_hHandle(0), m_pDevice(NULL) {}
// 	~sampleCapturer()
// 	{
// 		Final();
// 	}
// 
// 	bool Init(HWND nHwnd, rs3::RDeviceD3D* pDevice)
// 	{
// 		m_hHandle = nHwnd;
// 		m_pDevice = pDevice;
// 
// 		if(FAILED(m_bandiCaptureLibrary.Create(BANDICAP_DLL_FILE_NAME)))
// 		{
// 			_ASSERT(0);
// 			return false;
// 		}
// 
// 		if(FAILED(m_bandiCaptureLibrary.Verify("MAIET-PROJECT8-20080917", "4ab252ca")))
// 		{
// 			ASSERT(0);
// 			return false;
// 		}
// 
// 		m_bInited = true;
// 
// 		return true;
// 	}
// 
// 	bool Start(const char* pSzFileName)
// 	{
// 		if (CheckInit() == false) return false;
// 
// 		if(m_bandiCaptureLibrary.IsCapturing()==FALSE)
// 		{
// 			BCAP_CONFIG cfg = GetConfig();
// 
// 			m_bandiCaptureLibrary.CheckConfig(&cfg);	// 잘못된 설정 바로 잡기
// 			m_bandiCaptureLibrary.SetConfig(&cfg);		// 설정 적용
// 			m_bandiCaptureLibrary.SetMinMaxFPS(15, 30);	// 최소, 최대 프레임 설정
// 
// 			// 캡처를 시작합니다.
// 			HRESULT hr = m_bandiCaptureLibrary.Start(pSzFileName, m_hHandle, BCAP_MODE_D3D9_SCALE, (LONG_PTR)m_pDevice->GetD3DDevice());
// 			if(FAILED(hr)) 
// 			{
// 				_ASSERT(0);
// 				return false;
// 			}
// 		}
// 		else
// 		{
// 			m_bandiCaptureLibrary.Stop();
// 		}
// 
// 		return true;
// 	}
// 
// 	void Stop()
// 	{
// 		if (CheckInit() == false)
// 			return;
// 
// 		if(m_bandiCaptureLibrary.IsCapturing())
// 		{
// 			m_bandiCaptureLibrary.Stop();
// 		}
// 	}
// 
// 	bool IsCapturing()
// 	{
// 		if (CheckInit() == false)
// 			return false;
// 
// 		return (m_bandiCaptureLibrary.IsCapturing() == TRUE);
// 	}
// 
// 	void Run()
// 	{
// 		if (CheckInit() == false)
// 			return;
// 
// 		if(m_bandiCaptureLibrary.IsCapturing())
// 		{
// 			m_bandiCaptureLibrary.Work((LONG_PTR)m_pDevice->GetD3DDevice());
// 		}
// 	}
// 
// 
// 	void MakeFileName( const char* pSzFolderName, const char* pSzSampleName, char* szoutFileName )
// 	{
// 		m_bandiCaptureLibrary.MakePathnameByDate(pSzFolderName, pSzSampleName, szoutFileName, MAX_PATH);
// 	}
// };

using namespace rs3;

class sampleApp : public RD3DApplication {
protected:
	RFont				*m_pFont;
	RTextSceneNode		*m_pTextNode;
	RCameraSceneNode	*m_pCamera;

	RVector				m_cameraDir,m_cameraTargetPos;
	float				m_fCameraZ, m_fCameraX;
	float				m_fDistToCamera;
	bool				m_bFreeCameraMove;
	std::string			m_strCurrentDirectory;

	//sampleCapturer		m_sampleCapture;

	void SetCameraPos(const RVector& pos)
	{
		UpdateCameraDir();
		m_cameraTargetPos = pos + m_cameraDir * m_fDistToCamera;
	}

	void SetCameraTarget(const RVector& pos)
	{
		m_cameraTargetPos = pos;
	}

	void SetCameraDirection(const RVector& dir)
	{
		RVector dirNormalized = Normalize( dir );
		m_fCameraX = acosf(dirNormalized.z);
		m_fCameraZ = acosf ( dirNormalized.x / sinf(m_fCameraX) );
	}

	void UpdateCameraDir()
	{
		m_cameraDir = RVector(cosf(m_fCameraZ)*sinf(m_fCameraX),
							sinf(m_fCameraZ)*sinf(m_fCameraX),
							cosf(m_fCameraX));
	}

// 	bool IsCapturing()
// 	{
// 		return m_sampleCapture.IsCapturing();
// 	}
// 
// 	void StartCapture()
// 	{
// 		TCHAR	pathName[MAX_PATH];
// 		m_sampleCapture.MakeFileName(m_strCurrentDirectory.c_str(), GetName(), pathName);
// 		m_sampleCapture.Start(pathName);
// 	}
// 
// 	void StopCapture()
// 	{
// 		m_sampleCapture.Stop();
// 	}
// 	
// 	void RunCapture()
// 	{
// 		m_sampleCapture.Run();
// 	}

public:
	sampleApp() : RD3DApplication(), m_fDistToCamera(300.f), m_fCameraZ(.8f), m_fCameraX(2.f), m_cameraTargetPos(0,0,0), m_bFreeCameraMove(true)
	{ UpdateCameraDir(); }

	virtual char *GetName() { return "rs3 sample application"; }

	void ChangeDirectoryToModule()
	{
		// Current Directory를 맞춘다.
		char szModuleFileName[_MAX_DIR] = {0,};
		GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
		PathRemoveFileSpec(szModuleFileName);
		SetCurrentDirectory(szModuleFileName);

		m_strCurrentDirectory = szModuleFileName;
	}

	bool OnInit()
	{
		ChangeDirectoryToModule();

		MInitLog(MLOG_LEVEL_DEBUG, MLOG_FILE|MLOG_DEBUG);


		REngine::GetConfig().m_nWidth = 800;
		REngine::GetConfig().m_nHeight = 600;
		REngine::GetConfig().m_bFullScreen = false;

		REngine::GetConfig().LoadFromXMLFile("config.xml");


		return true;
	}

	RSceneManager*	GetSceneManager()	{	return REngine::GetSceneManagerPtr(); }
	RDevice*		GetDevice()			{	return REngine::GetDevicePtr(); }

	virtual bool Create() {

		

		if(!RD3DApplication::Create()) return false;

		// 미리 만들어놓고,
		m_pFont = new RFont;
		m_pTextNode = new RTextSceneNode;
		m_pCamera = new RCameraSceneNode;
//		m_pCamera->SetNearFarZ(1.f,10000.f);

		ResetCursor();

		// 초기화 한다
		// font 등록
		m_pFont->AddFontResourceFromFile(L"../data/interface/SFdhk.ttf");
//		m_pFont->Create(m_pDevice,_T("SF들국화"),20);
		m_pFont->Create(m_pDevice,L"굴림",12);

		// text 테스트 노드
		m_pTextNode->SetFont(m_pFont);
		m_pTextNode->SetText("test");
		m_pTextNode->SetPosition(0,0);
		GetSceneManager()->AddSceneNode(m_pTextNode);

		// 카메라
		GetSceneManager()->AddSceneNode(m_pCamera);
		GetSceneManager()->SetPrimaryCamera(m_pCamera);

		m_pDevice->SetCullMode(RCULL_CW);

		m_pDevice->SetTextureFilter(0,RTF_LINEAR);
		m_pDevice->SetTextureFilter(1,RTF_LINEAR);

		m_pDevice->SetLighting(false);
		m_pDevice->SetDepthBias(0);

		// Capture
		//m_sampleCapture.Init(m_hWnd, (RDeviceD3D*)m_pDevice);

		PFC_RESET;
		return true;
	}

	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
	{
		switch (message)
		{
			/*
			case WM_SYSCHAR:
				if(wParam==VK_RETURN) {
					REngine::GetConfig().m_bFullScreen = !REngine::GetConfig().m_bFullScreen;
					ResetDevice();
				}
				break;
				*/
			case WM_CHAR:
				switch(wParam)
				{
				case VK_ESCAPE :
					PostMessage(hWnd,WM_CLOSE,0,0);
					break;

				case '0' :
					PFC_RESET;
					break;
				}

				break;
		};

		return false;
	}

	void Destroy()
	{
		PFC_FINALANALYSIS("sampleprofile.txt");
		SAFE_DELETE(m_pFont);
		if (m_pCamera)
		{
			m_pCamera->RemoveFromParent();
			delete m_pCamera;
			m_pCamera = NULL;
		}

		if (m_pTextNode)
		{
			m_pTextNode->RemoveFromParent();
			delete m_pTextNode;
			m_pTextNode = NULL;
		}

		RD3DApplication::Destroy();
//		MFinalLog();
	}

	void ResetCursor()
	{
		POINT pt={GetDevice()->GetScreenWidth()/2,GetDevice()->GetScreenHeight()/2};
		ClientToScreen(GetWindowHandle(),&pt);
		SetCursorPos(pt.x,pt.y);
	}

	void SetText(const char* szText)
	{
		m_pTextNode->SetText(szText);
	}

	virtual void OnUpdate()
	{
		ProcessInputCameraMove();

		RVector cameraPos = m_cameraTargetPos - m_cameraDir*m_fDistToCamera;

		m_pCamera->Set(cameraPos, m_cameraDir);

		char szBuffer[256];
		sprintf_s(szBuffer,"FPS %3.1f camera ( %3.3f, %3.3f, %3.3f )",GetDevice()->GetFrameRate(),
			cameraPos.x,cameraPos.y,cameraPos.z);
		SetText(szBuffer);
	}

	void ProcessInputCameraMove()
	{
#define IsKeyDown(key) ((GetAsyncKeyState(key) & 0x8000)!=0)

		DWORD elapsed = GetDevice()->GetLastElapsedTime() * (IsKeyDown(VK_SHIFT) ? 15 : 1);

		float fRotateStep=elapsed*0.001f;
		float fMoveStep=elapsed*0.5f;
		RVector posDiff(0,0,0);

		HWND hWndForeground = GetForegroundWindow();
		if(IsActive() && (m_hWnd==hWndForeground) && m_bFreeCameraMove )
		{
			//////////////////////////////////////////////////////////////////////////
			// 회전
			if(IsKeyDown(VK_LEFT)) m_fCameraZ+=fRotateStep;
			if(IsKeyDown(VK_RIGHT)) m_fCameraZ-=fRotateStep;
			if(IsKeyDown(VK_UP)) m_fCameraX-=fRotateStep;
			if(IsKeyDown(VK_DOWN)) m_fCameraX+=fRotateStep;

			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(GetWindowHandle(),&pt);
			int iDeltaX, iDeltaY;
			iDeltaX = pt.x-GetDevice()->GetScreenWidth()/2;
			iDeltaY = pt.y-GetDevice()->GetScreenHeight()/2;

			m_fCameraZ+=-iDeltaX*0.01f;
			m_fCameraX+=iDeltaY*0.01f;
			m_fCameraX=max(0.01f,min(MMath::PI-0.01f,m_fCameraX));

			ResetCursor();

			//////////////////////////////////////////////////////////////////////////
			// W A S D 이동
			RVector dir = m_cameraDir;
			RVector right = RVector(0,0,1).CrossProduct(dir);
			right.Normalize();

			if(IsKeyDown('W')) posDiff+=fMoveStep*dir;
			if(IsKeyDown('S')) posDiff-=fMoveStep*dir;
			if(IsKeyDown('A')) posDiff+=fMoveStep*right;
			if(IsKeyDown('D')) posDiff-=fMoveStep*right;
			if(IsKeyDown(VK_SPACE)) posDiff+=fMoveStep*RVector(0,0,1);
			if(IsKeyDown(VK_CONTROL)) posDiff-=fMoveStep*RVector(0,0,1);
		}

		UpdateCameraDir();

		if(posDiff.Length()>0) {
			OnCameraTargetMove(m_cameraTargetPos + posDiff);
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// 이하 sample Application 들이 구현할만한 것

	virtual void OnCameraTargetMove(const RVector &to)
	{
		m_cameraTargetPos = to;
	}

	void SetFreeCameraMove(bool bFree)
	{
		m_bFreeCameraMove = bFree;
	}

	vec3& GetCameraDirect()
	{
		return m_cameraDir;
	}

	vec3 GetCameraPos()
	{
		return m_pCamera->GetPosition();
	}
};