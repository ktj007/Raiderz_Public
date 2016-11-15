/// @file VL_ToolBar.h
#pragma once

#include "afxpanedialog.h"
#include "VL_ListenableView.h"
#include "ext_lib/FooButton.h"
#include "ext_lib/InitDialogBar.h"
#include "ext_lib/XInfoTip.h"

#include "resource.h"

const UINT MOVEAXIS_X = 1;
const UINT MOVEAXIS_Y = 2;
const UINT MOVEAXIS_Z = 3;
const UINT MOVEAXIS_XY = 4;
const UINT MOVEAXIS_YZ = 5;
const UINT MOVEAXIS_XZ = 6;

/**
	@brief 툴바

	자주 사용되는 기능을 모아둔 툴바 입니다. \n
	Self-Drawing Button 기능을 이용하기 위해 CInitDialogBar 를 상속받습니다.
*/
class CORETOOL_API CCoreToolToolBar : public CPaneDialog, public CListenableViewImpl_Fixed<CCoreToolToolBar>
{
	DECLARE_DYNAMIC(CCoreToolToolBar)

public:

							CCoreToolToolBar(CWnd* pParent = NULL);		///< 표준 생성자입니다.
	virtual					~CCoreToolToolBar();						///< 소멸자

	// Listenable View
	virtual void			Listen(CoreMessage& _message);			///< Listen
	virtual const char*		GetViewName(){ return "Toolbar"; }			///< View Name

	//편집축 타입
	enum TOOLBAR_AXIS
	{
		TB_AXIS_X = 0, TB_AXIS_Y, TB_AXIS_Z,
		TB_AXIS_COMBO, TB_AXIS_BTN_NUM,

		//combo 축을 위한 enum
		TB_AXIS_XY, TB_AXIS_YZ, TB_AXIS_XZ
	};

	AXIS_STATE GetCurrentAxisToAXIS_STATE();
	TOOLBAR_AXIS GetCurrentAxis();
	void SetAxis(TOOLBAR_AXIS _type = TB_AXIS_BTN_NUM);
	void SetEnableAxis(bool bEnable);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VL_TOOL_BAR };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    ///< DDX/DDV 지원입니다.
	virtual void OnUpdateCmdUI(CCmdUI* pCmdUI); ///< 툴바의 컨트롤을 활성화 하기위한 함수
	virtual BOOL OnInitDialog(); ///< Self-Drawing Button 기능을 이용하기 위해 생성시에 호출하는 메소드
	virtual BOOL PreTranslateMessage(MSG* pMsg); ///< 툴팁을 위한 메시지 처리

	DECLARE_MESSAGE_MAP()

public:
	FooButton m_btnOpen;
	FooButton m_btnNew;
	FooButton m_btnSave;
	FooButton m_btnUndo;

	FooButton m_btnGroup[WORKBENCH_STATE::STATE_NUM]; ///< CCoreToolToolBar 의 첫번째 버튼그룹

	FooButton m_btnLocalAxis;
	FooButton m_btnObjectLock;
	FooButton m_btnOnTerrain;

	FooButton m_btnObjectList;
	FooButton m_btnLayerManager;
	FooButton m_btnWeather;
	FooButton m_btnClip;

	// rotation info
	static const char* ROTATION_DRAG_INFO_ID;

private:
	//Axis for WB Edition
	FooButton m_btnGroupAxis_[TB_AXIS_BTN_NUM];
	TOOLBAR_AXIS m_eMoveAxis_;
	TOOLBAR_AXIS m_eRotateAxis_;
	TOOLBAR_AXIS m_eCurrentAxis_;
	void SetAxisButton_(TOOLBAR_AXIS _type);		///< button ui 변경
	void UpdateButtonAxisXyz(TOOLBAR_AXIS axis);	///< 선택 스테이트 및 ui 변경

	CXInfoTip m_ctlToolTip_;
	CFont m_tooltipFont_;

	//버튼클릭 관련 메시지맵 함수
	afx_msg void OnBnClickedTopFileNew();
	afx_msg void OnBnClickedTopFileOpen();
	afx_msg void OnBnClickedTopFileSave();
	afx_msg void OnBnClickedTopCmdUndo();

	afx_msg void OnBnClickedTopHeigh();
	afx_msg void OnBnClickedTopAver();
	afx_msg void OnBnClickedTopFlat();
	afx_msg void OnBnClickedTopSplat();
	afx_msg void OnBnClickedTopWeather();
	afx_msg void OnBnClickedTopObjNew();
	afx_msg void OnBnClickedTopSelect();
	afx_msg void OnBnClickedTopMove();
	afx_msg void OnBnClickedTopRotate();
	afx_msg void OnBnClickedTopScale();
	afx_msg void OnBnClickedTopGrass();

	afx_msg void OnBnClickedAxisX();
	afx_msg void OnBnClickedAxisY();
	afx_msg void OnBnClickedAxisZ();
	afx_msg void OnBnClickedAxisXyz();
	afx_msg void OnBnClickedAxisLocal();
	afx_msg void OnBnClickedTopRelax();
	afx_msg void OnBnClickedTopWater();

	afx_msg void OnBnClickedTopPathFind();
	afx_msg void OnBnClickedTopEtcMapView();
	afx_msg void OnBnClickedTopEtcCamera();
	afx_msg void OnBnClickedTopEtcDev();
	afx_msg void OnBnClickedTopEtcDecal();

public:
	afx_msg void OnBnClickedTopObjOnTerrain();
	afx_msg void OnBnClickedTopLock();
	afx_msg void OnBnClickedTopObjectList();
	afx_msg void OnBnClickedTopLayerManager();
	afx_msg void OnBnClickedTopClip();
	afx_msg void OnDestroy();
};

//////////////////////////////////////////////////////////////////////////
inline 	CCoreToolToolBar::TOOLBAR_AXIS CCoreToolToolBar::GetCurrentAxis() { return m_eCurrentAxis_; }
