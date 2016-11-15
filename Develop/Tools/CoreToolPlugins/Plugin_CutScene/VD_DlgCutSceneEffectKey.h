#pragma once

#include "MTypes.h"
// CDlgCutSceneEffectKey 대화 상자입니다.
class CControlMediator;
class XEffectInfoMgr;

class CDlgCutSceneEffectKey : public CDialog
{
	DECLARE_DYNAMIC(CDlgCutSceneEffectKey)

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CDlgCutSceneEffectKey(CControlMediator* _pControlMediator, float fTime, float fDurationTime, vec3 vPos, vec3 vDir, vec3 vUp, const string& strEffectName, const string& strEffectInfo, CWnd* pParent = NULL);
	virtual ~CDlgCutSceneEffectKey();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CUTSCENE_EFFECTKEY };

	void GetEffectData(float& fTime, float& fDurationTime, vec3& vPos, vec3& vDir, vec3& vUp, std::string& _rEffectName, std::string& _rEffectInfo);
	virtual BOOL OnInitDialog();

public:
	float m_fTime;
	float m_fDurationTime;
	int m_nFixedOrFlow;

	vec3 m_vPos;
	vec3 m_vDir;
	vec3 m_vUp;

	CString m_strEffectName;
	CString m_strEffectInfo;

	CString	m_strPos;
	CString	m_strDir;
	CString	m_strUp;

	CString m_strNpcName;
	CString m_strBoneName;
	CString m_strPostActor;

	XEffectInfoMgr* m_pEffectInfoMgr;
	afx_msg void OnBnClickedRadioCutsceneEffectFixed();
	afx_msg void OnBnClickedRadioCutsceneEffectFollow();
	afx_msg void OnBnClickedRadioCutsceneEffectCamera();
	afx_msg void OnBnClickedRadioCutsceneEffectPost();
};
