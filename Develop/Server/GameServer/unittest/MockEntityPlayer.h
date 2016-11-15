#ifndef _MOCK_ENTITY_PLAYER_H_
#define _MOCK_ENTITY_PLAYER_H_

#include "GEntityPlayer.h"

class MockEntityPlayer : public GEntityPlayer
{
public:
	MockEntityPlayer() {};
	virtual ~MockEntityPlayer() {};

	virtual bool SetPos(vec3& vPos)
	{
		return GEntity::SetPos(vPos);
	}

	virtual void RouteToThisCellExceptMe(MCommand* pCommand) { SAFE_DELETE(pCommand); }
	virtual void RouteToMe(MCommand* pCommand) { SAFE_DELETE(pCommand); }
	virtual void RouteToParty(MCommand* pCommand) { SAFE_DELETE(pCommand); }


};

#endif //_MOCK_ENTITY_PLAYER_H_