#include "stdafx.h"
#include "MEditCtrl.h"
#include "Mint.h"
#include "MColorTable.h"
#include "MLocale.h"
#include "MScrollBar.h"


namespace mint3
{


// MEditCtrl
MEditCtrl::MEditCtrl( const char* szName, MWidget* pParent, MListener* pListener)
	: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( MEditCtrlLook);

	SetEnableFocus( true);

	m_nCaret = 0;
	m_nStartSel = 0;
	m_nEndSel = 0;
	m_bEditable = true;
	m_bEnsureCaretVisible = false;
	m_bUpdateScrollBar = false;

	m_pScrollBar = new MScrollBar( NULL, this, this, MSBS_VERT | MSBS_RIGHTALIGN);
	m_pScrollBar->SetAlwaysShow( true);
}


// ~MEditCtrl
MEditCtrl::~MEditCtrl()
{
	m_strString.clear();

	if ( m_pScrollBar)
	{
		delete m_pScrollBar;
		m_pScrollBar = NULL;
	}
}


// SetText
void MEditCtrl::SetText( const char* szText)
{
	MWidget::SetText( szText);

	string _mbs( szText);
	m_strString = MLocale::ConvAnsiToUTF16( _mbs.c_str());

	size_t _len = m_strString.length();
	m_nCaret = min( m_nCaret, _len);
	m_nStartSel = min( m_nStartSel, _len);
	m_nEndSel = min( m_nEndSel, _len);

	ReleaseSelect();

	UpdateLineInfo();
	UpdateScrollBar();
	EnsureCaretVisible();
}


// GetText
const char* MEditCtrl::GetText()
{
	m_strText = MLocale::ConvUTF16ToAnsi( m_strString.c_str());
	return m_strText.c_str();
}


// GetCurLine
size_t MEditCtrl::GetCurLine() const
{
	size_t _size = m_vLineInfo.size();
	for ( size_t i = 0;  i < _size;  i++)
	{
		size_t _line_st = m_vLineInfo[ i].first;
		size_t _line_ed = m_vLineInfo[ i].second;

		if ( m_nCaret >= _line_st  &&  m_nCaret <= _line_ed)
			return i;
	}

	return 0;
}


// SetSel
void MEditCtrl::SetSel( size_t nStartSel, size_t nEndSel)
{
	m_nStartSel = nStartSel;
	m_nEndSel = nEndSel;
}


// GetSel
void MEditCtrl::GetSel( size_t& nStartSel, size_t& nEndSel)
{
	if ( m_nStartSel > m_nEndSel)
	{
		nEndSel = m_nStartSel;
		nStartSel = m_nEndSel;
		return;
	}

	nStartSel = m_nStartSel;
	nEndSel = m_nEndSel;
}


// ReleaseSelect
void MEditCtrl::ReleaseSelect()
{
	m_nStartSel = m_nCaret;
	m_nEndSel = m_nCaret;
}


// Insert
void MEditCtrl::Insert( const char* str)
{
	wstring _wcs = MLocale::ConvAnsiToUTF16( str);
	Insert( _wcs);
}

void MEditCtrl::Insert( const wstring& str)
{
	m_strString.insert( m_nCaret, str);

	m_nCaret += str.length();
	m_nStartSel = m_nCaret;
	m_nEndSel = m_nCaret;

	UpdateLineInfo();
	UpdateScrollBar();
	EnsureCaretVisible();
}


// ReplaceSel
void MEditCtrl::ReplaceSel( const char* str)
{
	wstring _wcs = MLocale::ConvAnsiToUTF16( str);
	ReplaceSel( _wcs);
}

void MEditCtrl::ReplaceSel( const wstring& str)
{
	if ( IsSelected() == false)		return;

	size_t st, ed;
	GetSel( st, ed);

	m_strString.replace( st, ed - st, str);

	m_nCaret = st + str.length();
	m_nStartSel = m_nCaret;
	m_nEndSel = m_nCaret;

	UpdateLineInfo();
	UpdateScrollBar();
	EnsureCaretVisible();
}


// Clear
void MEditCtrl::Clear()
{
	m_strString.clear();
	m_nCaret = 0;
	m_nStartSel = 0;
	m_nEndSel = 0;

	UpdateLineInfo();
	UpdateScrollBar();
	EnsureCaretVisible();
}


// ScrollPage
void MEditCtrl::ScrollPage( int delta)
{
	float fDelta = (float)delta * 0.3f;
	if ( abs( (int)fDelta) < 1.0f)		fDelta = ( delta < 0.0f)  ?  -1.0f : 1.0f;

	int _value = m_pScrollBar->GetScrollValue() + (int)fDelta;
	m_pScrollBar->SetScrollValue( _value);
}


// CaretFromPoint
int MEditCtrl::CaretFromPoint( const MPOINT& pt, bool bFindStartCaret)
{
	if ( m_strString.empty() == true)
		return -1;

	MPOINT _pos = pt;
	MRECT _r = GetClientRect();
	_r.w -= 10;
	MFont* pFont = HasLookAndFill() ? m_pLookAndFill->GetFont() : MFontManager::Get( NULL);
	int _x = _r.x;
	int _h = pFont->GetHeight() + 2;
	int _line = m_pScrollBar->GetScrollValue() + ( _pos.y / _h);
	if ( _line >= (int)m_vLineInfo.size())	return m_strString.length();
	else if ( _line < 0)					return 0;

	size_t _line_st = m_vLineInfo[ _line].first;
	size_t _line_ed = m_vLineInfo[ _line].second;
	if ( _pos.x < _r.x)
		return _line_st;

	for ( size_t i = _line_st;  i < _line_ed;  i++)
	{
		wstring _tmpw;
		string _tmpa;

		_tmpw = m_strString.substr( _line_st, i - _line_st);
		_tmpa = MLocale::ConvUTF16ToAnsi( _tmpw.c_str());
		int _cx = pFont->GetWidth( _tmpa.c_str());

		_tmpw = m_strString.substr( i, 1);
		_tmpa = MLocale::ConvUTF16ToAnsi( _tmpw.c_str());
		int _cw = pFont->GetWidth( _tmpa.c_str());

		if ( _pos.x >= _cx  &&  _pos.x <= (_cx + _cw) )
			return i;
	}

	return _line_ed;
}


// UpdateLineInfo
void MEditCtrl::UpdateLineInfo()
{
	m_vLineInfo.clear();

	if ( m_strString.empty() == true)		return;


	MRECT _r = GetClientRect();
	_r.w -= 10;
	MFont* pFont = HasLookAndFill() ? m_pLookAndFill->GetFont() : MFontManager::Get( NULL);
	int _x = _r.x;

	wstring _line_str;
	size_t _line_st = 0, _line_ed = 0;
	size_t _len = m_strString.length();
	for ( size_t i = 0;  i <= _len;  )
	{
		bool _complete = true;

		if ( i < _len)
		{
			wchar_t _c = m_strString.at( i);
			if ( _c == L'\n')
			{
				_line_ed = i;
				i++;
			}
			else
			{
				wstring _tmp;
				_tmp += _line_str;
				_tmp += _c;

				string _mbs = MLocale::ConvUTF16ToAnsi( _tmp.c_str());
				int _mbs_w = pFont->GetWidth( _mbs.c_str());
				if ( _mbs_w < _r.w)
				{
					_line_str += _c;
					_line_ed = i;

					i++;
					_complete = false;
				}
			}
		}
		else
		{
			_line_ed = i;
			i++;
		}


		if ( _complete == true)
		{
			m_vLineInfo.push_back( pair< size_t, size_t>( _line_st, _line_ed));

			_x = _r.x;

			_line_str.clear();
			_line_st = i;
			_line_ed = i;
		}
	}
}


// _get_line_info
void MEditCtrl::_get_line_info( LINEINFO* pLineInfo)
{
	if ( pLineInfo == NULL)
		return;

	for ( int i = 0;  i < 3;  i++)
	{
		( pLineInfo + i)->str.c_str();
		( pLineInfo + i)->st = -1;
		( pLineInfo + i)->ed = -1;
	}

	if ( m_strString.empty() == true)
		return;


	MRECT _r = GetClientRect();
	_r.w -= 10;
	int _x = _r.x;
	MFont* pFont = HasLookAndFill() ? m_pLookAndFill->GetFont() : MFontManager::Get( NULL);


	wstring _line_str;
	size_t _line_st = 0, _line_ed = 0;
	size_t _len = m_strString.length();
	for ( size_t i = 0;  i <= _len;  )
	{
		bool _complete = true;

		if ( i < _len)
		{
			wchar_t _c = m_strString.at( i);
			if ( _c == L'\n')
			{
				_line_ed = i;
				i++;
			}
			else
			{
				wstring _tmp;
				_tmp += _line_str;
				_tmp += _c;

				string _mbs = MLocale::ConvUTF16ToAnsi( _tmp.c_str());
				int _mbs_w = pFont->GetWidth( _mbs.c_str());
				if ( _mbs_w < _r.w)
				{
					_line_str += _c;
					_line_ed = i;

					i++;
					_complete = false;
				}
			}
		}
		else
		{
			_line_ed = i;
			i++;
		}


		if ( _complete == true)
		{
			int _pos = LINE_CUR;
			if ( _line_ed < m_nCaret)			_pos = LINE_PREV;
			else if ( _line_st > m_nCaret)		_pos = LINE_NEXT;

			( pLineInfo + _pos)->str = _line_str;
			( pLineInfo + _pos)->st = (int)_line_st;
			( pLineInfo + _pos)->ed = (int)_line_ed;

			if ( _pos == LINE_NEXT)
				break;


			_x = _r.x;

			_line_str.clear();
			_line_st = i;
			_line_ed = i;
		}
	}
}


// OnEvent
bool MEditCtrl::OnEvent(MEvent* pEvent, MListener* pListener)
{
	if ( IsFocus() == false)							return false;
	if ( MWidget::OnEvent( pEvent, pListener) == true)	return true;

	switch ( pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN:
		{
			int nPos = CaretFromPoint( pEvent->Pos, true);
			if ( nPos >= 0)
			{
				m_nCaret = nPos;

				ReleaseSelect();

				EnsureCaretVisible();
			}

			break;
		}

	case MWM_MOUSEMOVE:
		{
			if ( m_bLButtonDown == true)
			{
				int pos = CaretFromPoint( pEvent->Pos, false);
				if ( pos >= 0)
				{
					m_nCaret = pos;
					m_nEndSel = pos;
				}
			}

			break;
		}

	case MWM_MOUSEWHEEL :
		ScrollPage( -(int)( (float)(pEvent->nDelta * 0.1f)));
		return true;

	case MWM_KEYUP:
		return true;

	case MWM_KEYDOWN:
		{
			if ( IsEditable()  &&  pEvent->nKey == VK_LEFT)
			{
				if ( m_nCaret > 0)
				{
					m_nCaret--;
					if ( pEvent->bShift)				m_nEndSel--;
					else								ReleaseSelect();
				}
				else if ( pEvent->bShift == false)		ReleaseSelect();

				EnsureCaretVisible();
			}
			else if ( IsEditable()  &&  pEvent->nKey == VK_RIGHT)
			{
				if ( m_nCaret < m_strString.length())
				{
					m_nCaret++;
					if ( pEvent->bShift)				m_nEndSel++;
					else								ReleaseSelect();
				}
				else if ( pEvent->bShift == false)		ReleaseSelect();

				EnsureCaretVisible();
			}
 			else if ( IsEditable()  &&  pEvent->nKey == VK_UP)
 			{
				size_t nCurLine = GetCurLine();
				if ( nCurLine > 0)
				{
					size_t _pos = m_vLineInfo[ nCurLine - 1].first + ( m_nCaret - m_vLineInfo[ nCurLine].first);
					if ( _pos > m_vLineInfo[ nCurLine - 1].second)		_pos = m_vLineInfo[ nCurLine - 1].second;
					m_nCaret = _pos;
					m_nEndSel = _pos;

					if ( pEvent->bShift == false)		ReleaseSelect();

					EnsureCaretVisible();
				}
 			}
 			else if ( IsEditable()  &&  pEvent->nKey == VK_DOWN)
 			{
				size_t nCurLine = GetCurLine();
				if ( (int)nCurLine < ( (int)m_vLineInfo.size() - 1))
				{
					size_t _pos = m_vLineInfo[ nCurLine + 1].first + ( m_nCaret - m_vLineInfo[ nCurLine].first);
					if ( _pos > m_vLineInfo[ nCurLine + 1].second)		_pos = m_vLineInfo[ nCurLine + 1].second;
					m_nCaret = _pos;
					m_nEndSel = _pos;

					if ( pEvent->bShift == false)		ReleaseSelect();

					EnsureCaretVisible();
				}
 			}
			else if ( IsEditable()  &&  pEvent->nKey == VK_HOME)
			{
				size_t nCurLine = GetCurLine();
				size_t _pos = m_vLineInfo[ nCurLine].first;
				if ( m_nCaret != _pos)
				{
					m_nCaret = _pos;
					m_nEndSel = _pos;

					if ( pEvent->bShift == false)		ReleaseSelect();

					EnsureCaretVisible();
				}
			}
			else if ( IsEditable()  &&  pEvent->nKey == VK_END)
			{
				size_t nCurLine = GetCurLine();
				size_t _pos = m_vLineInfo[ nCurLine].second;
				if ( m_nCaret != _pos)
				{
					m_nCaret = _pos;
					m_nEndSel = _pos;

					if ( pEvent->bShift == false)		ReleaseSelect();

					EnsureCaretVisible();
				}
			}

			// Delete
			else if ( IsEditable()  &&  pEvent->nKey == VK_DELETE)
			{
				if ( IsSelected()  ||  m_nCaret < m_strString.length())
				{
					if ( IsSelected() == false)		SetSel( m_nCaret, m_nCaret + 1);
					ReplaceSel( "");

					OnValueChanged();
				}
			}
			else if ( IsEditable()  &&  pEvent->nKey == VK_BACK)
			{
				if ( IsSelected()  ||  m_nCaret > 0)
				{
					if ( IsSelected() == false)		SetSel( m_nCaret, m_nCaret - 1);
					ReplaceSel( "");

					OnValueChanged();
				}
			}

			// Select all
			else if ( pEvent->nKey == 'A'  &&  pEvent->bCtrl == true)
			{
				m_nCaret = m_strString.length();
				SetSel( 0, m_nCaret);
			}

			// Copy
			else if ( pEvent->nKey == 'C'  &&  pEvent->bCtrl == true)
			{
				string _mbs;
				if ( IsSelected() == true)
				{
					size_t st, ed;
					GetSel( st, ed);

					wstring _wcs = m_strString;
					_wcs.erase( 0, st);
					_wcs.erase( ed - st, _wcs.length() - (ed - st));

					_mbs = MLocale::ConvUTF16ToAnsi( _wcs.c_str());
				}
				SetClipboard( _mbs.c_str());
			}

			// Paste
			else if ( IsEditable()  &&  pEvent->nKey == 'V'  &&  pEvent->bCtrl == true)
			{
				char buff[ 256];
				GetClipboard( buff, 256);

				if ( buff[ 0] != 0)
				{
					if ( IsSelected())		ReplaceSel( buff);
					else					Insert( buff);

					OnValueChanged();
				}
			}

			return true;
		}

	case MWM_CHAR:
		{
			if ( IsEditable() == false)					return true;

			char nKey = pEvent->nKey;
			if ( nKey == VK_RETURN)						nKey = '\n';
			else if ( nKey < 32  ||  nKey > 127)		return true;

			char buff[ 2] = { nKey, '\0' };
			if ( IsSelected() == true)		ReplaceSel( buff);
			else							Insert( buff);


			OnValueChanged();

			return true;
		}

	case MWM_IMECONVERSION:
		{
			if ( IsSelected())		ReplaceSel( pEvent->szIMECompositionResultString);
			else					Insert( pEvent->szIMECompositionResultString);

			OnValueChanged();

			return true;
		}

	case MWM_IMECOMPOSE:
		{
			string _mbs( pEvent->szIMECompositionString);
			wstring _wcs = MLocale::ConvAnsiToUTF16( _mbs.c_str());
			if ( IsSelected())
			{
				size_t st, ed;
				GetSel( st, ed);

				m_strString.replace( st, ed - st, _wcs);

				m_nCaret = st;
				SetSel( st, st + _wcs.length());
			}
			else
			{
				size_t _pos = m_nCaret;

				m_strString.insert( m_nCaret, _wcs);

				m_nCaret = _pos;
				SetSel( _pos, _pos + _wcs.length());
			}

			UpdateLineInfo();
			UpdateScrollBar();
			EnsureCaretVisible();

			OnValueChanged();

			return true;
		}
	}

	return false;
}


// OnRun
void MEditCtrl::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);

	if ( m_bUpdateScrollBar == true)
	{
		int _count = 0;

		MRECT _r = GetClientRect();
		_r.w -= 10;
		MFont* pFont = HasLookAndFill() ? m_pLookAndFill->GetFont() : MFontManager::Get( NULL);
		if ( m_strString.empty() == false)
		{
			int _x = _r.x;
			wstring _line_str;
			size_t _len = m_strString.length();
			for ( size_t i = 0;  i <= _len;  )
			{
				bool _complete = true;

				if ( i < _len)
				{
					wchar_t _c = m_strString.at( i);
					if ( _c == L'\n')
						i++;
					else
					{
						wstring _tmp;
						_tmp += _line_str;
						_tmp += _c;

						string _mbs = MLocale::ConvUTF16ToAnsi( _tmp.c_str());
						int _mbs_w = pFont->GetWidth( _mbs.c_str());
						if ( _mbs_w < _r.w)
						{
							i++;
							_line_str += _c;
							_complete = false;
						}
					}
				}
				else
					i++;

				if ( _complete == true)
				{
					_count++;
					_x = _r.x;
					_line_str.clear();
				}
			}
		}

		int _view = _r.h / ( pFont->GetHeight() + 2);
		if ( _count <= _view)		m_pScrollBar->SetScrollRange( 0, 0);
		else						m_pScrollBar->SetScrollRange( 0, _count - _view);

		m_bUpdateScrollBar = false;
		m_bEnsureCaretVisible = true;
	}


	if ( m_bEnsureCaretVisible == true)
	{
		int _line = 0;
		MRECT _r = GetClientRect();
		_r.w -= 10;
		MFont* pFont = HasLookAndFill() ? m_pLookAndFill->GetFont() : MFontManager::Get( NULL);
		if ( m_strString.empty() == false)
		{
			int _x = _r.x;
			wstring _line_str;
			size_t _line_st = 0;
			size_t _len = m_strString.length();
			for ( size_t i = 0;  i <= _len;  )
			{
				bool _complete = true;

				if ( i < _len)
				{
					wchar_t _c = m_strString.at( i);
					if ( _c == L'\n')
						i++;
					else
					{
						wstring _tmp;
						_tmp += _line_str;
						_tmp += _c;

						string _mbs = MLocale::ConvUTF16ToAnsi( _tmp.c_str());
						int _mbs_w = pFont->GetWidth( _mbs.c_str());
						if ( _mbs_w < _r.w)
						{
							i++;
							_line_str += _c;
							_complete = false;
						}
					}
				}
				else
					i++;

				if ( _complete == true)
				{
					if ( m_nCaret >= _line_st  &&  m_nCaret < i)
						break;

					_line++;

					_x = _r.x;
					_line_str.clear();
					_line_st = i;
				}
			}
		}


		int _min, _max, _val;
		int _view = _r.h / (pFont->GetHeight() + 2);
		m_pScrollBar->GetScrollRange( &_min, &_max);
		_val = m_pScrollBar->GetScrollValue();
		if ( _line < _val)						_val = _line;
		else if ( _line >= ( _val + _view))		_val = _line - _view + 1;
		m_pScrollBar->SetScrollValue( _val);

		m_bEnsureCaretVisible = false;
	}
}


// OnSetFocus
void MEditCtrl::OnSetFocus()
{
	MWidget::OnSetFocus();

	Mint::GetInstance()->EnableIME( true);
	Mint::GetInstance()->EnableCharInput( true);
}


// OnKillFocus
void MEditCtrl::OnKillFocus()
{
	MWidget::OnKillFocus();

	Mint::GetInstance()->EnableIME( false);
	Mint::GetInstance()->EnableCharInput( false);
}


// OnSize
void MEditCtrl::OnSize( int w, int h)
{
	MWidget::OnSize( w, h);

	UpdateLineInfo();
	UpdateScrollBar();
	EnsureCaretVisible();
}


// glueGetSel
MWLua::ret_int2	MEditCtrl::glueGetSel()
{
	return ( (m_nStartSel > m_nEndSel)  ?  MWLua::ret_int2( (int)m_nEndSel, (int)m_nStartSel)  :  MWLua::ret_int2( (int)m_nStartSel, (int)m_nEndSel) );
}










// MEditCtrlLook
MEditCtrlLook::MEditCtrlLook()
{
	SetWorkRectOffset( MRECT( 5, 5, -10, -10));
}

void MEditCtrlLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	OnDrawText( (MEditCtrl*)pWidget, pDC);
}

void MEditCtrlLook::OnDrawText( MEditCtrl* pEdit, MDrawContext* pDC)
{
	const char* state = pEdit->GetState();
	int state_n = pEdit->GetStateNum();

	// Set font
	MFont* pFont = GetFont();
	pDC->SetFont( pFont);

	// Set font color
	MCOLOR _col_text = GetFontColor( MWP_BORDER, state, MColorTable::TEXT[ state_n]);


	MRECT _r = pEdit->GetClientRect();
	_r.w -= 10;
	int _x = _r.x;
	int _y = _r.y - pEdit->GetScrollBar()->GetScrollValue() * ( pFont->GetHeight() + 2);

	size_t st, ed;
	pEdit->GetSel( st, ed);



	// Draw text
	wstring m_strString = MLocale::ConvAnsiToUTF16( pEdit->GetText());
	wstring _line_str;
	size_t _line_st = 0, _line_ed = 0;
	size_t _len = m_strString.length();
	for ( size_t i = 0;  i <= _len;  )
	{
		bool _complete = true;

		if ( i < _len)
		{
			wchar_t _c = m_strString.at( i);
			if ( _c == L'\n')
			{
				_line_ed = i;
				i++;
			}
			else
			{
				wstring _tmp;
				_tmp += _line_str;
				_tmp += _c;

				string _mbs = MLocale::ConvUTF16ToAnsi( _tmp.c_str());
				int _mbs_w = pFont->GetWidth( _mbs.c_str());
				if ( _mbs_w < _r.w)
				{
					_line_str += _c;
					_line_ed = i;

					i++;
					_complete = false;
				}
			}
		}
		else
		{
			_line_ed = i;
			i++;
		}


		// Draw line
		if ( _complete == true)
		{
			if ( _y >= 0)
			{
				size_t _caret = pEdit->GetCaretPos();
				if ( _caret >= _line_st  &&  _caret <= _line_ed)
				{
					wstring _tmpw = _line_str.substr( 0, _caret - _line_st);
					string _tmpa = MLocale::ConvUTF16ToAnsi( _tmpw.c_str());
					int _caret_x = pFont->GetWidth( _tmpa.c_str());

					if ( pEdit->IsFocus()  &&  pEdit->IsSelected() == false  &&  (timeGetTime() % MEDITCTRLBLINK_TIME) > (MEDITCTRLBLINK_TIME >> 1))
					{
						if ( Mint::GetInstance()->IsNativeIME())
						{
							_tmpw = _line_str.substr( _caret - _line_st, 1);
							_tmpa = MLocale::ConvUTF16ToAnsi( _tmpw.c_str());
							OnDrawIMECaret( pEdit, MRECT( _caret_x, _y, max( 10, pFont->GetWidth( _tmpa.c_str())), pFont->GetHeight() + 2), pDC);
						}
						else
							OnDrawCaret( pEdit, MRECT( _caret_x, _y, pFont->GetHeight(), pFont->GetHeight() + 2), pDC);
					}

					// Set candidate position
					MPOINT cp = MClientToScreen( pEdit, MPOINT( _caret_x, _y));
					Mint::GetInstance()->SetCandidateListPosition( cp, _r.h);
				}


				size_t _sel_st = max( st, _line_st);
				size_t _sel_ed = min( ed, i);

				// Selected text
				if ( pEdit->IsSelected()  &&  pEdit->IsFocus()  &&  _sel_st < _sel_ed)
				{
					wstring _tmpw = _line_str.substr( 0, _sel_st - _line_st);
					string _tmpa = MLocale::ConvUTF16ToAnsi( _tmpw.c_str());
					pDC->SetColor( _col_text);
					pDC->Text( _x, _y + 1, _tmpa.c_str());
					_x += pFont->GetWidth( _tmpa.c_str());


					_tmpw = _line_str.substr( _sel_st - _line_st, _sel_ed - _sel_st);
					_tmpa = MLocale::ConvUTF16ToAnsi( _tmpw.c_str());
					int _w = pFont->GetWidth( _tmpa.c_str());
					OnDrawSelectbar( pEdit, MRECT( _x, _y, _w, pFont->GetHeight() + 2), pDC);

					pDC->SetColor( GetFontColor( MWP_SELECTBAR, state, MColorTable::TEXT_SELECTED));
					pDC->Text( _x, _y + 1, _tmpa.c_str());
					_x += _w;


					_tmpw = _line_str.substr( min( _line_str.length(), _sel_ed - _line_st), _line_str.length());
					_tmpa = MLocale::ConvUTF16ToAnsi( _tmpw.c_str());
					pDC->SetColor( _col_text);
					pDC->Text( _x, _y + 1, _tmpa.c_str());
				}

				// Normal text
				else
				{
					string _mbs = MLocale::ConvUTF16ToAnsi( _line_str.c_str());
					pDC->SetColor( _col_text);
					pDC->Text( _x, _y + 1, _mbs.c_str());
				}
			}

			_x = _r.x;
			_y += pFont->GetHeight() + 2;
			if ( _y >= _r.h)
				break;

			_line_str.clear();
			_line_st = i;
			_line_ed = i;
		}
	}
}

void MEditCtrlLook::OnDrawCaret( MEditCtrl* pEdit, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_CARET, pEdit->GetState()) == false)
	{
		// Draw default
		MCOLOR _color = pDC->SetColor( MColorTable::TEXT[ pEdit->GetStateNum()]);
		pDC->Line( r.x, r.y - 1, r.x, r.y + r.h + 2);
		MCOLOR _col = pDC->GetColor();
		_col.a = 128;
		pDC->SetColor( _col);
		pDC->Line( r.x + 1, r.y - 1, r.x + 1, r.y + r.h + 2);
		pDC->SetColor( _color);
	}
}

void MEditCtrlLook::OnDrawIMECaret( MEditCtrl* pEdit, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_IMECARET, pEdit->GetState()) == false)
	{
		// Draw default
		MCOLOR _color = pDC->SetColor( MColorTable::TEXT[ pEdit->GetStateNum()]);
		pDC->FillRectangle( r.x, r.y + r.h - 2, r.w, 2);
		pDC->SetColor( _color);
	}
}

void MEditCtrlLook::OnDrawSelectbar( MEditCtrl* pEdit, MRECT& r, MDrawContext* pDC)
{
	if ( r.w <= 0)
		return;

	if ( Draw( pDC, r, MWP_SELECTBAR, pEdit->GetState()) == false)
	{
		// Draw default
		MCOLOR _color = pDC->SetColor( MColorTable::SELECTBAR[ pEdit->GetStateNum()]);
		pDC->FillRectangle( r);
		pDC->SetColor( _color);
	}
}


} // namespace mint3
