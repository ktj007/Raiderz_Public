#pragma once

#include "GConst.h"
#include "GDef.h"
#include "MockField.h"
#include "MockLink.h"
#include "CCommandTable.h"
#include "GTestWrapper.h"
#include "GUnitTestUtil.h"
#include "TestCallCounter.h"
#include "TestCombatCalculator.h"

#include "GUTHelper.h"
#include "GUTHelper_Item.h"
#include "GUTHelper_Field.h"
#include "GUTHelper_Actor.h"
#include "GUTHelper_NPC.h"
#include "GUTHelper_Network.h"
#include "GUTHelper_Talent.h"
#include "GUTHelper_Focus.h"
#include "GUTHelper_Buff.h"
#include "GUTHelper_Party.h"
#include "GUTHelper_Player.h"
#include "GUTHelper_Quest.h"
#include "GUTHelper_Interaction.h"
#include "GUTHelper_Dialog.h"
#include "GUTHelper_Etc.h"
#include "GUTHelper_Condition.h"
#include "GUTHelper_Faction.h"
#include "GUTHelper_NPCShop.h"
#include "GUTHelper_Field.h"
#include "GUTHelper_Trade.h"
#include "GUTHelper_Duel.h"
#include "GUTHelper_Loot.h"
#include "GUTHelper_QuestPVP.h"

#include "GTalentInfoMgr.h"
#include "GFactionInfoMgr.h"
#include "GFactionRelationInfoMgr.h"
#include "GQuestInfoMgr.h"
#include "GFieldInfoMgr.h"
#include "GFieldMgr.h"
#include "GNPCShopInfoMgr.h"
#include "GNPCMgr.h"
#include "GQPEventInfoMgr.h"
#include "GItemManager.h"


#define DECLWRAPPER_NPCInfoMgr			GTestMgrWrapper<GNPCInfoMgr>	NPCInfoMgr
#define DECLWRAPPER_PartyMgr			GTestMgrWrapper<GPartyManager>	PartyMgr
#define DECLWRAPPER_FieldInfoMgr		GTestMgrWrapper<GFieldInfoMgr>	FieldInfoMgr
#define DECLWRAPPER_FieldMgr			GTestMgrWrapper<GFieldMgr>		FieldMgr
#define DECLWRAPPER_ItemMgr				GTestMgrWrapper<GItemManager>	ItemMgr
#define DECLWRAPPER_BuffMgr				GTestMgrWrapper<GBuffInfoMgr>	BuffMgr
#define DECLWRAPPER_TalentMgr			GTestMgrWrapper<GTalentInfoMgr>	TalentMgr
#define DECLWRAPPER_SimpleCombatCalc	GTestSysWrapper2<GCombatCalculator, SimpleCombatCalculator>	CombatCalculator; \
										GTestSysWrapper2<GAttackDamageCalculator, SimpleAttackDamageCalculator>	m_AttackDamageCalculator
#define DECLWRAPPER_Field				GTestMgrWrapper<GFieldInfoMgr>				FieldInfoMgr; \
										GTestMgrWrapper<GFieldMgr> FieldMgr
#define DECLWRAPPER_CriticalCombatCalc	GTestSysWrapper2<GCombatCalculator, CombatCalculator_ForCritical>	CriticalCalculator
#define DECLWRAPPER_MissCombatCalc		GTestSysWrapper2<GCombatCalculator, CombatCalculator_ForMiss>	AvoidCalculator
#define DECLWRAPPER_GuardCombatCalc		GTestSysWrapper2<GCombatCalculator, CombatCalculator_ForGuard>	GuardCalculator
#define DECLWRAPPER_FakeSystem			GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem>  FakeSystemWrapper
#define DECLWRAPPER_FactionInfoMgr		GTestMgrWrapper<GFactionInfoMgr>	FactionInfoMgr
#define DECLWRAPPER_FactionRelationInfoMgr		GTestMgrWrapper<GFactionRelationInfoMgr>	FactionRelationInfoMgr
#define DECLWRAPPER_DialogInfoMgr		GTestMgrWrapper<GDialogInfoMgr>	DialogInfoMgr
#define DECLWRAPPER_ConditionsInfoMgr	GTestMgrWrapper<GConditionsInfoMgr>	ConditionsInfoMgr
#define DECLWRAPPER_NPCShopInfoMgr		GTestMgrWrapper<GNPCShopInfoMgr>	NPCShopInfoMgr
#define DECLWRAPPER_QuestInfoMgr		GTestMgrWrapper<GQuestInfoMgr>	QuestInfoMgr
#define DECLWRAPPER_ConditionsInfoMgr	GTestMgrWrapper<GConditionsInfoMgr>	ConditionsInfoMgr
#define DECLWRAPPER_FactionInfoMgr		GTestMgrWrapper<GFactionInfoMgr>	FactionInfoMgr
#define DECLWRAPPER_NPCMgr				GTestMgrWrapper<GNPCMgr>		NPCMgr
#define DECLWRAPPER_LootInfoMgr			GTestMgrWrapper<GLootInfoMgr>	LootInfoMgr
#define DECLWRAPPER_QPEventInfoMgr		GTestMgrWrapper<GQPEventInfoMgr>	QPEventInfoMgr

extern 
struct GTestHelpers
{
	GUTHelper_Item		item;
	GUTHelper_Actor		actor;
	GUTHelper_Player	player;
	GUTHelper_NPC		npc;
	GUTHelper_Network	network;
	GUTHelper_Talent	talent;
	GUTHelper_Focus		focus;
	GUTHelper_Buff		buff;
	GUTHelper_Party		party;
	GUTHelper_Quest		quest;
	GUTHelper_Interaction interaction;
	GUTHelper_Dialog	 dialog;
	GUTHelper_Condition condition;
	GUTHelper_Faction	faction;
	GUTHelper_NPCShop	npcshop;
	GUTHelper_Field		field;
	GUTHelper_Trade		trade;
	GUTHelper_Duel		duel;
	GUTHelper_Loot		loot;
	GUTHelper_QuestPVP	questpvp;

	GUTHelper_Etc		etc;
} test;

#define RUN_LIST(iterator_type, set, event)			\
	for each (iterator_type* pItem in set)			\
	pItem->event;									



//////////////////////////////////////////////////////////////////////////
//
// Util Functions
//
//////////////////////////////////////////////////////////////////////////
void MakeLevelFactor(int nLF, GEntityActor* pRequester, GEntityActor* pTarget);

class SuccessOnlyDice : public GPercentDice
{
	virtual bool Dice(float fPercent) override { return true; }
};

class FailOnlyDice : public GPercentDice
{
	virtual bool Dice(float fPercent) override { return false; }
};