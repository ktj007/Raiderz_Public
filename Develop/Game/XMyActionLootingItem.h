#ifndef _XMYACTION_LOOTING_ITEM_H
#define _XMYACTION_LOOTING_ITEM_H

#include "XMyActionState.h"
#include "MTime.h"

#define		DUMMY_PICKAXE	"dummy_pickaxe"
#define		DUMMY_SHOVEL	"dummy_shovel"
#define		NODE_PICKAXE	"pickaxe"
#define		NODE_SHOVEL		"trowel"

class XNPCInfo;

enum LOOTINGITEM_STATE
{
	LOOTINGITEM_NONE	= 0,
	LOOTINGITEM_READY,
	LOOTINGITEM_START,
	LOOTINGITEM_END
};

class XMyActionLootingItem: public XMyActionState
{
private:
	bool					m_bGather;				//루팅중이냐? 채집중이냐?
	LOOTINGITEM_STATE		m_nLootingItemState;
	MUID					m_TargetNPCUID;
	bool					m_bLootCancel;
	bool					m_bSendLootEnd;

	RActorNode*				m_pGatherNode;

	void					SetLootingState(LOOTINGITEM_STATE nState);
	LOOTINGITEM_STATE		GetLootingState() { return m_nLootingItemState; }

	void					Init();
	void					StopMove();
	void					LootingReady();
	void					LootingEnd();

	wstring					GetAnimationName(XNPCInfo* pNPCInfo);
	void					SetNextMotion();

	void					SetGatherModel( XNPCInfo* pNPCInfo );
	void					RemoveGatherModel();
	void					GetItems(XEvent &msg );

	XNonPlayer*				FindNpc();

public:
	XMyActionLootingItem(XModuleMyControl* pOwner);

	virtual bool			CheckEnterable(void* pParam);
	virtual bool			CheckExitable(int nNextStateID)		{ return true;}
	virtual void			Enter(void* pParam=NULL);
	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);
	virtual	bool			IsPostMovement()	{ return true; }

	int						GetID() { return ACTION_STATE_LOOTINGITEM; }
	void					LootingStart();
};



#endif // _XMYACTION_LOOTING_ITEM_H