// VR_CutSceneObjectList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "C_ControlMediator.h"
#include "VR_CutSceneObjectList.h"
#include "VR_EnvWeather.h"
#include "VD_DlgCutSceneEditor.h"
#include "VD_DlgCutSceneActorType.h"

#include "WS_CutScene.h"

#include "M_ToolCamera.h"
#include "M_ToolLogicObjectManager.h"
#include "M_ToolCmdMgr.h"

#include "C_StatusBarHandler.h"
#include "C_ControlMediatorHelperAction.h"

#include "Plugin_CutScene.h"

#include "V_MFCHelper.h"

// CRollupCutSceneObjectList 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRollupCutSceneObjectList, CDialog)

CRollupCutSceneObjectList::CRollupCutSceneObjectList(CWnd* pParent /*=NULL*/)
	: CDialog(CRollupCutSceneObjectList::IDD, pParent)
{
	m_pDlgCutSceneEditor_ = NULL;
	m_bCurrentPreviewRunning = false;

	m_ctlListCutSceneObjects_.m_pCutSceneWrapper = &m_CutSceneWrapper_;
	m_fPreviewStartTime = 0.f;
	m_fCustomElapsedTime = -1.f;
}

CRollupCutSceneObjectList::~CRollupCutSceneObjectList()
{
}

BOOL CRollupCutSceneObjectList::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_ctlOpenCutSceneEditorOnSelected.SetCheck(TRUE);

	m_pDlgCutSceneEditor_ = new CDlgCutSceneEditor;
	m_pDlgCutSceneEditor_->Init(m_pControlMediator_, &m_CutSceneWrapper_);

	AfxSetResourceHandle( theApp.m_hInstance );
	BOOL bCreated = m_pDlgCutSceneEditor_->Create(IDD_CUTSCENE_EDITOR, NULL);
	DWORD dwLastError = GetLastError();
	m_pDlgCutSceneEditor_->ShowEditor(false);
	return TRUE;
}

void CRollupCutSceneObjectList::OnDestroy()
{
	m_pDlgCutSceneEditor_->DestroyWindow();
	delete m_pDlgCutSceneEditor_;
	CDialog::OnDestroy();
}

void CRollupCutSceneObjectList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_CUTSCENE_CLAPPER, m_ctlTreeClapper_);
	DDX_Control(pDX, IDC_LIST_CUTSCENE_OBJ, m_ctlListCutSceneObjects_);
	DDX_Control(pDX, IDC_CHECK_SHOW_CUTSCENE_EDITOR, m_ctlOpenCutSceneEditorOnSelected);
	DDX_Text(pDX, IDC_EDIT_CUTSCENE_PREVIEW_STARTTIME, m_fPreviewStartTime);
}


BEGIN_MESSAGE_MAP(CRollupCutSceneObjectList, CDialog)
	ON_WM_DESTROY()
	ON_LBN_SELCHANGE(IDC_LIST_CUTSCENE_OBJ, &CRollupCutSceneObjectList::OnLbnSelchangeListCutsceneObj)
	ON_BN_CLICKED(IDC_BUTTON_CUTSCENE_PREVIEW, &CRollupCutSceneObjectList::OnBnClickedButtonCutscenePreview)
	ON_LBN_DBLCLK(IDC_LIST_CUTSCENE_OBJ, &CRollupCutSceneObjectList::OnLbnDblclkListCutsceneObj)
	ON_BN_CLICKED(IDC_BUTTON_CUTSCENE_ACTOR_DEL, &CRollupCutSceneObjectList::OnBnClickedButtonCutsceneActorDel)
	ON_BN_CLICKED(IDC_BUTTON_CUTSCENE_ACTOR_NEW, &CRollupCutSceneObjectList::OnBnClickedButtonCutsceneActorNew)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_CUTSCENE_EDITOR, &CRollupCutSceneObjectList::OnBnClickedButtonOpenCutsceneEditor)
END_MESSAGE_MAP()


// CRollupCutSceneObjectList 메시지 처리기입니다.


void CRollupCutSceneObjectList::Listen(CoreMessage& _message)
{
	switch(_message.nID)
	{
		case VIEW_MESSAGE::BEFORE_SCENE_CLEAR:
			{
				ResetUI(NULL);
			}
		break;

		case VIEW_MESSAGE::SELECTION_CHANGED:
			{
				ChangeObjectSelection(-1, false, -1);
			}
		break;
		case VIEW_MESSAGE::LOGIC_MOVE:
			{
				if (m_pDlgCutSceneEditor_)
					m_pDlgCutSceneEditor_->UpdateLocation();
			}
		break;
	}
}

void CRollupCutSceneObjectList::ResetUI( XCutSceneContent* pContent )
{
	m_EffectManager.Destroy();
	m_CutSceneWrapper_.ClearCutSceneContent();

	if (!pContent)
		return;

	BOOL bCutSceneEditorChecked = m_ctlOpenCutSceneEditorOnSelected.GetCheck();
	m_ctlOpenCutSceneEditorOnSelected.SetCheck(FALSE);

	m_CutSceneWrapper_.RegisterCutSceneContent(pContent);

	// clapper
	m_ctlTreeClapper_.DeleteAllItems();
	m_CutSceneWrapper_.WriteClapperInfo(m_ctlTreeClapper_, m_hClapperNameItem, m_hClapperDurationItem);

	// object list
	m_ctlListCutSceneObjects_.ResetContent();
	int nObjectCount = m_CutSceneWrapper_.GetCutSceneObjectCount();
	for (int i = 0; i < nObjectCount; ++i)
	{
		m_ctlListCutSceneObjects_.AddString( m_CutSceneWrapper_.GetCutSceneObjectName(i).c_str() );
	}

	m_CutSceneWrapper_.ResetObjectInfo(m_pDlgCutSceneEditor_);
	m_ctlListCutSceneObjects_.SetCurSel(0);
	OnLbnSelchangeListCutsceneObj();

	m_ctlOpenCutSceneEditorOnSelected.SetCheck(bCutSceneEditorChecked);
}

void CRollupCutSceneObjectList::OnLbnSelchangeListCutsceneObj()
{
	int nIdx = m_ctlListCutSceneObjects_.GetCurSel();
	if (nIdx < 0)
		return;

	ChangeObjectSelection(nIdx, (m_ctlOpenCutSceneEditorOnSelected.GetCheck() == TRUE), -1);
}

void CRollupCutSceneObjectList::ChangeObjectSelection( int nObjIdx, bool bTriggerShow, int nSetCurList )
{
	bool bScroll = true;
	if (nSetCurList > -1)
	{
		_ASSERT(nObjIdx > -1);
		m_ctlListCutSceneObjects_.SetCurSel(nObjIdx);
		bScroll = false;
	}

	int nSelectedIdx = m_CutSceneWrapper_.SetCurrentCutSceneObject(nObjIdx, m_pDlgCutSceneEditor_, bTriggerShow, bScroll);

	// 선택 리셋
	if (nObjIdx > -1)
	{
		m_pControlMediator_->GetHelperAction()->RemoveAllSelection(false);
	}
	else if (nSelectedIdx > -1)
	{
		m_ctlListCutSceneObjects_.SetCurSel(nSelectedIdx);
	}
}

void CRollupCutSceneObjectList::OnBnClickedButtonCutscenePreview()
{
	m_bCurrentPreviewRunning = !m_bCurrentPreviewRunning;
	if (m_bCurrentPreviewRunning)
	{
		GetDlgItem(IDC_BUTTON_CUTSCENE_PREVIEW)->SetWindowText("중단");
		m_pControlMediator_->GetToolCamera()->SetEnableLogToStatusBar(false);
		m_CutSceneWrapper_.ShowNoticeForTool(true);

		// for weather
		m_pControlMediator_->GetViewPtr<CRollupEnvWeather>()->GetWeatherInfo(m_weather.bView, m_weather.nTime, m_weather.nWeather, m_weather.strEnvSet);
		m_weather.bFog = m_pControlMediator_->GetViewPtr<CRollupEnvWeather>()->m_fogCheckBox.GetCheck();
		if(FALSE == m_weather.bFog)
			m_pControlMediator_->GetViewPtr<CRollupEnvWeather>()->ToggleFog();

		// reset custom elapsed Time
		UpdateData(TRUE);
		m_fCustomElapsedTime = m_fPreviewStartTime;
	}
	else
	{
		GetDlgItem(IDC_BUTTON_CUTSCENE_PREVIEW)->SetWindowText("미리보기");
		m_pControlMediator_->GetToolCamera()->SetEnableLogToStatusBar(true);
		m_CutSceneWrapper_.ResetRunning();
		m_CutSceneWrapper_.ShowNoticeForTool(false);

		// for weather
		m_pControlMediator_->GetViewPtr<CRollupEnvWeather>()->SetWeatherInfo(m_weather.bView, m_weather.nTime, m_weather.nWeather, m_weather.strEnvSet, 0.1f);
		if (FALSE == m_weather.bFog)
			m_pControlMediator_->GetViewPtr<CRollupEnvWeather>()->ToggleFog();

		// for camera
		rs3::RCameraSceneNode* pCamera = m_pControlMediator_->GetToolCamera();
		if (pCamera)
		{
			rs3::RCameraSceneNodeController* pController = pCamera->GetCameraController(rs3::RCCT_PREUPDATE, "00_AniController");
			if (pController)
			{
				pController->ReserveToRemove();
			}
		}
	}
}

void CRollupCutSceneObjectList::RunCutScene()
{
	m_CutSceneWrapper_.RenderCutSceneActors(m_bCurrentPreviewRunning);

	if (!m_bCurrentPreviewRunning)
		return;

	m_CutSceneWrapper_.RunCutSceneContent(m_fCustomElapsedTime);
	m_fCustomElapsedTime = -1.f;

	float fDuration, fCurrentRunning;
	m_CutSceneWrapper_.GetRunningTime(fDuration, fCurrentRunning);
	char buff[256] = {0,};
	sprintf_s(buff, "%f Running in %f Duration", fCurrentRunning, fDuration);
	CStatusBarHandler::GetInstance()->ShowStatusBarMessage(buff);

	if (fCurrentRunning >= fDuration)
		OnBnClickedButtonCutscenePreview();
}

bool CRollupCutSceneObjectList::SaveToXMLCurrentContent( const char* _pSzFileName )
{
	return m_CutSceneWrapper_.SaveCutSceneContent(_pSzFileName);
}
void CRollupCutSceneObjectList::OnLbnDblclkListCutsceneObj()
{
	rs3::RToolCamera* pCamear = m_pControlMediator_->GetToolCamera();
	if( !pCamear )
		return;

	CLogicObject* pFirstMoveKeyLogicObject = m_CutSceneWrapper_.GetCurrentCutSceneObjectFirstMoveKey();
	if ( !pFirstMoveKeyLogicObject)
		return;

	rs3::RVector vPos;
	if (pFirstMoveKeyLogicObject->GetPosition(vPos))
	{
		pCamear->SetPositionToViewCenter( vPos, Tool3Const::CONST_WB_ENTITY_OBJ_TO_CENTER );
		m_pControlMediator_->GetHelperAction()->AddSelectection( pFirstMoveKeyLogicObject, false);
	}
}

void CRollupCutSceneObjectList::CloseCutSceneEditor()
{
	m_pDlgCutSceneEditor_->ShowEditor(false);
}

void CCutSceneObjectListBox::DrawItem( LPDRAWITEMSTRUCT lpDIS )
{
	if ((int)lpDIS->itemID < 0)
		return;


	int		iPenSize	= 2;
	CDC		*pDC		= CDC::FromHandle(lpDIS->hDC);	

	COLORREF bg			= ::GetSysColor(COLOR_WINDOW);
	COLORREF fg			= GetDataStateColor((int)lpDIS->itemID);

	// 선택되었거나, Focus 가 있을 때 반전
	if (lpDIS->itemState & ODS_SELECTED)
		std::swap(bg, fg);

	// Draw background

	// -------------------------------------------------------------------
	CBrush brush(bg);
	pDC->FillRect(&lpDIS->rcItem, &brush);

	// Draw text
	//--------------------------------------------------------------------
	UINT nFormat = DT_SINGLELINE;

	int		 nOldBkMode		= pDC->SetBkMode(TRANSPARENT);
	COLORREF cOldFg			= pDC->SetTextColor(fg);
	CString	 sItemText;

	CRect rc(lpDIS->rcItem);
	rc.DeflateRect(iPenSize, iPenSize);

	GetText((int)lpDIS->itemID, sItemText);
	pDC->DrawText((LPCTSTR)sItemText, -1, &rc, nFormat);

	pDC->SetTextColor(cOldFg);
	pDC->SetBkMode(nOldBkMode);
}

COLORREF CCutSceneObjectListBox::GetDataStateColor( int nIndex )
{
	COLORREF fg	= ::GetSysColor(COLOR_WINDOWTEXT);

	if(m_pCutSceneWrapper == NULL)
		return fg;

	CUTSCENE_OBJECT_STATE state = m_pCutSceneWrapper->CheckCutSceneObjectControllerState(nIndex);

	if(state == CSBS_WARRING)
		fg = RGB(255, 102, 0);
	else if(state == CSBS_ERROR)
		fg = RGB(255, 0, 0);

	return fg;
}

void CRollupCutSceneObjectList::OnBnClickedButtonCutsceneActorDel()
{
	int nIdx = m_ctlListCutSceneObjects_.GetCurSel();

	// 0은 카메라고 카메라는 지워질수 없다.
	if (nIdx <= 0)
		return;

	m_pControlMediator_->GetHelperAction()->RemoveAllSelection(false);
	m_CutSceneWrapper_.RemoveCutSceneObject(nIdx);
	m_CutSceneWrapper_.ResetObjectInfo(m_pDlgCutSceneEditor_);
	m_ctlListCutSceneObjects_.DeleteString(nIdx);
	m_ctlListCutSceneObjects_.SetCurSel(nIdx-1);

	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);

	OnLbnSelchangeListCutsceneObj();
}

void CRollupCutSceneObjectList::OnBnClickedButtonCutsceneActorNew()
{
	CDlgCutSceneActorType* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneActorType>(m_pControlMediator_, "npc_1", "npc_1");
	if (pDlg)
	{
		std::string strType, strName;
		if (pDlg->m_nActorType == 0)
		{
			bool bAvailable = true;
			int nCount = m_CutSceneWrapper_.GetCutSceneObjectCount();
			for (int i = 0; i < nCount; ++i)
			{
				if(m_CutSceneWrapper_.GetCutSceneObjectName(i) == "my_player")
					bAvailable = false;
			}

			if (!bAvailable)
			{
				AfxMessageBox("my player exists already");
				return;
			}

			strType = "player";
			strName = "my_player";
		}
		else
		{
			CString strFormat;
			strFormat.Format("npc_%d", pDlg->m_nNPCID);
			strType = LPCTSTR(strFormat);
			strName = LPCTSTR(pDlg->m_strNpcName);
		}

		m_CutSceneWrapper_.AddCutSceneObject(strType.c_str(), strName.c_str());
		m_CutSceneWrapper_.ResetObjectInfo(m_pDlgCutSceneEditor_);

		int nListIdx = m_ctlListCutSceneObjects_.AddString(strName.c_str());
		m_ctlListCutSceneObjects_.SetCurSel(nListIdx);
		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);

		OnLbnSelchangeListCutsceneObj();

		delete pDlg;
	}
}

void CRollupCutSceneObjectList::OnBnClickedButtonOpenCutsceneEditor()
{
	ChangeObjectSelection(m_ctlListCutSceneObjects_.GetCurSel(), true, -1);
}

void CRollupCutSceneObjectList::ChangeCurrentObjectListName( const char* _pSzNewName )
{
	int nIdx = m_ctlListCutSceneObjects_.GetCurSel();
	m_ctlListCutSceneObjects_.DeleteString(nIdx);
	m_ctlListCutSceneObjects_.InsertString(nIdx, _pSzNewName);
	m_ctlListCutSceneObjects_.SetCurSel(nIdx);

	OnLbnSelchangeListCutsceneObj();
}
