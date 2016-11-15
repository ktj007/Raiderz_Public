#pragma once
#include "VL_ListenableView.h"

// CRollupTerrainFlat 대화 상자입니다.
/** @brief Terrain - Flat Rollup */
class CORETOOL_API CRollupTerrainFlat : public CDialog, public CListenableViewImpl_Rollup<CRollupTerrainFlat>
{
	DECLARE_DYNAMIC(CRollupTerrainFlat)

public:
							CRollupTerrainFlat(CWnd* pParent = NULL);	///< 표준 생성자입니다.
	virtual					~CRollupTerrainFlat();						///< 소멸자

	// Listenable View
	virtual void			Listen(CoreMessage& _message);			///< Listen
	virtual const char*		GetViewName(){ return "Flatten"; }			///< View Name

	// Dialog
	virtual BOOL			OnInitDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VR_TERRAIN_FLAT };

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
