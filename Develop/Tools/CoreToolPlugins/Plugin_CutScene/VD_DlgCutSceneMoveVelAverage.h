#pragma once

#include <vector>

class CXCutSceneWrapper;
// CDlgCutSceneMoveVelAverage 대화 상자입니다.

class CDlgCutSceneMoveVelAverage : public CDialog
{
	DECLARE_DYNAMIC(CDlgCutSceneMoveVelAverage)

private:
	CXCutSceneWrapper* m_pCutSceneWrapper;
	int m_nCurrentSelected;

	void SetEnableIntervalAll();
	void SetEnableIntervalPart();

public:
	CDlgCutSceneMoveVelAverage(CXCutSceneWrapper* pCutSceneWrapper, int nCurrentSelected, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCutSceneMoveVelAverage();

	int m_nIntervalType;
	float m_fIntervalStart;
	float m_fIntervalEnd;

	float m_fAverVel;

	void CalcPart(float& _interval, int& _first, int& _last, const std::vector<float>& _times);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CUTSCENE_MOVEVEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadioCutsceneMoveIntervalAll();
	afx_msg void OnBnClickedRadioCutsceneMoveIntervalPart();
	afx_msg void OnBnClickedButtonCutsceneCalcMoveIntervalVel();
};
