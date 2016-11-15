#pragma once

#include "XRealSound.h"
#include "RangeDlg.h"
#include "afxwin.h"
#include "afxcmn.h"


class CMainDlg : public CDialog
{
public:
	enum { IDD = IDD_MAIN_DIALOG };


protected:
	HICON				m_hIcon;
	XRealSound			m_Sound;
	CRangeDlg			m_SoundRange;
	int					m_nPlayedSound[ 4];


public:
	CMainDlg(CWnd* pParent = NULL);

	void OpenFileDialog( int nIndex);
	void ChangeType( int nIndex);

	void Play( int nIndex);
	void Mute( int nIndex);

	POINT GetPosition( int nNum)			{ return m_SoundRange.GetPosition( nNum);		}


protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedSearch1();
	afx_msg void OnBnClickedSearch2();
	afx_msg void OnBnClickedSearch3();
	afx_msg void OnBnClickedSearch4();
	afx_msg void OnCbnSelchangeType1();
	afx_msg void OnCbnSelchangeType2();
	afx_msg void OnCbnSelchangeType3();
	afx_msg void OnCbnSelchangeType4();
	afx_msg void OnBnClickedPlay1();
	afx_msg void OnBnClickedPlay2();
	afx_msg void OnBnClickedPlay3();
	afx_msg void OnBnClickedPlay4();
	afx_msg void OnBnClickedPlayAll();
	afx_msg void OnBnClickedStopAll();
	afx_msg void OnBnClickedReleaseAll();
	afx_msg void OnBnClickedMute1();
	afx_msg void OnBnClickedMute2();
	afx_msg void OnBnClickedMute3();
	afx_msg void OnBnClickedMute4();
	afx_msg void OnBnClickedEncompressor();
	afx_msg void OnCbnSelchangeReverb();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedHwaccel();
	afx_msg void OnBnClickedDoppler();
	afx_msg void OnBnClickedSilence();
	afx_msg void OnNMCustomdrawTimeline1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeSpeakermode();

	DECLARE_MESSAGE_MAP()
};
