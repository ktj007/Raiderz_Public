#include "StdAfx.h"
#include "XEffectPreLoaderResourceMgr.h"
// #include "REffectResource.h"
// #include "REffectResourceMgr.h"
#include "REffectInstance.h"

XEffectPreLoaderResourceMgr::XEffectPreLoaderResourceMgr(void)
{
}

XEffectPreLoaderResourceMgr::~XEffectPreLoaderResourceMgr(void)
{
}

void XEffectPreLoaderResourceMgr::AddXEffectPreLoader( const wstring& strEffectName )
{
// 	REffectResource* pEffectResource = REngine::GetSceneManager().CreateResource< REffectResource >( strEffectName.c_str() );
// 	_ASSERT(pEffectResource);
// 	if(pEffectResource == NULL)
// 		return;
// 
// 	bool bRet = pEffectResource->BuildAll( true );
// 	if (!bRet)
// 	{
// 		mlog("XEffectPreLoaderResourceMgr::AddXEffectPreLoader - 잘못된 이팩트 리소스(%s)\n", strEffectName.c_str());
// 		return;
// 	}

	REffectInstanceSceneNode* pNewInstance = new REffectInstanceSceneNode;
	pNewInstance->Create( MLocale::ConvUTF16ToAnsi(strEffectName.c_str()).c_str(), true );

	m_vecPreLoadResource.push_back(pNewInstance);
}

void XEffectPreLoaderResourceMgr::ReleaseXEffectPreLoader( const wstring& strEffectName )
{
// 	for(vector<REffectResource*>::iterator it = m_vecPreLoadResource.begin(); it != m_vecPreLoadResource.end(); it++)
// 	{
// 		REffectResource* pEffectResource = (*it);
// 		if(pEffectResource->GetName() == strEffectName)
// 		{
// 			REngine::GetSceneManager().ReleaseResource(pEffectResource);
// 			m_vecPreLoadResource.erase(it);
// 			return;
// 		}
// 	}

 	for(vector<REffectInstanceSceneNode*>::iterator it = m_vecPreLoadResource.begin(); it != m_vecPreLoadResource.end(); it++)
 	{
 		REffectInstanceSceneNode* pEffectInstance = (*it);
		if ( MLocale::ConvAnsiToUTF16(pEffectInstance->GetFileName().c_str())  == strEffectName )
		{
			delete pEffectInstance;
			m_vecPreLoadResource.erase(it);
			return;
		}
	}
}

void XEffectPreLoaderResourceMgr::AllReleaseXEffectPreLoader()
{
// 	for(vector<REffectResource*>::iterator it = m_vecPreLoadResource.begin(); it != m_vecPreLoadResource.end(); it++)
// 	{
// 		REngine::GetSceneManager().ReleaseResource(*it);
// 	}

	for(vector<REffectInstanceSceneNode*>::iterator it = m_vecPreLoadResource.begin(); it != m_vecPreLoadResource.end(); it++)
	{
		REffectInstanceSceneNode* pEffectInstance = (*it);
		delete pEffectInstance;
	}
	m_vecPreLoadResource.clear();
}
