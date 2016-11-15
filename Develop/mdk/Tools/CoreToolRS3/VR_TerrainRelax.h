#pragma once

#include "VL_ListenableView.h"

// CRollupTerrainRelax 대화 상자입니다.
/** @brief Terrain - Relax Rollup */
class CORETOOL_API CRollupTerrainRelax : public CDialog, public CListenableViewImpl_Rollup<CRollupTerrainRelax>
{
	DECLARE_DYNAMIC(CRollupTerrainRelax)

public:
							CRollupTerrainRelax(CWnd* pParent = NULL);	///< 표준 생성자입니다.
	virtual					~CRollupTerrainRelax();						///< 소멸자

	// Listenable View
	virtual void			Listen(CoreMessage& _message);			///< Listen
	virtual const char*		GetViewName(){ return "Relax"; }			///< View Name

	// Dialog
	virtual BOOL			OnInitDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VR_TERRAIN_RELAX };

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
