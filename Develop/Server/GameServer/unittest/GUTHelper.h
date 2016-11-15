#pragma once

#include "GTalentInfo.h"
#include "GNPCInfoMgr.h"
#include "GBuffInfo.h"
#include "MockPlayerObject.h"
#include "SUnitTestUtil.h"

struct LOOT_ITEM_INFO;
struct LOOT_MONEY_INFO;
struct LOOT_SOUL_INFO;

class MockField;
class GConditionsInfo;
class GCondition_Leaf;
class GCraftInfo;
class GRecipeInfo;
class GInnRoomInfo;
class GGuild;
class MockLink;
class MovableMockEntityNPC;
class GSoulBindingInfo;

class GSharedField;

class GQuestInfo;
class CSQObjectiveInfo;
class GPlayerQuest;
class GLootInfo;
class GDialogInfo;
class GDialogSayInfo;
class GDialogSelectInfo;
class GNPCInteractionElementInfo;
class GFactionInfo;
class GFactionRelationInfo;

enum INTERACTION_TYPE;
enum COLT_CHECK_TYPE;
enum COLT_ACTION_TYPE;
enum FACTION_RELATION_TYPE;


/// À¯´Ö Å×½ºÆ® ÇïÆÛ
class GUTHelper
{
private:

public:
	// net ----------------
	static MockLink* NewLink(GEntityPlayer* pPlayer, bool bIgnoreAllSendCommandID = false);
	static MockLink* NewLink( MUID uidLink, bool bIgnoreAllSendCommandID = false );

	static void ClearLinks();

	// talent -------------
	void SetMeleeTalentInfo(GTalentInfo* pTalentInfo, int nID);
	void SetPassiveTalentInfo(GTalentInfo* pTalentInfo, int nID);
	void SetArcheryTalentInfo(GTalentInfo* pTalentInfo, int nID);
	void SetMagicTalentInfo(GTalentInfo* pTalentInfo, int nID);
	void SetMagicMissileTalentInfo(GTalentInfo* pTalentInfo, int nID);
	void SetMagicTargetTalentInfo(GTalentInfo* pTalentInfo, int nID);
	void SetMagicAreaTalentInfo( GTalentInfo* pTalentInfo, int nID );
	void SetExtraPassiveTalentInfo(GTalentInfo* pTalentInfo, int nID, TALENT_EXTRA_PASSIVE_TYPE nExtraPassiveType);
	void SetExtraActiveTalentInfo(GTalentInfo* pTalentInfo, int nID, TALENT_EXTRA_ACTIVE_TYPE nExtraActiveType);
	void SetBuffTalentInfo(GTalentInfo* pTalentInfo, int nID);
	void SetBuffInfo(GBuffInfo* pInfo, int nID);
	void AddTalentEventInfo(GTalentInfo* pTalentInfo, TALENT_EVENT nEvent, float fTime);
	void AddTalentActEventInfo(GTalentInfo* pTalentInfo, TALENT_EVENT nEvent, float fTime);
	GTalentInfo* NewTalentInfo(int nID=0, int nTalentLine=0, int8 nRank=0);
	GTalentInfo* NewMeleeTalentInfo(int nID=0);
	GTalentInfo* NewPassiveTalentInfo(int nID);
	GTalentInfo* NewGuardTalentInfo(int nID=0);
	GTalentInfo* NewArcheryTalentInfo(int nID);
	GTalentInfo* NewBuffTalentInfo(int nID);
	GTalentInfo* NewMagicTalentInfo(int nID=0);
	GTalentInfo* NewItemTalentInfo(int nID = 0);
	GBuffInfo* NewBuffInfo(int nID = 0);
	void SetTalentDamage(GTalentInfo* pTalentInfo, int nDamage);
	void AddSimpleHitInfo( GTalentInfo* pTalentInfo, float fCheckTime=0.0f);
	void AddHitInfo( GTalentInfo* pTalentInfo, vec3 vCapsuleBottomPos, vec3 vCapsuleTopPos, float fCapsuleRadius, float fCheckTime=0.0f );
	void InsertTalentInfoToMgr(GTalentInfo* pTalentInfo);
	void InsertBuffInfoToMgr(GBuffInfo* pInfo);
	void NewTalentAndBuff( int nTalentID, int nBuffID, GTalentInfo*& pTalentinfo, GBuffInfo*& pBuffInfo );
	GTalentInfo* MakeSimpleTalent(int nDamage, int nTalentID = 1);

	// player --------------
	static GEntityPlayer* NewEntityPlayer(GField* pField=NULL, vec3 pos=vec3(0,0,0), MUID uidNew=MUID(0,0), MARKER_INFO markerExit=MARKER_INFO());
	static void InitEntityPlayer(GEntityPlayer* pNewEntityPlayer, MUID &uidNew, MARKER_INFO &markerExit, GField* pField, vec3 pos);

	static GEntityPlayer*	NewEntityPlayerForGameData(GField* pField=NULL, vec3 pos=vec3(0,0,0), MUID uidNew=MUID(0,0), MARKER_INFO markerExit=MARKER_INFO());
	MockPlayerObject* NewMockPlayerObject(GField* pField, vec3 pos=vec3(0,0,0), MUID uidNew=MUID(0,0));
	void			SetEntityPlayerStatus(GEntityPlayer* pEntityPlayer, int nHP, int nEN, int nSTA);


	// npc -----------------
	GNPCInfo* NewNPCInfo(int nNPCId = 0);	
	GEntityNPC* NewEntityNPC(GField* pField, vec3 pos=vec3(0,0,0), int nNPCId=0);
	void SetSimpleActorMeshInfoHit(GActorMeshInfo* pActorMeshInfo, vec3 vCapsuleBottom, vec3 vCapsuleTop, float fRadius);

	// item ----------------
	GItemData* NewItemData(int nID=INVALID_ID);
	GItemData* NewItemData(SH_ITEM_SLOT nSlot, int nID=INVALID_ID);
	GItem* MakeItem(int nItemID = INVALID_ID, int nAmount=1);
	GItem* GiveNewItem(GEntityPlayer* pPlayer, int nItemID=INVALID_ID, int nItemQuantity=1);
	GItem* EquipNewItem(GEntityPlayer* pPlayer, int nItemID=INVALID_ID, SH_ITEM_SLOT nItemSlot = ITEMSLOT_NONE);
	void EquipItem( GEntityPlayer* pPlayer, GItem* pItem, SH_ITEM_SLOT nSlot );

	// condition ------------
	GConditionsInfo* NewConditionsInfo(int nID=INVALID_ID);
	GConditionsInfo* NewConditionsInfo_Item();
	GConditionsInfo* NewConditionsInfoForLevel(int nMinLevel=0, int nMaxLevel=0);

	GCondition_Leaf* NewConditionLeaf(GConditionsInfo* pConditionsInfo);
	GCondition_Leaf* NewConditionInfoForLevel(GConditionsInfo* pConditionsInfo, int nMinLevel=0, int nMaxLevel=0);

	// craft ----------------
	GCraftInfo* NewCraftInfo();

	// recipe ---------------
	GRecipeInfo* NewRecipeInfo();
	
	// inn -------------
	GInnRoomInfo* NewInnRoomInfo();

	// colt -----------
	GColtCheck* GiveNewCheckInfo(GEntityNPC* pNPC, COLT_CHECK_TYPE nType, int nParam1=INT_MIN, int nParam2=INT_MIN, int nParam3=INT_MIN, bool bInsertDummyAction=true);
	GColtCheck* NewCheckInfo(COLT_CHECK_TYPE nType, int nParam1=INT_MIN, int nParam2=INT_MIN, int nParam3=INT_MIN);

	GColtAction* NewColtActionInfo(COLT_ACTION_TYPE nType, vector<wstring> qParams=vector<wstring>(), int nRate=100, int nMaxCount=INT_MAX);

	// lua ------------
	bool RunLuaString(const char* szFormat, ...);

	// Guild -------------
	GGuild* NewGuild(GEntityPlayer* pPlayer, const wchar_t* szGuildName, int nMoney=0, GEntityPlayer* pGuest=NULL);	

	// Training ----------
	GTalentInfo* LearnNewTalent(GEntityPlayer* pPlayer, int nLine=0, int nRank=0);

	// SoulBinding -------
	GSoulBindingInfo* NewSoulBindingInfo(MARKER_INFO* pMarkerInfo);
	GSoulBindingInfo* NewDefaultSoulBindingInfo(MARKER_INFO* pMarkerInfo);

	// ForTS2 -----------------
	void EquipSword_ForTS2(GEntityPlayer* pEntityPlayer);
	bool LearnTalent_ForTS2(GEntityPlayer* pEntityPlayer, int nTalentID);		
};