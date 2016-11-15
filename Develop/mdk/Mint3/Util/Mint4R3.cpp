#include "stdafx.h"

#include "Mint4R3.h"
#include "MWidget.h"
#include "Mint.h"
#include "RSceneNode.h"
#include "RProfileInfo.h"
#include "REngine.h"
#include "MAnimationTemplate.h"

using namespace rs3;

static int Floorer2PowerSize(int v)
{
	if(v<=2) return 2;
	else if(v<=4) return 4;
	else if(v<=8) return 8;
	else if(v<=16) return 16;
	else if(v<=32) return 32;
	else if(v<=64) return 64;
	else if(v<=128) return 128;
	else if(v<=256) return 256;
	else if(v<=512) return 512;
	else if(v<=1024) return 1024;

	_ASSERT(FALSE);	// Too Big!

	return 2;
}

//////////////////////////////////////////////////////////////////////////
// MDrawContextR3

rs3::RDevice* MDrawContextR3::m_pDevice = NULL;
// MDrawContextR3
struct PIXEL2DVERTEX { 
	D3DXVECTOR4 p;   
	DWORD color; 
};
const DWORD D3DFVF_PIXEL2DVERTEX = (D3DFVF_XYZRHW|D3DFVF_DIFFUSE);

// MBitmap
struct CUSTOMVERTEX{
	FLOAT	x, y, z, rhw;
	DWORD	color;
	FLOAT	tu, tv;
};
const DWORD D3DFVF_CUSTOMVERTEX = (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1);
DWORD MBitmapR3::m_dwStateBlock;

// MFont
const DWORD D3DFVF_FONT2DVERTEX = (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);

MDrawState MDrawContextR3::m_eCurrentState = MDS_NONE;	//  초기화

void MDrawContextR3::SetState(MDrawState eState, MDrawEffect eEffect)
{
//	TIMEPROFILE_THIS_FUNCTION;


	/*
	BeginDraw() 이후에 dc의 draw 관련 함수를 사용해야 합니다.
	
	ex)
	MDrawContext* pDC = global.ui->GetDC();	// dc를 얻었다면
	pDC->BeginDraw();						// 사용 하기 전에 BeginDraw()를
	...
	pDC->EndDraw();							// 사용 종료시 EndDraw()를 호출해야 합니다.
	*/
	_ASSERT(m_bInDraw);	
	

	RDevice* pD3dDevice = (RDevice*)m_pDevice;

	// 같은 State라면 넘긴다.
	if(m_eCurrentState == eState)
		return;

	if(m_eCurrentState==MDS_FONT)
		RFont::EndFont();

	switch(eState)
	{
	case MDS_CONTEXT:
		{
			pD3dDevice->ShaderOff();
			pD3dDevice->SetFvF( D3DFVF_PIXEL2DVERTEX );

			m_pDevice->SetBlending(RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
			m_pDevice->SetAlphaTestEnable(false);
			m_pDevice->SetDepthEnable(false, false);
			m_pDevice->SetFogEnable(false);
			m_pDevice->SetCullMode(RCULL_NONE);

			m_pDevice->SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
			m_pDevice->SetTextureStageState(0, RTSS_COLORARG1, RTA_DIFFUSE);
			m_pDevice->SetTextureStageState(0, RTSS_ALPHAOP, RTOP_SELECTARG1);
			m_pDevice->SetTextureStageState(0, RTSS_ALPHAARG1, RTA_DIFFUSE);
			m_pDevice->SetTextureStageState(0, RTSS_COLOROP, RTOP_DISABLE );
			m_pDevice->SetTextureStageState(0, RTSS_ALPHAOP, RTOP_DISABLE );

			m_pDevice->SetTexture(0, R_NONE);
			m_pDevice->SetTexture(1, R_NONE);
		}
		break;
	case MDS_BITMAP:
		{
			pD3dDevice->SetFvF( D3DFVF_CUSTOMVERTEX );

			switch (eEffect)
			{
			case MDE_NORMAL:
				{
					m_pDevice->SetBlending( RBLEND_SRCALPHA , RBLEND_INVSRCALPHA );
				}
				break;
			case MDE_ADD:
				{
					m_pDevice->SetBlending( RBLEND_SRCALPHA , RBLEND_ONE );
				}
				break;
			case MDE_MULTIPLY:
				{
					m_pDevice->SetBlending(RBLEND_ZERO,RBLEND_SRCCOLOR );
				}
				break;

			}

			pD3dDevice->SetTextureFilter(0,RTF_LINEAR);

			pD3dDevice->SetAlphaTestEnable(false);
			pD3dDevice->SetTextureFilter(0,RTF_LINEAR);

			pD3dDevice->SetStencilEnable(false);
			pD3dDevice->SetClipping(true);
			pD3dDevice->SetClipPlanes(NULL,0);
			pD3dDevice->SetVertexBlendEnable(false);
			pD3dDevice->SetIndexedVertexBlendEnable(false);
			pD3dDevice->SetFogEnable(false);
			pD3dDevice->SetDepthEnable(true,false);

			pD3dDevice->SetTextureStageState( 0,RTSS_COLOROP, RTOP_MODULATE );
			pD3dDevice->SetTextureStageState( 0,RTSS_COLORARG1, RTA_TEXTURE );
			pD3dDevice->SetTextureStageState( 0,RTSS_COLORARG2, RTA_DIFFUSE );
			pD3dDevice->SetTextureStageState( 0,RTSS_ALPHAOP, RTOP_MODULATE );
			pD3dDevice->SetTextureStageState( 0,RTSS_ALPHAARG1, RTA_TEXTURE );
			pD3dDevice->SetTextureStageState( 0,RTSS_ALPHAARG2, RTA_DIFFUSE );

			//pD3dDevice->SetTextureCoord(0,RTC_NONE);
			pD3dDevice->SetTextureStageState(0, RTSS_TEXCOORDINDEX, 0);
			pD3dDevice->SetTextureStageState(0, RTSS_TEXTURETRANSFORMFLAGS, RTTFF_DISABLE);

		}
		break;
	case MDS_FONT:
		{
			RFont::BeginFont();
// 			pD3dDevice->SetFvF(D3DFVF_FONT2DVERTEX);
// 
// 			// State
// 			pD3dDevice->SetBlending( RBLEND_SRCALPHA , RBLEND_INVSRCALPHA );
// 
// 			pD3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
// 			pD3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
// 
// 			pD3dDevice->SetTextureStageState( 0, RTSS_COLOROP,   RTOP_MODULATE);
// 			pD3dDevice->SetTextureStageState( 0, RTSS_COLORARG1, RTA_TEXTURE );
// 			pD3dDevice->SetTextureStageState( 0, RTSS_COLORARG2, RTA_DIFFUSE );
// 			pD3dDevice->SetTextureStageState( 0, RTSS_ALPHAOP,   RTOP_MODULATE );
// 			pD3dDevice->SetTextureStageState( 0, RTSS_ALPHAARG1, RTA_TEXTURE );
// 			pD3dDevice->SetTextureStageState( 0, RTSS_ALPHAARG2, RTA_DIFFUSE );
		}
		break;
	}

	m_eCurrentState = eState;
}

// 폰트를 강제로 flush한다
void MDrawContextR3::FlushFont()
{
	RFont::FlushFont();
}

MDrawContextR3::MDrawContextR3(rs3::RDevice* pDevice) : m_bInDraw(false)
{
	m_pDevice = pDevice;
}

MDrawContextR3::~MDrawContextR3(void)
{
}

void MDrawContextR3::SetPixel(int x, int y, MCOLOR& color)
{
	SetState(MDS_CONTEXT);

	static PIXEL2DVERTEX p;
	p.p.x = float(x);
	p.p.y = float(y);
	p.p.z = 0;
	p.p.w = 1;
	p.color = color.GetARGB();

	m_pDevice->DrawPrimitiveUP( RPT_POINTLIST, 1, &p, sizeof(PIXEL2DVERTEX) );

	REngine::GetResourceProfiler().AddPolygon(PPT_MINT, 1 );
}

void MDrawContextR3::HLine(int x, int y, int len)
{
	Line(x, y, x+len, y);
}

void MDrawContextR3::VLine(int x, int y, int len)
{
	Line(x, y, x, y+len);
}

void MDrawContextR3::Line(int sx, int sy, int ex, int ey)
{
	SetState(MDS_CONTEXT);

	sx += m_Origin.x;
	sy += m_Origin.y;
	ex += m_Origin.x;
	ey += m_Origin.y;

	DWORD argb = m_Color.GetARGB();
	static PIXEL2DVERTEX p[2];
	p[0].p.x = float(sx);
	p[0].p.y = float(sy);
	p[0].p.z = 0;
	p[0].p.w = 1;
	p[0].color = argb;
	p[1].p.x = float(ex);
	p[1].p.y = float(ey);
	p[1].p.z = 0;
	p[1].p.w = 1;
	p[1].color = argb;

	m_pDevice->DrawPrimitiveUP(RPT_LINELIST, 1, p, sizeof(PIXEL2DVERTEX));

	REngine::GetResourceProfiler().AddPolygon(PPT_MINT, 1 );
}

void MDrawContextR3::Rectangle(int x, int y, int cx, int cy)
{
	SetState(MDS_CONTEXT);

	x += m_Origin.x;
	y += m_Origin.y;

	DWORD argb = m_Color.GetARGB();
	static PIXEL2DVERTEX p[5];
	p[0].p.x = float(x);
	p[0].p.y = float(y);
	p[0].p.z = 0;
	p[0].p.w = 1;
	p[0].color = argb;
	p[1].p.x = float(x+cx);
	p[1].p.y = float(y);
	p[1].p.z = 0;
	p[1].p.w = 1;
	p[1].color = argb;
	p[2].p.x = float(x+cx);
	p[2].p.y = float(y+cy);
	p[2].p.z = 0;
	p[2].p.w = 1;
	p[2].color = argb;
	p[3].p.x = float(x);
	p[3].p.y = float(y+cy);
	p[3].p.z = 0;
	p[3].p.w = 1;
	p[3].color = argb;
	p[4].p.x = float(x);
	p[4].p.y = float(y);
	p[4].p.z = 0;
	p[4].p.w = 1;
	p[4].color = argb;

	m_pDevice->DrawPrimitiveUP(RPT_LINESTRIP, 4, p, sizeof(PIXEL2DVERTEX));

	REngine::GetResourceProfiler().AddPolygon(PPT_MINT, 4 );
}

void MDrawContextR3::FillRectangle(int x, int y, int cx, int cy)
{
	SetState(MDS_CONTEXT);

	x += m_Origin.x;
	y += m_Origin.y;

	DWORD argb = m_Color.GetARGB();
	static PIXEL2DVERTEX p[4];

	p[0].p.x = float(x);
	p[0].p.y = float(y);
	p[0].p.z = 0;
	p[0].p.w = 1;
	p[0].color = argb;

	p[1].p.x = float(x+cx);
	p[1].p.y = float(y);
	p[1].p.z = 0;
	p[1].p.w = 1;
	p[1].color = argb;

	p[2].p.x = float(x+cx);
	p[2].p.y = float(y+cy);
	p[2].p.z = 0;
	p[2].p.w = 1;
	p[2].color = argb;

	p[3].p.x = float(x);
	p[3].p.y = float(y+cy);
	p[3].p.z = 0;
	p[3].p.w = 1;
	p[3].color = argb;

	m_pDevice->DrawPrimitiveUP(RPT_TRIANGLEFAN, 2, p, sizeof(PIXEL2DVERTEX));

	REngine::GetResourceProfiler().AddPolygon(PPT_MINT, 2 );
}

void MDrawContextR3::Draw(MBitmap *pBitmap, int x, int y, int w, int h, int sx, int sy, int sw, int sh)
{
	if(pBitmap==NULL) return;

	MCOLOR color(m_BitmapColor.r,m_BitmapColor.g,m_BitmapColor.b, (unsigned char)(0xFF * m_fOpacity));

	((MBitmapR3*)pBitmap)->Draw((float)x+m_Origin.x, (float)y+m_Origin.y, (float)w, (float)h, (float)sx, (float)sy, (float)sw, (float)sh, color.GetARGB(), m_Effect);
}

void MDrawContextR3::DrawRotate( MBitmap *pBitmap, MVector3& pos, MVector3& dir, int x, int y, int w, int h, int sx, int sy, int sw, int sh)
{
	if(pBitmap==NULL) return;

	MCOLOR color(m_BitmapColor.r,m_BitmapColor.g,m_BitmapColor.b, (unsigned char)(0xFF * m_fOpacity));


	((MBitmapR3*)pBitmap)->DrawRotate(pos, dir, (float)x+m_Origin.x, (float)y+m_Origin.y, (float)w, (float)h
		, (float)sx, (float)sy, (float)sw, (float)sh, color.GetARGB(), m_Effect);
}

void MDrawContextR3::DrawRotate( MBitmap *pBitmap, int tx, int ty, int tw, int th, int rx, int ry, float ra, int sx, int sy, int sw, int sh)
{
	if(pBitmap==NULL) return;

	MCOLOR color(m_BitmapColor.r,m_BitmapColor.g,m_BitmapColor.b, (unsigned char)(0xFF * m_fOpacity));


	((MBitmapR3*)pBitmap)->DrawRotate( (float)(tx+m_Origin.x), (float)(ty+m_Origin.y), (float)tw, (float)th, (float)rx, (float)ry, ra,
		(float)sx, (float)sy, (float)sw, (float)sh, color.GetARGB(), m_Effect);
}

void MDrawContextR3::DrawTiled(MBitmap *pBitmap, int x, int y, int w, int h, int sx, int sy, int sw, int sh)
{
	if(pBitmap==NULL) return;

	MCOLOR color(m_BitmapColor.r,m_BitmapColor.g,m_BitmapColor.b, (unsigned char)(0xFF * m_fOpacity));

	((MBitmapR3*)pBitmap)->DrawTiled((float)x+m_Origin.x, (float)y+m_Origin.y, (float)w, (float)h, (float)sx, (float)sy, (float)sw, (float)sh, color.GetARGB(), m_Effect);
}


void MDrawContextR3::DrawEx(int tx1, int ty1, int tx2, int ty2, int tx3, int ty3, int tx4, int ty4)
{
	MBitmapR3* pBitmap = (MBitmapR3*)m_pBitmap;
	if(pBitmap==NULL) return;
	MCOLOR color(0xFF, 0xFF, 0xFF, (unsigned char)(0xFF * m_fOpacity));

	pBitmap->DrawEx((float)tx1+m_Origin.x, (float)ty1+m_Origin.y,
		            (float)tx2+m_Origin.x, (float)ty2+m_Origin.y,
					(float)tx3+m_Origin.x, (float)ty3+m_Origin.y,
					(float)tx4+m_Origin.x, (float)ty4+m_Origin.y, color.GetARGB(), m_Effect);
}

void MDrawContextR3::SetClipRect(MRECT& r)
{
	// 뷰포트가 바뀔때 폰트 flush & 재설정해주어야 한다
	if(m_eCurrentState==MDS_FONT)
		RFont::EndFont();

	MDrawContext::SetClipRect(r);

/*	r.w+=1;
	r.h+=1;

	if(r.x<0){
		r.w += r.x;
		r.x = 0;
	}
	if(r.y<0){
		r.h += r.y;
		r.y = 0;
	}
	if(r.x+r.w>=MGetWorkspaceWidth()) r.w = MGetWorkspaceWidth() - r.x;
	if(r.y+r.h>=MGetWorkspaceHeight()) r.h = MGetWorkspaceHeight() - r.y;

	m_pDevice->SetViewport(r.x, r.y, r.w, r.h );
*/

	m_pDevice->SetViewport( r.x, r.y, max( 0, r.w + 1), max( 0, r.h + 1));

	if(m_eCurrentState==MDS_FONT)
		RFont::BeginFont();
}


//////////////////////////////////////////////////////////////////////////
// MFontR3

MFontR3::MFontR3(void)
{
	m_fScale = 1.0f;
}

MFontR3::~MFontR3(void)
{
	Destroy();
}

bool MFontR3::Create(rs3::RDevice* pDevice, const char* szAliasName, const char* szFontName, int nHeight, float fScale, bool bBold, bool bItalic, bool bOutline, float fOutlineOpacity, int nCacheSize, bool bAntiAlias)
{
	m_fScale = fScale;

	m_bAntiAlias = bAntiAlias;
	m_bOutline = bOutline;

	MFont::Create(szAliasName);

	return m_Font.Create(pDevice, MLocale::ConvAnsiToUTF16(szFontName).c_str(), nHeight, bBold, bItalic, bOutline ? 1 : 0, fOutlineOpacity, bAntiAlias);
}

void MFontR3::Destroy(void)
{
	m_Font.Destroy();
}

int MFontR3::DrawText(int x, int y, const char* szText, int nLength, DWORD dwColor)
{
	// outline은 8번 더 찍는 것이라 당연히 성능이 좋지 않다.
	// RFont에서 텍스쳐 캐슁해줘야 함. 지금은 그냥 보기 위한 용도로 이렇게 사용함.
	DWORD dwOutlineColor = dwColor & 0xFF000000;
	m_Font.DrawText_UNICODE((float)x,(float)y, MLocale::ConvAnsiToUTF16(szText).c_str(), nLength, dwColor, dwOutlineColor);
	return 0;
}

int MFontR3::GetHeight(void)
{
	return int(m_Font.GetHeight()*m_fScale);
}

int MFontR3::GetWidth(const char* szText, int nSize)
{
	return int(m_Font.GetTextWidth(MLocale::ConvAnsiToUTF16(szText).c_str(), nSize)*m_fScale);
}



//////////////////////////////////////////////////////////////////////////
// MBitmapR3

const float ADJUST_SIZE = 0.5f;

MBitmapR3::MBitmapR3(void)
{
	m_nWidth = m_nHeight = 0;

	m_pDevice = NULL;
	m_hTexture = rs3::R_NONE;

	m_dwStateBlock = NULL;

	// [0] = x1, [1] = y1, [2] = x2, [3] = y2
	m_fTexCoord[0] = 0.0f;
	m_fTexCoord[1] = 0.0f;
	m_fTexCoord[2] = 1.0f;
	m_fTexCoord[3] = 1.0f;
}

MBitmapR3::~MBitmapR3(void)
{
	Destroy();
}

bool MBitmapR3::Create(const char* szName, rs3::RDevice* pDevice, const char* szFileName)
{
	m_pDevice = pDevice;

	MBitmap::Create(szName);

	m_hTexture = pDevice->CreateTexture(szFileName, RTF_LINEAR, RCT_UNREDUCIBLE);
	if (m_hTexture == rs3::R_NONE) return false;

	CalcSizeFromTexCoord();

	return true;
}


void MBitmapR3::Destroy(void)
{
	if (m_hTexture != rs3::R_NONE)
	{
		m_pDevice->DeleteTexture(m_hTexture);
		m_hTexture = rs3::R_NONE;
	}
}

void MBitmapR3::CalcSizeFromTexCoord()
{
	if (m_hTexture == rs3::R_NONE) return;

	const rs3::RTextureInfo* pTexInfo = m_pDevice->GetTextureInfo(m_hTexture);
	if (pTexInfo == NULL) return;

	int width = pTexInfo->nTextureWidth;
	int height = pTexInfo->nTextureHeight;

	m_nWidth = (int)((m_fTexCoord[2] - m_fTexCoord[0]) * width);
	m_nHeight = (int)((m_fTexCoord[3] - m_fTexCoord[1]) * height);
}

int MBitmapR3::GetWidth(void)
{
	return m_nWidth;
}

int MBitmapR3::GetHeight(void)
{
	return m_nHeight;
}

// 현재는 state block을 사용하지 않는다.
//#define USE_STATEBLOCK

void MBitmapR3::BeginState(MDrawEffect effect)
{
/*
#ifdef USE_STATEBLOCK
//	if(m_dwStateBlock==NULL){
//		pd3dDevice->BeginStateBlock();
#endif

		switch (effect)
		{
		case MDE_NORMAL:
			{
				m_pDevice->SetBlending( RBLEND_SRCALPHA , RBLEND_INVSRCALPHA );
			}
			break;
		case MDE_ADD:
			{
				m_pDevice->SetBlending( RBLEND_SRCALPHA , RBLEND_ONE );
			}
			break;
		case MDE_MULTIPLY:
			{
				m_pDevice->SetBlending(RBLEND_ZERO,RBLEND_SRCCOLOR );
			}
			break;

		}

		m_pDevice->SetAlphaTestEnable(false);
		m_pDevice->SetTextureFilter(0,RTF_LINEAR);

		m_pDevice->SetLighting(false);
		m_pDevice->SetStencilEnable(false);
		m_pDevice->SetClipping(true);
		m_pDevice->SetClipPlanes(NULL,0);
		m_pDevice->SetVertexBlendEnable(false);
		m_pDevice->SetIndexedVertexBlendEnable(false);
		m_pDevice->SetFogEnable(false);
		m_pDevice->SetDepthEnable(true,false);

		m_pDevice->SetTextureStageState( 0,RTSS_COLOROP, RTOP_MODULATE );
		m_pDevice->SetTextureStageState( 0,RTSS_COLORARG1, RTA_TEXTURE );
		m_pDevice->SetTextureStageState( 0,RTSS_COLORARG2, RTA_DIFFUSE );
		m_pDevice->SetTextureStageState( 0,RTSS_ALPHAOP, RTOP_MODULATE );
		m_pDevice->SetTextureStageState( 0,RTSS_ALPHAARG1, RTA_TEXTURE );
		m_pDevice->SetTextureStageState( 0,RTSS_ALPHAARG2, RTA_DIFFUSE );
		
		m_pDevice->SetTextureCoord(0,RTC_NONE);

#ifdef USE_STATEBLOCK
//	}
//	else{
//		pd3dDevice->ApplyStateBlock(m_dwStateBlock);
//	}
#endif
*/

}
void MBitmapR3::EndState(void)
{
#ifdef USE_STATEBLOCK
//	if(m_dwStateBlock==NULL) pd3dDevice->EndStateBlock(&m_dwStateBlock);
#endif

	m_pDevice->SetTexture(0, R_NONE);
	m_pDevice->SetTexture(1, R_NONE);
}

inline void _swap(float& a,float& b) {
	static float temp;
	temp = a;
	a = b;
	b = temp;
}

void MBitmapR3::CheckDrawMode(float* fuv)
{
	if(m_DrawMode) {

		float temp[2];

		if(m_DrawMode & MBM_FlipLR)	{//좌우 바꾸기
			_swap(fuv[0],fuv[2]);
			_swap(fuv[1],fuv[3]);
			_swap(fuv[4],fuv[6]);
			_swap(fuv[5],fuv[7]);
		}
		if(m_DrawMode & MBM_FlipUD) {//상하 바꾸기
			_swap(fuv[0],fuv[6]);
			_swap(fuv[1],fuv[7]);
			_swap(fuv[2],fuv[4]);
			_swap(fuv[3],fuv[5]);
		}
		if(m_DrawMode & MBM_RotL90) {
			temp[0] = fuv[4];temp[1] = fuv[5];
			fuv[4]  = fuv[6];fuv[5]  = fuv[7];
			fuv[6]  = fuv[0];fuv[7]  = fuv[1];
			fuv[0]  = fuv[2];fuv[1]  = fuv[3];
			fuv[2]  = temp[0];fuv[3]  = temp[1];
		}
		if(m_DrawMode & MBM_RotR90) {
			temp[0] = fuv[6];temp[1] = fuv[7];
			fuv[6]  = fuv[4];fuv[7]  = fuv[5];
			fuv[4]  = fuv[2];fuv[5]  = fuv[3];
			fuv[2]  = fuv[0];fuv[3]  = fuv[1];
			fuv[0]  = temp[0];fuv[1]  = temp[1];
		}
	}
}

void MBitmapR3::Draw(float x, float y, float w, float h, float sx, float sy, float sw, float sh, 
					 DWORD dwColor, MDrawEffect effect)
{
	MGetMint()->GetDC()->SetState(MDS_BITMAP, effect);

	float ftw = (float)GetWidth();
	float fth = (float)GetHeight();

	// SetTexCoord에 따른 계산
	float fTexWidth = m_fTexCoord[2] - m_fTexCoord[0];
	float fTexHeight = m_fTexCoord[3] - m_fTexCoord[1];
	float fTexX = m_fTexCoord[0];
	float fTexY = m_fTexCoord[1];

	// 이 부분을 수정, UV offset 은 바꿔주지 않고 vertex에 offset을 걸었다.
	// http://www.gamza.net/ez2000/ezboard.cgi?db=Programming&action=read&dbf=10&page=0&depth=1 : 마법의 숫자 .5f
	float fuv[8] = {
		fTexX + fTexWidth * (sx)/ftw	,fTexY + fTexHeight * (sy)/fth,
		fTexX + fTexWidth * (sx+sw)/ftw	,fTexY + fTexHeight * (sy)/fth,
		fTexX + fTexWidth * (sx+sw)/ftw	,fTexY + fTexHeight * (sy+sh)/fth,
		fTexX + fTexWidth * (sx)/ftw	,fTexY + fTexHeight * (sy+sh)/fth
	};

	CheckDrawMode(fuv);

	//CUSTOMVERTEX Sprite[4] = {
	//	{x-ADJUST_SIZE,     y+h-ADJUST_SIZE,0, 1.0f, dwColor, fuv[6], fuv[7]},
	//	{x+w-ADJUST_SIZE,	y+h-ADJUST_SIZE,0, 1.0f, dwColor, fuv[4], fuv[5]},
	//	{x+w-ADJUST_SIZE,	y-ADJUST_SIZE,  0, 1.0f, dwColor, fuv[2], fuv[3]},
	//	{x-ADJUST_SIZE,		y-ADJUST_SIZE,  0, 1.0f, dwColor, fuv[0], fuv[1]},
	//};
	CUSTOMVERTEX Sprite[4] = {
		{x-ADJUST_SIZE,     y+h-ADJUST_SIZE,0, 1.0f, dwColor, fuv[6], fuv[7]},
		{x+w-ADJUST_SIZE,	y+h-ADJUST_SIZE,0, 1.0f, dwColor, fuv[4], fuv[5]},
		{x+w-ADJUST_SIZE,	y-ADJUST_SIZE,  0, 1.0f, dwColor, fuv[2], fuv[3]},
		{x-ADJUST_SIZE,		y-ADJUST_SIZE,  0, 1.0f, dwColor, fuv[0], fuv[1]},
	};

	m_pDevice->SetTexture(0, m_hTexture);

	m_pDevice->DrawPrimitiveUP( RPT_TRIANGLEFAN, 2, Sprite, sizeof( CUSTOMVERTEX ) );

	REngine::GetResourceProfiler().AddPolygon(PPT_MINT, 2 );
}


void MBitmapR3::DrawTiled(float x, float y, float w, float h, float sx, float sy, float sw, float sh, DWORD dwColor, MDrawEffect effect)
{
	MGetMint()->GetDC()->SetState(MDS_BITMAP, effect);

	float ftw = (float)GetWidth();
	float fth = (float)GetHeight();

	// sw,sh가 크면 자른다.
	if (w > 0 && w < sw) sw = w;
	if (h > 0 && h < sh) sh = h;

	// 이것도 위와 같이 ...
	float fuv[8] =
	{
		(sx)/ftw	,(sy)/fth,
		(sx+sw)/ftw	,(sy)/fth,
		(sx+sw)/ftw	,(sy+sh)/fth,
		(sx)/ftw	,(sy+sh)/fth
	};
	CheckDrawMode(fuv);

	m_pDevice->SetTexture( 0, m_hTexture);

	float end_x = x + w;
	float end_y = y + h;
	float tw, th;

	for ( float ty = y;  ty <= end_y;  ty += sh)
	{
		th = end_y - ty;
		if ( th > sh)
			th = sh;

		for ( float tx = x;  tx <= end_x;  tx += sw)
		{
			tw = end_x - tx;
			if ( tw > sw)
				tw = sw;

			Draw(tx, ty, tw, th, sx, sy, sw, sh, dwColor, effect);
		}
	}
}


void MBitmapR3::DrawEx(float tx1, float ty1, float tx2, float ty2, 
			float tx3, float ty3, float tx4, float ty4, DWORD dwColor, MDrawEffect effect)
{
	MGetMint()->GetDC()->SetState(MDS_BITMAP, effect);

	float ftw = (float)GetWidth();
	float fth = (float)GetHeight();

	float fuv[8] = {
		0.f,0.f,
		1.f,0.f,
		1.f,1.f,
		0.f,1.f
	};

	CheckDrawMode(fuv);

	CUSTOMVERTEX Sprite[4] = {
		{tx1-ADJUST_SIZE, ty1-ADJUST_SIZE, 0, 1.0f, dwColor, fuv[0], fuv[1]},
		{tx2-ADJUST_SIZE, ty2-ADJUST_SIZE, 0, 1.0f, dwColor, fuv[2], fuv[3]},
		{tx4-ADJUST_SIZE, ty4-ADJUST_SIZE, 0, 1.0f, dwColor, fuv[4], fuv[5]},
		{tx3-ADJUST_SIZE, ty3-ADJUST_SIZE, 0, 1.0f, dwColor, fuv[6], fuv[7]},
	};

	m_pDevice->SetTexture(0, m_hTexture);

	m_pDevice->DrawPrimitiveUP(RPT_TRIANGLEFAN, 2, Sprite, sizeof( CUSTOMVERTEX ) );
	REngine::GetResourceProfiler().AddPolygon(PPT_MINT, 2 );
}

void MBitmapR3::OnLostDevice(void)
{

}

void MBitmapR3::OnResetDevice(void)
{

}

void MBitmapR3::SetTexCoord(float x1, float y1, float x2, float y2)
{
	m_fTexCoord[0] = x1;
	m_fTexCoord[1] = y1;
	m_fTexCoord[2] = x2;
	m_fTexCoord[3] = y2;

	CalcSizeFromTexCoord();
}

void MDrawContextR3::BeginDraw()
{
	m_pDevice->SetLighting(false);
	m_pDevice->SetAlphaTestEnable(false);
	m_pDevice->SetFogEnable(false);
	m_pDevice->SetCullMode(RCULL_NONE);
	m_pDevice->SetBlending( RBLEND_NONE, RBLEND_NONE);
	m_pDevice->SetTexture(0, R_NONE);
	m_pDevice->SetTexture(1, R_NONE);
	m_pDevice->SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
	m_pDevice->SetTextureStageState(0, RTSS_ALPHAOP, RTOP_SELECTARG1);
	m_pDevice->SetTextureStageState(0, RTSS_COLORARG1, RTA_DIFFUSE);
	m_pDevice->SetTextureStageState(0, RTSS_ALPHAARG1, RTA_DIFFUSE);
	m_pDevice->SetTextureStageState(1, RTSS_COLOROP, RTOP_DISABLE);
	m_pDevice->SetTextureStageState(1, RTSS_ALPHAOP, RTOP_DISABLE);

	m_bInDraw = true;

	SetState(MDS_NONE);

	//m_pDevice->SetShader(R_NONE);
	//m_pDevice->SetFvF( D3DFVF_PIXEL2DVERTEX );

	//m_pDevice->SetBlending(RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
	//m_pDevice->SetAlphaTestEnable(false);
	//m_pDevice->SetDepthEnable(false, false);
	//m_pDevice->SetFogEnable(false);
	//m_pDevice->SetCullMode(RCULL_NONE);

	//m_pDevice->SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
	//m_pDevice->SetTextureStageState(0, RTSS_COLORARG1, RTA_DIFFUSE);
	//m_pDevice->SetTextureStageState(0, RTSS_ALPHAOP, RTOP_SELECTARG1);
	//m_pDevice->SetTextureStageState(0, RTSS_ALPHAARG1, RTA_DIFFUSE);
	//m_pDevice->SetTextureStageState(0, RTSS_COLOROP, RTOP_DISABLE );
	//m_pDevice->SetTextureStageState(0, RTSS_ALPHAOP, RTOP_DISABLE );

	//m_pDevice->SetTexture(0, R_NONE);
	//m_pDevice->SetTexture(1, R_NONE);
}

void MDrawContextR3::EndDraw()
{
	// 마지막 state 가 font 이면 flush 해주어야 하기 때문에.
	SetState(MDS_NONE);

	m_bInDraw = false;
}

void MBitmapR3::DrawRotate( MVector3& pos, MVector3& dir, float x, float y, float w, float h, float sx, float sy, float sw, float sh,
					 DWORD dwColor, MDrawEffect effect/*=MDE_NORMAL*/ )
{
	MGetMint()->GetDC()->SetState(MDS_BITMAP, effect);

	float ftw = (float)GetWidth();
	float fth = (float)GetHeight();

	// SetTexCoord에 따른 계산
	float fTexWidth = m_fTexCoord[2] - m_fTexCoord[0];
	float fTexHeight = m_fTexCoord[3] - m_fTexCoord[1];
	float fTexX = m_fTexCoord[0];
	float fTexY = m_fTexCoord[1];

	// 이 부분을 수정, UV offset 은 바꿔주지 않고 vertex에 offset을 걸었다.
	// http://www.gamza.net/ez2000/ezboard.cgi?db=Programming&action=read&dbf=10&page=0&depth=1 : 마법의 숫자 .5f
	float fuv[8] = {
		fTexX + fTexWidth * (sx)/ftw	,fTexY + fTexHeight * (sy)/fth,
		fTexX + fTexWidth * (sx+sw)/ftw	,fTexY + fTexHeight * (sy)/fth,
		fTexX + fTexWidth * (sx+sw)/ftw	,fTexY + fTexHeight * (sy+sh)/fth,
		fTexX + fTexWidth * (sx)/ftw	,fTexY + fTexHeight * (sy+sh)/fth
	};

	CheckDrawMode(fuv);

	CUSTOMVERTEX Sprite[4] = {
		{x-ADJUST_SIZE,     y+h-ADJUST_SIZE,0, 1.0f, dwColor, fuv[6], fuv[7]},
		{x+w-ADJUST_SIZE,	y+h-ADJUST_SIZE,0, 1.0f, dwColor, fuv[4], fuv[5]},
		{x+w-ADJUST_SIZE,	y-ADJUST_SIZE,  0, 1.0f, dwColor, fuv[2], fuv[3]},
		{x-ADJUST_SIZE,		y-ADJUST_SIZE,  0, 1.0f, dwColor, fuv[0], fuv[1]},
	};

	mat4 matTM;
	matTM.SetLocalMatrix(vec3::ZERO, dir, vec3::AXISZ);

	vec3 datumpos = vec3(Sprite[0].x, Sprite[0].y, 0) + pos;

	for (int i = 0; i < 4; i++)
	{
		CUSTOMVERTEX vertex = Sprite[i];
		vec3 vertexpos = vec3(vertex.x,vertex.y,vertex.z)-datumpos;
		vertexpos = vertexpos * matTM;
		vertexpos += datumpos;
		Sprite[i].x = vertexpos.x;
		Sprite[i].y = vertexpos.y;
		Sprite[i].z = vertexpos.z;
	}

	m_pDevice->SetTexture(0, m_hTexture);

	m_pDevice->DrawPrimitiveUP( RPT_TRIANGLEFAN, 2, Sprite, sizeof( CUSTOMVERTEX ) );

	REngine::GetResourceProfiler().AddPolygon(PPT_MINT, 2 );
}


/*	DrawRotate

	x, y, w, h = 이미지를 출력할 타겟 위치 및 사이즈
	rx, ry = 이미지를 회전시키는 회전 축 좌표
	ra = 회전 각도(radian)
	sx, sy, sw, sh = 이미지를 가져올 소스 위치 및 사이즈
*/
void MBitmapR3::DrawRotate( float tx, float ty, float tw, float th, float rx, float ry, float ra, float sx, float sy, float sw, float sh, DWORD dwColor, MDrawEffect effect)
{
	MGetMint()->GetDC()->SetState( MDS_BITMAP, effect);

	float ftw = (float)GetWidth();
	float fth = (float)GetHeight();

	float fTexWidth = m_fTexCoord[2] - m_fTexCoord[0];
	float fTexHeight = m_fTexCoord[3] - m_fTexCoord[1];
	float fTexX = m_fTexCoord[0];
	float fTexY = m_fTexCoord[1];

	float fuv[ 8] =
	{
		fTexX + fTexWidth * (sx)/ftw	,fTexY + fTexHeight * (sy)/fth,
		fTexX + fTexWidth * (sx+sw)/ftw	,fTexY + fTexHeight * (sy)/fth,
		fTexX + fTexWidth * (sx+sw)/ftw	,fTexY + fTexHeight * (sy+sh)/fth,
		fTexX + fTexWidth * (sx)/ftw	,fTexY + fTexHeight * (sy+sh)/fth
	};

	CheckDrawMode( fuv);

	CUSTOMVERTEX Sprite[ 4] =
	{
		{   -rx-ADJUST_SIZE,	th-ry-ADJUST_SIZE,	0, 1.0f, dwColor, fuv[ 6], fuv[ 7]},
		{ tw-rx-ADJUST_SIZE,	th-ry-ADJUST_SIZE,	0, 1.0f, dwColor, fuv[ 4], fuv[ 5]},
		{ tw-rx-ADJUST_SIZE,	  -ry-ADJUST_SIZE,	0, 1.0f, dwColor, fuv[ 2], fuv[ 3]},
		{   -rx-ADJUST_SIZE,	  -ry-ADJUST_SIZE,	0, 1.0f, dwColor, fuv[ 0], fuv[ 1]},
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


	m_pDevice->SetTexture( 0, m_hTexture);

	m_pDevice->DrawPrimitiveUP( RPT_TRIANGLEFAN, 2, Sprite, sizeof( CUSTOMVERTEX));

	REngine::GetResourceProfiler().AddPolygon(PPT_MINT, 2 );
}

//////////////////////////////////////////////////////////////////////////
// MCursorR3
// Bitmap Hardware Cursor for realspace3
class MCursorR3 : public MCursor{
	rs3::RDeviceD3D*		m_pDevice;
	vector<RImage*>			m_cursorImages;
	int						m_nCurrnetImage;

public:
	MCursorR3(const char* szName, RDeviceD3D* pDevice) : MCursor(szName), m_pDevice(pDevice), m_nCurrnetImage(-1)
	{
	}
	~MCursorR3()
	{
		Destroy();
	}

	void Destroy()
	{
		while( !m_cursorImages.empty() )
		{
			delete m_cursorImages.back();
			m_cursorImages.pop_back();
		}
	}

protected:
	virtual void MCursorR3::OnSetBitmap(MBitmap* pBitmap)
	{
		_ASSERT( m_cursorImages.empty() );
		Destroy();

		MBitmapR3* pBitmapR3 = (MBitmapR3*)m_pBitmap;
		RTexture* pTexture = pBitmapR3->GetTexture();

		RLOCKED_RECT d3dLockRect;
		if(m_pDevice->LockRect( pTexture, 0, &d3dLockRect, NULL, 0))
		{
			RImage* pImage = m_pDevice->CreateImage(32,32,RFMT_A8R8G8B8, d3dLockRect.pBits,d3dLockRect.Pitch, 0,0 );
			m_cursorImages.push_back(pImage);
			m_pDevice->UnlockRect( pTexture, 0);
		}
	}

	virtual void OnSetAnimationTemplate(MAnimationTemplate* pAnimationTemplate)
	{
		_ASSERT( m_cursorImages.empty() );
		Destroy();

		MBitmapR3* pBitmapR3 = (MBitmapR3*)pAnimationTemplate->GetBitmap();
		RTexture* pTexture = pBitmapR3->GetTexture();

		RLOCKED_RECT d3dLockRect;
		if(m_pDevice->LockRect( pTexture, 0, &d3dLockRect, NULL, 0))
		{
			for(int i=0;i< pAnimationTemplate->GetSplitCount(); i++)
			{
				RImage* pImage = m_pDevice->CreateImage(32,32,RFMT_A8R8G8B8, d3dLockRect.pBits,d3dLockRect.Pitch, 32*i,0 );
				m_cursorImages.push_back(pImage);
			}
			m_pDevice->UnlockRect( pTexture, 0);
		}
	}

	virtual void OnDraw(MDrawContext* pDC, int x, int y)
	{
		// 하드웨어 커서를 사용하므로 아무것도 그리지 않는다
		// 설정만 바꿔준다
		if(m_nCurrnetImage!=m_nSceneNum)
		{
			m_pDevice->SetCursorProperties( 0,0, m_cursorImages[m_nSceneNum]);
			m_nCurrnetImage = m_nSceneNum;
		}
	}

	virtual void OnSet()	{	InvalidateCurrent(); }

private:
	void InvalidateCurrent()	{	m_nCurrnetImage = -1;	}

};

//////////////////////////////////////////////////////////////////////////
// Mint4R3

MBitmap* Mint4R3::NewBitmap(const char* szName, const char* szFileName)
{
	// 같은 이름으로는 등록이 안되도록.
	MBitmap* pBitmap = MBitmapManager::Get(szName);
	if( pBitmap != NULL)
		return NULL;

	MBitmapR3* pNew = new MBitmapR3();
	bool bRet = pNew->Create(szName, (rs3::RDevice*)REngine::GetDevicePtr(), szFileName);
	if(bRet==false){
		delete pNew;
		return NULL;
	}
	return pNew;
}

/*	NewFont
	Create a font

	var : pszName = 코드에서 사용할 폰트의 alias 이름(예 : "굴림", "바탕"...)
			(주의) 외부 폰트 파일을 사용할 경우엔 반드시 폰트의 face name과도 매칭되어야 한다.
			폰트의 face name은 윈도우즈에서 폰트 파일을 열었을 때 화면 맨 상단에 표시된다.

	pszFileName = 폰트의 파일명(예 : "굴림", "NGULIM.TTF").  외부 폰트 파일을 우선 검색한
			다음 없을 경우엔 윈도우즈에 등록된 시스템 폰트에서 검색한다
*/

MFont* Mint4R3::NewFont(const char* pszAliasName, const char* pszFontName, const char* pszFileName, int nHeight, bool bOutline, float fOutlineOpacity, bool bAntiAlias)
{
	float fScale = 1.0f;
	bool bBold = false;
	bool bItalic = false;

	// 외부 폰트 등록
	bool bRetVal = RFont::AddFontResourceFromFile(MLocale::ConvAnsiToUTF16(pszFileName).c_str());
	if ( bRetVal)
	{
		// Create font
		MFontR3* pFont = new MFontR3();

		if( !pFont->Create( MDrawContextR3::GetDevice(), pszAliasName, pszFontName, nHeight,
			                fScale, bBold, bItalic, bOutline, fOutlineOpacity, -1, bAntiAlias))
		{
			// Error
			SAFE_DELETE( pFont);
			return NULL;
		}

		return pFont;
	}

	// 만약 외부 폰트가 없으면 윈도우즈 시스템 폰트를 찾아서 등록한다
	else
	{
		// Create font
		MFontR3* pFont = new MFontR3();

		if( !pFont->Create( MDrawContextR3::GetDevice(), pszAliasName, pszFileName, nHeight,
			                fScale, bBold, bItalic, bOutline, fOutlineOpacity, -1, bAntiAlias))
		{
			// Error
			SAFE_DELETE( pFont);
			return NULL;
		}

		return pFont;
	}
}

MCursor* Mint4R3::NewCursor(const char* pszAliasName)
{
//	return Mint::NewCursor(pszAliasName);

	MCursor* pCursor = new MCursorR3(pszAliasName, (rs3::RDeviceD3D*)REngine::GetDevicePtr());
	return pCursor;
}
