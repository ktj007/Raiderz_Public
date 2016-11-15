
#pragma once

#include ".\Profile\profile.h"
#include <list>

class RCPUAnalyzeDialog;

class CRectangle
{
public:
	CRectangle(float Height,CProfileManager* pProfileManager)
		:m_Height(Height),m_pProfileManager(pProfileManager),m_bLeftSelected(false),m_bRightSelected(false),m_bInGraphCtrl(false)
	{
	}
	~CRectangle(void){}
public:
	float				m_Height;
	CProfileManager*	m_pProfileManager;
	bool				m_bLeftSelected;
	bool				m_bRightSelected;
	bool				m_bInGraphCtrl;
};

class CGraphCtrl: public CStatic
{
public:
	CGraphCtrl(void);
	~CGraphCtrl(void);

public:
	void	AddItem(CProfileManager* pProfileManager);
	bool	DeleteItem(CRectangle* pRectangle);
	void	SetProfileDialog(RCPUAnalyzeDialog *pProfileDialog)	{ m_pProfileDialog = pProfileDialog; }
	void	SetYScale(int YScale) {	m_YScale = YScale;	DisplayToDC(); }

	void	SetInit(bool b)		{	m_bInit = b;	}
	bool	IsInitialized(void)	{	return m_bInit;	}

private:
	void	DisplayToDC(void);
	int		GetRectangleWidth(void);
	void	ReSizeItem(int ReservedSize);
	

private:
	std::list<CRectangle*>		m_ListOfRect;
	int							m_RectNumLimit;	// Rect을 저장하는 리스트의 크기 제한
	int							m_YScale;

private://DC
	CRect		m_Rect;
	CDC			m_dcGraph;
	CBitmap		m_Bitmap, *m_pOldBitmap;

	bool		m_bInit;

private:
	RCPUAnalyzeDialog *m_pProfileDialog;
	CRectangle		*m_LeftSelectedRectangle;
	CRectangle		*m_RightSelectedRectangle;
	

protected:
	virtual void PreSubclassWindow();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};