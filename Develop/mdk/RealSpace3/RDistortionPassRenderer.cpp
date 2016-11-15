#include "StdAfx.h"
#include "RDistortionPassRenderer.h"

namespace rs3 
{

void RDistortionPassRenderer::RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;
	RPostEffectDistortion* pPostEffectDistortion = (RPostEffectDistortion*)REngine::GetSceneManager().GetPostEffect(PE_DISTORTION);
	if( pPostEffectDistortion==NULL || pPostEffectDistortion->IsValid()==false )
		return;


	for(SCENE_SET::iterator Iter = pRenderingList->begin(); Iter!=pRenderingList->end(); ++Iter)
	{
		RActor* pActor = MDynamicCast(RActor, *Iter);
		if(pActor==NULL)
			continue;

		_ASSERT(pActor->GetAlpha2PassRendering()==false);


		if( pPostEffectDistortion->IsCulledVisible(pActor, pCurrentCamera->GetWorldPosition())==false )
			continue;


		vector<RActorNode*>& ActorNodeList = pActor->GetSortedRenderableNodes();
		for(size_t i=0; i<ActorNodeList.size(); ++i)
		{
			RActorNode* pActorNode = ActorNodeList[i];

			if(pActorNode->IsAlpha())
				continue;

			if(pActorNode->HasTranslucentMaterial())
			{
				for(size_t i=0; i< pActorNode->m_vecMaterialTable.size(); ++i)
				{
					if(pActorNode->GetSubMaterial(i)->CheckFlag(RM_FLAG_USEOPACITY))
						continue;

					pPostEffectDistortion->RenderActorNodeDistortion( pCurrentCamera, pActorNode, i );
				}				
			}
			else
				pPostEffectDistortion->RenderActorNodeDistortion( pCurrentCamera, pActorNode, -1 );			
		}
	}
}

}