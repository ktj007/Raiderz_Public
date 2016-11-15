#ifndef _MTYPES_H
#define _MTYPES_H

// 기본형 줄임
#ifdef _WIN32

#ifdef _MSC_VER
typedef signed __int64		int64;
typedef short				int16;
typedef unsigned __int64	uint64;
typedef unsigned short		uint16;
typedef unsigned char		uint8;

#ifndef __MCORE_CONFIG_H
typedef long				int32;
typedef char				int8;
typedef unsigned long		uint32;
#endif

#else
typedef long long			int64;
typedef __int32_t			int32;
typedef __int16_t			int16;
typedef __int8_t			int8;
typedef unsigned long long	uint64;
typedef __uint32_t			uint32;
typedef __uint16_t			uint16;
typedef __uint8_t			uint8;
typedef uint16				WORD;
typedef uint32				DWORD;
#endif
#endif

#ifndef DWORD
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
#endif

struct MPoint
{
	int x;
	int y;

	MPoint() : x(0), y(0) {}
	MPoint(int nx, int ny) : x(nx), y(ny) {}

    bool operator == ( const MPoint& pt ) const { return x == pt.x && y == pt.y; }
    bool operator != ( const MPoint& pt ) const { return x != pt.x || y != pt.y; }
	bool operator < ( const MPoint& pt ) const
	{
		return (x * 10 + y < pt.x * 10 + pt.y);
	}
};

struct MRect
{
    int left;
    int top;
    int right;
    int bottom;

	MRect() : left(0), top(0), right(0), bottom(0) {}
	MRect(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}

	int GetWidth() const { return right - left; }
	int GetHeight() const { return bottom - top; }
};

struct MRectF
{
	float left, top, right, bottom;
	MRectF()
		: left(0), top(0), right(0), bottom(0)
	{}
	MRectF(float l, float t, float r, float b)
		: left(l), top(t), right(r), bottom(b)
	{}

	float GetWidth() const { return right - left; }
	float GetHeight() const	{ return bottom - top; }
};


/// 크기
struct MSize{
	int w, h;
public:
	MSize(void){}
	MSize(int w, int h){
		MSize::w = w;
		MSize::h = h;
	}
};

#define DW_AS_FLT(DW) (*(FLOAT*)&(DW))
#define FLT_AS_DW(F) (*(DWORD*)&(F))
#define FLT_SIGN(F) ((FLT_AS_DW(F) & 0x80000000L))
#define ALMOST_ZERO(F) ((FLT_AS_DW(F) & 0x7f800000L)==0)

#include "MUID.h"
#include "MVector2.h"
#include "MVector3.h"
#include "MVector4.h"
#include "MSVector.h"
#include "MMatrix.h"
#include "MQuat.h"
#include "MPlane.h"
#include "MSphere.h"
#include "MBox.h"
#include "MOBox.h"
#include "MSVector.h"
#include "MCapsule.h"
#include "MLine.h"

typedef MVector2			vec2;
typedef MVector3			vec3;
typedef MVector4			vec4;
typedef MMatrix				mat4;
typedef MQuat				quat;
typedef MPlane				plane;

typedef MVector2			MPointF;
typedef MSVector			svec3;
typedef MSVector2			svec2;


enum MDIRECTION
{
	MDIR_FRONT = 0,
	MDIR_BACK,
	MDIR_LEFT,
	MDIR_RIGHT,
};

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

#endif	// _MTYPES_H