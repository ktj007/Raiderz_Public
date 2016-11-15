#pragma once
#include "afxcmn.h"
#include "afxwin.h"


namespace rs3 
{
	class RToolTerrain;
};
// CRollupTerrainUserMaterial 대화 상자입니다.
/** @brief Terrain - User Material */
class CORETOOL_API CRollupTerrainUserMaterial : public CDialog, public CListenableViewImpl_Rollup<CRollupTerrainUserMaterial>
{
	DECLARE_DYNAMIC(CRollupTerrainUserMaterial)

public:
							CRollupTerrainUserMaterial(CWnd* pParent = NULL);	///< 표준 생성자입니다.
	virtual					~CRollupTerrainUserMaterial();						///< 소멸자

	// Listenable View
	virtual void			Listen(CoreMessage& _message);					///< Listen
	virtual const char*		GetViewName(){ return "User Material"; }			///< View Name

	// Dialog
	virtual BOOL			OnInitDialog();


// 대화 상자 데이터입니다.
	enum { IDD = IDD_VR_TERRAIN_USERMATERIAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	virtual void OnCancel(){}
	virtual void OnOK(){}

	DECLARE_MESSAGE_MAP()
public:
	float m_fEditSize;
	rs3::RToolTerrain *m_pTerrain_;
	
	CSliderCtrl m_sliderCtrlSize;
	CString m_cstrViewSize;
	DWORD m_dwEditID;
	CButton m_btnView;

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckView();
	afx_msg void OnEnChangeEditUsermaterialId();
};
