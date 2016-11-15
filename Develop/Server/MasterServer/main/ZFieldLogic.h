#pragma once

class ZGameServerObject;
class ZPlayer;
class ZDynamicFieldMaster;
struct TD_GATE_INFO;
struct TD_GATE_FIELD_GROUP_INFO;

class ZFieldLogic
{
public:
	ZFieldLogic() {}
	~ZFieldLogic() {}

	void AllFieldDeleteInGameServer(int nGameServerID);

	bool OnRequestDelDynamicFieldGroup(MUID uidRequestServer, MUID uidFieldGroup);

	void OnGateAskSharedField(ZGameServerObject* pRequestServer, ZPlayer* pRequestPlayer, TD_GATE_INFO* pGateInfo);
	void OnGateAskDynamicField(ZGameServerObject* pRequestServer, ZPlayer* pRequestPlayer, const TD_GATE_FIELD_GROUP_INFO* pGateFieldGroupInfo);

	void OnGatePrepareChangeChannelReq(ZGameServerObject* pRequestServer, ZPlayer* pRequestPlayer, int nChannelID);
	void OnGateCancelChangeChannel(ZPlayer* pRequestPlayer);
	void OnGateAskChangeChannel(ZGameServerObject* pRequestServer, ZPlayer* pRequestPlayer, const TD_GATE_INFO* pGateInfo);

private:
	void _GateFieldGroup_End(ZPlayer* pRequestPlayer, ZDynamicFieldMaster* pFieldGroup);

};