#ifndef _GFIELD_SYSTEM_H
#define _GFIELD_SYSTEM_H

#include "CCommandResultTable.h"

class GEntityActor;
class GEntityPlayer;

/// 필드 시스템
class GFieldSystem : public MTestMemPool<GFieldSystem>
{
private:

public:
	GFieldSystem() {}
	~GFieldSystem() {}

	bool PrepareChangeChannel( GEntityPlayer* pPlayer, CHANNELID nChannelID );
	bool CancelChangeChannel( GEntityPlayer* pPlayer );
	bool ChangeChannel( GEntityPlayer* pPlayer);

	void Warp( GEntityActor* pActor, vec3 vecDestPos, vec3 vecDestDir, bool bRoute );

private:
	bool Check_PrepareChangeChannel( GEntityPlayer* pPlayer, CHANNELID nChannelID );
	bool Check_ChangeChannel( GEntityPlayer* pPlayer );
	void Apply_ChangeChannel( GEntityPlayer* pPlayer );
	bool FailAndRoutePrepareChangeChannelRes( GEntityPlayer* pPlayer, CCommandResultTable error_code );
};

#endif // _GQUEST_SYSTEM_H