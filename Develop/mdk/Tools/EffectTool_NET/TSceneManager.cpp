#include "StdAfx.h"
#include "TSceneManager.h"
#include "RSceneNodeID.h"

TSceneManager::TSceneManager() 
{
}

//rs3::REffectInstance* TSceneManager::CreateEmptyEffect( const char* pEffectName )
//{
//	_ASSERT( m_pEffectMgr );
//	return m_pEffectMgr->CreateEmptyEffect( pEffectName );
//}
//
//void TSceneManager::ClearEffectMgr()
//{
//	_ASSERT( m_pEffectMgr );
//	m_pEffectMgr->Destroy();
//}

RSceneNode* TSceneManager::CreateSceneNode(const char *szNodeName)
{
	std::string strNodeName =szNodeName;

	if(stricmp(szNodeName,RSID_ACTOR)==0)
		strNodeName = RSID_EFFECTACTOR;

	return RSceneManager::CreateSceneNode(strNodeName.c_str());
}