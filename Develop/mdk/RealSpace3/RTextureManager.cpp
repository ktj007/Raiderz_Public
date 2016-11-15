#include "StdAfx.h"
#include "RTextureManager.h"
#include "MTypes.h"
#include "RDevice.h"
#include "RResourceLoader.h"

namespace rs3 {
RTexture* RT_FRAMEBUFFER = (RTexture*)-1;	///< frame buffer 를 나타낼 포인터, 내용자체는 의미없다

RTextureManager::RTextureManager(RDevice &device) : m_device(device)
{
}

RTextureManager::~RTextureManager(void)
{
}

RTexture* RTextureManager::CreateTextureForBuildLater(const char *fileName, const RTEXTUREFILTERTYPE filter,
	DWORD flags, RFORMAT pf, bool bBackground, RResource::LoadingController* pController)
{
	if ( fileName[0] == '\0' )
		return R_NONE;

	RSceneManager* pSceneManager = REngine::GetSceneManagerPtr();

	// 중복관리 reference count 체크
	RResource* pResource = RResourceManager::GetResource(fileName);
	if(pResource)	// 이미 있다면..
	{
		pResource->AddRef();
		return (RTexture*)pResource;
	}

	RTexture* pTexture = m_device.NewTexture();
	pTexture->m_strName = fileName;
	pTexture->m_bCreateFromFile = true;
	pTexture->filter = filter;
	pTexture->flags = flags;

	AddResource(pTexture);

	pTexture->BuildAll( bBackground, pController );

	//pSceneManager->AddResourceLoadMonitoring(pTexture, bBackground);
	//pSceneManager->AddLoadingList(pTexture, bBackground, pController);

	return pTexture;
}

RTexture* RTextureManager::CreateTexture(const char *fileName, const RTEXTUREFILTERTYPE filter,
	DWORD flags, RFORMAT pf, bool bBackground, RResource::LoadingController* pController)
{
	if ( fileName[0] == '\0' )
		return R_NONE;

	// 중복관리 reference count 체크
	RResource* pResource = RResourceManager::GetResource(fileName);
	if(pResource)	// 이미 있다면..
	{
		pResource->AddRef();
		return (RTexture*)pResource;
	}

	RTexture* pTexture = m_device.NewTexture();
	pTexture->m_strName = fileName;
	pTexture->m_bCreateFromFile = true;
	pTexture->filter = filter;
	pTexture->flags = flags;
	
	RBackgroundWorker& bg_worker = REngine::GetBackgroundWorker();

	// 백그라운드 로딩이면 작업큐에 추가후 리턴
	if( bBackground && bg_worker.IsInited() )
	{
		if(pController)
			pTexture->AddLoadingController(pController);

		AddResource(pTexture);
		pTexture->BuildAll( true );
// 		pTexture->m_pLoader = new RResourceLoader( pTexture);
// 		bg_worker.AddToReadyQueue( pTexture->m_pLoader );
// 		bg_worker.FlushReadyQueueAndRun();
		
		return pTexture;
	}

	pTexture->BuildAll( false );
	if( pTexture->GetState()==RRS_WRONG )
	{
		DeleteTexture(pTexture);
//		ReleaseRef(pTexture);
//		delete pTexture;
		return NULL;
	}

	AddResource(pTexture);
	return pTexture;
}

RTexture* RTextureManager::CreateTexture(int nWidth, int nHeight, RFORMAT pf , const RTEXTUREFILTERTYPE filter, DWORD flags , void *pixels, const char* szName )
{
	bool bNamed = szName && strlen(szName)>0;

	if(bNamed)
	{
		RResource* pResource = RResourceManager::GetResource(szName);
		if(pResource)	// 이미 있다면.. 에러.
			return NULL;
	}

	RTexture* pTexture = m_device.NewTexture();

	pTexture->filter = filter;
	pTexture->flags = flags;

	pTexture->info.nTextureWidth = nWidth;
	pTexture->info.nTextureHeight = nHeight;
	pTexture->info.pixelFormat = pf;

//	pTexture->m_state = RRS_FILLED;

	// 백그라운드가 아니게 로딩
	if( !pTexture->BuildAll( false ) )
	{
		_ASSERT(FALSE);

		ReleaseRef(pTexture);
		delete pTexture;
		return NULL;
	}

	if(pixels)
	{
		bool bResult = false;
		bResult = m_device.UpdateTexture(pTexture,0,0,0,nWidth,nHeight,pixels);
		_ASSERT(bResult);
		if ( pTexture->CheckFlags(RCT_AUTOGENMIPMAP) )
		{
			bResult = m_device.GenerateMipmapLevels(pTexture);
			_ASSERT(bResult);
		}
	}

	if(bNamed)
	{
		pTexture->m_strName = szName;
		AddResource(pTexture);
	}
	else
		Add(pTexture);
	return pTexture;
}

RTexture* RTextureManager::CreateRenderTargetTexture(int nWidth, int nHeight, RFORMAT pf , const RTEXTUREFILTERTYPE filter, DWORD flags)
{
	RTexture* pTexture = m_device.NewTexture();
//	pTexture->m_state = RRS_FILLED;
	pTexture->info.nTextureWidth = nWidth;
	pTexture->info.nTextureHeight = nHeight;
	pTexture->info.pixelFormat = pf;
	pTexture->flags = flags | RCT_RENDERTARGET;
	pTexture->filter = filter;

	if(!pTexture->BuildAll(false))
	{
		_ASSERT(FALSE);

		ReleaseRef(pTexture);
		delete pTexture;
		return NULL;
	}

	Add(pTexture);
	return pTexture;
}

/*
RTexture* RTextureManager::GetTextureByName( const char* pSzTextureName, bool bCreateIfNotExist, RTexture* pDefaultPixelSrcTexture)
{
	_ASSERT( pSzTextureName != NULL && "이름이 비어있음");

	// 중복관리 reference count 체크
	RResource* pResource = RResourceManager::GetResource(pSzTextureName);
	if(pResource)	// 이미 있다면..
	{
		pResource->AddRef();
		return (RTexture*)pResource;
	}
	else
	{
		RTexture* pTexture(NULL);
		if( bCreateIfNotExist )
		{
			pTexture = m_device.NewTexture();
			pTexture->m_strName = pSzTextureName;

			if( pDefaultPixelSrcTexture )
				pTexture->SetDefaultPixel( pDefaultPixelSrcTexture );

			AddResource( pTexture );
		}

		return pTexture;
	}
}

RTexture* RTextureManager::CreateTextureByName(const char *pSzTextureName, RTexture* pDefaultPixelSrcTexture, const RTEXTUREFILTERTYPE filter, DWORD flags, RFORMAT pf)
{
	_ASSERT( pSzTextureName != NULL && "이름이 비어있음");

	// 중복관리 reference count 체크
	RResource* pResource = RResourceManager::GetResource(pSzTextureName);
	if(pResource)	// 이미 있다면..
	{
		pResource->AddRef();
		return (RTexture*)pResource;
	}

	RTexture* pTexture(NULL);
	pTexture = m_device.NewTexture();
	pTexture->m_strName = pSzTextureName;
	pTexture->filter = filter;
	pTexture->flags = flags;
	pTexture->info.pixelFormat = pf;

	if( pDefaultPixelSrcTexture )
		pTexture->SetDefaultPixel( pDefaultPixelSrcTexture );

	AddResource( pTexture );

	return pTexture;
}
*/

RTexture* RTextureManager::Add(RTexture* pTexture)
{
	m_unnamedTextures.insert(pTexture);
// 	HTEXTURE hTexture = (HTEXTURE)AddResource(pTexture);
	return pTexture;
}

RTexture* RTextureManager::Get(RTexture* hTexture) const 
{
	return (RTexture*)hTexture;
}

bool RTextureManager::DeleteTexture(RTexture* pTexture)
{
	if(!pTexture) return false;

	if(0 == ReleaseRef(pTexture))
	{
		m_unnamedTextures.erase(pTexture);

		RemoveResource( pTexture->GetName() );
		RequestDeepDeletion( pTexture, NULL );

		return true;
	}
	return false;
}

void RTextureManager::ForceDeleteTexture(RTexture* pTexture)
{
	mlog("%s texture is not removed ( ref count = %d ).\n",pTexture->GetName().c_str(),pTexture->GetRefCount());

	// 강제로 릴리즈
	while(pTexture->GetRefCount()>1)
		ReleaseRef(pTexture);

	DeleteTexture((RTexture*)pTexture);
}

void RTextureManager::Destroy()
{
#ifndef MODIFY_LOADER
	while(!m_resources.empty())
	{
		RTexture* pTex = (RTexture*)m_resources.begin()->second;
		ForceDeleteTexture(pTex);
	}
#else
	class ForceDeleteTextureFunctor : public RResourceContainerFunctor
	{
	public:
		ForceDeleteTextureFunctor(RTextureManager* pTextureManager) : m_pTextureManager(pTextureManager) {}
	public:
		void operator() (RRESOURCEHASHMAP& mapResources)
		{
			for( RRESOURCEHASHMAP::iterator itr = mapResources.begin(); itr != mapResources.end(); ++itr )
			{
				RTexture* pTex = (RTexture*)itr->second;
				m_pTextureManager->ForceDeleteTexture(pTex);
			}
		}
	private:
		RTextureManager* m_pTextureManager;
	};

	m_Resources.Safe_Operation(ForceDeleteTextureFunctor(this));
#endif


	while(!m_unnamedTextures.empty())
	{
		RTexture* pTex = (RTexture*)*m_unnamedTextures.begin();
		ForceDeleteTexture(pTex);
	}
}

int RTextureManager::GetUsingVideoMemory()
{
	int nNamedTextureUsed = 0;
	int nUnnamedTextureUsed = 0;

#ifndef MODIFY_LOADER
	for(RRESOURCEHASHMAP::iterator itr = m_resources.begin(); itr!=m_resources.end(); ++itr)
	{
		RTexture* pTex = (RTexture*)itr->second;
		nNamedTextureUsed += pTex->info.GetSize();
	}
#else
	class VideoMemoryFunctor : public RResourceContainerFunctor
	{
	public:
		VideoMemoryFunctor(void):m_UsingVideoMemory(0) {}
	public:
		void operator() (RRESOURCEHASHMAP& mapResources)
		{
			for( RRESOURCEHASHMAP::iterator itr = mapResources.begin(); itr != mapResources.end(); ++itr )
			{
				RTexture* pTex = (RTexture*)itr->second;
				m_UsingVideoMemory += pTex->info.GetSize();	
			}
		}
	public:
		int		m_UsingVideoMemory;
	};

	VideoMemoryFunctor VideoMemory;
	m_Resources.Safe_Operation(VideoMemory);
	nNamedTextureUsed = VideoMemory.m_UsingVideoMemory;
#endif


	for(set<RTexture*>::iterator itr = m_unnamedTextures.begin();itr != m_unnamedTextures.end(); ++itr)
	{
		RTexture* pTex = (RTexture*)*itr;
		nUnnamedTextureUsed += pTex->info.GetSize();
	}

	return nNamedTextureUsed + nUnnamedTextureUsed;
}

void RTextureManager::Invalidate()
{
#ifndef MODIFY_LOADER
	for(RRESOURCEHASHMAP::iterator itr = m_resources.begin(); itr!=m_resources.end(); ++itr)
	{
		RTexture* pTex = (RTexture*)itr->second;
		pTex->Invalidate();
	}
#else
	class TextureInvalidateFunctor : public RResourceContainerFunctor
	{
	public:
		TextureInvalidateFunctor(void) {}
	public:
		void operator() (RRESOURCEHASHMAP& mapResources)
		{
			for( RRESOURCEHASHMAP::iterator itr = mapResources.begin(); itr != mapResources.end(); ++itr )
			{
				RTexture* pTex = (RTexture*)itr->second;
				pTex->Invalidate();
			}
		}
	};

	m_Resources.Safe_Operation( TextureInvalidateFunctor() );
#endif


	for(set<RTexture*>::iterator itr = m_unnamedTextures.begin();itr != m_unnamedTextures.end(); ++itr)
	{
		RTexture* pTex = (RTexture*)*itr;
		pTex->Invalidate();
	}
}

void RTextureManager::Restore()
{
#ifndef MODIFY_LOADER
	for(RRESOURCEHASHMAP::iterator itr = m_resources.begin(); itr!=m_resources.end(); ++itr)
	{
		RTexture* pTex = (RTexture*)itr->second;
		pTex->Restore();
	}
#else
	class TextureRestoreFunctor : public RResourceContainerFunctor
	{
	public:
		TextureRestoreFunctor(void) {}
	public:
		void operator() (RRESOURCEHASHMAP& mapResources)
		{
			for( RRESOURCEHASHMAP::iterator itr = mapResources.begin(); itr != mapResources.end(); ++itr )
			{
				RTexture* pTex = (RTexture*)itr->second;
				pTex->Restore();
			}
		}
	};

	m_Resources.Safe_Operation( TextureRestoreFunctor() );
#endif


	for(set<RTexture*>::iterator itr = m_unnamedTextures.begin();itr != m_unnamedTextures.end(); ++itr)
	{
		RTexture* pTex = (RTexture*)*itr;
		pTex->Restore();
	}
}


RTexture* RTextureManager::GetTexture( const char* pSzTextureName )
{
	RResource* pResource = RResourceManager::GetResource(pSzTextureName);
	if (pResource)
	{
		return dynamic_cast<RTexture*>(pResource);
	}
	return NULL;
}

}

