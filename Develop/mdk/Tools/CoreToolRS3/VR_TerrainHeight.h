#pragma once
#include "VL_ListenableView.h"

// CRollupTerrainHeight 대화 상자입니다.
/** @brief Terrain - Height Rollup */
class CORETOOL_API CRollupTerrainHeight : public CDialog, public CListenableViewImpl_Rollup<CRollupTerrainHeight>
{
	DECLARE_DYNAMIC(CRollupTerrainHeight)

public:
							CRollupTerrainHeight(CWnd* pParent = NULL);	///< 표준 생성자입니다.
	virtual					~CRollupTerrainHeight();					///< 소멸자

	// Listenable View
	virtual void			Listen(CoreMessage& _message);			///< Listen
	virtual const char*		GetViewName(){ return "Height Edit"; }		///< View Name

	// Dialog
	virtual BOOL			OnInitDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VR_TERRAIN_HEIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnCancel();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	float m_fEditSize;
	CString m_cstrViewSize;
	CSliderCtrl m_sliderCtrlSize;
	float m_fStr;
	CSliderCtrl m_sliderCtrlStr;
};
