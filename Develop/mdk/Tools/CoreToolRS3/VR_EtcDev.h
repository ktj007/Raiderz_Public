#pragma once

#include "VL_ListenableView.h"
#include "afxwin.h"

// CRollupEtcDev 대화 상자입니다.
/** @brief Development Pilot Rollup */
class CORETOOL_API CRollupEtcDev : public CDialog, public CListenableViewImpl_Rollup<CRollupEtcDev>
{
	DECLARE_DYNAMIC(CRollupEtcDev)

public:
						CRollupEtcDev(CWnd* pParent = NULL);	///< 표준 생성자입니다.
	virtual				~CRollupEtcDev();						///< 소멸자

	// Listenable View
	virtual void		Listen(CoreMessage& _message);		///< Listen
	virtual const char*	GetViewName(){ return "Development"; }	///< View Name

	virtual BOOL		OnInitDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VR_ETC_DEV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	virtual void OnCancel(){}
	virtual void OnOK(){}

	DECLARE_MESSAGE_MAP()
};
