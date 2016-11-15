#include "stdafx.h"
#include "MTextView.h"
#include "Mint.h"
#include "MColorTable.h"
#include "MLocale.h"


static wchar_t __LF[] = L"\n";			// Line feed
static wchar_t __SP[] = L" ";			// Space
static wchar_t __OT[] = L"{";			// Open tag parsing
static wchar_t __CT[] = L"}";			// Close tag parsing
static wchar_t __SL[] = L"/";			// Slash
static wchar_t __EQ[] = L"=";			// Equal marks
static wchar_t __DQ[] = L"\"";			// Double quotation marks


#define DEFAULT_ALIGN				(MAM_JUSTIFY | MAM_TOP)		// 기본 정렬 값
#define DEFAULT_INDENT				10
#define EQ_ALIGN(_align,comp)		( ((_align & comp) == comp) ? true : false)



namespace mint3
{




// MLineContens
MLineContents::MLineContents()
{
	m_nCount = 0;
	m_nWidth = 0;
	m_nColumnHeight = 0;
	m_nHeight = 0;
	m_nAlign = MAM_NOTALIGN;
	m_nSpaceCount = 0;
	m_nLastSpace = 0;
	m_nIndent = 0;
	m_fStartPos = 0.0f;
	m_fSpaceAddition = 0.0f;
}


// MLineContens
MLineContents::~MLineContents()
{
	Clear();
}


// AddContents
bool MLineContents::AddContents( __chunk* pContents)
{
	if ( pContents == NULL  ||  m_nCount >= 0xFF)
		return false;

	m_LineContents.push_back( pContents);
	m_nCount++;
	m_nWidth += pContents->_getwidth();
	m_nColumnHeight = max( m_nColumnHeight, pContents->_getheight());
	

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


// UpdateLineInfo
void MLineContents::UpdateLineInfo( MRECT& r)
{
	m_fStartPos = (float)m_nIndent;
	m_fSpaceAddition = 0.0f;
	if (      EQ_ALIGN( m_nAlign, MAM_JUSTIFY))		m_fSpaceAddition = (float)( r.w - m_nIndent - GetWidth()) / (float)( m_nSpaceCount - (m_nLastSpace > 0  ?  1 : 0));
	else if ( EQ_ALIGN( m_nAlign, MAM_HCENTER))		m_fStartPos = (float)( (r.w - GetWidth()) >> 1);
	else if ( EQ_ALIGN( m_nAlign, MAM_RIGHT))		m_fStartPos = (float)( r.w - GetWidth());


	float x = m_fStartPos;
	float w = 0.0f;
	MREFINFO* pRefInfo = NULL;
	for ( vector< __chunk*>::iterator itr = m_LineContents.begin();  itr != m_LineContents.end();  itr++)
	{
		__chunk* pChunk = (*itr);
		float _w = (float)pChunk->_getwidth() + ((pChunk->_getspace() > 0) ? m_fSpaceAddition : 0.0f);

		if ( pChunk->_getref() > 0)
		{
			if ( pRefInfo == NULL)
			{
				pRefInfo = new MREFINFO( pChunk->_getref(), x, _w);
				m_RefInfo.push_back( pRefInfo);
			}
			else if ( pRefInfo->_ref != pChunk->_getref())
			{
				pRefInfo = new MREFINFO( pChunk->_getref(), x, _w);
				m_RefInfo.push_back( pRefInfo);
			}
			else
			{
				pRefInfo->_w += _w;
			}
		}
		else
			pRefInfo = NULL;

		x += _w;
	}
}


// Clear
void MLineContents::Clear()
{
	m_nCount = 0;
	m_nWidth = 0;
	m_nColumnHeight = 0;
	m_nHeight = 0;
	m_nAlign = MAM_NOTALIGN;
	m_nSpaceCount = 0;
	m_nLastSpace = 0;
	m_nIndent = 0;
	m_fStartPos = 0.0f;
	m_fSpaceAddition = 0.0f;


	for ( vector< __chunk*>::iterator itr = m_LineContents.begin();  itr != m_LineContents.end();  itr++)
	{
		if ( (*itr)->_isdummy() == true)
			delete (*itr);
	}
	m_LineContents.clear();

	
	for ( vector< MREFINFO*>::iterator itr = m_RefInfo.begin();  itr != m_RefInfo.end();  itr++)
		delete (*itr);
	m_RefInfo.clear();
}









// MTextView
MTextView::MTextView( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( MTextViewLook);

	SetEnableFocus( true);

	m_pScrollbar = new MScrollBar( NULL, this, this, MSBS_VERT | MSBS_RIGHTALIGN);

	m_Reference.push_back( MTEXTVIEWREF( "", ""));
	m_nReferenceIndex = 0;
	m_nCaret = -1;
	m_nMouseOver = 0;
	m_bWrap = true;
	m_PageSize = MSIZE( 0, 0);
	m_bDoParseText = false;
	m_bUpdateScrollBar = false;
	m_bUpdateLineContents = false;
	m_bDragAndDrop = false;
}


// ~MTextView
MTextView::~MTextView()
{
	Clear();
}


// OnEvent
bool MTextView::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)		return true;


	switch ( pEvent->nMessage)
	{
	case MWM_KEYDOWN :
		switch ( pEvent->nKey)
		{
		case VK_UP :		ScrollPage( -10);							return true;
		case VK_DOWN :		ScrollPage( 10);							return true;
		case VK_PRIOR :		ScrollPage( -(GetClientRect().h >> 1));		return true;
		case VK_NEXT :		ScrollPage( GetClientRect().h >> 1);		return true;
		case VK_HOME :		ScrollPage( -m_pScrollbar->GetMax());		return true;
		case VK_END :		ScrollPage( m_pScrollbar->GetMax());		return true;
		}
		break;

	case MWM_MOUSEWHEEL :
		ScrollPage( -(int)( (float)(pEvent->nDelta * 0.1f)));
		return true;

	case MWM_LBUTTONDOWN :
		{
			MRefContect _ref = FindRefContent( pEvent->Pos);
			if ( _ref.i > 0)
			{
				m_nCaret = _ref.i;
				return true;
			}
			break;
		}

	case MWM_LBUTTONUP :
		{
			MRefContect _ref = FindRefContent( pEvent->Pos);
			if ( m_nCaret == _ref.i)
			{
				MWidget::m_EventArgs.m_nItemIndex = _ref.i;
				MWidget::m_EventArgs.m_ItemRect = _ref.r;
				OnItemClick( pEvent, _ref.i);
				return true;
			}
			break;
		}

	case MWM_LBUTTONDBLCLK :
		{
			MRefContect _ref = FindRefContent( pEvent->Pos);
			if ( m_nCaret == _ref.i)
			{
				MWidget::m_EventArgs.m_nItemIndex = _ref.i;
				MWidget::m_EventArgs.m_ItemRect = _ref.r;
				OnItemDblClick( pEvent, _ref.i);
				return true;
			}
			break;
		}

	case MWM_RBUTTONDOWN :
		{
			MRefContect _ref = FindRefContent( pEvent->Pos);
			if ( _ref.i > 0)
			{
				m_nCaret = _ref.i;

				MWidget::m_EventArgs.m_nItemIndex = _ref.i;
				MWidget::m_EventArgs.m_ItemRect = _ref.r;
				OnItemRClick( pEvent, _ref.i);
				return true;
			}
			break;
		}

	case MWM_MOUSEMOVE :
		{
			MRefContect _ref = FindRefContent( pEvent->Pos);
			if ( _ref.i != m_nMouseOver)
			{
				m_nMouseOver = _ref.i;
				
				m_strToolTip = m_Reference[ _ref.i].strToolTip;

				MWidget::m_EventArgs.m_nItemIndex = _ref.i;
				MWidget::m_EventArgs.m_ItemRect = _ref.r;
				MGetMint()->ChangedToolTipText( this);
			}
			break;
		}
	}

	return false;
}


// OnChangedLookAndFill
void MTextView::OnChangedLookAndFill()
{
	MWidget::OnChangedLookAndFill();

	ClearLineContents();
	ClearChunkList();

	m_pScrollbar->SetScrollRange( 0, 0);
	m_pScrollbar->SetScrollValue( 0);

	m_PageSize = MSIZE( 0, 0);

	m_nCaret = -1;
	m_nMouseOver = 0;

	m_bDoParseText = true;
	m_bUpdateLineContents = true;
}


// OnRun
void MTextView::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);

	// Parse text
	if ( m_bDoParseText == true)
	{
		m_bDoParseText = false;
		m_bUpdateLineContents = true;

		// Convert to wide-character string from multi-byte string
		wstring _wcs = MLocale::ConvAnsiToUTF16( m_strText.c_str());
		ParseText( _wcs.c_str());
	}


	// Update line contents
	if ( m_bUpdateLineContents == true)
	{
		m_bUpdateLineContents = false;
		UpdateLineContents();
	}


	// Update scrollbar
	if ( m_bUpdateScrollBar == true)
	{
		m_bUpdateScrollBar = false;

		MRECT	r = GetClientRect();
		int		height = 0;
		size_t	_size = m_LineContents.size();
		for ( size_t i = 0;  i < _size;  i++)
			height += m_LineContents[ i]->GetHeight();

		int _pos = m_pScrollbar->GetScrollValue();
		int _max = (height <= r.h) ? 0 : (height - r.h + 10 /* 여유있게 일정 픽셀만큼 공간을 추가한다 */); 
		m_pScrollbar->SetScrollRange( 0, _max);
		m_pScrollbar->SetScrollValue( min( _pos, _max));
	}
}


// OnSize
void MTextView::OnSize(int w, int h)
{
	MWidget::OnSize( w, h);

	// 넓이가 바뀐 경우는 무조건 라인 업데이트
	if ( m_PageSize.w != w)			m_bUpdateLineContents = true;

 	// 스크롤바가 자동으로 보여지는 상태에서 높이가 변한다면...
 	else if ( m_pScrollbar->IsAlwaysShow() == false  &&  m_pScrollbar->IsAlwaysHide() == false  &&  m_PageSize.h != h)
 	{
 		// 스크롤바가 안보이는데 높이가 페이지 크기보다 줄어든경우...
 		if ( m_pScrollbar->GetShow() == false  &&  h < m_PageSize.h)		m_bUpdateLineContents = true;
 
 		// 스크롤바가 보이는데 높이가 페이지 크기보다 늘어난경우...
 		else if ( m_pScrollbar->GetShow() == true  &&  h > m_PageSize.h)	m_bUpdateLineContents = true;
 	}
}


// OnToolTip
MWidget* MTextView::OnToolTip()
{
	if ( m_nMouseOver < 0  ||  m_nMouseOver >= (int)m_Reference.size())
		return NULL;

	m_strToolTip = m_Reference[ m_nMouseOver].strToolTip;

	MWidget::m_EventArgs.m_nItemIndex = m_nMouseOver;
	MWidget::m_EventArgs.m_ItemRect = GetRefRect( m_nMouseOver);

	return MWidget::OnToolTip();
}


// OnLeave
void MTextView::OnLeave()
{
	if ( m_nMouseOver > 0)		MGetMint()->GetToolTipManager()->Reset();
	m_nMouseOver = 0;

	MWidget::OnLeave();
}


// SetText
void MTextView::SetText( const char* szText)
{
	Clear();

	if ( szText == NULL  ||  szText[ 0] == 0)	return;
	m_strText = szText;

	m_bDoParseText = true;
}


// GetText
const char* MTextView::GetText( void)
{
	return m_strText.c_str();
}


// SetToolTipText
void MTextView::SetToolTipText( const char* szTooltip)
{
	m_strToolTip = szTooltip;

	// 위젯의 툴팁 메시지는 참조 테이블0에 넣어둔다
	m_Reference[ 0].strToolTip = szTooltip;
}


// GetToolTipText
const char* MTextView::GetToolTipText() const
{
	return m_Reference[ 0].strToolTip.c_str();
}


// ParseText
// 텍스트를 입력받으면 스트링과 태그를 파싱하여 chunk로 저장하는 함수
bool MTextView::ParseText( const wchar_t* c)
{
#define _add_str(__space)	{ if ( _len > 0)	{															\
								_buff[ _len] = 0;															\
								_str_chunk* chunk = new _str_chunk( MLocale::ConvUTF16ToAnsi( _buff), m_nReferenceIndex);	\
								if ( __space)	chunk->_space++;											\
								m_ChunkList.push_back( chunk);												\
								_len = 0;																	\
							}}

	wchar_t	_buff[ 32];
	size_t _len = 0;

	while ( 1)
	{
		if ( *c == 0)								// 종료
		{
			_add_str( false);
			break;
		}

		if ( *c == *__LF)							// 라인 피드 검사
		{
			_add_str( false);

			_lnf_chunk* chunk = new _lnf_chunk;
			m_ChunkList.push_back( chunk);
		}

		else if ( *c == *__SP)						// 스페이스 검사
		{
			// 텍스트 버퍼가 있으면...
			if ( _len > 0  ||  m_ChunkList.empty())
			{
				_buff[ _len++] = *c;

				_add_str( true);
			}
			// 없으면 이전 노드에 추가
			else
			{
				__chunk* pPrevChunk = (*m_ChunkList.rbegin());
				if ( pPrevChunk->_gettype() == _CT_STRING)
				{
					_str_chunk* pStrChunk = (_str_chunk*)pPrevChunk;
					pStrChunk->_str += " ";
					pStrChunk->_space++;
				}
				else
				{
					_buff[ _len++] = *c;

					_add_str( true);
				}
			}
		}

		else if ( *c == *__CT)						// 파싱 오류
		{
			Clear();

			return false;
		}

		else if ( *c == *__OT)						// 파싱
		{
			_add_str( false);

			if ( _parse_tag( c) == false)
			{
				Clear();

				return false;						// error
			}

			c = wcschr( c, *__CT);
			if ( c == 0)
			{
				Clear();

				return false;						// error
			}
		}

		else										// 일반 문자
		{
			_buff[ _len++] = *c;

			// 연속 스트링 개수 제한
			if ( _len == 16)
				_add_str( false);
		}


		*c++;
	}


	// 마지막에 종료 chunk를 추가한다
	{
		_end_chunk* pChunk = new _end_chunk();
		m_ChunkList.push_back( pChunk);
	}


	// 스트링 chunk에 대해서 크기와 스페이스의 크기를 구한다
	MFont* pFont = HasLookAndFill() ? GetLookAndFill()->GetFont() : MFontManager::Get( NULL);
	MFont* pOldFont = pFont;

	size_t size = m_ChunkList.size();
	for ( size_t i = 0;  i < size;  i++)
	{
		__chunk* pChunk = m_ChunkList[ i];

		switch ( pChunk->_gettype())
		{
		case _CT_FONT :
			{
				pOldFont = pFont;
				_fnt_chunk* _font_cunk = (_fnt_chunk*)pChunk;
				pFont = MFontManager::Get( _font_cunk->_fnt.c_str());
				continue;
			}
			break;

		case _CT_RFONT :
			pFont = pOldFont;
			break;

		case _CT_STRING :
			{
				_str_chunk* str_chunk = (_str_chunk*)pChunk;

				// 스트링 크기를 구함
				str_chunk->_w = pFont->GetWidth( str_chunk->_str.c_str());
				str_chunk->_h = pFont->GetHeight();

				// 스트링의 마지막이 스페이스로 끝나는지 검사해서 스페이스의 크기를 구함
				if ( str_chunk->_space > 0)
					str_chunk->_space *= pFont->GetWidth( " ");
			}
			break;

		default :
			break;
		}
	}

	return true;
}


#define _trim_eq( ch)		{ while(*c==*ch) { if (*c==*__CT || *c=='\0') return -1;  *c++;  _read++; } }
#define _trim_neq( ch)		{ while(*c!=*ch) { if (*c==*__CT || *c=='\0') return -1;  *c++;  _read++; }  *c++;  _read++; }


// Get tag
int _get_tag( const wchar_t* c, wstring& _tag)
{
	int _read = 0;
 	_trim_neq( __OT);
 	_trim_eq( __SP);

 	if ( *c == *__SL)
 	{
		_tag += *c++;
		_read++;
		_trim_eq( __SP);

		while ( *c != *__SP  &&  *c != *__CT)
		{
			if (  *c == 0)		return -1;
			_tag += *c++;
			_read++;
		}
	}

	else
	{
		while ( *c != VK_SPACE)
		{
			if ( *c == *__CT)	return _read;
			else if ( *c == 0)	return -1;
			_tag += *c++;
			_read++;
		}
	}

	return _read;
}


// Get attribute and value
int _get_attr_value( const wchar_t* c, wstring& _attr, string& _value)
{
 	int _read = 0;
	wstring _wvalue;
 	_trim_eq( __SP);

	while ( 1)
	{
		if ( *c == *__CT  ||  *c == 0)	return -1;
		else if ( *c == VK_SPACE)		break;
		else if ( *c == '=')			break;
		_attr += *c++;
		_read++;
	}

	_trim_neq( __EQ);
	_trim_eq( __SP);

	if ( *c == *__DQ)
	{
		*c++;
		_read++;

		while ( *c != *__DQ)
		{
			if ( *c == 0)				return -1;
			_wvalue += *c++;
			_read++;
		}

		*c++;
		_read++;
	}
	else
	{
		while ( *c != *__SP  &&  *c != *__CT)
		{
			if ( *c == 0)				return -1;
			_wvalue += *c++;
			_read++;
		}
	}

	_value = MLocale::ConvUTF16ToAnsi( _wvalue.c_str());
	return _read;
}


bool MTextView::_parse_tag( const wchar_t* c)
{
	int _read;
	wstring _tag;
	if ( (_read = _get_tag( c, _tag)) < 0)	return false;
	c += _read;

 	if ( _tag == L"BITMAP")				return _parse_bmp( c);
 	else if ( _tag == L"SPACE")			return _parse_spc( c);
 	else if ( _tag == L"FONT")			return _parse_fnt( c);
 	else if ( _tag == L"/FONT")			return _parse_rfn( c);
 	else if ( _tag == L"COLOR")			return _parse_col( c);
 	else if ( _tag == L"/COLOR")		return _parse_rco( c);
 	else if ( _tag == L"ALIGN")			return _parse_aln( c);
	else if ( _tag == L"COLUMN")		return _parse_hco( c);
	else if ( _tag == L"PRGSPC")		return _parse_prs( c);
	else if ( _tag == L"LINESPC")		return _parse_lns( c);
	else if ( _tag == L"CR")			return _parse_crt( c);
	else if ( _tag == L"INDENT")		return _parse_idt( c);
	else if ( _tag == L"OUTDENT")		return _parse_odt( c);
	else if ( _tag == L"/INDENT")		return _parse_cdt( c);
	else if ( _tag == L"SECONDENT")		return _parse_sid( c);
	else if ( _tag == L"REF")			return _parse_ref( c);
	else if ( _tag == L"/REF")			return _parse_erf( c);

	return true;
}

bool MTextView::_parse_bmp( const wchar_t* c)
{
	string _bitmap;
	int _w = 10,  _h = 10;
	while ( 1)
	{
		int _read;
		wstring _attr;
		string _value;
		if ( (_read = _get_attr_value( c, _attr, _value)) < 0)		break;
		c += _read;

		if ( _attr == L"name")		_bitmap = _value;
		else if ( _attr == L"w")	_w = atoi( _value.c_str());
		else if ( _attr == L"h")	_h = atoi( _value.c_str());
	}

	_bmp_chunk* chunk = new _bmp_chunk( _bitmap, _w, _h, m_nReferenceIndex);
	m_ChunkList.push_back( chunk);
	return true;
}

bool MTextView::_parse_spc( const wchar_t* c)
{
	int _w = 0,  _h = 0;
	while ( 1)
	{
		int _read;
		wstring _attr;
		string _value;
		if ( (_read = _get_attr_value( c, _attr, _value)) < 0)		break;
		c += _read;

		if ( _attr == L"w")			_w = atoi( _value.c_str());
		else if ( _attr == L"h")	_h = atoi( _value.c_str());
	}

	_spc_chunk* chunk = new _spc_chunk( _w, _h, m_nReferenceIndex);
	m_ChunkList.push_back( chunk);
	return true;
}

bool MTextView::_parse_fnt( const wchar_t* c)
{
	string _font;
	while ( 1)
	{
		int _read;
		wstring _attr;
		string _value;
		if ( (_read = _get_attr_value( c, _attr, _value)) < 0)		break;
		c += _read;

		if ( _attr == L"name")		_font = _value;
	}

	_fnt_chunk* chunk = new _fnt_chunk( _font);
	m_ChunkList.push_back( chunk);
	return true;
}

bool MTextView::_parse_rfn( const wchar_t* c)
{
	_rfn_chunk* chunk = new _rfn_chunk();
	m_ChunkList.push_back( chunk);
	return true;
}

bool MTextView::_parse_col( const wchar_t* c)
{
	unsigned char _r =255, _g =255, _b =255, _a =255;
	while ( 1)
	{
		int _read;
		wstring _attr;
		string _value;
		if ( (_read = _get_attr_value( c, _attr, _value)) < 0)		break;
		c += _read;

		if ( _attr == L"r")			_r = atoi( _value.c_str());
		else if ( _attr == L"g")	_g = atoi( _value.c_str());
		else if ( _attr == L"b")	_b = atoi( _value.c_str());
		else if ( _attr == L"a")	_a = atoi( _value.c_str());
	}

	_col_chunk* chunk = new _col_chunk( _r, _g, _b, _a);
	m_ChunkList.push_back( chunk);
	return true;
}

bool MTextView::_parse_rco( const wchar_t* c)
{
	_rco_chunk* chunk = new _rco_chunk();
	m_ChunkList.push_back( chunk);
	return true;
}

bool MTextView::_parse_aln( const wchar_t* c)
{
	unsigned char _horz = 0;
	unsigned char _vert = 0;

	while ( 1)
	{
		int _read;
		wstring _attr;
		string _value;
		if ( (_read = _get_attr_value( c, _attr, _value)) < 0)		break;
		c += _read;

		if ( _attr == L"hor")
		{
			if ( _value == "justify")			_horz = MAM_JUSTIFY;
			else if ( _value == "center")		_horz = MAM_HCENTER;
			else if ( _value == "hcenter")		_horz = MAM_HCENTER;
			else if ( _value == "right")		_horz = MAM_RIGHT;
			else								_horz = MAM_LEFT;
		}
		else if ( _attr == L"ver")
		{
			if ( _value == "center")			_vert = MAM_VCENTER;
			else if ( _value == "vcenter")		_vert = MAM_VCENTER;
			else if ( _value == "bottom")		_vert = MAM_BOTTOM;
			else								_vert = MAM_TOP;
		}
	}

	_aln_chunk* chunk = new _aln_chunk( _horz, _vert);
	m_ChunkList.push_back( chunk);
	return true;
}

bool MTextView::_parse_hco( const wchar_t* c)
{
	unsigned char _height = 0;
	while ( 1)
	{
		int _read;
		wstring _attr;
		string _value;
		if ( (_read = _get_attr_value( c, _attr, _value)) < 0)		break;
		c += _read;

		if ( _attr == L"h")
		{
			if ( _value == "auto")		_height = 0;
			else						_height = atoi( _value.c_str());
		}
	}

	_hco_chunk* chunk = new _hco_chunk( _height);
	m_ChunkList.push_back( chunk);
	return true;
}

bool MTextView::_parse_prs( const wchar_t* c)
{
	int _spacing = -1;
	while ( 1)
	{
		int _read;
		wstring _attr;
		string _value;
		if ( (_read = _get_attr_value( c, _attr, _value)) < 0)		break;
		c += _read;

		if ( _attr == L"spacing")
		{
			if ( _value == "auto")		_spacing = -1;
			else						_spacing = atoi( _value.c_str());
		}
	}

	_prs_chunk* chunk = new _prs_chunk( _spacing);
	m_ChunkList.push_back( chunk);
	return true;
}

bool MTextView::_parse_lns( const wchar_t* c)
{
	unsigned char _spacing = 0;
	while ( 1)
	{
		int _read;
		wstring _attr;
		string _value;
		if ( (_read = _get_attr_value( c, _attr, _value)) < 0)		break;
		c += _read;

		if ( _attr == L"spacing")
		{
			_spacing = atoi( _value.c_str());
		}
	}

	_lns_chunk* chunk = new _lns_chunk( _spacing);
	m_ChunkList.push_back( chunk);
	return true;
}

bool MTextView::_parse_crt( const wchar_t* c)
{
	int _h = -1;
	while ( 1)
	{
		int _read;
		wstring _attr;
		string _value;
		if ( (_read = _get_attr_value( c, _attr, _value)) < 0)		break;
		c += _read;

		if ( _attr == L"h")
		{
			_h = atoi( _value.c_str());
		}
	}

	if ( _h < 0)
	{
		_crt_chunk* chunk = new _crt_chunk();
		m_ChunkList.push_back( chunk);
	}
	else
	{
		_crh_chunk* chunk = new _crh_chunk( _h);
		m_ChunkList.push_back( chunk);
	}

	return true;
}

bool MTextView::_parse_idt( const wchar_t* c)
{
	int _w = -1;
	while ( 1)
	{
		int _read;
		wstring _attr;
		string _value;
		if ( (_read = _get_attr_value( c, _attr, _value)) < 0)		break;
		c += _read;

		if ( _attr == L"dent")
		{
			_w = atoi( _value.c_str());
		}
	}

	_idt_chunk* chunk = new _idt_chunk( _w);
	m_ChunkList.push_back( chunk);
	return true;
}

bool MTextView::_parse_odt( const wchar_t* c)
{
	int _w = -1;
	while ( 1)
	{
		int _read;
		wstring _attr;
		string _value;
		if ( (_read = _get_attr_value( c, _attr, _value)) < 0)		break;
		c += _read;

		if ( _attr == L"dent")
		{
			_w = atoi( _value.c_str());
		}
	}

	_odt_chunk* chunk = new _odt_chunk( _w);
	m_ChunkList.push_back( chunk);
	return true;
}

bool MTextView::_parse_cdt( const wchar_t* c)
{
	_cdt_chunk* chunk = new _cdt_chunk();
	m_ChunkList.push_back( chunk);
	return true;
}

bool MTextView::_parse_sid( const wchar_t* c)
{
	int _w = 0;
	while ( 1)
	{
		int _read;
		wstring _attr;
		string _value;
		if ( (_read = _get_attr_value( c, _attr, _value)) < 0)		break;
		c += _read;

		if ( _attr == L"dent")
		{
			_w = atoi( _value.c_str());
		}
	}

	_sid_chunk* chunk = new _sid_chunk( _w);
	m_ChunkList.push_back( chunk);
	return true;
}

bool MTextView::_parse_ref( const wchar_t* c)
{
	if ( m_nReferenceIndex != 0)	return false;

	string _text;
	string _tooltip;
	while ( 1)
	{
		int _read;
		wstring _attr;
		string _value;
		if ( (_read = _get_attr_value( c, _attr, _value)) < 0)		break;
		c += _read;

		if ( _attr == L"text")		_text = _value;
		if ( _attr == L"tooltip")	_tooltip = _value;
	}

	if ( _text.empty() == true  &&  _tooltip.empty() == true)
		return true;

	// Insert linked reference
	m_Reference.push_back( MTEXTVIEWREF( _text, _tooltip));
	m_nReferenceIndex = (unsigned char)m_Reference.size() - 1;
	return true;
}

bool MTextView::_parse_erf( const wchar_t* c)
{
	if ( m_nReferenceIndex == 0)	return false;

	m_nReferenceIndex = 0;
	return true;
}


// 분리된 chunk를 출력하기 편하도록 라인 단위로 재구성하는 함수. 리턴값으로 화면 영역을 넘어가는지 여부를 넘겨줌
bool MTextView::UpdateLineContents( bool bTestMode)
{
#define	ADD_LINECONTENTS(_LF,_LNSPC)	{																									\
											if ( column > 0) pCurrentLine->SetColumnHeight( column);										\
											if ( pCurrentLine->GetColumnHeight() == 0)  pCurrentLine->SetColumnHeight( pFont->GetHeight());	\
											pCurrentLine->SetHeight( pCurrentLine->GetColumnHeight() + _LNSPC);								\
											height += pCurrentLine->GetHeight();															\
											m_LineContents.push_back( pCurrentLine);														\
											pCurrentLine = new MLineContents();																\
											if ( _LF) pCurrentLine->SetIndent( indent);														\
											else pCurrentLine->SetIndent( max( 0, indent + secondent));										\
											if ( bTestMode  &&  height > r.h) {																\
												if ( pCurrentLine)	delete pCurrentLine;													\
												ClearLineContents();																		\
												return true;																				\
										}	}


	// 모든 line contents를 삭제함
	ClearLineContents();

	
	// chunk가 비어있으면 그냥 리턴
	if ( m_ChunkList.empty())		return false;


	MRECT _wr = GetWorkRectOffset();
	MRECT r( m_Rect.x + _wr.x, m_Rect.y + _wr.y, m_Rect.w + _wr.w, m_Rect.h + _wr.h);

	// 자동으로 스크롤바를 표시하는 경우, 먼저 테스트로 정렬해봐서 높이가 클라이언트 영역을 벗어나는지 확인한다.
	if ( bTestMode == false)
	{
		bool bAutoScroll = ( m_pScrollbar->IsAlwaysShow() == false  &&  m_pScrollbar->IsAlwaysHide() == false)  ?  true : false;
		if ( bAutoScroll == true)
		{
			if ( UpdateLineContents( true) == true)
				r.w -= m_pScrollbar->GetWidth();

			else
				return false;
		}
		else if ( m_pScrollbar->IsAlwaysShow() == true)
		{
			r.w -= m_pScrollbar->GetWidth();
		}
	}


	int height = 0;
	unsigned char align = DEFAULT_ALIGN;
	MFont* pFont = MFontManager::Get( NULL);
	MFont* pOldFont = pFont;
	int column = 0;
	int paragrphspc = pFont->GetHeight();
	int linespc = 5;
	int indent = 0, secondent = 0;

	if ( HasLookAndFill() == true)
	{
		MTextViewLook* pLook = (MTextViewLook*)GetLookAndFill();

		align = pLook->GetTextAlign();
		pFont = pLook->GetFont();
		pOldFont = pFont;
		column = pLook->GetColumnHeight();
		paragrphspc = pLook->GetParagraphSpacing();
		linespc = pLook->GetLineSpacing();
		indent = pLook->GetIndent();
		secondent = pLook->GetSecondent();

		if ( paragrphspc < 0)		paragrphspc = pFont->GetHeight();
	}


	MLineContents* pCurrentLine = new MLineContents();

	size_t _size = m_ChunkList.size();
	for ( size_t i = 0;  i < _size;  i++)
	{
		__chunk* pChunk = m_ChunkList[ i];

		switch ( pChunk->_gettype())
		{
		case _CT_FONT:
			{
				pOldFont = pFont;
				_fnt_chunk* fnt_chunk = (_fnt_chunk*)pChunk;
				pFont = MFontManager::Get( fnt_chunk->_fnt.c_str());
				break;
			}

		case _CT_RFONT:
			pFont = pOldFont;
			break;

		case _CT_ALIGN :
			{
				_aln_chunk* aln_chunk = (_aln_chunk*)pChunk;
				if ( aln_chunk->_horz > 0)		align = (align & 0x00F0) | aln_chunk->_horz;
				if ( aln_chunk->_vert > 0)		align = (align & 0x000F) | aln_chunk->_vert;
				continue;
			}

		case _CT_COLUMN :
			{
				_hco_chunk* hco_chunk = (_hco_chunk*)pChunk;
				column = hco_chunk->_h;
				continue;
			}

		case _CT_PRGSPC :
			{
				_prs_chunk *prs_chunk = (_prs_chunk*)pChunk;
				paragrphspc = prs_chunk->_spacing;
				continue;
			}

		case _CT_LINESPC :
			{
				_lns_chunk *lns_chunk = (_lns_chunk*)pChunk;
				linespc = lns_chunk->_spacing;
				continue;
			}

		case _CT_LINEFEED :
			{
				pCurrentLine->SetAlign( EQ_ALIGN( align, MAM_JUSTIFY) ? (align & 0xF4) : align);
				ADD_LINECONTENTS( true, linespc + (paragrphspc < 0  ?  pFont->GetHeight() : paragrphspc));
				continue;
			}

		case _CT_CRETURN :
			{
				pCurrentLine->SetAlign( EQ_ALIGN( align, MAM_JUSTIFY) ? (align & 0xF4) : align);
				ADD_LINECONTENTS( false, linespc);
				continue;
			}

		case _CT_CRTHEIGHT :
			{
				_crh_chunk *crh_chunk = (_crh_chunk*)pChunk;
				pCurrentLine->SetAlign( EQ_ALIGN( align, MAM_JUSTIFY) ? (align & 0xF4) : align);
				//ADD_LINECONTENTS( false, linespc);
				if ( column > 0) pCurrentLine->SetColumnHeight( column);
				pCurrentLine->SetHeight( crh_chunk->_h);
				height += pCurrentLine->GetHeight();
				m_LineContents.push_back( pCurrentLine);
				pCurrentLine = new MLineContents();
				pCurrentLine->SetIndent( max( 0, indent + secondent));
				if ( bTestMode  &&  height > r.h)
				{
					if ( pCurrentLine)	delete pCurrentLine;
					ClearLineContents();
					return true;
				}
				continue;
			}

		case _CT_INDENT :
		case _CT_OUTDENT :
		case _CT_CLRDENT :
			{
				switch ( pChunk->_gettype())
				{
				case _CT_INDENT :
					{
						_idt_chunk *idt_chunk = (_idt_chunk*)pChunk;
						if ( idt_chunk->_w < 0)		indent += DEFAULT_INDENT;
						else						indent += idt_chunk->_w;
						break;
					}

				case _CT_OUTDENT :
					{
						_odt_chunk *odt_chunk = (_odt_chunk*)pChunk;
						if ( odt_chunk->_w < 0)		indent -= DEFAULT_INDENT;
						else						indent -= odt_chunk->_w;
						break;
					}

				case _CT_CLRDENT :
					indent = 0;
					break;
				}

				if ( indent < 0)			indent = 0;

				// 출력할 contents가 없으면 바로 indent를 설정한다
				if ( pCurrentLine->GetWidth() == 0  ||  pCurrentLine->GetCount() == 0)
					pCurrentLine->SetIndent( indent);

				continue;
			}


		case _CT_SECONDENT :
			secondent = ((_sid_chunk*)pChunk)->_w;
			continue;

		case _CT_END :
			{
				pCurrentLine->SetAlign( EQ_ALIGN( align, MAM_JUSTIFY) ? (align & 0xF4) : align);
				ADD_LINECONTENTS( false, linespc);

				if ( pCurrentLine != NULL)		delete pCurrentLine;
				continue;
			}

		default :
			break;
		}



		// 새 contents를 추가할 경우 작업 라인 영역을 벗어나지 않는지 확인
		int _w = pCurrentLine->GetIndent() + pCurrentLine->GetWholeWidth() + pChunk->_getwidth();
		if ( _w <= r.w)
		{
			pCurrentLine->AddContents( pChunk);
		}

		// 작업 라인 영역 밖으로 벗어난 경우
		else
		{
			// 출력 내용이 아무것도 없으면 현재 contents를 강제로 라인에 삽입하여 리스트에 추가하고 다음 라인으로 넘어감
			if ( pCurrentLine->GetWidth() == 0  ||  pCurrentLine->GetCount() == 0)
			{
				pCurrentLine->AddContents( pChunk);
				pCurrentLine->SetAlign( align);
				ADD_LINECONTENTS( false, linespc);
			}

			// 마지막 공백을 제거했을 때 작업 라인 영역을 벗어나지 않는 경우
			else if ( (_w - pChunk->_getspace()) <= r.w)
			{
				pCurrentLine->AddContents( pChunk);
				pCurrentLine->SetAlign( align);
				ADD_LINECONTENTS( false, linespc);
			}

			// 마지막 contents가 스트링 형식이면 쪼개넣기를 시도
			else if ( m_bWrap == false  &&  pChunk->_gettype() == _CT_STRING  &&  pChunk->_getwidth() > 0)
			{
				wstring _head = L"";
				wstring _tail = MLocale::ConvAnsiToUTF16( pChunk->_getcdata());
				size_t _len = _tail.length();

				for ( size_t i = 0;  i < _len;  i++)
				{
					wchar_t _c = _tail.at( 0);
					wstring _wcs;
					_wcs += _head;
					_wcs += _c;
					if ( ( _c >= L'a'  &&  _c <= L'z')  ||  ( _c >= L'A'  &&  _c <= L'Z'))
						_wcs += L"-";

					string _mbs = MLocale::ConvUTF16ToAnsi( _wcs.c_str());
					int _w = pCurrentLine->GetIndent() + pCurrentLine->GetWholeWidth() + pFont->GetWidth( _mbs.c_str());
					if ( _w > r.w)
						break;

					_head += _c;
					_tail.erase( 0, 1);
				}


				if ( _head.empty() == false)
				{
					wchar_t _c = _head.at( _head.length() - 1);
					if ( ( _c >= L'a'  &&  _c <= L'z')  ||  ( _c >= L'A'  &&  _c <= L'Z'))
						_head += L"-";

					string _mbs_head = MLocale::ConvUTF16ToAnsi( _head.c_str());

					_str_chunk* pStrHeadChunk = new _str_chunk( _mbs_head, pChunk->_getref());
					pStrHeadChunk->_w = pFont->GetWidth( _mbs_head.c_str());
					pStrHeadChunk->_h = pFont->GetHeight();
					pStrHeadChunk->_dummy = true;
					pCurrentLine->AddContents( pStrHeadChunk);
				}
				pCurrentLine->SetAlign( align);
				ADD_LINECONTENTS( false, linespc);

				if ( _tail.empty() == false)
				{
					string _mbs_tail = MLocale::ConvUTF16ToAnsi( _tail.c_str());

					_str_chunk* pStrTailChunk = new _str_chunk( _mbs_tail, pChunk->_getref());
					pStrTailChunk->_w = pFont->GetWidth( _mbs_tail.c_str());
					pStrTailChunk->_h = pFont->GetHeight();
					pStrTailChunk->_space = pStrTailChunk->_getspace();
					pStrTailChunk->_dummy = true;
					pCurrentLine->AddContents( pStrTailChunk);
				}
			}

			// 지금까지 만든 라인을 등록하고 새 라인을 생성한다
			else
			{
				pCurrentLine->SetAlign( align);
				ADD_LINECONTENTS( false, linespc);

				pCurrentLine->AddContents( m_ChunkList[ i]);
			}
		}
	}


	// 라인 컨텐츠에 대한 정보를 업데이트 함
	for ( vector< MLineContents*>::iterator itr = m_LineContents.begin();  itr != m_LineContents.end();  itr++)
		(*itr)->UpdateLineInfo( r);


	// 페이지 사이즈 업데이트 - 한줄 이상이면 클라이언트 넓이가 페이지 넓이이며, 한줄일 경우엔 첫줄의 넓이가 페이지 넓이가 됨
	if ( m_LineContents.empty() == true)
	{
		m_PageSize.w = 0;
		m_PageSize.h = 0; 
	}
	else
	{
		MRECT _work_rect = HasLookAndFill() ? m_pLookAndFill->GetWorkRectOffset() : MRECT( 0, 0, 0, 0);
		int __w = ((m_LineContents.size() > 1)  ?  r.w  :  m_LineContents[ 0]->GetWidth()) - _work_rect.w;
		int __h = height - _work_rect.h;
		m_PageSize.w = max( 0, __w);
		m_PageSize.h = max( 0, __h); 
	}


	// 스크롤바 업데이트
	UpdateScrollbar();

	return false;
}


// FindLinkedContent
MRefContect MTextView::FindRefContent( MPOINT& pos)
{
	MRefContect _ref;
	_ref.i = 0;
	_ref.r = MRECT( 0, 0, 0, 0);

	if ( m_Reference.size() < 2)		return _ref;


	MRECT r = GetClientRect();
	if ( r.InPoint( pos) == false)		return _ref;


	// Find target line
	int _scrollval = m_pScrollbar->GetScrollValue();
	int y = -_scrollval;
	size_t _size = m_LineContents.size();
	for ( vector< MLineContents*>::iterator itr = m_LineContents.begin();  itr != m_LineContents.end();  itr++)
	{
		MLineContents* _contents = (*itr);

		if ( pos.y < y)					return _ref;

		if ( pos.y >= y  &&  pos.y < (y + _contents->GetColumnHeight()) )
		{
			// Find reference contents
			_size = _contents->GetRefInfoCount();
			for ( size_t i = 0;  i < _size;  i++)
			{
				MRECT r = _contents->GetRefInfoRect( i);

				if ( pos.x >= r.x  &&  pos.x < (r.x + r.w))
				{
					_ref.i = _contents->GetRefInfoRef( i);
					_ref.r = MRECT( r.x, y + _scrollval, r.w, r.h);
					return _ref;
				}
			}
		}

		y += _contents->GetHeight();
	}

	return _ref;
}


// ScrollPage
void MTextView::ScrollPage( int delta)
{
	int _value = m_pScrollbar->GetScrollValue() + delta;

	if ( _value > m_pScrollbar->GetMax())	m_pScrollbar->SetScrollValue( m_pScrollbar->GetMax());
	else if ( _value < 0)					m_pScrollbar->SetScrollValue( 0);

	m_pScrollbar->SetScrollValue( _value);
}


// 페이지 사이즈를 구하는 함수
MSIZE MTextView::GetPageSize()
{
	if ( m_bDoParseText == true)
	{
		wstring _wcs = MLocale::ConvAnsiToUTF16( m_strText.c_str());
		ParseText( _wcs.c_str());

		UpdateLineContents();

		m_bDoParseText = false;
		m_bUpdateLineContents = false;
	}

	return m_PageSize;
}


// NoWrap
void MTextView::SetWrapping( bool bWrap)
{
	if ( m_bWrap == bWrap)
		return;

	m_bWrap = bWrap;

	m_bUpdateLineContents = true;
}


// GetRefCount
size_t MTextView::GetRefCount()
{
	if ( m_bDoParseText == true)
	{
		wstring _wcs = MLocale::ConvAnsiToUTF16( m_strText.c_str());
		ParseText( _wcs.c_str());

		UpdateLineContents();

		m_bDoParseText = false;
		m_bUpdateLineContents = false;
	}

	return m_Reference.size();
}


// GetRefRect
MRECT MTextView::GetRefRect( int index)
{
	int y = 0;
	for ( vector< MLineContents*>::iterator itr = m_LineContents.begin();  itr != m_LineContents.end();  itr++)
	{
		MLineContents* pContent = (*itr);

		size_t _size = pContent->GetRefInfoCount();
		for ( size_t i = 0;  i < _size;  i++)
		{
			if ( pContent->GetRefInfoRef( i) == index)
			{
				MRECT r = pContent->GetRefInfoRect( i);
				return MRECT( r.x, y, r.w, r.h);
			}
		}

		y += pContent->GetHeight();
	}

	return MRECT( 0, 0, 0, 0);
}


// SetCaretPos
int MTextView::SetCaretPos( int index)
{
	if ( index < 0)				index = -1;
	else
	{
		int _size = (int)m_Reference.size();
		if ( index >= _size)		index = _size - 1;
	}

	if ( index == m_nCaret)		return m_nCaret;

	m_nCaret = index;
	return m_nCaret;
}


// Clear
// 모든 텍스트를 삭제하는 함수
void MTextView::Clear()
{
	m_strText.clear();

	ClearLineContents();
	ClearChunkList();

	m_pScrollbar->SetScrollRange( 0, 0);
	m_pScrollbar->SetScrollValue( 0);

	m_PageSize = MSIZE( 0, 0);

	m_nCaret = -1;
	m_nMouseOver = 0;
}

void MTextView::ClearChunkList()
{
	size_t _size = m_ChunkList.size();
	for ( size_t i = 0;  i < _size;  i++)
		delete m_ChunkList[ i];

	m_ChunkList.clear();


	// 원래 툴팁을 복구한다
	m_strToolTip = m_Reference[ 0].strToolTip;

	// 참조 테이블을 재설정 한다
	m_Reference.clear();
	m_Reference.push_back( MTEXTVIEWREF( "", m_strToolTip));
	m_nReferenceIndex = 0;
}

void MTextView::ClearLineContents()
{
	for ( vector< MLineContents*>::iterator itr = m_LineContents.begin();  itr != m_LineContents.end();  itr++)
		delete (*itr);

	m_LineContents.clear();
}


// glueGetRefRect
MWLua::ret_int4 MTextView::glueGetRefRect( int index)
{
	MRECT _r = GetRefRect( index);
	return MWLua::ret_int4( _r.x, _r.y, _r.w, _r.h);
}


// glueGetPageSize
MWLua::ret_int2	MTextView::glueGetPageSize()
{
	MSIZE _size = GetPageSize();
	return MWLua::ret_int2( _size.w, _size.h);
}








// MTextViewLook
MTextViewLook::MTextViewLook()
{
	m_nTextAlign = DEFAULT_ALIGN;
	m_nColumnHeight = 0;
	m_nParagraphSpacing = -1;
	m_nLineSpacing = 5;
	m_nIndent = 0;
	m_nSecondent = 0;
}

void MTextViewLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	OnDrawText( (MTextView*)pWidget, pDC);
}

void MTextViewLook::OnDrawText( MTextView* pTextView, MDrawContext* pDC)
{
	MRECT			r = pTextView->GetClientRect();
	float			x = 0.0f;
	int				y = -pTextView->GetScrollBar()->GetScrollValue();
	float			spc_addition = 0.0f;
	MFont*			pOldFont = GetFont();
	MCOLOR			nOldColor = GetFontColor( MWP_BORDER, pTextView->GetState(), MColorTable::TEXT[ pTextView->GetStateNum()]);


	// 처음 폰트와 색상 설정
	pDC->SetFont( pOldFont);
	pDC->SetColor( nOldColor);


	//  라인 출력
	size_t _size = pTextView->GetLineContentsCount();
	for ( size_t i = 0;  i < _size;  i++)
	{
		MLineContents* pContents = pTextView->GetLineContents( i);
		if ( pContents == NULL)				continue;


		// 영역을 벗어나는지 검사
		bool bInner = true;
		if ( (y + pContents->GetColumnHeight()) < 0)	bInner = false;
		else if ( y > r.h)								break;


		// Draw reference rectangle
		size_t _ref_size = pContents->GetRefInfoCount();
		for ( size_t i = 0;  i < _ref_size;  i++)
		{
			MRECT r = pContents->GetRefInfoRect( i);
			r.y += y;

			if ( Draw( pDC, r, MWP_ITEMBKGRND, pTextView->GetState()) == false)
			{
				MCOLOR _col = pDC->SetColor( MColorTable::SELECTBAR[ pTextView->GetStateNum()]);
				pDC->FillRectangle( r);
				pDC->SetColor( _col);
			}
		}


		// Draw line contents
		x = pContents->GetStartPos() + 0.5f;
		spc_addition = pContents->GetSpaceAddition();
		size_t _size = pContents->GetCount();
		for ( size_t j = 0;  j < _size;  j++)
		{
			__chunk* pChunk = pContents->GetContents( j);
			if ( pChunk == NULL)			continue;


			// Draw contents
			switch ( pChunk->_gettype())
			{
			case _CT_STRING :
				{
					if ( bInner == false)	continue;

					_str_chunk* str_chunk = (_str_chunk*)pChunk;
					if ( EQ_ALIGN( pContents->GetAlign(), MAM_VCENTER))			pDC->Text( (int)x, y + ((pContents->GetColumnHeight() - str_chunk->_getheight()) >> 1), str_chunk->_str.c_str());
					else if ( EQ_ALIGN( pContents->GetAlign(), MAM_BOTTOM))		pDC->Text( (int)x, y + pContents->GetColumnHeight() - str_chunk->_getheight(), str_chunk->_str.c_str());
					else														pDC->Text( (int)x, y, str_chunk->_str.c_str());

					break;
				}

			case _CT_BITMAP :
				{
					if ( bInner == false)	continue;

					_bmp_chunk* bmp_chunk = (_bmp_chunk*)pChunk;

					pDC->SetBitmap( MBitmapManager::Get( bmp_chunk->_bmp.c_str()));
					if (      EQ_ALIGN( pContents->GetAlign(), MAM_VCENTER))	pDC->Draw( (int)x, y + ((pContents->GetColumnHeight() - bmp_chunk->_getheight()) >> 1), bmp_chunk->_w, bmp_chunk->_h);
					else if ( EQ_ALIGN( pContents->GetAlign(), MAM_BOTTOM))		pDC->Draw( (int)x, y + pContents->GetColumnHeight() - bmp_chunk->_getheight(), bmp_chunk->_w, bmp_chunk->_h);
					else														pDC->Draw( (int)x, y, bmp_chunk->_w, bmp_chunk->_h);

					break;
				}

			case _CT_SPACE :
					break;

			case _CT_FONT :
				{
					pOldFont = pDC->GetFont();
					_fnt_chunk* fnt_chunk = (_fnt_chunk*)pChunk;
					pDC->SetFont( MFontManager::Get( fnt_chunk->_fnt.c_str()) );
					continue;
				}

			case _CT_RFONT :
				pDC->SetFont( pOldFont);
				continue;

			case _CT_COLOR :
				{
					nOldColor = pDC->GetColor();
					_col_chunk* col_chunk = (_col_chunk*)pChunk;
					pDC->SetColor( col_chunk->_r, col_chunk->_g, col_chunk->_b, col_chunk->_a);
					continue;
				}

			case _CT_RCOLOR :
				pDC->SetColor( nOldColor);
				continue;

			default :
				continue;
			}


			// Draw caret and mouse overred reference
			for ( size_t i = 0;  i < _ref_size;  i++)
			{
				int nRef = pContents->GetRefInfoRef( i);

				if ( nRef == pTextView->GetMouseOver())
				{
					MRECT r = pContents->GetRefInfoRect( i);
					r.x = r.x - 2;
					r.y = r.y + y - 2;
					r.w += 4;
					r.h += 4;

					if ( Draw( pDC, r, MWP_SELECTBAR, pTextView->GetState()) == false)
					{
						MCOLOR _col = pDC->SetColor( MColorTable::TEXT_SELECTED);
						pDC->Line( r.x, r.y + r.h, r.x + r.w, r.y + r.h);
						pDC->SetColor( _col);
					}
				}

				if ( nRef == pTextView->GetCaretPos())
				{
					MRECT r = pContents->GetRefInfoRect( i);
					r.x = r.x - 2;
					r.y = r.y + y - 2;
					r.w += 4;
					r.h += 4;

					if ( Draw( pDC, r, MWP_CARET, pTextView->GetState()) == false)
					{
						MCOLOR _col = pDC->SetColor( MColorTable::TEXT_SELECTED);
						pDC->Line( r.x, r.y + r.h, r.x + r.w, r.y + r.h);
						pDC->SetColor( _col);
					}
				}
			}


			x += (float)pContents->GetContents( j)->_getwidth() + ((pContents->GetContents( j)->_getspace() > 0) ? spc_addition : 0.0f);
		}

		y += pContents->GetHeight();
	}
}


} // namespace mint3
