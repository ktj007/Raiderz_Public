#pragma once

#include "GDef.h"

class MockField;
class GEntityNPC;
class GEntityActor;
class GNPCInfo;

class GUTHelper_NPC
{
public:
	// NPCInfo를 생성
	static GNPCInfo*	NewNPCInfo(int nNPCId=INVALID_ID);
	// 해당 목필드에 NPC를 스폰 시킨다.
	static GEntityNPC* SpawnNPC(MockField* pField, int nNPCId=INVALID_ID, const vec3& vPos=vec3::ZERO);
	// 해당 목필드에 MockNPC를 스폰 시킨다.
	template<typename T>
	static T* NewMockNPC(MockField* pField, int nNPCId=INVALID_ID, const vec3& vPos=vec3::ZERO)
	{
		GNPCInfo* pNPCInfo = NewNPCInfo(nNPCId);
		return pField->NewMockNPC<T>(pNPCInfo, vPos);
	}
	// 목표 액터가 전투 상태를 만듬
	static void MakeCombat(GEntityNPC* pNPC, GEntityActor* pOpponent);
	static void MakeCombat(GEntityNPC* pNPC);
	static GNPCInfo*	NewNPCInfo_DeulFlag();
};
