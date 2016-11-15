#include "stdafx.h"
#include "XUnitTestHelper.h"
#include "XGame.h"
#include "MockTalentInfo.h"
#include "MockAnimationController.h"
#include "MockItemInfo.h"
#include "MockMeshInfo.h"
#include "MockCharacter.h"
#include "XMyPlayer.h"
#include "XNetPlayer.h"
#include "XNonPlayer.h"
#include "XModuleEntity.h"
#include "XSystem.h"
#include "XMotion.h"
#include "XUnitTestUtil.h"
#include "XModuleCollision.h"
#include "CSFormatString.h"
#include "XStrings.h"


XNonPlayer* XUnitTestHelper::InNPC( MUID uidNPC, int nNPCID, vec3 vPos, TestAniDataSet* pAniDataSet, MCapsule* pColCapsule, const wchar_t* szMeshName )
{
	TD_UPDATE_CACHE_NPC td_update_cache_npc;
	td_update_cache_npc.uid = uidNPC;
	td_update_cache_npc.nUIID = XUnitTestUtil::NewUIID();
	td_update_cache_npc.nNPCID = nNPCID;
	td_update_cache_npc.vPos = vPos;

	gg.currentgamestate->InNPCs(&td_update_cache_npc, 1);

	XNonPlayer* pNPC = gg.omgr->FindNPC(uidNPC);
	if (pNPC && pAniDataSet)
	{
		MockAnimationController* ani_controller = dynamic_cast<MockAnimationController*>(pNPC->GetModuleEntity()->GetAnimationController());
		if (ani_controller == NULL) return pNPC;

		ani_controller->SetAniDataSet(pAniDataSet);
	}

	MCapsule capCol(vec3(0.0f, 0.0f, 40.0f), vec3(0.0f, 0.0f, 150.0f), 40.0f);
	if (pColCapsule) capCol = *pColCapsule;

	if (pNPC)
	{
		wchar_t colName[64];
		if (szMeshName) wcscpy_s(colName, szMeshName);
		else swprintf_s(colName, L"NPC%u", uidNPC.Low);

		// 메쉬 충돌 정보 입력
		XUnitTestHelper::SetMeshColCapsule(colName, capCol);

		MockCharacter* pMockCharacter = dynamic_cast<MockCharacter*>(pNPC->GetActor());
		if (pMockCharacter)
		{
			pMockCharacter->CreateMeshForCol(colName);
		}

		// 애니메이션 업데이트 완료로 처리
		pNPC->GetActor()->SetAnimationUpdated(true);

		// MeshInfo 다시 설정
		CSMeshInfo* pMeshInfo = info.mesh_info->GetInfo(colName);
		pNPC->GetModuleCollision()->InitCol(pMeshInfo);
	}

	return pNPC;
}

void XUnitTestHelper::InNPCs( TD_UPDATE_CACHE_NPC* pNpcNode, int nNPCCount, TestAniDataSet* pAniDataSet )
{
	for (int i = 0; i < nNPCCount; i++)
	{
		gg.currentgamestate->InNPCs(&pNpcNode[i], 1);

		XNonPlayer* pNPC = gg.omgr->FindNPC(pNpcNode[i].uid);
		if (pNPC && pAniDataSet)
		{
			pNPC->GetActor()->SetAnimationUpdated(true);
			MockAnimationController* ani_controller = dynamic_cast<MockAnimationController*>(pNPC->GetModuleEntity()->GetAnimationController());
			if (ani_controller == NULL) return;

			ani_controller->SetAniDataSet(pAniDataSet);
		}
	}
}

XTalentInfo* XUnitTestHelper::CreateMeleeTalentInfo( int nTalentID )
{
	MockTalentInfoMgr* pMgr = dynamic_cast<MockTalentInfoMgr*>(info.talent);
	if (pMgr == NULL) return NULL;

	XTalentInfo* pNewTalentInfo = new XTalentInfo();

	pNewTalentInfo->m_nID = nTalentID;

	pMgr->SetTalent(pNewTalentInfo, L"<TALENT name=\"Sample\" TalentType=\"skill\" Category=\"melee\"							\
		SkillType=\"melee\" EffectRadius=\"3\" HitTestType=\"area\" HitMotionDelay=\"0\" HPCost=\"0\" APCost=\"0\" ENCost=\"0\"		\
		STACost=\"10\" HPTickCost=\"0\" ENTickCost=\"0\" STATickCost=\"0\" CoolTime=\"0\" Stance=\"all\" UseAni=\"PS_dodge_back\"	\
		AttackDirection=\"front\" InvincibleTimeStart=\"0.2\" InvincibleTimeEnd=\"0.5\" MF_BEATEN=\"0\" MF_WEIGHT_BEATEN=\"0\"		\
		MF_KNOCKBACK=\"0\" MF_WEIGHT_KNOCKBACK=\"0\" MF_STUN=\"0\" MF_WEIGHT_STUN=\"0\" MF_DEFLECT=\"0\" MF_WEIGHT_DEFLECT=\"0\"	\
		MF_THROWUP=\"0\" MF_WEIGHT_THROWUP=\"0\" MF_KNOCKDOWN=\"0\" MF_WEIGHT_KNOCKDOWN=\"0\"	\
		MF_UPPERED=\"0\" MF_WEIGHT_UPPERED=\"0\" MinDamage=\"0\" MaxDamage=\"0\" DamageAttrib=\"slash\" Guardable=\"True\"			\
		Cancelable=\"True\" NeededTP=\"0\" />");

	return pNewTalentInfo;
}

void XUnitTestHelper::CreateGuardTalentInfo( int nTalentID )
{
	MockTalentInfoMgr* pMgr = dynamic_cast<MockTalentInfoMgr*>(info.talent);
	if (pMgr == NULL) return;

	XTalentInfo* pNewTalentInfo = new XTalentInfo();

	pNewTalentInfo->m_nID = nTalentID;
	pNewTalentInfo->m_nSkillType = ST_GUARD;
	pNewTalentInfo->m_nTiming = TC_GUARD_ME;
	pNewTalentInfo->m_nGuardDirection = GUARD_DIRECTION_ALL;
	pNewTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
	pNewTalentInfo->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;

	pMgr->Insert(pNewTalentInfo);
	pMgr->Cooking();
}

void XUnitTestHelper::CreateMagicAreaTalentInfo( int nTalentID )
{
	MockTalentInfoMgr* pMgr = dynamic_cast<MockTalentInfoMgr*>(info.talent);
	if (pMgr == NULL) return;

	XTalentInfo* pNewTalentInfo = new XTalentInfo();

	pNewTalentInfo->m_nID = nTalentID;

	pMgr->SetTalent(pNewTalentInfo, L"<TALENT name=\"sample\" TalentType=\"skill\" Category=\"magic\" SkillType=\"magic\"		\
									EffectTarget=\"area_enemy\" EffectRadius=\"2\" HitTestType=\"area\" SeparateHit=\"True\"	\
									TargetsAllowed=\"enemy\" CoolTime=\"5\" PeriodTime=\"1\" DurationTime=\"10\"				\
									UseAni=\"atk_special\" AttackDirection=\"front\" MF_BEATEN=\"30\" MF_WEIGHT_BEATEN=\"2000\"	\
									MF_KNOCKBACK=\"40\" MF_WEIGHT_KNOCKBACK=\"500\" MF_STUN=\"30\" MF_WEIGHT_STUN=\"5000\"		\
									MinDamage=\"15\" MaxDamage=\"20\" DamageAttrib=\"blunt\" Guardable=\"True\" Cancelable=\"False\" />" );
}

void XUnitTestHelper::CreateProjectileMagicTalentInfo( int nTalentID )
{
	MockTalentInfoMgr* pMgr = dynamic_cast<MockTalentInfoMgr*>(info.talent);
	if (pMgr == NULL) return;

	XTalentInfo* pNewTalentInfo = new XTalentInfo();

	pNewTalentInfo->m_nID = nTalentID;

	pMgr->SetTalent(pNewTalentInfo, L"<TALENT name=\"sample\" TalentType=\"skill\" Category=\"magic\" SkillType=\"magic\"		\
									EffectTarget=\"victim\" HitTestType=\"target\" TargetingType=\"sticky\" \
									EffectPoint=\"target\"																	\
									TargetsAllowed=\"enemy\" ENCost=\"29\" CoolTime=\"1.6\"	Stance=\"all\" 						\
									CastingLoopAni=\"spell_loop1\" CastingTime=\"1.6\" UseAni=\"spell_fire1\"					\
									AMF_KNOCKBACK=\"100\" MF_WEIGHT_KNOCKBACK=\"80\" MinDamage=\"80\" MaxDamage=\"90\"			\
									ResistMethod=\"partial\" ResistType=\"FR\" ResistDifficulty=\"190\" CriticalEnable=\"True\"	\
									DamageAttrib=\"fire\" Guardable=\"False\" Cancelable=\"True\" Hate=\"80\"  Range=\"30\" />" );

	// 탤런트 이벤트 셋팅
	pMgr->SetTalentExt(pNewTalentInfo, L"    <TALENT time=\"2.700000\">												\
											<Events>																			\
											<Normal />																			\
											<Act>																				\
											<Event type=\"projectile\" time=\"0.410000\" time2=\"-1.000000\" Follow=\"true\"	\
											ProjectileType=\"guided\" Range=\"30.000000\" ProjectileHitRadius=\"30.000000\"		\
											ProjectileSpeed=\"2200\" start_x=\"9.858228\" start_y=\"-101.595467\" start_z=\"121.591148\" \
											BoneName=\"Bip01 R Finger1\"/>														\
											</Act>																				\
											</Events>																			\
											</TALENT> " );
}

void XUnitTestHelper::SetAniDataSet_NPCSample1( TestAniDataSet* pAniDataSet )
{
	pAniDataSet->AddAni(L"idle", 2, true);
	pAniDataSet->AddAni(L"run", 0.8, true);
	pAniDataSet->AddAni(L"walk", 1.7, true);
	pAniDataSet->AddAni(L"die", 1.77, false);
	pAniDataSet->AddAni(L"MF_BT1", 1, false);
	pAniDataSet->AddAni(L"MF_BT2", 1, false);
	pAniDataSet->AddAni(L"MF_BT3", 1, false);
	pAniDataSet->AddAni(L"MF_BT4", 1, false);
	pAniDataSet->AddAni(L"MF_KB1", 0.27, false);
	pAniDataSet->AddAni(L"MF_KB2", 1, false);
	pAniDataSet->AddAni(L"MF_KB3", 1, false);
	pAniDataSet->AddAni(L"MF_KD1", 1.77, false);
	pAniDataSet->AddAni(L"MF_KD2", 1.67, true);
	pAniDataSet->AddAni(L"MF_KD3", 2, false);
	pAniDataSet->AddAni(L"MF_sleep", 2, true);
	pAniDataSet->AddAni(L"MF_STUN", 1.17, true);
	pAniDataSet->AddAni(L"MF_TU1", 0.67, false);
	pAniDataSet->AddAni(L"MF_TU2", 1.13, false);
	pAniDataSet->AddAni(L"MF_UP", 1.13, false);
}

void XUnitTestHelper::SetAniDataSet_Player( TestAniDataSet* pAniDataSet )
{
	pAniDataSet->AddAni(L"none_idle", 2, true);
	pAniDataSet->AddAni(L"none_run_f", 0.8, true);
	pAniDataSet->AddAni(L"none_run_b", 0.87, true);
	pAniDataSet->AddAni(L"none_run_l", 0.8, true);
	pAniDataSet->AddAni(L"none_run_r", 0.8, true);

	pAniDataSet->AddAni(L"none_jump_up", 0.33, false);
	pAniDataSet->AddAni(L"none_jump_down", 1.5, false);
	pAniDataSet->AddAni(L"none_jump_updown", 2.33, false);
	pAniDataSet->AddAni(L"none_jump_land", 0.23, false);

	pAniDataSet->AddAni(L"none_die", 2.1, false);
	pAniDataSet->AddAni(L"none_spell_cast1", 2.17, false);
	pAniDataSet->AddAni(L"none_spell_cast2", 0.5, false);
	pAniDataSet->AddAni(L"none_spell_cast3", 1.6, false);

	pAniDataSet->AddAni(L"none_spell_fire1", 1.1, false);
	pAniDataSet->AddAni(L"none_spell_fire2", 1.3, false);
	pAniDataSet->AddAni(L"none_spell_fire3", 2.3, false);

	pAniDataSet->AddAni(L"none_spell_loop1", 1, true);
	pAniDataSet->AddAni(L"none_spell_loop2", 1, true);
	pAniDataSet->AddAni(L"none_spell_loop3", 2, true);

	pAniDataSet->AddAni(L"none_spell_use1", 1.1, false);
	pAniDataSet->AddAni(L"none_spell_use2", 1.3, false);
	pAniDataSet->AddAni(L"none_spell_instant", 1.67, false);

	pAniDataSet->AddAni(L"none_dodge_b", 1.6, false);
	pAniDataSet->AddAni(L"none_dodge_r", 1.6, false);
	pAniDataSet->AddAni(L"none_dodge_l", 1.6, false);
	pAniDataSet->AddAni(L"none_sleep", 2, true);

	pAniDataSet->AddAni(L"none_loot_end", 0.3, false);
	pAniDataSet->AddAni(L"none_loot_start", 0.3, false);
	pAniDataSet->AddAni(L"none_swim_b", 1.7, true);
	pAniDataSet->AddAni(L"none_swim_f", 1.7, true);
	pAniDataSet->AddAni(L"none_swim_idle", 2, true);
	pAniDataSet->AddAni(L"none_swim_l", 1.3, true);
	pAniDataSet->AddAni(L"none_swim_r", 1.3, true);
	pAniDataSet->AddAni(L"none_walk", 1, true);

	pAniDataSet->AddAni(L"none_MF_BT4", 1, false);
	pAniDataSet->AddAni(L"none_MF_BT3", 1, false);
	pAniDataSet->AddAni(L"none_MF_BT2", 1, false);
	pAniDataSet->AddAni(L"none_MF_BT1", 1, false);
	pAniDataSet->AddAni(L"none_MF_KB3", 0.8, false);
	pAniDataSet->AddAni(L"none_MF_KB2", 1, false);
	pAniDataSet->AddAni(L"none_MF_KB1", 0.3, false);
	pAniDataSet->AddAni(L"none_MF_KD1", 2, false);
	pAniDataSet->AddAni(L"none_MF_KD2", 1.6, true);
	pAniDataSet->AddAni(L"none_MF_KD3", 2, false);
	pAniDataSet->AddAni(L"none_MF_TU1", 0.7, false);
	pAniDataSet->AddAni(L"none_MF_TU2", 1.3, false);
	pAniDataSet->AddAni(L"none_MF_UP", 1.6, false);
	pAniDataSet->AddAni(L"none_MF_STUN", 1.7, true);


	// 1hs
	pAniDataSet->AddAni(L"1hs_run_f", 0.8, true);
	pAniDataSet->AddAni(L"1hs_run_b", 0.8, true);
	pAniDataSet->AddAni(L"1hs_run_r", 0.8, true);
	pAniDataSet->AddAni(L"1hs_run_l", 0.8, true);

	pAniDataSet->AddAni(L"1hs_idle", 2, true);
	pAniDataSet->AddAni(L"1hs_ends", 1, false);
	pAniDataSet->AddAni(L"1hs_end", 1, false);

	pAniDataSet->AddAni(L"1hs_die", 2, false);
	pAniDataSet->AddAni(L"1hs_starts", 1, false);
	pAniDataSet->AddAni(L"1hs_start", 1, false);

	pAniDataSet->AddAni(L"1hs_guards_end", 0.3, false);
	pAniDataSet->AddAni(L"1hs_guards_idle", 1, true);
	pAniDataSet->AddAni(L"1hs_guard_end", 0.3, false);
	pAniDataSet->AddAni(L"1hs_guard_idle", 1, true);
	pAniDataSet->AddAni(L"1hs_guard_block", 0.3, false);

	pAniDataSet->AddAni(L"1hs_jump_up", 0.3, false);
	pAniDataSet->AddAni(L"1hs_jump_down", 1.5, false);
	pAniDataSet->AddAni(L"1hs_jump_updown", 2.33, false);
	pAniDataSet->AddAni(L"1hs_jump_land", 0.3, false);

	pAniDataSet->AddAni(L"1hs_dodge_f", 1.5, false);
	pAniDataSet->AddAni(L"1hs_dodge_b", 1.5, false);
	pAniDataSet->AddAni(L"1hs_dodge_l", 1.5, false);
	pAniDataSet->AddAni(L"1hs_dodge_r", 1.5, false);

	pAniDataSet->AddAni(L"1hs_atk2", 0.8, false);
	pAniDataSet->AddAni(L"1hs_atk2_r", 0.2, false);
	pAniDataSet->AddAni(L"1hs_atk3_r", 0.2, false);
	pAniDataSet->AddAni(L"1hs_atk3", 0.8, false);
	pAniDataSet->AddAni(L"1hs_atk1f", 0.8, false);
	pAniDataSet->AddAni(L"1hs_atk1f_r", 0.2, false);
	pAniDataSet->AddAni(L"1hs_atk_start", 1.2, false);
	pAniDataSet->AddAni(L"1hs_atk_down", 0.9, false);
	pAniDataSet->AddAni(L"1hs_atk1", 0.6, false);
	pAniDataSet->AddAni(L"1hs_atk1_r", 0.2, false);
	pAniDataSet->AddAni(L"1hs_atk_air", 2, false);
	pAniDataSet->AddAni(L"1hs_atk_upper", 1.4, false);

	pAniDataSet->AddAni(L"1hs_sleep", 1.6, true);

	pAniDataSet->AddAni(L"1hs_spell_cast1", 2.17, false);
	pAniDataSet->AddAni(L"1hs_spell_cast2", 0.5, false);
	pAniDataSet->AddAni(L"1hs_spell_cast3", 1.6, false);

	pAniDataSet->AddAni(L"1hs_spell_fire1", 1.1, false);
	pAniDataSet->AddAni(L"1hs_spell_fire2", 1.3, false);
	pAniDataSet->AddAni(L"1hs_spell_fire3", 2.3, false);

	pAniDataSet->AddAni(L"1hs_spell_loop1", 1, true);
	pAniDataSet->AddAni(L"1hs_spell_loop2", 1, true);
	pAniDataSet->AddAni(L"1hs_spell_loop3", 2, true);


	pAniDataSet->AddAni(L"1hs_active_cast", 0.3, false);
	pAniDataSet->AddAni(L"1hs_active_fire", 0.8, false);
	pAniDataSet->AddAni(L"1hs_active_idle", 1.7, true);
	pAniDataSet->AddAni(L"1hs_active_1", 1, false);
	pAniDataSet->AddAni(L"1hs_active_2", 1, false);
	pAniDataSet->AddAni(L"1hs_active_3", 2, false);
	pAniDataSet->AddAni(L"1hs_counter", 13, false);
	pAniDataSet->AddAni(L"1hs_blocked", 0.5, false);
	pAniDataSet->AddAni(L"1hs_chainsmash", 1.4, false);

	pAniDataSet->AddAni(L"1hs_MF_BT1", 1, false);
	pAniDataSet->AddAni(L"1hs_MF_BT2", 1, false);
	pAniDataSet->AddAni(L"1hs_MF_BT3", 1, false);
	pAniDataSet->AddAni(L"1hs_MF_BT4", 1, false);

	pAniDataSet->AddAni(L"1hs_MF_KD1", 2, false);
	pAniDataSet->AddAni(L"1hs_MF_KD2", 1.6, true);
	pAniDataSet->AddAni(L"1hs_MF_KD3", 2, false);

	pAniDataSet->AddAni(L"1hs_MF_KB1", 0.3, false);
	pAniDataSet->AddAni(L"1hs_MF_KB2", 1, false);
	pAniDataSet->AddAni(L"1hs_MF_KB3", 0.7, false);

	pAniDataSet->AddAni(L"1hs_MF_TU1", 0.7, false);
	pAniDataSet->AddAni(L"1hs_MF_TU2", 1.3, false);

	pAniDataSet->AddAni(L"1hs_MF_STS", 1.3, false);
	pAniDataSet->AddAni(L"1hs_MF_ST", 1.3, false);
	pAniDataSet->AddAni(L"1hs_MF_DT", 0.8, false);
	pAniDataSet->AddAni(L"1hs_MF_UP", 1.3, false);
	pAniDataSet->AddAni(L"1hs_MF_STUN", 1.2, true);
}

void XUnitTestHelper::SetAniDataSet( XObject* pObject, TestAniDataSet* pAniDataSet )
{
	if (pObject == NULL) return;
	if (pObject->GetModuleEntity() == NULL) return;

	MockAnimationController* ani_controller = dynamic_cast<MockAnimationController*>(pObject->GetModuleEntity()->GetAnimationController());
	if (ani_controller == NULL) return;

	ani_controller->SetAniDataSet(pAniDataSet);
}

void XUnitTestHelper::CreateItemSwordInfo( int nItemID)
{
	MockItemInfoMgr* pMgr = dynamic_cast<MockItemInfoMgr*>(info.item);
	if (pMgr == NULL) return;

	CSItemData* pItemData = pMgr->NewItemData();
	pItemData->m_nID = nItemID;

	pMgr->SetItem(pItemData, L"<ITEM name=\"보급형 검\" mesh_name=\"longsword_02\" \
							node_name=\"longsword_02\" mesh_path=\"weapons\" parent_name=\"dummy_1hs_r\" \
							type=\"weapon\" sex=\"All\" slot=\"rweapon\" weapon_type=\"1hs\" buyprice=\"0\" \
							sellprice=\"5\" stacksize=\"1\" weight=\"5\" max_durability=\"20\" min_damage=\"25\" \
							max_damage=\"30\" />" );
}

void XUnitTestHelper::CreateItemBowInfo( int nItemID)
{
	MockItemInfoMgr* pMgr = dynamic_cast<MockItemInfoMgr*>(info.item);
	if (pMgr == NULL) return;

	CSItemData* pItemData = pMgr->NewItemData();
	pItemData->m_nID = nItemID;

	pMgr->SetItem(pItemData, L"<ITEM name=\"보급형 활\" mesh_name=\"Bow_01\" node_name=\"bow_01\" \
							 mesh_path=\"weapons\" parent_name=\"dummy_arc\" type=\"weapon\" \
							 sex=\"All\" slot=\"rweapon\" weapon_type=\"arc\" buyprice=\"0\" \
							 sellprice=\"5\" stacksize=\"1\" weight=\"7\" max_durability=\"20\" \
							 min_damage=\"24\" max_damage=\"31\" />" );
}

void XUnitTestHelper::CreateWeaponItemData( int nItemID, WEAPON_TYPE nWeaponType )
{
	MockItemInfoMgr* pMgr = dynamic_cast<MockItemInfoMgr*>(info.item);
	if (pMgr == NULL) return;

	XItemData* pNewItemData = new XItemData();
	pNewItemData->m_nID = nItemID;
	pNewItemData->m_ItemType = ITEMTYPE_WEAPON;
	pNewItemData->m_nWeaponType = nWeaponType;
	pNewItemData->m_nItemSlot = ITEMSLOT_RWEAPON;

	pMgr->InsertItemData(pNewItemData);
}

void XUnitTestHelper::CreateItemAndEquip( XMyPlayer* pMyPlayer, int nItemID, SH_ITEM_SLOT nSlot )
{
	XItem* pNewItem = info.item->MakeNewItem(nItemID);
	pNewItem->m_nSlotID = nSlot;
	pNewItem->m_nAmount = 1;
	
	int nInvenSlotID = 0;
	int nInvenSize = gvar.MyInfo.Inventory.GetSlotSize();
	for(int i = 0; i < nInvenSize; i++)
	{
		XItem* pItem = gvar.MyInfo.Inventory.GetItem(nSlot);
		if (pItem == NULL)
		{
			nInvenSlotID = i;
			gvar.MyInfo.Inventory.PushItem(nInvenSlotID, pNewItem);
		}
	}

	pMyPlayer->EquipItem(SLOTTYPE_INVENTORY, nInvenSlotID, nSlot);
}

void XUnitTestHelper::SetMeshColCapsule( std::wstring strMeshName, MCapsule cap )
{
	MockMeshInfoMgr* pMgr = static_cast<MockMeshInfoMgr*>(info.mesh_info);
	if(pMgr == NULL) return;

	MCapsule meshColCapsule = cap;

	wostringstream ostr;
	ostr << L"<MeshInfo MeshName=\""
			<< strMeshName
			<< L"\" >\
			   <ColInfo>\
			   <Capsule ID=\"1\" TopBone=\"\" BottomBone=\"\">"
			<< meshColCapsule.bottom.x << " "
			<< meshColCapsule.bottom.y << " "
			<< meshColCapsule.bottom.z << " "
			<< meshColCapsule.top.x << " "
			<< meshColCapsule.top.y << " "
			<< meshColCapsule.top.z << " "
			<< meshColCapsule.radius << " "
			<< L"</Capsule>\
			   </ColInfo>\
			   </MeshInfo>";

	std::wstring strXML(ostr.str());

	pMgr->SetMeshColInfo(strMeshName, strXML);
}

void XUnitTestHelper::SetMeshHitCapsule( std::wstring strMeshName, MCapsule* pCapsule /*= NULL*/ )
{
	MockMeshInfoMgr* pMgr = static_cast<MockMeshInfoMgr*>(info.mesh_info);
	if(pMgr == NULL) return;

	MCapsule meshHitCapsule(vec3(0.0f, 0.0f, 50.0f), vec3(0.0f, 0.0f, 150.0f), 50.0f);
	if(pCapsule)
	{
		meshHitCapsule.bottom	= pCapsule->bottom;
		meshHitCapsule.top		= pCapsule->top;
		meshHitCapsule.radius	= pCapsule->radius;
		meshHitCapsule.height	= pCapsule->height;
	}

	wostringstream ostr;
	ostr << L"<MeshInfo MeshName=\""
		<< strMeshName
		<< L"\" >\
		   <HitInfo>\
		   <Capsule ID=\"0\" ColIndex=\"0\">"
		   << meshHitCapsule.bottom.x << L" "
		   << meshHitCapsule.bottom.y << L" "
		   << meshHitCapsule.bottom.z << L" "
		   << meshHitCapsule.top.x << L" "
		   << meshHitCapsule.top.y << L" "
		   << meshHitCapsule.top.z << L" "
		   << meshHitCapsule.radius << L" "
		   << L"</Capsule>\
			  </HitInfo>\
			  </MeshInfo>";

	std::wstring strXML(ostr.str());

	pMgr->SetMeshColInfo(strMeshName, strXML);
}

void XUnitTestHelper::CreateTalentInfo( int nTalentID, const std::wstring& strInfo )
{
	MockTalentInfoMgr* pMgr = dynamic_cast<MockTalentInfoMgr*>(info.talent);
	if (pMgr == NULL) return;

	XTalentInfo* pNewTalentInfo = new XTalentInfo();

	pNewTalentInfo->m_nID = nTalentID;

	pMgr->SetTalent(pNewTalentInfo, strInfo);
}

void XUnitTestHelper::Clear()
{
	// 아이템
	MockItemInfoMgr* pItemMgr = dynamic_cast<MockItemInfoMgr*>(info.item);
	if(pItemMgr)
	{
		pItemMgr->Clear();
	}

	// 텔런트
	MockTalentInfoMgr* pTalentMgr = dynamic_cast<MockTalentInfoMgr*>(info.talent);
	if(pTalentMgr)
	{
		map<int , CSTalentInfo* >::iterator itInfo = pTalentMgr->begin();
		while(itInfo != pTalentMgr->end())
		{
			SAFE_DELETE((*itInfo).second);
			itInfo++;
		}

		pTalentMgr->clear();
	}

	// 충돌
	MockMeshInfoMgr* pMeshInfoMgr = static_cast<MockMeshInfoMgr*>(info.mesh_info);
	if(pMeshInfoMgr)
	{
		pMeshInfoMgr->m_mapMeshInfo.clear();
	}
}

void XUnitTestHelper::AddNPCMotions()
{
	info.motion->AddMotionForTest(L"	<MOTION name='npc_idle' loop='true' cancelEnabled='true'>		\
								  <Default>									\
								  <Sequence ani='idle'/>					\
								  </Default>								\
								  </MOTION>");

	info.motion->AddMotionForTest(L"	<MOTION name='npc_mf_beaten_back' loop='false' cancelEnabled='true'>		\
								  <Default>									\
								  <Sequence ani='MF_BT2'/>					\
								  </Default>								\
								  </MOTION>");

}



XNPCInfo* XUnitTestHelper::NewNPCInfo( int nNPCID /*= 1*/, const wchar_t* szName /*= L"SampleNPC"*/, int nGrade /*= 3*/, const wchar_t* szMeshName /*= L"hm_adult"*/, const wchar_t* szMeshPath /*= L"NPC/hm_adult"*/, const wchar_t* szType )
{
	wchar_t szText[1024];
	swprintf_s(szText, L"<NPC id=\"%d\" Name=\"%s\" Grade=\"%d\" Type=\"%s\" MeshPath=\"%s\" MeshName=\"%s\"	/>", 
		nNPCID, szName, nGrade, szType, szMeshPath, szMeshName);

	// npc 정보 추가
	info.npc->CreateNPCFromString(szText);

	
	// 문자열 테이블에 이름넣기
	wstring strKey = CSFormatString::Format(L"NPC_NAME_{0}", FSParam(nNPCID));
		
	XStringResMgr::GetInstance()->AddLocale(L"ko_KR");				// 로케일 설정 없으면, 설정.

	XStringResMgr::GetInstance()->DeleteString(strKey);				// 혹시 다른 테스트에서 동일키에 값을 넣었다면 지워준다.
	XStringResMgr::GetInstance()->InsertString(strKey, szName);

	return info.npc->Get(nNPCID);
}

void XUnitTestHelper::CreateItemAndInven( XMyPlayer* pMyPlayer, int nItemID, SH_ITEM_SLOT nSlot )
{
	XItem* pNewItem = info.item->MakeNewItem(nItemID);
	pNewItem->m_nSlotID = nSlot;
	pNewItem->m_nAmount = 1;

	int nInvenSlotID = 0;
	int nInvenSize = gvar.MyInfo.Inventory.GetSlotSize();
	for(int i = 0; i < nInvenSize; i++)
	{
		XItem* pItem = gvar.MyInfo.Inventory.GetItem(nSlot);
		if (pItem == NULL)
		{
			nInvenSlotID = i;
			gvar.MyInfo.Inventory.PushItem(nInvenSlotID, pNewItem);
		}
	}
}
