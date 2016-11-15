#pragma once

//#include <d3dx9.h>
#include "RTypes.h"

struct RS_API RVt_pos
{
	rs3::RVector vPos;
	static const DWORD FVF;
};

struct RS_API RVt_pos_clr
{
	rs3::RVector vPos;
	DWORD dwColor;
	static const DWORD FVF;

	RVt_pos_clr()
	{}
	RVt_pos_clr(const rs3::RVector &position, DWORD color)
		: vPos(position)
		, dwColor(color)
	{}
};

struct RS_API RVt_pos_tex1
{
	rs3::RVector vPos;
	float u, v;
	static const DWORD FVF;

	RVt_pos_tex1()	{}
	RVt_pos_tex1(float x, float y, float z, float tu, float tv)
		: vPos(x, y, z), u(tu), v(tv) {}
};

//particle
struct RS_API RVt_pos_point_clr
{
	rs3::RVector vPos;
	float pointsize;
	DWORD dwColor;
	static const DWORD FVF;
};

struct RS_API RVt_pos_clr_tex1
{
	rs3::RVector vPos;
	DWORD dwColor;
	float u, v;

	static const DWORD FVF;
};

struct RS_API RVt_pos_clr_tex2
{
	rs3::RVector vPos;
	DWORD dwColor;
	float u, v;
	float s, t;
	static const DWORD FVF;
};

struct RS_API RVt_pos_tex_rhw
{
	rs3::RVector4 vPos;
	float u, v;

	static const DWORD FVF;
};

struct RS_API RVt_pos_normal_tex1
{
	rs3::RVector vPos;
	rs3::RVector vNormal;
	float u, v;
	static const DWORD FVF;
};

struct RS_API RVt_pos4_normal_clr_tex1
{
	rs3::RVector4 vPos;
	rs3::RVector vNormal;
	DWORD dwColor;
	float u, v;

	static const DWORD FVF;
};

struct RVt_particle
{
	rs3::RVector vPos;
	DWORD dwColor;
	float u, v;
	float s, t;
	float blend;
	static const DWORD FVF;
};
typedef RVt_particle PVERTEXFORMAT;

#ifdef _DEBUG
struct RS_API RVt_cloth_pos
{
	rs3::RVector3	vPos;
	rs3::RVector2	vTexCoord;
	rs3::RVector2	vPosCoord;
	static const DWORD FVF;
};

// RVt_cloth_normal rendering
struct RS_API RVt_cloth_normal
{
	rs3::RVector3	vPos;
	rs3::RVector2	vTexCoord;
	float			fLength;
	static const DWORD FVF;
};

// Anchor points
struct RS_API RVt_cloth_anchor
{
	rs3::RVector3	vPos;
	rs3::RVector2	vXY;
	rs3::RVector3	vValue;
	static const DWORD FVF;
};
#else
struct RS_API RVt_cloth_pos
{
	rs3::RVector2	vTexCoord;
	rs3::RVector2	vPosCoord;
	static const DWORD FVF;
};

struct RS_API RVt_cloth_normal
{
	rs3::RVector2	vTexCoord;
	float			fLength;
	static const DWORD FVF;
};

// Anchor points
struct RS_API RVt_cloth_anchor
{
	rs3::RVector2	vXY;
	rs3::RVector3	vValue;
	static const DWORD FVF;
};
#endif

inline DWORD BitFlag(unsigned int n)
{
	return ( 0x01 << n );
}

void _getpath(const char *szFullName, char *path);
void _getname(const char *szFullName, char *name);

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)	{ if (p)	{ delete p; p = NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)	{ if (p)	{ delete [] p; p = NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)	{ if (p)	{ (p)->Release(); p = NULL; } }
#endif