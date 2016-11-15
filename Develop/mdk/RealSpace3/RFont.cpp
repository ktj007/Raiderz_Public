//----------------------------------------------------------------------------------------------------
// 파일명 : RFont.cpp
// 설  명 : Embodiment of RFont.h
//
// Copyright(c) 2008 Maiet Ent. Co.,Ltd. All Rights Reserved.
//----------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "RFont.h"
#include <crtdbg.h>
#include <mbstring.h>
#include "mDebug.h"
#include "mprofiler.h"
#include "dxerr.h"
#include "RUtil.h"
#include "RProfileInfo.h"
#include "RSUtil.h"
#include "MFileSystem.h"
#include <atlconv.h>
#include <atlexcept.h>

#pragma comment(lib, "dxerr.lib")

#ifdef _USE_GDIPLUS
#include "unknwn.h"
#include "gdiplus.h"
	 using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")
#endif
//#undef _USE_GDIPLUS

namespace rs3 {

//////////////////////////////////////////////////////////////////////////
// Define
//////////////////////////////////////////////////////////////////////////
// Font Texture
#define TEXTURE_SIZE	1024	
#define CELL_SIZE		32		
#define	MAX_CHARCOUNT	(TEXTURE_SIZE * TEXTURE_SIZE ) / (CELL_SIZE * CELL_SIZE)	// 최대 문자열 개수
// Font
#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define MAX_FONT_BUFFER		4000
#define RFONT_VERTEXCOUNT	4

//////////////////////////////////////////////////////////////////////////
// Struct
//////////////////////////////////////////////////////////////////////////
struct FONT2DVERTEX { 
	RVector3 p;   
	DWORD color;     
	FLOAT tu, tv; 
};

//////////////////////////////////////////////////////////////////////////
// RFont static member-variable
//////////////////////////////////////////////////////////////////////////
RDevice*		RFont::m_pDevice = NULL;
bool			RFont::m_bInFont	= false;
RMatrix			RFont::m_matOrgView;
RMatrix			RFont::m_matOrgProj;

// Font Texture
RFontTexture	g_FontTexture;		

// Font
static	int				g_nFontCount = 0;
static  int				g_nCntUsedID = 0;
static	FONT2DVERTEX	g_FontVertexBuffer_Font[4*MAX_FONT_BUFFER];
static	FONT2DVERTEX	g_FontVertexBuffer_Outline[4*MAX_FONT_BUFFER];
static	WORD			g_FontIndexBuffer[6*MAX_FONT_BUFFER];
static  bool			g_bFinal = false;	// 프로그램에서 한번만 생성하고 종료하게 하기 위해 있는 변수.
static	int				g_nOutlineWidth = 0;


// typdef으로 tstring를 만들어 두었으면 좋겠음.
//#ifdef _UNICODE
static std::vector<std::wstring> m_vFontFile;
//#else
//static std::vector<std::string> m_vFontFile;
//#endif

static std::vector<std::wstring> m_vFontMemory;
static std::vector<HANDLE> m_vFontMemoryHandle;


//__inline 
wchar_t* wcsinc(const wchar_t * _Pc) 
{ 
	return (wchar_t*)(_Pc+1); 
}

//----------------------------------------------------------------------------------------------------
// RFontTexture
//----------------------------------------------------------------------------------------------------

RFontTexture::RFontTexture() : 
m_pd3dDevice(NULL), 
m_CellInfo(NULL), 
m_pTexture_Outline(NULL), 
m_pTexture_Font(NULL),
m_pBitmapBits_Font(NULL),
m_pBitmapBits_Outline(NULL),
m_nWidth(0),
m_nHeight(0),
m_nX(0),
m_nY(0),
m_nCell(0),
m_LastUsedID(0)
{ 

}

RFontTexture::~RFontTexture()
{

}

//----------------------------------------------------------------------------------------------------
//  Create(RDevice *pDevice)
/**
@brief		Font Texture의 생성과 Cell에 대한 정보 배열의 초기화를 한다.
@param		pDevice		\n[in] Device Pointer.
@return		bool
*/
//----------------------------------------------------------------------------------------------------
bool RFontTexture::Create(RDevice *pDevice) 
{
	m_pd3dDevice = pDevice;

	m_nWidth	= TEXTURE_SIZE;
	m_nHeight	= TEXTURE_SIZE;

	m_nX = m_nWidth / CELL_SIZE;
	m_nY = m_nHeight/ CELL_SIZE;

	m_pTexture_Font	= m_pd3dDevice->CreateTexture(m_nWidth, m_nHeight, RFMT_A8R8G8B8);
	if(!m_pTexture_Font) 
		return false;

	m_pTexture_Outline = m_pd3dDevice->CreateTexture(m_nWidth, m_nHeight, RFMT_A8R8G8B8);
	if(!m_pTexture_Outline) 
		return false;

	m_nCell = m_nX * m_nY;
	m_CellInfo = new RFONTTEXTURECELLINFO[m_nCell];
	for( int i = 0; i < m_nCell; i++ ) 
	{
		m_CellInfo[i].nID		= 0;
		m_CellInfo[i].nIndex	= i;
		m_PriorityQueue.push_back(&m_CellInfo[i]);
	}

	//	memory DC 와 Buffer 를 만든다
	m_hDC = CreateCompatibleDC(NULL);

	//////////////////////////////////////////////////////////////////////////
	// Bitmap Creation
	BITMAPINFO bmi;
	ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
	bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth       =  (int)CELL_SIZE;
	bmi.bmiHeader.biHeight      = -(int)CELL_SIZE;
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount    = 32;

	// Font
	m_hbmBitmap_Font = CreateDIBSection(m_hDC, &bmi, DIB_RGB_COLORS, (VOID**)&m_pBitmapBits_Font, NULL, 0);
	if( m_pBitmapBits_Font == NULL )
	{
		DeleteDC(m_hDC);
		return false;
	}

	// Outline
	m_hbmBitmap_Outline = CreateDIBSection(m_hDC, &bmi, DIB_RGB_COLORS, (VOID**)&m_pBitmapBits_Outline, NULL, 0);
	if( m_pBitmapBits_Outline == NULL )
	{
		DeleteDC(m_hDC);
		return false;
	}

	SetMapMode(m_hDC, MM_TEXT);

#ifdef _USE_GDIPLUS
	GdiplusStartupInput gpsi;
	if (GdiplusStartup(&m_pToken, &gpsi, NULL) != Ok) 
	{
		assert(0 && "GDI+ 라이브러리를 초기화할 수 없습니다.");
		return false;
	}
#endif

	return true;
}

//----------------------------------------------------------------------------------------------------
//  Destroy()
/**
@brief		HDC와 Texture 해제.
@return		void
*/
//----------------------------------------------------------------------------------------------------
void RFontTexture::Destroy() 
{
#ifdef _USE_GDIPLUS
	GdiplusShutdown(m_pToken);
#endif

	if(m_hDC) 
	{
		//SelectObject(m_hDC, m_hPrevBitmap);
		
		if(m_hbmBitmap_Font)
		{
			DeleteObject(m_hbmBitmap_Font);
			m_hbmBitmap_Font = NULL;
		}

		if(m_hbmBitmap_Outline)
		{
			DeleteObject(m_hbmBitmap_Outline);
			m_hbmBitmap_Outline = NULL;
		}

		DeleteDC(m_hDC);
		m_hDC = NULL;
	}

	if(m_pTexture_Font) 
	{
		m_pd3dDevice->DeleteTexture(m_pTexture_Font);
		m_pTexture_Font = NULL;
	}
	if(m_pTexture_Outline) 
	{
		m_pd3dDevice->DeleteTexture(m_pTexture_Outline);
		m_pTexture_Outline = NULL;
	}

	SAFE_DELETE_ARRAY(m_CellInfo);
	m_PriorityQueue.clear();
}

//----------------------------------------------------------------------------------------------------
//  GetCharWidth(HFONT hFont, const TCHAR* szChar)
/**
@brief		문자열에서 폰트의 크기, 스타일, 자간까지 고려하여 정확하게 문자열의 실제 길이를 계산 한다.
@param		hFont		\n[in] Handle to Font object.
@param		szChar		\n[in] Null-terminated string.
@return		int
*/
//----------------------------------------------------------------------------------------------------
int RFontTexture::GetCharWidth(HFONT hFont, const wchar_t* szChar)
{
	SIZE size;
	HFONT hPrevFont = (HFONT)SelectObject(m_hDC, hFont);

	GetTextExtentPoint32W(m_hDC, szChar, wcslen(szChar), &size);

	SelectObject(m_hDC, hPrevFont);

	return size.cx;
}

//----------------------------------------------------------------------------------------------------
//  IsNeedUpdate(int nIndex, int nID)
/**
@brief		텍스쳐가 갱신 되어야 하는지 검사 한다.
@param		nIndex		\n[in] Texutere에서의 Index.
@param		nID			\n[in] 등록 할때 순번.
@return		bool
*/
//----------------------------------------------------------------------------------------------------
bool RFontTexture::IsNeedUpdate(int nIndex, int nID)
{
	// 1. 등록되지 않은 글자라면 추가 한다.
	if(nIndex == -1) 
		return true;

	// 2. 이미 같은 ID로 등록이 되어 있다면
	if(m_CellInfo[nIndex].nID == nID) 
	{
		// 3. 막 사용되어진것이므로 뒤로 돌린다. (삭제 우선 순위를 낮춘다.)
		m_PriorityQueue.splice(m_PriorityQueue.end(),m_PriorityQueue,m_CellInfo[nIndex].itr);
		return false;	// 갱신할 필요 없다.
	}

	return true;
}

//----------------------------------------------------------------------------------------------------
//  GetCharABCWidth(const TCHAR* szText, SIZE size)
/**
@brief		GetCharABCWidths는 hdc 매게변수로 지정된 DC에 현재 선정된 글꼴을 기준으로 uFirstchar와
uLastChar로 지정한 문자들의 "ABC"너비를 알려준다. 

abcA 필드는 그리프( glyph = 글자모양 ) 앞 여백너비, 
abcB 필드는 그리프 너비, 
abcC 필드는 그리프 뒤 여백 너비이다.
*/
//----------------------------------------------------------------------------------------------------
int RFontTexture::GetCharABCWidth(const wchar_t* szText, SIZE size)
{
	ABCFLOAT abcfloat;	 
	UINT uiChar = szText[0];

	// 지정된 범위에 있는 연속된 문자의 폭을 논리 단위로 얻는다
	GetCharABCWidthsFloatW(m_hDC, uiChar, uiChar, &abcfloat);
	if (abcfloat.abcfA < 0.f)
	{
		int ioutLeftSpace  = (int)abcfloat.abcfA;
		size.cx -= ioutLeftSpace;
	}

	uiChar = szText[wcslen(szText)-1];
	GetCharABCWidthsFloatW(m_hDC, uiChar, uiChar, &abcfloat);
	if (abcfloat.abcfC < 0.f)
	{
		int ioutRightSpace = (int)abcfloat.abcfC;
		size.cx -= ioutRightSpace;
	}

	return size.cx;
}

//----------------------------------------------------------------------------------------------------
//  MakeFontBitmap(HFONT hFont, RCHARINFO *pInfo, const TCHAR* szText, int nOutlineStyle, DWORD nColorArg1, DWORD nColorArg2)
/**
@brief		
@param		hFont			\n[in] Handle to Font object.
@param		pInfo			\n[in] RCHARINFO Pointer.
@param		szText			\n[in] Null-terminated string.
@param		nOutlineStyle	\n[in] 현재 지원 하지 않음.
@param		nColorArg1		\n[in] 현재 지원 하지 않음.
@param		nColorArg2		\n[in] 현재 지원 하지 않음.
@return		bool
*/
//----------------------------------------------------------------------------------------------------
bool RFontTexture::MakeFontBitmap(HFONT hFont, RCHARINFO *pInfo, const wchar_t* szText, int nOutlineWidth, float fOutlineOpacity, DWORD nColorArg1, DWORD nColorArg2)
{
	HFONT hPrevFont = (HFONT)SelectObject(m_hDC, hFont);

	SIZE size;
	GetTextExtentPoint32W(m_hDC, szText, wcslen(szText), &size);
	size.cx = GetCharABCWidth(szText, size);
	int nWidth	= min(size.cx, CELL_SIZE);
	int nHeight = min(size.cy, CELL_SIZE);

#ifdef _USE_GDIPLUS
	if(nOutlineWidth)
		Outline_GDIPlusPath(hFont, szText, nOutlineWidth);	// 외부 폰트 지원 안됨
#else

	m_LastUsedID++;
	// 맨 앞에 있으면 가장 오래전에 사용된 Cell이다.
	RFONTTEXTURECELLINFO *pCellInfo = *(m_PriorityQueue.begin());
	pCellInfo->nID = m_LastUsedID;
	pCellInfo->itr = m_PriorityQueue.begin();

	RECT rt;
	rt.left = rt.top = 0;
	rt.bottom = rt.right = CELL_SIZE;

	if(nOutlineWidth)	// 외곽선의 두께가 있으면 외곽선 출력
	{
		m_hPrevBitmap = (HBITMAP)SelectObject(m_hDC, m_hbmBitmap_Outline);

		FillRect( m_hDC, &rt, (HBRUSH)GetStockObject(BLACK_BRUSH));
		
		SetBkMode(m_hDC, TRANSPARENT);			// 배경모드 설정
		int opacity = (int)( 255.0f * min( 1.0f, fOutlineOpacity));
		SetTextColor(m_hDC, RGB(opacity,opacity,opacity));	// RGBA(255,255,255,0));
		SetBkColor(m_hDC, 0x00000000);			// Alpha가 0이고 BkColor의 Alpha도 0이면 RBG값 자체가 기록되지 않는다.
		SetTextAlign(m_hDC, TA_TOP);

		// OPAQUE		불투명한 배경을 사용한다. 그래서 SetBkColor에 의해 지정된 배경 색상에 의해 뒷쪽의 그림이나 무늬가 지워진다. 이 모드가 디폴트이다.
		// TRANSPARENT	투명한 배경색상을 사용한다. 그래서 문자를 출력한 후에도 배경이 바뀌지 않는다. 즉 문자 사이 사이의 여백에 있는 원래 배경이 지워지지 않는다.
		Outline_GDIOverDraw(szText, nOutlineWidth);

		// nOutlineWidth * 2 : 양쪽으로 늘어나기 때문에 * 2
		UploadTexture( m_pTexture_Outline, pInfo, m_pBitmapBits_Outline, pCellInfo->nIndex, size.cx + nOutlineWidth * 2, size.cy + nOutlineWidth * 2);
		SelectObject(m_hDC, m_hPrevBitmap);
	}

#endif

	m_hPrevBitmap = (HBITMAP)SelectObject(m_hDC, m_hbmBitmap_Font);
	SetBkMode(m_hDC, OPAQUE);		// 배경모드 설정
	SetTextColor(m_hDC, RGB(255,255,255));	// RGBA(255,255,255,0));
	SetBkColor(m_hDC, 0x00000000);			// Alpha가 0이고 BkColor의 Alpha도 0이면 RBG값 자체가 기록되지 않는다.
	SetTextAlign(m_hDC, TA_TOP);
	FillRect( m_hDC, &rt, (HBRUSH)GetStockObject(BLACK_BRUSH));
	ExtTextOutW(m_hDC, nOutlineWidth, nOutlineWidth, ETO_OPAQUE, NULL, szText, wcslen(szText), NULL);

	bool bRet = UploadTexture( m_pTexture_Font, pInfo, m_pBitmapBits_Font, pCellInfo->nIndex, size.cx + nOutlineWidth * 2, size.cy + nOutlineWidth * 2 );
	
	// 막 사용 되어진것 이므로 뒤로 돌린다.
	// m_PriorityQueue 리스트의 m_PriorityQueue.begin()을 m_PriorityQueue의 m_PriorityQueue.end() 앞으로 이동시키고, m_PriorityQueue.begin()은 삭제 한다.
	m_PriorityQueue.splice(m_PriorityQueue.end(),m_PriorityQueue,m_PriorityQueue.begin());
	pInfo->nFontTextureID	 = pCellInfo->nID;
	pInfo->nFontTextureIndex = pCellInfo->nIndex;
	pInfo->nWidth			 = nWidth;
	pInfo->nHeight			 = nHeight;

	SelectObject(m_hDC, hPrevFont);
	SelectObject(m_hDC, m_hPrevBitmap);

	return bRet;
}

// 외곽선 겹쳐 그리기
void RFontTexture::Outline_GDIOverDraw(const wchar_t* szText, int nOutlineWidth)
{
	for (int x = -nOutlineWidth; x <= nOutlineWidth; ++x)
		for (int y = -nOutlineWidth; y <= nOutlineWidth; ++y)
			ExtTextOutW(m_hDC, nOutlineWidth + x, nOutlineWidth + y, ETO_OPAQUE, NULL, szText, wcslen(szText), NULL);
}

// GDI에서 얻은 Path를 GDI+에 적용
void RFontTexture::Outline_GDIPlusPath(HFONT hFont, const wchar_t* szText, int nOutlineWidth)
{
#ifdef _USE_GDIPLUS
	// GDI+ Font
	WCHAR wstrText[256];
	int nTextLen = strlen(szText)+1;
	MultiByteToWideChar(CP_ACP, 0, szText, -1, wstrText, nTextLen-1);
	wstrText[nTextLen-1] = 0;

	Graphics graphics(m_hDC);
	Gdiplus::Font font(m_hDC, hFont);
	
//	graphics.Clear(Color(0,0,0));

	//if (nOutlineStyle == 0)
	//{
	//	SolidBrush  solidBrush(Color(255, 255, 255, 255));
	//	graphics.DrawString(wstrText, -1, &font, PointF(0.0f, 0.0f), pTypoFormat, &solidBrush);
	//} else 
	//if (nOutlineStyle == 1)	// 아웃라인이 그려진다
	//{
		GraphicsPath path;
		FontFamily fontFamily;
		font.GetFamily(&fontFamily);
		const StringFormat* pTypoFormat = StringFormat::GenericTypographic();

		TEXTMETRIC tm;
		GetTextMetrics(m_hDC, &tm);
		int nHeight = min( (int)tm.tmHeight, (int)CELL_SIZE-2);

		path.AddString(	wstrText, -1, &fontFamily, FontStyleBold, nHeight, PointF(-1.0f, -1.0f), pTypoFormat);

		graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);

		Pen pen(Color(nColorArg2), 2.0f);
		graphics.DrawPath(&pen, &path);

//		SolidBrush brush(Color((ARGB)nColorArg1));
//		graphics.FillPath((Brush*)&brush, &path);
	//}
	//else if (nOutlineStyle == 2)	// 그림자만 포함된다. 좌측의 한픽셀 여백이 있으므로 왼쪽으로 한픽셀. 자간은 같다
	//{
	//	SolidBrush  solidBrush2(Color((ARGB)nColorArg2));
	//	graphics.DrawString(wstrText, -1, &font, PointF(1.0f, 1.0f), pTypoFormat, &solidBrush2);

	//	SolidBrush  solidBrush1(Color((ARGB)nColorArg1));

	//	graphics.DrawString(wstrText, -1, &font, PointF(0.0f, 0.0f), pTypoFormat, &solidBrush1);

	//	// 숫자일 경우엔 자간이 좁기 때문에 1을 늘린다.
	//	char chChar = (char)wstrText[0];
	//	if ( (chChar >= '0') && (chChar <= '9'))
	//		nWidth++;
	//}
#endif
}


//----------------------------------------------------------------------------------------------------
//  UploadTexture(RCHARINFO *pCharInfo, DWORD* pBitmapBits, int w, int h)
/**
@brief		Font Texture에 락을 걸고 폰트를 출력 한다.
@param		pCharInfo		\n[in] RCHARINFO Pointer.
@param		pBitmapBits		\n[in] Texture bit values.
@param		w				\n[in] Width of the texture.
@param		h				\n[in] Height of the texture.
@return		bool
*/
//----------------------------------------------------------------------------------------------------
bool RFontTexture::UploadTexture(RTexture* pTexture, RCHARINFO *pCharInfo, DWORD* pBitmapBits, int nIndex, int w, int h)
{
	RLOCKED_RECT d3dlr;
	if( m_pd3dDevice->LockRect(pTexture, 0, &d3dlr, NULL, NULL) )
	{
		// 실제로 텍스쳐에 업로드한다.
		int x = nIndex % GetCellCountX();
		int y = nIndex / GetCellCountX();

		// 실제 픽셀의 위치
		int _x = x*CELL_SIZE;
		int _y = y*CELL_SIZE;

		// 최대 크기 설정
		// -1을 하는 이유는 외부 폰트의 필터링등의 이유로 1픽셀이 더 포함되기 때문에 이것을 없애기 위한 수치.
		int uw = min(CELL_SIZE - 1, w);
		int uh = min(CELL_SIZE - 1, h);

		BlitRect((BYTE*)d3dlr.pBits, d3dlr.Pitch, _x, _y, _x+uw, _y+uh, CELL_SIZE, CELL_SIZE, pBitmapBits);

		m_pd3dDevice->UnlockRect(pTexture, 0);

		return true;
	}

	return false;
}


//----------------------------------------------------------------------------------------------------
//  BlitRect(BYTE* pDest, int x1, int y1, int x2, int y2, int w, int h, DWORD* pBitmapBits, INT Pitch)
/**
@brief		텍셀에 출력.
@param		pDest		\n[in] Destination Texture bit values
@param		x1			\n[in] x-coord of destination upper-left corner.
@param		y1			\n[in] y-coord of destination upper-left corner.
@param		x2			\n[in] width of destination rectangle.
@param		y2			\n[in] height of destination rectangle.
@param		w			\n[in] Width of the source bitmapBits.
@param		h			\n[in] Height of the source bitmapBits.
@param		pBitmapBits	\n[in] source bitmapBits.
@param		Pitch		\n[in] bit values in line.
@return		void
*/
//----------------------------------------------------------------------------------------------------
void RFontTexture::BlitRect(BYTE* pDest, INT destPitch, int x1, int y1, int x2, int y2, int sw, int sh, DWORD* pSrcBitmapBits)
{
	if(pDest == NULL) 
		return;

	DWORD* pDestTemp = NULL;

	int by = 0;
	int bx = 0;

	// 픽셀이 튀는것을 방지하기 위해 1픽셀만큼 더 그리되 마지막 픽셀은 투명값(0) 을 넣어준다
	for(int y = y1; y < y2 + 1; ++y) 
	{
		pDestTemp = (DWORD*)( pDest + (y * destPitch) );
		bx = 0;

		for(int x = x1; x < x2 + 1; ++x)
		{
			DWORD dwPixel = pSrcBitmapBits[ (sw * by) + (bx) ];

			// 글자색이 흰색이므로 RGB중 아무값이나 얻어와서 Alpha 채널에 넣는다.
			dwPixel = dwPixel << 8;		 // Red -> Alpha
			dwPixel = dwPixel | 0xffffff;
			//dwPixel |= 0xff0000;	// ARGB
			//if(dwPixel)	// 흰색이므로 알파값만 추가해 준다.
			//	dwPixel |= 0xff000000;

			*(pDestTemp+x) = dwPixel;

			++bx;

			if(x == x2)	// 마지막 열
				*(pDestTemp+x) = 0x0;
		}

		++by;

		if(y == y2)	// 마지막 행
		{
			for(int x = x1; x < x2+1; ++x)
			{
				*(pDestTemp+x) = 0x0;
			}
		}
	}
}


//----------------------------------------------------------------------------------------------------
// RFont
//----------------------------------------------------------------------------------------------------

RFont::RFont()
{
	m_nFontHeight = 0;
	m_nOutlineWidth = 0;
	m_fOutlineOpacity = 1.0f;
}

RFont::~RFont()
{
	Destroy();
}

HRESULT RFont::OnLostDevice()
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT RFont::OnResetDevice()
{
	HRESULT hr = S_OK;

	return hr;
}

bool RFont::Initialize(RDevice *pDevice)
{
	if ( pDevice == NULL )
		return false;

	m_pDevice = pDevice;
	return true;
}

void RFont::ResetFinalizeFlag()
{
	_ASSERT(g_bFinal == true);
	g_bFinal = false;
}

//----------------------------------------------------------------------------------------------------
// Finalize()
/**
@brief		응용 프로그램에서 한번만 호출 되어야 하며, 정상적인 순서는 Destroy(소멸자)->Finalize 이다.
			g_bFinal 변수를 이용해서 Finalize 이전에 Destroy가 호출되는 것을 방지 한다.
@return		void
*/
//----------------------------------------------------------------------------------------------------
void RFont::Finalize()
{
	g_FontTexture.Destroy();

	RCHARINFO::Release();

	//by pok, AddFont
	for( vector<wstring>::iterator itr = m_vFontFile.begin(); itr != m_vFontFile.end(); ++itr)
		RemoveFontResourceW( itr->c_str() );

	for( vector<HANDLE>::iterator itr = m_vFontMemoryHandle.begin(); itr != m_vFontMemoryHandle.end(); ++itr)
		RemoveFontMemResourceEx( (*itr));

	g_bFinal = true;
}

//----------------------------------------------------------------------------------------------------
//  Create(RDevice *pDevice, const TCHAR* szFontName, int nHeight, bool bBold, bool bItalic, int nOutlineStyle, int nCacheSize, bool bAntiAlias, DWORD nColorArg1, DWORD nColorArg2)
/**
@brief		폰트 생성.
@param		pDevice			\n[in] Device Pointer.
@param		szFontName		\n[in] Null-terminated string.
@param		nHeight			\n[in] height of font. 최대 32.
@param		bBold			\n[in] bold attribure option.
@param		bItalic			\n[in] italic attribute option.
@param		nOutlineStyle	\n[in] 현재 지원 안함.
@param		nCacheSize		\n[in] 현재 지원 안함.
@param		bAntiAlias		\n[in] 현재 지원 안함.
@param		nColorArg1		\n[in] 현재 지원 안함.
@param		nColorArg2		\n[in] 현재 지원 안함.
@return		bool
*/
//----------------------------------------------------------------------------------------------------
bool RFont::Create(RDevice *pDevice, const wchar_t* szFontName, int nHeight, bool bBold/* =false */, bool bItalic/* =false */, int nOutlineWidth/* =0 */, float fOutlineOpacity/* =1.0f */, bool bAntiAlias, DWORD nColorArg1, DWORD nColorArg2)
{
	_ASSERT(pDevice);		// 사용 하지는 않는다.
	_ASSERT(m_pDevice);
	_ASSERT(nHeight <= CELL_SIZE);

	HWND hWnd = GetDesktopWindow();
	HDC hDC = GetDC(hWnd);
	SetMapMode(hDC, MM_ANISOTROPIC);

	// MulDiv(피승수, 승수, 나누는 수) // 여기서 72는 1인치당 픽셀수를 말하므로 폰트의 높이를 계산해 낸다
	// GetDeviceCaps(LOGPIXELSY) // 모니터의 논리적인 해상도를 알아내는 함수이다. 세로방향으로 로직컬 인치당 픽셀수
	// TODO : 줄간(m_nHeight)을 폰트 크기에서 픽셀로 변환 한다.
	//m_nHeight = MulDiv(nHeight, nDeviceCaps, 72);

	//if(m_nHeight > CELL_SIZE)
	//{
	//	int nValid = CELL_SIZE * 72 / nDeviceCaps;
	//	mlog("사용할수 있는 최대 Font 크기는 %d 입니다", nValid);
	//	assert(m_nHeight <= CELL_SIZE);

	//	return false;
	//}
	
	// 줄간을 폰트 크기 그대로 쓴다.
	//m_nHeight = nHeight;
	m_nFontHeight = nHeight + nOutlineWidth;
	m_nOutlineWidth = nOutlineWidth;
	m_fOutlineOpacity = fOutlineOpacity;

	//_ASSERT( m_nHeight <= CELL_SIZE );
	//_ASSERT( m_nHeight + m_nOutlineWidth <= CELL_SIZE );
	//_ASSERT( m_nHeight + m_nOutlineWidth * 2 <= CELL_SIZE );

	m_ColorArg1		= nColorArg1;
	m_ColorArg2		= nColorArg2;
	m_bAntiAlias	= bAntiAlias;
	// m_nSamplingMultiplier = nSamplingMultiplier;	// 한방향 샘플링 횟수, 2이면 2*2크기를 샘플링한다.

	// 높이가 양수일 경우 셀의 높이를 지정하고 음수일 경우 글자의 높이를 절대값으로 지정한다.
	m_hFont = CreateFontW(-nHeight, 0, 0, 0, bBold == true ? FW_BOLD : FW_NORMAL, bItalic == true ? TRUE : FALSE,
		FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, (bAntiAlias == true) ? ANTIALIASED_QUALITY : DEFAULT_QUALITY, DEFAULT_PITCH, szFontName);

	if(m_hFont == NULL)
		return false;

	ReleaseDC(hWnd, hDC);

	m_pFontTexture = &g_FontTexture;
	if(m_pFontTexture->GetTextureFont() == NULL) 
		m_pFontTexture->Create(pDevice);

	return true;
}

//----------------------------------------------------------------------------------------------------
//  Destroy()
/**
@brief		폰트 리소스 해제. 소멸자에서 호출한다.
@return		void
*/
//----------------------------------------------------------------------------------------------------
void RFont::Destroy()
{
	if ( g_bFinal == true )
	{
		assert(!"RFont::Destroy()는 RFont::Finalize() 이전에 호출 되어야 합니다.");
		return;
	}

	DeleteObject(m_hFont);
	m_hFont = NULL;

	while( m_CharInfoMap.size() )
	{
		RCHARINFO* pCharInfo = m_CharInfoMap.begin()->second;
		SAFE_DELETE(pCharInfo);
		m_CharInfoMap.erase(m_CharInfoMap.begin());
	}
}

//----------------------------------------------------------------------------------------------------
//  AddFontResourceFromFile(const char *szFontResName)
/**
@brief		시스템에 외부 폰트 추가.
@param		szFontResName		\n[in] Null-terminated string.
@return		bool
*/
//----------------------------------------------------------------------------------------------------
bool RFont::AddFontResourceFromFile(const wchar_t *szFontResName)
{
	for( UINT ui = 0; ui < m_vFontFile.size(); ++ui )
	{
		if( m_vFontFile[ui] == szFontResName )
			return true;
	}

	// 시스템으로부터 폰트가 추가되거나 삭제된다
	if ( AddFontResourceW(szFontResName) != NULL )
	{
		m_vFontFile.push_back(szFontResName);
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------------------------
//  AddFontResourceFromMemory(const _TCHAR *szFontResName)
/**
@brief		시스템에 메모리 리소스 폰트 추가.
@param		szFontResName		\n[in] Null-terminated string.
@return		bool
*/
//----------------------------------------------------------------------------------------------------
bool RFont::AddFontResourceFromMemory(const wchar_t *szFontResName)
{
	for( UINT ui = 0; ui < m_vFontMemory.size(); ++ui )
	{
		if( m_vFontMemory[ui] == szFontResName )
			return true;
	}

	// 시스템으로부터 폰트가 추가되거나 삭제된다	
	MFile mf;
	if ( mf.Open( MLocale::ConvUTF16ToAnsi(szFontResName).c_str()) == true)
	{
		unsigned int _size = mf.GetLength();
		wchar_t* pBuffer = new wchar_t[ _size + 1];
		memset( pBuffer, 0, _size + 1);
		mf.ReadAll( pBuffer, _size);
		mf.Close();

		DWORD dwCount;
		HANDLE hFont = AddFontMemResourceEx( pBuffer, _size, 0, &dwCount);
		delete [] pBuffer;

		if ( hFont != NULL)
		{
			m_vFontMemory.push_back( szFontResName);
			m_vFontMemoryHandle.push_back( hFont);
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------------------------------
//  GetTextSize(const _TCHAR *szText, int iLength)
/**
@brief		문자열에서 폰트의 크기, 스타일, 자간까지 고려하여 정확하게 문자열의 실제 길이를 계산 한다.
@param		szText		\n[in] Null-terminated string.
@param		iLength		\n[in] 사용 안함.
@return		SIZE
*/
//----------------------------------------------------------------------------------------------------
SIZE RFont::GetTextSize(const wchar_t *szText, int iLength /* = -1 */)
{
	SIZE kSize;

	if ( iLength == -1 )
		iLength = wcslen(szText);

	// 폰트를 선택해야 한다. 그래야 정확한 글자 길이가 나온다.
	HFONT hPrevFont = (HFONT)SelectObject(m_pFontTexture->GetDC(), m_hFont);
	GetTextExtentPoint32W(m_pFontTexture->GetDC(), szText, iLength, &kSize);
	SelectObject(m_pFontTexture->GetDC(), hPrevFont);

	return kSize;
}

//----------------------------------------------------------------------------------------------------
//  GetTextWidth(const TCHAR* szText, int nSize)
/**
@brief		문자열에서 폰트의 크기, 스타일, 자간까지 고려하여 정확하게 문자열의 실제 길이를 계산 한다.
@param		szText		\n[in] Null-terminated string.
@param		iLength		\n[in] 사용 안함.
@return		int
@sa			RFontTexture::GetCharWidth()
*/
//----------------------------------------------------------------------------------------------------
int RFont::GetTextWidth(const wchar_t* szText, int nSize /* =-1 */)
{
	if(nSize==0) return 0;
	if(szText==NULL) return 0;
	if(szText[0]==NULL) return 0;

	int nWidth = 0, nCount = 0;
	wchar_t szChar[4];
	const wchar_t* p = (const wchar_t*)szText;

	while(1) 
	{
		wchar_t* pp = wcsinc(p);

		if((pp-p)*2==sizeof(wchar_t))
		{
			szChar[0] = *p;
			szChar[1] = 0;
			nCount++;
		}
		else
		{
			szChar[0] = *p;
			szChar[1] = *(p+1);
			szChar[2] = 0;
			nCount+=2;
		}
		_ASSERT((pp-p) * 2 == sizeof(wchar_t)*2 || (pp-p) *2 == sizeof(wchar_t));

		WORD key = *(WORD*)szChar;

		RCHARINFOMAP::iterator i = m_CharInfoMap.find(key);
		RCHARINFO *pInfo = NULL;
		int nCurWidth;
		if(i == m_CharInfoMap.end()) 
		{
			// Font Texture 에서 전체 수용 가능한 글자의 개수를 넘으면 가장 오래 된것부터 삭제 한다.
			if( MAX_CHARCOUNT <= m_CharInfoMap.size() )
			{
				RCHARINFO* pCharInfo = m_CharInfoMap.begin()->second;
				SAFE_DELETE(pCharInfo);
				m_CharInfoMap.erase(m_CharInfoMap.begin());
			}

			nCurWidth = m_pFontTexture->GetCharWidth(m_hFont, szChar);
			pInfo = new RCHARINFO;
			pInfo->nWidth = nCurWidth;
			pInfo->nFontTextureIndex = -1;
			m_CharInfoMap.insert(RCHARINFOMAP::value_type(key,pInfo));
		}
		else
		{
			// GetCharWidth()함수는 GDI를 이용해서 Font의 Size를 구하는 함수인데 Cycle이 크다.
			// 이를 방지하기 위해 미리 구해둔 Size를 바로 돌려준다.
			nCurWidth=(*i->second).nWidth;
		}
		
		nWidth+=nCurWidth;

		p = pp;

		if(pp==NULL) break;
		if(*pp==0)	 break;
		if(nSize!=-1 && nCount>=nSize) break;
	}

	return nWidth;
}

//----------------------------------------------------------------------------------------------------
//  BeginFont(DWORD _flag)
/**
@brief		BeginFont
@return		bool
*/
//----------------------------------------------------------------------------------------------------
bool RFont::BeginFont(/*DWORD _flag*/)
{
	if ( m_bInFont == true )
	{
		assert(!"RFont::BeginFont() - already excute BeginFont func");
		return false;
	}

	// State
	m_pDevice->SetLighting(false);
	m_pDevice->SetBlending( RBLEND_SRCALPHA , RBLEND_INVSRCALPHA );
	m_pDevice->SetAlphaTestEnable(true);
	m_pDevice->SetAlphaRef( 0x08 );
	m_pDevice->SetAlphaFunc(RCMP_GREATEREQUAL);

	m_pDevice->SetTextureFilter(0, RTF_LINEAR );
// 	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
// 	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	m_pDevice->SetTextureStageState( 0, RTSS_COLOROP,   RTOP_MODULATE);
	m_pDevice->SetTextureStageState( 0, RTSS_COLORARG1, RTA_TEXTURE );
	m_pDevice->SetTextureStageState( 0, RTSS_COLORARG2, RTA_DIFFUSE );
	m_pDevice->SetTextureStageState( 0, RTSS_ALPHAOP,   RTOP_MODULATE );
	m_pDevice->SetTextureStageState( 0, RTSS_ALPHAARG1, RTA_TEXTURE );
	m_pDevice->SetTextureStageState( 0, RTSS_ALPHAARG2, RTA_DIFFUSE );
	m_pDevice->SetTextureStageState( 1, RTSS_COLOROP, RTOP_DISABLE);
	m_pDevice->SetTextureStageState( 1, RTSS_ALPHAOP, RTOP_DISABLE);

	/// SetOrthoCenterLH 를 사용하는 코드로 변경 (간단해짐)
	m_matOrgView = m_pDevice->GetTransform(RST_VIEW);
	m_matOrgProj = m_pDevice->GetTransform(RST_PROJECTION);

	m_pDevice->SetTransform(RST_WORLD, RMatrix::IDENTITY);
	m_pDevice->SetTransform(RST_VIEW, RMatrix::IDENTITY);

	// viewport 의 screen 좌표와 1:1 mapping 이 되도록 한다.
	RViewport vp = m_pDevice->GetViewport();
	RMatrix matScreenOrtho;
	matScreenOrtho.SetOrthoOffCenterLH(vp.x, vp.x + vp.nWidth, vp.y + vp.nHeight, vp.y, vp.fMinZ, vp.fMaxZ);

	m_pDevice->SetTransform(RST_PROJECTION, matScreenOrtho);

	g_nCntUsedID = g_FontTexture.GetLastUsedID();
	g_nFontCount = 0;

	m_bInFont = true;

	return true;
}

//----------------------------------------------------------------------------------------------------
//  EndFont()
/**
@brief		EndFont
@return		bool
*/
//----------------------------------------------------------------------------------------------------
bool RFont::EndFont()
{
	if ( m_bInFont == false )
	{
		assert(!"RFont::EndFont() - already execute EndFont func");
		return false;
	}

	FlushFont();

	m_pDevice->SetBlending(RBLEND_NONE);
	m_pDevice->SetAlphaTestEnable(false);

	m_pDevice->SetTexture(0,NULL);
	m_pDevice->SetTextureFilter(0, RTF_LINEAR );
// 	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
// 	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	// Transform 복구
	m_pDevice->SetTransform(RST_VIEW, m_matOrgView);
	m_pDevice->SetTransform(RST_PROJECTION, m_matOrgProj);

	g_nCntUsedID = 0;
	m_bInFont	 = false;

	return true;
}

//----------------------------------------------------------------------------------------------------
//  FlushFont()
/**
@brief		폰트 출력.
@return		void
*/
//----------------------------------------------------------------------------------------------------
void RFont::FlushFont()
{
	if(g_nFontCount)
	{
		m_pDevice->ShaderOff();
		m_pDevice->SetFvF(D3DFVF_FONT2DVERTEX);

		// Outline
		if(g_nOutlineWidth)
		{
			m_pDevice->SetTexture( 0,g_FontTexture.GetTextureOutline() );
			m_pDevice->DrawIndexedPrimitiveUP(RPT_TRIANGLELIST, 0, g_nFontCount*4, g_nFontCount*2,
				g_FontIndexBuffer, g_FontVertexBuffer_Outline, sizeof(FONT2DVERTEX), RFMT_INDEX16);

			REngine::GetResourceProfiler().AddPolygon(PPT_FONT, g_nFontCount*2 );
		}



		// Font
		m_pDevice->SetTexture( 0,g_FontTexture.GetTextureFont() );
		m_pDevice->DrawIndexedPrimitiveUP(RPT_TRIANGLELIST, 0, g_nFontCount*4, g_nFontCount*2,
			g_FontIndexBuffer, g_FontVertexBuffer_Font, sizeof(FONT2DVERTEX), RFMT_INDEX16);

		REngine::GetResourceProfiler().AddPolygon(PPT_FONT, g_nFontCount*2 );

		g_nFontCount = 0;
	}
}

//----------------------------------------------------------------------------------------------------
//  DrawText(float x, float y, const char* szText, int iLength, DWORD dwColor, float fScale)
/**
@brief		Font Texture에서 출력할 문자열에 대한 영역을 버텍스 버퍼에 담는다.
@param		x			\n[in] X Screen 좌표.
@param		y			\n[in] Y Screen 좌표.
@param		szText		\n[in] Null-terminated string.
@param		iLength		\n[in] 현재 사용 하지 않음.
@param		dwColorFont	\n[in] 출력할 문자열 색깔.
@param		dwColorOutline	\n[in] 출력할 문자 외곽선의 색깔.
@param		fScale		\n[in] 현재 사용 하지 않음.
@return		void
*/
//----------------------------------------------------------------------------------------------------
void RFont::DrawText_UNICODE(float x, float y, const wchar_t* szText, int iLength, DWORD dwColorFont/* =0xFFFFFFFF */, DWORD dwColorOutline /*=0xFF000000*/, float fScale/* =1.0f */)
{
	//TIMEPROFILE_THIS_FUNCTION;

	if ( m_bInFont == false )
	{
		assert(!"RFont::BeginFont() - BeginFont()가 되어있지 않습니다!");
		return;
	}

	if(szText==NULL)	return;
	if(szText[0]==NULL) return;
	if(szText[0]=='\n') return;

	const wchar_t* p = (const wchar_t*)szText;
	wchar_t szChar[4];

	int count = 0;
	while(1) 
	{
		// Advances a string pointer by one character.
		wchar_t* pp = wcsinc(p);

		if( (pp - p) * 2 == sizeof(wchar_t) )
		{
			szChar[0] = *p;
			szChar[1] = 0;
		}
		else
		{
			szChar[0] = *p;
			szChar[1] = *(p+1);
			szChar[2] = 0;
		}
		_ASSERT((pp-p) * 2 == sizeof(wchar_t)*2 || (pp-p) *2 == sizeof(wchar_t));

		WORD key = *(WORD*)szChar;

		RCHARINFOMAP::iterator i = m_CharInfoMap.find(key);
		
		RCHARINFO *pInfo = NULL;
		if( i == m_CharInfoMap.end() ) 
		{
			pInfo = new RCHARINFO;
			bool bRet = m_pFontTexture->MakeFontBitmap(m_hFont,pInfo,szChar,m_nOutlineWidth,m_fOutlineOpacity,m_ColorArg1,m_ColorArg2);
			if(bRet)
				m_CharInfoMap.insert(RCHARINFOMAP::value_type(key,pInfo));
			else 
				SAFE_DELETE(pInfo);
		}else
		{// 갱신할 필요가 있는지 검사
			pInfo = i->second;
			if( m_pFontTexture->IsNeedUpdate(pInfo->nFontTextureIndex,pInfo->nFontTextureID) )
				m_pFontTexture->MakeFontBitmap(m_hFont,pInfo,szChar,m_nOutlineWidth,m_fOutlineOpacity,m_ColorArg1,m_ColorArg2);
		}

		if(pInfo != NULL) 
		{
			// 버텍스를 만들어서 버퍼에 밀어넣는다
			static FONT2DVERTEX vertices[RFONT_VERTEXCOUNT];
			WORD indices[6] = { 3, 0, 2, 0, 1, 2 };
			/*		0 3
					1 2			*/
			int nWidth = min(CELL_SIZE,pInfo->nWidth);
			int nHeight= min(CELL_SIZE, pInfo->nHeight);
			int w = nWidth  * fScale;
			//int h = m_nHeight * fScale;
			int h = nHeight * fScale;

			if( x + w > m_pDevice->GetViewport().x && x < m_pDevice->GetViewport().x + m_pDevice->GetViewport().nWidth &&
				y + h > m_pDevice->GetViewport().y && y < m_pDevice->GetViewport().y + m_pDevice->GetViewport().nHeight ) 
			{
				int nCellX = pInfo->nFontTextureIndex % m_pFontTexture->GetCellCountX();
				int nCellY = pInfo->nFontTextureIndex / m_pFontTexture->GetCellCountX();

				for(int i=0;i<6;i++)
					indices[i]+=g_nFontCount*4;

				memcpy(g_FontIndexBuffer + g_nFontCount*6,indices,sizeof(indices));


				// Outline -----------------------------------------------------------------------
				if(m_nOutlineWidth)	// 외곽선의 두께가 있으면 외곽선 출력
				{
					int nh = h + m_nOutlineWidth * 2;
					int nw = w + m_nOutlineWidth * 2;

					vertices[0].p = RVector3(x,		y,		0);
					vertices[1].p = RVector3(x,		y+nh,	0);
					vertices[2].p = RVector3(x+nw,	y+nh,	0);
					vertices[3].p = RVector3(x+nw,	y,		0);

					float fMinX = (float)(0.5f+nCellX*CELL_SIZE) / (float)m_pFontTexture->GetWidth();
					float fMaxX = (float)(m_nOutlineWidth*2 + 0.5f+nCellX*CELL_SIZE+nWidth) / (float)m_pFontTexture->GetWidth();
					float fMinY = (float)(0.5f+nCellY*CELL_SIZE) / (float)m_pFontTexture->GetHeight();
					float fMaxY = (float)(m_nOutlineWidth*2 + 0.5f+nCellY*CELL_SIZE+nHeight) / (float)m_pFontTexture->GetHeight();

					vertices[0].tu = fMinX; vertices[0].tv = fMinY;
					vertices[1].tu = fMinX; vertices[1].tv = fMaxY;
					vertices[2].tu = fMaxX; vertices[2].tv = fMaxY;
					vertices[3].tu = fMaxX; vertices[3].tv = fMinY;
					
					vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = dwColorOutline;
					memcpy(g_FontVertexBuffer_Outline + g_nFontCount*4,vertices,sizeof(vertices));
				}

				// Font -----------------------------------------------------------------------
				int nh = h + m_nOutlineWidth * 2;
				int nw = w + m_nOutlineWidth * 2;

				vertices[0].p = RVector3(x,		y,		0);
				vertices[1].p = RVector3(x,		y+nh,	0);
				vertices[2].p = RVector3(x+nw,	y+nh,	0);
				vertices[3].p = RVector3(x+nw,	y,		0);

				float fMinX = (float)(0.5f+nCellX*CELL_SIZE) / (float)m_pFontTexture->GetWidth();
				float fMaxX = (float)(m_nOutlineWidth * 2 + 0.5f+nCellX*CELL_SIZE+nWidth) / (float)m_pFontTexture->GetWidth();
				float fMinY = (float)(0.5f+nCellY*CELL_SIZE) / (float)m_pFontTexture->GetHeight();
				float fMaxY = (float)(m_nOutlineWidth * 2 + 0.5f+nCellY*CELL_SIZE+nHeight) / (float)m_pFontTexture->GetHeight();

				vertices[0].tu = fMinX; vertices[0].tv = fMinY;
				vertices[1].tu = fMinX; vertices[1].tv = fMaxY;
				vertices[2].tu = fMaxX; vertices[2].tv = fMaxY;
				vertices[3].tu = fMaxX; vertices[3].tv = fMinY;

				// Font
				vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = dwColorFont;
				memcpy(g_FontVertexBuffer_Font  + g_nFontCount*4,vertices,sizeof(vertices));
				
				g_nFontCount++;
			}

			//if (0)//m_nOutlineWidth)
			//	x += min( (pInfo->nWidth*fScale+m_nOutlineWidth), CELL_SIZE );
			//else
				x += (pInfo->nWidth*fScale);
		}

		p = pp;

		if(g_nFontCount >= MAX_CHARCOUNT)
			FlushFont();

		if(pp==NULL) break;
		if(*pp==0)	 break;
		if(*pp=='\n') break;
		if(iLength==++count)	break;
	}

	// For Flush
	g_nOutlineWidth = m_nOutlineWidth;
}

// DrawText 가 다른 함수 이름이랑 겹쳐서 이걸로 사용해야 합니다
//void RFont::DrawText_UNICODE(float x, float y, const wchar_t* szText, int iLength, DWORD dwColorFont, DWORD dwColorOutline, float fScale)
//{
////	USES_CONVERSION_EX;
//	DrawText(x, y, szText, iLength, dwColorFont, dwColorOutline, fScale);
//}

//----------------------------------------------------------------------------------------------------
//  DrawTextIn3DSpace(const RMatrix& _refMat, const _TCHAR* szText, int iLength, DWORD dwColor)
/**
@brief		문자열을 3D 공간상에 출력한다.
			빌보드 기능을 구현 하고자 한다면 _refMat에 World * InvView 가 계산된 Matix를 넘겨주어야 한다.
@param		_refMat		\n[in] Transform Matrix.
@param		szText		\n[in] Null-terminated string.
@param		dwColor		\n[in] 출력할 문자열 색깔.
@return		bool
*/
//----------------------------------------------------------------------------------------------------
bool RFont::DrawTextIn3DSpace(const RMatrix& _refMat, const wchar_t* szText, int iLength, DWORD dwColor)
{
	if ( m_bInFont == false )
	{
		assert(!"RFont::BeginFont() - BeginFont()가 되어있지 않습니다!");
		return false;
	}

	// 원점이 중간이 되게
	SIZE nTextSize = GetTextSize(szText);
	RMatrix matTrans;
	matTrans.MakeIdentity();
	matTrans._41 = -nTextSize.cx * 0.5f;

	m_pDevice->SetTransform(RST_WORLD, matTrans * _refMat);
	m_pDevice->SetTransform(RST_PROJECTION, m_matOrgProj);

	m_pDevice->SetTextureFilter(0, RTF_BILINEAR_ANISO_LINEAR_FOR_FONT );

	DrawText_UNICODE(0, 0, szText, iLength, dwColor);

	// World Transform 이 다르므로 Batch Drawing 할 수 없다. 바로 출력.
	FlushFont();

	return true;
}

bool RFont::DrawTextIn3DSpace(const RVector& _pos, const wchar_t* szText, int iLength, DWORD dwColor)
{
	if ( m_bInFont == false )
	{
		assert(!"RFont::BeginFont() - BeginFont()가 되어있지 않습니다!");
		return false;
	}

	// 원점이 중간이 되게
	SIZE nTextSize = GetTextSize(szText);

	RMatrix matVP = m_matOrgView * m_matOrgProj;
	RMatrix matWorld = RMatrix::IDENTITY;
	RVector vecProjected;
	const RViewport &viewport = m_pDevice->GetViewport();

	GetScreenFromWorld(&vecProjected, &_pos, &viewport, &matWorld, &m_matOrgView, &m_matOrgProj);

	if ((vecProjected.z > 0.0f) && (vecProjected.z < 1.0f))
	{
		int x = vecProjected.x-nTextSize.cx * 0.5f;
		int y = vecProjected.y;
		DrawText_UNICODE(x+1, y+1, szText, iLength, 0xA0000000 );
		DrawText_UNICODE(x, y, szText, iLength, dwColor);
	}

	return true;
}


void RFont::Transform3DTo2D(const RVector& vPos, RVector& vOutCenter)
{
	return;
}

// bool RFont::DrawTextToTexture(LPDIRECT3DTEXTURE9 lpTexture, int x, int y, const _TCHAR *szText, int iLength, DWORD dwColor)
// {
// 	return false;
// }

bool RFont::DrawTextByTexture(RTexture* pTextureText, SIZE& nTextSize, const RVector& vPos, int nWidth, int nHeight)
{
	return false;
}

bool DumpFontTexture()
{
	return g_FontTexture.GetTextureFont()->SaveToFile("fonttex.bmp");
}

}