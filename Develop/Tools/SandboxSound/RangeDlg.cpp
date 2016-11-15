#include "stdafx.h"
#include "RangeDlg.h"
#include "resource.h"


BEGIN_MESSAGE_MAP(CRangeDlg, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



CRangeDlg::CRangeDlg()
{
	m_pDC = NULL;
	m_pDCRange = NULL;
	m_pBitmap = NULL;
	m_pBmpRange = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nBmpWidth = 0;
	m_nBmpHeight = 0;

	m_nSelected = -1;

	m_ptPosition[ 0].x = -500;
	m_ptPosition[ 0].y = 250;
	m_ptPosition[ 1].x = -250;
	m_ptPosition[ 1].y = 500;
	m_ptPosition[ 2].x = 250;
	m_ptPosition[ 2].y = 500;
	m_ptPosition[ 3].x = 500;
	m_ptPosition[ 3].y = 250;
}


CRangeDlg::~CRangeDlg()
{
	if ( m_pDC != NULL)
	{
		m_pBitmap->DeleteObject();
		delete m_pBitmap;
		m_pBitmap = NULL;

		m_pDC->DeleteDC();
		delete m_pDC;
		m_pDC = NULL;
	}

	if ( m_pDCRange != NULL)
	{
		m_pBmpRange->DeleteObject();
		delete m_pBmpRange;
		m_pBmpRange = NULL;

		m_pDCRange->DeleteDC();
		delete m_pDCRange;
		m_pDCRange = NULL;
	}
}


void CRangeDlg::Create()
{
	if ( m_pDC != NULL)			return;

	CDC* pDC = GetDC();
	if ( pDC == NULL)			return;

	m_pDC = new CDC();
	m_pDC->CreateCompatibleDC( pDC);

	RECT r;
	GetWindowRect( &r);
	m_nWidth = r.right - r.left;
	m_nHeight = r.bottom - r.top;

	m_pBitmap = new CBitmap();
	m_pBitmap->CreateCompatibleBitmap( pDC, m_nWidth, m_nHeight);
	m_pDC->SelectObject( m_pBitmap);


	CFont font;
	font.CreateFont( 15, 0, 0, 0, false, false, false, 0, ANSI_CHARSET, OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
	m_pDC->SelectObject( font);
	m_pDC->SetTextColor( RGB( 255, 255, 0));



	m_pBmpRange = new CBitmap();
	if ( m_pBmpRange->LoadBitmap( IDB_RANGE) == TRUE)
	{
		m_pDCRange = new CDC();
		m_pDCRange->CreateCompatibleDC( pDC);
		m_pDCRange->SelectObject( m_pBmpRange);

		BITMAP bmpInfo;
		m_pBmpRange->GetBitmap( &bmpInfo);
		m_nBmpWidth = bmpInfo.bmWidth;
		m_nBmpHeight = bmpInfo.bmHeight;
	}


	Draw();
}


void CRangeDlg::SetPosition( int nNum, int x, int y)
{
	m_ptPosition[ nNum].x = x;
	m_ptPosition[ nNum].y = y;

	Draw();

	CDC* pDC = GetDC();
	if ( m_pDC != NULL  &&  pDC != NULL)	pDC->BitBlt( 0, 0, m_nWidth, m_nHeight, m_pDC, 0, 0, SRCCOPY);	
}


POINT CRangeDlg::GetPosition( int nNum)
{
	return m_ptPosition[ nNum];
}


void CRangeDlg::Draw()
{
	if ( m_pDC == NULL)							return;

	m_pDC->FillSolidRect( 0, 0, m_nWidth, m_nHeight, RGB( 0, 0, 0));
	m_pDC->BitBlt( (m_nWidth - m_nBmpWidth) >> 1, (m_nHeight - m_nBmpHeight) >> 1, m_nBmpWidth, m_nBmpHeight, m_pDCRange, 0, 0, SRCCOPY);


	for ( int i = 0;  i < 4;  i++)
	{
		int x = (m_nWidth >> 1) + (int)( (float)m_ptPosition[ i].x * 0.125f);
		int y = (m_nHeight >> 1) - (int)( (float)m_ptPosition[ i].y * 0.125f);

		m_pDC->FillSolidRect( x - 10, y - 10, 20, 20, RGB( 255, 100, 100));
		m_pDC->FillSolidRect( x - 8, y - 8, 16, 16, RGB( 160, 0, 0));

		CString str;
		str.Format( "%d", i + 1);
		m_pDC->TextOut( x - 4, y - 8, str);
	}
}


BOOL CRangeDlg::PreTranslateMessage(MSG* pMsg)
{
	switch ( pMsg->message)
	{
	case WM_LBUTTONDOWN :
		{
			RECT rect;
			GetWindowRect( &rect);

			POINT pt;
			pt.x = (pMsg->pt.x - rect.left) - ((rect.right - rect.left) >> 1);
			pt.y = (pMsg->pt.y - rect.top) - ((rect.bottom - rect.top) >> 1);

			for ( int i = 0;  i < 4;  i++)
			{
				int x =  (int)( (float)m_ptPosition[ i].x * 0.125f);
				int y = -(int)( (float)m_ptPosition[ i].y * 0.125f);

				if ( pt.x >= (x - 10)  &&  pt.x <= (x + 10)  &&  pt.y >= (y - 10)  &&  pt.y <= (y + 10))
				{
					m_nSelected = i;
					m_ptLButtonDown = pMsg->pt;

					SetCapture();
					break;
				}
			}
			break;
		}

	case WM_LBUTTONUP :
		{
			m_nSelected = -1;

			ReleaseCapture();
			break;
		}

	case WM_MOUSEMOVE :
		if ( m_nSelected >= 0)
		{
			POINT delta;
			delta.x = (pMsg->pt.x - m_ptLButtonDown.x) * 8;
			delta.y = (pMsg->pt.y - m_ptLButtonDown.y) * 8;

			m_ptPosition[ m_nSelected].x += delta.x;
			m_ptPosition[ m_nSelected].y -= delta.y;

			m_ptLButtonDown = pMsg->pt;


			Draw();

			CDC* pDC = GetDC();
			if ( m_pDC != NULL  &&  pDC != NULL)	pDC->BitBlt( 0, 0, m_nWidth, m_nHeight, m_pDC, 0, 0, SRCCOPY);
			break;
		}
	}


	return CWnd::PreTranslateMessage( pMsg);
}


void CRangeDlg::OnPaint() 
{
	if ( m_pDC != NULL)
	{
		CPaintDC dc( this);
		dc.BitBlt( 0, 0, m_nWidth, m_nHeight, m_pDC, 0, 0, SRCCOPY);
	}
}
