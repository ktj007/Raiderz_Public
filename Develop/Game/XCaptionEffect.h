#pragma once

#include "XDef.h"
#include "XModuleActorControl.h"
#include "XHeadEffect.h"
#include "XGameState.h"

class XObject;
class XDamageCaption;
class XExpCaption;

/// 대미지 등의 캐릭터에 나오는 글씨 이펙트를 만들어내는 클래스
class XCaptionEffect
{
private:
	bool IsMyPlayer(XObject* pOwner);
	XDamageCaption* NewDamageCaption();
	XDamageTextureCaption* NewDamageTextureCaption();
	XExpCaption* NewExpCaption();

	template< typename T >
	T*					NewCaption()
	{
		XCaptionMgr* pCaptionMgr = global.ui->GetCaptionMgr();

		RWorldSceneNode* pWorldSceneNode = NULL;
		if(gg.currentgamestate->GetWorld())
			pWorldSceneNode = gg.currentgamestate->GetWorld()->GetWorldSceneNode();
		else
			return NULL;

		const RSceneNode& rootnode = *pWorldSceneNode;
		RSceneNode* pRootNode = const_cast<RSceneNode*>(&rootnode);
		T* pNewCaption = pCaptionMgr->InsertCaption<T>(pRootNode);

		return pNewCaption;
	}

	vec3 GetHeadPos( XObject* pOwner );
	wstring GetDamageFlag( const uint32& nFlags, TALENT_RESIST_TYPE nResistType, bool bHitDodge );

	void SetupDamageCaption(XObject* pOwner, XDamageCaption* pNewCaption, _DamageInfo * pInfo, vec3 vDamagePos, wstring& strDamage);

public:
	XCaptionEffect() {}

	void ShowDamage( XObject* pOwner, _DamageInfo * pInfo, vec3 vDamagePos );
	void ShowHeal( XObject* pOwner, int nHeal, UIID nUserUIID = UIID_INVALID );
	void ShowHPRegen( XObject* pOwner, int nHeal );
	void ShowEn( XObject* pOwner, int nEn );
	void ShowExp( XObject* pOwner, int nXP );
	void ShowBuff( XObject* pOwner, const wchar_t* szName, bool bIsDebuff);
	void ShowGuardDefense(XObject* pOwner, int nDamage, GUARD_TYPE eGuardType);
	void ShowDebug( XObject* pOwner, const wchar_t* szMessage, uint32 nColor );


	wstring GetDamageString(_DamageInfo * pInfo);
};
