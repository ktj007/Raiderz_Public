#pragma once
#include "afxwin.h"
#include "resource.h"

// CDlgInputSensorWarp 대화 상자입니다.

class CControlMediator;
class CORETOOL_SOULHUNT_API CDlgInputSensorWarp : public CDialog
{
	DECLARE_DYNAMIC(CDlgInputSensorWarp)

public:
	CDlgInputSensorWarp(CControlMediator* _pControlMediator, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgInputSensorWarp();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_INPUT_SENSOR_WARP };
	static bool m_isActive;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	void EnableSharedFieldGroup(bool isEnable);
	void EnableTrialFieldGroup(bool isEnable);

	DECLARE_MESSAGE_MAP()

private:
	CControlMediator* m_pControlMediator_;
public:	
	int m_nWarpFieldType;
	CString m_strTitle;	
	int m_nCondition;	
	int m_nGateType;	
	int m_nField;
	int m_nMarker;	
	int m_nFieldGroupID;
	int m_nQuestID;
	CEdit m_edtField;
	CEdit m_edtMarker;
	CStatic m_sttSharedFieldGroup;
	CStatic m_sttTrialGroup;
	CEdit m_edtFieldGroupID;
	CEdit m_edtQuestID;
	CStatic m_sttFieldGroupID;
	CStatic m_sttQuestID;
	CComboBox m_cmbGateType;
	CButton m_btnField;
	CButton m_btnFieldGroupID;
	CStatic m_sttField;
	CStatic m_sttMarker;

	afx_msg void OnBnClickedButtonOpenFieldMgr();
	afx_msg void OnBnClickedButtonOpenFieldGroupMgr();
	afx_msg void OnBnClickedRadioSharedWarpType();
	afx_msg void OnBnClickedRadioTrialWarpType();
};
