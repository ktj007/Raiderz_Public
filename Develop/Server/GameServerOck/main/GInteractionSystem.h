#ifndef _G_INTERACTION_SYSTEM_H_
#define _G_INTERACTION_SYSTEM_H_

class GNPCInteractor;
class GPlayerInteractor;
class GEntityPlayer;
class GEntityActor;

class GInteractionSystem : public MTestMemPool<GInteractionSystem>
{
private:	
	GNPCInteractor*		m_pNPCInteractor;
	GPlayerInteractor*	m_pPlayerInteractor;

public:
	GInteractionSystem();
	virtual ~GInteractionSystem();

	bool Interaction(GEntityPlayer* pReqPlayer, const MUID& nTarUID);
	
	GNPCInteractor&		GetNPCInteractor()		{ return *m_pNPCInteractor; }
	GPlayerInteractor&	GetPlayerInteractor()	{ return *m_pPlayerInteractor; }

	bool IsInteractableDistance(GEntityActor* pActor1, GEntityActor* pActor2);
	float GetInteractableDistance(GEntityActor* pActor1, GEntityActor* pActor2);
};


#endif//_G_INTERACTION_SYSTEM_H_

