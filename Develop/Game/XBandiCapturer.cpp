#include "stdafx.h"
#include "XBandiCapturer.h"
#include "XBaseApplication.h"
#include "XPath.h"
#include "XGameInterface.h"


bool XBandiCapturer::Init(HWND nHwnd, rs3::RDeviceD3D* pDevice)
{
	m_hHandle = nHwnd;
	m_pDevice = pDevice;

	if(FAILED(m_bandiCaptureLibrary.Create(BANDICAP_DLL_FILE_NAME)))
	{
		_ASSERT(0);
		return false;
	}

	if(FAILED(m_bandiCaptureLibrary.Verify("MAIET-PROJECT8-20080917", "4ab252ca")))
	{
		ASSERT(0);
		return false;
	}

	m_bInited = true;

	return true;
}

bool XBandiCapturer::Start()
{
	if (CheckInit() == false) return false;

	if(m_bandiCaptureLibrary.IsCapturing()==FALSE)
	{
		BCAP_CONFIG cfg = GetConfig();
		BCAP_PRESET preset = BCAP_PRESET_DEFAULT;
		if ( XGETCFG_VIDEO_MOVIEENCODINGTYPE == "MPEG1")				preset = BCAP_PRESET_MPEG1;
		else if ( XGETCFG_VIDEO_MOVIEENCODINGTYPE == "MJPEG")			preset = BCAP_PRESET_MJPEG;
		else if ( XGETCFG_VIDEO_MOVIEENCODINGTYPE == "MJPEG HQ")		preset = BCAP_PRESET_MJPEG_HIGH_QUALITY;
		else if ( XGETCFG_VIDEO_MOVIEENCODINGTYPE == "MPEG4")			preset = BCAP_PRESET_MPEG4;
		else if ( XGETCFG_VIDEO_MOVIEENCODINGTYPE == "YouTube")			preset = BCAP_PRESET_YOUTUBE;
		else if ( XGETCFG_VIDEO_MOVIEENCODINGTYPE == "YouTube HQ")		preset = BCAP_PRESET_YOUTUBE_HIGH_QUALITY;
		else if ( XGETCFG_VIDEO_MOVIEENCODINGTYPE == "YouTube HD")		preset = BCAP_PRESET_YOUTUBE_HIGH_DEFINITION;
		else if ( XGETCFG_VIDEO_MOVIEENCODINGTYPE == "DAUM TV Pot")		preset = BCAP_PRESET_DAUM_TVPOT;
		else if ( XGETCFG_VIDEO_MOVIEENCODINGTYPE == "NAVER Blog")		preset = BCAP_PRESET_NAVER_BLOG;
		else if ( XGETCFG_VIDEO_MOVIEENCODINGTYPE == "Video Editing")	preset = BCAP_PRESET_VIDEO_EDITING;
		BCapConfigPreset( &cfg, preset);


		int w, h;
		XGetGameInterface().OPTIONS.GetScreenResolution( &w, &h);

		float fScreenRatio = max( 0.1f,  1.0f - (float)XGETCFG_VIDEO_MOVIEENCODINGRES * 0.25f);
		cfg.VideoSizeW = (int)( (float)w * fScreenRatio);
		cfg.VideoSizeH = (int)( (float)h * fScreenRatio);

		m_bandiCaptureLibrary.CheckConfig(&cfg);	// 잘못된 설정 바로 잡기
		m_bandiCaptureLibrary.SetConfig(&cfg);		// 설정 적용
		m_bandiCaptureLibrary.SetMinMaxFPS(15, 30);	// 최소, 최대 프레임 설정

		wchar_t	pathName[MAX_PATH];
		MakePathName(pathName);

		// 캡처를 시작합니다.
		HRESULT hr = m_bandiCaptureLibrary.Start(pathName, m_hHandle, BCAP_MODE_D3D9_SCALE, (LONG_PTR)m_pDevice->GetD3DDevice());
		if(FAILED(hr)) 
		{
			_ASSERT(0);
			return false;
		}
	}
	else
	{
		m_bandiCaptureLibrary.Stop();
	}

	return true;
}

void XBandiCapturer::Stop()
{
	if (CheckInit() == false) return;

	if(m_bandiCaptureLibrary.IsCapturing())
	{
		m_bandiCaptureLibrary.Stop();
	}
}

bool XBandiCapturer::IsCapturing()
{
	if (CheckInit() == false) return false;
	return (m_bandiCaptureLibrary.IsCapturing() == TRUE);
}

void XBandiCapturer::DrawCapture( bool bInRendering )
{
	if (CheckInit() == false) return;

	if(m_bandiCaptureLibrary.IsCapturing())
	{
		m_bandiCaptureLibrary.Work((LONG_PTR)m_pDevice->GetD3DDevice());

		// 캡처중임을 알리는 메시지 출력
		OnDrawCapture(bInRendering);
	}
}

void XBandiCapturer::ToggleStart()
{
	if (CheckInit() == false) return;

	if (IsCapturing() == false)
	{
		Start();
	}
	else
	{
		Stop();
	}
}

BCAP_CONFIG XBandiCapturer::GetConfig()
{
	BCAP_CONFIG cfg;

	return cfg;
}

void XBandiCapturer::MakeFolderName( wchar_t* pPath )
{
	wchar_t foldername[MAX_PATH] = L"c:\\";

	if (XPath::CreateDirectoryInMyDocuments(PATH_MYDOC_VIDEO))
	{
		wstring strVideoFolderPath = XPath::GetMyDocumentsRaiderzSubPath(PATH_MYDOC_VIDEO);
		
		string::size_type n = strVideoFolderPath.find_last_not_of( L" /\\");
		if ( n != string::npos)
			strVideoFolderPath = strVideoFolderPath.substr( 0, n + 1);

		wcscpy_s(foldername, strVideoFolderPath.c_str());
	}

	wcscpy_s(pPath, MAX_PATH, foldername);
}

void XBandiCapturer::MakePathName( wchar_t* szoutFileName )
{
	// 현재시간을 이용해서 파일명을 만듭니다. 경로명은 가급적 사용자의 "내 문서" 폴더로 지정해 주세요.

	wchar_t folderName[MAX_PATH];
	MakeFolderName(folderName);
	m_bandiCaptureLibrary.MakePathnameByDate(folderName, GAME_NAME_STR, _T("avi"), szoutFileName, MAX_PATH);
}

bool XBandiCapturer::CheckInit()
{
	if (m_bInited) return true;

	if (global.app == NULL || global.device == NULL)
	{
		return false;
	}

//	HWND nHwnd = RApplication::GetInstance().GetWindowHandle();
	HWND nHwnd = global.app->GetWindowHandle();
	rs3::RDeviceD3D* pDevice = global.device;

	m_bInited = Init(nHwnd, pDevice);

	return m_bInited;
}

void XBandiCapturer::OnDrawCapture( bool bInRenderLoop )
{
	INT msec = m_bandiCaptureLibrary.GetCaptureTime();
	char text[256];
	sprintf_s(text, "Video REC [ %02d:%02d:%03d ]", msec/(60*1000), msec%(60*1000)/1000, msec%1000);

	if (!bInRenderLoop)
		global.device->BeginScene();

	global.ui->GetDC()->BeginDraw();

	global.ui->GetDC()->SetFont(MFontManager::Get( NULL));
	global.ui->GetDC()->SetColor(MCOLOR(0xFFFF0000));
	global.ui->GetDC()->Text(10, 10, text);

	global.ui->GetDC()->EndDraw();

	if (!bInRenderLoop)
		global.device->EndScene();
}

XBandiCapturer::~XBandiCapturer()
{
	Final();
}

void XBandiCapturer::Final()
{
	if (m_bInited)
	{
		m_bandiCaptureLibrary.Destroy();
		m_bInited = false;
	}
}