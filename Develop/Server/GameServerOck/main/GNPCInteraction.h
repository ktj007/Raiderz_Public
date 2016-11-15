#pragma once

#include "MMemPool.h"

class GEntityNPC;
class GEntityPlayer;

class GNPCInteraction : public MMemPool<GNPCInteraction>
{
private:
	GEntityNPC*		m_pOwner;
	set<MUID>		m_setInteractingPlayer;		///< 인터랙션중인 플레이어	
	bool			m_bEnable;					///< 현재 인터랙션 가능한 상태인지
	MUID			m_nLastInteractPlayerUID;	///< 마지막으로 인터랙션한 플레이어

	bool			m_bEnabledBeforeCombat;		///< 전투하기 전에 인터랙션이 가능한 상태 였는지

public:
	GNPCInteraction(GEntityNPC* pOwner);

	void Begin(GEntityPlayer* pPlayer);
	void End(GEntityPlayer* pPlayer);

	set<GEntityPlayer*> GetInteractingPlayer();
	bool IsInteractingPlayer(GEntityPlayer* pPlayer);

	GEntityPlayer* GetLastInteractPlayer();

	void Enable();
	void Disable();
	bool IsEnable() { return m_bEnable; }
	bool IsDisable() { return !m_bEnable; }

	void DisableByCombatEnter();
	void EnableByCombatExit();	
};
