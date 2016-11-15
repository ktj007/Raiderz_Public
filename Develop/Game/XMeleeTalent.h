#ifndef _XMELEE_TALENT_H
#define _XMELEE_TALENT_H

#include "XTalent.h"


class XMeleeTalent : public XBaseTalent, public MMemPool<XMeleeTalent>
{
private:
	void					SwordTarilStart();
	void					SwordTarilEnd();

protected:
	virtual void			OnEnterPhasePrepare();
	virtual void			OnEnterPhaseAct();
	virtual void			OnEnterPhaseFinish();
	virtual XEventResult	OnEvent(XEvent& msg);

public:
	XMeleeTalent(XObject* pOwner) : XBaseTalent(pOwner) {}
	virtual ~XMeleeTalent() {}
	virtual XTalentClassID	GetID() { return TCID_MELEE; }
};

#endif // _XMELEE_TALENT_H