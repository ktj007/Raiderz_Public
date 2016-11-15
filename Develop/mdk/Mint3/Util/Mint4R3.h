#ifndef MINT4R3_H
#define MINT4R3_H

#include "Mint.h"
#include "MDrawContext.h"
#include "RFont.h"
#include "RTypes.h"
#include "RDevice.h"
#include <D3dx9.h>

using namespace mint3;

class MDrawContextR3 : public MDrawContext{
protected:
	static rs3::RDevice*	m_pDevice;
	static MDrawState		m_eCurrentState;
	bool					m_bInDraw;

public:
	MDrawContextR3(rs3::RDevice* pDevice);
	//MDrawContextR3(void);
	virtual ~MDrawContextR3(void);
	static rs3::RDevice* GetDevice(){ return m_pDevice; }

	// Basic Drawing Functions
	virtual void BeginDraw();	// 기본 drawing function 을 위한 스테이트 설정
	virtual void EndDraw();

	virtual void SetPixel(int x, int y, MCOLOR& color);
	virtual void HLine(int x, int y, int len);
	virtual void VLine(int x, int y, int len);
	virtual void Line(int sx, int sy, int ex, int ey);
	virtual void Rectangle(int x, int y, int cx, int cy);
	virtual void FillRectangle(int x, int y, int cx, int cy);

	// Bitmap Drawing
	virtual void DrawEx(int tx1, int ty1, int tx2, int ty2, int tx3, int ty3, int tx4, int ty4);

	virtual void SetClipRect(MRECT& r);

	// MDrawContext, MBitmap, MFont의 State를 변경한다.
	virtual void SetState(MDrawState eState, MDrawEffect effect = MDE_NORMAL);

	// 폰트를 강제로 flush한다
	virtual void FlushFont();

private:
	virtual void Draw(MBitmap *pBitmap, int x, int y, int w, int h, int sx, int sy, int sw, int sh);
	virtual void DrawRotate( MBitmap *pBitmap, MVector3& pos, MVector3& dir, int x, int y, int w, int h, int sx, int sy, int sw, int sh);
	virtual void DrawRotate( MBitmap *pBitmap, int tx, int ty, int tw, int th, int rx, int ry, float ra, int sx, int sy, int sw, int sh);
	virtual void DrawTiled(MBitmap *pBitmap, int x, int y, int w, int h, int sx, int sy, int sw, int sh);
};

class MFontR3 : public MFont{
public:
	bool		m_bAntiAlias;
	float		m_fScale;
	rs3::RFont	m_Font;
public:
	MFontR3(void);
	virtual ~MFontR3(void);

	virtual bool Create(rs3::RDevice* pDevice, const char* szAliasName, const char* szFontName, int nHeight, float fScale=1.0f, bool bBold=false, bool bItalic=false, bool bOutline=false, float fOutlineOpacity =1.0f, int nCacheSize=-1, bool bAntiAlias = false);
	virtual void Destroy(void);
	virtual int	DrawText(int x, int y, const char* szText, int nLength = -1, DWORD dwColor = 0xffffffff);

	virtual int GetHeight(void);
	virtual int GetWidth(const char* szText, int nSize=-1);
};



class MBitmapR3 : public MBitmap{
private:
	rs3::RDevice*		m_pDevice;
	rs3::RTexture*			m_hTexture;
	int						m_nWidth;
	int						m_nHeight;
	static	DWORD			m_dwStateBlock;
	float					m_fTexCoord[4];
protected:
	void CalcSizeFromTexCoord();
	void BeginState(MDrawEffect effect);
	void EndState(void);
public:
	MBitmapR3(void);
	virtual ~MBitmapR3(void);

	virtual bool Create(const char* szAliasName, rs3::RDevice* pDevice, const char* szFileName);
	virtual void Destroy(void);

	virtual void SetTexCoord(float x1, float y1, float x2, float y2);
	virtual int GetWidth(void);
	virtual int GetHeight(void);

	void CheckDrawMode(float* fuv);

	void Draw(float x, float y, float w, float h, float sx, float sy, float sw, float sh, 
		DWORD dwColor, MDrawEffect effect=MDE_NORMAL);
	void DrawRotate( MVector3& pos, MVector3& dir, float x, float y, float w, float h,
		float sx, float sy, float sw, float sh, DWORD dwColor, MDrawEffect effect=MDE_NORMAL );
	virtual void DrawRotate( float tx, float ty, float tw, float th, float rx, float ry, float ra,
								float sx, float sy, float sw, float sh, DWORD dwColor, MDrawEffect effect=MDE_NORMAL);
	void DrawTiled(float x, float y, float w, float h, float sx, float sy, float sw, float sh, 
		DWORD dwColor, MDrawEffect effect=MDE_NORMAL);
	void DrawEx(float tx1, float ty1, float tx2, float ty2, 
		        float tx3, float ty3, float tx4, float ty4, DWORD dwColor, MDrawEffect effect=MDE_NORMAL);

	void OnLostDevice(void);
	void OnResetDevice(void);
	
	rs3::RTexture* GetTexture() { return m_hTexture; }
};

class Mint4R3 : public Mint {
public:
	virtual MBitmap*	NewBitmap(const char* szName, const char* szFileName);
	virtual MFont*		NewFont(const char* pszAliasName, const char* pszFontName, const char* pszFileName, int nHeight, bool bOutline, float fOutlineOpacity =1.0f, bool bAntiAlias =false);
	virtual MCursor*	NewCursor(const char* pszAliasName);
};

#endif