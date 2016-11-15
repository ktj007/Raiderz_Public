#ifndef MINT4NULL_H
#define MINT4NULL_H

#include "Mint.h"
#include "MDrawContext.h"

using namespace mint3;

class MDrawContextNull : public MDrawContext{
protected:
	bool					m_bInDraw;

public:
	MDrawContextNull() {}
	virtual ~MDrawContextNull(void);

	// Basic Drawing Functions
	virtual void BeginDraw();	// 기본 drawing function 을 위한 스테이트 설정
	virtual void EndDraw();

	virtual void SetPixel(int x, int y, MCOLOR& color);
	virtual void HLine(int x, int y, int len);
	virtual void VLine(int x, int y, int len);
	virtual void Line(int sx, int sy, int ex, int ey);
	virtual void Rectangle(int x, int y, int cx, int cy);
	virtual void FillRectangle(int x, int y, int cx, int cy);

	virtual void DrawEx(int tx1, int ty1, int tx2, int ty2, int tx3, int ty3, int tx4, int ty4);

	virtual void SetClipRect(MRECT& r);

	virtual void SetState(MDrawState eState, MDrawEffect effect = MDE_NORMAL);

	// 폰트를 강제로 flush한다
	virtual void FlushFont()	{ }

private:
	virtual void Draw(MBitmap *pBitmap, int x, int y, int w, int h, int sx, int sy, int sw, int sh);
	virtual void DrawRotate( MBitmap *pBitmap, MVector3& pos, MVector3& dir, int x, int y, int w, int h, int sx, int sy, int sw, int sh);
	virtual void DrawRotate( MBitmap *pBitmap, int tx, int ty, int tw, int th, int rx, int ry, float ra, int sx, int sy, int sw, int sh);
	virtual void DrawTiled(MBitmap *pBitmap, int x, int y, int w, int h, int sx, int sy, int sw, int sh);
};

class MFontNull : public MFont
{
public:
	bool		m_bAntiAlias;
	float		m_fScale;
public:
	MFontNull(void);
	virtual ~MFontNull(void);

	virtual bool Create(const char* szAliasName, const char* szFontName, int nHeight);
	virtual void Destroy(void);

	virtual int DrawText(int x, int y, const char* szText, int nLength = -1, DWORD dwColor = 0xffffffff) { return 0; }
	virtual int GetHeight(void) { return 1; }
	virtual int GetWidth(const char* szText, int nSize=-1) { return 1; }
};



class MBitmapNull : public MBitmap{
public:
	MBitmapNull(void);
	virtual ~MBitmapNull(void);

	virtual bool Create(const char* szAliasName, const char* szFileName);
	virtual void Destroy(void);

	virtual void SetTexCoord(float x1, float y1, float x2, float y2) {}
	virtual int GetWidth(void) { return 0; }
	virtual int GetHeight(void) { return 0; }
};



class Mint4Null : public Mint {
private:
	
public:
	virtual MBitmap* NewBitmap(const char* szName, const char* szFileName) override;
	virtual MFont* NewFont(const char* pszAliasName, const char* pszFontName, const char* pszFileName, int nHeight, bool bOutline, float fOutlineOpacity =1.0f, bool bAntiAlias =false) override 
	{
		return NULL;
	}
	virtual MCursor* NewCursor(const char* pszAliasName) override { return NULL; };

};

#endif