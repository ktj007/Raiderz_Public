#pragma once

#include <d3d9.h>

class RSurfaceUtil
{
	// A, B는 동일한 크기이어야 한다.
	// A = A and B;
	static bool MaskingAnd(LPDIRECT3DSURFACE9 pSurfaceA, LPDIRECT3DSURFACE9 pSurfaceB);
};