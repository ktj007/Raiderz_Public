#include "stdafx.h"
#include "GNPCInfoHelper.h"

SUITE(NPCInfoHelper)
{
	TEST(NPCInfoHelper_IsThinkableObject)
	{
		GNPCInfo info;

		// m_bObjectThinkable는 OBJECT 타입만 검사한다.
		info.nNpcType = NPC_TYPE_OBJECT; info.m_bObjectThinkable = true;
		CHECK_EQUAL(true, GNPCInfoHelper::IsThinkableObject(&info));

		info.nNpcType = NPC_TYPE_OBJECT; info.m_bObjectThinkable = false;
		CHECK_EQUAL(false, GNPCInfoHelper::IsThinkableObject(&info));

		// 채집 오브젝트는 무조건 false
		info.nNpcType = NPC_TYPE_GATHER; info.m_bObjectThinkable = false;
		CHECK_EQUAL(false, GNPCInfoHelper::IsThinkableObject(&info));

		info.nNpcType = NPC_TYPE_GATHER; info.m_bObjectThinkable = true;
		CHECK_EQUAL(false, GNPCInfoHelper::IsThinkableObject(&info));

		// npc, monster는 무조건 true
		info.nNpcType = NPC_TYPE_NPC; info.m_bObjectThinkable = false;
		CHECK_EQUAL(true, GNPCInfoHelper::IsThinkableObject(&info));

		info.nNpcType = NPC_TYPE_MONSTER; info.m_bObjectThinkable = false;
		CHECK_EQUAL(true, GNPCInfoHelper::IsThinkableObject(&info));

	}

	TEST(NPCInfoHelper_IsUsingGravityNPC)
	{
		CHECK_EQUAL(true, GNPCInfoHelper::IsUsingGravityNPC(NPC_TYPE_MONSTER));
		CHECK_EQUAL(true, GNPCInfoHelper::IsUsingGravityNPC(NPC_TYPE_NPC));
	}

	TEST(NPCInfoHelper_IsNotUsingGravityNPC)
	{
		CHECK_EQUAL(false, GNPCInfoHelper::IsUsingGravityNPC(NPC_TYPE_OBJECT));
		CHECK_EQUAL(false, GNPCInfoHelper::IsUsingGravityNPC(NPC_TYPE_GATHER));

	}

}