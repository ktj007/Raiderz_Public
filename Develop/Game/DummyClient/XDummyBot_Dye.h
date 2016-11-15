#pragma once

#include "XDummyBot.h"



class XDummyBot_Dye : public XDummyBot
{
public:
	XDummyBot_Dye(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	virtual ~XDummyBot_Dye(void);

	enum PHASE
	{
		PHASE_INIT,
		PHASE_INIT_DONE,
		PHASE_ADDITEM,
		PHASE_EQUIPITEM,
		PHASE_DYE_PREPARE_BEGIN,
		PHASE_DYE_PREPARE,
		PHASE_DYE,
		PHASE_FINI,
	};

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"Dye"; }	

protected:
	virtual void OnRun(float fDelta);

private:
	void ChangePhase(PHASE nPhase);
	void InitTestItemID();

private:
	PHASE				m_nPhase;
	vector<int>			m_vecEquipItemID;
	vector<int>			m_vecDyeItemID;

	int					m_nDyeItemID;
	int					m_nEquipItemID;

	bool				m_bDyeToInvenItem;

	XMyInventory		m_Inventory;			///< 아이템 인벤토리
	XMyEquipmentSlot	m_EquipmentSlot;		///< 아이템 장비 슬롯
};
