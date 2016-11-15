#include "stdafx.h"
#include "Mint4R3.h"
#include "MDrawContext.h"
#include "MBitmap.h"
#include "MFont.h"
#include "MCursor.h"
#include "MAnimationTemplate.h"
#include "MResourceManager.h"
#include "REngine.h"
using namespace rs3;




// Swap
inline void _swap( float& a,float& b)
{
	static float temp;
	temp = a;
	a = b;
	b = temp;
}


// Adjust size offset
const float ADJUST_SIZE = 0.5f;





//////////////////////////////////////////////////////////////////////////
// MDrawContextR3

struct PIXEL2DVERTEX		{ D3DXVECTOR4 p;			DWORD color;									};
struct CUSTOMVERTEX			{ FLOAT x, y, z, rhw;		DWORD color;		FLOAT tu, tv;				};
struct MASKVERTEX			{ FLOAT x, y, z, rhw;		DWORD color;		FLOAT tu, tv, tu2, tv2;		};

const DWORD D3DFVF_PIXEL2DVERTEX = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
const DWORD D3DFVF_CUSTOMVERTEX = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE|D3DFVF_TEX1);
const DWORD D3DFVF_MASKVERTEX = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE|D3DFVF_TEX2);
const DWORD D3DFVF_FONT2DVERTEX = (D3DFVF_XYZ | D3DFVF_DIFFUSE|D3DFVF_TEX1);

rs3::RDevice* MDrawContextR3::m_pDevice = NULL;
MDrawState MDrawContextR3::m_eCurrentState = MDS_NONE;
MDrawEffect MDrawContextR3::m_eCurrentEffect = MDE_NORMAL;
MBitmap* MDrawContextR3::m_pCurrentMaskBitmap = NULL;



MDrawContextR3::MDrawContextR3( rs3::RDevice* pDevice)
:m_pSharedCustomVertexBuffer(NULL),m_pSharedMaskVertexBuffer(NULL),m_pSharedPixel2dVertexBuffer(NULL)
{
	m_pDevice = pDevice;
	m_bInDraw = false;

	m_pSharedCustomVertexBuffer = new MSharedVertexBuffer<CUSTOMVERTEX>;
	m_pSharedMaskVertexBuffer = new MSharedVertexBuffer<MASKVERTEX>;
	m_pSharedPixel2dVertexBuffer = new MSharedVertexBuffer<PIXEL2DVERTEX>;
}


MDrawContextR3::~MDrawContextR3()
{
	SAFE_DELETE(m_pSharedCustomVertexBuffer);
	SAFE_DELETE(m_pSharedMaskVertexBuffer);
	SAFE_DELETE(m_pSharedPixel2dVertexBuffer);
	
}


void MDrawContextR3::overrideBeginDraw()
{
	m_pDevice->SetLighting( false);
	m_pDevice->SetAlphaTestEnable( false);
	m_pDevice->SetFogEnable( false);
	m_pDevice->SetCullMode( RCULL_NONE);
	m_pDevice->SetBlending( RBLEND_NONE, RBLEND_NONE);
	m_pDevice->SetTexture( 0, R_NONE);
	m_pDevice->SetTexture( 1, R_NONE);
	m_pDevice->SetTextureStageState( 0, RTSS_COLOROP, RTOP_SELECTARG1);
	m_pDevice->SetTextureStageState( 0, RTSS_ALPHAOP, RTOP_SELECTARG1);
	m_pDevice->SetTextureStageState( 0, RTSS_COLORARG1, RTA_DIFFUSE);
	m_pDevice->SetTextureStageState( 0, RTSS_ALPHAARG1, RTA_DIFFUSE);
	m_pDevice->SetTextureStageState( 1, RTSS_COLOROP, RTOP_DISABLE);
	m_pDevice->SetTextureStageState( 1, RTSS_ALPHAOP, RTOP_DISABLE);
	m_pDevice->SetTextureAddressClamp( 0);
	m_pDevice->SetTextureAddressClamp( 1);
	m_pDevice->ShaderOff();

	m_bInDraw = true;

	SetState( MDS_NONE);
}


void MDrawContextR3::overrideEndDraw()
{
	// 마지막 스테이트가 font 이면 flush 해주어야 함
	SetState( MDS_NONE);

	m_bInDraw = false;
}


void MDrawContextR3::overrideSetState( MDrawState eState, MDrawEffect eEffect)
{
/*
	BeginDraw() 이후에 dc의 draw 관련 함수를 사용해야 합니다.

	ex)
		MDrawContext* pDC = global.ui->GetDC();	// dc를 얻었다면
		pDC->BeginDraw();						// 사용 하기 전에 BeginDraw()를
		...
		pDC->EndDraw();							// 사용 종료시 EndDraw()를 호출해야 합니다.
*/

	_ASSERT( m_bInDraw);	


	RDevice* pD3dDevice = (RDevice*)m_pDevice;

	// 같은 State라면 넘긴다.
	if ( m_eCurrentState == eState  &&  m_eCurrentEffect == eEffect  &&  m_pCurrentMaskBitmap == m_pMaskBitmap)
		return;

	if ( m_eCurrentState == MDS_FONT)		RFont::EndFont();

	switch ( eState)
	{
	case MDS_CONTEXT:
		pD3dDevice->ShaderOff();
		pD3dDevice->SetFvF( D3DFVF_PIXEL2DVERTEX);

		m_pDevice->SetBlending( RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
		m_pDevice->SetAlphaTestEnable( false);
		m_pDevice->SetDepthEnable( false, false);
		m_pDevice->SetFogEnable( false);
		m_pDevice->SetCullMode( RCULL_NONE);

		m_pDevice->SetTextureStageState( 0, RTSS_COLOROP, RTOP_SELECTARG1);
		m_pDevice->SetTextureStageState( 0, RTSS_COLORARG1, RTA_DIFFUSE);
		m_pDevice->SetTextureStageState( 0, RTSS_ALPHAOP, RTOP_SELECTARG1);
		m_pDevice->SetTextureStageState( 0, RTSS_ALPHAARG1, RTA_DIFFUSE);
		m_pDevice->SetTextureStageState( 0, RTSS_COLOROP, RTOP_DISABLE);
		m_pDevice->SetTextureStageState( 0, RTSS_ALPHAOP, RTOP_DISABLE);

		pD3dDevice->SetTextureStageState( 1,RTSS_COLOROP, RTOP_DISABLE);
		pD3dDevice->SetTextureStageState( 1,RTSS_ALPHAOP, RTOP_DISABLE);

		m_pDevice->SetTexture( 0, R_NONE);
		m_pDevice->SetTexture( 1, R_NONE);
		break;


	case MDS_BITMAP:
		switch ( eEffect)
		{
		case MDE_NORMAL:
			m_pDevice->SetBlending( RBLEND_SRCALPHA , RBLEND_INVSRCALPHA);
			break;

		case MDE_ADD:
			m_pDevice->SetBlending( RBLEND_SRCALPHA , RBLEND_ONE);
			break;

		case MDE_MULTIPLY:
			m_pDevice->SetBlending(RBLEND_ZERO,RBLEND_SRCCOLOR);
			break;
		}


		pD3dDevice->SetTextureFilter( 0, RTF_LINEAR);

		pD3dDevice->SetAlphaTestEnable( false);
		pD3dDevice->SetTextureFilter( 0, RTF_LINEAR);

		pD3dDevice->SetStencilEnable( false);
		pD3dDevice->SetClipping( true);
		pD3dDevice->SetClipPlanes( NULL, 0);
		pD3dDevice->SetVertexBlendEnable( false);
		pD3dDevice->SetIndexedVertexBlendEnable( false);
		pD3dDevice->SetFogEnable( false);
		pD3dDevice->SetDepthEnable( false, false);

		pD3dDevice->SetTextureStageState( 0, RTSS_COLOROP, RTOP_MODULATE);
		pD3dDevice->SetTextureStageState( 0, RTSS_COLORARG1, RTA_TEXTURE);
		pD3dDevice->SetTextureStageState( 0, RTSS_COLORARG2, RTA_DIFFUSE);
		pD3dDevice->SetTextureStageState( 0, RTSS_ALPHAOP, RTOP_MODULATE);
		pD3dDevice->SetTextureStageState( 0, RTSS_ALPHAARG1, RTA_TEXTURE);
		pD3dDevice->SetTextureStageState( 0, RTSS_ALPHAARG2, RTA_DIFFUSE);

		pD3dDevice->SetTextureStageState( 0, RTSS_TEXCOORDINDEX, 0);
		pD3dDevice->SetTextureStageState( 0, RTSS_TEXTURETRANSFORMFLAGS, RTTFF_DISABLE);

		if ( m_pMaskBitmap != NULL)
		{
			pD3dDevice->SetTexture( 1, ((MBitmapR3*)m_pMaskBitmap)->GetTexture());

			pD3dDevice->SetTextureStageState( 1, RTSS_COLOROP, RTOP_SELECTARG2);
			pD3dDevice->SetTextureStageState( 1, RTSS_COLORARG1, RTA_TEXTURE);
			pD3dDevice->SetTextureStageState( 1, RTSS_COLORARG2, RTA_CURRENT);
			pD3dDevice->SetTextureStageState( 1, RTSS_ALPHAOP, RTOP_MODULATE);
			pD3dDevice->SetTextureStageState( 1, RTSS_ALPHAARG1, RTA_TEXTURE);
			pD3dDevice->SetTextureStageState( 1, RTSS_ALPHAARG2, RTA_DIFFUSE);

			pD3dDevice->SetTextureStageState( 1, RTSS_TEXCOORDINDEX, 1);
			pD3dDevice->SetTextureStageState( 1, RTSS_TEXTURETRANSFORMFLAGS, RTTFF_DISABLE);

			pD3dDevice->SetFvF( D3DFVF_MASKVERTEX);
		}
		else
		{
			pD3dDevice->SetTexture( 1, R_NONE);
			pD3dDevice->SetFvF( D3DFVF_CUSTOMVERTEX);
			pD3dDevice->SetTextureStageState( 1,RTSS_COLOROP, RTOP_DISABLE);
			pD3dDevice->SetTextureStageState( 1,RTSS_ALPHAOP, RTOP_DISABLE);
		}
		break;


	case MDS_FONT:
		RFont::BeginFont();

//		pD3dDevice->SetFvF(D3DFVF_FONT2DVERTEX);
// 
//		// State
//		pD3dDevice->SetBlending( RBLEND_SRCALPHA , RBLEND_INVSRCALPHA);
// 
//		pD3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
//		pD3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
// 
//		pD3dDevice->SetTextureStageState( 0, RTSS_COLOROP,   RTOP_MODULATE);
//		pD3dDevice->SetTextureStageState( 0, RTSS_COLORARG1, RTA_TEXTURE);
//		pD3dDevice->SetTextureStageState( 0, RTSS_COLORARG2, RTA_DIFFUSE);
//		pD3dDevice->SetTextureStageState( 0, RTSS_ALPHAOP,   RTOP_MODULATE);
//		pD3dDevice->SetTextureStageState( 0, RTSS_ALPHAARG1, RTA_TEXTURE);
//		pD3dDevice->SetTextureStageState( 0, RTSS_ALPHAARG2, RTA_DIFFUSE);
		break;
	}


	m_eCurrentState = eState;
	m_eCurrentEffect = eEffect;
	m_pCurrentMaskBitmap = m_pMaskBitmap;
}


void MDrawContextR3::overrideSetPixel( int x, int y)
{
	SetState( MDS_CONTEXT);

	unsigned long _color = MCOLOR( m_Color.r, m_Color.g, m_Color.b, m_Color.a * m_fOpacity).GetARGB();
	static PIXEL2DVERTEX p;
	p.p.x = (float)x;
	p.p.y = (float)y;
	p.p.z = 0.0f;
	p.p.w = 1.0f;
	p.color = _color;

	GetSharedPixel2dVertexBuffer()->Render(RPT_POINTLIST,1,&p,sizeof(PIXEL2DVERTEX),1);

	REngine::GetResourceProfiler().AddPolygon( PPT_MINT, 1);
}


void MDrawContextR3::overrideLine( int sx, int sy, int ex, int ey)
{
	SetState( MDS_CONTEXT);

	sx += m_Origin.x;
	sy += m_Origin.y;
	ex += m_Origin.x;
	ey += m_Origin.y;

	unsigned long _color = MCOLOR( m_Color.r, m_Color.g, m_Color.b, m_Color.a * m_fOpacity).GetARGB();
	static PIXEL2DVERTEX p[ 2];
	p[0].p.x = (float)sx;
	p[0].p.y = (float)sy;
	p[0].p.z = 0.0f;
	p[0].p.w = 1.0f;
	p[0].color = _color;
	p[1].p.x = (float)ex;
	p[1].p.y = (float)ey;
	p[1].p.z = 0.0f;
	p[1].p.w = 1.0f;
	p[1].color = _color;

	GetSharedPixel2dVertexBuffer()->Render(RPT_LINELIST,1,p,sizeof(PIXEL2DVERTEX),2);

	REngine::GetResourceProfiler().AddPolygon(PPT_MINT, 1);
}


void MDrawContextR3::overrideRectangle( int x, int y, int w, int h)
{
	SetState( MDS_CONTEXT);

	x += m_Origin.x;
	y += m_Origin.y;

	unsigned long _color = MCOLOR( m_Color.r, m_Color.g, m_Color.b, m_Color.a * m_fOpacity).GetARGB();
	static PIXEL2DVERTEX p[ 5];
	p[ 0].p.x = (float)x;
	p[ 0].p.y = (float)y;
	p[ 0].p.z = 0.0f;
	p[ 0].p.w = 1.0f;
	p[ 0].color = _color;
	p[ 1].p.x = (float)( x + w);
	p[ 1].p.y = (float)y;
	p[ 1].p.z = 0.0f;
	p[ 1].p.w = 1.0f;
	p[ 1].color = _color;
	p[ 2].p.x = (float)( x + w);
	p[ 2].p.y = (float)( y + h);
	p[ 2].p.z = 0.0f;
	p[ 2].p.w = 1.0f;
	p[ 2].color = _color;
	p[ 3].p.x = (float)x;
	p[ 3].p.y = (float)( y + h);
	p[ 3].p.z = 0.0f;
	p[ 3].p.w = 1.0f;
	p[ 3].color = _color;
	p[ 4].p.x = (float)x;
	p[ 4].p.y = (float)y;
	p[ 4].p.z = 0.0f;
	p[ 4].p.w = 1.0f;
	p[ 4].color = _color;

	GetSharedPixel2dVertexBuffer()->Render(RPT_LINESTRIP,4,p,sizeof(PIXEL2DVERTEX),5);

	REngine::GetResourceProfiler().AddPolygon( PPT_MINT, 4);
}


void MDrawContextR3::overrideFillRectangle( int x, int y, int w, int h)
{
	SetState( MDS_CONTEXT);

	x += m_Origin.x;
	y += m_Origin.y;

	unsigned long _color = MCOLOR( m_Color.r, m_Color.g, m_Color.b, m_Color.a * m_fOpacity).GetARGB();
	static PIXEL2DVERTEX p[ 4];

	p[ 0].p.x = (float)x;
	p[ 0].p.y = (float)y;
	p[ 0].p.z = 0.0f;
	p[ 0].p.w = 1.0f;
	p[ 0].color = _color;

	p[ 1].p.x = (float)( x + w);
	p[ 1].p.y = (float)y;
	p[ 1].p.z = 0.0f;
	p[ 1].p.w = 1.0f;
	p[ 1].color = _color;

	p[ 2].p.x = float(x+w);
	p[ 2].p.y = float(y+h);
	p[ 2].p.z = 0.0f;
	p[ 2].p.w = 1.0f;
	p[ 2].color = _color;

	p[ 3].p.x = (float)x;
	p[ 3].p.y = (float)( y + h);
	p[ 3].p.z = 0.0f;
	p[ 3].p.w = 1.0f;
	p[ 3].color = _color;

	GetSharedPixel2dVertexBuffer()->Render(RPT_TRIANGLEFAN,2,p,sizeof(PIXEL2DVERTEX),4);

	REngine::GetResourceProfiler().AddPolygon( PPT_MINT, 2);
}


void MDrawContextR3::overrideClipRect( int x, int y, int w, int h)
{
	if ( m_eCurrentState == MDS_FONT)		RFont::EndFont();

	m_pDevice->SetViewport( x, y, max( 0, w + 1), max( 0, h + 1));

	if ( m_eCurrentState == MDS_FONT)		RFont::BeginFont();
}


void MDrawContextR3::overrideDraw( int tx, int ty, int tw, int th, int sx, int sy, int sw, int sh)
{
	if ( m_pBitmap == NULL)			return;
	if (((MBitmapR3*)m_pBitmap)->GetTexture() == NULL) return;

	SetState( MDS_BITMAP, m_Effect);

	float fBmpWidth = (float)m_pBitmap->GetWidth();
	float fBmpHeight = (float)m_pBitmap->GetHeight();
	float fTextCoord[ 4];
	m_pBitmap->GetTexCoord( fTextCoord);

	float fTexX = fTextCoord[ 0];
	float fTexY = fTextCoord[ 1];
	float fTexW = fTextCoord[ 2] - fTextCoord[ 0];
	float fTexH = fTextCoord[ 3] - fTextCoord[ 1];

	// 이 부분을 수정, UV offset 은 바꿔주지 않고 vertex에 offset을 걸었다.
	// http://www.gamza.net/ez2000/ezboard.cgi?db=Programming&action=read&dbf=10&page=0&depth=1 : 마법의 숫자 .5f
	float fuv[ 8] =
	{
		fTexX + fTexW * (float)sx / fBmpWidth,			fTexY + fTexH * (float)sy / fBmpHeight,
		fTexX + fTexW * (float)(sx + sw) / fBmpWidth,	fTexY + fTexH * (float)sy / fBmpHeight,
		fTexX + fTexW * (float)(sx + sw) / fBmpWidth,	fTexY + fTexH * (sy + sh) / fBmpHeight,
		fTexX + fTexW * (float)sx / fBmpWidth,			fTexY + fTexH * (sy + sh) / fBmpHeight
	};


	unsigned int nDrawMode = m_pBitmap->GetDrawMode();
	if ( nDrawMode)
	{
		// 좌우 바꾸기
		if ( nDrawMode & MBM_FlipLR)
		{
			_swap( fuv[ 0], fuv[ 2]);
			_swap( fuv[ 1], fuv[ 3]);
			_swap( fuv[ 4], fuv[ 6]);
			_swap( fuv[ 5], fuv[ 7]);
		}

		// 상하 바꾸기
		if ( nDrawMode & MBM_FlipUD)
		{
			_swap( fuv[ 0], fuv[ 6]);
			_swap( fuv[ 1], fuv[ 7]);
			_swap( fuv[ 2], fuv[ 4]);
			_swap( fuv[ 3], fuv[ 5]);
		}

		// 회전
		if ( nDrawMode & MBM_RotL90)
		{
			float temp[ 2];
			temp[ 0] = fuv[ 4];		temp[ 1] = fuv[ 5];
			fuv[ 4]  = fuv[ 6];		fuv[ 5]  = fuv[ 7];
			fuv[ 6]  = fuv[ 0];		fuv[ 7]  = fuv[ 1];
			fuv[ 0]  = fuv[ 2];		fuv[ 1]  = fuv[ 3];
			fuv[ 2]  = temp[ 0];	fuv[ 3]  = temp[ 1];
		}

		if ( nDrawMode & MBM_RotR90)
		{
			float temp[ 2];
			temp[ 0] = fuv[ 6];		temp[ 1] = fuv[ 7];
			fuv[ 6]  = fuv[ 4];		fuv[ 7]  = fuv[ 5];
			fuv[ 4]  = fuv[ 2];		fuv[ 5]  = fuv[ 3];
			fuv[ 2]  = fuv[ 0];		fuv[ 3]  = fuv[ 1];
			fuv[ 0]  = temp[ 0];	fuv[ 1]  = temp[ 1];
		}
	}


	m_pDevice->SetTexture( 0, ((MBitmapR3*)m_pBitmap)->GetTexture());


	unsigned long _color = MCOLOR( m_BitmapColor.r, m_BitmapColor.g, m_BitmapColor.b, (unsigned char)( 0xFF * m_fOpacity)).GetARGB();
	if ( m_pMaskBitmap != NULL)
	{
		MRECT _maskrect = m_MaskRect;
		_maskrect.w = (_maskrect.w < 0) ? m_pMaskBitmap->GetWidth() : _maskrect.w;
		_maskrect.h = (_maskrect.h < 0) ? m_pMaskBitmap->GetHeight() : _maskrect.h;

		float u1 = 0.0f;
		float v1 = 0.0f;
		float u2 = 1.0f;
		float v2 = 1.0f;

		float _du = (float)tw / (float)_maskrect.w - 1.0f;
		float _dv = (float)th / (float)_maskrect.h - 1.0f;
		u2 += _du;
		v2 += _dv;

		_du = (float)(_maskrect.x - tx) / (float)_maskrect.w;
		_dv = (float)(_maskrect.y - ty) / (float)_maskrect.h;
		u1 -= _du;
		u2 -= _du;
		v1 -= _dv;
		v2 -= _dv;

		tx += m_Origin.x;
		ty += m_Origin.y;

		MASKVERTEX _sprite[ 4] =
		{
			{ tx - ADJUST_SIZE,			ty + th - ADJUST_SIZE,	0, 1.0f, _color, fuv[ 6], fuv[ 7], u1, v2 },
			{ tx + tw - ADJUST_SIZE,	ty + th - ADJUST_SIZE,	0, 1.0f, _color, fuv[ 4], fuv[ 5], u2, v2 },
			{ tx + tw - ADJUST_SIZE,	ty - ADJUST_SIZE,		0, 1.0f, _color, fuv[ 2], fuv[ 3], u2, v1 },
			{ tx - ADJUST_SIZE,			ty - ADJUST_SIZE,		0, 1.0f, _color, fuv[ 0], fuv[ 1], u1, v1 }
		};

		m_pDevice->SetTextureAddressClamp( 1);
		GetSharedMaskVertexBuffer()->Render(RPT_TRIANGLEFAN, 2 ,_sprite, sizeof(MASKVERTEX),4);
	}

	else
	{
		tx += m_Origin.x;
		ty += m_Origin.y;

		CUSTOMVERTEX _sprite[ 4] =
		{
			{ tx - ADJUST_SIZE,			ty + th - ADJUST_SIZE,	0, 1.0f, _color, fuv[ 6], fuv[ 7]},
			{ tx + tw - ADJUST_SIZE,	ty + th - ADJUST_SIZE,	0, 1.0f, _color, fuv[ 4], fuv[ 5]},
			{ tx + tw - ADJUST_SIZE,	ty - ADJUST_SIZE,		0, 1.0f, _color, fuv[ 2], fuv[ 3]},
			{ tx - ADJUST_SIZE,			ty - ADJUST_SIZE,		0, 1.0f, _color, fuv[ 0], fuv[ 1]}
		};

		GetSharedCustomVertexBuffer()->Render(RPT_TRIANGLEFAN, 2 ,_sprite, sizeof(CUSTOMVERTEX),4);
	}

	REngine::GetResourceProfiler().AddPolygon( PPT_MINT, 2);
}


void MDrawContextR3::overrideDrawRotate( int tx, int ty, int tw, int th, int rx, int ry, float ra, int sx, int sy, int sw, int sh)
{
	if ( m_pBitmap == NULL)			return;

	SetState( MDS_BITMAP, m_Effect);

	float fBmpWidth = (float)m_pBitmap->GetWidth();
	float fBmpHeight = (float)m_pBitmap->GetHeight();
	float fTextCoord[ 4];
	m_pBitmap->GetTexCoord( fTextCoord);

	float fTexX = fTextCoord[ 0];
	float fTexY = fTextCoord[ 1];
	float fTexW = fTextCoord[ 2] - fTextCoord[ 0];
	float fTexH = fTextCoord[ 3] - fTextCoord[ 1];

	float fuv[ 8] =
	{
		fTexX + fTexW * (float)sx / fBmpWidth,			fTexY + fTexH * (float)sy / fBmpHeight,
		fTexX + fTexW * (float)(sx + sw) / fBmpWidth,	fTexY + fTexH * (float)sy / fBmpHeight,
		fTexX + fTexW * (float)(sx + sw) / fBmpWidth,	fTexY + fTexH * (sy + sh) / fBmpHeight,
		fTexX + fTexW * (float)sx / fBmpWidth,			fTexY + fTexH * (sy + sh) / fBmpHeight
	};


	unsigned int nDrawMode = m_pBitmap->GetDrawMode();
	if ( nDrawMode)
	{
		// 좌우 바꾸기
		if ( nDrawMode & MBM_FlipLR)
		{
			_swap( fuv[ 0], fuv[ 2]);
			_swap( fuv[ 1], fuv[ 3]);
			_swap( fuv[ 4], fuv[ 6]);
			_swap( fuv[ 5], fuv[ 7]);
		}

		// 상하 바꾸기
		if ( nDrawMode & MBM_FlipUD)
		{
			_swap( fuv[ 0], fuv[ 6]);
			_swap( fuv[ 1], fuv[ 7]);
			_swap( fuv[ 2], fuv[ 4]);
			_swap( fuv[ 3], fuv[ 5]);
		}

		// 회전
		if ( nDrawMode & MBM_RotL90)
		{
			float temp[ 2];
			temp[ 0] = fuv[ 4];		temp[ 1] = fuv[ 5];
			fuv[ 4]  = fuv[ 6];		fuv[ 5]  = fuv[ 7];
			fuv[ 6]  = fuv[ 0];		fuv[ 7]  = fuv[ 1];
			fuv[ 0]  = fuv[ 2];		fuv[ 1]  = fuv[ 3];
			fuv[ 2]  = temp[ 0];	fuv[ 3]  = temp[ 1];
		}

		if ( nDrawMode & MBM_RotR90)
		{
			float temp[ 2];
			temp[ 0] = fuv[ 6];		temp[ 1] = fuv[ 7];
			fuv[ 6]  = fuv[ 4];		fuv[ 7]  = fuv[ 5];
			fuv[ 4]  = fuv[ 2];		fuv[ 5]  = fuv[ 3];
			fuv[ 2]  = fuv[ 0];		fuv[ 3]  = fuv[ 1];
			fuv[ 0]  = temp[ 0];	fuv[ 1]  = temp[ 1];
		}
	}


	m_pDevice->SetTexture( 0, ((MBitmapR3*)m_pBitmap)->GetTexture());

	
	unsigned long _color = MCOLOR( m_BitmapColor.r, m_BitmapColor.g, m_BitmapColor.b, (unsigned char)( 0xFF * m_fOpacity)).GetARGB();
	if ( m_pMaskBitmap != NULL)
	{
		MRECT _maskrect = m_MaskRect;
		_maskrect.w = (_maskrect.w < 0) ? m_pMaskBitmap->GetWidth() : _maskrect.w;
		_maskrect.h = (_maskrect.h < 0) ? m_pMaskBitmap->GetHeight() : _maskrect.h;

		float u1 = 0.0f;
		float v1 = 0.0f;
		float u2 = 1.0f;
		float v2 = 1.0f;

		float _du = (float)tw / (float)_maskrect.w - 1.0f;
		float _dv = (float)th / (float)_maskrect.h - 1.0f;
		u2 += _du;
		v2 += _dv;

		_du = (float)(_maskrect.x - tx) / (float)_maskrect.w;
		_dv = (float)(_maskrect.y - ty) / (float)_maskrect.h;
		u1 -= _du;
		u2 -= _du;
		v1 -= _dv;
		v2 -= _dv;

		tx += m_Origin.x + rx;
		ty += m_Origin.y + ry;

		MASKVERTEX Sprite[ 4] =
		{
			{ -rx - ADJUST_SIZE,		th - ry - ADJUST_SIZE,	0, 1.0f, _color, fuv[ 6], fuv[ 7], u1, v2 },
			{ tw - rx - ADJUST_SIZE,	th - ry - ADJUST_SIZE,	0, 1.0f, _color, fuv[ 4], fuv[ 5], u2, v2 },
			{ tw - rx - ADJUST_SIZE,	-ry - ADJUST_SIZE,		0, 1.0f, _color, fuv[ 2], fuv[ 3], u2, v1 },
			{ -rx - ADJUST_SIZE,		-ry - ADJUST_SIZE,		0, 1.0f, _color, fuv[ 0], fuv[ 1], u1, v1 }
		};

		float x, y;
		float _cos = cos( ra);
		float _sin = sin( ra);
		for ( int i = 0;  i < 4;  i++)
		{
			x = Sprite[ i].x;
			y = Sprite[ i].y;
			Sprite[ i].x = tx + (x * _cos) - (y * _sin);
			Sprite[ i].y = ty + (x * _sin) + (y * _cos);
		}

		m_pDevice->SetTextureAddressClamp( 1);
		GetSharedMaskVertexBuffer()->Render(RPT_TRIANGLEFAN, 2 ,Sprite, sizeof(MASKVERTEX),4);
	}
	else
	{
		tx += m_Origin.x + rx;
		ty += m_Origin.y + ry;

		CUSTOMVERTEX Sprite[ 4] =
		{
			{ -rx - ADJUST_SIZE,		th - ry - ADJUST_SIZE,	0, 1.0f, _color, fuv[ 6], fuv[ 7]},
			{ tw - rx - ADJUST_SIZE,	th - ry - ADJUST_SIZE,	0, 1.0f, _color, fuv[ 4], fuv[ 5]},
			{ tw - rx - ADJUST_SIZE,	-ry - ADJUST_SIZE,		0, 1.0f, _color, fuv[ 2], fuv[ 3]},
			{ -rx - ADJUST_SIZE,		-ry - ADJUST_SIZE,		0, 1.0f, _color, fuv[ 0], fuv[ 1]}
		};

		float x, y;
		float _cos = cos( ra);
		float _sin = sin( ra);
		for ( int i = 0;  i < 4;  i++)
		{
			x = Sprite[ i].x;
			y = Sprite[ i].y;
			Sprite[ i].x = tx + (x * _cos) - (y * _sin);
			Sprite[ i].y = ty + (x * _sin) + (y * _cos);
		}

		GetSharedCustomVertexBuffer()->Render(RPT_TRIANGLEFAN, 2 ,Sprite, sizeof(CUSTOMVERTEX),4);
	}

	REngine::GetResourceProfiler().AddPolygon(PPT_MINT, 2);
}


void MDrawContextR3::overrideText( int x, int y, const char* szText)
{
	SetState( MDS_FONT);

	x += m_Origin.x;
	y += m_Origin.y;

	unsigned long nColor = MCOLOR( m_Color.r, m_Color.g, m_Color.b, m_Color.a * m_fOpacity).GetARGB();
	unsigned long nOutlineColor = nColor & 0xFF000000;

	rs3::RFont* pFont = ((MFontR3*)m_pFont)->GetFont();
	pFont->DrawText_UNICODE( (float)x, (float)y, MLocale::ConvAnsiToUTF16(szText).c_str(), -1, nColor, nOutlineColor);
}



void MDrawContextR3::overrideFlushFont()
{
	RFont::FlushFont();
}









//////////////////////////////////////////////////////////////////////////
// MBitmapR3

MBitmapR3::MBitmapR3()
{
	m_pDevice = NULL;
	m_hTexture = rs3::R_NONE;
	m_nWidth = 0;
	m_nHeight = 0;
	m_fTexCoord[ 0] = 0.0f;
	m_fTexCoord[ 1] = 0.0f;
	m_fTexCoord[ 2] = 1.0f;
	m_fTexCoord[ 3] = 1.0f;
}


MBitmapR3::~MBitmapR3()
{
	Destroy();
}


bool MBitmapR3::Create( rs3::RDevice* pDevice, const char* szName, const char* szFileName)
{
	if ( pDevice == NULL)									return false;
	if ( MBitmap::Create( szName, szFileName) == false)		return false;

	m_pDevice = pDevice;

	//ParseBitmap()에서 비트맵 텍스쳐 만들지 않도록
	//return CreateTexture();
	return true;
}

bool MBitmapR3::Create( rs3::RDevice* pDevice, rs3::RTexture* pTexture )
{
	m_pDevice = pDevice;

	m_hTexture = pTexture;
	if ( m_hTexture == rs3::R_NONE)		return false;

	CalcSizeFromTexCoord();

	return true;
}


bool MBitmapR3::Destroy()
{
	if ( m_hTexture != rs3::R_NONE)
	{
		m_pDevice->DeleteTexture( m_hTexture);
		m_hTexture = rs3::R_NONE;

		m_nTime = INT_MAX;
	}

	return true;
}


void MBitmapR3::SetTexCoord( float x1, float y1, float x2, float y2)
{
	m_fTexCoord[ 0] = x1;
	m_fTexCoord[ 1] = y1;
	m_fTexCoord[ 2] = x2;
	m_fTexCoord[ 3] = y2;

	CalcSizeFromTexCoord();
}


void MBitmapR3::GetTexCoord( float* pTexCoord)
{
	if ( pTexCoord == NULL)			return;

	pTexCoord[ 0] = m_fTexCoord[ 0];
	pTexCoord[ 1] = m_fTexCoord[ 1];
	pTexCoord[ 2] = m_fTexCoord[ 2];
	pTexCoord[ 3] = m_fTexCoord[ 3];
}


void MBitmapR3::BeginState( MDrawEffect effect)
{
}


void MBitmapR3::EndState()
{
	m_pDevice->SetTexture( 0, R_NONE);
	m_pDevice->SetTexture( 1, R_NONE);
}


void MBitmapR3::CalcSizeFromTexCoord()
{
	if ( m_hTexture == rs3::R_NONE)		return;

	const rs3::RTextureInfo* pTexInfo = m_pDevice->GetTextureInfo( m_hTexture);
	if ( pTexInfo == NULL) return;

	int width = pTexInfo->nTextureWidth;
	int height = pTexInfo->nTextureHeight;

	m_nWidth = (int)( (m_fTexCoord[2] - m_fTexCoord[0]) * width);
	m_nHeight = (int)( (m_fTexCoord[3] - m_fTexCoord[1]) * height);
}

bool MBitmapR3::CreateTexture()
{
	m_hTexture = m_pDevice->CreateTexture( m_strFileName.c_str(), RTF_LINEAR, RCT_UNREDUCIBLE);
	if ( m_hTexture == rs3::R_NONE)		return false;

	CalcSizeFromTexCoord();

	ResetTime();

	return true;
}

void MBitmapR3::ResetTime()
{
	m_nTime = timeGetTime();
}







//////////////////////////////////////////////////////////////////////////
// MFontR3

MFontR3::MFontR3()
{
	m_pFont = NULL;
}


MFontR3::~MFontR3()
{
	Destroy();
}


bool MFontR3::Create( rs3::RDevice* pDevice, const char* szName, const char* szFileName, int nHeight, bool bBold, bool bItalic, bool bAntiAlias, float fOutline, float fScale)
{
	if ( pDevice == NULL)		return false;
	if ( szName[ 0] == 0)		return false;

	if ( MFont::Create( szName, szFileName, nHeight, bBold, bItalic, bAntiAlias, fOutline, fScale) == false)	return false;

	int nOutlineTick = (fOutline > 0.001f)  ?  1 : 0;

	m_pFont = new rs3::RFont();
	if ( m_pFont->Create( pDevice, MLocale::ConvAnsiToUTF16(szFileName).c_str(), nHeight, bBold, bItalic, nOutlineTick, fOutline, bAntiAlias) == false)
	{
		if ( m_pFont != NULL)
		{
			delete m_pFont;
			m_pFont = NULL;
		}
		return false;
	}

	return true;
}


bool MFontR3::Destroy()
{
	if ( m_pFont != NULL)
	{
		m_pFont->Destroy();

		delete m_pFont;
		m_pFont = NULL;
	}

	return true;
}


int MFontR3::GetWidth(const char* szText, int nLen)
{
	if ( m_pFont == NULL)		return 0;
	return (int)( m_pFont->GetTextWidth(  MLocale::ConvAnsiToUTF16(szText).c_str(), nLen) * m_fScale + 0.5f);
}


int MFontR3::GetHeight()
{
	if ( m_pFont == NULL)		return 0;
	return (int)( m_pFont->GetHeight() * m_fScale + 0.5f);
}







//////////////////////////////////////////////////////////////////////////
// MCursorR3

MCursorR3::MCursorR3( RDeviceD3D* pDevice, const char* szName)
:MCursor( szName)
{
	m_pDevice = pDevice;
	m_nCurrnetImage = -1;
}


MCursorR3::~MCursorR3()
{
	Release();
}


bool MCursorR3::Release()
{
	while( !m_cursorImages.empty())
	{
		delete m_cursorImages.back();
		m_cursorImages.pop_back();
	}

	return true;
}


void MCursorR3::OnDraw( MDrawContext* pDC, int x, int y)
{
	// 하드웨어 커서를 사용하므로 아무것도 그리지 않으므로 설정만 바꿔준다
	if ( m_nCurrnetImage != m_nSceneNum  &&  m_cursorImages.empty() == false)
	{
		m_pDevice->SetCursorProperties( m_ptHotSpot.x, m_ptHotSpot.y, m_cursorImages[ m_nSceneNum]);
		m_nCurrnetImage = m_nSceneNum;
	}
}


void MCursorR3::OnSet()
{
	m_nCurrnetImage = -1;
}


void MCursorR3::OnSetBitmap( MBitmap* pBitmap)
{
	_ASSERT( m_cursorImages.empty());

	Release();

	MBitmapR3* pBitmapR3 = (MBitmapR3*)m_pBitmap;
	RTexture* pTexture = pBitmapR3->GetTexture();

	RLOCKED_RECT d3dLockRect;
	if ( m_pDevice->LockRect( pTexture, 0, &d3dLockRect, NULL, 0))
	{
		RImage* pImage = m_pDevice->CreateImage( pBitmapR3->GetWidth(), pBitmapR3->GetHeight(), RFMT_A8R8G8B8, d3dLockRect.pBits, d3dLockRect.Pitch, 0, 0);
		m_cursorImages.push_back( pImage);
		m_pDevice->UnlockRect( pTexture, 0);
	}
}


void MCursorR3::OnSetAnimationTemplate( MAnimationTemplate* pAnimationTemplate)
{
	_ASSERT( m_cursorImages.empty());

	Release();

	MBitmapR3* pBitmapR3 = (MBitmapR3*)pAnimationTemplate->GetBitmap();
	if (pBitmapR3)
	{
		RTexture* pTexture = pBitmapR3->GetTexture();

		MSIZE _size( pBitmapR3->GetWidth() / pAnimationTemplate->GetSplitCount(), pBitmapR3->GetHeight());
		RLOCKED_RECT d3dLockRect;
		if ( m_pDevice->LockRect( pTexture, 0, &d3dLockRect, NULL, 0))
		{
			for( int i = 0;  i < pAnimationTemplate->GetSplitCount();  i++)
			{
				RImage* pImage = m_pDevice->CreateImage( _size.w, _size.h, RFMT_A8R8G8B8, d3dLockRect.pBits, d3dLockRect.Pitch, 32 * i, 0);
				m_cursorImages.push_back( pImage);
			}
			m_pDevice->UnlockRect( pTexture, 0);
		}
	}
}






//////////////////////////////////////////////////////////////////////////
// Mint4R3

MBitmap* Mint4R3::NewBitmap(const char* szName, const char* szFileName)
{
	// 같은 이름으로는 등록이 안되도록.
	if ( MBitmapManager::IsExist( szName) == true)		return NULL;

	MBitmapR3* pBitmap = new MBitmapR3();
	if ( pBitmap->Create( REngine::GetDevicePtr(), szName, szFileName) == false)
	{
		delete pBitmap;
		return NULL;
	}

	return pBitmap;
}


MFont* Mint4R3::NewFont( const char* szName, const char* szFontName, const char* szFileName, int nHeight, bool bBold, bool bItalic, bool bAntiAlias, float fOutline, float fScale)
{
	if ( MFontManager::IsExist( szName) == true)		return NULL;

	bool bRetVal = RFont::AddFontResourceFromFile(  MLocale::ConvAnsiToUTF16(szFileName).c_str());
	if ( bRetVal == true)
	{
		MFontR3* pFont = new MFontR3();
		if ( pFont->Create( MDrawContextR3::GetDevice(), szName, szFontName, nHeight, bBold, bItalic, bAntiAlias, fOutline, fScale) == true)
			return pFont;

		SAFE_DELETE( pFont);
		return NULL;
	}
	else
	{
		MFontR3* pFont = new MFontR3();
		if ( pFont->Create( MDrawContextR3::GetDevice(), szName, szFileName, nHeight, bBold, bItalic, bAntiAlias, fOutline, fScale) == true)
			return pFont;

		SAFE_DELETE( pFont);
		return NULL;
	}

	return NULL;
}


MFont* Mint4R3::NewFontMemory( const char* szName, const char* szFontName, const char* szFileName, int nHeight, bool bBold, bool bItalic, bool bAntiAlias, float fOutline, float fScale)
{
	if ( MFontManager::IsExist( szName) == true)		return NULL;

	bool bRetVal = RFont::AddFontResourceFromMemory(  MLocale::ConvAnsiToUTF16(szFileName).c_str());
	if ( bRetVal == true)
	{
		MFontR3* pFont = new MFontR3();
		if ( pFont->Create( MDrawContextR3::GetDevice(), szName, szFontName, nHeight, bBold, bItalic, bAntiAlias, fOutline, fScale) == true)
			return pFont;

		SAFE_DELETE( pFont);
		return NULL;
	}
	else
	{
		MFontR3* pFont = new MFontR3();
		if ( pFont->Create( MDrawContextR3::GetDevice(), szName, szFileName, nHeight, bBold, bItalic, bAntiAlias, fOutline, fScale) == true)
			return pFont;

		SAFE_DELETE( pFont);
		return NULL;
	}

	return NULL;
}


MCursor* Mint4R3::NewCursor( const char* pszAliasName)
{
	if ( MCursorSystem::IsExist( pszAliasName) == true)		return NULL;

	MCursor* pCursor = new MCursorR3( (rs3::RDeviceD3D*)REngine::GetDevicePtr(), pszAliasName);
	return pCursor;
}