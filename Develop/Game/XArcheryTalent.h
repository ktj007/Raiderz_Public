#ifndef _XARCHERY_TALENT_H
#define _XARCHERY_TALENT_H

#include "XTalent.h"

class XArcheryTalent : public XBaseTalent, public MMemPool<XArcheryTalent>
{
protected:
	virtual void			OnEnterPhasePrepare();
	virtual void			OnEnterPhaseAct();
	virtual void			OnStart();

public:
	XArcheryTalent(XObject* pOwner) : XBaseTalent(pOwner) {}
	virtual ~XArcheryTalent() {}
	virtual bool			IsNeedTarget()	{ return true;	}
	virtual XTalentClassID	GetID() { return TCID_ARCHERY; }

};

#endif	//_XARCHERY_TALENT_H
