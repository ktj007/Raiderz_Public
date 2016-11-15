#ifndef _XITEM_TALENT_H
#define	_XITEM_TALENT_H

#include "XTalent.h"

class XItemTalent : public XBaseTalent, public MMemPool<XItemTalent>
{
protected:
	virtual void			OnEnterPhasePrepare();
	virtual void			OnEnterPhaseAct();

public:
	XItemTalent(XObject* pOwner) : XBaseTalent(pOwner) {}
	virtual ~XItemTalent() {}
	virtual XTalentClassID	GetID()		{ return TCID_ITEM; }

	virtual bool			IsNeedTarget();
};

#endif // _XITEM_TALENT_H
