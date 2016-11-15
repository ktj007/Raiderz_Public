#include "stdafx.h"
#include "MDrawContext.h"
#include "Mint.h"
#include "MResourceManager.h"


namespace mint3 {


#define MWIDGET_TEXT_LENGTH			256			///< 위젯 텍스트 최대 길이



// MDrawContex
MDrawContext::MDrawContext(void)
{
	m_Color = 0x00000000;
	m_pBitmap = NULL;
	m_BitmapColor = 0xFFFFFFFF;
	m_pMaskBitmap = NULL;
	m_MaskRect = MRECT( 0, 0, -1, -1);
	m_pFont = 0;
	m_Clip = MRECT( 0, 0, MGetWorkspaceWidth(), MGetWorkspaceHeight());
	m_Origin = MPOINT( 0, 0);
	m_Effect = MDE_NORMAL;
	m_fTransparency = 1.0f;
	m_fOpacity = 1.0f;
#ifdef _DEBUG
	m_nTypeID = 0;
#endif
}


MDrawContext::~MDrawContext()
{
	m_pBitmap = NULL;
	m_pMaskBitmap = NULL;
}


MCOLOR MDrawContext::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	MCOLOR temp = m_Color;
	m_Color = MCOLOR( r, g, b, a);

	return temp;
}


MBitmap* MDrawContext::SetBitmap( MBitmap* pBitmap)
{
	if ( m_pBitmap == pBitmap)		return m_pBitmap;

	MBitmap* temp = m_pBitmap;
	m_pBitmap = pBitmap;
	return temp;
}


MCOLOR MDrawContext::SetBitmapColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	MCOLOR temp = m_BitmapColor;

	m_BitmapColor.r = r;
	m_BitmapColor.g = g;
	m_BitmapColor.b = b;
	m_BitmapColor.a = a;

	return temp;
}


MBitmap* MDrawContext::SetMaskBitmap( MBitmap* pBitmap)
{
	if ( m_pMaskBitmap == pBitmap)	return m_pMaskBitmap;

	MBitmap* temp = m_pMaskBitmap;
	m_pMaskBitmap = pBitmap;
	return temp;
}


MRECT MDrawContext::SetMaskRect( int x, int y, int w, int h)
{
	MRECT temp = m_MaskRect;

	m_MaskRect.x = x;
	m_MaskRect.y = y;
	m_MaskRect.w = w;
	m_MaskRect.h = h;

	return temp;
}


MFont* MDrawContext::SetFont( MFont* pFont)
{
	if ( m_pFont == pFont)			return m_pFont;

	MFont* temp = m_pFont;
	m_pFont = (m_pFont == NULL)  ?  MFontManager::Get( 0) : pFont;
	return temp;
}


MFont* MDrawContext::SetFont( const char* szFontName)
{
	return SetFont( MFontManager::Get( szFontName));
}


MRECT MDrawContext::SetClipRect( int x, int y, int w, int h)
{
	MRECT temp = m_Clip;

	m_Clip.x = x;
	m_Clip.y = y;
	m_Clip.w = w;
	m_Clip.h = h;

	m_Origin.x = x;
	m_Origin.y = y;

	overrideClipRect( x, y, w, h);

	return temp;
}


MPOINT MDrawContext::SetOrigin(int x, int y)
{
	MPOINT temp = m_Origin;

	m_Origin.x = x;
	m_Origin.y = y;

	return temp;
}


MDrawEffect MDrawContext::SetEffect( MDrawEffect effect)
{
	MDrawEffect temp = m_Effect;
	m_Effect = effect;
	return temp;
}


float MDrawContext::SetTransparency( float fTransparency)
{
	fTransparency = min( 1.0f, max( 0.0f, fTransparency));

	float temp = fTransparency;
	m_fTransparency = fTransparency;
	return temp;
}


float MDrawContext::SetOpacity( float fOpacity)
{
	fOpacity = min( 1.0f, max( 0.0f, fOpacity));

	float temp = m_fOpacity;
	m_fOpacity = fOpacity;
	return temp;
}


void MDrawContext::Draw( int x, int y)
{
	if ( m_pBitmap == NULL)				return;
	int _w = m_pBitmap->GetWidth();
	int _h = m_pBitmap->GetHeight();
	overrideDraw( x, y, _w, _h, 0, 0, _w, _h);
}


void MDrawContext::Draw( int x, int y, int w, int h)
{
	if ( m_pBitmap==NULL) return;
	int _w = m_pBitmap->GetWidth();
	int _h = m_pBitmap->GetHeight();
	overrideDraw( x, y, w, h, 0, 0, _w, _h);
}


/*	DrawRotate

	x, y, w, h = 이미지를 출력할 타겟 위치 및 사이즈
	rx, ry = 이미지를 회전시키는 회전 축 좌표
	ra = 회전 각도(radian)
	sx, sy, sw, sh = 이미지를 가져올 소스 위치 및 사이즈
*/
void MDrawContext::DrawRotate( int tx, int ty, int tw, int th, int rx, int ry, float ra, int sx, int sy, int sw, int sh)
{
	overrideDrawRotate( tx, ty, tw, th, rx, ry, ra, sx, sy, sw, sh);
}


void MDrawContext::Text( int x, int y, const char* szText)
{
	if ( m_pFont == NULL  ||  szText[ 0] == 0)		return;

	overrideText( x, y, szText);
}


void MDrawContext::Text( int x, int y, int w, int h, const char* szText, MAlignmentMode am)
{
	if ( m_pFont == NULL  ||  szText[ 0] == 0)		return;

	MPOINT p( x, y);
	MSIZE _size( m_pFont->GetWidth( szText), m_pFont->GetHeight());

	bool bOutline = m_pFont->GetOutline() > 0.001f  ?  true : false;

	if ( am & MAM_HCENTER)			p.x += (int)((float)(w - _size.w) - 0.5f) >> 1;
	else if ( am & MAM_RIGHT)		p.x += w - _size.w - ( bOutline ? 1 : 0);

	if ( am & MAM_VCENTER)			p.y += (int)((float)(h - _size.h) - 0.5f) >> 1;
	else if ( am & MAM_BOTTOM)		p.y += h - _size.h - ( bOutline ? 1 : 0);

	overrideText( p.x, p.y, szText);
}


}
