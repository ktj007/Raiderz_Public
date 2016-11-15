#pragma once

class ZSharedField;
class ZGameServerObject;
enum DYNAMIC_FIELD_TYPE;
class ZDynamicFieldMaster;
struct TD_GATE_FIELD_GROUP_INFO;

class ZFieldFinder
{
public:
	bool				FindEnterableSharedField(ZSharedField*& poutDestField, int nFieldID, int nChannelID = INVALID_CHANNELID);
	ZGameServerObject*	FindFirstGameServer(int nFieldID);

	ZDynamicFieldMaster* GetKnownFieldGroup(MUID uidPlayer, MUID uidFieldGroup, int nFieldGroup, DYNAMIC_FIELD_TYPE eType);
	ZDynamicFieldMaster* GetUnknownFieldGroup(MUID uidPlayer, int nFieldGroup, DYNAMIC_FIELD_TYPE eType, bool isSingleType);

	ZGameServerObject*	FindGameServer_HadReservedChannel(int nFieldID, int nChannelID);

private:
	ZDynamicFieldMaster* _FindExistTrialFieldGroup(MUID uidPlayer, int nFieldGroupID, DYNAMIC_FIELD_TYPE eType);
	ZDynamicFieldMaster* _FindExistInnRoom();

	ZDynamicFieldMaster* _GetEnterableFieldGroup(MUID uidPlayer, int nFieldGroupID, DYNAMIC_FIELD_TYPE eType);

};
