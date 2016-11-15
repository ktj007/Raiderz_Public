#pragma once

#include "GTestWrapper.h"
#include "GPlayerObjectManager.h"
#include "STypes.h"

class MockPlayerObject;
class GField;
class GEntityPlayer;
class GEntityActor;

struct FBasePlayer
{
	FBasePlayer()
	{

	}
	virtual ~FBasePlayer()
	{

	}

	MockPlayerObject* NewMockPlayerObject(MUID uidPlayerObject);

	// 플레이어 엔티티를 생성한다.  uidNew가 무효할경우 새로운 UID를 발급해준다.
	GEntityPlayer* NewEntityPlayer( GField* pField, vec3 pos=vec3::ZERO, MUID uidNew=MUID::Invalid());
	// 플레이어 엔티티를 삭제한다.
	void DeleteEntityPlayer(GEntityPlayer* pPlayer);
	// 플레이어의 상태값을 설정
	void SetStatus( GEntityPlayer* pEntityPlayer, int nHP, int nEN, int nSTA );
	// 해당 액터와 전투상태로 만듬
	void MakeCombat( GEntityPlayer* pEntityPlayer, GEntityNPC* pOpponent );

	GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectManagerWrapper;
};