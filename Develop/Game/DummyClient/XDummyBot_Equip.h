#pragma once

#include "XDummyBot.h"

class XDummyLazyCommandPoster;

class XDummyBot_Equip : public XDummyBot
{
	enum PHASE
	{
		PHASE_INIT,
		PHASE_INIT_DONE,
		PHASE_ADDITEM,
		PHASE_EQUIP,
		PHASE_UNEQUIP,		
	};

public:
	XDummyBot_Equip(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_Equip(void);

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"Equip"; }

protected:
	virtual void OnRun(float fDelta);

private:
	void ChangePhase(PHASE nPhase);
	void MakeTestItem();

private:
	vec3				m_vCenter;
	int					m_nFieldID;
	PHASE				m_nPhase;
	XMyInventory		m_Inventory;			///< 아이템 인벤토리
	XMyEquipmentSlot	m_EquipmentSlot;		///< 아이템 장비 슬롯

	int					m_nTestItemCount;
};
