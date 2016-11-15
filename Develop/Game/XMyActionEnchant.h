#ifndef _XMYACTION_ENCHANT_H
#define _XMYACTION_ENCHANT_H

#include "XMyActionState.h"

enum ECHANT_STATE
{
	ES_READY,
	ES_START,
	ES_CHECK,
	ES_ENCHANT,
	ES_DONE,
	ES_CANCEL,
};


class XMyActionEnchant : public XMyActionState
{
private:
	struct  
	{
		bool	bEquipped;
		int		nItemIndex;
		int		nAgentInvenIndex;
	} m_EnchantData;

	ECHANT_STATE			m_eEnchantState;
	int						m_nEnchantItemInvenSlotID;

	MRegulator				m_Regulator;

private:
	void					SendEnchantInfo(SH_ITEM_SLOT_TYPE eSlotType, int nTargetSlotID, int nEnchantStoneSlotID, int nEnchantAgentSlotID);

	bool					CheckEnchantEnable(int nSlotID);
	bool					CheckEnchantAbleToDo(int nTargetSlotID, int nEnchantStoneSlotID, bool bEquipSlot);

	void					Init();
	void					EndEnchant(bool bUseIdleMotion);
	void					CancelEnchant();

protected:
	virtual bool			CheckEnterable(void* pParam);
	//virtual bool			CheckExitable(int nNextStateID);

public:
	XMyActionEnchant(XModuleMyControl* pOwner);

	virtual void			Enter(void* pParam=NULL);
	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);
	virtual void			Update(float fDelta);
	virtual	bool			IsPostMovement()	{ return false; }
	virtual bool			CheckExitable(int nNextStateID);

	int						GetID() { return ACTION_STATE_ENCHANT; }
};


#endif // _XMYACTION_ENCHANT_H