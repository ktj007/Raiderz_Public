#pragma once


namespace mint3
{

struct MRECT;
class MDrawContext;
class MBitmap;


enum MDRAWBITMAPSTYLE
{
	DRAWBITMAP_NORMAL		= 1,	// 기본적인 Draw 스타일
	DRAWBITMAP_STRETCH,				// 이미지 늘이기
};


void DrawBitmapByStyle( MDrawContext* pDC, MRECT& r, MBitmap* Bms, MRECT* rCoord, MDRAWBITMAPSTYLE byStyle = DRAWBITMAP_NORMAL, bool bTiled =false);

}

