#pragma once

#include "afxspinbuttonctrl.h"

class CReflectiveFloatProperty;

class CReflectiveSpinCtrl : public CMFCSpinButtonCtrl
{
public:
	CReflectiveSpinCtrl(CReflectiveFloatProperty* pProperty);
	virtual ~CReflectiveSpinCtrl();

	void	SetCaptureTimer();
	void	ReleaseCaptureTimer();

protected:
	CReflectiveFloatProperty* m_pProperty;

	// 스핀콘트롤이 수시로 다시 만들어져서 static 으로 선언
	static float	m_fOriginalValue;
	static CPoint	m_ptBeginDrag;
	static BOOL		m_bDrag;
	float			m_fLastValue;

	//{{AFX_MSG(ReflectiveSpinCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDraw(CDC* pDC);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};