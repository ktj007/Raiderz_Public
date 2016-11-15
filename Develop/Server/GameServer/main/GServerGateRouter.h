#pragma once

class GPlayerGameData;
struct GGateInfo;
enum DYNAMIC_FIELD_TYPE;

class GServerGateRouter
{
public:
	GServerGateRouter();
	virtual ~GServerGateRouter();

	void			ResponseLoginGameServer(MUID uidRequestServer, MUID uidRequestID, int nResult, MUID uidConnectionKey);
	void			RequestReserveMoveGameServer(MUID uidRequestPlayer, int nMoveTargetServerID, const GPlayerGameData* pPlayerGameData);
	void			RequestCancelMoveGameServer(MUID uidRequestPlayer);
	void			ResponseReadyEnterMovePlayer(MUID uidRequestGameServer, MUID uidRequestPlayer, int nResult, MUID uidConnectionKey);
	void			AskGateSharedField(MUID uidRequestPlayer, const GGateInfo* pGateInfo);
	void			AskGateDynamicField(MUID uidRequestPlayer, const GGateInfo* pGateInfo, int nFieldGroupID, DYNAMIC_FIELD_TYPE eType, bool isSingleType);
	
	virtual void	PrepareChangeChannelReq(MUID uidRequestPlayer, int nChannelID);
	virtual void	CancelChangeChannel(MUID uidRequestPlayer);
	virtual void	AskGateChangeChannel(MUID uidRequestPlayer, const GGateInfo* pGateInfo);
};
