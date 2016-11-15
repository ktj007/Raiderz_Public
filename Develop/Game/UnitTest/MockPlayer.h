#pragma once

#include "XPlayer.h"
#include "MockModuleMotion.h"
#include "MockModuleAction.h"
#include "MockModuleCollision.h"

class MockPlayer : public XPlayer
{
private:
	ARMOR_TYPE						m_nArmorType;
	WEAPON_TYPE						m_nWeaponType;
	bool							m_bEqupShield;

	MockModuleMotion*				m_pMockModuleMotion;
	MockModuleAction*				m_pMockModuleAction;
	MockModuleCollision*			m_pMockModuleCollision;

protected:
	SEX		m_nSex;

	virtual void					RegisterModules() 
	{
		XPlayer::RegisterModules();
	}
	virtual void					UnregisterModules() 
	{
		XPlayer::UnregisterModules();
	}

	virtual ARMOR_TYPE				GetArmorType()	{ return m_nArmorType;}	// MyPlayer, NetPlayer 각각 작성해야한다.
	virtual WEAPON_TYPE				GetCurrWeaponType() { return m_nWeaponType;	}	// MyPlayer, NetPlayer 각각 작성해야한다.
	virtual bool					IsEquipShield()		{ return m_bEqupShield;			}	// MyPlayer, NetPlayer 각각 작성해야한다.

public:
	MockPlayer(MUID uid, SEX nSex) : XPlayer(uid, 0), m_nSex(nSex)
	{
		m_nTypeID = ETID_PLAYER;

		m_nArmorType = ARMOR_NONE;
		m_nWeaponType = WEAPON_NONE;
		m_bEqupShield = false;

		// Module
		m_pMockModuleMotion			= NULL;
		m_pMockModuleAction			= NULL;
		m_pMockModuleCollision		= NULL;
	}
	virtual ~MockPlayer() {}
	virtual XObjectID				GetType() { return XOBJ_MOCK_PLAYER; }

	virtual SEX						GetSex()	{ return m_nSex; }

	// for test
	void SetDead(bool bDead) { m_bDead = bDead; }

	void SetArmorType(ARMOR_TYPE nArmorType) { m_nArmorType = nArmorType; }
	void SetCurrWeaponType(WEAPON_TYPE nWeaponType) { m_nWeaponType = nWeaponType; }
	void SetEquipShield(bool bShiled) { m_bEqupShield = bShiled; }

	// Set Mock Module
	void SetMockModuleMotion(MockModuleMotion* pModuleMotion, bool bReg = false);
	void SetMockModuleAction(MockModuleAction* pModuleAction, bool bReg = false);
	void SetMockModuleCollision(MockModuleCollision* pModuleCollision, bool bReg = false);

	// Get Mock Module
	MockModuleMotion* GetMockModuleMotion() { return m_pMockModuleMotion; }
	MockModuleAction* GetMockModuleAction() { return m_pMockModuleAction; }
	MockModuleCollision* GetMockModuleCollision() { return m_pMockModuleCollision; }
};
