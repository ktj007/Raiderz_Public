#include "stdafx.h"
#include "MSlideShow.h"
#include "MResourceManager.h"
#include "Mint.h"
#include <math.h>


// namespace : mint3
namespace mint3
{



// MSlideShow
MSlideShow::MSlideShow( const char* szName /*=NULL*/, MWidget* pParent /*=NULL*/, MListener* pListener /*=NULL*/)
: MWidget( szName, pParent, pListener)
{
	m_bStatic = true;

	m_pBitmap = NULL;
	m_nRow = 0;
	m_nCol = 0;
	m_SceneSize = MSIZE( 0, 0);
	m_nMaxSceneCount = 0;
	m_nSceneCount = 0;
	m_nCurScene = 0;
	m_nPrevScene = 0;
	m_bSlideShow = false;
	m_nDelayTime = 3000;
	m_dwSlideShowTimer = 0;
	m_tFade.SetArrivalTime( 1000);
	m_bPlayOnce = false;
	m_Effect = MDE_NORMAL;
	m_dwRotate = MBM_NORMAL;
	m_Colorize = MCOLOR( 255, 255, 255);
}


// SetBitmap
void MSlideShow::SetBitmap( MBitmap* pBitmap)
{
	if ( m_pBitmap == pBitmap)		return;


	if ( pBitmap == NULL)
	{
		m_pBitmap = NULL;
		m_nRow = 0;
		m_nCol = 0;
		m_SceneSize = MSIZE( 0, 0);
		m_nMaxSceneCount = 0;
		m_nSceneCount = 0;
		m_nCurScene = 0;
		m_nPrevScene = 0;
		m_bSlideShow = false;
	}
	else
	{
		m_pBitmap = pBitmap;
		m_nRow = 1;
		m_nCol = 1;
		m_SceneSize = MSIZE( pBitmap->GetWidth(), pBitmap->GetHeight());
		m_nMaxSceneCount = 1;
		m_nSceneCount = 1;
		m_nCurScene = 0;
		m_nPrevScene = 0;
		m_bSlideShow = false;
	}
}


// SliceScene
int MSlideShow::SliceScene( int _row, int _col)
{
	if ( _row < 0)		_row = 0;
	if ( _col < 0)		_col = 0;

	m_nRow = _row + 1;
	m_nCol = _col + 1;
	m_SceneSize = MSIZE( m_pBitmap->GetWidth() / m_nRow, m_pBitmap->GetHeight() / m_nCol);
	m_nMaxSceneCount = m_nRow * m_nCol;
	m_nSceneCount = m_nMaxSceneCount;
	m_nCurScene = 0;
	m_nPrevScene = 0;
	m_bSlideShow = false;

	return m_nMaxSceneCount;
}


// SetSceneCount
int MSlideShow::SetSceneCount( int nCount)
{
	if ( nCount < 0)							nCount = 0;
	if ( nCount > (int)m_nMaxSceneCount)		nCount = m_nMaxSceneCount;

	m_nSceneCount = nCount;
	m_nCurScene = 0;
	m_nPrevScene = 0;
	m_bSlideShow = false;

	return m_nSceneCount;
}


// SetCurScene
int MSlideShow::SetCurScene( int nCurScene)
{
	if ( nCurScene >= (int)m_nSceneCount)		nCurScene = m_nSceneCount - 1;
	if ( nCurScene < 0)							nCurScene = 0;

	if ( m_nCurScene == nCurScene)				return m_nCurScene;

	m_nPrevScene = m_nCurScene;
	m_nCurScene = nCurScene;
	m_bSlideShow = false;
	m_tFade.Start();

	return m_nCurScene;
}


// SetSlideShow
bool MSlideShow::SlideShow( bool bSlideShow)
{
	if ( m_pBitmap == NULL)						return false;
	if ( m_bSlideShow == bSlideShow)			return m_bSlideShow;

	m_bSlideShow = bSlideShow;
	m_dwSlideShowTimer = MGetMint()->GetUpdateTime();

	return m_bSlideShow;
}


// SetEffect
MDrawEffect MSlideShow::SetEffect( MDrawEffect _effect)
{
	MDrawEffect _RetVal = m_Effect;
	m_Effect = _effect;
	return _RetVal;
}


// SetColorize
MCOLOR MSlideShow::SetColorize( MCOLOR& color)
{
	MCOLOR _RetVal = m_Colorize;
	m_Colorize = color;
	return _RetVal;
}


// SetFlipHorizontal
void MSlideShow::SetFlipHorizontal()
{
	unsigned int nBit90L = MBM_RotL90;
	unsigned int nBit90R = MBM_RotR90;
	unsigned int nBit = ((m_dwRotate & nBit90L) || (m_dwRotate & nBit90R))  ?  MBM_FlipUD : MBM_FlipLR;

	if ( m_dwRotate & nBit)		m_dwRotate &= ~nBit;
	else						m_dwRotate |= nBit;
}


// SetFlipVertical
void MSlideShow::SetFlipVertical()
{
	unsigned int nBit90L = MBM_RotL90;
	unsigned int nBit90R = MBM_RotR90;
	unsigned int nBit = ((m_dwRotate & nBit90L) || (m_dwRotate & nBit90R))  ?  MBM_FlipLR : MBM_FlipUD;

	if ( m_dwRotate & nBit)		m_dwRotate &= ~nBit;
	else						m_dwRotate |= nBit;
}


// SetRotate90CW
void MSlideShow::SetRotate90CW()
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
void MSlideShow::SetRotate90CCW()
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
void MSlideShow::SetRotate180()
{
	SetFlipHorizontal();
	SetFlipVertical();
}


// GetSceneRect
MRECT MSlideShow::GetSceneRect( int nSceneNum) const
{
	MRECT r;
	r.x = m_SceneSize.w * (nSceneNum % m_nRow);
	r.y = m_SceneSize.h * (nSceneNum / m_nRow);
	r.w = m_SceneSize.w;
	r.h = m_SceneSize.h;
	return r;
}


// OnRun
void MSlideShow::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);

	if ( m_bSlideShow == true)
	{
		DWORD _diff = dwCurrTime - m_dwSlideShowTimer;
		if ( _diff > m_nDelayTime)
		{
			int nCount = _diff / m_nDelayTime;
			m_dwSlideShowTimer += nCount * m_nDelayTime;

			int nNextScene = m_nCurScene + 1;
			if ( nNextScene >= (int)m_nSceneCount)
				nNextScene = 0;

			m_nPrevScene = m_nCurScene;
			m_nCurScene = nNextScene;
			
			m_tFade.Start();

			if ( m_bPlayOnce == true  &&  nNextScene == 0)
				m_bSlideShow = false;

			OnSelChanged();
		}
	}
}


// OnDraw
void MSlideShow::OnDraw( MDrawContext* pDC)
{
	MWidget::OnDraw( pDC);


	if ( m_pBitmap == NULL)		return;


	DWORD _old_drawmode = m_pBitmap->GetDrawMode();
	m_pBitmap->SetDrawMode( m_dwRotate);

	MCOLOR _old_color = pDC->GetBitmapColor();
	pDC->SetBitmapColor( m_Colorize);

	MDrawEffect _old_effect = pDC->SetEffect( m_Effect);


	MRECT r = GetClientRect();

	if ( m_nCurScene != m_nPrevScene  &&  m_tFade.IsArrival() == false)
	{
		float fOpacity = pDC->GetOpacity();
		float fRatio = (float)sqrt( m_tFade.GetRatio());

		MRECT br = GetSceneRect( m_nCurScene);
		pDC->SetBitmap( m_pBitmap);
		pDC->SetOpacity( fOpacity * fRatio);
		pDC->Draw( r.x, r.y, r.w, r.h, br.x, br.y, br.w, br.h);

		br = GetSceneRect( m_nPrevScene);
		pDC->SetOpacity( fOpacity * (1.0f - fRatio));
		pDC->Draw( r.x, r.y, r.w, r.h, br.x, br.y, br.w, br.h);

		pDC->SetOpacity( fOpacity);
	}
	else
	{
		MRECT br = GetSceneRect( m_nCurScene);
		pDC->SetBitmap( m_pBitmap);
		pDC->Draw( r.x, r.y, r.w, r.h, br.x, br.y, br.w, br.h);
	}

	pDC->SetEffect( _old_effect);
	pDC->SetBitmapColor( _old_color);
	m_pBitmap->SetDrawMode( _old_drawmode);
}


// glueSetBitmap
void MSlideShow::glueSetImage( const char* szName)
{
	SetBitmap( MBitmapManager::Get( szName));
}


// glueGetImage
const char* MSlideShow::glueGetImage()
{
	if ( m_pBitmap)			return m_pBitmap->GetName();
	return "";
}


// SetEffect
void MSlideShow::glueSetEffect( const char* szEffect)
{
	MDrawEffect _effect = MDE_NORMAL;
	if ( strcmp( szEffect, "add") == 0)				_effect = MDE_ADD;
	else if ( strcmp( szEffect, "multiply") == 0)	_effect = MDE_MULTIPLY;
	SetEffect( _effect);
}


// GetEffect
const char* MSlideShow::glueGetEffect()
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
MWLua::ret_int4 MSlideShow::glueSetColorize( int r, int g, int b)
{
	MWLua::ret_int4 _col( m_Colorize.r, m_Colorize.g, m_Colorize.b, m_Colorize.a);
	SetColorize( MCOLOR( r, g, b));
	return _col;
}


// glueGetColorize
MWLua::ret_int4 MSlideShow::glueGetColorize()
{
	MCOLOR _col = GetColorize();
	return MWLua::ret_int4( _col.r, _col.g, _col.b, _col.a);
}


} // namespace mint3
