#pragma once

class CControlMediator;

// CDlgNewMiniMap 대화 상자입니다.

class CDlgNewMiniMap : public CDialog
{
	DECLARE_DYNAMIC(CDlgNewMiniMap)

public:
	CDlgNewMiniMap(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgNewMiniMap();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_NEW_MINI_MAP };
	bool CreateMiniMap(CControlMediator* _pControlMediator);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
