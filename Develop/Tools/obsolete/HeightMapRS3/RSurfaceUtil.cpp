#include "stdafx.h"
#include "RSurfaceUtil.h"
#include <d3d9.h>

bool RSurfaceUtil::MaskingAnd(LPDIRECT3DSURFACE9 pSurfaceA, LPDIRECT3DSURFACE9 pSurfaceB)
{
	D3DSURFACE_DESC kDescA, kDescB;
	pSurfaceA->GetDesc(&kDescA);
	pSurfaceB->GetDesc(&kDescB);
	// 크기가 동일해야 한다.
	if ( kDescA.Width != kDescB.Width )
		return false;
	if ( kDescA.Height != kDescB.Height )
		return false;
	// 포맷이 A8로 동일해야 한다.
	if ( kDescA.Format != D3DFMT_A8 )
		return false;
	if ( kDescA.Format != kDescB.Format )
		return false;

	D3DLOCKED_RECT kRectA, kRectB;

	pSurfaceA->LockRect(&kRectA, NULL, 0);
	pSurfaceB->LockRect(&kRectB, NULL, 0);

	BYTE *pDataA = static_cast<BYTE*>(kRectA.pBits);
	BYTE *pDataB = static_cast<BYTE*>(kRectB.pBits);

	for ( UINT uy = 0; uy<kDescA.Height; ++uy )
	{
		for ( UINT ux = 0; ux<kDescA.Width; ++ux )
		{
			int nIndexA = ux + uy*kRectA.Pitch;
			int nIndexB = ux + uy*kRectB.Pitch;

			pDataA[nIndexA] = pDataA[nIndexA]*pDataB[nIndexB];
		}
	}

	return true;
}