#pragma once
#include "VL_ListenableView.h"

// CRollupTerrainAverage 대화 상자입니다.
/** @brief Terrain - Average Rollup */
class CORETOOL_API CRollupTerrainAverage : public CDialog, public CListenableViewImpl_Rollup<CRollupTerrainAverage>
{
	DECLARE_DYNAMIC(CRollupTerrainAverage)

public:
						CRollupTerrainAverage(CWnd* pParent = NULL);	///< 표준 생성자입니다.
	virtual				~CRollupTerrainAverage();						///< 소멸자

	// Listenable View
	virtual void		Listen(CoreMessage& _message);				///< Listen
	virtual const char*	GetViewName(){ return "Average Brush"; }			///< View Name


	// Dialog
	virtual BOOL		OnInitDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VR_TERRAIN_AVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnCancel();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

public:
	CString m_cstrViewSize;
	float m_fEditSize;
	CSliderCtrl m_sliderCtrlSize;
	float m_fStr;
	CSliderCtrl m_sliderCtrlStr;
};
