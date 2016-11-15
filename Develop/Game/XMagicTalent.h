#ifndef _XMAGIC_TALENT_H
#define	_XMAGIC_TALENT_H

#include "XTalent.h"

#define STEPUP_SHOW_NODE			"Bip01 Spine2"

class XMagicTalent : public XBaseTalent, public MMemPool<XMagicTalent>
{
private:
	void					SendMsg(int nEventID);

protected:
	virtual void			OnEnterPhasePrepare();
	virtual void			OnEnterPhaseAct();
	virtual void			OnStart();
	virtual XEventResult	OnEvent(XEvent& msg);

	virtual void			OnReCastingTalent(int nTalentID, int nStep);

public:
	XMagicTalent(XObject* pOwner) : XBaseTalent(pOwner) {}
	virtual ~XMagicTalent() {}
	virtual bool			IsNeedTarget();
	virtual XTalentClassID	GetID()		{ return TCID_MAGIC; }

};

#endif // _XMAGIC_TALENT_H
