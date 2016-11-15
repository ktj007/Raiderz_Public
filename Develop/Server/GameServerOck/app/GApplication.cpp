#include "stdafx.h"
#include "GApplication.h"
#include "GSystem.h"
#include "resource.h"
#include <commctrl.h>
#include <richedit.h>
#include "GField.h"
#include "GFieldInfo.h"
#include "GSpawnManager.h"
#include "GBirdMonitor.h"
#include "GReporter.h"
#include "GEntityNPC.h"
#include "GSharedFieldMaster.h"
#include "GFieldMgr.h"
#include "GConfig.h"
#include "GCommandLogger.h"
#include "GValidatorSilo.h"
#include "SVNRevision.h"
#include "version.h"
#include "CAppHelper.h"
#include "GFieldInfoMgr.h"
#include "GRuntimeValidator.h"
#include "CSBaseValidator.h"
#include "GValidateLogger.h"
#include "GMasterServerFacade.h"
#include "GProxyFieldManager.h"
#include "GCmdHandler_App.h"
#include "SCommandTable_App.h"
#include "GCommandCenter.h"
#include "SProxyPlayerManager.h"
#include "SAppHelper.h"
#include "GMonitorHelper.h"
#include "MStatisticsProfiler.h"
#include "GConst.h"

#pragma comment(lib,"comctl32.lib")

HWND GApplication::m_hDlg = 0;
HWND GApplication::m_hListBox = 0;
vector<MUID> GApplication::m_vecSelectedList;

vector<int> g_qFieldID4Combobox;
int g_nFieldSelectedID=0;

#define MINI_MAP_UPDATE_TICK	(1.0f)		// 1초
#define WM_DESTROY_SERVER		(WM_USER+25)
#define WM_ICON_NOTIFY			(WM_USER+10)
#define WM_SERVER_RELOAD		(WM_USER+6)
#define MAX_OUTPUT_COUNT		1000

class GAppCommandLogListener : public ICommandLogListener
{
public:
	virtual void OnLog(const char* szLog) 
	{
		if (GApplication::GetInstancePtr())
		{
			USES_CONVERSION_EX;
			GApplication::GetInstancePtr()->LogCommand(A2W_EX(szLog, 0));
		}
	}
};


// 리치에디트에 로그 출력한다. 옵션 미구현
void GApplication::AddLogString(const wchar_t* szString, COLORREF color)
{	
	AddStringToRichEditor(m_hRichEdLog, szString, color);
}

void GApplication::AddErrorLogString(const wchar_t* szString, COLORREF color)
{
	AddStringToRichEditor(m_hRichEdErrorLog, szString, color);	
}

void GApplication::SetColorRichEdit(HWND hRichEdit, COLORREF color)
{
	CHARFORMAT2 cf;
	memset(&cf, 0, sizeof(CHARFORMAT2));
	cf.cbSize = sizeof(CHARFORMAT2);
	SendMessage(hRichEdit, EM_GETCHARFORMAT, 0, (LPARAM)&cf);

	cf.dwEffects = 0;
	cf.crTextColor = color;
	cf.dwMask = CFM_COLOR;
	SendMessage(hRichEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

GApplication::GApplication(bool bMakeTrayIcon) : 
GBaseApplication(),
m_bDrawMinimap(false),
m_pMonitorEd(NULL),
m_bMakeTrayIcon(bMakeTrayIcon),
m_pRuntimeValidator(NULL),
m_pAppCmdHandler(NULL)
{
	m_hRichEd = ::LoadLibrary(L"Riched20.dll");
	m_hCommandLogEdit = 0;
	m_hRichEdLog = 0;
	m_hStatus =0;

	m_pMinimapRglt = new MRegulator(MINI_MAP_UPDATE_TICK);
}

GApplication::~GApplication()
{
	Destroy();

	delete m_pMinimapRglt; m_pMinimapRglt = NULL;
	FreeLibrary(m_hRichEd);
	m_hRichEdLog = 0;
	m_hRichEd = 0;
}

inline void DrawPixel(HDC &hdc, int x, int y, int w, int h, COLORREF color)
{
	HBRUSH hbrNew = CreateSolidBrush( color );
	HBRUSH hbrOld = (HBRUSH) SelectObject(hdc, hbrNew);

	Rectangle(hdc, x, y, x+w, y+h);

	SelectObject(hdc, hbrOld);
	DeleteObject(hbrNew);
}


void			
GApplication::StartDrawMinimap()
{
	m_bDrawMinimap = true;
}

void			
GApplication::StopDrawMinimap()
{
	m_bDrawMinimap = false;
}

void GApplication::DrawMiniMap(HWND hWnd, int mx, int my)
{
	HDC hdc = GetDC(hWnd);

	RECT rt;
	GetClientRect(hWnd,&rt);

	// clear window
	HBRUSH hbrBlack = CreateSolidBrush( RGB(0,0,0));
	HBRUSH hbrOld = (HBRUSH)SelectObject(hdc, hbrBlack);

	FillRect(hdc, &rt, hbrBlack);

	SelectObject(hdc, hbrOld);
	DeleteObject(hbrBlack);

	// window_width, window_height
	int nWindowWidth = rt.right - rt.left;
	int nWindowHeight = rt.bottom - rt.top;

	int nFieldID = g_nFieldSelectedID;

	GField* pField = NULL;
	if (gmgr.pFieldMgr) pField = gmgr.pFieldMgr->GetSharedField(nFieldID, PRIME_CHANNELID);
	if (pField != NULL)
	{
		const GFieldInfo* map_info = pField->GetInfo();
		float field_width = map_info->MaxX() - pField->GetInfo()->MinX();
		float field_height = map_info->MaxY() - pField->GetInfo()->MinY();

		GEntityMgr::ENTITY_UID_MAP& mapEntity = pField->GetEntityMap();

		// entity width, height
		int nEntityWidth = static_cast<int>(nWindowWidth*(100.f/field_width));
		int nEntityHeight = static_cast<int>(nWindowHeight*(100.f/field_height));

		const int MINIMUN_SIZE = 4;

		nEntityWidth = max(nEntityWidth,MINIMUN_SIZE);
		nEntityHeight = max(nEntityHeight,MINIMUN_SIZE);

		// draw entity
		GEntity* pEntity;
		for (GEntityMgr::ENTITY_UID_MAP::iterator iter=mapEntity.begin();
			iter != mapEntity.end();
			++iter)
		{
			pEntity = (iter)->second;

			COLORREF color;

			{
				if (pEntity->GetTypeID() == ETID_PLAYER)	color = RGB(0xFF, 0xFF, 0xFF);
				else if (pEntity->GetTypeID() == ETID_NPC)	color = RGB(255, 0, 0);
				else										color = RGB(255, 0, 0);
			}

			int nEntityX = static_cast<int>(nWindowWidth*((pEntity->GetPos().x-pField->GetInfo()->MinX())/field_width));
			int nEntityY = static_cast<int>(nWindowHeight*((pEntity->GetPos().y-pField->GetInfo()->MinY())/field_height));

			if ( (mx - nEntityX) >= 0 && (mx - nEntityX<nEntityWidth)
				&& (my - nEntityY) >= 0 && (my - nEntityY<nEntityHeight))
			{
				// selected entity
				MUID uID = pEntity->GetUID();

				if (!pEntity->IsNPC())	continue;

				// add wstring
				wstring strName = L"Unknown";
				GEntityNPC* pNPC = static_cast<GEntityNPC*>(pEntity);
				if (pNPC->GetNPCInfo())
				{
					strName = pNPC->GetNPCInfo()->strName;
				}

				m_vecSelectedList.push_back(uID);

				wchar_t pcText[128];
				swprintf_s(pcText, L"%s (%I64u)", strName.c_str(), uID.Value);
				::SendMessage(m_hListBox, LB_ADDSTRING, 0,(LPARAM)pcText);
			}

			DrawPixel(hdc, nEntityX, nEntityY, nEntityWidth, nEntityHeight, color);
		}

		if (!m_vecSelectedList.empty())
		{
			TrackingToEntity(m_vecSelectedList[0]);
		}
	}

	ReleaseDC(hWnd, hdc);
}

void GApplication::OnPaint()
{
	bool bValidation = false;

	if (bValidation == false && m_bDrawValidatorAlert)
	{
		DrawValidationAlert();
	}

	if (m_bDrawMinimap)
	{
		HWND hWnd = GetDlgItem(this->m_hDlg, IDC_STATIC_MINIMAP);
		_ASSERT(hWnd);

		UpdateWindow(hWnd);

		DrawMiniMap(hWnd,-1,-1);
	}

	return;
}

void GApplication::OutputLogProc(const char* szText, MLogLevel nLevel)
{
	USES_CONVERSION_EX;

	DWORD dwCurrentThreadID = GetCurrentThreadId();
	if (GApplication::GetInstance().IsMainThread(dwCurrentThreadID))
	{
		GApplication::GetInstance().OutputLog(A2W_EX(szText, 0), nLevel);
	}
	else
	{
		MCommand* pNewCmd = gsys.pCommandCenter->NewCommand(MAPP_LOG_OUT_PUT);
		pNewCmd->AddParameter(NEW_WSTR(A2W_EX(szText, 0)));
		pNewCmd->AddParameter(NEW_INT(nLevel));

		pNewCmd->SetReceiverUID(gsys.pServer->GetUID());
		gsys.pCommandCenter->PostCommand(pNewCmd);
	}
}

void GApplication::OutputLog(const wchar_t* szText, MLogLevel nLevel)
{
	wstring strDateTime = SAppHelper::MakeCurrentDateTimeString();
	wstring strText = strDateTime + L" ";
	strText += szText;

	switch (nLevel)
	{
	case MLOG_LEVEL_DEBUG:			GApplication::GetInstance().AddLogString(strText.c_str(), RGB(130, 130, 130)); break;
	case MLOG_LEVEL_INFORMATION:	GApplication::GetInstance().AddLogString(strText.c_str()); break;
	case MLOG_LEVEL_WARNING:		GApplication::GetInstance().AddLogString(strText.c_str(), RGB(0, 0, 255)); break;
	case MLOG_LEVEL_ERROR:
		{
			GApplication::GetInstance().AddLogString(strText.c_str(), RGB(255, 0, 0));
			GApplication::GetInstance().AddErrorLogString(strText.c_str(), RGB(255, 0, 0));
		}
		break;
	default:
		GApplication::GetInstance().AddLogString(strText.c_str());	
	};
}

void GApplication::InitWindowHandles()
{
	m_hRichEdLog = GetDlgItem(GApplication::m_hDlg, IDC_RICHEDIT21);
	m_hRichEdErrorLog = GetDlgItem(GApplication::m_hDlg, IDC_RICHEDIT_ERRORLOG);

	m_pMonitorEd = new SAppMonitorEditBox(GetDlgItem(GApplication::m_hDlg, IDC_RICHEDIT_MONITOR));
}

bool GApplication::OnCreate()
{
	InitWindowHandles();
	AddAppCommandTable();

	StartDrawMinimap();
	MGetDefaultLogger()->SetCustomOutputCallback(GApplication::OutputLogProc);

	AddLogString(L"");

	if (GBaseApplication::OnCreate() == false)
	{
		return false;
	}
	
	if (SetupTrayIcon() == false) return false;

	SetupFieldComboBox();

	if (!GConfig::m_bWindowVisible)
	{
		Show(false);
	}

	GCommandLogger* pCommandLogger = new GCommandLogger(new GAppCommandLogListener());
	BirdM().SetListener(&m_BirdMonitorListener);

	m_pAppCmdHandler = new GCmdHandler_App(gsys.pCommandCenter->GetNetServer());
	gsys.pCommandCenter->SetLogger(pCommandLogger);

	return true;
}

bool GApplication::OnUpdate( float fDelta )
{
	if (!GBaseApplication::OnUpdate(fDelta))
		return false;

	UpdateServerInfo();

	PFI_B(11, "GApplication::OnUpdate - GApplication:UpdateMinimap");
	UpdateMinimap(fDelta);
	PFI_E(11);

	PFI_B(12, "GApplication::OnUpdate - SAppMonitorEditBox:Update");
	m_pMonitorEd->Update(fDelta);
	PFI_E(12);

	return true;
}


void GApplication::OnDestroy()
{
	mlog("GApplication > OnDestroy \n");

	if (m_pRuntimeValidator)
	{
		m_pRuntimeValidator->Stop();
		SAFE_DELETE(m_pRuntimeValidator);
	}


	SAFE_DELETE(m_pAppCmdHandler);

	GBaseApplication::OnDestroy();

	if (m_bMakeTrayIcon)
		m_TrayIcon.RemoveIcon();

	if (m_pMonitorEd)
	{
		SAFE_DELETE(m_pMonitorEd);
	}
}



// 메인 다이알로그 메세지 핸들러
BOOL CALLBACK GApplication::MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_COMBO_FIELD:
			switch(HIWORD(wParam))
			{
			case CBN_SELCHANGE:
				{
					int nSel = (int)SendMessage(GetDlgItem(m_hDlg, IDC_COMBO_FIELD), CB_GETCURSEL, 0, 0);
					if (nSel >= 0 && nSel < (int)g_qFieldID4Combobox.size())
						g_nFieldSelectedID = g_qFieldID4Combobox[nSel];
				}break;
			}
			return TRUE;
		case IDC_LIST_SELECT_ENTITY:
			switch(HIWORD(wParam))
			{
			case LBN_SELCHANGE:
				{
					int nSel = (int)SendMessage(GetDlgItem(m_hDlg, IDC_LIST_SELECT_ENTITY), LB_GETSEL, 0, 0);
					if (nSel > 0 && nSel <= (int)m_vecSelectedList.size())
						TrackingToEntity(m_vecSelectedList[nSel-1]);
				}break;
			}
			

			return TRUE;
		case IDC_BUTTON_CLOSE_AFTER_10MIN:

			return TRUE;
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg,0);
			return TRUE;
		case IDC_BUTTON_REPORT:
			{
//				GReporter reporter;
//				reporter.ReportTargetEntity(BirdM().GetUIDTargetField(), BirdM().GetUIDTargetNPC());
			}
			return TRUE;		
		}
		break;
	
	case WM_LBUTTONUP:
		{
			int mx = LOWORD(lParam);
			int my = HIWORD(lParam);

			HWND hWnd = GetDlgItem(hDlg, IDC_STATIC_MINIMAP);
			
			RECT rtDlg, rtView;
			GetWindowRect(hDlg,&rtDlg);
			GetWindowRect(hWnd,&rtView);

			mx -= ( rtView.left - rtDlg.left);
			my -= ( rtView.top - rtDlg.top);

			// reset listbox
			::SendMessage(m_hListBox, LB_RESETCONTENT, 0, 0);
			m_vecSelectedList.clear();


			DrawMiniMap(hWnd, mx, my);
		return FALSE;
		}
	}

	return FALSE;
}



void GApplication::OnMsg( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	__super::OnMsg(hWnd, uMsg, wParam, lParam);

    switch (uMsg)
    {
	case WM_SERVER_RELOAD:
		{
			if (IsRunForTest())
			{
				gsys.pServer->Reload();
			}
		}
		break;
	case WM_CREATE:
		{
			SetupControls(hWnd);
			break;
		}
	case WM_KEYDOWN:
		{
#ifdef _DEBUG
			if(wParam==VK_ESCAPE) exit(0);
#endif
		}
		break;
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case ID_MENU_START:
				OnServerStart();
				break;
			case ID_MENU_STOP:
				OnServerStop();
				break;
			case ID_MENU_PROFILE_START:
				{
					StartProfiling();					
				}
				break;
			case ID_MENU_RELOAD:
				{
					gsys.pServer->Reload();
				}
				break;
			case ID_TOOLS_PROFILE_FINISH:
				{
					AnalysisProfiling();					
				}
				break;
			case ID_TOOLS_CMD_PROFILE_START:
				{
					gsys.pServer->StartCommandProfile();
				}
				break;
			case ID_TOOLS_CMD_PROFILE_FINISH:
				{
					gsys.pServer->FinishCommandProfile();
				}
				break;
			case ID_MENU_EXIT:
			case ID_POPUP_QUIT:
				{
					if (gsys.pServer != NULL)
					{
						gsys.pServer->CloseServer();
					}
					else
					{					
						PostQuitMessage(0);
					}
				}
				break;
			case ID_POPUP_OPEN:
				Show(true);
				break;
			case ID_POPUP_HIDE:
				Show(false);
				break;

			case IDC_BUTTON_CLOSE_AFTER_10MIN:

				break;
			}
		}
		break;
	case WM_DESTROY_SERVER:
		{
			if (IsRunForTest())
			{
				CloseServer();
			}
		}
		break;
	case WM_ICON_NOTIFY:
		{
			OnTrayNotification(wParam, lParam);	
		}
	case WM_PAINT:
		{
			OnPaint();
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// Message Sender
//////////////////////////////////////////////////////////////////////////
void GApplication::SetStatusText(int nID, LPCTSTR str)
{
	::SendMessage(m_hStatus, SB_SETTEXT, nID, (LPARAM)str);
}


//////////////////////////////////////////////////////////////////////////
// Message Handler
//////////////////////////////////////////////////////////////////////////

LRESULT GApplication::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(lParam) == WM_RBUTTONUP)
	{
		HMENU menu = ::LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_TRAY_MENU));
		HMENU submenu = GetSubMenu(menu, 0);
		
		POINT pt;
		GetCursorPos(&pt);

		SetForegroundWindow(m_hWnd);

		TrackPopupMenu(submenu, TPM_LEFTALIGN, pt.x, pt.y, 0, m_hWnd, NULL);
		PostMessage(m_hWnd, WM_NULL, 0, 0);
	}
	else if (LOWORD(lParam) == WM_LBUTTONDBLCLK)
	{
		if (IsWindowVisible(m_hWnd)) ShowWindow(m_hWnd, SW_HIDE);
		else ShowWindow(m_hWnd, SW_SHOW);
	}
	return 1;
}

void GApplication::OnServerStart()
{
}

void GApplication::OnServerStop()
{
}



void GApplication::SetServerInfoView(GServerInfo& server_info) 
{
	server_info.nMaxRecvBPS = max(server_info.nMaxRecvBPS, server_info.nRecvBPS);
	server_info.nMaxSendBPS = max(server_info.nMaxSendBPS, server_info.nSendBPS);
	
	server_info.nTotalRecvBytes += server_info.nRecvBPS;
	server_info.nTotalSendBytes += server_info.nSendBPS;
	server_info.nFrameIndex++;

	float fRecvBPSk = server_info.nRecvBPS / (float)1024;
	float fSendBPSk = server_info.nSendBPS / (float)1024;
	float fMaxRecvBPSk = server_info.nMaxRecvBPS / (float)1024;
	float fMaxSendBPSk = server_info.nMaxSendBPS / (float)1024;
	float fAvrRecvBPSk = server_info.nTotalRecvBytes / (float)server_info.nFrameIndex / (float)1024;
	float fAvrSendBPSk = server_info.nTotalSendBytes / (float)server_info.nFrameIndex / (float)1024;

	wchar_t text[1024] = L"";
	wchar_t t1[1024] = L"";
	swprintf_s(text, L"FPS: %d(%I64d), BPS:R(%.2fk max=%.2fk avr=%.2fk) S(%.2fk max=%.2fk avr=%.2fk), CPS:R(%d) S(%d) L(%d), C: %d, S: %d, DDBQ(%u, max=%u)", 
		server_info.nFPS, 
		server_info.nFrameIndex,
		fRecvBPSk, 
		fMaxRecvBPSk,
		fAvrRecvBPSk,
		fSendBPSk,
		fMaxSendBPSk,
		fAvrSendBPSk,
		server_info.nRecvCPS, 
		server_info.nSendCPS, 
		server_info.nLocalCPS,
		server_info.nNowPlayer, 
		0,
		server_info.nAsyncDBTask,
		server_info.nMaxAsyncDBTask);
	SetStatusText(0, (LPCTSTR)text);

	switch (server_info.nStat)
	{
	case SERVER_STAT_INACTIVE:	wcscpy_s(t1, L"INACTIVE");	break;
	case SERVER_STAT_ACTIVE:	wcscpy_s(t1, L"ACTIVE");	break;
	case SERVER_STAT_FULL:		wcscpy_s(t1, L"FULL");		break;
	case SERVER_STAT_INCHECK:	wcscpy_s(t1, L"INCHECK");	break;
	case SERVER_STAT_ERROR:		wcscpy_s(t1, L"ERROR");		break;
	default:					wcscpy_s(t1, L"");			break;
	}

	wstring strStandAlone;
	if (GConfig::IsStandAloneMode())
	{
		strStandAlone = L" (StandAlone)";
	}

	uint32 globaltime = gsys.pSystem->GetNowTime();

	swprintf_s(text, L"서버상태 = %s%s , GlobalTime = %u", t1, strStandAlone.c_str(), globaltime);
	SetMonitorText(0, text);

	GSharedFieldMaster* pSharedField = gmgr.pFieldMgr->GetSharedFieldMaster(g_nFieldSelectedID);
	int nPlayerQtyInField=0;
	int nSpawnQtyInField=0;
	if (pSharedField)
	{
		GField* pChannel = pSharedField->GetChannel(0);
		if (pChannel)
		{
			nPlayerQtyInField = pChannel->GetEntityQty(ETID_PLAYER);
			nSpawnQtyInField = pChannel->GetEntityQty(ETID_NPC);
		}
	}

	// 접속자
	SProxyPlayerManager* pProxyPlayerManager = gsys.pMasterServerFacade->GetProxyPlayerManager();
	
	swprintf_s(text, L"접속자 : %d / %d(Total=%d) (cap: %d, max: %d)", 
		nPlayerQtyInField, 
		server_info.nNowPlayer, 
		pProxyPlayerManager->GetPlayerCount(),
		server_info.nMaxPlayerCapacity, 
		server_info.nMaxPlayer);

	SetMonitorText(1, text);

	
	// 필드 수
	int nMasterServerSharedFieldCount = 0, nMasterServerDynamicFieldCount = 0, nMasterServerTotalFieldCount = 0;

	GProxyFieldManager* pProxyFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();
	nMasterServerTotalFieldCount = pProxyFieldManager->GetFieldCount();
	nMasterServerSharedFieldCount = nMasterServerTotalFieldCount;

	swprintf_s(text, L"공유필드: (%d:%d), 동적필드그룹: (%d:%d), 전체 필드 : (%d:%d)", 
		gmgr.pFieldMgr->GetSharedFieldQty(), nMasterServerSharedFieldCount,
		gmgr.pFieldMgr->GetDynamicFieldGroupQty(), nMasterServerDynamicFieldCount,
		gmgr.pFieldMgr->GetFieldQty(), nMasterServerTotalFieldCount);
	SetMonitorText(2, text);

	// 해당 필드 NPC 수
	uint32 nAllNPCCount = GMonitorHelper::GetTotalNPCCount();

	swprintf_s(text, L"전체 NPC 수 : %u", nAllNPCCount);
	SetMonitorText(3, text);
}

void GApplication::UpdateMinimap(float fDelta)
{
	if (m_bDrawMinimap)
	{
		if (!m_pMinimapRglt->IsReady(fDelta)) return;

		HWND hWnd = GetDlgItem(m_hDlg, IDC_STATIC_MINIMAP);
		
		if (hWnd)
		{
			DrawMiniMap(hWnd, -1, -1);
		}
//		::InvalidateRect(m_hDlg, &rtView, FALSE);
	}
}

void GApplication::Show(bool bVisible)
{
	if (bVisible)
	{
		ShowWindow(m_hWnd, SW_SHOW);
	}
	else
	{
		ShowWindow(m_hWnd, SW_HIDE);
	}
}


void GApplication::SetMonitorText(int nLine, const wchar_t* szMsg)
{
	if (!m_pMonitorEd) return;
	m_pMonitorEd->SetText(nLine, szMsg);
	return;
}

void GApplication::TrackingToEntity( MUID uidEntity )
{
//	BirdM().SetTargetNPC(uidEntity);
}

void GApplication::SetupFieldComboBox()
{
	if (!IsRunForTest())
	{
		return;
	}

	// 콤보박스에 필드 목록 추가
	SendDlgItemMessage(m_hDlg, IDC_COMBO_FIELD, CB_RESETCONTENT,0,(LPARAM)0);

	GFieldMgr::SHARED_FIELD_MASTER_MAP& SharedFieldMap = gmgr.pFieldMgr->GetSharedFieldMap();

	wchar_t buff[512];
	for (GFieldMgr::SHARED_FIELD_MASTER_MAP::iterator iter = SharedFieldMap.begin();
		iter != SharedFieldMap.end();
		++iter)
	{
		int nFieldID							= iter->first;
		GSharedFieldMaster* pSharedFieldMaster	= iter->second;

		g_qFieldID4Combobox.push_back(nFieldID);

		GSharedField* pSharedField = pSharedFieldMaster->GetChannel(PRIME_CHANNELID);
		if (pSharedField)
		{
			swprintf_s(buff, 512, L"%05d : %s", nFieldID, pSharedField->GetInfo()->m_strFieldName.c_str());
			SendDlgItemMessage(m_hDlg, IDC_COMBO_FIELD, CB_ADDSTRING,0,(LPARAM)buff);
		}
	}

	if (SharedFieldMap.empty() == false)
	{
		const int nDefaultSelection = 0;
		g_nFieldSelectedID = g_qFieldID4Combobox[nDefaultSelection];
		SendDlgItemMessage(m_hDlg, IDC_COMBO_FIELD, CB_SETCURSEL,(WPARAM)nDefaultSelection,(LPARAM)0);
	}
}

bool GApplication::SetupTrayIcon()
{
	if (m_bMakeTrayIcon)
	{
		if(!m_TrayIcon.Create(m_hWnd, WM_ICON_NOTIFY, GConfig::m_strServerName.c_str(), NULL, IDR_MAINFRAME))
		{
			mlog3("Tray Icon Creation Failed.\n");
			return false;
		}

		HICON icon = ::LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINFRAME));
		m_TrayIcon.SetIcon(icon);
	}
	return true;
}

void GApplication::UpdateServerInfo()
{
	if (gsys.pServer)
	{
		SetServerInfoView(gsys.pServer->GetServerInfo());
	}
}

void GApplication::DrawValidationAlert()
{
	PAINTSTRUCT ps;
	HDC hDC;
	RECT clientRC;

	hDC = BeginPaint( m_hDlg, &ps );

	GetClientRect( m_hDlg, &clientRC );

	HBRUSH hbrBlack = CreateSolidBrush( RGB(255,0,0));
	FillRect(hDC, &clientRC, hbrBlack);

	EndPaint( m_hDlg, &ps );
	ReleaseDC( m_hDlg, hDC );
}

void GApplication::_RunRuntimeValidator()
{
	if (IsRunForTest())
	{
		m_pRuntimeValidator = new GRuntimeValidator();
		m_pRuntimeValidator->Start();
	}
}

wstring GApplication::_MakeServerVersion()
{	
	USES_CONVERSION_EX;
	return CAppHelper::MakeServerVersion(A2W_EX(PRODUCT_VERSION, 0), A2W_EX(SVN_REVISION, 0), A2W_EX(SVN_DATE, 0));
}

bool GApplication::_ValidateResouces()
{
	return GValidateResouces();
}

void GApplication::_LogCommand( const wchar_t* szMsg )
{
	if (m_hCommandLogEdit)
	{
		SendMessage(m_hCommandLogEdit, EM_REPLACESEL, FALSE, (LPARAM)szMsg);
		SendMessage (m_hCommandLogEdit, EM_LINESCROLL, 0, 1) ;
	}
}

void GApplication::_Quit( int nRetCode )
{
	CloseServer();
}

wstring GApplication::_MakeTitleCaption()
{
	USES_CONVERSION_EX;

	// 서버 시작 시간 표시
	wchar_t szProg[128] = L"";
	SYSTEMTIME stCurTime;
	GetLocalTime(&stCurTime);

	swprintf_s(szProg, L"%s [%02d/%02d %02d:%02d:%02d]"
		, GConfig::m_strServerName.c_str()
		, stCurTime.wMonth
		, stCurTime.wDay
		, stCurTime.wHour
		, stCurTime.wMinute
		, stCurTime.wSecond);

	return CAppHelper::MakeTitleCaption(szProg, A2W_EX(PRODUCT_VERSION, 0), A2W_EX(SVN_REVISION, 0), A2W_EX(SVN_DATE, 0));
}

void GApplication::SetupControls( HWND hWnd )
{
	wstring strTitleCaption = _MakeTitleCaption();

	SetWindowText(hWnd, strTitleCaption.c_str());

	// 메인 다이알로그 생성
	HWND hDlg = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SERVER_DIALOG), hWnd, MainDlgProc);
	m_hDlg = hDlg;
	ShowWindow(hDlg, SW_SHOW);

	// 상태창 생성
	m_hStatus = CreateStatusWindow(WS_CHILD | WS_VISIBLE, L"Status Line", hWnd, 0);

	RECT rt;
	GetWindowRect(m_hStatus, &rt);
	int arPart[2];
	arPart[0] = 750;
	arPart[1] = rt.right;

	SendMessage(m_hStatus, SB_SETPARTS, 2, (LPARAM)arPart);

	m_hCommandLogEdit = GetDlgItem(hDlg, IDC_RICHEDIT22);

	SetStatusText(0, L"");
	SetStatusText(1, L"Active");

	m_hListBox = GetDlgItem(hDlg, IDC_LIST_SELECT_ENTITY);
}

void GApplication::AddStringToRichEditor( HWND hRichEditor, const wchar_t* szString, COLORREF color )
{
	// 색지정
	SetColorRichEdit(hRichEditor, color);

	
	// 현재 커서 위치 기억
	int nOldSelBegin;
	int nOldSelEnd;
	SendMessage(hRichEditor, EM_GETSEL, (WPARAM)&nOldSelBegin, (LPARAM)&nOldSelEnd);	
	int nVisiableLine = SendMessage(hRichEditor, EM_GETFIRSTVISIBLELINE, 0, 0);
	int nLineCount = SendMessage(hRichEditor, EM_GETLINECOUNT, 0, 0);


	// 커서를 끝으로 보내 로그 출력한다.
	int nLength = ::GetWindowTextLength(hRichEditor);	
	SendMessage(hRichEditor, EM_SETSEL, nLength, nLength);
	SendMessage(hRichEditor, EM_GETSEL, (WPARAM)&nLength, (LPARAM)&nLength);	// 끝 위치 오차보정
	SendMessage(hRichEditor, EM_REPLACESEL, FALSE, (LPARAM)szString);


	// 커서위치를 원상 복귀	
	SendMessage(hRichEditor, EM_SETSEL, nOldSelBegin, nOldSelEnd);

	int nDiffLineCount = nVisiableLine - SendMessage(hRichEditor, EM_GETFIRSTVISIBLELINE, 0, 0);
	SendMessage(hRichEditor, EM_LINESCROLL, 0, nDiffLineCount);


	// EM_GETLASTVISIBLELINE이 없어서 대신 구현; 현재 에디트상자에 보여지는 마지막 줄 수 확인
	RECT rectEditBox;
	GetWindowRect(hRichEditor, &rectEditBox);

	POINT ptPoint;
	ptPoint.x = (rectEditBox.right - rectEditBox.left) - 1;
	ptPoint.y = (rectEditBox.bottom - rectEditBox.top) - 1;

	int nLastCharIndex = SendMessage(hRichEditor, EM_CHARFROMPOS, 0, LPARAM(&ptPoint));
	int nVisiableLastLine = SendMessage(hRichEditor, EM_LINEFROMCHAR, (WPARAM)nLastCharIndex, 0);


	// 현재 스크롤 위치에서 마지막 라인이 보였으면 스크롤.
	if (nLineCount == nVisiableLastLine)
	{
		// 커서가 끝에 있었다면, 커서를 끝으로 보낸다.
		if (nOldSelBegin == nOldSelEnd && nLength == nOldSelBegin)
		{
			nLength = ::GetWindowTextLength(hRichEditor);
			SendMessage(hRichEditor, EM_SETSEL, nLength, nLength);
		}

		SendMessage(hRichEditor, LOWORD(WM_VSCROLL), SB_BOTTOM, 0);
	}
}

void GAppBirdMonitorListener::OnOutput( int nLine, const wchar_t* szMsg )
{
	if (GApplication::GetInstancePtr())
	{
		GApplication::GetInstancePtr()->SetMonitorText(nLine, szMsg);
	}
}
