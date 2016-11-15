#pragma once

#include "MWidget.h"
#include "MScrollBar.h"
#include "MLookBase.h"
#include <string>
#include <vector>


namespace mint3
{


class MTextView;




/// class : __chunk

// Chunk types
enum _chunk_type
{
	_CT_NONE	= 0,
	_CT_STRING,			// String
	_CT_BITMAP,			// Bitmap
	_CT_SPACE,			// Null space
	_CT_FONT,			// Font
	_CT_RFONT,			// Restore font
	_CT_COLOR,			// Color
	_CT_RCOLOR,			// Restore color
	_CT_ALIGN,			// Align
	_CT_COLUMN,			// Height of column
	_CT_PRGSPC,			// Paragraph spacing
	_CT_LINESPC,		// Line spacing
	_CT_LINEFEED,		// Line feed
	_CT_CRETURN,		// Carriage return
	_CT_CRTHEIGHT,		// Carriage return with height
	_CT_INDENT,			// First line indent
	_CT_OUTDENT,		// First line outdent
	_CT_CLRDENT,		// Clear first line indent
	_CT_SECONDENT,		// Second line indent
	_CT_END				// End of chunk
};

// Chunk
class __chunk
{
public:
	__chunk ()				{}
	virtual ~__chunk ()		{}
	virtual int _gettype()				{ return _CT_NONE;			}		// 타입을 구함
	virtual const char* _getcdata()		{ return "";				}		// char 데이터를 구함
	virtual size_t _getlength()			{ return 0;					}		// 길이를 구함
	virtual int _getwidth()				{ return 0;					}		// 넓이를 구함
	virtual int _getheight()			{ return 0;					}		// 높이를 구함
	virtual int _getspace()				{ return 0;					}		// 스페이스의 넓이를 구함
	virtual bool _isdummy()				{ return false;				}		// 더미 chunk인지 여부를 구함
	virtual unsigned char _getref()		{ return 0;					}		// 참조 인덱스를 구함
};

// String
class _str_chunk : public __chunk
{
public:
	string			_str;
	short			_w, _h;
	short			_space;
	bool			_dummy;
	unsigned char	_ref;

	_str_chunk( const string& str, unsigned char tref) : _str( str), _w( 0), _h( 0), _space( 0), _dummy( false), _ref( tref) {}
	virtual int _gettype()				{ return _CT_STRING;		}
	const char* _getcdata()				{ return _str.c_str();		}
	size_t _getlength()					{ return _str.length();		}
	int _getwidth()						{ return _w;				}
	int _getheight()					{ return _h;				}
	int _getspace()						{ return _space;			}
	bool _isdummy()						{ return _dummy;			}
	unsigned char _getref()				{ return _ref;				}
};

// Bitmap
class _bmp_chunk : public __chunk
{
public:
	string			_bmp;
	short			_w, _h;
	unsigned char	_ref;


	_bmp_chunk( const string& bmp, short w, short h, unsigned char bref) : _bmp( bmp), _w( w), _h( h), _ref( bref) {}
	virtual int _gettype()				{ return _CT_BITMAP;		}
	size_t _getlength()					{ return 0;					}
	int _getwidth()						{ return _w;				}
	int _getheight()					{ return _h;				}
	unsigned char _getref()				{ return _ref;				}
};

// Null space
class _spc_chunk : public __chunk
{
public:
	short			_w, _h;
	unsigned char	_ref;

	_spc_chunk( short w, short h, unsigned char sref) : _w( w), _h( h), _ref( sref) {}
	virtual int _gettype()				{ return _CT_SPACE;			}
	size_t _getlength()					{ return 0;					}
	int _getwidth()						{ return _w;				}
	int _getheight()					{ return _h;				}
	unsigned char _getref()				{ return _ref;				}
};

// Font
class _fnt_chunk : public __chunk
{
public:
	string			_fnt;

	_fnt_chunk( const string& fnt) : _fnt( fnt) {}
	virtual int _gettype()				{ return _CT_FONT;			}
};

// Restore font
class _rfn_chunk : public __chunk
{
public:
	_rfn_chunk() {}
	virtual int _gettype()				{ return _CT_RFONT;			}
};

// Color
class _col_chunk : public __chunk
{
public:
	unsigned char	_r, _g, _b, _a;

	_col_chunk( unsigned char r, unsigned char g, unsigned char b, unsigned char a =255) : _r( r), _g( g), _b( b), _a( a) {}
	virtual int _gettype()				{ return _CT_COLOR;			}
};

// Restore color
class _rco_chunk : public __chunk
{
public:
	_rco_chunk() {}
	virtual int _gettype()				{ return _CT_RCOLOR;		}
};

// Align
class _aln_chunk : public __chunk
{
public:
	unsigned char	_horz;
	unsigned char	_vert;

	_aln_chunk( unsigned char horz, unsigned char vert) : _horz( horz), _vert( vert) {}
	virtual int _gettype()				{ return _CT_ALIGN;			}
};

// Height of column
class _hco_chunk : public __chunk
{
public:
	unsigned char	_h;			// 0이면 자동

	_hco_chunk( int height) : _h( (unsigned char)height) {}
	virtual int _gettype()				{ return _CT_COLUMN;		}
};

// Paragraph spacing
class _prs_chunk : public __chunk
{
public:
	int				_spacing;

	_prs_chunk( int spacing =-1) : _spacing( spacing) {}
	virtual int _gettype()				{ return _CT_PRGSPC;		}
};

// Line spacing
class _lns_chunk : public __chunk
{
public:
	unsigned char	_spacing;

	_lns_chunk( int spacing) : _spacing( spacing) {}
	virtual int _gettype()				{ return _CT_LINESPC;		}
};

// Line feed
class _lnf_chunk : public __chunk
{
public:
	_lnf_chunk() {}
	virtual int _gettype()				{ return _CT_LINEFEED;		}
};

// Carriage return
class _crt_chunk : public __chunk
{
public:
	_crt_chunk() {}
	virtual int _gettype()				{ return _CT_CRETURN;		}
};

// Carriage return with height
class _crh_chunk : public __chunk
{
public:
	unsigned char	_h;

	_crh_chunk( int height) : _h( (unsigned char)height) {}
	virtual int _gettype()				{ return _CT_CRTHEIGHT;		}
};

// Indent
class _idt_chunk : public __chunk
{
public:
	int				_w;

	_idt_chunk( int w) : _w( w) {}
	virtual int _gettype()				{ return _CT_INDENT;		}
};

// Indent
class _odt_chunk : public __chunk
{
public:
	int				_w;

	_odt_chunk( int w) : _w( w) {}
	virtual int _gettype()				{ return _CT_OUTDENT;		}
};

// Clear indent
class _cdt_chunk : public __chunk
{
public:
	_cdt_chunk() {}
	virtual int _gettype()				{ return _CT_CLRDENT;		}
};

// Second indent
class _sid_chunk : public __chunk
{
public:
	int				_w;

	_sid_chunk( int w) : _w( w) {}
	virtual int _gettype()				{ return _CT_SECONDENT;		}
};

// End of chunk
class _end_chunk : public __chunk
{
public:
	_end_chunk() {}
	virtual int _gettype()				{ return _CT_END;			}
};

typedef vector< __chunk*>		MChunkList;








/// class : MLineContents
class MLineContents
{
/// Reference content info
public:
	struct MREFINFO
	{
		int				_ref;
		float			_x;
		float			_w;

		MREFINFO( int _ref_id, float x, float w) : _ref( _ref_id), _x( x), _w( w) {}
	};



/// Member variables
protected:
	vector< __chunk*>	m_LineContents;				// 라인 컨텐츠
	unsigned char		m_nCount;					// 컨텐츠 갯수
	unsigned short		m_nWidth;					// 라인 전체 넓이
	unsigned char		m_nColumnHeight;			// 행 높이
	unsigned char		m_nHeight;					// 전체 높이
	unsigned char		m_nAlign;					// 라인 정렬
	unsigned char		m_nSpaceCount;				// 스페이스의 개수
	unsigned char		m_nLastSpace;				// 마지막 스페이스의 넓이
	unsigned char		m_nIndent;					// 들여쓰기 위치
	float				m_fStartPos;				// 시작 위치
	float				m_fSpaceAddition;			// 띄어쓰기 가중치
	vector< MREFINFO*>	m_RefInfo;					// 참조 영역에 대한 정보



/// Member function
public:
	MLineContents();
	virtual ~MLineContents();

	bool AddContents( __chunk* pContents);

	__chunk* GetContents( int nIndex)				{ return m_LineContents[ nIndex];					}
	int GetCount() const							{ return (int)m_nCount;								}

	// 전체 넓이/높이를 구함
	int GetWholeWidth() const						{ return (int)m_nWidth;								}

	// 마지막 스페이스를 뺀 전체 넓이를 구함
	int GetWidth() const							{ return ((int)m_nWidth - (int)m_nLastSpace);		}
	
	// 행 높이를 설정함
	void SetColumnHeight( int nHeight)				{ m_nColumnHeight = (int)nHeight;					}
	int GetColumnHeight() const						{ return (int)m_nColumnHeight;						}

	// 전체 높이를 설정함
	void SetHeight( int nHeight)					{ m_nHeight = (int)nHeight;							}
	int GetHeight() const							{ return (int)m_nHeight;							}

	// 정렬을 설정 및 구함
	virtual void SetAlign( MAlignmentMode align)	{ m_nAlign = align;									}
	virtual MAlignmentMode GetAlign() const			{ return m_nAlign;									}

	// 라인에 들어있는 처리할 스페이스의 개수
	int GetSpaceCount() const						{ return ( (m_nLastSpace == 0) ? m_nSpaceCount : (m_nSpaceCount - 1));	}

	// 들여쓰기를 설정 및 구함
	void SetIndent( int indent)						{ m_nIndent = (unsigned char)indent;				}
	int GetIndent() const							{ return m_nIndent;									}

	// 시작위치를 구함
	float GetStartPos() const						{ return m_fStartPos;								}

	// 띄어쓰기 가중치를 구함
	float GetSpaceAddition() const					{ return m_fSpaceAddition;							}

	// 참조 영역 정보를 구함
	size_t GetRefInfoCount() const					{ return m_RefInfo.size();							}
	int GetRefInfoRef( int i) const					{ return m_RefInfo[ i]->_ref;						}
	MRECT GetRefInfoRect( int i) const				{ return MRECT( (int)m_RefInfo[ i]->_x, 0, (int)m_RefInfo[ i]->_w, (int)m_nColumnHeight);	}

	// 라인 정보 업데이트
	void UpdateLineInfo( MRECT& r);

	// 모두 삭제
	void Clear();
};










/// class : MTextViewLook
class MTextViewLook : public MLookBase
{
/// Member variables
protected:
	int		m_nColumnHeight;
	int		m_nParagraphSpacing;
	int		m_nLineSpacing;
	int		m_nIndent;
	int		m_nSecondent;



/// Member functions
public:
	MTextViewLook();

	void SetColumnHeight( int height)				{ m_nColumnHeight = height;				}
	int GetColumnHeight() const						{ return m_nColumnHeight;				}

	void SetParagraphSpacing( int spacing)			{ m_nParagraphSpacing = spacing;		}
	int GetParagraphSpacing() const					{ return m_nParagraphSpacing;			}

	void SetLineSpacing( int spacing)				{ m_nLineSpacing = spacing;				}
	int GetLineSpacing() const						{ return m_nLineSpacing;				}

	void SetIndent( int indent)						{ m_nIndent = indent;					}
	int GetIndent() const							{ return m_nIndent;						}

	void SetSecondent( int secondent)				{ m_nSecondent = secondent;				}
	int GetSecondent() const						{ return m_nSecondent;					}

	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;

protected:
	virtual void OnDrawText( MTextView* pTextView, MDrawContext* pDC);
};






/// struct MRefContent
struct MRefContect		{ int i;  MRECT r; };



/// class : MTextView
class MTextView : public MWidget
{
/// Structure of reference
protected:
	struct MTEXTVIEWREF
	{
		string		strText;
		string		strToolTip;

		MTEXTVIEWREF( const string& _text, const string& _tooltip) : strText( _text), strToolTip( _tooltip) {}
	};



/// Member variables
protected:
	MScrollBar*					m_pScrollbar;
	MChunkList					m_ChunkList;
	vector< MLineContents*>		m_LineContents;
	vector< MTEXTVIEWREF>		m_Reference;
	unsigned char				m_nReferenceIndex;
	unsigned char				m_nCaret;
	unsigned char				m_nMouseOver;
	MSIZE						m_PageSize;
	bool						m_bWrap;
	bool						m_bDoParseText;
	bool						m_bUpdateScrollBar;
	bool						m_bUpdateLineContents;
	bool						m_bDragAndDrop;



/// Member functions
public:
	MTextView( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~MTextView();

	// 페이지 사이즈를 구하는 함수
	MSIZE GetPageSize();

	// 텍스트 설정 및 구함
	virtual void SetText( const char* szText);
	virtual const char* GetText( void);

	/// 툴팁 메시지 설정 및 구함
	virtual void SetToolTipText( const char* szTooltip);
	virtual const char* GetToolTipText() const;

	// 텍스트를 삭제하는 함수
	void Clear();
	void ClearChunkList();
	void ClearLineContents();

	// 비어있는지 확인하는 함수
	bool IsEmpty() const										{ return m_ChunkList.empty();						}

	// 단어 단위로 래핑(Wrapping)을 하는지 여부를 설정하는 함수
	void SetWrapping( bool bWrap);
	bool IsWrapping() const										{ return m_bWrap;									}

	// 참조
	size_t GetRefCount();
	const char* GetRefText( int index)							{ return m_Reference[ index].strText.c_str();		}
	const char* GetRefToolTip( int index)						{ return m_Reference[ index].strToolTip.c_str();	}
	MRECT GetRefRect( int index);

	// 캐럿이 위치하고 있는 참조 인덱스를 구함
	int SetCaretPos( int index);
	int GetCaretPos()											{ return (int)m_nCaret;								}

	// 마우스가 위치하고 있는 참조 인덱스
	int GetMouseOver() const									{ return (int)m_nMouseOver;							}

	// 라인 컨텐츠 정보
	size_t GetLineContentsCount()								{ return m_LineContents.size();						}
	MLineContents* GetLineContents( size_t index)				{ return m_LineContents[ index];					}

	/// 드래그 앤 드롭
	virtual void EnableDragAndDrop( bool bEnable) override		{ m_bDragAndDrop = bEnable;							}
	virtual bool IsEnableDragAndDrop() const override			{ return m_bDragAndDrop;							}

	virtual MScrollBar* GetScrollBar() override					{ return m_pScrollbar;								}
	void ScrollPage( int delta);


protected:
	// 텍스트를 입력받으면 스트링과 태그를 파싱하여 chunk로 저장하는 함수
	bool ParseText( const wchar_t* c);

	// 분리된 chunk를 출력하기 편하도록 라인 단위로 재구성하는 함수. 리턴값으로 화면 영역을 넘어가는지 여부를 넘겨줌
	bool UpdateLineContents( bool bTestMode =false);

	// 해당 좌표에 해당하는 참조 컨텐츠를 찾는다
	MRefContect FindRefContent( MPOINT& pos);

	// 스크롤바를 업데이트 하는 함수
	void UpdateScrollbar()										{ m_bUpdateScrollBar = true;						}



protected:
	bool _parse_tag( const wchar_t* c);
 	bool _parse_bmp( const wchar_t* c);
 	bool _parse_spc( const wchar_t* c);
 	bool _parse_fnt( const wchar_t* c);
 	bool _parse_rfn( const wchar_t* c);
 	bool _parse_col( const wchar_t* c);
 	bool _parse_rco( const wchar_t* c);
 	bool _parse_aln( const wchar_t* c);
	bool _parse_hco( const wchar_t* c);
	bool _parse_prs( const wchar_t* c);
	bool _parse_lns( const wchar_t* c);
	bool _parse_crt( const wchar_t* c);
	bool _parse_idt( const wchar_t* c);
	bool _parse_odt( const wchar_t* c);
	bool _parse_cdt( const wchar_t* c);
	bool _parse_sid( const wchar_t* c);
	bool _parse_ref( const wchar_t* c);
	bool _parse_erf( const wchar_t* c);




/// Event handler
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual void OnChangedLookAndFill() override;
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnSize(int w, int h) override;
	virtual MWidget* OnToolTip() override;
	virtual void OnLeave() override;



/// Lua script
public:
	void glueClear()											{ Clear();											}
	int glueGetRefCount( int index)								{ return (int)GetRefCount();						}
	const char* glueGetRefText( int index)						{ return GetRefText( index);						}
	const char* glueGetRefToolTip( int index)					{ return GetRefToolTip( index);						}
	MWLua::ret_int4 glueGetRefRect( int index);
	int glueSetCaretPos( int index)								{ return SetCaretPos( index);						}
	int glueGetCaretPos()										{ return GetCaretPos();								}
	int glueGetMouseOver()										{ return GetMouseOver();							}
	void glueScrollPage( int delta)								{ ScrollPage( delta);								}
	MWLua::ret_int2 glueGetPageSize();



#define MINT_TEXTVIEW	"TextView"
	virtual const char* GetClassName( void)						{ return MINT_TEXTVIEW;								}
};

} // namespace mint3