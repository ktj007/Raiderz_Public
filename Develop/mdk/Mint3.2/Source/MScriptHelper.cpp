#include "stdafx.h"
#include "MScriptHelper.h"
#include "Mint.h"
#include "MDrawContext.h"
#include "MColorTable.h"
#include "MResourceManager.h"
#include "MLocale.h"

namespace mint3 {


MScriptHelper::MScriptHelper()
{
	m_nAlign = MAM_LEFT | MAM_TOP;
}


void MScriptHelper::Line( int x1, int y1, int x2, int y2)
{
	MDrawContext* pDC = MGetMint()->GetDC();
	if ( pDC != NULL)	pDC->Line( x1, y1, x2, y2);
}


void MScriptHelper::Rect( int x, int y, int w, int h)
{
	MDrawContext* pDC = MGetMint()->GetDC();
	if ( pDC != NULL)	pDC->Rectangle( x, y, w - 1, h - 1);
}


void MScriptHelper::FillRect( int x, int y, int w, int h)
{
	MDrawContext* pDC = MGetMint()->GetDC();
	if ( pDC != NULL)	pDC->FillRectangle( x, y, w, h);
}


void MScriptHelper::Draw( int x, int y, int w, int h)
{
	MDrawContext* pDC = MGetMint()->GetDC();
	if ( pDC != NULL)	pDC->Draw( x, y, w, h);
}


void MScriptHelper::DrawEx( int tx, int ty, int tw, int th, int sx, int sy, int sw, int sh)
{
	MDrawContext* pDC = MGetMint()->GetDC();
	if ( pDC != NULL)	pDC->Draw( tx, ty, tw, th, sx, sy, sw, sh);
}


void MScriptHelper::Text( int x, int y, const char* szText)
{
	MDrawContext* pDC = MGetMint()->GetDC();
	if ( pDC != NULL)	pDC->Text( x, y, szText);
}


void MScriptHelper::TextEx( int x, int y, int w, int h, const char* szText)
{
	MDrawContext* pDC = MGetMint()->GetDC();
	if ( pDC != NULL)	pDC->Text( MRECT( x, y, w, h), szText, m_nAlign);
}


MWLua::ret_int4 MScriptHelper::SetColor( int r, int g, int b)
{
	MDrawContext* pDC = MGetMint()->GetDC();
	MCOLOR col( 255, 255, 255);
	if ( pDC != NULL)		col = pDC->SetColor( r, g, b);
	return MWLua::ret_int4( col.r, col.g, col.b, col.a);
}


MWLua::ret_int4 MScriptHelper::SetColorEx( int r, int g, int b, int a)
{
	MDrawContext* pDC = MGetMint()->GetDC();
	MCOLOR col( 255, 255, 255);
	if ( pDC != NULL)	col = pDC->SetColor( r, g, b, a);
	return MWLua::ret_int4( col.r, col.g, col.b, col.a);
}


MWLua::ret_int4 MScriptHelper::GetColor() const
{
	MDrawContext* pDC = MGetMint()->GetDC();
	MCOLOR col( 255, 255, 255);
	if ( pDC != NULL)	col = pDC->GetColor();
	return MWLua::ret_int4( col.r, col.g, col.b, col.a);
}


MWLua::ret_int4 MScriptHelper::SetBitmapColor( int r, int g, int b)
{
	MDrawContext* pDC = MGetMint()->GetDC();
	MCOLOR col( 255, 255, 255);
	if ( pDC != NULL)		col = pDC->SetBitmapColor( r, g, b);
	return MWLua::ret_int4( col.r, col.g, col.b, col.a);
}


MWLua::ret_int4 MScriptHelper::SetBitmapColorEx( int r, int g, int b, int a)
{
	MDrawContext* pDC = MGetMint()->GetDC();
	MCOLOR col( 255, 255, 255);
	if ( pDC != NULL)	col = pDC->SetBitmapColor( r, g, b, a);
	return MWLua::ret_int4( col.r, col.g, col.b, col.a);
}


MWLua::ret_int4 MScriptHelper::GetBitmapColor() const
{
	MDrawContext* pDC = MGetMint()->GetDC();
	MCOLOR col( 255, 255, 255);
	if ( pDC != NULL)	col = pDC->GetBitmapColor();
	return MWLua::ret_int4( col.r, col.g, col.b, col.a);
}


const char* MScriptHelper::SetEffect( const char* szEffect)
{
	MDrawContext* pDC = MGetMint()->GetDC();

	MDrawEffect _effect = MDE_NORMAL;
	if ( strcmp( szEffect, "add") == 0)				_effect = MDE_ADD;
	else if ( strcmp( szEffect, "multiply") == 0)	_effect = MDE_MULTIPLY;

	MDrawEffect _old_effect = pDC->SetEffect( _effect);

	if ( _old_effect == MDE_ADD)		return string( "add").c_str();
	else if ( _old_effect == MDE_ADD)	return string( "multiply").c_str();
	return string( "normal").c_str();
}


const char* MScriptHelper::GetEffect()
{
	MDrawContext* pDC = MGetMint()->GetDC();
	MDrawEffect _effect = pDC->GetEffect();

	if ( _effect == MDE_ADD)			return string( "add").c_str();
	else if ( _effect == MDE_ADD)		return string( "multiply").c_str();
	return string( "normal").c_str();
}


float MScriptHelper::SetOpacity( float fOpacity)
{
	MDrawContext* pDC = MGetMint()->GetDC();
	if ( pDC != NULL)	return pDC->SetOpacity( fOpacity);
	return 1.0f;
}


float MScriptHelper::GetOpacity() const
{
	MDrawContext* pDC = MGetMint()->GetDC();
	if ( pDC != NULL)	return pDC->GetOpacity();
	return 1.0f;
}

void MScriptHelper::SetBitmap( const char* szBitmap)
{
	MDrawContext* pDC = MGetMint()->GetDC();
	if ( pDC != NULL)	pDC->SetBitmap( MBitmapManager::Get( szBitmap));
}


const char* MScriptHelper::GetBitmap() const
{
	MDrawContext* pDC = MGetMint()->GetDC();
	if ( pDC != NULL)	return pDC->GetBitmap()->GetName();
	return "";
}


void MScriptHelper::SetFont( const char* szFont)
{
	MDrawContext* pDC = MGetMint()->GetDC();
	if ( pDC != NULL)	pDC->SetFont( MFontManager::Get( szFont));
}


void MScriptHelper::SetTextAlign( const char* szHorz, const char* szVert)
{
	string strHorz( szHorz);
	transform( strHorz.begin(), strHorz.end(), strHorz.begin(), tolower);

	string strVert( szVert);
	transform( strVert.begin(), strVert.end(), strVert.begin(), tolower);

	MAlignmentMode _align = 0;
	if ( strHorz == "left")				_align |= MAM_LEFT;
	else if ( strHorz == "right")		_align |= MAM_RIGHT;
	else if ( strHorz == "center")		_align |= MAM_HCENTER;
	else if ( strHorz == "hcenter")		_align |= MAM_HCENTER;

	if ( strVert == "top")				_align |= MAM_TOP;
	else if ( strVert == "bottom")		_align |= MAM_BOTTOM;
	else if ( strVert == "center")		_align |= MAM_VCENTER;
	else if ( strVert == "vcenter")		_align |= MAM_VCENTER;

	if ( _align == 0)					_align = MAM_LEFT | MAM_TOP;

	m_nAlign = _align;
}


int MScriptHelper::GetTextWidth( const char* szFont, const char* szText)
{
	MFont* pFont = MFontManager::Get( szFont);

	if ( pFont != NULL)		return pFont->GetWidth( szText);
	return 0;
}


const char* MScriptHelper::SubTextWidth( const char* szFont, const char* szText, int nWidth)
{
	wstring strText = MLocale::ConvAnsiToUTF16( szText);

	MFont* pFont = MFontManager::Get( szFont);
	if ( pFont != NULL)
	{
		if ( pFont->GetWidth( szText) > nWidth)
		{
			while ( strText.length() > 3)
			{
				strText = strText.substr( 0, strText.length() - 1);

				wstring _str = strText + wstring( L"...");

				if ( pFont->GetWidth( MLocale::ConvUTF16ToAnsi( _str).c_str()) <= nWidth)
				{
					strText = _str;
					break;
				}
			}
		}
	}

	static string strRetVal;
	strRetVal = MLocale::ConvUTF16ToAnsi( strText);
	return strRetVal.c_str();
}


} // namespace mint3
