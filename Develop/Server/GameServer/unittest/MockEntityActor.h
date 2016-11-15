#ifndef _MOCK_ENTITY_ACTOR_H_
#define _MOCK_ENTITY_ACTOR_H_

#include "GEntityActor.h"

class MockEntityActor : public GEntityActor
{
public:
	MockEntityActor() {};
	virtual ~MockEntityActor() {};

	virtual bool SetPos(vec3& vPos)
	{
		return GEntity::SetPos(vPos);
	}
};

#endif //_MOCK_ENTITY_ACTOR_H_