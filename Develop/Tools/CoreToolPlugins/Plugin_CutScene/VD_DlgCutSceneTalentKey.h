#pragma once
#include "afxwin.h"


// CDlgCutSceneTalentKey 대화 상자입니다.

class CDlgCutSceneTalentKey : public CDialog
{
	DECLARE_DYNAMIC(CDlgCutSceneTalentKey)

public:
	CDlgCutSceneTalentKey(float _fTime, const int _nID = 0, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCutSceneTalentKey();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CUTSCENE_TALENTKEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	float	m_fTime;
	int		m_nID;
};
