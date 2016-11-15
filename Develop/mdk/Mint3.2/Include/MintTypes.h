#pragma once

#include <string>
using namespace std;


namespace mint3 {


/// 위치
struct MPOINT{
	int x, y;
public:
	MPOINT(void){}
	MPOINT(int x, int y){ MPOINT::x = x, MPOINT::y = y; }

	void Scale(float x, float y);
	void ScaleRes(void);				///< 648*480기준값을 현재 해상도에 따른 스케일링
	void TranslateRes(void);			///< 648*480 중심점과의 거리만큼 현재 해상도에 맞게 이동

	bool operator==(const MPOINT& other) const { return other.x==x && other.y==y; }
	bool operator!=(const MPOINT& other) const { return other.x!=x || other.y!=y; }
};

/// 영역
struct MRECT{
	int	x, y;	///< 위치
	int w, h;	///< 크기
public:
	MRECT(void){}
	MRECT(int x, int y, int w, int h){
		MRECT::x = x, MRECT::y = y;
		MRECT::w = w, MRECT::h = h;
	}
	bool InPoint(MPOINT& p){
		if(p.x>=x && p.x<=x+w && p.y>=y && p.y<=y+h) return true;
		return false;
	}

	void ScalePos(float x, float y);	///< 시작점에 대한 스케일링
	void ScaleArea(float x, float y);	///< 시작점과 넓이에 대한 스케일링
	void ScalePosRes(void);				///< 648*480기준값을 현재 해상도에 따른 스케일링
	void ScaleAreaRes(void);			///< 648*480기준값을 현재 해상도에 따른 스케일링
	void TranslateRes(void);			///< 648*480 중심점과의 거리만큼 현재 해상도에 맞게 이동

	void EnLarge(int w);				///< 좌우 상하를 w만큼씩 크기 키우기
	void Offset(int x, int y);			///< 포지션 이동

	bool Intersect(MRECT* pIntersect, const MRECT& r);	///< 두 사각형의 공통 영역 얻어내기

	bool operator==(const MRECT& other) const { return other.x==x && other.y==y && other.w==w && other.h==h; }
	bool operator!=(const MRECT& other) const { return other.x!=x || other.y!=y || other.w!=w || other.h!=h; }
};

/// 크기
struct MSIZE{
	int w, h;
public:
	MSIZE(void){}
	MSIZE(int w, int h){
		MSIZE::w = w;
		MSIZE::h = h;
	}

	bool operator==(const MSIZE& other) const { return other.w==w && other.h==h; }
	bool operator!=(const MSIZE& other) const { return other.w!=w || other.h!=h; }
};

/// a,r,g,b값을 unsigned long int값으로 변환
#define MINT_ARGB(a,r,g,b)		( ((((unsigned long int)a)&0xFF)<<24) | ((((unsigned long int)r)&0xFF)<<16) | ((((unsigned long int)g)&0xFF)<<8) | (((unsigned long int)b)&0xFF) )
/// r,g,b값을 unsigned long int값으로 변환
#define MINT_RGB(r,g,b)			( ((((unsigned long int)r)&0xFF)<<16) | ((((unsigned long int)g)&0xFF)<<8) | (((unsigned long int)b)&0xFF) )

/// r, g, b, a 컬러
struct MCOLOR{
public:
	unsigned char	r;		///< Red
	unsigned char	g;		///< Green
	unsigned char	b;		///< Blue
	unsigned char	a;		///< Alpha
public:
	MCOLOR(void){
		r = g = b = a = 0;
	}
	MCOLOR(unsigned char r, unsigned char g, unsigned char b, unsigned char a=255){
		MCOLOR::r = r, MCOLOR::g = g, MCOLOR::b = b, MCOLOR::a = a;
	}
	MCOLOR(unsigned long int argb){
		a = unsigned char( (argb & 0xFF000000) >> 24 );
		r = unsigned char( (argb & 0x00FF0000) >> 16 );
		g = unsigned char( (argb & 0x0000FF00) >> 8 );
		b = unsigned char( (argb & 0x000000FF) );
	}
	
	unsigned long int GetARGB(void){
		return MINT_ARGB(a, r, g, b);
	}
	unsigned long int GetRGB(void){
		return MINT_RGB(r, g, b);
	}

	bool operator==(const MCOLOR& other) const { return other.r==r && other.g==g && other.b==b && other.a==a; }
	bool operator!=(const MCOLOR& other) const { return other.r!=r || other.g!=g || other.b!=b || other.a!=a; }

};


// MBitmapRep
struct MBitmapRep
{
	string	strBitmapName;
	MRECT	m_rCoord[ 9];

	MBitmapRep()
	{
		memset(m_rCoord, 0, sizeof(MRECT)*9);
	}
};


// MAnchorStyle
struct MAnchorStyle
{
	bool bLeft;
	bool bRight;
	bool bTop;
	bool bBottom;
	MAnchorStyle(void)	{ bLeft = true; bRight = false; bTop = true; bBottom = false; }
	MAnchorStyle(bool left, bool right, bool top, bool bottom)
	{ bLeft = left; bRight = right; bTop = top; bBottom = bottom; }
};


// MTimer
class MTimer
{
protected:
	bool			m_bEnable;
	bool			m_bReversed;
	unsigned long	m_nStartTime;
	unsigned long	m_nArrivalTime;
	int				m_nUserData;

public:
	MTimer();

	void SetEnable( bool bEnable)					{ m_bEnable = bEnable;			}
	bool IsEnabled() const							{ return m_bEnable;				}

	void SetStartTime( unsigned long nStartTime)	{ m_nStartTime = nStartTime;	}
	unsigned long GetStartTime() const				{ return m_nStartTime;			}

	void SetArrivalTime( unsigned long nArrivalTime);
	unsigned long GetArrivalTime()					{ return m_nArrivalTime;		}
	bool IsArrival() const;

	void SetReverse( bool bReverse);
	bool IsReversed() const							{ return m_bReversed;			}

	void Start();

	double GetRatio() const;
	unsigned long GetElapsed() const;

	void SetUserData( int nData)					{ m_nUserData = nData;			}
	int GetUserData() const							{ return m_nUserData;			}
};



/// 정렬 모드 ( 비트 플래그 )
typedef int MAlignmentMode;
#define MAM_NOTALIGN	0	///< 정렬하지 않음
#define MAM_LEFT		1	///< 왼쪽 정렬
#define MAM_RIGHT		2	///< 오른쪽 정렬
#define MAM_HCENTER		4	///< 가로 가운데 정렬
#define MAM_TOP			16	///< 윗 정렬
#define MAM_BOTTOM		32	///< 아래 정렬
#define MAM_VCENTER		64	///< 세로 가운데 정렬
#define MAM_JUSTIFY		3	///> 양쪽 정렬


enum MDrawEffect
{
	MDE_NORMAL = 0,
	MDE_ADD,
	MDE_MULTIPLY,

	MDE_MAX

};

// Draw Context의 변경 State 목록
enum MDrawState {
	MDS_NONE	 = 0,
	MDS_CONTEXT,
	MDS_BITMAP,
	MDS_FONT,
};


// 방향
#define MLEFT				0
#define MTOP				1
#define MRIGHT				2
#define MBUTTOM				3


} // namespace mint3
