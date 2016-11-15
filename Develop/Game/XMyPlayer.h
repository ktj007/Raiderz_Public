#ifndef _X_MY_PLAYER_H
#define _X_MY_PLAYER_H

#include "XPlayer.h"
#include "CTransData.h"
#include "XModuleMyControl.h"

class XTalentInfo;
class XModuleMyControl;
class XModuleActorControl;
class XModuleNetControl;
class XModulePost;

class XMyPlayer : public XPlayer
{
	MDeclareRTTI;
protected:
	XModuleMyControl*				m_pModuleMyControl;
	XModulePost*					m_pModulePost;

protected:
	virtual void					OnDestroy();
	virtual void					OnUpdate(float fDelta);
	virtual void					RegisterModules();
	virtual void					UnregisterModules();

	bool							CheckEnchantBuffGinEnable(SH_ITEM_SLOT nItemSlot);
	bool							CheckEnchantBuffEffectShow(XItem* pItem, SH_ITEM_SLOT nSlotID);

	SH_ITEM_SLOT					GetCurrentRWeaponItemSlot();
	SH_ITEM_SLOT					GetCurrentLWeaponItemSlot();

public:
	XMyPlayer(MUID uid, UIID nUIID);
	virtual ~XMyPlayer();

	virtual XObjectID				GetType() { return XOBJ_MY_PLAYER; }
	virtual XModuleMyControl*		GetModuleMyControl()		{ return m_pModuleMyControl; }
	virtual XModuleActorControl*	GetModuleActorControl()		{ return m_pModuleMyControl; }
	virtual XModulePost*			GetModulePost()				{ return m_pModulePost; }
	virtual WEAPON_TYPE				GetCurrWeaponType();
	virtual WEAPON_TYPE				GetEquipWeaponType(bool bLeft);
	virtual ARMOR_TYPE				GetArmorType();
	virtual void					GetWeaponParentName(wstring& strRWeapon, wstring& strLWeapon);
	virtual void					GetCurrentWeaponSetWeaponItemData(XItemData** pItemDataR, XItemData** pItemDataL);

	virtual bool					IsEquipShield();	
		
	bool							EquipItem(SH_ITEM_SLOT_TYPE nSlotTypeFrom, int nSlotIDFrom, SH_ITEM_SLOT nSlotIDTo);
	bool							EquipItem(XItem* pItem, SH_ITEM_SLOT nSlotIDTo);
	bool							UnEquipItem(SH_ITEM_SLOT nSlot, int nInvenSlotID, bool bSwapToInven = false);	
	XItem*							RemoveEquipItem( SH_ITEM_SLOT nSlot );

	virtual void					EquipOverlapped(int nItemID);
	virtual void					UnEquipOverlapped(int nItemID);	

	bool							OnDyeEquipItem(SH_ITEM_SLOT_TYPE nSlotType, int nSlotID, int nColor);
	
	bool							ValidateChangeStance(CHAR_STANCE nStanceTo);

	void							InMyInfo(XGameState* pState);
	void							OutMyInfo();

	void							SwitchingWeaponSet(int8 nWeaponSet);

	virtual void					SetCharacterPane_Player();

	virtual const wchar_t*			GetName();
	virtual SEX						GetSex();

	// 캐릭터를 안정적으로 멈추게 하는 작업(텀블링이나 기타 모션중에 갑자기 멈추는 것을 막음)
	void							SafeStop();

	bool							CheckDieToHP();
	void							CheckDie();

	void							SetMyTimeInfo(  const vector<TD_BUFF_REMAIN_TIME>&			vecTDBuffRemainTime,
													const vector<TD_TALENT_REMAIN_COOLTIME>&	vecTDTalentRemainCoolTime, 
													float										fAutoRebirthRemainTime);

	void							ChangeUIID(UIID nNewUIID);	// 서버 이동하면 UIID가 바뀔 수 있다.

	// 인첸트
	void							EnchantDone(bool bEquipSlot, int nItemSlot, int nEnchantSlot, int nEnchantItemID);
	void							EnchantBuffGain(XItem* pItem, SH_ITEM_SLOT nItemSlot, int nEnchantSlot);
	void							EnchantBuffLost(XItem* pItem, SH_ITEM_SLOT nItemSlot, int nEnchantSlot);
	void							EnchantBuffAllGainForEquip(XItem* pItem, SH_ITEM_SLOT nItemSlot);
	void							EnchantBuffAllLostForUnEquip(XItem* pItem, SH_ITEM_SLOT nItemSlot);

	virtual int						GetVisualEnchantBuffID();

	// UnitTest 용
	int8							GetWeaponSet();
};






#endif