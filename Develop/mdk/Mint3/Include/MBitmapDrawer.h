#pragma once

#include "MintTypes.h"

namespace mint3 {

#define FRAME_OUTLINE_WIDTH	1
#define FRAME_WIDTH		5
#define FRAME_INLINE_WIDTH 1

// 비트맵 그리기 스타일
enum MDRAWBITMAPSTYLE
{
	DRAWBITMAP_NORMAL		= 1,	// 기본적인 Draw 스타일
	DRAWBITMAP_STRETCH,				// 이미지 늘이기
	DRAWBITMAP_TILED				// 타일 형식으로 그리기
};

class MDrawContext;
class MBitmap;

void HLineBitmap(MDrawContext* pDC, int x, int y, int w, MBitmap* pBm, bool bStretch, float fScale = 1.f);

void DrawBitmapFrame2(MDrawContext* pDC, MRECT& r, MRECT& cliprect,MBitmap* Bms[9]);
void DrawBitmapFrame9(MDrawContext* pDC, MRECT& r, MBitmap* Bms[9],bool bStretch = true, float fScale = 1.f);

void DrawBitmapFrameH3(MDrawContext* pDC, MRECT& r, MBitmap* Bms[3]);	// Draw Horizontal 3 Bitmaps
void DrawBitmapFrameV3(MDrawContext* pDC, MRECT& r, MBitmap* Bms[3]);	// Draw Vertical 3 Bitmaps
void DrawBitmapFrameCustom1(MDrawContext* pDC, MRECT& r, MBitmap* Bms[9], bool bStretch = true );
void DrawBitmapFrameCustom2(MDrawContext* pDC, MRECT& r, MBitmap* Bms[9], MCOLOR bgColor, bool bStretch = true );
void DrawBitmapButtonCustom1( MDrawContext* pDC, MRECT& r, MBitmap* Bms[9], bool bDown = false, bool bStretch = true );

// madduck [4/5/2006]
void DrawBitmapByStyle(MDrawContext* pDC, MRECT& r, MBitmap* Bms, MRECT* rCoord, MDRAWBITMAPSTYLE byStyle = DRAWBITMAP_NORMAL, bool bTiled =false);

bool IsNull(MBitmap** ps, int nCount);

/// 이미지 그리기 스타일 함수
void BitmapStretch(int& nOutW, int& nOutH, const MRECT& r, const MRECT* rCoord);

// Safety Call
#define GETWIDTH(_Image)	((_Image==NULL)?0:_Image->GetWidth())
#define GETHEIGHT(_Image)	((_Image==NULL)?0:_Image->GetHeight())

} // namespace mint3
