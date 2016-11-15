/// @file MainFrm.h
#pragma once

class CControlMediator;
class CCoreToolRS3View;
class CCoreToolToolBar;
class CCoreToolStatusBar;
class CWorkSpaceManager;
class CRollupCtrl;

#define CFrameWnd CFrameWndEx

/** @brief CoreToolRS3 메인 프레임 */
class CMainFrame : public CFrameWnd
{
protected:
public:
	CMainFrame();
//	DECLARE_DYNAMIC(CMainFrame)
	DECLARE_DYNCREATE(CMainFrame)

// 재정의
public:
	virtual ~CMainFrame();

	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CWnd* pParentWnd = NULL, LPCTSTR lpszMenuName = NULL, DWORD dwExStyle = 0, CCreateContext* pContext = NULL);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:

	CControlMediator*		GetControlMediator(){ return m_pControlMediator_; }	///< Control Mediator Getter
	void					ChangeWorkSpace(const std::string& _rWorkSpaceKeyword);

	/** @brief MainFrame OupBar를 통한 출력 */
	void					OutputText(const char* szText);

	/// Log 초기화
	void InitLog();

	/// MLog 받는곳
	static void MLogOutputCallback(const char* szText, MLogLevel nLevel);

	static string& GetErrorString();
	static void ClearErrorString();

	static string& GetDumpPathString();

	bool	IsActivated()		{ return m_bMainFrameActived_; }

	UINT	GetSnapToGridID( int nSize );
	int		GetSnapToGridSize()	{ return m_nSnapGridSize; }
	int		GetAngleSnapSize()	{ return m_nAngleSnapSize; }
	void	IncreaseSnapToGridSize();
	void	DecreaseSnapToGridSize();

	void	SetSnapToGridSize(int nSnapGridSize);
	void	SetAngleSnapSize(int nAngleSnapSize);

public:
	bool				OnDocViewCreated_();
private:
	int					CreateViews_();
	void				DestroyViews();

	BOOL				OnShowPopupMenu (CMFCPopupMenu* pMenuPopup) override;

	// core tool 로직
	CControlMediator*	m_pControlMediator_;		// Control Meidator
	CWorkSpaceManager*	m_pWorkSpaceManager;		// Shared WorkSpace

	// view
	CCoreToolRS3View*	m_pWorkBenchView_;			// MFC MainView
	CMFCMenuBar			m_wndMenuBar;				// 메뉴 바
	CCoreToolToolBar*	m_pToolBar_;				// TopBar 뷰
	CCoreToolStatusBar*	m_pStatusBar_;				// StatusBar 뷰
	CPaneDialog			m_outputBar;				// output log 창

// 	CBHTabedReBar		m_wndTabedReBar_;			// 사이드 바 바인더
	vector<CPaneDialog*> m_arryWndSidePanels_;		// 사이드 바 패널
	CRollupCtrl*		m_arryWndRollupContainer_;	// 롤업 컨테이너. 사이드바 패널과 갯수가 같다.
	int					m_nTabCount;

	int					m_nSnapGridSize;			///< Snap to Grid ( unit length )
	int					m_nAngleSnapSize;			///< Angle Snap ( degree )

	bool				m_bHDREnable;
	bool				m_bHDRViewRenderTargetEnable;

	// app 관련
	bool				m_bMainFrameActived_;
	static MLogger*		CoreToolRS3MainLogger;

	/// 마지막 에러스트링
	static string		m_errorString;

	/// 덤프 패쓰
	static string		m_dumpPathString;

public:
	// 메시지 맵 함수를 생성했습니다.
	DECLARE_MESSAGE_MAP()

	virtual void OnUpdateCmdUI(CCmdUI* pCmdUI); ///< 메인 프레임의 메뉴 활성화
	afx_msg LRESULT OnRebarExpaned(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSidePanelExpaned(WPARAM wParam, LPARAM lParam);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();

	// 파일 메뉴
	afx_msg void OnFileOpenall(); ///< 파일열기
	afx_msg void OnFileSaveall();
	afx_msg void OnFileSaveAsAll();

	afx_msg void OnObjImport();
	afx_msg void OnObjExport();

	afx_msg void OnToolsOption();
	afx_msg void OnPause();

	afx_msg void OnToolsDataFolderOpen();
	afx_msg void OnToolsShortCut();
	afx_msg void OnToolsMapResMgr();

	afx_msg void OnToolsXmlZone();
	afx_msg void OnToolsXmlField();
	afx_msg void OnToolsXmlNpc();
	afx_msg void OnToolsXmlProperty();

	afx_msg void OnEditUndo();
	afx_msg void OnToolsMapBatchScriptOpen();

	afx_msg void OnImportScene();
	afx_msg void OnSetAutoAnimationProperty();
	afx_msg void OnExportScene();

	afx_msg void OnFileNew0();
	afx_msg void OnFileNew1();
	afx_msg void OnFileNew2();
	afx_msg void OnFileNew3();
	afx_msg void OnFileNew4();
	afx_msg void OnFileNew5();

	// 도구
	afx_msg void OnToolEtc1();
	afx_msg void OnToolEtc2();
	afx_msg void OnToolEtc3();
	afx_msg void OnToolEtc4();
	afx_msg void OnToolEtc5();

	// 보기 메뉴들
	afx_msg void OnWBInfoView();
	afx_msg void OnViewProfileInfo();
	afx_msg void OnViewTerrainInfo();
	afx_msg void OnUpdateViewTerrainInfo(CCmdUI *pCmdUI);
	afx_msg void OnViewAlphaSorting();
	afx_msg void OnUpdateViewAlphaSorting(CCmdUI *pCmdUI);
	afx_msg void OnViewLighting();
	afx_msg void OnUpdateViewLighting(CCmdUI *pCmdUI);
	afx_msg void OnViewStaticRenderCell();
	afx_msg void OnUpdateViewStaticRenderCell(CCmdUI *pCmdUI);
	afx_msg void OnViewStaticRenderAll();
	afx_msg void OnUpdateViewStaticRenderAll(CCmdUI *pCmdUI);
	afx_msg void OnViewHDR();
	afx_msg void OnUpdateViewHDR(CCmdUI *pCmdUI);
	afx_msg void OnViewHDRRenderTarget();
	afx_msg void OnUpdateViewHDRRenderTarget(CCmdUI *pCmdUI);
	afx_msg void OnViewDof();
	afx_msg void OnUpdateViewDof(CCmdUI *pCmdUI);
	afx_msg void OnViewScreenMotionBlur();
	afx_msg void OnUpdateViewScreenMotionBlur(CCmdUI *pCmdUI);
	afx_msg void OnViewClipping();
	afx_msg void OnUpdateViewClipping(CCmdUI *pCmdUI);
	afx_msg void OnViewSsao();
	afx_msg void OnUpdateViewSsao(CCmdUI *pCmdUI);

	afx_msg void OnViewDistortion();
	afx_msg void OnUpdateViewDistortion(CCmdUI *pCmdUI);
	afx_msg void OnViewNormalMap();
	afx_msg void OnUpdateViewNormalMap(CCmdUI *pCmdUI);
	afx_msg void OnViewGammaCorrect();
	afx_msg void OnUpdateGammaCorrect(CCmdUI *pCmdUI);
	afx_msg void OnViewFXAA();
	afx_msg void OnUpdateFXAA(CCmdUI *pCmdUI);
	afx_msg void OnViewShadow();
	afx_msg void OnUpdateViewShadow(CCmdUI *pCmdUI);
	afx_msg void OnSnapToGridSize(UINT nID);
	afx_msg void OnUpdateUISnapToGridSize(CCmdUI *pCmdUI);
	afx_msg void OnAngleSnapSize(UINT nID);
	afx_msg void OnUpdateUIAngleSnapSize(CCmdUI *pCmdUI);
	afx_msg void OnUpdateUIViewPane(CCmdUI *pCmdUI);
	afx_msg void OnViewPane(UINT nID);
	afx_msg void OnSelectNone();

	afx_msg void OnViewDefaultLight();
	afx_msg void OnViewDefaultLightToCamera();
	afx_msg void OnUpdateViewDefaultLight(CCmdUI *pCmdUI);
	afx_msg void OnViewOccluder();
	afx_msg void OnUpdateViewOccluder(CCmdUI *pCmdUI);
	afx_msg void OnViewAnisotropy();
	afx_msg void OnUpdateViewAnisotropy(CCmdUI *pCmdUI);
};
