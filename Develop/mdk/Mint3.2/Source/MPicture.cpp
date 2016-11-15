#include "stdafx.h"
#include "MPicture.h"
#include "MResourceManager.h"



// namespace : mint3
namespace mint3
{



// MPicture
MPicture::MPicture( const char* szName /*=NULL*/, MWidget* pParent /*=NULL*/, MListener* pListener /*=NULL*/)
: MWidget( szName, pParent, pListener)
{
	m_bStatic = true;

	m_pBitmap = NULL;
	m_bStretch = false;	
	m_Effect = MDE_NORMAL;
	m_dwRotate = MBM_NORMAL;
	m_Colorize = MCOLOR( 255, 255, 255);
}


// SetStretch
bool MPicture::SetStretch( bool bStretch)
{
	bool bRetVal = m_bStretch;
	m_bStretch = bStretch;
	return bRetVal;
}


// SetEffect
MDrawEffect MPicture::SetEffect( MDrawEffect _effect)
{
	MDrawEffect _RetVal = m_Effect;
	m_Effect = _effect;
	return _RetVal;
}


// SetColorize
MCOLOR MPicture::SetColorize( MCOLOR& color)
{
	MCOLOR _RetVal = m_Colorize;
	m_Colorize = color;
	return _RetVal;
}


// SetFlipHorizontal
void MPicture::SetFlipHorizontal()
{
	unsigned int nBit90L = MBM_RotL90;
	unsigned int nBit90R = MBM_RotR90;

	unsigned int nBit = ((m_dwRotate & nBit90L) || (m_dwRotate & nBit90R))  ?  MBM_FlipUD : MBM_FlipLR;


	// 이미 좌우를 뒤집은 상태면...
	if ( m_dwRotate & nBit)		m_dwRotate &= ~nBit;
	else						m_dwRotate |= nBit;
}


// SetFlipVertical
void MPicture::SetFlipVertical()
{
	unsigned int nBit90L = MBM_RotL90;
	unsigned int nBit90R = MBM_RotR90;
	unsigned int nBit = ((m_dwRotate & nBit90L) || (m_dwRotate & nBit90R))  ?  MBM_FlipLR : MBM_FlipUD;

	if ( m_dwRotate & nBit)		m_dwRotate &= ~nBit;
	else						m_dwRotate |= nBit;
}


// SetRotate90CW
void MPicture::SetRotate90CW()
{
	unsigned int nBit90L = MBM_RotL90;
	unsigned int nBit90R = MBM_RotR90;

	if ( m_dwRotate & nBit90L)
		m_dwRotate &= ~nBit90L;
	else if ( m_dwRotate & nBit90R)
	{
		SetRotate180();

		m_dwRotate &= ~nBit90R;
	}
	else
		m_dwRotate |= nBit90R;
}


// SetRotate90CCW
void MPicture::SetRotate90CCW()
{
	unsigned int nBit90L = MBM_RotL90;
	unsigned int nBit90R = MBM_RotR90;

	if ( m_dwRotate & nBit90R)
		m_dwRotate &= ~nBit90R;
	else if ( m_dwRotate & nBit90L)
	{
		SetRotate180();

		m_dwRotate &= ~nBit90L;
	}
	else
		m_dwRotate |= nBit90L;
}


// SetRotate180
void MPicture::SetRotate180()
{
	SetFlipHorizontal();
	SetFlipVertical();
}


// OnDraw
void MPicture::OnDraw( MDrawContext* pDC)
{
	if ( m_pBitmap == NULL)		return;

	DWORD _old_drawmode = m_pBitmap->GetDrawMode();
	m_pBitmap->SetDrawMode( m_dwRotate);

	MCOLOR _old_color = pDC->GetBitmapColor();
	pDC->SetBitmapColor( m_Colorize);


	pDC->SetBitmap( m_pBitmap);

	MDrawEffect _old_effect = pDC->SetEffect( m_Effect);

	if ( m_bStretch)
	{
		MRECT rect = GetClientRect();
		pDC->Draw( rect.x, rect.y, rect.w, rect.h, 0, 0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight());
	}
	else
		pDC->Draw( 0, 0, 0, 0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight());

	pDC->SetEffect( _old_effect);

	pDC->SetBitmapColor( _old_color);
	m_pBitmap->SetDrawMode( _old_drawmode);


	MWidget::OnDraw( pDC);
}


// glueSetBitmap
void MPicture::glueSetImage( const char* szBitmapName)
{
	MBitmap* pBitmap = MBitmapManager::Get( szBitmapName);
	SetBitmap(pBitmap);
}

// glueGetImage
const char* MPicture::glueGetImage()
{
	MBitmap* pBitmap = GetBitmap();
	if ( pBitmap)	return pBitmap->GetName();
	return "";
}


// SetEffect
void MPicture::glueSetEffect( const char* szEffect)
{
	MDrawEffect _effect = MDE_NORMAL;
	if ( strcmp( szEffect, "add") == 0)				_effect = MDE_ADD;
	else if ( strcmp( szEffect, "multiply") == 0)	_effect = MDE_MULTIPLY;
	SetEffect( _effect);
}


// GetEffect
const char* MPicture::glueGetEffect()
{
	MDrawEffect _effect = GetEffect();
	static string _str;
	switch ( _effect)
	{
	case MDE_NORMAL :		_str = "normal";		break;
	case MDE_ADD :			_str = "add";			break;
	case MDE_MULTIPLY :		_str = "multiply";		break;
	}
	return _str.c_str();
}


// glueSetColorize
MWLua::ret_int4 MPicture::glueSetColorize( int r, int g, int b)
{
	MWLua::ret_int4 _col( m_Colorize.r, m_Colorize.g, m_Colorize.b, m_Colorize.a);
	SetColorize( MCOLOR( r, g, b));
	return _col;
}


// glueGetColorize
MWLua::ret_int4 MPicture::glueGetColorize()
{
	MCOLOR _col = GetColorize();
	return MWLua::ret_int4( _col.r, _col.g, _col.b, _col.a);
}


} // namespace mint3
