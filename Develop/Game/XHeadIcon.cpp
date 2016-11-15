#include "stdafx.h"
#include "XHeadIcon.h"
#include "XMiniHpBar.h"
#include "XModuleEntity.h"
#include "XCharacterCaption.h"
#include "XCaptionMgr.h"
#include "XCharacter.h"
#include "RMesh.h"
#include "XNonPlayer.h"
#include "XEffect.h"
#include "XEffectManager.h"
#include "XModuleMovable.h"

XHeadIconPlacer::XHeadIconPlacer( XCharacterCaption* pNameCaption, XCharacterCaption* pGuildCaption, XMiniHpBar* pMiniHPBar ) 
: m_pNameCaption(pNameCaption)
, m_pGuildCaption(pGuildCaption)
, m_pMiniHPBar(pMiniHPBar)
, m_bNameCaptionLastVisible(false)
, m_bGuildCaptionLastVisible(false)
, m_uidNPCIConEffect(MUID::ZERO)
, m_pTitleBoneActorNode(NULL)
{

}

void XHeadIconPlacer::SetCaptionOffset( float fOffset, XCharacterCaption* pNameCaption, XCharacterCaption* pGuildCaption, XMiniHpBar* pMiniHPBar )
{
	float fFontHeight = GetFontHeight();

	float fCaptionOffset = fOffset + fFontHeight;

	pMiniHPBar->SetOffSet(fCaptionOffset + MINIBAR_MARGIN);

	if (IsCaptionVisible(pGuildCaption))
	{
		pGuildCaption->SetOffSet( fCaptionOffset);

		NextOffset(fCaptionOffset, fFontHeight);
	}

	pNameCaption->SetOffSet( fCaptionOffset);

	if (IsCaptionVisible(pNameCaption))
	{
		NextOffset(fCaptionOffset, fFontHeight);
	}
}

void XHeadIconPlacer::UpdatePlayerNameCaption()
{
	SetCaptionOffset(PLAYER_ID_OFFSET_Z, m_pNameCaption, m_pGuildCaption, m_pMiniHPBar);
}

void XHeadIconPlacer::UpdateNPCNameCaption( XCharacter* pActor, wstring strTitleBoneName, float fTitleHeight, float fScale )
{
	PFC_THISFUNC;

	RActorNode* pActorNode = NULL;

	if (NULL == m_pTitleBoneActorNode)
	{
		m_pTitleBoneActorNode = pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(strTitleBoneName.c_str()).c_str());
	}		
	
	pActorNode = m_pTitleBoneActorNode;
	

	float fOffSet = fTitleHeight;

	if(pActorNode)
	{
		pActor->AddActiveActorNode(pActorNode);
		m_pNameCaption->SetBoneNode(pActorNode);
		m_pGuildCaption->SetBoneNode(pActorNode);
		m_pMiniHPBar->SetBoneNode(pActorNode);
	}

	SetCaptionOffset(fOffSet, m_pNameCaption, m_pGuildCaption, m_pMiniHPBar);
}

void XHeadIconPlacer::MakeNPCIconLocalMatrix( RMatrix& matOut, float fTitleHeight, vec3& vDirection )
{
	float fFontHeight = GetFontHeight();

	float fNPCIconOffset = fTitleHeight + NPC_ICON_MARGIN;

	if (IsCaptionVisible(m_pGuildCaption))
	{
		NextOffset(fNPCIconOffset, fFontHeight);
	}

	if (IsCaptionVisible(m_pNameCaption))
	{
		NextOffset(fNPCIconOffset, fFontHeight);
	}

	matOut.SetLocalMatrix(vec3(0,0,fNPCIconOffset), vDirection, vec3::AXISZ);
}

bool XHeadIconPlacer::MakeIndicatorLocalMatrix( RMatrix& matOut, XObject* pOwner )
{
	if (pOwner->GetEntityType() == ETID_NPC)
	{
		XNonPlayer* pOwnerNPC	= AsNPC(pOwner);
		XNPCInfo* pNPCInfo		= pOwnerNPC->GetInfo(true);

		// 죽은상태나 넉백상태일때는 아래 코드로
		if (pOwnerNPC->IsDead() == false)
		{
			XModuleMovable* pModuleMovable = pOwnerNPC->GetModuleMovable();
			if(pModuleMovable &&
				pModuleMovable->GetMovableFactor().KNOCK_BACK.m_bKnockback == false)
			{
				MakeNPCIconLocalMatrix(matOut, pNPCInfo->fTitleHeight, pOwner->GetDirection());
				if (IsHeadIcon())
				{
					RMatrix mattemp;
					mattemp.MakeZero();
					mattemp.SetTranslation(vec3( 0.f, 0.f, 50.f));

					matOut = matOut + mattemp;
				}

				if (FindTitleBone(pOwner))
				{
					return true;
				}
				else if(pNPCInfo->fTitleHeight > 0.f)
				{
					return false;
				}
			}
		}
	}
	else if (pOwner->GetEntityType() == ETID_PLAYER)
	{
		MakeNPCIconLocalMatrix(matOut, PLAYER_ID_OFFSET_Z, pOwner->GetDirection());
		return true;
	}

	// 죽은상태나 넉백상태
	RBoundingBox InterAABB = pOwner->GetActor()->GetInteractionAABB();
	vec3 vCenter = InterAABB.GetCenter();
	if (vCenter <= vec3::ZERO)
	{
		vCenter = pOwner->GetActor()->GetAABB().GetCenter();

		RMatrix mat;
		pOwner->GetWorldTransform().GetInverse(&mat);
		vCenter = vCenter * mat;
	}
	else
	{
		if(pOwner->IsHuge())
		{
			vCenter.z = (InterAABB.maxz-InterAABB.minz) * 0.7f;
		}
	}

	matOut.MakeIdentity();
	matOut.SetTranslation(vCenter);

	return false;
}

float XHeadIconPlacer::GetFontHeight()
{
	float fFontHeight = 0.0f;

	XCaptionMgr* pCharacterCaptionMgr = global.ui->GetCaptionMgr();
	if(pCharacterCaptionMgr->m_pCaptionFont)
	{
		fFontHeight = pCharacterCaptionMgr->m_pCaptionFont->GetHeight() * CAPTION_CAP;
	}
	return fFontHeight;
}

bool XHeadIconPlacer::IsCaptionVisible( XCharacterCaption* pCaption )
{
	return (pCaption->GetVisible() && !pCaption->IsTextEmpty());
}

RActorNode* XHeadIconPlacer::FindTitleBone( XObject* pOwner )
{
	if (pOwner->GetEntityType() == ETID_NPC)
	{
		XNonPlayer* pOwnerNPC	= AsNPC(pOwner);
		XNPCInfo* pNPCInfo		= pOwnerNPC->GetInfo(true);

		if (m_pTitleBoneActorNode) return m_pTitleBoneActorNode;

		return pOwnerNPC->GetActor()->GetActorNode(MLocale::ConvUTF16ToAnsi(pNPCInfo->strTitleBone.c_str()).c_str());
	}

	return NULL;
}

void XHeadIconPlacer::Refresh( XObject* pOwner )
{
	if (pOwner->GetEntityType() == ETID_PLAYER)
	{
		UpdatePlayerNameCaption();
	}
	if (pOwner->GetEntityType() == ETID_NPC)
	{
		XNonPlayer* pOwnerNPC	= AsNPC(pOwner);
		XModuleEntity* pModuleEntity = pOwner->GetModuleEntity();
		XNPCInfo* pNPCInfo		= pOwnerNPC->GetInfo(true);

		UpdateNPCNameCaption(pOwnerNPC->GetActor(), pNPCInfo->strTitleBone, pNPCInfo->fTitleHeight, pModuleEntity->GetScale());

		if (m_uidNPCIConEffect != MUID::ZERO)
		{
			XEffect* pEffect = global.emgr->GetXEffect(m_uidNPCIConEffect);
			if (pEffect)
			{
				RMatrix matLocal;
				MakeNPCIconLocalMatrix(matLocal, pNPCInfo->fTitleHeight, pOwnerNPC->GetDirection());

				pEffect->GetEffect()->SetTransform(matLocal);
			}
			else
			{
				m_uidNPCIConEffect = MUID::ZERO;
			}
		}
	}
}

void XHeadIconPlacer::CheckRefresh( XObject* pOwner )
{
	if (m_uidNPCIConEffect.Low == 0) return;

	bool bNameCaptionVisible = IsCaptionVisible(m_pNameCaption);
	bool bGuildCaptionVisible = IsCaptionVisible(m_pGuildCaption);

	if (bNameCaptionVisible != m_bNameCaptionLastVisible ||
		bGuildCaptionVisible != m_bGuildCaptionLastVisible)
	{
		m_bNameCaptionLastVisible = bNameCaptionVisible;
		m_bGuildCaptionLastVisible = bGuildCaptionVisible;

		Refresh(pOwner);
	}
}

void XHeadIconPlacer::SetNPCIConEffectUID( MUID uidEffect )
{
	m_uidNPCIConEffect = uidEffect;
}