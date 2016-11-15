#include "stdafx.h"
#include "XCharacterSerializeStrategy.h"
#include "XCharacter.h"
#include "XNaming.h"
#include "XNetPlayer.h"
#include "XNPCInfo.h"
#include "XItemManager.h"
#include "XModelAnimationInfo.h"

#include "XFieldInfo.h"

XCharacterSerializeStrategy::XCharacterSerializeStrategy( XCharacter* pActor, bool bResourceLoading )
: m_pActor(pActor), m_nLoadingPolicy(LOADING_ASYNC), m_bLoadingCompleted(false), m_bResourceLoading(bResourceLoading)
{

}

void XCharacterSerializeStrategy::_SetMesh( const wstring& strMeshName, float fScale, XCharacterSerializeStrategyType eType  )
{
	if ( m_pActor->m_pMesh ) return;

	if (m_Feature.strMeshName == strMeshName) return;

	m_pActor->SetUsable(false);
	m_pActor->SetScale(vec3(fScale,fScale,fScale));

	if (m_bResourceLoading)
	{
		// 백그라운드 여부
		bool bBackgroundCreation = (m_nLoadingPolicy == LOADING_ASYNC);

		// PreLoading이면 백그라운드 로딩 하지 않는다.
		if ( info.field && info.field->IsFieldPreLoadResource(strMeshName))
		{
			//mlog("%s mesh preloading\n", MLocale::ConvUTF16ToAnsi(strMeshName).c_str());
			bBackgroundCreation = false;
		}

		if(info.model_info)
			m_pActor->SetAnimationInfoMgr(info.model_info->GetAnimationInfoMgr(strMeshName));		// 애니메이션 정보 셋팅

		bool bRet = m_pActor->Create(strMeshName.c_str(), bBackgroundCreation);
		if( bRet == false )
		{
			dlog("메쉬 로딩 실패(%s)\n", MLocale::ConvUTF16ToAnsi(strMeshName.c_str()).c_str());
			return;
		}

		if( (eType == CSST_MY_PLAYER) && bBackgroundCreation )
		{
			// 우선순위 높여주기
			m_pActor->m_nBGWKPriorityOffset = RBGWK_HIGH_OFFSET;
		}
		else
		{
			m_pActor->m_nBGWKPriorityOffset = RBGWK_HIGH_OFFSET;
			//			_ASSERT( m_pActor->GetUsable() == true );
		}
	}
	else
	{
		m_pActor->SetAnimationForMock();
	}

	m_Feature.strMeshName = strMeshName;
}

void XCharacterSerializeStrategy::CreatePartsNodeAsync( XCreatePartsNodeParam& p )
{
	XPartsParam partsParam;

	partsParam.Make(p.nPartsID, p.strItemMeshName.c_str(), p.pItemData);

	wstring strParentName;
	if (p.bWeaponOrShield && p.bLeftWeapon)
	{
		strParentName = p.pItemData->m_strParentNameLeft;
	}
	else
	{
		strParentName = p.pItemData->m_strParentName;
	}

	for (size_t i = 0; i < p.pItemData->m_strNodeNames.size(); i++)
	{
		if (p.pItemData == NULL) continue;

		unsigned int nColor = 0xFFFFFFFF;

		if (XNaming::IsSkinNodeName(p.pItemData->m_strNodeNames[i].c_str()))
		{
			nColor = m_Feature.nSkinColor;
		}
		else if (XNaming::IsHairNodeName(p.pItemData->m_strNodeNames[i].c_str()))
		{
			nColor = m_Feature.nHairColor;
		}
		else
		{
			nColor = p.pItemData->m_nTexColor;

			if(p.nDyedColor > 0)
				nColor = p.nDyedColor;
		}

		wstring strActorNodeName = p.pItemData->m_strNodeNames[i];
		if (p.bWeaponOrShield && p.bLeftWeapon)
		{
			strActorNodeName = strActorNodeName + L"_left";
		}

		partsParam.AddPart(p.pItemData->m_strNodeNames[i], strParentName, strActorNodeName, nColor);

		if (p.bWeaponOrShield)
		{
			// weapon_body
			partsParam.AddPart(p.pItemData->m_strNodeNames[i], (strParentName+L"_b"), (strActorNodeName+L"_b"), nColor);

			// weapon_world
			partsParam.AddPart(p.pItemData->m_strNodeNames[i], (strParentName+L"_w"), (strActorNodeName+L"_w"), nColor);

			// weapon_left
			partsParam.AddPart(p.pItemData->m_strNodeNames[i], (strParentName+L"_c"), (strActorNodeName+L"_c"), nColor);
		}
	}

	if (p.pItemData->m_ItemType == ITEMTYPE_WEAPON)
	{
		// swordtrail_start
		partsParam.AddPart(wstring(SWORDTRAIL_START_NODE_NAME), strParentName, wstring());

		// swordtrail_end
		partsParam.AddPart(wstring(SWORDTRAIL_END_NODE_NAME), strParentName, wstring());
		
		// dummy effect
		for(int iEffect = 0; iEffect < WEAPON_EFFECT_DUMMY_MAX_COUNT; ++iEffect)
		{
			TCHAR chEffectName[256] = {0,};
			if(p.bLeftWeapon)
				wsprintf(chEffectName, L"%s%02d", WEAPON_LEFT_EFFECT_DUMMY_NAME, iEffect);
			else
				wsprintf(chEffectName, L"%s%02d", WEAPON_EFFECT_DUMMY_NAME, iEffect);

			partsParam.AddPart(wstring(chEffectName), strParentName, wstring());
		}
	}
	else if(p.bWeaponOrShield)
	{
		// 
		partsParam.AddPart(wstring(SHIELD_EFFECT_DUMMY_NAME), strParentName, wstring());
	}
	else if(p.bEye)
	{
		partsParam.AddPart(wstring(XNaming::EyeNodeName()), wstring(L""), wstring(L""));
	}


	m_pActor->CreatePartsNodeAsync( &partsParam );
}


