#pragma once

#include "MUID.h"
#include "MTypes.h"
#include "CSChrInfo.h"

class GField;

class GUTHelper_Actor
{
public:
	// 목 액터를 초기화 시키고 필드에 입장시킴
	template<typename ACTOR_TYPE>
	void Init_ForActor(GField* pField, ACTOR_TYPE*& pActor, vec3 pos=vec3::ZERO, MUID uidNew=MUID::Invalid());

private:
	// 액터의 기본 캐릭터정보를 반환함, 액터는 캐릭터정보가 널이다.
	CHR_INFO* DefaultActorCharInfo();
};






//////////////////////////////////////////////////////////////////////////
//
//	Template Implements
//
//////////////////////////////////////////////////////////////////////////

template<typename ACTOR_TYPE>
void GUTHelper_Actor::Init_ForActor( GField* pField, ACTOR_TYPE*& pActor, vec3 pos/*=vec3::ZERO*/, MUID uidNew/*=MUID::Invalid()*/ )
{
	VALID(pField);
	pActor = new ACTOR_TYPE();
	pActor->Create(SUnitTestUtil::NewUID(uidNew));
	pActor->EnterToField(pField, pos);
	if (pActor->GetChrInfo())
	{
		pActor->GetChrInfo()->nMaxHP = 10000;
		pActor->GetChrInfo()->nMaxEN = 10000;
		pActor->GetChrInfo()->nMaxSTA = 10000;
		pActor->GetChrInfo()->fSpeed = 100;
	}
	else
	{
		pActor->SetChrInfo(DefaultActorCharInfo());
	}

	pActor->SetHP(pActor->GetMaxHP());
	pActor->SetEN(pActor->GetMaxEN());
	pActor->SetSTA(pActor->GetMaxSTA());
	pActor->SetDir(vec3(0,1,0));
	pActor->SetRegenActive(false);
}
