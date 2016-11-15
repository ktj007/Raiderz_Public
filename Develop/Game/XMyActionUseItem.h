#ifndef _XMYACTION_USE_ITEM_H
#define _XMYACTION_USE_ITEM_H

#include "XMyActionState.h"
#include "XModuleCollision.h"

class XMyActionUseItem : public XMyActionState
{
private:
	float					m_fElipsed;				// 경과 시간(임시로 사용중)
	int						m_nInvenSlotID;

private:
	MUID					CheckInteractionItem(XItemData* pItemData);
	MUID					GetInteractionObject(INTERACTIONTYPE FindType);
public:
	XMyActionUseItem(XModuleMyControl* pOwner);
	virtual ~XMyActionUseItem();
	virtual bool			CheckEnterable(void* pParam);
	virtual void			Enter(void* pParam=NULL);
	virtual XEventResult	Event(XEvent& msg);
	virtual void			Update(float fDelta);

	int						GetID() { return ACTION_STATE_USE_ITEM; }
	virtual	bool			IsPostMovement()	{ return false; }
};


#endif // _XMYACTION_USETALENT_H