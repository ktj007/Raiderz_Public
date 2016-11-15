#pragma once

#include "CSCommonLib.h"
#include "CSModifier.h"

class CSCOMMON_API CSModEffector
{
public:
	// 액터용 모디파이어(영구속성)를 적용 
	void AddActorModifier(CSActorModifier& actorModifier, const CSActorModifier& Modifier);
	// 액터용 모디파이어(영구속성)를 제거
	void EraseActorModifier(CSActorModifier& actorModifier, const CSActorModifier& Modifier);
	// 플레이어용 모디파이어(영구속성)를 적용
	void AddPlayerModifier(CSPlayerModifier& playerModifier, const CSPlayerModifier& Modifier);
	// 플레이어용 모디파이어(영구속성)를 제거
	void ErasePlayerModifier(CSPlayerModifier& playerModifier, const CSPlayerModifier& Modifier);
};
