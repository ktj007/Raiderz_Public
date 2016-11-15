#include "stdafx.h"
#include "Horizon.h"
#include "RUtil.h"
#include "RDevice.h"
#include "RProfileInfo.h"

//////////////////////////////////////////////////////////////////////////
inline int integerFloor(const float& v)
{
	const int& u = *(const int*)&v;
	int sign = u >> 31;
	int exponent = ((u & 0x7fffffff)>>23) - 127;
	int expsign = ~(exponent >> 31);
	int mantissa = (u & ((1<<23) - 1));
	return (((((unsigned int)(mantissa | (1<<23)) << 8) >> (31-exponent)) & expsign) ^ (sign)) + ((!((mantissa<<8) & (((1<<(31-(exponent & expsign)))) - 1))) & sign);
}

/// fast floating point to integer conversion with ceiling.
/// warning: this code requires double precision!

inline int integerCeiling(const float& v)
{
	int u = (*(const int*)&v) ^ 0x80000000;
	int sign = u >> 31;
	int exponent = ((u & 0x7fffffff)>>23) - 127;
	int expsign = ~(exponent>>31);
	int mantissa = (u & ((1<<23) - 1));
	return -(int)((((((unsigned int)(mantissa | (1<<23)) << 8) >> (31-exponent)) & expsign) ^ (sign)) + ((!((mantissa<<8) & (((1<<(31-(exponent & expsign)))) - 1))) & sign & expsign));
}

//////////////////////////////////////////////////////////////////////////
// 
Horizon::Result Horizon::Test(float x1, float y1, float x2, float y2)
{
	if ( x1>x2 )
	{
		float t = x1;
		x1 = x2;
		x2 = t;

		t = y1;
		y1 = y2;
		y2 = t;
	}

	if (x2<0 || x1>m_nWidth-1)
		return ABOVE;

	float dx = x2 - x1;
	float dy = y2 - y1;

	const float dydx = dy / dx;

	if ( x1<0 )
	{
		// clip left
		y1 += -x1 * dydx;
		x1 = 0;
	}

	const float right = (float)(m_nWidth-1);
	if ( x2>right )
	{
		// clip right
		y2 -= (right-x2)*dydx;
		x2 = right;
	}

	const int ix1 = integerCeiling(x1-0.5f);
	const int ix2 = integerFloor(x2-0.5f);

	float y = y1;

	for ( int x = ix1; x<=ix2; ++x )
	{
		_ASSERT(x>=0);
		_ASSERT(x<(int)m_vecBuffer.size());

		if ( y>m_vecBuffer[x])
			return ABOVE;

		y += dydx;
	}

	return BELOW;
}

void Horizon::Insert(float x1, float y1, float x2, float y2)
{
	if ( x1>x2 )
	{
		float t = x1;
		x1 = x2;
		x2 = t;

		t = y1;
		y1 = y2;
		y2 = t;
	}

	if ( x2<0 || x1>=m_nWidth-1)
		return;

	float dx = x2 - x1;
	float dy = y2 - y1;

	const float dydx = dy / dx;

	if ( x1<0 )
	{
		// clip left
		y1 += -x1 * dydx;
		x1 = 0;
	}

	const float right = (float)(m_nWidth-1);

	if ( x2>right )
	{
		// clip right
		y2 -= (right-x2) * dydx;
		x2 = right;
	}

	const int ix1 = integerCeiling(x1-0.5f);
	const int ix2 = integerFloor(x2-0.5f);

	float y = y1;

	for ( int x=ix1; x<=ix2; ++x )
	{
		_ASSERT(x>=0);
		_ASSERT(x<(int)m_vecBuffer.size());

		if ( m_vecBuffer[x]<y )
			m_vecBuffer[x] = y;

		y += dydx;
	}
}
int Horizon::ClipCode(const MVector4 &v)
{
	int code = 0;

	if ( v.z <= 0.0f )
		code = 0x01;

	return code;
}

bool Horizon::ClipLine(MVector4 &a, MVector4 &b)
{
	int code_a = ClipCode(a);
	int code_b = ClipCode(b);


	if ((code_a | code_b)==0)
	{
		return true;
	}
	else if (code_a & code_b)
	{
		return false;
	}
	
	const float dx = b.x - a.x;
	const float dy = b.y - a.y;
	const float dz = b.z - a.z;
	const float dw = b.w - a.w;

	if ( code_a )
	{
		const float t = -a.z / dz;
		
		a.x = a.x + dx * t;
		a.y = a.y + dy * t;
		a.z = a.z + dz * t;
		a.w = a.w + dw * t;
	}
	else if ( code_b )
	{
		const float t = -b.z / dz;

		b.x = a.x + dx * t;
		b.y = a.y + dy * t;
		b.z = a.z + dz * t;
		b.w = a.z + dz * t;
	}

	return true;
}

void Horizon::ProjectAndViewportTransform(MVector4 &v, int width, int height)
{
	const float rhw = 1.0f/v.w;

	v.x *= rhw;
	v.y *= rhw;

	v.x = (v.x + 1.0f)*width*0.5f;
	v.y = (v.y + 1.0f)*height*0.5f;
}

void Horizon::ClipAndRenderLine(MVector4 a, MVector4 b)
{
	RPFC_THISFUNC;
	if ( ClipLine(a, b))
	{
		ProjectAndViewportTransform(a, m_nWidth, m_nHeight);
		ProjectAndViewportTransform(b, m_nWidth, m_nHeight);

		Insert(a.x, a.y, b.x, b.y);
	}
}

bool Horizon::ClipAndTestLine(MVector4 a, MVector4 b)
{
	RPFC_THISFUNC;
	if ( ClipLine(a, b))
	{
		ProjectAndViewportTransform(a, m_nWidth, m_nHeight);
		ProjectAndViewportTransform(b, m_nWidth, m_nHeight);

		return (Test(a.x, a.y, b.x, b.y) == BELOW);
	}

	return true;
}

void Horizon::Render(rs3::RDevice *pdevice)
{
	using namespace rs3;

	_ASSERT(pdevice);
	if ( m_vecBuffer.empty())
		return;


	static std::vector<RVt_posrhw_color> VerticesArray;
	VerticesArray.resize(m_vecBuffer.size());

	const RViewport &vp = pdevice->GetViewport();

	// set render state
	pdevice->SetTextureStageState( 0, RTSS_ALPHAOP, RTOP_SELECTARG1);
	pdevice->SetTextureStageState( 0, RTSS_ALPHAARG1, RTA_DIFFUSE);
	pdevice->SetTextureStageState( 0, RTSS_COLOROP, RTOP_SELECTARG1);
	pdevice->SetTextureStageState( 0, RTSS_COLORARG1, RTA_DIFFUSE);
	
	for ( unsigned int ix = 0; ix<m_vecBuffer.size(); ++ix )
	{
		float y = ( vp.nHeight - 1 ) - m_vecBuffer[ix];
		float x = ix/(float)m_nWidth * vp.nWidth;

		VerticesArray[ix].pos = RVector4(x, y, 0, 1);
		VerticesArray[ix].color = D3DCOLOR_XRGB(255, 0, 0);
	}

	bool bFogEnable = pdevice->GetFogEnable();
	pdevice->SetFogEnable(false);
	pdevice->SetFvF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	pdevice->DrawPrimitiveUP(RPT_LINESTRIP, VerticesArray.size()-1, (void*)&VerticesArray[0], sizeof(RVt_posrhw_color));
	pdevice->SetFogEnable(bFogEnable);

	return;
}
