#include "stdafx.h"
#include "XNoiseFadeEffect.h"
#include "XCharacter.h"

XNoiseFadeEffect::XNoiseFadeEffect()
:XAlphaTestFadeEffect()
{

}

XNoiseFadeEffect::~XNoiseFadeEffect()
{

}

void XNoiseFadeEffect::UpdateXEffect( float fDelta )
{
	XAlphaTestFadeEffect::UpdateXEffect(fDelta);

	XObject* pObject = gg.omgr->FindActor(m_uidOwer);
	if(pObject == NULL)
	{
		// 없어졌다.
		m_bDelMe = true;
		return;
	}

	if(m_bInverse)
		UpdateInverseFadeFfect(pObject);
	else
		UpdateFadeFfect(pObject);
}

void XNoiseFadeEffect::SetFadeEffectColor( DWORD& dwColor1, DWORD& dwColor2, float& fSize1, float& fSize2 )
{
	XAlphaTestFadeEffect::SetFadeEffectColor(dwColor1, dwColor2, fSize1, fSize2);

	XObject* pObject = gg.omgr->FindActor(m_uidOwer);
	if(pObject == NULL)
	{
		// 없어졌다.
		m_bDelMe = true;
		return;
	}

	//pObject->GetActor()->SetNoiseRefColor1()
}

void XNoiseFadeEffect::UpdateFadeFfect( XObject* pObject )
{
	RSceneNode * pSceneNode = pObject->GetActor();
	if(m_vecMeshNodeName.empty() == false)
	{
		for(vector<tstring>::iterator it = m_vecMeshNodeName.begin(); it != m_vecMeshNodeName.end(); ++it)
		{
			RActorNode* pActorNode = pObject->GetActor()->GetActorNode(MLocale::ConvUTF16ToAnsi(it->c_str()).c_str());
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

	pSceneNode->SetNoiseRef(m_fD);
}

void XNoiseFadeEffect::UpdateInverseFadeFfect( XObject* pObject )
{
	RSceneNode * pSceneNode = pObject->GetActor();
	if(m_vecMeshNodeName.empty() == false)
	{
		RSceneNodeList listChildren = pSceneNode->GetChildren();
		for(RSceneNodeList::iterator it = listChildren.begin(); it != listChildren.end(); ++it)
		{
			bool bSet = true;
			for(vector<tstring>::iterator itList = m_vecMeshNodeName.begin(); itList != m_vecMeshNodeName.end(); ++itList)
			{
				if(MLocale::ConvUTF16ToAnsi(itList->c_str()) == (*it)->GetNodeName())
					bSet = false;
			}

			if(bSet)
			{
				(*it)->SetNoiseRef(m_fD);
			}
		}
	}
	else
		pSceneNode->SetNoiseRef(m_fD);;
}
