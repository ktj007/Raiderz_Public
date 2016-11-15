#pragma once

#include "VL_ListenableView.h"

namespace rs3{ class RToolTerrain; };
// CDlgRelaxAll 대화 상자입니다.
/** @brief Relax All Dialog */
class CDlgRelaxAll : public CDialog, public CListenableViewImpl_Dialog<CDlgRelaxAll>
{
	DECLARE_DYNAMIC(CDlgRelaxAll)

public:
							CDlgRelaxAll(CWnd* pParent = NULL);		///< 표준 생성자입니다.
	virtual					~CDlgRelaxAll();						///< 소멸자

	// Listenable View
	virtual void			Listen(CoreMessage& _message);		///< Listen
	virtual const char*		GetViewName(){ return "Relax All"; }	///< View Name

	// Dialog
	virtual BOOL			OnInitDialog();
	virtual void			OnCancel();
	virtual void			OnOK();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VL_TERRAIN_RELAX_ALL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	int m_nRelaxFactor_;
	rs3::RToolTerrain* m_pTerrain_;
};
