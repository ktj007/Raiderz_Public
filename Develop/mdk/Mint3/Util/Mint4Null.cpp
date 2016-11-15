#include "stdafx.h"

#include "Mint4Null.h"
#include "MWidget.h"
#include "Mint.h"

using namespace rs3;

void MDrawContextNull::SetState(MDrawState eState, MDrawEffect eEffect)
{
}

MDrawContextNull::~MDrawContextNull(void)
{
}

void MDrawContextNull::SetPixel(int x, int y, MCOLOR& color)
{

}

void MDrawContextNull::HLine(int x, int y, int len)
{
	Line(x, y, x+len, y);
}

void MDrawContextNull::VLine(int x, int y, int len)
{
	Line(x, y, x, y+len);
}

void MDrawContextNull::Line(int sx, int sy, int ex, int ey)
{

}
void MDrawContextNull::Rectangle(int x, int y, int cx, int cy)
{

}

void MDrawContextNull::FillRectangle(int x, int y, int cx, int cy)
{

}

void MDrawContextNull::Draw(MBitmap *pBitmap, int x, int y, int w, int h, int sx, int sy, int sw, int sh)
{

}

void MDrawContextNull::DrawRotate( MBitmap *pBitmap, MVector3& pos, MVector3& dir, int x, int y, int w, int h, int sx, int sy, int sw, int sh)
{

}

void MDrawContextNull::DrawRotate( MBitmap *pBitmap, int tx, int ty, int tw, int th, int rx, int ry, float ra, int sx, int sy, int sw, int sh)
{

}

void MDrawContextNull::DrawTiled(MBitmap *pBitmap, int x, int y, int w, int h, int sx, int sy, int sw, int sh)
{

}


void MDrawContextNull::DrawEx(int tx1, int ty1, int tx2, int ty2, int tx3, int ty3, int tx4, int ty4)
{

}

void MDrawContextNull::SetClipRect(MRECT& r)
{

}

MFontNull::MFontNull(void)
{

}

MFontNull::~MFontNull(void)
{
	Destroy();
}

bool MFontNull::Create(const char* szAliasName, const char* szFontName, int nHeight)
{
	MFont::Create(szAliasName);

	return true;
}

void MFontNull::Destroy(void)
{

}


MBitmapNull::MBitmapNull(void)
{

}

MBitmapNull::~MBitmapNull(void)
{
	Destroy();
}

bool MBitmapNull::Create(const char* szName, const char* szFileName)
{
	MBitmap::Create(szName);

	return true;
}


void MBitmapNull::Destroy(void)
{

}


MBitmap* Mint4Null::NewBitmap(const char* szName, const char* szFileName)
{
	// 같은 이름으로는 등록이 안되도록.
	MBitmap* pBitmap = MBitmapManager::Get(szName);
	if( pBitmap != NULL)
		return NULL;

	MBitmapNull* pNew = new MBitmapNull();
	bool bRet = pNew->Create(szName, szFileName);
	if(bRet==false){
		delete pNew;
		return NULL;
	}
	return pNew;
}


void MDrawContextNull::BeginDraw()
{

}

void MDrawContextNull::EndDraw()
{

}

