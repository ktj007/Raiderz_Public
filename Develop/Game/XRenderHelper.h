#ifndef _XRENDER_HELPER_H
#define _XRENDER_HELPER_H

#include "RRenderHelper.h"

using namespace rs3;

class XRenderHelper : public RRenderHelper
{
public:
	/// 3d좌표 vPos의 화면에 문자열 출력
	static void DrawText(vec3& vPos, MCOLOR color, const wchar_t* text);
};





#endif // _XRENDER_HELPER_H