#pragma once

#include "MintTypes.h"
#include "MBitmap.h"
#include "MFont.h"
using namespace std;



namespace mint3
{


/// class : MDrawContext
class MDrawContext
{
protected:
	MCOLOR			m_Color;			///< 현재 선택되어 있는 컬러
	MBitmap*		m_pBitmap;			///< 현재 선택되어 있는 비트맵
	MCOLOR			m_BitmapColor;		/// < 비트맵에 섞는 색 (기본값 : White(255,255,255,255) )
	MBitmap*		m_pMaskBitmap;		///< 현재 선택되어 있는 마스킹 비트맵
	MRECT			m_MaskRect;			///< 마스킹 비트맵 출력 영역
	MFont*			m_pFont;			///< 현재 선택되어 있는 폰트
	MRECT			m_Clip;				///< 스크린 좌표계에서의 클리핑 영역
	MPOINT			m_Origin;			///< 스크린에 그려질때 현재 논리적 원점
	MDrawEffect		m_Effect;			///< 효과
	float			m_fTransparency;	///< 전체 투명도
	float			m_fOpacity;			///< 투명도
#ifdef _DEBUG
	int				m_nTypeID;
#endif



public:
	MDrawContext();
	virtual ~MDrawContext();

	// Set color
	MCOLOR SetColor( MCOLOR& color)										{ return SetColor( color.r, color.g, color.b, color.a);			}
	MCOLOR SetColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a=255);
	MCOLOR GetColor() const												{ return m_Color;								}

	// Set bitmap
	MBitmap* SetBitmap( MBitmap* pBitmap);
	MBitmap* GetBitmap()												{ return m_pBitmap;								}

	// Set bitmap color
	MCOLOR SetBitmapColor( MCOLOR& color)								{ return SetBitmapColor( color.r, color.g, color.b, color.a);	}
	MCOLOR SetBitmapColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a=255);
	MCOLOR GetBitmapColor() const										{ return m_BitmapColor;							}

	// Set mask bitmap
	MBitmap* SetMaskBitmap( MBitmap* pBitmap);
	MBitmap* GetMaskBitmap()											{ return m_pMaskBitmap;							}

	// Set mask rectangle
	MRECT SetMaskRect( MRECT& r =MRECT(0,0,-1,-1))						{ return SetMaskRect( r.x, r.y, r.w, r.h);		}
	MRECT SetMaskRect( int x, int y, int w =-1, int h =-1);
	MRECT GetMaskRect() const											{ return m_MaskRect;							}

	// Font Setting
	MFont* SetFont( MFont* pFont);
	MFont* SetFont( const char* szFontName);
	MFont* GetFont()													{ return m_pFont;								}

	// Set clipping
	MRECT SetClipRect( MRECT& r)										{ return SetClipRect( r.x, r.y, r.w, r.h);		}
	MRECT SetClipRect( int x, int y, int w, int h);
	MRECT GetClipRect() const											{ return m_Clip;								}

	// Set origin
	MPOINT SetOrigin( MPOINT& p)										{ return SetOrigin( p.x, p.y);					}
	MPOINT SetOrigin( int x, int y);
	MPOINT GetOrigin() const											{ return m_Origin;								}

	// Draw Effect
	MDrawEffect SetEffect( MDrawEffect effect);
	MDrawEffect GetEffect() const										{ return m_Effect;								}

	// Transparency를 설정한다.
	float SetTransparency( float fTransparency);
	float GetTransparency() const										{ return m_fTransparency;						}

	// Opacity를 설정한다.
	float SetOpacity( float fOpacity);
	float GetOpacity() const											{ return m_fOpacity;							}



	// Basic Drawing Functions
	void BeginDraw()													{ overrideBeginDraw();							}
	void EndDraw()														{ overrideEndDraw();							}

	// MDrawContext, MBitmap, MFont의 State를 변경한다.
	void SetState( MDrawState eState, MDrawEffect eEffect =MDE_NORMAL)	{ overrideSetState( eState, eEffect);			}

	// Basic draw functions
	void SetPixel( MPOINT& p)											{ overrideSetPixel( p.x, p.y);					}
	void SetPixel( int x, int y)										{ overrideSetPixel( x, y);						}
	void Line( int sx, int sy, int ex, int ey)							{ overrideLine( sx, sy, ex, ey);				}
	void Rectangle( MRECT& r)											{ overrideRectangle( r.x, r.y, r.w, r.h);		}
	void Rectangle( int x, int y, int w, int h)							{ overrideRectangle( x, y, w, h);				}
	void FillRectangle( MRECT& r)										{ overrideFillRectangle( r.x, r.y, r.w, r.h);	}
	void FillRectangle( int x, int y, int w, int h)						{ overrideFillRectangle( x, y, w, h);			}


	// Draw bitmap
	void Draw( MPOINT& p)												{ Draw( p.x, p.y);								}
	void Draw( int x, int y);
	void Draw( MRECT &r)												{ Draw( r.x, r.y, r.w, r.h);					}
	void Draw( int x, int y, int w, int h);
	void Draw( int tx, int ty, int sx, int sy, int sw, int sh)					{ overrideDraw( tx, ty, sw, sh, sx, sy, sw, sh);	}
	void Draw( int tx, int ty, int tw, int th, int sx, int sy, int sw, int sh)	{ overrideDraw( tx, ty, tw, th, sx, sy, sw, sh);	}
	void DrawRotate( int tx, int ty, int tw, int th, int rx, int ry, float ra, int sx, int sy, int sw, int sh);

	// Draw text
	void Text( MPOINT& p, const char* szText)							{ Text( p.x, p.y, szText);						}
	void Text( int x, int y, const char* szText);
	void Text( MRECT& r, const char* szText, MAlignmentMode am =0)		{ Text( r.x, r.y, r.w, r.h, szText, am);		}
	void Text( int x, int y, int w, int h, const char* szText, MAlignmentMode am =0);

	// Flush font
	void FlushFont()													{ overrideFlushFont();							}


/// Override functions
protected:
	virtual void overrideBeginDraw()																	{}
	virtual void overrideEndDraw()																		{}
	virtual void overrideSetState( MDrawState eState, MDrawEffect eEffect)								{}
	virtual void overrideSetPixel( int x, int y)														{}
	virtual void overrideLine( int sx, int sy, int ex, int ey)											{}
	virtual void overrideRectangle( int x, int y, int w, int h)											{}
	virtual void overrideFillRectangle( int x, int y, int w, int h)										{}
	virtual void overrideClipRect( int x, int y, int w, int h)											{}
	virtual void overrideDraw( int tx, int ty, int tw, int th, int sx, int sy, int sw, int sh)			{}
	virtual void overrideDrawRotate( int tx, int ty, int tw, int th, int rx, int ry, float ra, int sx, int sy, int sw, int sh)	{}
	virtual void overrideText( int x, int y, const char* szText)										{}
	virtual void overrideFlushFont()																	{}
};


}
