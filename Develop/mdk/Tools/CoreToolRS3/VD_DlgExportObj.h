#pragma once
#include "afxwin.h"


// CDlgExportObj 대화 상자입니다.
class CDlgExportObj : public CDialog
{
	DECLARE_DYNAMIC(CDlgExportObj)

public:
	CDlgExportObj(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgExportObj();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_OBJ_EXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_nLODLevel;
	int m_nWPart;
	int m_nHPart;
};
