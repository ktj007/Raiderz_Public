#pragma once

#include "MMath.h"
#include <Windows.h>

struct RectF;

namespace rs3 {

class RDevice;

bool IsIntersect( const MBox &aabb, const MVector3 &v0, const MVector3 &v1, const MVector3 &v2);

float GetDistanceBetweenLineSegment(const MVector3 &a,const MVector3 &aa,const MVector3 &c,const MVector3 &cc,MVector3 *ap,MVector3 *cp);
// (P1, P2) - (P3, P4)
bool IntersectLine(MVector2 *pOut, const MVector2 &P1, const MVector2 &P2, const MVector2 &P3, const MVector2 &P4);

// Path와 Filename을 분리한다.
RS_API void SplitPath(const char *pFullname, char *path, char *file);

/** 
@brief 2D 좌표를 이용하여, 현재 카메라 대한 Direction(정규화된 반직선)을 구하는 함수
@param sx	커서의 x좌표
@param sy	커서의 y좌표
@param rt	2D 스크린의 영역
@param View	현재 카메라의 ViewMatrix ( DirectX )
@param Projection	현재 카메라의 ProjectionMatrix ( DirectX )
@param dir	반직선 값을 얻어올 벡터 변수

@return		정상적으로 인자가 들어왔으면 true를 리턴, 그렇지 않으면 false를 리턴
*/
RS_API bool RGetScreenLine(int sx,int sy, RECT &rt, const MMatrix &View, const MMatrix &Projection, MVector3 *dir);

// DWORD <-> RVector
inline void DWColorToFloat3(MVector3 *pOut, DWORD dwColor)
{
	DWORD dwR = ( dwColor >> 16 ) & 0xff;
	DWORD dwG = ( dwColor >> 8 ) & 0xff;
	DWORD dwB = ( dwColor ) & 0xff;

	if ( pOut )
	{
		pOut->x = (float)(dwR/255.0f);
		pOut->y = (float)(dwG/255.0f);
		pOut->z = (float)(dwB/255.0f);
	}
}

#define DWORDCOLOR_ARGB(a,r,g,b) \
	((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

inline DWORD RVector4ToDWORD(const RVector4 &v)
{
	return DWORDCOLOR_ARGB((int)(v.w*255),(int)(v.x*255),(int)(v.y*255),(int)(v.z*255));
}

inline DWORD FloatToDWORD(float r, float g, float b, float a = 1.f)
{
	return DWORDCOLOR_ARGB((int)(a*255),(int)(r*255),(int)(g*255),(int)(b*255));
}

inline void DWORD2RGBA(DWORD dwColor, float &r, float &g, float &b, float &a)
{
	//((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
	DWORD dwR, dwG, dwB, dwA;

	dwA = ( dwColor >> 24 ) & 0xff;
	dwR = ( dwColor >> 16 ) & 0xff;
	dwG = ( dwColor >> 8 )  & 0xff;
	dwB = ( dwColor ) & 0xff;

	r = dwR/255.0f;
	g = dwG/255.0f;
	b = dwB/255.0f;
	a = dwA/255.0f;

	if ( r>1.0f)	r = 1.0f;
	if ( r<0.0f)	r = 0.0f;

	if ( g>1.0f)	g = 1.0f;
	if ( g<0.0f)	g = 0.0f;

	if ( b>1.0f)	b = 1.0f;
	if ( b<0.0f)	b = 0.0f;

	return;
}

/// 0 ~ 1로 잘라냄
inline float Saturate(float x) { return max( 0, min( 1, (x) ) ); }

}
