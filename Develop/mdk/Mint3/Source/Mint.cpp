#include "stdafx.h"
#include "MWLua.h"
#include "MScriptManager.h"
#include "MFileSystem.h"
#include "Mint.h"
#include "MWidget.h"
#include "MWidgetManager.h"
#include "TChar.h"
#include "MFileDialog.h"
#include "MPopupMenu.h"
#include "MListBox.h"
#include "MComboBox.h"
#include "MGroup.h"
#include "MSlider.h"
#include "MSpinCtrl.h"
#include "MHyperText.h"
#include "MTextArea.h"
#include "MResource.h"
#include "MDragObject.h"
#include "MPicture.h"
#include "MAnimation.h"
#include "MHotKey.h"
#include "MActionKey.h"
#include "MBmButton.h"
#include "MTabCtrl.h"
#include "MPanel.h"
#include "MPaint.h"
#include "MMTimer.h"
#include "MConsole.h"
#include "MSheet.h"
#include "MintLoader.h"
#include "MMainFrame.h"
#include "MButtonGroup.h"
#include "MView.h"
#include "MLayer.h"
#include "MTextView.h"
#include "MTreeView.h"

// madduck [4/5/2006]
#include "MLookManager.h"

namespace mint3 {

class MBoundsListItem : public MListItem{
public:
	//MWidget*	m_pWidget;
	char		m_szHierachicalName[256];
	char		m_szBounds[64];
public:
	virtual const char* GetString(void){
		return GetString(0);
	}
	virtual const char* GetString(int i){
		if(i==0) return m_szHierachicalName;
		else if(i==1) return m_szBounds;
		return NULL;
	}
};


class MDesignerModeInfo : public MFrame
{
protected:
	MListBox*	m_pBoundsList;
	MButton*	m_pDelete;
	MButton*	m_pReset;
	MButton*	m_pLoad;
	MButton*	m_pSave;
protected:
	virtual void OnSize(int w, int h)
	{
		MRECT r = GetClientRect();
		const int BTN_W	= 90;
		const int BTN_H	= 20;
		m_pBoundsList->SetBounds(r.x+2, r.y+2, r.w-4, r.h-BTN_H*2-10);
		m_pDelete->SetBounds(r.x+r.w-BTN_W*2-5, r.y+r.h-BTN_H*2-5, BTN_W, BTN_H);
		m_pReset->SetBounds(r.x+r.w-BTN_W, r.y+r.h-BTN_H*2-5, BTN_W, BTN_H);
		m_pLoad->SetBounds(r.x+r.w-BTN_W*2-5, r.y+r.h-BTN_H, BTN_W, BTN_H);
		m_pSave->SetBounds(r.x+r.w-BTN_W, r.y+r.h-BTN_H, BTN_W, BTN_H);
	}
	virtual bool OnCommand(MWidget* pWindow, const char* szMessage)
	{
		//if(pWindow==m_pSave && IsMsg(MBTN_CLK_MSG, szMessage)==true){
		//	MSaveDesignerMode();
		//	return true;
		//}
		//else if(pWindow==m_pLoad && IsMsg(MBTN_CLK_MSG, szMessage)==true){
		//	MLoadDesignerMode();
		//	MUpdateDesignerMode();
		//	return true;
		//}
		//else if(pWindow==m_pReset && IsMsg(MBTN_CLK_MSG, szMessage)==true){
		//	MResetDesignerMode();
		//	MGetMainFrame()->ResetModifiedByDesignerFlagRecursively(false);
		//	return true;
		//}
		//else if(pWindow==m_pDelete && IsMsg(MBTN_CLK_MSG, szMessage)==true){
		//	int i = m_pBoundsList->GetSelIndex();
		//	if(i<0) return true;
		//	MBoundsListItem* pItem = (MBoundsListItem*)m_pBoundsList->Get(i);
		//	if(pItem==NULL) return true;
		//	MWidget* pWidget = MGetMainFrame()->FindWidgetByHierarchicalName(pItem->m_szHierachicalName);
		//	if(pWidget==NULL) return true;
		//	pWidget->ResetModifiedByDesignerFlag(false);
		//	m_pBoundsList->Remove(i);
		//	return true;
		//}
		return false;
	}
public:
	MDesignerModeInfo(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL)
		: MFrame(szName, pParent, pListener){
			m_pBoundsList = new MListBox(this, this);
			m_pBoundsList->EnableDesignerMode(false);
			m_pBoundsList->AddField("Widget", 200);
			m_pBoundsList->AddField("Bounds", 100);

			m_pDelete = new MButton("DM Delete", this, this);
			m_pDelete->SetText("Delete");

			m_pReset = new MButton("DM Reset All", this, this);
			m_pReset->SetText("Reset All");

			m_pLoad = new MButton("DM Load Design", this, this);
			m_pLoad->SetText("Load Design");

			m_pSave = new MButton("DM Save Design", this, this);
			m_pSave->SetText("Save Design");

			EnableDesignerMode(false);
	}
	virtual ~MDesignerModeInfo(void){
		delete m_pBoundsList;
		delete m_pDelete;
		delete m_pReset;
		delete m_pLoad;
		delete m_pSave;
	}

	void AddBoundsItem(const char* szHierachicalName, MRECT& r){
		MBoundsListItem* pItem = new MBoundsListItem;
		strcpy(pItem->m_szHierachicalName, szHierachicalName);
		sprintf(pItem->m_szBounds, "%d,%d,%d,%d", r.x, r.y, r.w, r.h);
		m_pBoundsList->Add(pItem);
	}
	bool SetBoundsItem(int i, const char* szHierachicalName, MRECT& r){
		MBoundsListItem* pItem = new MBoundsListItem;
		strcpy(pItem->m_szHierachicalName, szHierachicalName);
		sprintf(pItem->m_szBounds, "%d,%d,%d,%d", r.x, r.y, r.w, r.h);
		return m_pBoundsList->Set(i, pItem);
	}
	int GetBoundsItemCount(void){
		return m_pBoundsList->GetCount();
	}
	void RemoveBoundsItem(int i){
		m_pBoundsList->Remove(i);
	}

	void RefreshResourceMap(void)
	{
		//MResourceMap* pRM = &g_ResourceMap;
		//pRM->clear();
		//g_pMainFrame->GetResourceMapRecursively(pRM, true, true);
		//int nCount = 0;
		//for(MResourceMap::iterator i=pRM->begin(); i!=pRM->end(); i++){
		//	string s = (*i).first;
		//	MWIDGETRESOURCE* pWR = (*i).second;
		//	if(pWR->bBounds==false) continue;
		//	//MRECT r = (*i).second;
		//	if(SetBoundsItem(nCount, s.c_str(), pWR->Bounds)==false){
		//		//g_pBoundsInfo->m_pBoundsList->Add(temp);
		//		AddBoundsItem(s.c_str(), pWR->Bounds);
		//	}
		//	nCount++;
		//}

		//if(nCount<GetBoundsItemCount()){
		//	for(int i=0; i<GetBoundsItemCount()-nCount; i++){
		//		RemoveBoundsItem(GetBoundsItemCount()-1);
		//	}
		//}
	}
};

bool g_bDesignerMode = false;
int g_nSnapSize = 4;
MDesignerModeInfo*	g_pBoundsInfo = NULL;


////////////////////////////////////////////////////
Mint* Mint::m_pInstance = NULL;

void Mint::DrawCandidateList(MDrawContext* pDC, MPOINT& p)
{
	if(GetCandidateCount()>0){
		MCOLOR c = pDC->GetColor();

		MRECT r(p.x, p.y, GetCandidateListWidth(), GetCandidateListHeight());

//		pDC->SetColor(MCOLOR(0x30000000));		// 그림자
//		pDC->FillRectangle( r.x+5, r.y+5, r.w, r.h);
		
		pDC->SetColor(MCOLOR(0xFF050505));		// 프레임 바탕
		pDC->FillRectangle(r);

		pDC->SetColor(MCOLOR(0xFF505050));		// 프레임 어두운 부분
		pDC->Rectangle( r.x+1, r.y+1, r.w,   r.h);

		pDC->SetColor(MCOLOR(0xFFB0B0B0));		// 프레임 밝은 부분
		pDC->Rectangle(r);

		MFont* pFont = pDC->GetFont();
		pDC->SetFont( MFontManager::Get( "Default"));		// 강제로 폰트를 디폴트로 고정한다.

		int nStart = GetCandidatePageStart();

//		if (pDC->BeginFont())
		{
			char temp[MIMECOMPOSITIONSTRING_LENGTH+4];
			for(int i=nStart; i<(int)min(GetCandidateCount(), nStart+GetCandidatePageSize()); i++)
			{
				const char* szCandidate = GetCandidate(i);
				if(i==GetCandidateSelection())
					pDC->SetColor( MColorTable::NORMAL );
				else
					pDC->SetColor(MCOLOR(0xFF909090));
				int nIndexInPage = i-nStart;
				sprintf(temp, "%d: %s", nIndexInPage+1, szCandidate);
				pDC->Text(p.x+4, p.y + nIndexInPage*pDC->GetFont()->GetHeight() + 4, temp);
			}

			// 현재 선택 인덱스 및 총 개수 출력
			sprintf(temp, "(%d/%d)", GetCandidateSelection()+1, GetCandidateCount());
			pDC->SetColor( MColorTable::NORMAL );
			pDC->Text(p.x + 4, p.y + GetCandidatePageSize()*pDC->GetFont()->GetHeight() + 4, temp);

//			pDC->EndFont();
		}

		pDC->SetColor(c);
		pDC->SetFont( MFontManager::Get( pFont->m_szName));		// 원래 폰트로 복구
	}
}


Mint::Mint()
{
	_ASSERT(m_pInstance==NULL);	// Singleton!!!
	m_pInstance = this;
	m_pMainFrame = NULL;
	m_pDC = NULL;
	m_pScriptManager = NULL;
	m_hImc = NULL;

//	m_nDragObjectID = -1;
	m_szDragObjectString[0] = 0;
	m_szDragObjectItemString[0] = 0;
	m_pDragObjectBitmap = NULL;
	m_bVisibleDragObject = false;
	m_pDropableObject = NULL;
	m_pDragSourceObject = NULL;

	m_nWorkspaceWidth = 640;
	m_nWorkspaceHeight = 480;
	m_nStandardWidth = m_nWorkspaceWidth;
	m_nStandardHeight = m_nWorkspaceHeight;
	m_fRatioToStandardScreenWidth = 1.0f;
	m_fRatioToStandardScreenHeight = 1.0f;

	m_fnGlobalEventCallBack = NULL;

	m_pAnimationTemplateManager = NULL;

	memset(m_ActionKeyPressedTable, 0, sizeof(bool)*ACTIONKEYMAP_IDCOUNT);

	m_pCandidateList = NULL;
	m_nCandidateListSize = 0;

	m_nCompositionAttributeSize = 0;
	memset(m_nCompositionAttributes, 0, sizeof(BYTE)*(MIMECOMPOSITIONSTRING_LENGTH));
	//memset(m_dwCompositionClauses, 0, sizeof(DWORD)*(MIMECOMPOSITIONSTRING_LENGTH));

	m_nCompositionCaretPosition = 0;

	m_bEnableIME = false;
	m_bEnableCharInput = false;

	m_pWidgetManager = new MWidgetManager(this);
	m_pScriptManager = new MScriptManager();
	m_pButtonGroupManager = new MButtonGroupManager();

	// madduck [4/5/2006]
	m_pLookManager	= new MLookManager();
}

Mint::~Mint()
{
	Clear();

	m_fnGlobalEventCallBack = NULL;
	m_pInstance = NULL;

	if ( m_pTooltip)
		m_pTooltip = NULL;

	delete m_pWidgetManager;m_pWidgetManager=NULL;
	delete m_pScriptManager;
	m_pScriptManager = NULL;
	delete m_pButtonGroupManager; m_pButtonGroupManager=NULL;

	// madduck [4/5/2006]
	delete m_pLookManager; m_pLookManager = NULL;
}

Mint* Mint::GetInstance(void)
{
	_ASSERT(m_pInstance!=NULL);
	return m_pInstance;
}

bool Mint::Initialize(int nWorkspaceWidth, int nWorkspaceHeight, int nStandardWidth, int nStandardHeight, 
					  MDrawContext* pDC, MFont* pDefaultFont, MWLua* pLua)
{
	_ASSERT(m_pMainFrame==NULL);

	m_pDC = pDC;

	if (m_pScriptManager->Initialize(pLua) == false)
	{
		assert(!"Script Manager Init 실패");
	}

//	MFontManager::Add(pDefaultFont);
	MFontManager::SetDefaultFont(pDefaultFont);
	m_pAnimationTemplateManager = new MAnimationTemplateManager;

	m_pMainFrame = new MMainFrame("Mint");
	m_pMainFrame->SetBounds(0, 0, nWorkspaceWidth, nWorkspaceHeight);
	m_pScriptManager->DeclWidget(MINT_WIDGET, "Mint", m_pMainFrame);	// 루아 오브젝트 생성

	m_pTooltip = new MTooltip( m_pMainFrame);

	g_pBoundsInfo = new MDesignerModeInfo("MINT Designer", m_pMainFrame, m_pMainFrame);
	g_pBoundsInfo->SetBounds(10, 10, 300, 400);
	g_pBoundsInfo->Show(false);


	SetWorkspaceSize(nWorkspaceWidth, nWorkspaceHeight);
	SetStandardSize(nStandardWidth, nStandardHeight);
	// 화면 비율값을 미리 계산 해둔다.
	m_fRatioToStandardScreenWidth = (float)nWorkspaceWidth / nStandardWidth;
	m_fRatioToStandardScreenHeight = (float)nWorkspaceHeight / nStandardHeight;

	return true;
}

void Mint::Finalize(void)
{
	if(g_pBoundsInfo!=NULL)
	{
		delete g_pBoundsInfo;
		g_pBoundsInfo = NULL;
	}

	if(m_pMainFrame!=NULL){
		delete m_pMainFrame;
		m_pMainFrame = NULL;
	}

	// LookManager 삭제
	m_pLookManager->ClearLooks();

	MFontManager::Destroy();

	MCursorSystem::Destroy();

	if(m_pAnimationTemplateManager)
	{
		m_pAnimationTemplateManager->Destroy();
		delete m_pAnimationTemplateManager;
		m_pAnimationTemplateManager = NULL;
	}

	m_pDC = NULL;
}

bool Mint::ProcessEvent(MEvent& e)
{
	if(!m_pMainFrame) return false;


	// Clear tooltip info cash
	if ( e.nMessage == MWM_MOUSEMOVE)
	{
		m_TooltipInfo.Clear();

		MWidget* pTooltipWidget = m_pMainFrame->Find( e.Pos);
		if ( pTooltipWidget != NULL  &&  pTooltipWidget->GetTooltipMsg() != NULL  &&  pTooltipWidget->GetTooltipMsg()[ 0] != 0)
			m_pTooltip->SetTooltip( pTooltipWidget, e.Pos, pTooltipWidget->GetTooltipMsg());
		else
			m_pTooltip->SetTooltip( NULL, e.Pos, "");
	}


	// Drag & Drop -------------------------------------------------------------
	if(m_pDragSourceObject!=NULL){
#define DRAm_VISIBLE_LENGTH	2	// 드래그 오브젝트를 보여주기 시작하는 간격
		if(e.nMessage==MWM_MOUSEMOVE){
			MPOINT p = e.Pos;
			//MPOINT p = MEvent::GetMousePos();
			int px = m_GrabPoint.x - p.x;
			if ( px < 0)  px *= -1;
			int py = m_GrabPoint.y - p.y;
			if ( py < 0)  py *= -1;

			if( m_bVisibleDragObject==false &&
				((px > DRAm_VISIBLE_LENGTH) || (py > DRAm_VISIBLE_LENGTH)))
                m_bVisibleDragObject = true;
			MWidget* pFind = FindWidget(p);
			if(pFind!=NULL && pFind->IsDropable(m_pDragSourceObject)==true)
				m_pDropableObject = pFind;
			else
				m_pDropableObject = NULL;
		}
		if(e.nMessage==MWM_LBUTTONUP){
			MPOINT p = e.Pos;
			MWidget* pFind = FindWidgetDropAble(p);
			if ( pFind  &&  pFind != m_pDragSourceObject)
			{
				if(pFind!=NULL && pFind->IsDropable(m_pDragSourceObject)==true)
					pFind->Drop(m_pDragSourceObject, m_pDragObjectBitmap, m_szDragObjectString, m_szDragObjectItemString);	// 해당 위젯에 드롭
				else
					m_pDragSourceObject->UnDrop(pFind, m_pDragObjectBitmap, m_szDragObjectString, m_szDragObjectItemString);	// 드롭을 하는 위젯에게 실패를 알림

				m_pDragSourceObject->InitMouseState();
				m_pDragSourceObject = NULL;
				m_pMainFrame->ReleaseCapture();
				return true;
			}
			else
			{
				if ( pFind)
					m_pDragSourceObject->UnDrop(pFind, m_pDragObjectBitmap, m_szDragObjectString, m_szDragObjectItemString);	// 드롭을 하는 위젯에게 실패를 알림

				m_pDragSourceObject = NULL;
				m_pMainFrame->ReleaseCapture();
			}
		}
	}

	// 전역 이벤트 처리
	if (m_fnGlobalEventCallBack) 
		if (m_fnGlobalEventCallBack(&e)==true) return true;

	// 일반 이벤트 처리
	if(m_pMainFrame->Event(&e)==true) return true;
	// 없으면 Accelerator 처리
	if(m_pMainFrame->EventAccelerator(&e)==true) return true;
	// Default Key(Enter, ESC) 처리
	if(m_pMainFrame->EventDefaultKey(&e)==true) return true;


	// Set tooltip
//	if ( e.nMessage == MWM_MOUSEMOVE)
//		m_pTooltip->SetTooltip( m_TooltipInfo.m_pSender, m_TooltipInfo.m_ptPoint, m_TooltipInfo.m_strString);

	return false;
}

bool Mint::ProcessEvent(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	MEvent e;
	int nResult = e.TranslateEvent(hwnd, message, wparam, lparam);

	if(nResult&EVENT_MINT_TRANSLATED) return ProcessEvent(e);
	if(nResult&EVENT_PROCESSED) return true;	// 무조건 처리됨

	return false;
}

void Mint::Run(void)
{
	if (m_pMainFrame)
		m_pMainFrame->Run();

	if (g_pBoundsInfo)
		g_pBoundsInfo->RefreshResourceMap();
}

void Mint::Draw(void)
{
	if(!m_pMainFrame) return;

	MDrawContext* pDC = m_pDC;
	if(pDC==NULL) return;

	// Draw widgets
	pDC->SetOpacity(1.0f);
	pDC->SetClipRect( 0, 0, MGetWorkspaceWidth()-1, MGetWorkspaceHeight()-1);
	pDC->SetOrigin( 0, 0);
	pDC->BeginDraw();
	m_pMainFrame->Draw(pDC);

	// Draw tooltip
	pDC->SetClipRect( 0, 0, MGetWorkspaceWidth()-1, MGetWorkspaceHeight()-1);
	pDC->SetOrigin( 0, 0);
	m_pTooltip->Draw( pDC);

/*	pDC->SetClipRect( 0, 0, MGetWorkspaceWidth()-1, MGetWorkspaceHeight()-1);
	pDC->SetOrigin( 0, 0);

	DWORD dwCurrTime = timeGetTime();
	static DWORD dwPrevTime = dwCurrTime;
	float fElipsed = (float)( dwCurrTime - dwPrevTime) * 0.004f;
	dwPrevTime = dwCurrTime;

	if ( m_pToolTipShowed  &&  m_pToolTipShowed == m_pToolTip)
		m_fToolTipOpacity = min( m_fToolTipOpacity + fElipsed,  1.0f);
	else
		m_fToolTipOpacity = max( m_fToolTipOpacity - fElipsed, -1.0f);

	pDC->SetOpacity( max( m_fToolTipOpacity, 0.0f));

	if ( m_pDragSourceObject != NULL)
		m_pToolTipShowed = NULL;
	else if ( m_fToolTipOpacity < -0.5f)
		m_pToolTipShowed = m_pToolTip;

	if ( m_pToolTipShowed)
		m_pToolTipShowed->Draw( pDC);
	*/


	MRECT sr = m_pMainFrame->GetScreenRect();
	pDC->SetOpacity(1.0f);
	pDC->SetOrigin(MPOINT(sr.x, sr.y));
	pDC->SetClipRect(sr);

	if(MIsDesignerMode()==true)
	{
		DrawDesignerMode(pDC);
	}

	// Candidate List 그리기
	DrawCandidateList(pDC, m_CandidateListPos);


	// Drag & Drop
	/*
	MDragObject* pDragObject = MGetDragObject();
	if(pDragObject!=NULL){
		pDC->SetBitmap(pDragObject->GetBitmap());
		MPOINT p = MEvent::GetMousePos();
		pDC->Draw(p);
		pDC->Text(p, pDragObject->GetString());
	}
	*/
	//int nDragObjectID = GetDragObject();
	MWidget* pDragSourceObject = GetDragObject();
	if(pDragSourceObject!=NULL && m_bVisibleDragObject==true){
		// 현재 위치에서 드롭할 수 있는 위젯을 표시
		if(m_pDropableObject!=NULL){
			MRECT r = m_pDropableObject->GetScreenRect();
			pDC->SetColor(0, 0, 0, 32);
			pDC->FillRectangle(r);
		}

		// 커서에 붙는 드래그 오브젝트 그리기
		pDC->SetBitmap(m_pDragObjectBitmap);
		MPOINT p = MEvent::GetMouseClientPos();
		p.x-=(m_pDragObjectBitmap!=NULL?m_pDragObjectBitmap->GetWidth()/2:0);
		p.y-=(m_pDragObjectBitmap!=NULL?m_pDragObjectBitmap->GetHeight()/2:0);
		if (m_pDragObjectBitmap)
		{
			pDC->SetOpacity(0.5f);
			pDC->Draw(p.x, p.y, m_pDragObjectBitmap->GetWidth(), m_pDragObjectBitmap->GetHeight());
			pDC->SetOpacity(1.0f);
		}
		p.x+=((m_pDragObjectBitmap!=NULL?m_pDragObjectBitmap->GetWidth():0) + 2);
		p.y+=((m_pDragObjectBitmap!=NULL?m_pDragObjectBitmap->GetHeight()/2:0)-pDC->GetFont()->GetHeight()/2);

//		if (pDC->BeginFont())
		{
			pDC->SetColor(0, 0, 0, 128);
			pDC->Text(p, m_szDragObjectString);
			pDC->SetColor(255, 255, 255, 128);
			p.x--;
			p.y--;
			pDC->Text(p, m_szDragObjectString);
//			pDC->EndFont();
		}
	}

	// Cursor
	MPOINT p = MEvent::LatestPos;
	MCursorSystem::Draw(pDC, p.x, p.y);

	pDC->EndDraw();

	pDC->SetClipRect( 0, 0, MGetWorkspaceWidth()-1, MGetWorkspaceHeight()-1);
	pDC->SetOrigin( 0, 0);
}

bool Mint::EventActionKey(unsigned long int nKey, bool bPressed)
{
	MEvent e;

	if(bPressed==true) e.nMessage = MWM_ACTIONKEYDOWN;
	else e.nMessage = MWM_ACTIONKEYUP;
	e.nKey = nKey;

	if (m_pMainFrame == NULL)
		return false;

	if (m_fnGlobalEventCallBack) m_fnGlobalEventCallBack(&e);
	m_pMainFrame->Event(&e);

	ACTIONKEYMAP::iterator i = m_ActionKeyMap.find(nKey);
	int nActionID = -1;
	if(i!=m_ActionKeyMap.end()) nActionID = (*i).second;
	if(bPressed==true){
		e.nMessage = MWM_ACTIONPRESSED;
		if(nActionID>=0 && nActionID<ACTIONKEYMAP_IDCOUNT) m_ActionKeyPressedTable[nActionID] = true;
	}
	else{
		e.nMessage = MWM_ACTIONRELEASED;
		if(nActionID>=0 && nActionID<ACTIONKEYMAP_IDCOUNT) m_ActionKeyPressedTable[nActionID] = false;
	}
	e.nKey = nActionID;

	if (m_fnGlobalEventCallBack) m_fnGlobalEventCallBack(&e);

	return m_pMainFrame->Event(&e);
}

MMainFrame* Mint::GetMainFrame(void)
{
	return m_pMainFrame;
}

MDrawContext* Mint::GetDrawContext(void)
{
	return m_pDC;
}

#ifdef WIN32
HWND m_hWnd = NULL;
void Mint::SetHWND(HWND hWnd)
{
	m_hWnd = hWnd;

	// ime status 창을 hide한다
	HWND imehwnd=ImmGetDefaultIMEWnd(Mint::GetInstance()->GetHWND());

	LRESULT lr=SendMessage(imehwnd,WM_IME_CONTROL ,IMC_CLOSESTATUSWINDOW,0);
	_ASSERT(lr==0);

	HIMC	hImc;
	hImc = ImmGetContext(Mint::GetInstance()->GetHWND());
	if (hImc)
	{
		// Get current IME status
		DWORD dwIMEConvMode, dwSentMode;
		ImmGetConversionStatus(hImc, &dwIMEConvMode, &dwSentMode);
		ImmSetConversionStatus(hImc,IME_CMODE_ALPHANUMERIC, dwSentMode);

		Mint::EnableIME(false);
	}
}
HWND Mint::GetHWND(void)
{
	return m_hWnd;
}
#endif

void Mint::EnableIME(bool bEnable)
{
	_ASSERT(GetHWND());
	if ( (bEnable == true) && MEvent::GetIMESupport()) {
		if (m_hImc) {
			ImmAssociateContext(GetHWND(), m_hImc);
			m_hImc = NULL;	// EnableIME(false) 할때 다시 셋팅된다
			::SetFocus(GetHWND());
		}
		m_bEnableIME = true;
	} else {
		// HIMC를 m_hImc에 임시 보관해 뒀다가, Enable때 복구한다.
		m_hImc = ImmGetContext(GetHWND());
		if (m_hImc) {
			ImmAssociateContext(GetHWND(), NULL);
			ImmReleaseContext(GetHWND(), m_hImc);
			::SetFocus(GetHWND());
		}
		//ImmDisableIME(0);
		m_bEnableIME = false;
	}

	// Composition중인 문자열 제거
	HIMC hImc = ImmGetContext(GetInstance()->GetHWND());
	if(hImc!=NULL){
		ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
		ImmReleaseContext(GetInstance()->GetHWND(), hImc);
	}
}

bool Mint::IsEnableIME(void)
{
	return m_bEnableIME;
}

void Mint::EnableCharInput(bool bEnable)
{
	m_bEnableCharInput = bEnable;
}

bool Mint::IsEnableCharInput()
{
	return m_bEnableCharInput;
}

int Mint::RegisterHotKey(unsigned long int nModifier, unsigned long int nVirtKey)
{
#ifdef WIN32
	_ASSERT(m_hWnd!=NULL);	// Should call SetHWND() before this function

	char szAtomName[64] = {0,};
	if(nModifier==MMODIFIER_ALT) strcat(szAtomName, "Alt");
	if(nModifier==MMODIFIER_CTRL) strcat(szAtomName, "Ctrl");
	if(nModifier==MMODIFIER_SHIFT) strcat(szAtomName, "Shift");
	char szKey[16] = {0, };
	sprintf(szKey, "%d", nVirtKey);
	strcat(szAtomName, szKey);

	int nID = GlobalAddAtom(szAtomName);
	if(nID==0) return 0;

	if(::RegisterHotKey(m_hWnd, nID, nModifier, nVirtKey)==TRUE)
		return nID;

	return 0;
#else
	// Not Implemented
	_ASSERT(FALSE);
	return 0;
#endif
}
void Mint::UnregisterHotKey(int nID)
{
#ifdef WIN32
	_ASSERT(m_hWnd!=NULL);	// Should call SetHWND() before this function
	::UnregisterHotKey(m_hWnd, nID);
	GlobalDeleteAtom(nID);
#else
	// Not Implemented
	_ASSERT(FALSE);
	return 0;
#endif
}

bool Mint::RegisterActionKey(int nActionID, unsigned long int nKey)
{
	if(nActionID<0 || nActionID>=ACTIONKEYMAP_IDCOUNT){
		_ASSERT(FALSE);	// 0 ~ ACTIONKEYMAP_IDCOUNT-1 사이값이여야 한다.
		return false;
	}

	m_ActionKeyMap.insert(ACTIONKEYMAP::value_type(nKey, nActionID));
	return true;
}

bool Mint::UnregisterActionKey(int nActionID)
{
	if(nActionID<0 || nActionID>=ACTIONKEYMAP_IDCOUNT){
		_ASSERT(FALSE);	// 0 ~ ACTIONKEYMAP_IDCOUNT-1 사이값이여야 한다.
		return false;
	}

	for(ACTIONKEYMAP::iterator i=m_ActionKeyMap.begin(); i!=m_ActionKeyMap.end(); i++){
		if((*i).second==nActionID){
			m_ActionKeyMap.erase(i);
			return true;
		}
	}
	return false;
}

const char* Mint::GetActionKeyName(unsigned long int nKey)
{
	_ASSERT(FALSE);
	// Not Implemented. User must implement

	static char* szNullName = "Unknown";
	return szNullName;
}

unsigned long int Mint::GetActionKey(int nActionID)
{
	for(ACTIONKEYMAP::iterator i=m_ActionKeyMap.begin(); i!=m_ActionKeyMap.end(); i++){
		if((*i).second==nActionID){
			return (*i).first;
		}
	}
	return -1;
}

bool Mint::IsActionKeyPressed(int nActionID)
{
	if(nActionID<0 || nActionID>=ACTIONKEYMAP_IDCOUNT){
		_ASSERT(FALSE);	// 0 ~ ACTIONKEYMAP_IDCOUNT-1 사이값이여야 한다.
		return false;
	}
	return m_ActionKeyPressedTable[nActionID];
}

void Mint::SetGlobalEvent(MGLOBALEVENTCALLBACK pGlobalEventCallback)
{
	m_fnGlobalEventCallBack = pGlobalEventCallback;
}

/*
MDragObject* m_pDragObject = NULL;

MDragObject* MSetDragObject(MDragObject* pDragObject)
{
	MDragObject* pPrevDragObject = m_pDragObject;
	m_pDragObject = pDragObject;
	return pPrevDragObject;
}
MDragObject* MGetDragObject(void)
{
	return m_pDragObject;
}
*/

MWidget* Mint::SetDragObject(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString)
{
	MWidget* pPrev = m_pDragSourceObject;
	m_pDragSourceObject = pSender;
	if(szString==NULL) m_szDragObjectString[0] = 0;
	else strncpy(m_szDragObjectString, szString, 256);
	if(szItemString==NULL) m_szDragObjectItemString[0] = 0;
	else strncpy(m_szDragObjectItemString, szItemString, 256);
	m_pDragObjectBitmap = pBitmap;
	m_GrabPoint = MEvent::GetMouseClientPos();
	m_bVisibleDragObject = false;
	return pPrev;
}
MWidget* Mint::GetDragObject(void)
{
	return m_pDragSourceObject;
}

/*
#define MINT_WIDGET	"GenericWidget"
#define MINT_LABEL			"Label"
#define MINT_BUTTON			"Button"
#define MINT_EDIT			"Edit"
#define MINT_LISTBOX		"ListBox"
#define MINT_COMBOBOX		"ComboBox"
#define MINT_FILEBOX		"FileBox"
#define MINT_FRAME			"Frame"
#define MINT_GROUP			"Group"
#define MINT_POPUPMENU			"Menu"
#define MINT_SCROLLBAR		"ScrollBar"
#define MINT_SLIDER			"Slider"
#define MINT_TEXTAREA		"TextArea"
#define MINT_MSGBOX		"MessageBox"
*/

/*
char* m_szWidgetClasses[] = {
	MINT_WIDGET,
	MINT_LABEL,
	MINT_BUTTON,
	MINT_EDIT,
	MINT_LISTBOX,
	MINT_COMBOBOX,
	MINT_FILEBOX,
	MINT_FRAME,
	MINT_GROUP,
	MINT_POPUPMENU,
	MINT_SCROLLBAR,
	MINT_SLIDER,
	MINT_TEXTAREA,
	MINT_MSGBOX,
};
*/

MWidget* Mint::NewWidget(const char* szClass, const char* szName, MWidget* pParent, MListener* pListener)
{
	MWidget* pNewWidget = NULL;

	if(strcmp(szClass, MINT_WIDGET)==0) pNewWidget = new MWidget(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_LABEL)==0) pNewWidget = new MLabel(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_BUTTON)==0) pNewWidget = new MButton(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_BMBUTTON)==0) pNewWidget = new MBmButton(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_EDIT)==0) pNewWidget = new MEdit(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_LISTBOX)==0) pNewWidget = new MListBox(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_COMBOBOX)==0) pNewWidget = new MComboBox(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_FILEBOX)==0) pNewWidget = new MFileBox(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_FRAME)==0) pNewWidget = new MFrame(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_GROUP)==0) pNewWidget = new MGroup(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_POPUPMENU)==0) pNewWidget = new MPopupMenu(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_SCROLLBAR)==0) pNewWidget = new MScrollBar(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_SPINCTRL)==0) pNewWidget = new MSpinCtrl(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_SLIDER)==0) pNewWidget = new MSlider(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_TEXTAREA)==0) pNewWidget = new MTextArea(256, szName, pParent, pListener);
	else if(strcmp(szClass, MINT_TABCTRL)==0) pNewWidget = new MTabCtrl(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_MSGBOX)==0) pNewWidget = new MMsgBox(szName, pParent, pListener, MT_OK);
	else if(strcmp(szClass, MINT_PICTURE)==0) pNewWidget = new MPicture(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_ANIMATION)==0) pNewWidget = new MAnimation(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_MENUITEM)==0) pNewWidget = new MMenuItem(szName);
	else if(strcmp(szClass, MINT_HOTKEY)==0) pNewWidget = new MHotKey(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_ACTIONKEY)==0) pNewWidget = new MActionKey(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_PANEL)==0) pNewWidget = new MPanel(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_PAINT)==0) pNewWidget = new MPaint(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_TIMER)==0) pNewWidget = new MMTimer(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_CONSOLE)==0) pNewWidget = new MConsole(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_SHEET)==0) pNewWidget = new MSheet(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_VIEW)==0) pNewWidget = new MView(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_TEXTVIEW)==0) pNewWidget = new MTextView(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_TREEVIEW)==0) pNewWidget = new MTreeView(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_LAYER)==0) pNewWidget = new MLayer(szName, pParent, pListener);

	return pNewWidget;
}

MWidget* Mint::FindWidgetDropAble(MPOINT& p)
{
	return m_pMainFrame->FindDropAble(p);
}

MWidget* Mint::FindWidget(MPOINT& p)
{
	return m_pMainFrame->Find(p);
}

MWidget* Mint::FindWidget(int x, int y)
{
	return m_pMainFrame->Find(x, y);
}

MWidget* Mint::FindWidget(const char* szName)
{
	return m_pWidgetManager->FindWidget(string(szName));
}

int Mint::GetWorkspaceWidth(void)
{	
	return m_nWorkspaceWidth;
}

int Mint::GetWorkspaceHeight(void)
{
	return m_nWorkspaceHeight;
}

int Mint::GetStandardWidth(void)
{	
	return m_nStandardWidth;
}

int Mint::GetStandardHeight(void)
{
	return m_nStandardHeight;
}

void Mint::SetWorkspaceSize(int w, int h)
{
	m_nWorkspaceWidth = w;
	m_nWorkspaceHeight = h;
}

void Mint::SetStandardSize(int w, int h)
{
	m_nStandardWidth = w;
	m_nStandardHeight = h;
}

const char* Mint::GetDefaultFontName(void) const
{
	static char* szFontName[] = {
		"Tahoma",		// Default
		"Gulim",		// 한글
		"MS PGothic",	// 일본
		"MingLiU",		// 번체
		"NSimSun"		// 간체
	};

	int nFont = 0;
	if(GetPrimaryLanguageIdentifier()==LANG_KOREAN) nFont = 1;
	else if(GetPrimaryLanguageIdentifier()==LANG_JAPANESE) nFont = 2;
	else if(GetPrimaryLanguageIdentifier()==LANG_CHINESE){
		if(GetSubLanguageIdentifier()==SUBLANG_CHINESE_TRADITIONAL) nFont = 3;
		else nFont = 4;
	}

	return szFontName[nFont];
}

int Mint::GetPrimaryLanguageIdentifier(void) const
{
	/*
	Language Identifiers
	+-------------------------+-------------------------+
	|      SubLanguage ID     |   Primary Language ID   |
	+-------------------------+-------------------------+
	15                    10  9                         0   bit

	ms-help://MS.VSCC.2003/MS.MSDNQTR.2003FEB.1042/intl/nls_8xo3.htm
	ms-help://MS.VSCC.2003/MS.MSDNQTR.2003FEB.1042/intl/nls_238z.htm
	*/
	HKL hKeyboardLayout = GetKeyboardLayout(0);
	WORD nLanguageIdentifier = LOWORD(hKeyboardLayout);
	return PRIMARYLANGID(nLanguageIdentifier);
}

int Mint::GetSubLanguageIdentifier(void) const
{
	HKL hKeyboardLayout = GetKeyboardLayout(0);
	WORD nLanguageIdentifier = LOWORD(hKeyboardLayout);
	return SUBLANGID(nLanguageIdentifier);
}

const char* Mint::GetLanguageIndicatorString(void) const
{
	/*
	static char* szIndicator[] = {
	"EN", "KO", "JP", "CH", "?",
	};
	*/
	static char* szIndicator[] = {
		"EN", "한", "궇", "中", "?",
	};
	switch(GetPrimaryLanguageIdentifier()){
	case LANG_ENGLISH:
		return szIndicator[0];
	case LANG_KOREAN:
		return szIndicator[1];
	case LANG_JAPANESE:
		return szIndicator[2];
	case LANG_CHINESE:
		return szIndicator[3];
	default:
		return szIndicator[4];
	}
}

bool Mint::IsNativeIME(void) const
{
	HIMC hImc = ImmGetContext(GetInstance()->GetHWND());
	if(hImc==NULL) return false;

	bool bNative = false;

	if(GetPrimaryLanguageIdentifier()==LANG_JAPANESE){
		// 일본어인경우 ConversionMode가 아닌 OpenSatus로 입력기가 활성화 되었는지 알 수 있다.
		// 일본어 IME 이상함!
		bNative = (ImmGetOpenStatus(hImc)==TRUE);
	}
	else{
		// 영문(IME_CMODE_ALPHANUMERIC) 또는 네이티브(IME_CMODE_NATIVE) 언어인지를 판별
		DWORD dwConvMode, dwSentMode;
		ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);

		bNative = (dwConvMode&IME_CMODE_NATIVE);
	}

	ImmReleaseContext(GetInstance()->GetHWND(), hImc);

	return bNative;
}

void Mint::OpenCandidateList(void)
{
	// Candidate List 얻어내기
	HIMC hImc = ImmGetContext(GetInstance()->GetHWND());
	if(hImc==NULL) return;

	m_nCandidateListSize = ImmGetCandidateList(hImc, 0, NULL, 0);

	if(m_pCandidateList!=NULL) delete[] m_pCandidateList;
	m_pCandidateList = new unsigned char[m_nCandidateListSize];

	if(m_nCandidateListSize>0) { 
		ImmGetCandidateList(hImc, 0, (LPCANDIDATELIST)m_pCandidateList, m_nCandidateListSize);
	}

	ImmReleaseContext(GetInstance()->GetHWND(), hImc);
}

void Mint::CloseCandidateList(void)
{
	// Candidate List 닫힘
	if(m_pCandidateList!=NULL) delete[] m_pCandidateList;
	m_pCandidateList = NULL;
	m_nCandidateListSize = 0;
}

const char* Mint::GetCandidate(int nIndex) const
{
	if(m_pCandidateList==NULL) return NULL;

	CANDIDATELIST* pCandidateList = (CANDIDATELIST*)m_pCandidateList;

	if(nIndex>=(int)pCandidateList->dwCount) return NULL;

	char* pCandidate = (char*)((BYTE*)pCandidateList+pCandidateList->dwOffset[nIndex]);
	return pCandidate;
}

int Mint::GetCandidateCount(void) const
{
	if(m_pCandidateList==NULL) return 0;

	CANDIDATELIST* pCandidateList = (CANDIDATELIST*)m_pCandidateList;

	return pCandidateList->dwCount;
}

int Mint::GetCandidateSelection(void) const
{
	if(m_pCandidateList==NULL) return 0;

	CANDIDATELIST* pCandidateList = (CANDIDATELIST*)m_pCandidateList;

	return pCandidateList->dwSelection;
}

int Mint::GetCandidatePageStart(void) const
{
	if(m_pCandidateList==NULL) return 0;

	// GetCandidatePageStart(); 가 일본어에서 버그가 있으므로, 수동으로 계산
	int nStart = GetCandidatePageSize() * (GetCandidateSelection()/GetCandidatePageSize());

	return nStart;

	/*
	// 일본어를 제외하고 작동되는 원래 코드
	if(m_pCandidateList==NULL) return 0;

	CANDIDATELIST* pCandidateList = (CANDIDATELIST*)m_pCandidateList;

	return pCandidateList->dwPageStart;
	*/
}

int Mint::GetCandidatePageSize(void) const
{
	if(m_pCandidateList==NULL) return 0;

	CANDIDATELIST* pCandidateList = (CANDIDATELIST*)m_pCandidateList;

	return pCandidateList->dwPageSize;
}

void Mint::SetCandidateListPosition(MPOINT& p, int nWidgetHeight)
{
	MPOINT cp = p;

	// 가로 영역 체크
	if((cp.x+GetCandidateListWidth())>=MGetWorkspaceWidth()){
		cp.x = MGetWorkspaceWidth()-GetCandidateListWidth();
	}
	else{
//		cp.x -= 4;
	}
	// 세로 영역 체크
	if((cp.y+GetCandidateListHeight()+nWidgetHeight+8)>=MGetWorkspaceHeight()){
		cp.y -= GetCandidateListHeight() + 6;
	}
	else{
		cp.y += (nWidgetHeight+6);
	}

	m_CandidateListPos = cp;
}

int Mint::GetCandidateListWidth(void)
{
	int w = 60;
	if(GetCandidateCount()>0){
		const char* szCandidate = GetCandidate(0);
		w = max(w, MFontManager::Get( "Default")->GetWidth(szCandidate)+100);	// 다른 문자열의 너비가 더 클 수 있으므로 여유값을 충분히 준다.
	}
	return w + 4;
}

int Mint::GetCandidateListHeight(void)
{
	return (MFontManager::Get( "Default")->GetHeight()*(GetCandidatePageSize()+1) + 6);
}

// 멀티라인 지원을 위해 글자 단위 출력
int Mint::DrawCompositionAttribute(MDrawContext* pDC, MPOINT& p, const char* szComposition, int i)
{
	if(i>=(int)strlen(szComposition)) return 0;
	//if(pMint->GetPrimaryLanguageIdentifier()!=LANG_JAPANESE) return;

	const BYTE* pCompAttr = GetCompositionAttributes();
	DWORD nCompAttrSize = GetCompositionAttributeSize();

	if(i>=(int)nCompAttrSize) return 0;	// Composition Attribute 범위를 벗어나는 경우

	MFont* pFont = pDC->GetFont();
	int nFontHeight = pFont->GetHeight();
	MCOLOR c = pDC->GetColor();

	int nCharSize = 1;
	bool bTwoByteChar = IsHangul(szComposition[i]);
	if(bTwoByteChar) nCharSize = 2;
	int nWidth = pFont->GetWidth(&(szComposition[i]), nCharSize);

	if(pCompAttr[i]==ATTR_TARGET_CONVERTED)				// 변환될 내용
		pDC->SetColor(MCOLOR(255, 0, 0, 128));
	else if(pCompAttr[i]==ATTR_TARGET_NOTCONVERTED)		// 변환되지 않는 내용
		pDC->SetColor(MCOLOR(0, 196, 0, 128));
	else												// 변환 가능 절
		pDC->SetColor(MCOLOR(128, 128, 128, 128));

	pDC->FillRectangle(p.x, p.y, nWidth, nFontHeight);

	pDC->SetColor(c);

	return nWidth;
}

void Mint::DrawCompositionAttributes(MDrawContext* pDC, MPOINT& p, const char* szComposition)
{
	if(strlen(szComposition)==0) return;

	int nWidth = 0;

	for(int i=0; szComposition[i]!=NULL; i++){
		bool bTwoByteChar = IsHangul(szComposition[i]);

		nWidth += DrawCompositionAttribute(pDC, MPOINT(p.x+nWidth, p.y), szComposition, i);

		if(bTwoByteChar) i++;
	}
}

void Mint::DrawIndicator(MDrawContext* pDC, MRECT& r)
{
	MFont* pFont = pDC->GetFont();
	int nFontHeight = pFont->GetHeight();

	// 해당 언어에 따라 표기
	const char* szLanguageIndicator = GetLanguageIndicatorString();

	int nIdicatorWidth = pFont->GetWidth(szLanguageIndicator);
	MCOLOR c = pDC->GetColor();

	MRECT fr(r.x+r.w-nIdicatorWidth-4, r.y+2, nIdicatorWidth+3, r.h-4);

	if(IsNativeIME()==true)
	{
		pDC->SetColor(MCOLOR(130,130,130));
		pDC->FillRectangle(fr);
		pDC->SetColor(MCOLOR(0, 0, 0));
//		if (pDC->BeginFont())
		{
			pDC->Text(r.x+r.w-nIdicatorWidth-2, r.y + (r.h-nFontHeight)/2, szLanguageIndicator);
//			pDC->EndFont();
		}
	}
	else{
//		pDC->SetColor(MCOLOR(0, 0, 0));
//		pDC->FillRectangle(fr);
//		pDC->SetColor( MColorTable::NORMAL );
	}

//	pDC->Text(r.x+r.w-nIdicatorWidth-2, r.y + (r.h-nFontHeight)/2, szLanguageIndicator);
	pDC->SetColor(c);
}


void Mint::Clear()
{
	// TODO: 리소스 삭제


	MWidget* pMainFrame = GetMainFrame();
	if (pMainFrame)
	{
		// 자식 위젯 모두 삭제
		pMainFrame->DeleteChildren();
	}


	


	m_pWidgetManager->Clear();
}

void Mint::TransformScreenSizeRatioToStandardSize(int &nx, int &ny, int &nWidth, int &nHeight)
{
	nWidth = (int)(nWidth * m_fRatioToStandardScreenWidth);
	nHeight = (int)(nHeight * m_fRatioToStandardScreenHeight);

	nx = (int)(nx * m_fRatioToStandardScreenWidth);
	ny = (int)(ny * m_fRatioToStandardScreenHeight);
}

void Mint::DrawDesignerMode( MDrawContext* pDC )
{
	pDC->SetColor(0xFF, 0xFF, 0xFF, 168);

//	pDC->BeginFont();
	pDC->SetFont(MFontManager::Get(NULL));
	pDC->Text(0, 0, "Designer Mode");
//	pDC->EndFont();

	// Grid
	pDC->SetColor(255, 255, 255, 32);
	for(int x=0; x<MGetWorkspaceWidth(); x+=MGetDesignerModeSnapSize()){
		pDC->Line(x, 0, x, MGetWorkspaceHeight()-1);
	}
	for(int y=0; y<MGetWorkspaceHeight(); y+=MGetDesignerModeSnapSize()){
		pDC->Line(0, y, MGetWorkspaceWidth()-1, y);
	}

	// Cursor
	MPOINT p = MEvent::GetMouseClientPos();
	pDC->SetColor(0,0,0,255);
	pDC->Line(0, p.y, MGetWorkspaceWidth()-1, p.y);
	pDC->Line(p.x, 0, p.x, MGetWorkspaceHeight()-1);
	pDC->SetColor(255,255,255,255);
	pDC->Line(0, p.y-1, MGetWorkspaceWidth()-1, p.y-1);
	pDC->Line(p.x-1, 0, p.x-1, MGetWorkspaceHeight()-1);

//	pDC->BeginFont();
	char szPos[64];
	sprintf(szPos, "(%d, %d)", p.x, p.y);
	pDC->SetColor(0,0,0,255);
	pDC->Text(p.x, p.y-pDC->GetFont()->GetHeight(), szPos);
	pDC->SetColor(255,255,255,255);
	pDC->Text(p.x-1, p.y-pDC->GetFont()->GetHeight()-1, szPos);
//	pDC->EndFont();
}

MCursor* Mint::NewCursor(const char* pszAliasName)
{
	return new MDefaultCursor(pszAliasName);
}


// Sample Widgets


TCHAR* szStar[] = {
	_T("계절이 지나가는 하늘에는"),
	_T("가을로 가득 차 있습니다."),
	_T("나는 아무 걱정도 없이"),
	_T("가을 속의 별들을 다 헤일 듯합니다. "),
	_T("가슴 속에 하나 둘 새겨지는 별을  "),
	_T("이제 다 못헤는 것은"),
	_T("쉬이 아침이 오는 까닭이오, "),
	_T("내일밤이 남은 까닭이오,"),
	_T("아직 나의 청춘이 다하지 않은 까닭입니다. "),
	_T("별 하나에 추억과"),
	_T("별 하나에 사랑과"),
	_T("별 하나에  쓸쓸함과"),
	_T("별 하나에 동경과"),
	_T("별 하나에 시와"),
	_T("별 하 나에 어머니, 어머니"),
	_T("어머니, 나는 별 하나에 아름 다운 말 한 마디씩 불러 봅니다. 소학교 때 책상을 "),
	_T("같이했던 아이들의 이름과, 패, 경, 옥, 이런 이국 소녀들의 이름과,"),
	_T("벌써 아기 어머니 된 계집애들의 이름과, 가난한 이웃 사람들의 이름과,"),
	_T("비둘기, 강아지, 토끼, 노새,  노루, 프랑시스 잼, 라이너 마리아 릴케,"),
	_T("이런 시인의 이름을 불러 봅니다."),
	_T("이네들은 너무나 멀리 있습니다."),
	_T("별이 아스라이 멀 듯이,"),
	_T("어머님,"),
	_T("그리고 당신은 멀리 북간도에 계십니다."),
	_T("나는 무엇인지 그리워"),
	_T("이 많은 별빛이 내린 언덕 위에"),
	_T("내 이름자를 써 보고,"),
	_T("흙으로 덮어 버리었습니다."),
	_T("딴은, 밤을 새워 우는 벌레는"),
	_T("부끄러운 이름을 슬퍼하는 까닭입니다."),
	_T("그러나 겨울이 지나고 나의 별에도 봄이 오면,"),
	_T("무엄 위에 파란 잔디가 피어나듯이"),
	_T("내 이름자 묻힌 언덕 위에도,"),
	_T("자랑처럼 풀이 무성할 거외다. "),
};


void MCreateSample(void)
{
	MWidget* pMainFrame = Mint::GetInstance()->GetMainFrame();
	MWidget* pNew = new MFileDialog("*.*", pMainFrame, pMainFrame);
	//pNew->SetBounds(10, 10, 300, 300);
	pNew->Show(true);

	class MFrameBitmap : public MFrame{
		int			m_nBitmap;
	protected:
		virtual void OnDraw(MDrawContext* pDC){
			MFrame::OnDraw(pDC);

			MRECT r = GetClientRect();

			if(MBitmapManager::GetCount()==0) return;
			m_nBitmap %= MBitmapManager::GetCount();
			pDC->SetBitmap(MBitmapManager::Get(m_nBitmap));
			m_nBitmap++;
			pDC->Draw(r.x, r.y);
			//pDC->Draw(r);
		}
	public:
		MFrameBitmap(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL)
			: MFrame(szName, pParent, pListener){
			m_nBitmap = 0;
		}
		virtual ~MFrameBitmap(void){
		}
	};

	MWidget* pNewFrame = new MFrameBitmap("Bitmap Test", pMainFrame, pMainFrame);
	pNewFrame->SetBounds(0, 20, 200, 200);
	pNewFrame->Show(true);

	class MFrameInfo : public MFrame{
		int m_nRenderCount;
		DWORD m_nPrevTime;
		int	m_nFPS;
	protected:
		virtual void OnDraw(MDrawContext* pDC){
			MFrame::OnDraw(pDC);

			MRECT r = GetClientRect();
			char temp[256];

			pDC->SetColor(MCOLOR(255,255,255));

			// FPS
			DWORD nCurrTime = timeGetTime();
			if(nCurrTime-m_nPrevTime>1000){
				m_nFPS = m_nRenderCount;
				m_nPrevTime = nCurrTime;
				m_nRenderCount = 0;
			}
			m_nRenderCount++;

//			if (pDC->BeginFont())
			{
				sprintf(temp, "FPS = %d", m_nFPS);
				pDC->Text(r.x, r.y, temp);

				// Cursor Pos
				/*
				POINT p;
				GetCursorPos(&p);
				ScreenToClient(m_hWnd, &p);
				*/
				MPOINT p = MEvent::GetMousePos();
				sprintf(temp, "Cursor Pos = %d, %d", p.x, p.y);
				pDC->Text(r.x, r.y+GetFont()->GetHeight(), temp);
// 				pDC->EndFont();
			}

		}
	public:
		MFrameInfo(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL)
			: MFrame(szName, pParent, pListener){
			m_nRenderCount = 0;
			m_nPrevTime = timeGetTime();
			m_nFPS = 0;
		}
		virtual ~MFrameInfo(void){
		}
	};

	MWidget* pNewCursorInfo = new MFrameInfo("정보", pMainFrame, pMainFrame);
	pNewCursorInfo->SetBounds(400, 0, 200, 200);
	pNewCursorInfo->Show(true);

	class MFrameStar : public MFrame{
	protected:
		virtual void OnDraw(MDrawContext* pDC){
			MFrame::OnDraw(pDC);

			MRECT r = GetClientRect();
			int nRawSize = sizeof(szStar)/sizeof(char*);
			static int nRaw = 0;
			pDC->SetColor(MCOLOR(128,128,255));
			for(int i=0; i<20; i++){
				pDC->Text(r.x, r.y+i*(GetFont()->GetHeight()+2), szStar[(nRaw+i)%nRawSize]);
			}
			nRaw++;
			nRaw%=nRawSize;
		}
		virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL){
			/*
			if(pWidget->GetID()==MGetResourceID("ID_OK") && IsMsg(szMessage, MBTN_CLK_MSG)==true){
				Hide();
			}
			*/
			return false;
		}
	public:
		MFrameStar(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL)
			: MFrame(szName, pParent, pListener){
		}
		virtual ~MFrameStar(void){
		}
	};

	MWidget* pNewStar = new MFrameStar("별 헤는 밤", pMainFrame, pMainFrame);
	pNewStar->SetBounds(30, 250, 500, 200);
	pNewStar->Show(true);

	MPopupMenu* pNewMenu = new MPopupMenu("SampleMenu", pMainFrame, pMainFrame, MPMT_HORIZONTAL);
	MMenuItem* pFile = pNewMenu->AddMenuItem("File");
	MMenuItem* pWidget = pNewMenu->AddMenuItem("Widget");
	MPopupMenu* pFileMenu = pFile->CreateSubMenu();
	MPopupMenu* pWidgetMenu = pWidget->CreateSubMenu();
	pFileMenu->AddMenuItem("Open");
	pFileMenu->AddMenuItem("Exit");
	pWidgetMenu->AddMenuItem("Info");
	pWidgetMenu->AddMenuItem("별헤는밤");

	pNewMenu->SetBounds(0, 0, MGetWorkspaceWidth(), 16);
	pNewMenu->Show(0, 0, true);
	pNewMenu->Show(true);
}

void MDestroySample(void)
{
}

//////////////////////////////////////



void MSetDesignerMode(bool bEnable)
{
	g_bDesignerMode = bEnable;
	if(g_pBoundsInfo!=NULL) 
		g_pBoundsInfo->Show(bEnable);

	if(bEnable==false)
	{
		if (Mint::GetInstance()->GetMainFrame())
			Mint::GetInstance()->GetMainFrame()->HideDesignerMode();
	}
}

bool MIsDesignerMode(void)
{
	return g_bDesignerMode;
}

int MGetDesignerModeSnapSize(void)
{
	return g_nSnapSize;
}

void MSetDesignerModeSnapSize(int s)
{
	g_nSnapSize = s;
}

#define DEFAULT_MRC_FILENAME	"Mint.mrc"
struct MRCHEADER{
	int	nID;
	int	nBMCount;
};
#define MRC_IDENTIFIER	0x123004

struct BMITEM{
	char	szName[256];
	bool	bBounds;	// Bounds가 유효한가?
	MRECT	Bounds;
	int		nChildCount;
};
struct STRING2ID{
	char	szResourceName[256];
	int		nID;
};
bool MSaveDesignerMode(const char* szFileName, MResourceMap* pRM)
{
	//char szName[256] = DEFAULT_MRC_FILENAME;
	//if(szFileName!=NULL) strcpy(szName, szFileName);

	//MResourceMap* pDefBM = pRM;
	//if(pDefBM==NULL) pDefBM = &g_ResourceMap;

	//FILE* fp = fopen(szName, "wb");
	//if(fp==NULL) return false;

	//MRCHEADER bh;
	//bh.nID = MRC_IDENTIFIER;
	//bh.nBMCount = g_ResourceMap.size();
	//fwrite(&bh, sizeof(bh), 1, fp);

	//// 위젯별 리소스 저장
	//for(MResourceMap::iterator i=pDefBM->begin(); i!=pDefBM->end(); i++){
	//	string s = (*i).first;
	//	MWIDGETRESOURCE* pWR = (*i).second;

	//	BMITEM b;
	//	strcpy(b.szName, s.c_str());
	//	b.bBounds = pWR->bBounds;
	//	memcpy(&(b.Bounds), &(pWR->Bounds), sizeof(MRECT));
	//	b.nChildCount = pWR->Children.size();
	//	fwrite(&b, sizeof(BMITEM), 1, fp);

	//	for(MWIDGETRESOURCE::MWIDGETINFOLIST::iterator ii=pWR->Children.begin(); ii!=pWR->Children.end(); ii++){
	//		MWIDGETINFO* pWI = *ii;
	//		fwrite(pWI, sizeof(MWIDGETINFO), 1, fp);
	//	}
	//}

	//// 리소스 아이디 저장
	//int nReouceIDCount = g_ResourceIDMap.size();
	//fwrite(&nReouceIDCount, sizeof(nReouceIDCount), 1, fp);
	//for(MStringIDMap::iterator it=g_ResourceIDMap.begin(); it!=g_ResourceIDMap.end(); it++){
	//	STRING2ID si;
	//	strcpy(si.szResourceName, (*it).first.c_str());
	//	si.nID = (*it).second;
	//	fwrite(&si, sizeof(si), 1, fp);
	//}

	//fclose(fp);

	return true;
}

bool MLoadDesignerMode(const char* szFileName, MResourceMap* pRM)
{
	//char szName[256] = DEFAULT_MRC_FILENAME;
	//if(szFileName!=NULL) strcpy(szName, szFileName);

	//MResourceMap* pDefBM = pRM;
	//if(pDefBM==NULL) pDefBM = &g_ResourceMap;

	//pDefBM->clear();
	//g_ResourceIDMap.clear();

	//FILE* fp = fopen(szName, "rb");
	//if(fp==NULL) return false;

	//MRCHEADER bh;
	//fread(&bh, sizeof(bh), 1, fp);
	//if(bh.nID!=MRC_IDENTIFIER) return false;

	//for(int i=0; i<bh.nBMCount; i++){
	//	BMITEM b;
	//	fread(&b, sizeof(BMITEM), 1, fp);
	//	MWIDGETRESOURCE* pWR = new MWIDGETRESOURCE;
	//	(*pDefBM)[b.szName] = pWR;
	//	pWR->Bounds = b.Bounds;
	//	for(int ii=0; ii<b.nChildCount; ii++){
	//		MWIDGETINFO* pWI = new MWIDGETINFO;
	//		fread(pWI, sizeof(MWIDGETINFO), 1, fp);
	//		pWR->Children.insert(pWR->Children.end(), pWI);
	//	}
	//}

	//// 리소스 아이디 읽기
	//int nReouceIDCount = 0;
	//fread(&nReouceIDCount, sizeof(nReouceIDCount), 1, fp);
	//for(int it=0; it<nReouceIDCount; it++){
	//	STRING2ID si;
	//	fread(&si, sizeof(si), 1, fp);
	//	g_ResourceIDMap[si.szResourceName] = si.nID;
	//}

	//fclose(fp);

	return true;
}

void MResetDesignerMode(MResourceMap* pRM)
{
	//MResourceMap* pDefBM = pRM;
	//if(pDefBM==NULL) pDefBM = &g_ResourceMap;

	//g_ResourceMap.clear();
}

void MUpdateDesignerMode(void)
{
	//g_pMainFrame->SetByResourceMapRecursively(&g_ResourceMap);
}

void MShowDesignerMenu(MPOINT& p, MWidget* pCaller)
{
//	g_pMainFrame->ShowDesignerMenu(p, pCaller);
}


/*
TODO: Edit 셀렉션 지원
TODO: Text Area IME 지원
*/

} // namespace mint3