#pragma once


class CRangeDlg : public CWnd
{
protected:
	CDC*		m_pDC;
	CDC*		m_pDCRange;
	CBitmap*	m_pBitmap;
	CBitmap*	m_pBmpRange;

	int			m_nWidth, m_nHeight;
	int			m_nBmpWidth, m_nBmpHeight;

	int			m_nSelected;
	POINT		m_ptLButtonDown;
	POINT		m_ptPosition[ 4];


public:
	CRangeDlg();
	virtual ~CRangeDlg();

	void Create();

	void SetPosition( int nNum, int x, int y);
	POINT GetPosition( int nNum);

	void Draw();


protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};
