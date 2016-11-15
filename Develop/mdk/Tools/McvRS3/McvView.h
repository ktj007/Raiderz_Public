#include "afxwin.h"
#if !defined(AFX_MCVVIEW_H__F6EB1E45_E7A7_4458_BE9F_B50A9E68AAA9__INCLUDED_)
#define AFX_MCVVIEW_H__F6EB1E45_E7A7_4458_BE9F_B50A9E68AAA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxcmn.h"
#include "MDebug.h"
#include "ControlToolBar.h"
#include "AnimationSidebar.h"
#include "ModelInfoSidebar.h"
#include "RAlignController.h"
#include "AniBlendDlg.h"
#include "ProjectType.h"
#include "Main.h"
#include "MaterialDecalDlg.h"

class CMcvMain;

CMcvView* GetMainView();
CMcvMain* GetMain();
RDevice*  GetDevice();

namespace rs3
{
	class RAnimationBlendTable;
}

class CMcvView : public CFormView
{
	friend class CAniBlendDlg;
protected: 
	CMcvView();
	DECLARE_DYNCREATE(CMcvView)

public:
	//{{AFX_DATA(CMcvView)
	enum { IDD = IDD_MCV_FORM };


//	CListBox&	m_AnimationListBox;
	//CSliderCtrl m_SliderAniSpeed;
	//CString		m_SelMeshNodeName;
	//CTreeCtrl	m_treeNodes;
	//BOOL m_bNodeMesh;
	//BOOL m_bNodeBone;
	//BOOL m_bNodeDummy;
	//CString m_strNodeInfo;
	//CStatic m_strFrame;
	//CButton m_btnAlignTest;

//	CEdit m_editLog;

	//}}AFX_DATA

public:

	CMcvDoc*	GetDocument();

private:

	// dlg

	CScaleDlg		m_ScaleDlg;
	CMtrlEditDlg	m_MtrlDlg;
	CModelInfoDlg	m_ModelInfoDlg;
	CRCFrameInfoDlg	m_FrameInfoDlg;
	CAniBlendDlg	m_AniBlendDlg;
	MaterialDecalDlg m_MaterialDecalDlg;

//	CControlToolBar m_controlToolBar;

	bool			m_bScaleDlg;
	bool			m_bMtrlEditDlg;
	bool			m_bFrameInfoDlg;
	bool			m_bModelInfoDlg;

	//	option

	bool			m_bTextureRender;
	bool			m_bVertexNormal;
	bool			m_bDrawGrid;
	bool			m_bDrawBBox;
	bool			m_bEffectModel;
	bool			m_bCharacterView;
	bool			m_bAlpha2Pass;

	// file open

	string			m_LastOpenFile;

	CPoint			m_oldCursorPos;
	CPoint			m_LButtonClickPos;

	//AnimationEventListener	m_eventListener;
//	SoulHuntAnimationEventListener m_eventListener;

	// controller test
	RAlignController m_testController;

	// Animation
	int				m_nPrevFrame;				// 이전 프레임

	// External xml and animation list
	map< string, string>	m_ExternXmlFile;	// Path Name, Filter String
	map< string, string>	m_ExternAni;		// Animation Name, Xml File Name

	// Sound
	map< string, string>	m_SoundList;		// Sound Name, File Name

	// AnimationBlendTable
//	RAnimationBlendTable* m_AnimationBlendTable;

	// Project type
	ProjectType		m_ProjectType;

	RSceneNode*	m_pCurrentSceneNode;	// 이펙툴을 따라해본다.이펙툴은 아니므로걍 씬 노드 -090602, OZ

public:

	bool Init();
	void Idle();

	void Resize(int x, int y);

	static void McvLog(const char* szText, MLogLevel nLevel);


	// dlg

	void ToggleFrameInfoDlg();
	void ToggleMrtlEditDlg();
	void ToggleScaleDlg();
	void ToggleModelInfoDlg();

	// option

	void GridOnOff();
	
	bool isDrawGrid()			{ return m_bDrawGrid;}
	bool isEffectModel()		{ return m_bEffectModel;}
	bool isDrawBBox()			{ return m_bDrawBBox;}
	bool isCharacterView()		{ return m_bCharacterView;}
	bool isVertexNormal()		{ return m_bVertexNormal; }
	bool isAlpha2PassRendering(){ return m_bAlpha2Pass; }

	// interface

//	RMeshMgr*		GetMeshMgr()		{ return &m_pMcvMain->m_MeshMgr; }
//	RMeshMgr*		GetWeaponMeshMgr()	{ return &m_pMcvMain->m_WeaponMeshMgr; }
//	RActorMgr*		GetActorMgr()		{ return &m_pMcvMain->m_ActorMgr; }
	RMeshAnimationMgr*	GetAnimationMgr()	{ return &GetActor()->m_pMesh->m_AniMgr; }
	RActor*				GetActor()			{ return GetMain()->m_pActor; }
	CDialog*			GetAniBlendDlg()	{ return &m_AniBlendDlg; }

	// file open

	bool IsActorCreated();
	void ClearActor();

	void FileOpen(char* FileName);

	void LoadFile(char* FileName);
	void LoadAniFile(char* FileName);
//	void LoadXmlFile(char* FileName);
	void LoadSound( const char* szFileName);
	bool LoadXmlFile(char* FileName);	// 씬 로딩으로 바꾸면서 - 090602, OZ
	bool LoadSceneFile( const char* FileName);	// 씬 로딩으로 바꾸면서 - 090602, OZ
	bool ReadXmlInfo( const char* xmlFileName, bool bExtern =false, const char* szFilter ="");

	bool SaveXmlFile(const char* szFileName);

	// 씬의 모든 액터를 에니메이트 시킨다. - 090605, OZ
	// xml파일에 등록 된 라이트를 따로 컨트롤 안하고 그냥 무작정 돌려돌려~ 
	bool AnimateAllActor( const RSceneNodeList& nodes);	

//	void WFileOpen(char* FileName);

//	void LoadWeaponFile(char* FileName);
//	void LoadWeaponXmlFile(char* FileName);

	const char* GetLastOpenFile();

	// 모델의 에러 체크
	void ValidateModel();

	void SelectNode(RActorNode* pActorNode);
	void Unselect();

	// node tree 와 정보 창
	void UpdateNodes();
	void AddInfo(const char *pFormat, ...);
	void UpdateNodeInfo();

	void ChangeWeaponListBox();

	// animation list box
	void ChangeAnimationListBox();
	void ChangeAnimationInfoString();
	void DeleteSelectedAnimation();

	void PlayAnimation();
	void PauseAnimation();

	void SetFrame( int nFrame);

	ProjectType* GetProjectType()			{ return &m_ProjectType;	}


	//{{AFX_VIRTUAL(CMcvView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

private:
	bool IsCursorInView();
	void UpdateAnimationSlider();
//	bool EditEvent(RAnimationEvent* pEvent);
//	bool EditEventHandler(AnimationEventHandler *pHandler);

//	void DeleteSelectedEvent();
//	void DeleteSelectedEventHandler();

	bool EditAnimation(RAnimation* pAnimation);

	void ChangeAnimationListSub( RActor* pActor, RAnimation* pAni );
	void ChangeAnimationBlendType( RANIMATIONTRANSITIONTYPE blendType );

public:
	void SelChangeAnimationList();
	void ChangeAnimationList( const char* name );
	void ChangeAnimationList( int index );

public:
	virtual ~CMcvView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

public:
	//{{AFX_MSG(CMcvView)
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileOpen();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTexOnOff();
	afx_msg void OnVertexNormal();
	afx_msg void OnUpdateVertexNormal(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTexOnOff(CCmdUI *pCmdUI);
//	afx_msg void OnCbnSelchangePartsType();
//	afx_msg void OnLbnSelchangePartsList();
//	afx_msg void OnLbnSelchangeWeaponList();
	afx_msg void OnBnClickedPlayPause();
	afx_msg void OnBnClickedStop();
//	afx_msg void OnFileWopen();
	afx_msg void OnCameraReset();
//	afx_msg void OnCbnSelchangeWeaponType();
public:
//	CComboBox m_WeaponType2Combo;
//	afx_msg void OnCbnSelchangeWeaponType2();
	afx_msg void OnNMThemeChangedSliderSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnScaledlg();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMapObjPos();
	afx_msg void OnGridOnoff();
	afx_msg void OnBboxOnoff();
	afx_msg void OnUpdateBboxOnoff(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGridOnoff(CCmdUI *pCmdUI);
	afx_msg void OnEffectRender();
	afx_msg void OnUpdateEffectRender(CCmdUI *pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCharViewMode();
	afx_msg void OnUpdateCharViewMode(CCmdUI *pCmdUI);
//	afx_msg void OnPartscolor();
//	afx_msg void OnMtrleditdlg();
//	afx_msg void OnPartscolorall();
	afx_msg void OnAnimationinfo();
	afx_msg void OnBlendColor();
	afx_msg void OnCartoonLightOnoff();
	afx_msg void OnUpdateCartoonLightOnoff(CCmdUI *pCmdUI);
	afx_msg void OnModelinfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbnSelchangePartsType();
	afx_msg void OnCbnSelchangeComboNodes();
	afx_msg void OnBnClickedButtonReplaceMesh();
//	afx_msg void OnLbnDblclkListEvent();
	afx_msg void OnBnClickedButtonAddEvent();
	afx_msg void OnBnClickedButtonAddHandler();
//	afx_msg void OnLbnDblclkListHandler();
	afx_msg void OnShowBones();
	afx_msg void OnUpdateShowBones(CCmdUI *pCmdUI);
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSave();
//	afx_msg void OnLbnDblclkAnimationList();
	afx_msg void OnTvnSelchangedTreeNodes(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckNode();
	afx_msg void OnBnClickedCheckNodeBone();
	afx_msg void OnBnClickedCheckNodeDummy();
	afx_msg void OnRotationTest();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnShowCollision();
	afx_msg void OnUpdateShowCollision(CCmdUI *pCmdUI);
	afx_msg void OnAnimationBlend();
	afx_msg void OnAniblendImmediate();
	afx_msg void OnAniblendDelayed();
	afx_msg void OnAniblendCrossfade();
	afx_msg void OnAniblendDelete();
	afx_msg void OnAniblendImmediateTargetAll();
	afx_msg void OnAniblendDelayedTargetAll();
	afx_msg void OnAniblendCrossfadeTargetAll();
	afx_msg void OnAniblendDeleteTargetAll();
	afx_msg void OnAniblendEditDuration();
	afx_msg void OnAlpha2pass();
	afx_msg void OnUpdateAlpha2pass(CCmdUI *pCmdUI);
	afx_msg void OnHDROnOff();
	afx_msg void OnUpdateHDROnOff(CCmdUI *pCmdUI);
	afx_msg void OnMaterialdecaldlg();
};

#ifndef _DEBUG  
inline CMcvDoc* CMcvView::GetDocument()
   { return (CMcvDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCVVIEW_H__F6EB1E45_E7A7_4458_BE9F_B50A9E68AAA9__INCLUDED_)
