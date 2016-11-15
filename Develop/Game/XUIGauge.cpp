#include "stdafx.h"
#include "XUIGauge.h"
#include "MProgressCtrl.h"
#include "MColorTable.h"



// XUIGauge
XUIGauge::XUIGauge( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( XGaugeLook);

	m_bStatic = true;

	m_nLower = 0;
	m_nUpper = 100;
	m_nBeginPos = 0;
	m_nEndPos = 0;
	m_bEnableSliding = false;

	m_tSliding.SetArrivalTime( 300);
}


// SetRange
void XUIGauge::SetRange( int nLower, int nUpper)
{
	if ( nLower == m_nLower  &&  nUpper == m_nUpper)	return;
	if ( nLower > nUpper)
	{
		// Lower가 Upper보다 낮아질수 있습니다. (특수한경우)
		nLower = nUpper;
	}

	m_nLower = nLower;
	m_nUpper = nUpper;

	if ( m_nEndPos < nLower)			m_nBeginPos = m_nEndPos = nLower;
	else if ( m_nEndPos > nUpper)		m_nBeginPos = m_nEndPos = nUpper;
}


// GetRange
void XUIGauge::GetRange( int& nLower, int& nUpper) const
{
	nLower = m_nLower;
	nUpper = m_nUpper;
}


// SetPos
int XUIGauge::SetPos( int nPos)
{
	nPos = max( m_nLower, nPos);
	nPos = min( m_nUpper, nPos);

	if ( nPos == m_nEndPos)		return nPos;

	if ( m_bEnableSliding == false)
	{
		m_nBeginPos = nPos;
		m_nEndPos = nPos;
	}
	else
	{
		if ( m_tSliding.IsArrival() == true)
		{
			m_nBeginPos = GetPos();
			m_nEndPos = nPos;

			m_tSliding.Start();
		}
		else
		{
			m_nEndPos = nPos;
		}
	}

	return m_nEndPos;
}


// GetPos
int XUIGauge::GetPos() const
{
	int nPos = 0;

	if ( m_bEnableSliding == false)
	{
		nPos = m_nEndPos;
	}
	else
	{
		float _ratio = m_tSliding.GetRatio();
		if ( _ratio < 0.5f)		_ratio = 0.0f;
		else					_ratio = ( _ratio - 0.5f) * 2.0f;
		nPos = m_nBeginPos + (int)( (double)(m_nEndPos - m_nBeginPos) * _ratio);
	}

	return nPos;
}


// GetEndPos
int XUIGauge::GetEndPos() const
{
	return m_nEndPos;
}


// EnableSliding
void XUIGauge::EnableSliding( bool bEnable)
{
	m_bEnableSliding = bEnable;

	m_tSliding.SetEnable( bEnable);
}












// XGaugeLook
XGaugeLook::XGaugeLook()
{
	m_pSmallFont = NULL;
	m_nTextViewStyle = MPCTS_NONE;
	SetTextAlign( MAM_RIGHT | MAM_VCENTER);
}

void XGaugeLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
	XUIGauge* pGauge = (XUIGauge*)pWidget;
	MRECT r = pGauge->GetClientRect();
	r.x += m_WorkRectOffset.x;
	r.y += m_WorkRectOffset.y;

	MLookBase::OnDrawBorder( pWidget, pDC);
	OnDrawGauge( pGauge, r, pDC);
	OnDrawText( pGauge, r, pDC);
}

void XGaugeLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
}

void XGaugeLook::OnDrawGauge( XUIGauge* pGauge, MRECT& r, MDrawContext* pDC)
{
	MDrawEffect _effect = pDC->SetEffect( MDE_ADD);

	int nLower, nUpper, nPos, nEndPos;
	pGauge->GetRange( nLower, nUpper);
	nPos = pGauge->GetPos();
	nEndPos = pGauge->GetEndPos();


	float fRatio;
	int nLength;
	if ( nEndPos < nPos)
	{
		fRatio = (float)(nEndPos - nLower) / (float)(nUpper - nLower);
		nLength = (int)( (float)r.w * fRatio + 0.5f);
	}
	else
	{
		fRatio = (float)(nPos - nLower) / (float)(nUpper - nLower);
		nLength = (int)( (float)r.w * fRatio + 0.5f);
	}


	// Draw gauge
	{
		MRECT sr = GetAssignRect( MWP_PROGRESSBAR, MWS_NORMAL)[ 0];
		MRECT tr( sr.x, sr.y, sr.w * fRatio, sr.h); 
		pDC->Draw( r.x, r.y, nLength, r.h, tr.x, tr.y, tr.w, tr.h);


		if ( nEndPos < nPos)
		{
			float _fRatio = (float)(nPos - nLower) / (float)(nUpper - nLower);
			int _nLength = (int)( (float)r.w * _fRatio + 0.5f);

			MRECT _sr = GetAssignRect( MWP_PROGRESSBAR, MWS_DISABLE)[ 0];
			MRECT _tr( _sr.x + tr.w, _sr.y, _sr.w * (_fRatio - fRatio), _sr.h);

			pDC->Draw( nLength, r.y, _nLength - nLength, r.h, _tr.x, _tr.y, _tr.w, _tr.h);
		}
		else if ( nEndPos > nPos)
		{
			float _fRatio = (float)(nEndPos - nLower) / (float)(nUpper - nLower);
			int _nLength = (int)( (float)r.w * _fRatio + 0.5f);

			MRECT _sr = GetAssignRect( MWP_PROGRESSBAR, MWS_FOCUS)[ 0];
			MRECT _tr( _sr.x + tr.w, _sr.y, _sr.w * (_fRatio - fRatio), _sr.h);

			pDC->Draw( nLength, r.y, _nLength - nLength, r.h, _tr.x, _tr.y, _tr.w, _tr.h);
		}
	}


	// Caret
	if ( nPos != nEndPos  ||  nPos < nUpper)
	{
//		pDC->SetEffect( MDE_ADD);

		float fOpacity = ((MGetMint()->GetUpdateTime() % 200) < 100) ? 1.0f : 0.75f;
		float old_opacity = pDC->SetOpacity( fOpacity);
		int nSize = r.h * 1.80;
		Draw( pDC, MRECT( r.x + nLength - (nSize >> 1), r.y + (r.h >> 1) - (nSize >> 1), nSize, nSize), MWP_CARET, MWS_NORMAL);

		pDC->SetOpacity( old_opacity);
//		pDC->SetEffect( MDE_NORMAL);
	}

	pDC->SetEffect( _effect);
}

void XGaugeLook::OnDrawText( XUIGauge* pGauge, MRECT& r, MDrawContext* pDC)
{
	// Name
	pDC->SetFont( GetFont());
	pDC->SetColor( GetFontColor( MWP_BORDER, pGauge->GetState(), MColorTable::TEXT[ MWSN_NORMAL]));
	pDC->Text( r, pGauge->GetText(), (GetTextAlign() & 0xF0) | MAM_LEFT);


	// Progress
	int nLower, nUpper, nPos;
	pGauge->GetRange( nLower, nUpper);
	nPos = pGauge->GetPos();

	char buff[ 128];
	switch ( m_nTextViewStyle)
	{
	case MPCTS_PERCENT :
		sprintf_s( buff, "%.1f%%", (float)(nPos - nLower) / (float)(nUpper - nLower) * 100.0f);
		break;

	case MPCTS_PROGRESS :
		sprintf_s( buff, "%d / %d", nPos, nUpper);
		break;

	default :
		return;
	}

	pDC->SetFont( GetSmallFont());
	pDC->SetColor( GetFontColor( MWP_PROGRESSBAR, pGauge->GetState(), MColorTable::TEXT[ MWSN_NORMAL]));
	pDC->Text( MRECT( r.x + 5, r.y, r.w - 10, r.h), buff, GetTextAlign());
}

void XGaugeLook::SetSmallFont(const char* szFont)
{
	m_pSmallFont = MFontManager::Get( szFont);
}