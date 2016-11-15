//----------------------------------------------------------------------------------------------------
// 파일명 : RFont.h
// 수  정 : 2008. 01. 18 - Font 픽셀 밀리는 버그 수정 및 1차 완료. -3DJun
// 설  명 : RealSpace3의 Font Class.
//			Mint3와 연동 되어 사용되며, Sample이나 Tool 프로젝트에서는 RTextSceneNode를 통해 직접 참조해 사용 한다.
//
// Copyright(c) 2008 Maiet Ent. Co.,Ltd. All Rights Reserved.
//----------------------------------------------------------------------------------------------------
#ifndef RFONT_H
#define RFONT_H

//#pragma warning(disable: 4786)

#include "RTypes.h"
//#include <d3dx9.h>
#include <map>
#include <list>
#include <string>
#include <wchar.h>
#include "MemPool.h"
//#include "RDeviceD3D.h"
#include "RVType.h"
#include "d3d9types.h"
#include "MLocale.h"

namespace rs3 {

/// 한글자의 정보를 갖는다
struct RCHARINFO : public CMemPoolSm<RCHARINFO> {		
	int nWidth;
	int nHeight;
	int nFontTextureID;
	int nFontTextureIndex;
};

typedef map<WORD,RCHARINFO*> RCHARINFOMAP;
struct RFONTTEXTURECELLINFO;
typedef list<RFONTTEXTURECELLINFO*> RFONTTEXTURECELLINFOLIST;

/// 커다란 텍스쳐인 RFontTexture안의 각 셀의 정보를 담고있는 구조체
struct RFONTTEXTURECELLINFO {
	int nID;
	int nIndex;
	RFONTTEXTURECELLINFOLIST::iterator itr;
};

//----------------------------------------------------------------------------------------------------
// RFontTexture
/**
* \brief 여러글자를 저장하고 있는 커다란 텍스쳐 한장
* \li  Header  : RFontTexture.h
* \li  Library : RealSpace3.lib
*/
//----------------------------------------------------------------------------------------------------
class RFontTexture {

	RDevice*	m_pd3dDevice;		///< Renderer Device

	//////////////////////////////////////////////////////////////////////////
	// DC
	HDC			m_hDC;				///< 텍스쳐에 들어갈 글자를 그리는 임시 DC & dibsection
	DWORD*		m_pBitmapBits_Font;		///< Texture의 Rect Pointer
	DWORD*		m_pBitmapBits_Outline;
	HBITMAP		m_hbmBitmap_Font;
	HBITMAP		m_hbmBitmap_Outline; 
	HBITMAP		m_hPrevBitmap;
	ULONG_PTR	m_pToken;
	
	//////////////////////////////////////////////////////////////////////////
	// Texture Info
	RTexture*	m_pTexture_Font;	///< CreateDIBSection으로 생성한 Texture Pointer.
									///< Rfont는 각각 하나의 Texture_Font와 Texture_Outline 을 가진다. ( Width x Height 셀로 이루어진 하나의 텍스쳐를 버퍼로 쓴다 )
	RTexture*	m_pTexture_Outline;	///< Outline Texture
	int			m_nWidth;			///< 텍스쳐의 너비
	int			m_nHeight;			///< 텍스쳐의 폭
	int			m_nX,m_nY;			///< 셀의 개수(너비, 폭)
	int			m_nCell;			///< 셀의 개수 = nX * nY

	//////////////////////////////////////////////////////////////////////////
	//
	int			m_LastUsedID;		///< 현재 할당된 글자의 개수
	RFONTTEXTURECELLINFO*	 m_CellInfo;		///< 커다란 텍스쳐인 RFontTexture안의 각 셀의 정보를 담고있는 구조체
	RFONTTEXTURECELLINFOLIST m_PriorityQueue;	///< 셀 정보의 리스트 가장 최근에 쓰인 것이 뒤쪽으로 정렬 한다.
												///< 삭제시에 사장 앞쪽의 최근 접근성이 적은 셀부터 삭제한다.

public:
	RFontTexture();
	~RFontTexture();

	//////////////////////////////////////////////////////////////////////////
	// Create
	bool	Create(RDevice *pDevice);

	//////////////////////////////////////////////////////////////////////////
	// Destroy
	void	Destroy();

	//////////////////////////////////////////////////////////////////////////
	// Update
	bool	IsNeedUpdate(int nIndex, int nID);		///< 갱신되어야 하는지 검사
	bool	MakeFontBitmap(HFONT hFont, RCHARINFO *pInfo, const wchar_t* szText, int nOutlineWidth, float fOutlineOpacity, DWORD nColorArg1, DWORD nColorArg2);

	void	Outline_GDIPlusPath(HFONT hFont, const wchar_t* szText, int nOutlineWidth);
	void	Outline_GDIOverDraw(const wchar_t* szText, int nOutlineWidth);

	//////////////////////////////////////////////////////////////////////////
	// Render
	bool	UploadTexture(RTexture* pTexture, RCHARINFO *pCharInfo, DWORD* pBitmapBits, int nIndex, int w,int h);
	void	BlitRect(BYTE* pDest, INT Pitch, int x1, int y1, int x2, int y2, int sw, int sh, DWORD* pSrcBitmapBits);

	//////////////////////////////////////////////////////////////////////////
	// Get / Set
	int		GetCharWidth(HFONT hFont, const wchar_t* szChar);
	int		GetCharABCWidth(const wchar_t* szText, SIZE size);

	HDC 	GetDC() { return m_hDC; }
	int 	GetWidth() { return m_nWidth; }
	int 	GetHeight() { return m_nHeight; }
	int 	GetCellCountX() { return m_nX; }
	int 	GetCellCountY() { return m_nY; }
	int 	GetLastUsedID() { return m_LastUsedID; }

	RTexture* GetTextureFont() { return m_pTexture_Font; }
	RTexture* GetTextureOutline() { return m_pTexture_Outline; }
};

//----------------------------------------------------------------------------------------------------
// RFont
/**
* \brief RealSpace3 Font
* \li  Header  : RFont.h
* \li  Library : RealSpace3.lib
*/
//----------------------------------------------------------------------------------------------------
class RS_API RFont {
	static void ResetFinalizeFlag();

protected:
	static RDevice*		m_pDevice;		///< Renderer Device

	//////////////////////////////////////////////////////////////////////////
	// Font
	HFONT				m_hFont;			///< Font Handle
	RCHARINFOMAP		m_CharInfoMap;		///< 문자열에 대한 맵리스트 // 처음 한번은 등록하고 다음부터는 꺼내 쓴다.
	RFontTexture*		m_pFontTexture;		///< 폰트 텍스쳐

	//////////////////////////////////////////////////////////////////////////
	// Begin, End
	static RMatrix		m_matOrgView;		///< VIEW Transform을 BeginFont에서 저장하고 EndFont에서 복구 해준다.
	static RMatrix		m_matOrgProj;		///< PROJECT Transform을 BeginFont에서 저장하고 EndFont에서 복구 해준다.
	static	bool		m_bInFont;			///< beginfont endfont 사이에 있는지.

	//////////////////////////////////////////////////////////////////////////
	// 
	//int					m_nHeight;			///< 줄간 : 다음줄과의 간격. 
	int					m_nFontHeight;		///< 생성시 폰트 크기
	int					m_nOutlineWidth;	///< 외곽선 두께
	float				m_fOutlineOpacity;	///< 외곽선 투명도
	bool				m_bAntiAlias;		///< 안티 앨리어싱
	DWORD				m_ColorArg1;		
	DWORD				m_ColorArg2;

	
public:
	RFont(void);
	virtual ~RFont(void);

	//////////////////////////////////////////////////////////////////////////
	// Static
	static bool Initialize(RDevice *pDevice);
	static void Finalize();

	static bool BeginFont(/*DWORD _flag = NULL*/);
	static bool EndFont();
	static void	FlushFont();
	
	static bool AddFontResourceFromFile(const wchar_t* szFontResName);	///< 외부 폰트 파일을 추가
	static bool AddFontResourceFromMemory(const wchar_t* szFontResName);	///< 외부 폰트 파일을 추가

	//////////////////////////////////////////////////////////////////////////
	// Create
	bool		Create(RDevice *pDevice, const wchar_t* szFontName, int nHeight, bool bBold=false, bool bItalic=false, int nOutlineWidth=0, float fOutlineOpacity=1.0f, bool bAntiAlias=false, DWORD nColorArg1=0, DWORD nColorArg2=0);
	//////////////////////////////////////////////////////////////////////////
	// Destroy
	void		Destroy(void);

	//////////////////////////////////////////////////////////////////////////
	// Update
	HRESULT		OnLostDevice();
	HRESULT		OnResetDevice();

	//////////////////////////////////////////////////////////////////////////
	// Render

	//void		DrawText(float x, float y, const wchar_t* szText,  int iLength=-1, DWORD dwColorFont=0xFFFFFFFF, DWORD dwColorOutline=0xFF000000, float fScale=1.0f);	
	// DrawText 가 다른 함수 이름이랑 겹쳐서 외부에서는 이것으로 사용해야 합니다
	void		DrawText_UNICODE(float x, float y, const wchar_t* szText, int iLength=-1, DWORD dwColorFont=0xFFFFFFFF, DWORD dwColorOutline=0xFF000000, float fScale=1.0f);	
	bool		DrawTextIn3DSpace(const RMatrix& _refMat, const wchar_t* szText, int iLength = -1, DWORD dwColor = D3DCOLOR_XRGB(255,255,255));
	bool		DrawTextIn3DSpace(const RVector& _pos, const wchar_t* szText, int iLength = -1, DWORD dwColor = D3DCOLOR_XRGB(255,255,255));

	//////////////////////////////////////////////////////////////////////////
	// Get / Set
	int			GetHeight(void){ return m_nFontHeight; }
	//int			GetTexHeight(void) { return m_nHeight; }
	int			GetTextWidth(const wchar_t* szText, int nSize=-1);
	SIZE		GetTextSize(const wchar_t *szText, int iLength = -1);

	//////////////////////////////////////////////////////////////////////////
	// 사용 안함
	void		Transform3DTo2D(const RVector& vPos, RVector& vOutCenter);
// 	bool		DrawTextToTexture(LPDIRECT3DTEXTURE9 lpTexture, int x, int y, const _TCHAR *szText, int iLength = -1, DWORD dwColor = D3DCOLOR_XRGB(255,255,255));
	bool		DrawTextByTexture(RTexture* pTextureText, SIZE& nTextSize, const RVector& vPos, int nWidth, int nHeight);

};

//////////////////////////////////////////////////////////////////////////
// Debug
bool DumpFontTexture();

}

#endif
