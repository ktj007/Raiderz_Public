#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CPage1 대화 상자입니다.

class CPageEmitter : public CPPage
{
	DECLARE_DYNAMIC(CPageEmitter)

public:
	CPageEmitter();
	virtual ~CPageEmitter();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_EMITTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	inline void SendMessageEmitterModify()
	{
		AfxGetMainWnd()->SendMessage( WM_MODIFY_MSG , 27);
	}
	inline void SendMessageSelectEmitter(int sel)
	{
		AfxGetMainWnd()->SendMessage( WM_MODIFY_MSG , 5, sel);
	}
	inline void SendMessageDelMesh(int sel)
	{
		AfxGetMainWnd()->SendMessage( WM_MODIFY_MSG , 26, sel);
	}

	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEdites();
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheck();
	afx_msg void OnLbnSelchangeListEmitterTrack();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonRemove();
	afx_msg void OnBnClickedButtonUpdate();
	afx_msg void OnCbnSelchangeComboType();

	CComboBox	m_ctlType;
	UINT		m_uMaxCount;
	UINT		m_uAmount;
	float		m_fLife;
	float		m_fAngleD;
// 	float		m_fMinLife;
// 	float		m_fMaxLife;

	float		m_fPosX;
	float		m_fPosY;
	float		m_fPosZ;

	float		m_fDirX;
	float		m_fDirY;
	float		m_fDirZ;
	BOOL		m_bLooping;
	BOOL		m_bSorting;

	BOOL		m_bRotation;
	float		m_fMinRadian;
	float		m_fMaxRadian;

	BOOL		m_bColorRand;
	BOOL		m_bVelRand;

	BOOL		m_bGather;
	BOOL		m_bExplosion;

	CSliderCtrl m_ctlMaxCount;
	CSliderCtrl m_ctlAmount;
	CSliderCtrl m_ctlLife;
	CSliderCtrl m_ctlAngle;

	float		m_fTime;
	CListBox	m_ctlListTrack;
	CSliderCtrl m_ctlSlideTrack;

	CSliderCtrl m_ctlMaxRadian;
	CSliderCtrl m_ctlMinRadian;

	static const int RATE = 10;

	bool		ConfigFromXML();
	void		InitUIState();
	void		GetVariables(const vector<REmitterVariable> &rVar);
	void		SetVariables(std::vector<REmitterVariable> &rVar, int iSel, bool btime = false);

	CButton m_ctlRadioXYZ;
	CButton m_ctlRadioXY;
	CButton m_ctlRadioPlaneXY;
	CButton m_ctlRadioSpread;

	afx_msg void OnBnClickedRadioBillboard();
};
