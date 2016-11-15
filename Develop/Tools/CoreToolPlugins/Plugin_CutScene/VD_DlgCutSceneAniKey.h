#pragma once
#include "afxwin.h"


// CDlgCutSceneAniKey 대화 상자입니다.

class CDlgCutSceneAniKey : public CDialog
{
	DECLARE_DYNAMIC(CDlgCutSceneAniKey)

public:
	CDlgCutSceneAniKey(float _fTime, const char* _pSzStrAnimation = NULL, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCutSceneAniKey();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CUTSCENE_ANIKEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	float	m_fTime;
	CString m_strAnimation;
};
