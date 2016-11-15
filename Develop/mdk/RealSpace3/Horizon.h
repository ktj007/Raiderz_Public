#pragma once

/**
 */

#include "RTypes.h"
#include <utility>
#include <vector>

namespace rs3
{
	class RDevice;
};

// GPG 4권 5.15 수평선 클래스 소스 코드 참조
class Horizon
{
private:
	int m_nWidth;
	int m_nHeight;

	std::vector<float> m_vecBuffer;

	int  ClipCode(const MVector4 &v);
	bool ClipLine(MVector4 &a, MVector4 &b);
	void ProjectAndViewportTransform(MVector4 &v, int width, int height);
public:
	// RHW 버텍스 구조체
	struct RVt_posrhw_color
	{
		MVector4 pos;
		DWORD color;
	};

	enum Result
	{
		ABOVE, BELOW
	};

	Horizon()
		: m_nWidth(0)
		, m_nHeight(0)
	{
	}

	void Init(int width, int height)
	{
		m_nWidth = width;
		m_nHeight = height;

		m_vecBuffer.resize(m_nWidth);
		
		Clear();
	}
	
	inline void Clear()
	{
		std::fill(m_vecBuffer.begin(), m_vecBuffer.end(), -1.1f);
	}

	Result Test(float x1, float y1, float x2, float y2);
	void Insert(float x1, float y1, float x2, float y2);

	// MVector4 는 matrix World, View, Projection을 거쳐서 생성된 좌표계 
	void ClipAndRenderLine(MVector4 a, MVector4 b);
	bool ClipAndTestLine(MVector4 a, MVector4 b);
	void Render(rs3::RDevice *pdevice);
};
