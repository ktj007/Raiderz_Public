#pragma once

class GEntityPlayer;
class GEntityActor;
class PLAYER_INFO;
struct MARKER_INFO;

class GRebirthSystem : public MTestMemPool<GRebirthSystem>
{
public:		
	bool RebirthToPlace(GEntityActor* pUser, GEntityPlayer* pTarget, float fRecoverRate);	///< 제자리 부활	
	bool RebirthToPlaceInBattleArena(GEntityPlayer* pPlayer);								///< 배틀 아레나에서 제자리 부활
	bool AutoRebirthInBattleArena(GEntityPlayer* pPlayer, vec3 vPos, vec3 vDir);			///< 배틀 아레나에서 자동 부활

	bool RebirthToSoulBinding(GEntityPlayer* pPlayer);					///< 저장한 영혼석 위치로 부활		
	bool RebirthToNearSoulBinding(GEntityPlayer* pPlayer);				///< 가장 가까운 영혼석 위치로 부활		

	bool RebirthToCheckPoint(GEntityPlayer* pPlayer);					///< 안전지점 위치로 부활
	bool RebirthToQPVP(GEntityPlayer* pPlayer);							///< QPVP부활지점으로 부활

	bool RebirthForGM(GEntityPlayer* pPlayer);							///< 운영자 명령어로 부활

	void ApplyRebirthRecoverRate(GEntityPlayer* pPlayer, float fRecoverRate);
	void ApplyRebirthBuff(GEntityPlayer* pPlayer, int nBuffID1, int nBuffID2=0, int nBuffTargetLevel=0);

private:
	bool Rebirth(GEntityActor* pUser, GEntityPlayer* pTarget, int nFieldID, vec3 vPos, vec3 vDir, float fRecoverRate, int nBuffID1=0, int nBuffID2=0, int nBuffTargetLevel=0);
	void Route(GEntityPlayer* pPlayer, int nFieldID, vec3 vPos, vec3 vDir);

	const MARKER_INFO* GetNearSoulBindingMarker(GEntityPlayer* pPlayer);
};
