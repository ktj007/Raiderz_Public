#ifndef _XMYACTION_ATTUNE_H
#define _XMYACTION_ATTUNE_H

#include "XMyActionState.h"

enum ATTUNE_STATE
{
	AS_READY,
	AS_START,
	AS_CHECK,
	AS_ATTUNING,
	AS_DONE,
	AS_CANCEL,
};


class XMyActionAttune : public XMyActionState
{
private:
	struct  
	{
		//bool	bEquipped;
		int		nSlotID;
		//int		nAgentInvenIndex;
	} m_AttuneData;

	ATTUNE_STATE			m_eAttuneState;
	//int						m_nAttuneItemInvenSlotID; //This is for the enchant item, we do not use this since we have XP.
	bool					m_bAttunable;

	MRegulator				m_Regulator;

private:
	void					SendAttuneInfo(SH_ITEM_SLOT_TYPE eSlotType, int nTargetSlotID/*, int nEnchantStoneSlotID, int nEnchantAgentSlotID*/);

	bool					CheckAttuneEnable(int nSlotID);
	bool					CheckAttuneAbleToDo(int nTargetSlotID/*, int nEnchantStoneSlotID, bool bEquipSlot*/);

	void					Init();
	void					EndAttune(bool bUseIdleMotion);
	void					CancelAttune();

protected:
	virtual bool			CheckEnterable(void* pParam);
	virtual bool			CheckExitable(int nNextStateID);

public:
	XMyActionAttune(XModuleMyControl* pOwner);

	virtual void			Enter(void* pParam=NULL);
	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);
	virtual void			Update(float fDelta);
	virtual	bool			IsPostMovement()	{ return false; }
	//virtual bool			CheckExitable(int nNextStateID);

	int						GetID() { return ACTION_STATE_ATTUNE; }
};


#endif // _XMYACTION_ENCHANT_H