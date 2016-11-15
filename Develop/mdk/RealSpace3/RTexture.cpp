#include "StdAfx.h"
#include "RTexture.h"
#include "RResourceID.h"

namespace rs3 {

RTexture::RTexture(void) : RResource(), flags(0), filter(RTF_POINT), m_bCreateFromFile(false)
{
	m_nResID = RRESOURCETYPE::RR_TEXTURE;
}

RTexture::~RTexture(void)
{
}

bool RTexture::SaveToFile(const char* szFileName, RFORMAT format)
{
	return false;
}

unsigned long RTextureInfo::GetSize() const
{
	/**
	 	RGB 24의 크기는 원래 24bit이나, 이 포맷(D3DFMT_R8G8B8)을 지원하는 그래픽카드는 없고, 99%의 그래픽카드가 XRGB32 즉, D3DFMT_X8R8G8B8 을 지원한다.
		그래서, RGB24를 4 로 하는 것인데 ATI PerfStudio의 크기는 RGB24를 3으로 지정한 값과 거의 동일하다.
	 */
	float rate = 4.f;
	switch(pixelFormat)
	{
	case RFMT_DXT1:
		rate = 4.f / 8.f;
		break;
	case RFMT_DXT3:
	case RFMT_DXT5:
		rate = 4.f / 4.f;
		break;
	case RFMT_RGB24:
		rate = 4.0f;
		break;
	case RFMT_XRGB32:
	case RFMT_ARGB32:
	case RFMT_INDEX32:
	case RFMT_D24FS8:
	case RFMT_D32F_LOCKABLE:
	case RFMT_D32:
		rate = 4.f;
		break;
	case RFMT_D24S8:
	case RFMT_D24X8:
		rate = 4.f;
		break;
	case RFMT_D16:
	case RFMT_INDEX16:
	case RFMT_RGB16:
		rate = 2.f;
		break;
	}

	int nLevel0Size = static_cast<int>(nTextureWidth*nTextureHeight*rate);
	return bMipmap ? static_cast<int>(nLevel0Size * 1.3f) : nLevel0Size;
}

}