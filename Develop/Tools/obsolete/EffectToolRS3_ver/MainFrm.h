// MainFrm.h : CMainFrame 클래스의 인터페이스
//


#pragma once

class CSplitterX : public CSplitterWnd
{
public:
	CSplitterX();
	~CSplitterX();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitterX)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CSplitterX)
	afx_msg LRESULT OnNcHitTest(CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CMainFrame : public CFrameWnd
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성
public:

// 작업
public:

// 재정의
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

// 구현
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CSplitterWnd	m_wndSplitter;

// 메시지 맵 함수를 생성했습니다.
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnModifyParticle(WPARAM wParam, LPARAM lParam);

	afx_msg void OnViewStart();
	afx_msg void OnViewStop();
	afx_msg void OnViewPause();
	afx_msg void OnViewBgcolor();
	afx_msg void OnWireFrame();
	afx_msg void OnWireFrameUpdateUI(CCmdUI *pCmdUI);
	afx_msg void OnViewAxis();
	afx_msg void OnViewAxisUpdateUI(CCmdUI *pCmdUI);
	afx_msg void OnViewGrid();
	afx_msg void OnViewGridUpdateUI(CCmdUI *pCmdUI);
	afx_msg void OnViewEmitter();
	afx_msg void OnViewEmitterUpdateUI(CCmdUI *pCmdUI);
	afx_msg void OnViewPath();

	afx_msg void OnNew();
	afx_msg void OnEffectFileOpen();
	afx_msg void OnEffectFileSave();
	afx_msg void OnEffectFileSaveAs();
	afx_msg void OnMeshEffectFileOpen();
	afx_msg void OnCharFileOpen();

	DECLARE_MESSAGE_MAP()
};


