#include "StdAfx.h"
#include "M_Visitors.h"
#include "REffectInstance.h"
#include "M_ToolSceneNodeInstance.h"

bool CVisibleSetter::OnPreOrderVisit( rs3::RSceneNode* pSceneNode )
{
	if (pSceneNode->GetNodeIDString() == rs3::RSID_ACTOR ||
		pSceneNode->GetNodeIDString() == rs3::RSID_SCENEINSTANCE
		)
		pSceneNode->SetVisible(m_bVisible);

	return true;

}

bool CEffectStarter::OnPreOrderVisit( rs3::RSceneNode* pSceneNode )
{
	if (rs3::REffectSceneNode* pEffectInstance = MDynamicCast( rs3::REffectSceneNode, pSceneNode))
	{
		pEffectInstance->Start();
		return false;
	}
	return true;
}

bool CEffectStopper::OnPreOrderVisit( rs3::RSceneNode* pSceneNode )
{
	if (rs3::REffectSceneNode* pEffectInstance = MDynamicCast( rs3::REffectSceneNode, pSceneNode))
	{
		pEffectInstance->Stop();
		return false;
	}
	return true;
}

bool CEffectInstanceReloader::OnPreOrderVisit( rs3::RSceneNode* pSceneNode )
{
	rs3::REffectInstanceSceneNode* pEffectInstance = MDynamicCast( rs3::REffectInstanceSceneNode, pSceneNode);
	if( pEffectInstance && stricmp( pEffectInstance->GetFileName().c_str(), m_strEffectName.c_str() ) == 0 )
	{
		pEffectInstance->Reload();
	}

	return true;
}

bool CSceneInstanceClearVisitor::OnPreOrderVisit( rs3::RSceneNode* pSceneNode )
{
	rs3::RToolSceneNodeInstance* pSceneInstance = MDynamicCast( rs3::RToolSceneNodeInstance, pSceneNode);
	if( pSceneInstance && stricmp( pSceneInstance->GetFileName().c_str(), m_strSceneName.c_str() ) == 0 )
	{
		pSceneInstance->ClearForReload();
	}

	return true;
}

bool CSceneInstanceReloader::OnPreOrderVisit( rs3::RSceneNode* pSceneNode )
{
	rs3::RToolSceneNodeInstance* pSceneInstance = MDynamicCast( rs3::RToolSceneNodeInstance, pSceneNode);
	if( pSceneInstance && stricmp( pSceneInstance->GetFileName().c_str(), m_strSceneName.c_str() ) == 0 )
	{
		pSceneInstance->Create( m_strSceneName.c_str(), false );
	}

	return true;
}

bool CEditableNodeCollector::OnPreOrderVisit( rs3::RSceneNode* pSceneNode )
{
	if( pSceneNode->QueryAttribute( rs3::RSNA_DO_NOT_SAVE ) )
		return false;

	return true; 
};

void CEditableNodeCollector::OnPostOrderVisit( rs3::RSceneNode* pSceneNode )
{
	m_SceneNodes.push_back(pSceneNode);
}