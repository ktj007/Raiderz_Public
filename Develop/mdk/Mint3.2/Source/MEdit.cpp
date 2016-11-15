#include "stdafx.h"
#include "MEdit.h"
#include "Mint.h"
#include "MColorTable.h"
#include "MLocale.h"

namespace mint3
{


#define MAX_HISTORYCOUNT		10

// 패스워드 치환 스트링
static string g_strPasswordChar = "●";



// MEdit
MEdit::MEdit( const char* szName, MWidget* pParent, MListener* pListener)
	: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( MEditLook);

	m_nStartPos = 0;
	m_nCaret = 0;
	m_nStartSel = 0;
	m_nEndSel = 0;
	m_bEditable = true;
	m_bNumber = false;
	m_bLowerCase = false;
	m_bUpperCase = false;
	m_bPassword = false;
	m_nLimitText = 0;
	m_bEnableHistory = false;
	m_nHistoryIndex = 0;
	m_bSelectAllOnFocus = true;
	m_bEnsureCaretVisible = false;

	SetEnableFocus(true);
}


// ~MEdit
MEdit::~MEdit()
{
	m_strString.clear();
	m_History.clear();
}


// SetText
void MEdit::SetText( const char* szText)
{
	MWidget::SetText( szText);

	string _mbs( szText);
	m_strString = MLocale::ConvAnsiToUTF16( _mbs.c_str());

	size_t _len = m_strString.length();
	m_nCaret = min( m_nCaret, _len);
	m_nStartSel = min( m_nStartSel, _len);
	m_nEndSel = min( m_nEndSel, _len);
	m_nStartPos = min( m_nStartPos, _len);

	ReleaseSelect();
}


// GetText
const char* MEdit::GetText()
{
	m_strText = MLocale::ConvUTF16ToAnsi( m_strString.c_str());
	return m_strText.c_str();
}


// SetSel
void MEdit::SetSel( size_t nStartSel, size_t nEndSel)
{
	m_nStartSel = nStartSel;
	m_nEndSel = nEndSel;

	if ( m_bSelectAllOnFocus == true)
		m_bSelectAllOnFocus = false;
}


// GetSel
void MEdit::GetSel( size_t& nStartSel, size_t& nEndSel)
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
void MEdit::ReleaseSelect()
{
	m_nStartSel = m_nCaret;
	m_nEndSel = m_nCaret;
}


// Insert
void MEdit::Insert( const char* str)
{
	wstring _wcs = MLocale::ConvAnsiToUTF16( str);
	m_strString.insert( m_nCaret, _wcs);

	if ( m_bNumber == true)
	{
		int num = _wtoi(m_strString.c_str());

		wchar_t pNum[64] = {0,};
		_itow(num, pNum, 10);
		wstring strString = pNum;
		if (strString != m_strString)
		{
			m_nCaret = 0;
		}
		m_strString = strString;
	}

	m_nCaret += _wcs.length();
	m_nStartSel = m_nCaret;
	m_nEndSel = m_nCaret;
}

void MEdit::Insert( const wstring& str)
{
	m_strString.insert( m_nCaret, str);

	if ( m_bNumber == true)
	{
		int num = _wtoi(m_strString.c_str());

		wchar_t pNum[2] = {0,};
		_itow(num, pNum, 10);
		wstring strString = pNum;
		if (strString != m_strString)
		{
			m_nCaret = 0;
		}
		m_strString = strString;
	}

	m_nCaret += str.length();
	m_nStartSel = m_nCaret;
	m_nEndSel = m_nCaret;
}


// ReplaceSel
void MEdit::ReplaceSel( const char* str)
{
	if ( IsSelected() == false)		return;

	size_t st, ed;
	GetSel( st, ed);

	wstring _wcs = MLocale::ConvAnsiToUTF16( str);
	m_strString.replace( st, ed - st, _wcs);

	if ( m_bNumber == true)
	{
		int num = _wtoi(m_strString.c_str());

		wchar_t pNum[2] = {0,};
		_itow(num, pNum, 10);
		m_strString = pNum;
	}

	m_nCaret = st + _wcs.length();
	m_nStartSel = m_nCaret;
	m_nEndSel = m_nCaret;
}

void MEdit::ReplaceSel( const wstring& str)
{
	if ( IsSelected() == false)		return;

	size_t st, ed;
	GetSel( st, ed);

	m_strString.replace( st, ed - st, str);

	if ( m_bNumber == true)
	{
		int num = _wtoi(m_strString.c_str());

		wchar_t pNum[2] = {0,};
		_itow(num, pNum, 10);
		m_strString = pNum;
	}

	m_nCaret = st + str.length();
	m_nStartSel = m_nCaret;
	m_nEndSel = m_nCaret;
}


// Clear
void MEdit::Clear()
{
	m_strString.clear();
	m_nStartPos = 0;
	m_nCaret = 0;
	m_nStartSel = 0;
	m_nEndSel = 0;
}


// CaretFromPoint
int MEdit::CaretFromPoint( const MPOINT& pt, bool bFindStartCaret)
{
	MPOINT _pos = pt;
	int x = 0;
	MRECT r = GetClientRect();
	MFont* pFont = HasLookAndFill() ? m_pLookAndFill->GetFont() : MFontManager::Get( NULL);

	size_t _len = m_strString.length();
	for ( size_t i = m_nStartPos;  i < _len;  i++)
	{
		wchar_t _wc = m_strString.at( i);
		wstring _wcs;
		_wcs += _wc;

		string _mbs = m_bPassword ? g_strPasswordChar : MLocale::ConvUTF16ToAnsi( _wcs.c_str());
		int _w = pFont->GetWidth( _mbs.c_str());

		if ( (x + _w) > _pos.x)
		{
			if ( bFindStartCaret == true)	return i;

			if ( (x + (_w >> 1)) > _pos.x)	return i;
			else							return (i + 1);
		}

		x += _w;
	}

	return _len;
}


// SetPasswordChar
void MEdit::SetPasswordChar( const char* szChar)
{
	if ( szChar[ 0] != 0)
		g_strPasswordChar = szChar;
}


// GetPasswordChar
const string& MEdit::GetPasswordChar() const
{
	return g_strPasswordChar;
}


// PushHistory
void MEdit::PushHistory()
{
	string strText = GetText();
	if ( strText.empty())	return;

	m_History.push_back( strText);

	if ( m_History.size() > MAX_HISTORYCOUNT)
		m_History.erase( m_History.begin());

	m_nHistoryIndex = m_History.size();
}


// OnEvent
bool MEdit::OnEvent(MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)	return true;

	switch ( pEvent->nMessage)
	{
	case MWM_KEYUP:
		{
			return true;
		}
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
			else if ( IsEditable()  &&  pEvent->nKey == VK_UP  &&  m_bEnableHistory == true)
			{
				if ( m_bEnableHistory == true  &&  m_History.empty() == false)
				{
					if ( m_nHistoryIndex == 0)
					{
						m_nHistoryIndex = m_History.size();
						Clear();
					}
					else
					{
						m_nHistoryIndex--;
						SetText( m_History[ m_nHistoryIndex].c_str());
					}

					m_nCaret = m_strString.length();
					SetSel( 0, m_nCaret);
					EnsureCaretVisible();
				}
			}
			else if ( IsEditable()  &&  pEvent->nKey == VK_DOWN  &&  m_bEnableHistory == true)
			{
				if ( m_bEnableHistory == true  &&  m_History.empty() == false)
				{
					if ( m_nHistoryIndex == m_History.size())
					{
						m_nHistoryIndex = 0;
						SetText( m_History[ m_nHistoryIndex].c_str());
					}
					else
					{
						m_nHistoryIndex++;

						if ( m_nHistoryIndex == m_History.size())	Clear();
						else										SetText( m_History[ m_nHistoryIndex].c_str());
					}

					m_nCaret = m_strString.length();
					SetSel( 0, m_nCaret);
					EnsureCaretVisible();
				}
			}
			else if ( IsEditable()  &&  pEvent->nKey == VK_HOME)
			{
				m_nCaret = 0;
				if ( pEvent->bShift)		m_nEndSel = m_nCaret;
				else						ReleaseSelect();
				EnsureCaretVisible();
			}
			else if ( IsEditable()  &&  pEvent->nKey == VK_END)
			{
				m_nCaret = m_strString.length();
				if ( pEvent->bShift)		m_nEndSel = m_nCaret;
				else						ReleaseSelect();
				EnsureCaretVisible();
			}

			// Delete
			else if ( IsEditable()  &&  pEvent->nKey == VK_DELETE)
			{
				if ( IsSelected()  ||  m_nCaret < m_strString.length())
				{
					if ( IsSelected() == false)		SetSel( m_nCaret, m_nCaret + 1);
					ReplaceSel( "");
					EnsureCaretVisible();

					OnValueChanged();
				}
			}
			else if ( IsEditable()  &&  pEvent->nKey == VK_BACK)
			{
				if ( IsSelected()  ||  m_nCaret > 0)
				{
					if ( IsSelected() == false)		SetSel( m_nCaret, m_nCaret - 1);
					ReplaceSel( "");
					EnsureCaretVisible();

					OnValueChanged();
				}
			}

			// Select all
			else if ( pEvent->nKey == 'A'  &&  pEvent->bCtrl == true)
			{
				m_nCaret = m_strString.length();
				SetSel( 0, m_nCaret);
				EnsureCaretVisible();
			}

			// Copy
			else if ( pEvent->nKey == 'C'  &&  pEvent->bCtrl == true  &&  m_bPassword == false)
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
			else if ( IsEditable()  &&  pEvent->nKey == 'V'  &&  pEvent->bCtrl == true  &&  m_bPassword == false)
			{
				char buff[ 256];
				GetClipboard( buff, 256);

				if ( buff[ 0] != 0)
				{
					if ( IsSelected())		ReplaceSel( buff);
					else					Insert( buff);
					EnsureCaretVisible();

					OnValueChanged();
				}
			}

			return true;
		}

	case MWM_CHAR:
		{
			if ( IsEditable() == false  ||  ( m_nLimitText > 0  &&  m_strString.length() >= m_nLimitText) )
			{
				ReleaseSelect();
				return true;
			}

			char nKey = pEvent->nKey;
			if ( nKey < 32  ||  nKey > 127)					return true;

			if ( m_bNumber == true)
			{
				if ( nKey < '0'  ||  nKey > '9')			return true;
			}
			else if ( m_bLowerCase == true)
			{
				if ( nKey >= 'A'  &&  nKey <= 'Z')			nKey += 'a' - 'A';
			}
			else if ( m_bUpperCase == true)
			{
				if ( nKey >= 'a'  &&  nKey <= 'z')			nKey += 'A' - 'a';
			}

			char buff[ 2] = { nKey, '\0' };
			if ( IsSelected() == true)		ReplaceSel( buff);
			else							Insert( buff);


			EnsureCaretVisible();

			OnValueChanged();

			return true;
		}

	case MWM_IMECONVERSION:
		{
			if ( IsEditable() == false  ||  ( m_nLimitText > 0  &&  m_strString.length() >= m_nLimitText) )
			{
				ReleaseSelect();
				return true;
			}

			if ( IsSelected())		ReplaceSel( pEvent->szIMECompositionResultString);
			else					Insert( pEvent->szIMECompositionResultString);

			EnsureCaretVisible();

			OnValueChanged();

			return true;
		}

	case MWM_IMECOMPOSE:
		{
			if ( IsEditable() == false  ||  ( m_nLimitText > 0  &&  m_strString.length() >= m_nLimitText) )
			{
				MGetMint()->ClearImeCompose();

				ReleaseSelect();
				return true;
			}

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

			EnsureCaretVisible();

			OnValueChanged();

			return true;
		}

	case MWM_LBUTTONDOWN:
		{
			if ( m_bSelectAllOnFocus == true)
			{
				m_nCaret = m_strString.length();
				SetSel( 0, m_nCaret);

				m_bSelectAllOnFocus = false;

				m_bLButtonDown = false;
			}
			else
			{
				int nPos = CaretFromPoint( pEvent->Pos, true);
				if ( nPos >= 0)
				{
					m_nCaret = nPos;

					ReleaseSelect();

					EnsureCaretVisible();
				}
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

					EnsureCaretVisible();
				}
			}

			break;
		}
	}

	return false;
}


// OnRun
void MEdit::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);

	// Ensure caret visible
	if ( m_bEnsureCaretVisible == true)
	{
		m_bEnsureCaretVisible = false;

		if ( m_nCaret <= m_nStartPos)
		{
			m_nStartPos = m_nCaret;

			if ( m_nStartPos > 0)	m_nStartPos--;
		}
		else if ( GetRect().w < 20)
		{
			m_nStartPos = m_nCaret;
		}
		else
		{
			MFont* pFont = HasLookAndFill() ? GetLookAndFill()->GetFont() : MFontManager::Get( NULL);
			MRECT r = GetClientRect();
			r.w = ( r.w > 100)  ?  r.w - pFont->GetHeight()  :  r.w - 5;
			int width = 0;

			size_t _size = m_strString.length();
			for ( size_t i = (m_nCaret - 1);  i >= 0;  i--)
			{
				wchar_t _wc = m_strString.at( i);
				wstring _wcs;
				_wcs += _wc;

				string _mbs = m_bPassword ? g_strPasswordChar : MLocale::ConvUTF16ToAnsi( _wcs.c_str());
				width += pFont->GetWidth( _mbs.c_str());

				if ( width > r.w)
				{
					m_nStartPos = i + 1;
					return;
				}
				else if ( i == m_nStartPos)		return;
			}

			m_nStartPos = 0;
		}
	}
}


// OnSetFocus
void MEdit::OnSetFocus()
{
	MWidget::OnSetFocus();

	if ( m_bNumber == false)
		Mint::GetInstance()->EnableIME( true);

	Mint::GetInstance()->EnableCharInput( true);

	m_bSelectAllOnFocus = true;
}


// OnKillFocus
void MEdit::OnKillFocus()
{
	MWidget::OnKillFocus();

	Mint::GetInstance()->EnableIME( false);
	Mint::GetInstance()->EnableCharInput( false);
}


// OnSize
void MEdit::OnSize( int w, int h)
{
	MWidget::OnSize( w, h);

	EnsureCaretVisible();
}


// OnSetCursor
bool MEdit::OnSetCursor( MEvent* pEvent, string* pCursor)
{
	if ( IsEnabled() == true  &&  m_bStatic == false)
		*pCursor = MCURSOR_IBEAM;

	return MWidget::OnSetCursor( pEvent, pCursor);
}


// glueGetSel
MWLua::ret_int2	MEdit::glueGetSel()
{
	return ( (m_nStartSel > m_nEndSel)  ?  MWLua::ret_int2( (int)m_nEndSel, (int)m_nStartSel)  :  MWLua::ret_int2( (int)m_nStartSel, (int)m_nEndSel) );
}










// MEditLook
MEditLook::MEditLook()
{
	SetWorkRectOffset( MRECT( 5, 5, -10, -10));
}

void MEditLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	OnDrawText( (MEdit*)pWidget, pDC);
}

void MEditLook::OnDrawText( MEdit* pEdit, MDrawContext* pDC)
{
	const char* state = pEdit->GetState();
	int state_n = pEdit->GetStateNum();

	size_t st, ed;
	pEdit->GetSel( st, ed);

	int x = 0, y = 0;
	MRECT r = pEdit->GetClientRect();
	bool _sel = false;
	string _sel_str;
	int _sel_width = 0;


	// Set font
	MFont* pFont = GetFont();
	pDC->SetFont( pFont);

	// Set font color
	pDC->SetColor( GetFontColor( MWP_BORDER, state, MColorTable::TEXT[ state_n]));


	// Draw text
	size_t _size = pEdit->GetLength();
	for ( size_t i = pEdit->GetStartPos();  i <= _size;  i++)
	{
		// Get a character
		string _mbs;
		if ( i >= _size)					_mbs = "";
		else if ( pEdit->IsPassword())		_mbs = pEdit->GetPasswordChar();
		else
		{
			wchar_t _wc = pEdit->GetWChar( i);
			wstring _wcs;
			_wcs += _wc;
			_mbs = MLocale::ConvUTF16ToAnsi( _wcs.c_str());
		}

		// Get width of character
		int _w = pFont->GetWidth( _mbs.c_str());


		// Check out of rect
		if ( (x + _w) > r.w)
		{
			i = _size + 1;		// End of string
			_mbs = "";
			_w = 0;
		}


		// Draw caret marker
		if ( i == pEdit->GetCaretPos()  &&  pEdit->IsEditable())
		{
			if ( pEdit->IsFocus()  &&  pEdit->IsSelected() == false  &&  (timeGetTime() % MEDITBLINK_TIME) > (MEDITBLINK_TIME >> 1))
			{
				if ( Mint::GetInstance()->IsNativeIME())
					OnDrawIMECaret( pEdit, MRECT( x, y, (_w ? max( _w, 3) : 10), pFont->GetHeight()), pDC);
				else
					OnDrawCaret( pEdit, MRECT( x, y, pFont->GetHeight(), pFont->GetHeight()), pDC);
			}


			// Set candidate position
			MPOINT cp = MClientToScreen( pEdit, MPOINT( x, 0));
			Mint::GetInstance()->SetCandidateListPosition( cp, r.h);
		}


		// Check selected region
		if ( pEdit->IsSelected()  &&  pEdit->IsFocus()  &&  (i >= st  &&  i < ed))
		{
			_sel = true;
			_sel_str += _mbs;
			_sel_width += _w;

			x += _w;
		}
		else 
		{
			// Draw select region
			if ( _sel == true  &&  _sel_width > 0)
			{
				int _x = x - _sel_width;
				int _y = y;

				// Draw select bar
				OnDrawSelectbar( pEdit, MRECT( _x, _y, _sel_width, pFont->GetHeight() + 2), pDC);

				// Draw selected text
				MCOLOR _color = pDC->SetColor( GetFontColor( MWP_SELECTBAR, state, MColorTable::TEXT_SELECTED));
				pDC->Text( _x, _y, _sel_str.c_str());
				pDC->SetColor( _color);

				_sel = false;
			}

			// Draw normal text
			pDC->Text( x, y, _mbs.c_str());
			x += _w;
		}


		// Check end of string
		if ( i > _size)		break;
	}
}

void MEditLook::OnDrawCaret( MEdit* pEdit, MRECT& r, MDrawContext* pDC)
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

void MEditLook::OnDrawIMECaret( MEdit* pEdit, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_IMECARET, pEdit->GetState()) == false)
	{
		// Draw default
		MCOLOR _color = pDC->SetColor( MColorTable::TEXT[ pEdit->GetStateNum()]);
		pDC->FillRectangle( r.x, r.y + r.h - 2, r.w, 2);
		pDC->SetColor( _color);
	}
}

void MEditLook::OnDrawSelectbar( MEdit* pEdit, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_SELECTBAR, pEdit->GetState()) == false)
	{
		// Draw default
		MCOLOR _color = pDC->SetColor( MColorTable::SELECTBAR[ pEdit->GetStateNum()]);
		pDC->FillRectangle( r);
		pDC->SetColor( _color);
	}
}


} // namespace mint3
