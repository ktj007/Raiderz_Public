#pragma once


// CDlgInputEnvSetImport 대화 상자입니다.

class CORETOOL_API CDlgInputEnvSetImport : public CDialog
{
	DECLARE_DYNAMIC(CDlgInputEnvSetImport)

public:
	CDlgInputEnvSetImport(CWnd* pParent = NULL);   // 표준 생성자입니다.
	CDlgInputEnvSetImport(int _nTime, int _nWeather);   // 표준 생성자입니다.
	virtual ~CDlgInputEnvSetImport();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_INPUT_ENV_IMPORT };

	int m_nTime;
	int m_nWeather;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	afx_msg void OnCbnSelchangeComboTimesImport();
	afx_msg void OnCbnSelchangeComboWeathersImport();

	DECLARE_MESSAGE_MAP()

private:
	CComboBox m_ctlTimesToImport_;
	CComboBox m_ctlWeathersToImport_;
};
