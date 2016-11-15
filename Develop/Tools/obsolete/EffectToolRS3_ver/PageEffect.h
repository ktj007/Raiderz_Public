#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CPage1 대화 상자입니다.

class CPage1 : public CPPage
{
	DECLARE_DYNAMIC(CPage1)

public:
	CPage1();
	virtual ~CPage1();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EFFECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	inline void SendMessageCreateEmitter()
	{
		AfxGetMainWnd()->SendMessage( WM_MODIFY_MSG , 1);
	}
	inline void SendMessageSelectMesh(int sel)
	{
		AfxGetMainWnd()->SendMessage( WM_MODIFY_MSG , 4, sel);
	}
	inline void SendMessageSelectEmitter(int sel)
	{
		AfxGetMainWnd()->SendMessage( WM_MODIFY_MSG , 5, sel);
	}
	inline void SendMessageDelEmitter(int sel)
	{
		AfxGetMainWnd()->SendMessage( WM_MODIFY_MSG , 6, sel);
	}
	inline void SendMessageLoadEmitter()
	{
		AfxGetMainWnd()->SendMessage( WM_MODIFY_MSG , 7);
	}
	inline void SendMessageSaveEmitter(int sel)
	{
		AfxGetMainWnd()->SendMessage( WM_MODIFY_MSG , 8, sel);
	}
	inline void SendMessageDelMesh(int sel)
	{
		AfxGetMainWnd()->SendMessage( WM_MODIFY_MSG , 26, sel);
	}
	inline void SendMessageMeshModify()
	{
		AfxGetMainWnd()->SendMessage( WM_MODIFY_MSG , 10);
	}
	inline void SendMessageSetEffectName()
	{
		AfxGetMainWnd()->SendMessage( WM_MODIFY_MSG , DUCK_MSG_SETEFFECTNAME);
	}

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnEnChangeEdites();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLbnSelchangeListEmitter2();
	afx_msg void OnBnClickedCheckMeshlooping();

	//Effect
	CListBox	m_EmitterList;
	CString		m_strEffectName;

	//메쉬관련
	BOOL		m_bMeshLooping;
	float		m_fScale;
	CSliderCtrl m_ctlScale;

	static const int RATE = 10;

	void	InitValue();
	void	SetEffectName(const char * const pName);
	void	AddListBox(const char * const pName);
	bool	IsMeshEffect(int iSel);
};
