#include "stdafx.h"
#include "MTextView.h"


#define _tok_tag_open			'{'							// 태그 열기 문자 정의
#define _tok_tag_close			'}'							// 태그 닫기 문자 정의
#define DEFAULT_ALIGN			(MAM_JUSTIFY | MAM_TOP)		// 기본 정렬 값
#define INDENT_WIDTH			10							// 들여쓰기 간격
#define MIN_WIDTH				100							// 위젯의 최소 넓이
#define AUTOSCROLL_DELAY		3000						// 자동 스크롤 시작 후 잠시 정지하는 시간



namespace mint3
{


// MLineContens
MLineContents::MLineContents()
{
	m_nCount = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nAlign = MAM_NOTALIGN;
	m_nSpaceCount = 0;
	m_nLastSpace = 0;
	m_nIndent = 0;
	m_nTrimHead = 0;
	m_nTrimTail = 0;
	m_bHyphen = false;
}


// MLineContens
MLineContents::~MLineContents()
{
	m_LineContents.clear();
}


// AddContents
bool MLineContents::AddContents( __chunk* pContents)
{
	if ( pContents == NULL  ||  m_nCount >= 0xFF)
		return false;

	m_LineContents.push_back( pContents);
	m_nCount++;
	m_nWidth += pContents->_getwidth();
	m_nHeight = max( m_nHeight, pContents->_getheight());
	

	// 스트링이나 비트맵 chunk는 스페이스를 갖고 있을 수 있으므로 개수를 카운트 한다
	if ( pContents->_gettype() == _CT_STRING  ||  pContents->_gettype() == _CT_BITMAP  ||  pContents->_gettype() == _CT_SPACE)
	{
		// 마지막 chunk는 전체 스페이스 개수에서 빼야하므로 항상 마지막인지 검사해야한다
		if ( pContents->_getspace() > 0)
		{
			m_nSpaceCount++;
			m_nLastSpace = pContents->_getspace();
		}
		else
		{
			m_nLastSpace = 0;
		}
	}

	return true;
}

bool MLineContents::AddTrimHeadContents( __chunk* pContents, int trim_pos, int trim_width)
{
	if ( pContents == NULL  ||  m_nCount >= 0xFF  ||  trim_pos >= (int)pContents->_getlength()  ||  trim_width <= 0  ||  pContents->_gettype() != _CT_STRING)
		return false;

	m_LineContents.push_back( pContents);
	m_nCount++;
	m_nWidth += trim_width;
	m_nHeight = max( m_nHeight, pContents->_getheight());
	m_nLastSpace = 0;
	m_nTrimHead = trim_pos;

	// 마지막 chunk는 전체 스페이스 개수에서 빼야하므로 항상 마지막인지 검사해야한다
	if ( pContents->_getspace() > 0)
	{
		m_nSpaceCount++;
		m_nLastSpace = pContents->_getspace();
	}
	else
	{
		m_nLastSpace = 0;
	}

	return true;
}

bool MLineContents::AddTrimTailContents( __chunk* pContents, int trim_pos, int trim_width, bool hyphen)
{
	if ( pContents == NULL  ||  m_nCount >= 0xFF  ||  trim_pos <= 0  ||  trim_width <= 0  ||  pContents->_gettype() != _CT_STRING)
		return false;

	m_LineContents.push_back( pContents);
	m_nCount++;
	m_nWidth += trim_width;
	m_nHeight = max( m_nHeight, pContents->_getheight());
	m_nLastSpace = 0;
	m_nTrimTail = trim_pos;
	m_bHyphen = hyphen;

	return true;
}


// GetContents
__chunk* MLineContents::GetContents( int nIndex)
{
	return m_LineContents[ nIndex];
}


// GetCount
int MLineContents::GetCount()
{
	return (int)m_nCount;
}


// GetWidth
int MLineContents::GetWidth()
{

	return ((int)m_nWidth - (int)m_nLastSpace);
}


// GetWholeWidth
int MLineContents::GetWholeWidth()
{
	return (int)m_nWidth;
}


// SetHeight
void MLineContents::SetHeight( int height)
{
	m_nHeight = (unsigned char)height;
}


// GetCount
int MLineContents::GetHeight()
{
	return (int)m_nHeight;
}


// SetAlign
void MLineContents::SetAlign( unsigned char align)
{
	m_nAlign = align;
}


// GetAlign
unsigned char MLineContents::GetAlign()
{
	return m_nAlign;
}


// GetSpaceCount
int MLineContents::GetSpaceCount()
{
	return ( (m_nLastSpace == 0) ? m_nSpaceCount : (m_nSpaceCount - 1));
}


// GetTrim
int MLineContents::GetTrimHead()
{
	return m_nTrimHead;
}

int MLineContents::GetTrimTail()
{
	return m_nTrimTail;
}


// SetIndent
void MLineContents::SetIndent( int indent)
{
	m_nIndent = (unsigned char)indent;
}


// GetIndent
int MLineContents::GetIndent()
{
	return m_nIndent;
}


// IsHyphen
bool MLineContents::IsHyphen()
{
	return m_bHyphen;
}


// Clear
void MLineContents::Clear()
{
	m_nCount = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nAlign = MAM_NOTALIGN;
	m_nSpaceCount = 0;
	m_nLastSpace = 0;
	m_nTrimHead = 0;
	m_nTrimTail = 0;
	m_nIndent = 0;

	m_LineContents.clear();
}









// MTextView
MTextView::MTextView( const char* szName, MWidget* pParent, MListener* pListener) : MWidget( szName, pParent, pListener)
{
	m_pScrollbar = new MScrollBar( this, this);
	m_pScrollbar->Show( false);
	m_pScrollbar->SetRange( 0, 0);
	m_pScrollbar->SetValue( 0);

	m_nScrollbarVisible = MSB_AUTO;

	m_bWrap = true;

	m_PageSize = MSIZE( m_Rect.w, m_Rect.h);

	m_bAutoScroll = false;
	m_dwAutoScroll = 0;


	SetFocusEnable( true);
	SetClipByParent( true);
}


// ~MTextView
MTextView::~MTextView()
{
	Clear();
}


// OnEvent
bool MTextView::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)
		return true;


	if ( pEvent->nMessage == MWM_KEYDOWN)
	{
		switch ( pEvent->nKey)
		{
		case VK_UP :		ScrollPage( -10);							return true;
		case VK_DOWN :		ScrollPage( 10);							return true;
		case VK_PRIOR :		ScrollPage( -(GetClientRect().h >> 1));		return true;
		case VK_NEXT :		ScrollPage( GetClientRect().h >> 1);		return true;
		case VK_HOME :		ScrollPage( -m_pScrollbar->GetMax());		return true;
		case VK_END :		ScrollPage( m_pScrollbar->GetMax());		return true;
		}
	}

	else if ( pEvent->nMessage == MWM_MOUSEWHEEL)
	{
		if ( GetClientRect().InPoint( pEvent->Pos))
		{
			ScrollPage( -(int)( (float)(pEvent->nDelta * 0.1f)));
			return true;
		}
	}

	return false;
}


// OnCommand
bool MTextView::OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	if ( pWidget == m_pScrollbar  &&  nMsg == MEVENT_LIST_VALUE_CHANGED)
	{
		OnScrollbarChanged( m_pScrollbar->GetValue());
		return true;
	}

	return false;
}


// OnRun
void MTextView::OnRun( void)
{
	// 자동 스크롤 설정
	if ( m_bAutoScroll == true)
	{
		DWORD dwElapsed = timeGetTime() - m_dwAutoScroll;
		if ( dwElapsed > AUTOSCROLL_DELAY)
		{
			int _pos = (int)( (double)(dwElapsed - AUTOSCROLL_DELAY) * 0.012 /* 스크롤 되는 속도 조절 */);
			if ( _pos >= m_pScrollbar->GetMax())
			{
				_pos = m_pScrollbar->GetMax();

				StopAutoScroll();
			}

			m_pScrollbar->SetValue( _pos);
		}
	}
}


#define		EQ_ALIGN(align,comp)		( ((align & comp) == comp) ? true : false)

// OnDraw
void MTextView::OnDraw( MDrawContext* pDC)
{
	MRECT			r = GetWorkRect();
	float			x = 0.0f;
	int				y = -m_pScrollbar->GetValue();
	float			justify = 0.0f;
	MFont*			pOldFont = GetFont();
	MCOLOR			nOldColor = MCOLOR( 255, 255, 255);


	// client edge
#ifdef _DEBUG
	// 텍스트가 "show rect"이면 윈도우 영역을 표시한다
	if ( m_strText == string( "show rect"))
	{
		pDC->SetColor( 255, 255, 255, (int)(25.0 * sin( (double)timeGetTime() * 0.005) + 25.0));
		pDC->Rectangle( 0, 0, GetRect().w, GetRect().h);
		return;
	}
#endif


	// 처음 폰트와 색상 설정
	pDC->SetFont( GetFont());
	pDC->SetColor( 255, 255, 255);


	//  라인 출력
	size_t _size = m_LineContents.size();
	for ( size_t i = 0;  i < _size;  i++)
	{
		MLineContents* pContents = m_LineContents[ i];
		bool bInner = true;			// 해당 라인이 화면 안에 있는지 여부


		// 영역을 벗어나는지 검사
		if ( (y + pContents->GetHeight()) < 0)		bInner = false;
		else if ( y > r.h)							break;


		// Align
		x = (float)pContents->GetIndent();
		justify = 0.0f;
		if ( bInner)
		{
			if (      EQ_ALIGN( pContents->GetAlign(), MAM_JUSTIFY))	justify = (float)( r.w - pContents->GetIndent() - pContents->GetWidth()) / (float)pContents->GetSpaceCount();
			else if ( EQ_ALIGN( pContents->GetAlign(), MAM_HCENTER))	x = (float)( r.w - pContents->GetWidth()) * 0.5f;
			else if ( EQ_ALIGN( pContents->GetAlign(), MAM_RIGHT))		x = (float)( r.w - pContents->GetWidth());
		}


		// Draw line contents
		for ( int c = 0;  c < pContents->GetCount();  c++)
		{

			switch ( pContents->GetContents( c)->_gettype())
			{
			case _CT_STRING :
				{
					if ( bInner == false)	continue;

					_str_chunk* str_chunk = (_str_chunk*) pContents->GetContents( c);

					// Head trim이 있는지 확인
					if ( pContents->GetTrimHead()  &&  c == 0)
					{
						if (      EQ_ALIGN( pContents->GetAlign(), MAM_VCENTER))	pDC->Text( (int)x, y + ((pContents->GetHeight() - str_chunk->_getheight()) >> 1), str_chunk->_str.c_str() + pContents->GetTrimHead());
						else if ( EQ_ALIGN( pContents->GetAlign(), MAM_BOTTOM))		pDC->Text( (int)x, y + pContents->GetHeight() - str_chunk->_getheight(), str_chunk->_str.c_str() + pContents->GetTrimHead());
						else														pDC->Text( (int)x, y, str_chunk->_str.c_str() + pContents->GetTrimHead());

						x -= (int)pDC->GetFont()->GetWidth( str_chunk->_str.c_str(), pContents->GetTrimHead());
					}

					// Tail trim이 있는지 확인
					else if ( pContents->GetTrimTail()  &&  c == (pContents->GetCount() - 1))
					{
						char szBuff[ 256];
						strncpy_s( szBuff, str_chunk->_str.c_str(), pContents->GetTrimTail());

						if (      EQ_ALIGN( pContents->GetAlign(), MAM_VCENTER))	pDC->Text( (int)x, y + ((pContents->GetHeight() - str_chunk->_getheight()) >> 1), szBuff);
						else if ( EQ_ALIGN( pContents->GetAlign(), MAM_BOTTOM))		pDC->Text( (int)x, y + pContents->GetHeight() - str_chunk->_getheight(), szBuff);
						else														pDC->Text( (int)x, y, szBuff);

						if ( pContents->IsHyphen() == true)
							pDC->Text( (int)x + pDC->GetFont()->GetWidth( szBuff), y, "-");
					}

					// Trim 없음
					else
					{
						if (      EQ_ALIGN( pContents->GetAlign(), MAM_VCENTER))	pDC->Text( (int)x, y + ((pContents->GetHeight() - str_chunk->_getheight()) >> 1), str_chunk->_str.c_str());
						else if ( EQ_ALIGN( pContents->GetAlign(), MAM_BOTTOM))		pDC->Text( (int)x, y + pContents->GetHeight() - str_chunk->_getheight(), str_chunk->_str.c_str());
						else														pDC->Text( (int)x, y, str_chunk->_str.c_str());
					}

					break;
				}

			case _CT_BITMAP :
				{
					if ( bInner == false)	continue;

					_bmp_chunk* bmp_chunk = (_bmp_chunk*) pContents->GetContents( c);
					pDC->SetBitmap( MBitmapManager::Get( bmp_chunk->_bmp.c_str()));
					if (      EQ_ALIGN( pContents->GetAlign(), MAM_VCENTER))	pDC->Draw( (int)x, y + ((pContents->GetHeight() - bmp_chunk->_getheight()) >> 1), bmp_chunk->_w, bmp_chunk->_h);
					else if ( EQ_ALIGN( pContents->GetAlign(), MAM_BOTTOM))		pDC->Draw( (int)x, y + pContents->GetHeight() - bmp_chunk->_getheight(), bmp_chunk->_w, bmp_chunk->_h);
					else														pDC->Draw( (int)x, y, bmp_chunk->_w, bmp_chunk->_h);

					break;
				}

			case _CT_SPACE :
					break;

			case _CT_FONT :
				{
					pOldFont = pDC->GetFont();
					_fnt_chunk* fnt_chunk = (_fnt_chunk*) pContents->GetContents( c);
					pDC->SetFont( MFontManager::Get( fnt_chunk->_fnt.c_str()) );
					continue;
				}

			case _CT_RFONT :
				pDC->SetFont( pOldFont);
				continue;

			case _CT_COLOR :
				{
					nOldColor = pDC->GetColor();
					_col_chunk* col_chunk = (_col_chunk*) pContents->GetContents( c);
					pDC->SetColor( col_chunk->_r, col_chunk->_g, col_chunk->_b, col_chunk->_a);
					continue;
				}

			case _CT_RCOLOR :
				pDC->SetColor( nOldColor);
				continue;

			default :
				continue;
			}

			x += (float)pContents->GetContents( c)->_getwidth() + ((pContents->GetContents( c)->_getspace() > 0) ? justify : 0.0f);
		}

		y += pContents->GetHeight();
	}
}


// OnSize
void MTextView::OnSize(int w, int h)
{
	MWidget::OnSize( w, h);


	// 넓이가 바뀐 경우는 무조건 라인 업데이트
	if ( m_PageSize.w != w)
		UpdateLineContents();

	// 스크롤바가 자동으로 보여지는 상태에서 높이가 변한다면...
	else if ( m_nScrollbarVisible == MSB_AUTO  &&  m_PageSize.h != h)
	{
		// 스크롤바가 안보이는데 높이가 페이지 크기보다 줄어든경우...
		if ( m_pScrollbar->IsVisible() == false  &&  h < m_PageSize.h)
			UpdateLineContents();

		// 스크롤바가 보이는데 높이가 페이지 크기보다 늘어난경우...
		else if ( m_pScrollbar->IsVisible() == true  &&  h >= m_PageSize.h)
			UpdateLineContents();
	}

	// 그 외에는 라인 업데이트를 할 필요가 없음
	else
	{
	}
}


// OnScrollbarChanged
void MTextView::OnScrollbarChanged( int nPos)
{
	StopAutoScroll();
}


// SetText
void MTextView::SetText( const char* szText)
{
	Clear();

	if ( ParseText( szText) == true)
		UpdateLineContents();
//	else
//		Clear();
}

// GetText
const char* MTextView::GetText( void)
{
	return m_strText.c_str();
}


// Scrollbar
void MTextView::ShowScrollbar( int mode)
{
	if ( m_nScrollbarVisible == mode)
		return;

	m_nScrollbarVisible = mode;

	switch ( mode)
	{
	case MSB_HIDE :
	case MSB_AUTO :
		m_pScrollbar->Show( false);
		break;

	case MSB_SHOW :
		m_pScrollbar->Show( true);
		break;
	}

	UpdateLineContents();
}

void MTextView::SetScrollbarLook( MScrollBarLook* pLook)
{
	m_pScrollbar->ChangeCustomLook( pLook);
}

void MTextView::SetScrollbarWidth( int nWidth)
{
	m_pScrollbar->SetWidth( nWidth);

	if ( m_nScrollbarVisible == MSB_AUTO  ||  m_nScrollbarVisible == MSB_SHOW)
		UpdateLineContents();
}




// ParseText
// 텍스트를 입력받으면 스트링과 태그를 파싱하여 chunk로 저장하는 함수
bool MTextView::ParseText( const char* c)
{
#define _add_str()		{	if (_len) {											\
							_buff[_len]='\0';									\
							m_strText+=_buff;									\
							_len=0;												\
							_str_chunk* chunk = new _str_chunk( _buff);			\
							m_ChunkList.push_back( chunk);						\
						} }

#define	IsMultiByteChar(c)		((IsDBCSLeadByte(*c)==TRUE) ? true : false)


	// 2바이트 문자 때문에 캐릭터 단위로 비교를 해줘야 함. 일반적인 스트링 관련 API 함수는 사용 금지.
	char	_buff[ 256];
	size_t	_len = 0;
	while ( 1)
	{
		if ( *c == '\0')							// 종료 검사
		{
			_add_str();
			break;
		}

		else if ( IsMultiByteChar( c))				// 2바이트 문자 검사
		{
			_buff[ _len++] = *c++;
			if ( *c == '\0')  return false;			// error

			_buff[ _len++] = *c++;
		}

		else if ( *c == '\n')						// Line feed 검사
		{
			*c++;
			_add_str();

			_lnf_chunk* chunk = new _lnf_chunk;
			m_ChunkList.push_back( chunk);
		}

		else if ( *c == VK_SPACE)					// 스페이스 문자 검사
		{
			_buff[ _len++] = *c++;
			_add_str();
		}

		else if ( *c == _tok_tag_open)				// 파싱
		{
			_add_str();

			if ( _parse_tag( c) == false)  return false;	// error

			c = strchr( c, _tok_tag_close) + 1;
		}

		else if ( *c == _tok_tag_close)				// 파싱 오류
		{
			return false;
		}

		else										// 일반 캐릭터
		{
			_buff[ _len++] = *c++;
		}

		// 버퍼가 200자를 넘어가면 스트링에 복사함
		if ( _len > 200)
			_add_str();
	}


	// 스트링 chunk에 대해서 크기와 스페이스의 크기를 구한다
	MFont* pFont = GetFont();
	MFont* pOldFont = pFont;
	size_t size = m_ChunkList.size();

	for ( size_t i = 0;  i < size;  i++)
	{
		switch ( m_ChunkList[ i]->_gettype())
		{
		case _CT_FONT :
			{
				pOldFont = pFont;
				_fnt_chunk* _font_cunk = (_fnt_chunk*)m_ChunkList[ i];
				pFont = MFontManager::Get( _font_cunk->_fnt.c_str());
				continue;
			}
			break;

		case _CT_RFONT :
			pFont = pOldFont;
			break;

		case _CT_STRING :
			{
				_str_chunk* str_chunk = (_str_chunk*)m_ChunkList[ i];

				// 스트링 크기를 구함
				str_chunk->_w = pFont->GetWidth( str_chunk->_str.c_str());
				str_chunk->_h = pFont->GetHeight();

				// 스트링의 마지막이 스페이스로 끝나는지 검사해서 스페이스의 크기를 구함
				if ( str_chunk->_str.at( max( 0, str_chunk->_getlength() - 1)) == VK_SPACE)
					str_chunk->_space = pFont->GetWidth( " ");
			}
			break;

		default :
			break;
		}
	}

	return true;
}


#define _trim_eq( ch)		{ while(*c==ch) { if (*c==_tok_tag_close||*c=='\0') return -1;  *c++;  read++; } }
#define _trim_neq( ch)		{ while(*c!=ch) { if (*c==_tok_tag_close||*c=='\0') return -1;  *c++;  read++; }  *c++;  read++; }


// Get tag
int _get_tag( const char* c, string& tag)
{
	int read = 0;
	_trim_neq( _tok_tag_open);
	_trim_eq( VK_SPACE);

	if ( *c == '/')
	{
		tag += *c++;
		read++;
		_trim_eq( VK_SPACE);

		while ( *c != VK_SPACE  &&  *c != _tok_tag_close)
		{
			if (  *c == '\0')  return -1;
			tag += *c++;
			read++;
		}
	}

	else
	{
		while ( *c != VK_SPACE)
		{
			if ( *c == _tok_tag_close)		return read;
			else if ( *c == '\0')			return -1;
			tag += *c++;
			read++;
		}
	}

	return read;
}


// Get attribute and value
int _get_attr_value( const char* c, string& attr, string& value)
{
	int read = 0;
	_trim_eq( VK_SPACE);

	while ( 1)
	{
		if ( *c == _tok_tag_close || *c == '\0')	return -1;
		else if ( *c == VK_SPACE)	break;
		else if ( *c == '=')		break;
		attr += *c++;
		read++;
	}

	_trim_neq( '=');
	_trim_eq( VK_SPACE);

	if ( *c == '\"')
	{
		*c++;
		read++;

		while ( *c != '\"')
		{
			if ( *c == '\0')		return -1;
			value += *c++;
			read++;
		}

		*c++;
		read++;
	}
	else
	{
		while ( *c != VK_SPACE  &&  *c != _tok_tag_close)
		{
			if ( *c == '\0')		return -1;
			value += *c++;
			read++;
		}
	}

	return read;
}


bool MTextView::_parse_tag( const char* c)
{
	int read;
	string tag;
	if ( (read = _get_tag( c, tag)) < 0)	return false;
	c += read;

	if ( tag == "BITMAP")		return _parse_bmp( c);
	else if ( tag == "SPACE")	return _parse_spc( c);
	else if ( tag == "FONT")	return _parse_fnt( c);
	else if ( tag == "/FONT")	return _parse_rfn( c);
	else if ( tag == "COLOR")	return _parse_col( c);
	else if ( tag == "/COLOR")	return _parse_rco( c);
	else if ( tag == "ALIGN")	return _parse_aln( c);
	else if ( tag == "COLUMN")	return _parse_hco( c);
	else if ( tag == "CR")		return _parse_crt( c);
	else if ( tag == "INDENT")	return _parse_idt( c);
	else if ( tag == "OUTDENT")	return _parse_odt( c);
	else if ( tag == "/INDENT")	return _parse_cdt( c);

	// Skip tag
	while ( *c != _tok_tag_close)
	{
		if ( *c=='\0') return false;
		*c++;
	}
	*c++;

	return true;
}


bool MTextView::_parse_bmp( const char* c)
{
	string bitmap;
	int w = 10,  h = 10;
	while ( 1)
	{
		int read;
		string attr, value;
		if ( (read = _get_attr_value( c, attr, value)) < 0) break;
		c += read;

		if ( attr == string( "name"))		bitmap = value;
		else if ( attr == string( "w"))		w = atoi( value.c_str());
		else if ( attr == string( "h"))		h = atoi( value.c_str());
	}

	_bmp_chunk* chunk = new _bmp_chunk( bitmap, w, h);
	m_ChunkList.push_back( chunk);
	return true;
}


bool MTextView::_parse_spc( const char* c)
{
	int w = 0,  h = 0;
	while ( 1)
	{
		int read;
		string attr, value;
		if ( (read = _get_attr_value( c, attr, value)) < 0) break;
		c += read;

		if ( attr == string( "w"))			w = atoi( value.c_str());
		else if ( attr == string( "h"))		h = atoi( value.c_str());
	}

	_spc_chunk* chunk = new _spc_chunk( w, h);
	m_ChunkList.push_back( chunk);
	return true;
}


bool MTextView::_parse_fnt( const char* c)
{
	string font;
	while ( 1)
	{
		int read;
		string attr, value;
		if ( (read = _get_attr_value( c, attr, value)) < 0) break;
		c += read;

		if ( attr == string( "name"))		font = value;
	}

	_fnt_chunk* chunk = new _fnt_chunk( font);
	m_ChunkList.push_back( chunk);
	return true;
}


bool MTextView::_parse_rfn( const char* c)
{
	_rfn_chunk* chunk = new _rfn_chunk();
	m_ChunkList.push_back( chunk);
	return true;
}


bool MTextView::_parse_col( const char* c)
{
	unsigned char r =255, g =255, b =255, a =255;
	while ( 1)
	{
		int read;
		string attr, value;
		if ( (read = _get_attr_value( c, attr, value)) < 0) break;
		c += read;

		if ( attr == string( "r"))			r = atoi( value.c_str());
		else if ( attr == string( "g"))		g = atoi( value.c_str());
		else if ( attr == string( "b"))		b = atoi( value.c_str());
		else if ( attr == string( "a"))		a = atoi( value.c_str());
	}

	_col_chunk* chunk = new _col_chunk( r, g, b, a);
	m_ChunkList.push_back( chunk);
	return true;
}


bool MTextView::_parse_rco( const char* c)
{
	_rco_chunk* chunk = new _rco_chunk();
	m_ChunkList.push_back( chunk);
	return true;
}


bool MTextView::_parse_aln( const char* c)
{
	unsigned char align = 0;
	while ( 1)
	{
		int read;
		string attr, value;
		if ( (read = _get_attr_value( c, attr, value)) < 0) break;
		c += read;

		if ( attr == string( "hor"))
		{
			if ( value == string( "justify"))		align |= MAM_JUSTIFY;
			else if ( value == string( "center"))	align |= MAM_HCENTER;
			else if ( value == string( "hcenter"))	align |= MAM_HCENTER;
			else if ( value == string( "right"))	align |= MAM_RIGHT;
			else									align |= MAM_LEFT;
		}
		else if ( attr == string( "ver"))
		{
			if ( value == string( "center"))		align |= MAM_VCENTER;
			if ( value == string( "vcenter"))		align |= MAM_VCENTER;
			else if ( value == string( "bottom"))	align |= MAM_BOTTOM;
			else									align |= MAM_TOP;
		}
	}

	_aln_chunk* chunk = new _aln_chunk( align);
	m_ChunkList.push_back( chunk);
	return true;
}


bool MTextView::_parse_hco( const char* c)
{
	unsigned char height = 0;
	while ( 1)
	{
		int read;
		string attr, value;
		if ( (read = _get_attr_value( c, attr, value)) < 0) break;
		c += read;

		if ( attr == string( "height"))
		{
			if ( value == string( "auto"))	height = 0;
			else							height = atoi( value.c_str());
		}
	}

	_hco_chunk* chunk = new _hco_chunk( height);
	m_ChunkList.push_back( chunk);
	return true;
}

bool MTextView::_parse_crt( const char* c)
{
	_crt_chunk* chunk = new _crt_chunk();
	m_ChunkList.push_back( chunk);
	return true;
}

bool MTextView::_parse_idt( const char* c)
{
	_idt_chunk* chunk = new _idt_chunk();
	m_ChunkList.push_back( chunk);
	return true;
}


bool MTextView::_parse_odt( const char* c)
{
	_odt_chunk* chunk = new _odt_chunk();
	m_ChunkList.push_back( chunk);
	return true;
}


bool MTextView::_parse_cdt( const char* c)
{
	_cdt_chunk* chunk = new _cdt_chunk();
	m_ChunkList.push_back( chunk);
	return true;
}


// 분리된 chunk를 출력하기 편하도록 라인 단위로 재구성하는 함수. 리턴값으로 화면 영역을 넘어가는지 여부를 넘겨줌
bool MTextView::UpdateLineContents( bool bTestMode)
{
	StopAutoScroll();


#define	ADD_LINECONTENTS(linefeed)		{																									\
											if ( column)																					\
												pCurrentLine->SetHeight( column + (linefeed ? (column >> 1) : 0));							\
											else if ( pCurrentLine->GetHeight() == 0)														\
												pCurrentLine->SetHeight( pFont->GetHeight() + (linefeed ? (pFont->GetHeight() >> 1) : 0));	\
											else if ( linefeed)																				\
												pCurrentLine->SetHeight( pCurrentLine->GetHeight() + (pFont->GetHeight() >> 1));			\
											height += pCurrentLine->GetHeight();						\
											if ( bTestMode) {											\
												pCurrentLine->Clear();									\
												if ( height > r.h) {									\
													if ( pCurrentLine)	delete pCurrentLine;			\
													return true;										\
												}														\
											}															\
											else {														\
												m_LineContents.push_back( pCurrentLine);				\
												pCurrentLine = new MLineContents();						\
											}															\
											pCurrentLine->SetIndent( indent);							\
										}


	// 자동으로 스크롤바를 표시하는 경우, 먼저 테스트로 정렬해봐서 높이가 클라이언트 영역을 벗어나는지 확인한다.
	if ( m_nScrollbarVisible == MSB_AUTO  &&  bTestMode == false)
	{
		if ( UpdateLineContents( true) == true)		m_pScrollbar->Show( true);
		else										m_pScrollbar->Show( false);
	}


	// chunk가 비어있으면 그냥 리턴
	if ( m_ChunkList.empty())	return false;


	// 테스트 모드가 아니면 모든 contents를 지운다
	if ( bTestMode == false)	ClearLineContents();


	// 라인 contents를 구성한다
	MRECT			r = (bTestMode ? GetClientRect() : GetWorkRect());
	int				height = 0;
	unsigned char	align = DEFAULT_ALIGN;
	MFont*			pFont = GetFont();
	MFont*			pOldFont = pFont;
	int				column = 0;
	int				indent = 0;
	MLineContents*	pCurrentLine = new MLineContents();


	size_t _size = m_ChunkList.size();
	for ( size_t i = 0;  i < _size;  i++)
	{
		switch ( m_ChunkList[ i]->_gettype())
		{
		case _CT_FONT:
			{
				pOldFont = pFont;
				_fnt_chunk* fnt_chunk = (_fnt_chunk*) m_ChunkList[ i];
				pFont = MFontManager::Get( fnt_chunk->_fnt.c_str());
				break;
			}

		case _CT_RFONT:
			pFont = pOldFont;
			break;

		case _CT_ALIGN :
			{
				_aln_chunk* aln_chunk = (_aln_chunk*) m_ChunkList[ i];
				align = aln_chunk->_align;
				continue;
			}

		case _CT_COLUMN :
			{
				_hco_chunk* hco_chunk = (_hco_chunk*) m_ChunkList[ i];
				column = hco_chunk->_height;
				continue;
			}

		case _CT_LINEFEED :
			{
				pCurrentLine->SetAlign( EQ_ALIGN( align, MAM_JUSTIFY) ? (align & 0xF4) : align);
				ADD_LINECONTENTS( true);
				continue;
			}

		case _CT_CRETURN :
			{
				pCurrentLine->SetAlign( EQ_ALIGN( align, MAM_JUSTIFY) ? (align & 0xF4) : align);
				ADD_LINECONTENTS( false);
				continue;
			}

		case _CT_INDENT :
		case _CT_OUTDENT :
		case _CT_CLRDENT :
			{
				switch ( m_ChunkList[ i]->_gettype())
				{
				case _CT_INDENT :	indent += INDENT_WIDTH;						break;
				case _CT_OUTDENT :	indent = max( 0, indent - INDENT_WIDTH);	break;
				case _CT_CLRDENT :	indent = 0;									break;
				}

				// 출력할 contents가 없으면 바로 indent를 설정한다
				if ( pCurrentLine->GetWidth() == 0  ||  pCurrentLine->GetCount() == 0)
					pCurrentLine->SetIndent( indent);

				continue;
			}

		default :
			break;
		}


		// 새 contents를 추가할 경우 작업 영역 안에 있는지 확인
		int _w = pCurrentLine->GetIndent() + pCurrentLine->GetWholeWidth() + m_ChunkList[ i]->_getwidth() - m_ChunkList[ i]->_getspace();
		if ( _w <= r.w)
		{
			// 아직 작업 영역 안이라면 라인에 추가함
			pCurrentLine->AddContents( m_ChunkList[ i]);
		}

		// 작업 영역을 벗어났는데 출력할게 아무것도 없으면...
		else if ( pCurrentLine->GetWidth() == 0  ||  pCurrentLine->GetCount() == 0)
		{
			// 현재 contents를 강제로 라인에 삽입하여 리스트에 추가하고 다음 라인으로 넘어감
			pCurrentLine->AddContents( m_ChunkList[ i]);
			pCurrentLine->SetAlign( align);
			ADD_LINECONTENTS( false);
		}

		// 그 외에 작업 영역을 벗어난 경우...
		else
		{
			// 마지막 contents가 스트링 형식이고, 라인에 남은 공간(넓이)이 유효한지 확인해서 쪼개넣기를 한다
			if ( m_bWrap == false  &&
				 m_ChunkList[ i]->_gettype() == _CT_STRING  &&
				(r.w - pCurrentLine->GetWidth()) > (pCurrentLine->GetSpaceCount() * pFont->GetWidth( " " /* space */) * 1.5))
			{
				int hyphen_width = pFont->GetWidth( "-");		// 하이픈의 크기를 매번 구하지 않도록 미리 계산해놓는다
				_w = pCurrentLine->GetIndent() + pCurrentLine->GetWholeWidth();
				int trim = 0;
				bool hyphen = false;
				const char* c = m_ChunkList[ i]->_getcdata();

				while ( 1)
				{
					_w += pFont->GetWidth( c, 1);

					// 알파벳이면...
					if ( (*c >= 'a'  &&  *c <= 'z')  ||  (*c >= 'A'  &&  *c <= 'Z'))
					{
						// 하이픈까지 추가한 넓이가 벗어나는지 검사함
						if ( (_w + hyphen_width) > r.w)
						{
							hyphen = true;
							break;
						}
						trim++;
						*c++;
					}

					// 그 외...
					else
					{
						// 현재 위치까지 추가한 넓이가 벗어나는지 검사함
						if ( _w > r.w)		break;
						else if ( IsMultiByteChar( c))
						{
							trim += 2;
							c += 2;
						}
						else
						{
							trim++;
							*c++;
						}
					}
				}

				int w_tail = pFont->GetWidth( m_ChunkList[ i]->_getcdata(), trim);
				pCurrentLine->AddTrimTailContents( m_ChunkList[ i], trim, w_tail + (hyphen ? hyphen_width : 0), hyphen);
				pCurrentLine->SetAlign( align);
				ADD_LINECONTENTS( false);

				pCurrentLine->AddTrimHeadContents( m_ChunkList[ i], trim, m_ChunkList[ i]->_getwidth() - w_tail);
			}

			// 지금까지 만든 라인을 리스트에 추가하고 다음 라인으로 넘어감
			else
			{
				pCurrentLine->SetAlign( align);
				ADD_LINECONTENTS( false);

				pCurrentLine->AddContents( m_ChunkList[ i]);
			}
		}
	}

	// 마지막으로 남은 contents를 처리함
	if ( pCurrentLine->GetCount() == 0)
	{
		delete pCurrentLine;
	}
	else
	{
		height += pCurrentLine->GetHeight();

		if ( bTestMode)
		{
			delete pCurrentLine;
		}
		else
		{
			pCurrentLine->SetIndent( indent);
			pCurrentLine->SetAlign( EQ_ALIGN( align, MAM_JUSTIFY) ? (align & 0xF4) : align);
			m_LineContents.push_back( pCurrentLine);
		}
	}


	if ( bTestMode == false)
	{
		// 스크롤바 업데이트
		UpdateScrollbar();


		// 페이지 사이즈 업데이트
		m_PageSize.w = (m_LineContents.size() > 1) ? m_Rect.w : m_LineContents[ 0]->GetWidth();		// 한줄 이상이면 클라이언트 넓이가 페이지 넓이이며, 한줄일 경우엔 첫줄의 넓이가 페이지 넓이가 됨
		m_PageSize.h = height;
	}


	return ( (height > r.h) ? true : false);
}


// 스크롤바를 업데이트 하는 함수
void MTextView::UpdateScrollbar()
{
	StopAutoScroll();


	MRECT	r = GetWorkRect();
	int		height = 0;
	size_t	_size = m_LineContents.size();
	for ( size_t i = 0;  i < _size;  i++)
		height += m_LineContents[ i]->GetHeight();

	int _pos = m_pScrollbar->GetValue();
	int _max = (height <= r.h) ? 0 : (height - r.h + 10 /* 여유있게 일정 픽셀만큼 공간을 추가한다 */); 
	m_pScrollbar->SetRange( 0, _max);
	m_pScrollbar->SetValue( min( _pos, _max));
}


// ScrollPage
void MTextView::ScrollPage( int delta)
{
	StopAutoScroll();

	int _value = m_pScrollbar->GetValue() + delta;

	if ( _value > m_pScrollbar->GetMax())	m_pScrollbar->SetValue( m_pScrollbar->GetMax());
	else if ( _value < 0)					m_pScrollbar->SetValue( 0);

	m_pScrollbar->SetValue( _value);
}


// 페이지 사이즈를 구하는 함수
MSIZE MTextView::GetPageSize() const
{
	return m_PageSize;
}


// 자동 스크롤 함수
void MTextView::StartAutoScroll()
{
	m_dwAutoScroll = timeGetTime();
	m_bAutoScroll = true;

	m_pScrollbar->SetValue( m_pScrollbar->GetMin());
}

void MTextView::StopAutoScroll()
{
	m_bAutoScroll = false;
}


// NoWrap
void MTextView::SetWrapping( bool bWrap)
{
	if ( m_bWrap == bWrap)
		return;

	m_bWrap = bWrap;

	UpdateLineContents();
}

bool MTextView::IsWrapping()
{
	return m_bWrap;
}


// Clear
// 모든 텍스트를 삭제하는 함수
void MTextView::Clear()
{
	StopAutoScroll();

	m_strText.clear();

	ClearLineContents();
	ClearChunkList();

	if ( m_nScrollbarVisible == MSB_AUTO)	m_pScrollbar->Show( false);
	m_pScrollbar->SetRange( 0, 0);
	m_pScrollbar->SetValue( 0);

	m_PageSize.w = 0;
	m_PageSize.h = 0;
}

void MTextView::ClearChunkList()
{
	size_t _size = m_ChunkList.size();
	for ( size_t i = 0;  i < _size;  i++)
		delete m_ChunkList[ i];

	m_ChunkList.clear();
}

void MTextView::ClearLineContents()
{
	size_t _size = m_LineContents.size();
	for ( size_t i = 0;  i < _size;  i++)
		delete m_LineContents[ i];

	m_LineContents.clear();
}


// GetWorkRect
MRECT MTextView::GetWorkRect()
{
	MRECT r = GetClientRect();

	if ( m_pScrollbar->IsVisible())
		r.w = max( 0, r.w - m_pScrollbar->GetWidth() - 2);

	return r;
}





// Glue functions
void MTextView::glueClear()
{
	Clear();
}

void MTextView::glueScrollPage( int delta)
{
	ScrollPage( delta);
}

int MTextView::glueGetPageWidth()
{
	return m_PageSize.w;
}

int MTextView::glueGetPageHeight()
{
	return m_PageSize.h;
}

void MTextView::glueStartAutoScroll()
{
	StartAutoScroll();
}

void MTextView::glueStopAutoScroll()
{
	StopAutoScroll();
}



} // namespace mint3
