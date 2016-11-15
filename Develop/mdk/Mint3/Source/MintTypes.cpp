#include "stdafx.h"
#include "MintTypes.h"
#include "MWidget.h"
#include "Mint.h"

namespace mint3 {

void MPOINT::Scale(float x, float y)
{
	MPOINT::x = int(MPOINT::x*x);
	MPOINT::y = int(MPOINT::y*y);
}

void MPOINT::ScaleRes(void)
{
	x = MPOINT::x*MGetWorkspaceWidth()/640;
	y = MPOINT::y*MGetWorkspaceHeight()/480;
}

void MPOINT::TranslateRes(void)
{
	int nDiffX = x - 320;
	int nDiffY = y - 240;

	x = nDiffX + MGetWorkspaceWidth()/2;
	y = nDiffY + MGetWorkspaceHeight()/2;
}

void MRECT::ScalePos(float x, float y)
{
	MRECT::x = int(MRECT::x*x);
	MRECT::y = int(MRECT::y*y);
}

void MRECT::ScaleArea(float x, float y)
{
	MRECT::x = int(MRECT::x*x);
	MRECT::y = int(MRECT::y*y);
	MRECT::w = int(MRECT::w*x);
	MRECT::h = int(MRECT::h*y);
}

void MRECT::ScalePosRes(void)
{
	float x = MGetWorkspaceWidth()/(float)640;
	float y = MGetWorkspaceHeight()/(float)480;
	ScalePos(x, y);
}

void MRECT::ScaleAreaRes(void)
{
	float x = MGetWorkspaceWidth()/(float)640;
	float y = MGetWorkspaceHeight()/(float)480;
	ScaleArea(x, y);
}

void MRECT::TranslateRes(void)
{
	int nDiffX = x - 320;
	int nDiffY = y - 240;

	x = nDiffX + MGetWorkspaceWidth()/2;
	y = nDiffY + MGetWorkspaceHeight()/2;
}

void MRECT::EnLarge(int w)
{
	x -= w;
	y -= w;
	w += (w*2);
	h += (w*2);
}

void MRECT::Offset(int x, int y)
{
	MRECT::x += x;
	MRECT::y += y;
}

bool MRECT::Intersect(MRECT* pIntersect, const MRECT& r)
{
	_ASSERT( pIntersect != NULL);
/*
	if(x>r.x){
		pIntersect->x = x;
		pIntersect->w = min(r.x+r.w, x+w) - x;
	}
	else{
		pIntersect->x = r.x;
		pIntersect->w = min(r.x+r.w, x+w) - r.x;
	}
	if(y>r.y){
		pIntersect->y = y;
		pIntersect->h = min(r.y+r.h, y+h) - y;
	}
	else{
		pIntersect->y = r.y;
		pIntersect->h = min(r.y+r.h, y+h) - r.y;
	}

	if(pIntersect->w<0) return false;
	if(pIntersect->h<0) return false;

	return true;
*/

	bool bIntersect = true;


	int rx1 = max( x,  r.x);
	int rx2 = min( x + w,  r.x + r.w);

	if ( rx1 < rx2)
	{
		pIntersect->x = rx1;
		pIntersect->w = rx2 - rx1;
	}
	else
	{
		pIntersect->x = x;
		pIntersect->w = 0;

		bIntersect = false;
	}


	int ry1 = max( y,  r.y);
	int ry2 = min( y + h,  r.y + r.h);

	if ( ry1 < ry2)
	{
		pIntersect->y = ry1;
		pIntersect->h = ry2 - ry1;
	}
	else
	{
		pIntersect->y = y;
		pIntersect->h = 0;

		bIntersect = false;
	}


	return bIntersect;
}

} // namespace mint3