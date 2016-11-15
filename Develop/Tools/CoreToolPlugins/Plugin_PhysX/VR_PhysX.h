#pragma once

#include "VL_ListenableView.h"
#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"

// CRollupPhysX 대화 상자입니다.

class CRollupPhysX : public CDialog, public CListenableViewImpl_Rollup<CRollupPhysX>
{
	DECLARE_DYNAMIC(CRollupPhysX)

public:
	CRollupPhysX(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CRollupPhysX();

	virtual void Listen(CoreMessage& _message);
	virtual const char* GetViewName(){ return "MPhysX Rollup"; }

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VR_PHYSX };

	void LoadPxTabList(int nForceSet = -1);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	virtual void OnCancel(){}
	virtual void OnOK(){}

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnTcnSelchangeTabPhx(NMHDR *pNMHDR, LRESULT *pResult);
	CTabCtrl m_ctrlPhxListTab;
	enum TAB_TYPE
	{
		E_ALL = 0,
		E_MONSTER,

		E_TAB_NUM
	};
	CListBox m_ctrlObjList;
	afx_msg void OnLbnSelchangeListPxobj();
	afx_msg void OnBnClickedButtonBpart1();
	afx_msg void OnBnClickedButtonBpart2();
	afx_msg void OnBnClickedButtonBpart3();
	afx_msg void OnBnClickedButtonBpart4();
	afx_msg void OnBnClickedButtonPxReload();
};
