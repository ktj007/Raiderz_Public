#pragma once

class GEntityActor;
class GEntityPlayer;
class GTalentInfo;
enum QUESTPVP_TEAM;

//////////////////////////////////////////////////////////////////////////
//
// GPlayerPVPArea
//
//////////////////////////////////////////////////////////////////////////

class GPlayerPVPArea
{
public:
	GPlayerPVPArea(GEntityPlayer*	pOwnerPlayer);
	TEST_VIRTUAL ~GPlayerPVPArea(void);

	// 공격자로부터 피격가능한지 여부
	bool IsHittable( GEntityActor* pAttacker );
	// PVP 가능한 지역에 있는지 여부
	TEST_VIRTUAL bool IsLocatedPvPArea();
	// 해당 PVP 지역에서 소유자가 누구편인지 반환
	TEST_VIRTUAL QUESTPVP_TEAM GetTeam();

	// PVP 가능한 지역에 있고 특정편에 속해있는지 반환
	TEST_VIRTUAL bool IsLocateAndBelongTeam();
	
private:
	GEntityPlayer*	m_pOwner;
};
