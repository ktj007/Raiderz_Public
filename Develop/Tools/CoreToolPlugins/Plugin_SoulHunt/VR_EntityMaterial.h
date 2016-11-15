#pragma once

#include "VL_ListenableView.h"
#include "afxwin.h"

// CRollupEntityMaterial 대화 상자입니다.
/** @brief Entity Material Rollup */
class CRollupEntityMaterial : public CDialog, public CListenableViewImpl_Rollup<CRollupEntityMaterial>
{
	DECLARE_DYNAMIC(CRollupEntityMaterial)

public:
							CRollupEntityMaterial(CWnd* pParent = NULL);	///< 표준 생성자입니다.
	virtual					~CRollupEntityMaterial();						///< 소멸자

	// Listenable View
	virtual void			Listen(CoreMessage& _message);				///< Listen
	virtual const char*		GetViewName(){ return "Entity Material"; }		///< View Name

	// CDialog
	virtual BOOL			OnInitDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VR_ENTITY_MATERIAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	virtual void OnCancel(){}
	virtual void OnOK(){}

	DECLARE_MESSAGE_MAP()
};
