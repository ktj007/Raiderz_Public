#include "stdafx.h"
#include "TNoiseFadeEffect.h"
#include "XCharacter.h"
#include "MLocale.h"

TNoiseFadeEffect::TNoiseFadeEffect()
:XAlphaTestFadeEffect()
{

}

TNoiseFadeEffect::~TNoiseFadeEffect()
{

}

void TNoiseFadeEffect::UpdateXEffect( float fDelta )
{
	XAlphaTestFadeEffect::UpdateXEffect(fDelta);

	if(g_pMainApp->GetActor() == NULL)
		return;

	if(m_bInverse)
		UpdateInverseFadeFfect();
	else
		UpdateFadeFfect();
}

void TNoiseFadeEffect::UpdateFadeFfect()
{
	RSceneNode * pSceneNode = g_pMainApp->GetActor();
	if(m_vecMeshNodeName.empty() == false)
	{
		for(vector<tstring>::iterator it = m_vecMeshNodeName.begin(); it != m_vecMeshNodeName.end(); ++it)
		{
			RActorNode* pActorNode = g_pMainApp->GetActor()->GetActorNode(it->c_str());
			if(pActorNode)
			{
				int nMtrlCount = pActorNode->m_pMeshNode->GetMtrlCount();
				if(nMtrlCount > 0)
				{
					pActorNode->SetNoiseRef(m_fD);
				}
			}
		}
	}
	else
		pSceneNode->SetNoiseRef(m_fD);
}

void TNoiseFadeEffect::UpdateInverseFadeFfect()
{
	RSceneNode * pSceneNode = g_pMainApp->GetActor();
	if(m_vecMeshNodeName.empty() == false)
	{
		RSceneNodeList listChildren = pSceneNode->GetChildren();
		for(RSceneNodeList::iterator it = listChildren.begin(); it != listChildren.end(); ++it)
		{
			bool bSet = true;
			for(vector<tstring>::iterator itList = m_vecMeshNodeName.begin(); itList != m_vecMeshNodeName.end(); ++itList)
			{
				if(*itList == (*it)->GetNodeName())
					bSet = false;
			}

			if(bSet)
			{
				(*it)->SetNoiseRef(m_fD);
			}
		}
	}
	else
		pSceneNode->SetNoiseRef(m_fD);
}
