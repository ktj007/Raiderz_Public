#pragma once

#include "afxwin.h"

#include <string>


// CDlgCutSceneEnvKey 대화 상자입니다.

class CControlMediator;
class CDlgCutSceneEnvKey : public CDialog
{
	DECLARE_DYNAMIC(CDlgCutSceneEnvKey)

public:
	CDlgCutSceneEnvKey(CControlMediator* _pControlMediator, float fTime, int nEnvTime, int nEnvWeather, const char* pSzSetName, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCutSceneEnvKey();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CUTSCENE_ENVKEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	CControlMediator* m_pControlMediator_;

public:
	float m_fTime;
	int m_nEnvTime;
	int m_nEnvWeather;
	std::string m_strEnvSetName;

	CComboBox m_ctlEnvSet;
	CComboBox m_ctlEnvTimes;
	CComboBox m_ctlEnvWeathers;

	void GetEnvKeyInfo(float& fTime, int& nEnvTime, int& nEnvWeather, std::string& rEnvSetName);
	afx_msg void OnDestroy();
};
