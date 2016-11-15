#ifndef _X_NET_PLAYER_H
#define _X_NET_PLAYER_H

#include "XPlayer.h"
#include "CTransData.h"
#include "XPlayerInfo.h"

class XModuleNetControl;
class XModuleActorControl;
//class XModuleInteraction;
struct XCharExtFeature;

class XNetPlayer : public XPlayer, public MMemPool<XNetPlayer>
{
	MDeclareRTTI;

private:
	wstring							m_strName;		// Player ID
	XPlayerInfoFeature				m_Feature;		// NetPlayer만 사용
	XPlayerInfoFeature				m_OverlappedFeature;		// NetPlayer만 사용
	bool							m_bPartyLeader;

	XIDContainer					m_ctBuffList;			///< 버프효과리스트

	struct _KeepingDataForPvp
	{
		bool bChanged;
		wstring	strName;
		wstring	strGuild;

		_KeepingDataForPvp() : bChanged( false)		{}
	};
	_KeepingDataForPvp				m_KeepingDataForPvp;
protected:
	XModuleNetControl*				m_pModuleNetControl;
	//XModuleUI*					m_pModuleUI;

private:
	void							_RenderDebug();

protected:
	virtual void					RegisterModules();
	virtual void					UnregisterModules();
	virtual void					OnUpdate(float fDelta);
	virtual void					OnRender();

	bool							CheckEnchantBuffGinEnable(SH_FEATURE_ITEMSLOT nFeatureItemSlot);
	bool							CheckEnchantBuffEffectShow(int nItemID, SH_FEATURE_ITEMSLOT nFeatureItemSlot);

	int								GetEnchantBuffID(int nID);

	void							EnchantBuffLost(SH_FEATURE_ITEMSLOT& nFeatureItemSlot);


public:
	XNetPlayer(MUID uid, UIID nUIID);
	virtual ~XNetPlayer();

	virtual XObjectID				GetType() { return XOBJ_NET_PLAYER; }
	virtual XModuleNetControl*		GetModuleNetControl()			{ return m_pModuleNetControl; }
	virtual XModuleActorControl*	GetModuleActorControl();
	virtual WEAPON_TYPE				GetCurrWeaponType();
	virtual WEAPON_TYPE				GetEquipWeaponType(bool bLeft);
	virtual ARMOR_TYPE				GetArmorType();
	void							GetCurrWeapon(XItemData* pItemDataR, XItemData* pItemDataL);					///< 현재 장비한 아이템의 무기
	virtual void					GetCurrentWeaponSetWeaponItemData(XItemData** pItemDataR, XItemData** pItemDataL);

	virtual bool					IsEquipShield();

	void							EquipItem(SH_FEATURE_ITEMSLOT nFeatureItemSlot, int nItemID, SH_FEATURE_ITEMSLOT nUnEquipFeatureItemSlot, int nColor, int nEnchantItemID);
	void							UnEquipItem(SH_FEATURE_ITEMSLOT nFeatureItemSlot);

	virtual void					EquipOverlapped(int nItemID);
	virtual void					UnEquipOverlapped(int nItemID);

	void							SwitchingWeaponSet(int8 nWeaponSet);
	
	//virtual void					SetCharacterPane_AllyNetPlayer();
	virtual void					SetCharacterPane_VSNetPlayer();
	virtual void					SetCharacterPane_Player();

	void							SetFeature(const wchar_t* szID, XPlayerInfoFeature& feature);
	void							UpdateFeature();

	void							ChangeGuildName( wstring strName );
	void							ChangeNameForPvP(wstring strName);
	void							RestoreNameForPvP();
	virtual const wchar_t*			GetRealNameInPvP();

	virtual const wchar_t*			GetName()						{ return m_strName.c_str(); }
	virtual SEX						GetSex()						{ return m_Feature.nSex; }

	XPlayerInfoFeature&				GetFeature()					{ return m_Feature; }
	CSItemData*						GetEquipedItemData(SH_FEATURE_ITEMSLOT nFeatureItemSlot);

	XPlayerInfoFeature&				GetOverlappedFeature()				{ return m_OverlappedFeature; }

	void							InPlayer(TD_UPDATE_CACHE_PLAYER* pPlayerInfo, TD_PLAYER_FEATURE_TATTOO* pTattooInfo, bool bAppearEffect=true, bool bLoadingAsync = true);

	void							SetCharExtFeature( const XCharExtFeature* pExtFeature);

	void							SetBuff( TD_UPDATE_CACHE_PLAYER* pPlayerInfo );
	void							PushBuff(int nBuff)				{ m_ctBuffList.PushBackItem(nBuff); }
	void							PopBuff(int nBuff)				{ m_ctBuffList.PopItem(nBuff); }
	XIDContainer&					GetBuffList()					{ return m_ctBuffList; }
	int								GetBuffStackCount(int nBuffID);

	SH_FEATURE_ITEMSLOT				GetRWeaponCurrentSlot();
	SH_FEATURE_ITEMSLOT				GetLWeaponCurrentSlot();

	virtual int						GetVisualEnchantBuffID();
	void							DelVisualEnchantBuffEffect();

	void							SetPartyLeader(bool val) { m_bPartyLeader = val;}
	bool							IsPartyLeader() const	 { return m_bPartyLeader; }
};








#endif