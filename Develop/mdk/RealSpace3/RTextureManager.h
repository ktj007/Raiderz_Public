#pragma once

#include "RPreDefine.h"
#include "RResourceManager.h"
#include "RTexture.h"
#include "RDevice.h"

//TODO_ignore444 : 건즈/레이더즈 하위호환을 위해 빈파일을 둠. 삭제해야함

namespace rs3
{
	class RDevice;

	class RS_API RTextureManager : public RResourceManager
	{

	protected:
		set<RTexture*> m_unnamedTextures;
		RDevice		&m_device;

	public:
		RTextureManager(RDevice &device);
		virtual ~RTextureManager(void);

		void Destroy();

		// create texture
		RTexture* CreateTextureForBuildLater(const char *fileName, const RTEXTUREFILTERTYPE filter = RTF_LINEAR, DWORD flags = RCT_AUTOGENMIPMAP, 
			RFORMAT pf = RFMT_NONE, bool bBackground = false, RResource::LoadingController* pController = NULL);
		RTexture* CreateTexture(const char *fileName, const RTEXTUREFILTERTYPE filter = RTF_LINEAR, DWORD flags = RCT_AUTOGENMIPMAP, 
			RFORMAT pf = RFMT_NONE, bool bBackground = false, RResource::LoadingController* pController = NULL);
		RTexture* CreateTexture(int nWidth, int nHeight, RFORMAT pf , const RTEXTUREFILTERTYPE filter = RTF_LINEAR, DWORD flags = 0, void *pixels = NULL, const char* szName = NULL);
		RTexture* CreateRenderTargetTexture(int nWidth, int nHeight, RFORMAT pf , const RTEXTUREFILTERTYPE filter = RTF_LINEAR, DWORD flags = 0);
		//	RTexture* CreateTextureByName(const char *pSzTextureName, RTexture* pDefaultPixelSrcTexture, const RTEXTUREFILTERTYPE filter = RTF_LINEAR, DWORD flags = 0, RFORMAT pf = RFMT_NONE);

		// common management
		bool DeleteTexture(RTexture* pTexture);	// 리턴값은 ref = 0되어 정말 삭제되었는지 여부
		void ForceDeleteTexture(RTexture* pTexture);

		// unnamed texture management --
		RTexture* Add(RTexture* pTexture);
		RTexture* Get(RTexture* hTexture) const;

		// named texture management --
		//	RTexture* GetTextureByName( const char* pSzTextureName, bool bCreateIfNotExist, RTexture* pDefaultPixelSrcTexture);
		RTexture* GetTexture(const char* pSzTextureName);

		virtual int GetUsingVideoMemory();
		virtual int GetUsingSystemMemory(){ return GetUsingVideoMemory(); }

		void Invalidate();
		void Restore();
	};

}