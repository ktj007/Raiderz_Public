#pragma once

#include "VL_ListenableView.h"
#include "resource.h"

// CRollupCoreToolRS3 대화 상자입니다.

class CORETOOL_API CRollupCoreToolRS3 : public CDialog, public CListenableViewImpl_Rollup<CRollupCoreToolRS3>
{
	DECLARE_DYNAMIC(CRollupCoreToolRS3)

public:
	CRollupCoreToolRS3(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CRollupCoreToolRS3();

	virtual void Listen(CoreMessage& _message){}
	virtual const char* GetViewName(){ return "Hello CoreToolRS3"; }

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VR_CORE_TOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	virtual void OnCancel(){}
	virtual void OnOK(){}

	DECLARE_MESSAGE_MAP()
};
