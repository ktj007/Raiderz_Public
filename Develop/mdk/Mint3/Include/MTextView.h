#pragma once

#include "MWidget.h"
#include "MDrawContext.h"
#include "MScrollBar.h"
#include <string>
#include <vector>


namespace mint3
{



/*************************************************************************
 (class) Chunk
**************************************************************************/
// Chunk types
enum _chunk_type
{
	_CT_NONE	= 0,
	_CT_STRING,		// String
	_CT_BITMAP,		// Bitmap
	_CT_SPACE,		// Null space
	_CT_FONT,		// Font
	_CT_RFONT,		// Restore font
	_CT_COLOR,		// Color
	_CT_RCOLOR,		// Restore color
	_CT_ALIGN,		// Align
	_CT_COLUMN,		// Height of column
	_CT_LINEFEED,	// Line feed
	_CT_CRETURN,	// Carriage return
	_CT_INDENT,		// Indent
	_CT_OUTDENT,	// Outdent
	_CT_CLRDENT		// Clear indent
};

// Chunk
class __chunk
{
public:
	__chunk ()				{}
	virtual ~__chunk ()		{}
	virtual int _gettype()			{ return _CT_NONE;			}		// 타입을 구함
	virtual const char* _getcdata()	{ return "";				}		// char 데이터를 구함
	virtual size_t _getlength()		{ return 0;					}		// 길이를 구함
	virtual int _getwidth()			{ return 0;					}		// 넓이를 구함
	virtual int _getheight()		{ return 0;					}		// 높이를 구함
	virtual int _getspace()			{ return 0;					}		// 스페이스의 넓이를 구함
};

// String
class _str_chunk : public __chunk
{
public:
	string		_str;
	short		_w, _h;
	short		_space;

	_str_chunk( const string& str) : _str( str), _w( 0), _h( 0), _space( 0)  {}
	virtual int _gettype()			{ return _CT_STRING;		}
	const char* _getcdata()			{ return _str.c_str();		}
	size_t _getlength()				{ return _str.length();		}
	int _getwidth()					{ return _w;				}
	int _getheight()				{ return _h;				}
	int _getspace()					{ return _space;			}
};

// Bitmap
class _bmp_chunk : public __chunk
{
public:
	string		_bmp;
	short		_w, _h;

	_bmp_chunk( const string& bmp, short w, short h) : _bmp( bmp), _w( w), _h( h) {}
	virtual int _gettype()			{ return _CT_BITMAP;		}
	size_t _getlength()				{ return 0;					}
	int _getwidth()					{ return _w;				}
	int _getheight()				{ return _h;				}
};

// Null space
class _spc_chunk : public __chunk
{
public:
	short		_w, _h;

	_spc_chunk( short w, short h) : _w( w), _h( h) {}
	virtual int _gettype()			{ return _CT_SPACE;			}
	size_t _getlength()				{ return 0;					}
	int _getwidth()					{ return _w;				}
	int _getheight()				{ return _h;				}
};

// Font
class _fnt_chunk : public __chunk
{
public:
	string		_fnt;

	_fnt_chunk( const string& fnt) : _fnt( fnt) {}
	virtual int _gettype()			{ return _CT_FONT;			}
};

// Restore font
class _rfn_chunk : public __chunk
{
public:
	_rfn_chunk() {}
	virtual int _gettype()			{ return _CT_RFONT;			}
};

// Color
class _col_chunk : public __chunk
{
public:
	unsigned char	_r, _g, _b, _a;

	_col_chunk( unsigned char r, unsigned char g, unsigned char b, unsigned char a =255) : _r( r), _g( g), _b( b), _a( a) {}
	virtual int _gettype()			{ return _CT_COLOR;			}
};

// Restore color
class _rco_chunk : public __chunk
{
public:
	_rco_chunk() {}
	virtual int _gettype()			{ return _CT_RCOLOR;		}
};

// Align
class _aln_chunk : public __chunk
{
public:
	unsigned char	_align;

	_aln_chunk( unsigned char align) : _align( align) {}
	virtual int _gettype()			{ return _CT_ALIGN;			}
};

// Height of column
class _hco_chunk : public __chunk
{
public:
	unsigned char	_height;		// 0이면 자동

	_hco_chunk( int height) : _height( (unsigned char)height) {}
	virtual int _gettype()			{ return _CT_COLUMN;		}
};

// Line feed
class _lnf_chunk : public __chunk
{
public:
	_lnf_chunk() {}
	virtual int _gettype()			{ return _CT_LINEFEED;		}
};

// Carriage return
class _crt_chunk : public __chunk
{
public:
	_crt_chunk() {}
	virtual int _gettype()			{ return _CT_CRETURN;		}
};

// Indent
class _idt_chunk : public __chunk
{
public:
	_idt_chunk() {}
	virtual int _gettype()			{ return _CT_INDENT;		}
};

// Outdent
class _odt_chunk : public __chunk
{
public:
	_odt_chunk() {}
	virtual int _gettype()			{ return _CT_OUTDENT;		}
};

// Clear indent
class _cdt_chunk : public __chunk
{
public:
	_cdt_chunk() {}
	virtual int _gettype()			{ return _CT_CLRDENT;		}
};

typedef vector< __chunk*>		MChunkList;




/*************************************************************************
 (class) MLineContents
**************************************************************************/
class MLineContents
{
protected:
	vector< __chunk*>	m_LineContents;				// 라인 컨텐츠
	unsigned char		m_nCount;					// 컨텐츠 갯수
	unsigned short		m_nWidth;					// 라인 전체 넓이
	unsigned char		m_nHeight;					// 라인 전체 높이
	unsigned char		m_nAlign;					// 라인 정렬
	unsigned char		m_nSpaceCount;				// 스페이스의 개수
	unsigned char		m_nLastSpace;				// 마지막 스페이스의 넓이
	unsigned char		m_nTrimHead;				// 처음 contents에 대한 문자열 자름 위치
	unsigned char		m_nTrimTail;				// 마지막 contents에 대한 문자열 자름 위치
	unsigned char		m_nIndent;					// 들여쓰기
	bool				m_bHyphen;					// 마지막에 하이픈 문자가 들어가는지 여부


public:
	MLineContents();
	virtual ~MLineContents();

	bool AddContents( __chunk* pContents);
	bool AddTrimHeadContents( __chunk* pContents, int trim_pos, int trim_width);
	bool AddTrimTailContents( __chunk* pContents, int trim_pos, int trim_width, bool hyphen);

	__chunk* GetContents( int index);
	int GetCount();

	int GetWidth();									// 마지막 스페이스를 뺀 전체 넓이
	int GetWholeWidth();							// 전체 넓이를 구함
	
	void SetHeight( int height);					// 전체 높이를 설정함
	int GetHeight();								// 전체 높이를 구함
	
	void SetAlign( unsigned char align);			// 정렬을 설정함
	unsigned char GetAlign();						// 정렬을 구함

	int GetSpaceCount();							// 라인에 들어있는 처리할 스페이스의 개수

	int GetTrimHead();								// 처음 contents에 문자열 자름 위치를 구함
	int GetTrimTail();								// 마지막 contents에 문자열 자름 위치를 구함

	void SetIndent( int indent);					// 들여쓰는 거리
	int GetIndent();								// 들여쓰기 거리를 구함	

	bool IsHyphen();								// 하이픈'-' 문자가 있는지 여부를 구함

	void Clear();
};




/*************************************************************************
 (class) MTextView
**************************************************************************/
class MTextView : public MWidget
{
// Member variables
protected:
	MScrollBar*					m_pScrollbar;
	int							m_nScrollbarVisible;
	MChunkList					m_ChunkList;
	vector< MLineContents*>		m_LineContents;
	MSIZE						m_PageSize;
	bool						m_bWrap;
	bool						m_bAutoScroll;
	DWORD						m_dwAutoScroll;



// Member functions
public:
	MTextView( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~MTextView();

	// Scrollbar
	void ShowScrollbar( int mode);
	void SetScrollbarLook( MScrollBarLook* pLook);
	void SetScrollbarWidth( int nWidth);
	
	// 페이지를 위(-)/아래(+)로 스크롤하는 함수
	void ScrollPage( int delta);

	// 페이지 사이즈를 구하는 함수
	MSIZE GetPageSize() const;

	// 페이지 자동 스크롤 함수
	void StartAutoScroll();
	void StopAutoScroll();

	// 단어 단위로 래핑(Wrapping)을 하는지 여부를 설정하는 함수
	void SetWrapping( bool bWrap);
	bool IsWrapping();




protected:
	// 텍스트를 입력받으면 스트링과 태그를 파싱하여 chunk로 저장하는 함수
	bool ParseText( const char* c);

	// 분리된 chunk를 출력하기 편하도록 라인 단위로 재구성하는 함수. 리턴값으로 화면 영역을 넘어가는지 여부를 넘겨줌
	bool UpdateLineContents( bool bTestMode =false);

	// 스크롤바를 업데이트 하는 함수
	void UpdateScrollbar();


public:
	// 텍스트를 삭제하는 함수
	void Clear();
protected:
	void ClearChunkList();
	void ClearLineContents();


protected:
	bool _parse_tag( const char* c);
	bool _parse_bmp( const char* c);
	bool _parse_spc( const char* c);
	bool _parse_fnt( const char* c);
	bool _parse_rfn( const char* c);
	bool _parse_col( const char* c);
	bool _parse_rco( const char* c);
	bool _parse_aln( const char* c);
	bool _parse_hco( const char* c);
	bool _parse_crt( const char* c);
	bool _parse_idt( const char* c);
	bool _parse_odt( const char* c);
	bool _parse_cdt( const char* c);


protected:
	MRECT GetWorkRect();


protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener);
	virtual bool OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs =NULL);
	virtual void OnRun( void);
	virtual void OnDraw( MDrawContext* pDC);
	virtual void OnSize(int w, int h);
	virtual void OnScrollbarChanged(int nPos);

public:
	virtual void SetText( const char* szText);
	virtual const char* GetText( void);


public:
	void glueClear();
	void glueScrollPage( int delta);
	int glueGetPageWidth();
	int glueGetPageHeight();
	void glueStartAutoScroll();
	void glueStopAutoScroll();



#define MINT_TEXTVIEW	"TextView"
	virtual const char* GetClassName( void)		{ return MINT_TEXTVIEW;		}
};

} // namespace mint3