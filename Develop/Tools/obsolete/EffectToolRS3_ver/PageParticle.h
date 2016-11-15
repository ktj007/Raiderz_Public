#pragma once

#include "RTypes.h"
#include "RPType.h"
#include <vector>
#include "afxwin.h"
#include "afxcmn.h"
#include "HexEdit.h"

using namespace rs3;

// CPage3 대화 상자입니다.

class CPage3 : public CPPage
{
	DECLARE_DYNAMIC(CPage3)

public:
	CPage3();
	virtual ~CPage3();

	virtual BOOL OnInitDialog();

	void		GetVariables(const std::vector<RParticleVariable> &rVar);
	void		SetVariables(std::vector<RParticleVariable> &rVar, int iSel, bool btime = false);
	void		UpdateGradation();
	bool		ConfigFromXML();
	void		InitUIState();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PARTICLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	inline void SendMessageParticleModyfy()
	{
		AfxGetMainWnd()->SendMessage( WM_MODIFY_MSG , 28);
	}

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	afx_msg void OnLbnSelchangeListTrack();
	afx_msg void OnEnChangeEdits();
	afx_msg void OnBnClickedButtonP1();
	afx_msg void OnBnClickedButtonP2();
	afx_msg void OnBnClickedButtonUpdate();
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);

	// 비율
	static const int RATE = 10;

	float		m_fMinLife;
	float		m_fMaxLife;

	float		m_fTime;
	float		m_fAlpha;
	float		m_fScale;
	float		m_fScaleA;
	float		m_fVelX;
	float		m_fVelY;
	float		m_fVelZ;
	float		m_fGRate;
	float		m_fScale2;
	//float		m_fPosX;
	//float		m_fPosY;
	//float		m_fPosZ;
	float		m_fMaxVelocity;
	float		m_fMinVelocity;
// 	BOOL		m_bRandColor;
// 	BOOL		m_bRandVel;

	float		m_fGRateMin;
	float		m_fGRateMax;

	float		m_fWind;
	float		m_fWDirX;
	float		m_fWDirY;
	float		m_fWDirZ;

	CSliderCtrl m_ctlMaxLife;
	CSliderCtrl m_ctlMinLife;

	CHexEdit	m_ctlColor;
	CListBox	m_ctlListTrack;
	CSliderCtrl m_ctlSlideTrack;
	CSliderCtrl m_ctlMaxVelocity;
	CSliderCtrl m_ctlMinVelocity;
	CSliderCtrl m_ctlGravity;
	CSliderCtrl m_ctlScale2;
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonRemove();
	afx_msg void OnEnChangeEditColor();
};
