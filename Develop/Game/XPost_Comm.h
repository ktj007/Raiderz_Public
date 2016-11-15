#ifndef _XPOST_COMM_H
#define _XPOST_COMM_H

#include "XNetClient.h"
#include "XCommandTable.h"

inline void XPostChar_RequestAccountCharList()
{
	XPOSTCMD0(MC_COMM_REQUEST_ACCOUNT_CHAR_LIST);
}

inline void XPostComm_RequestLogin(const wchar_t* szID, const wchar_t* szPassword, int nCommandVersion)
{
	XPOSTCMD3(MC_COMM_REQUEST_LOGIN, MCommandParameterWideString(szID), MCommandParameterWideString(szPassword), MCommandParameterInt(nCommandVersion));
}

inline void XPostComm_StartMoveToLoginServerReq()
{
	XPOSTCMD0(MC_COMM_START_MOVE_TO_LOGIN_SERVER_REQ);
}


inline void XPostComm_KillEntity(UIID nUIID)
{
	XPOSTCMD1(MC_GM_KILL_ENTITY_REQ, MCommandParameterUShort(nUIID));
}

inline void XPostComm_RangeKillEntity(float fRange)
{
	XPOSTCMD1(MC_GM_RANGE_KILL_NPC_REQ, MCommandParameterFloat(fRange));
}

inline void XPostLoot_Begin(MUID uidNPC)
{
	XPOSTCMD1(MC_LOOT_BEGIN_REQ, MCommandParameterUID(uidNPC));
}

inline void XPostBattleArena_Register()
{
	XPOSTCMD0(MC_BATTLEARENA_REGISTER_REQ);
}

inline void XPostBattleArena_Deregister()
{
	XPOSTCMD0(MC_BATTLEARENA_DEREGISTER_REQ);
}

inline void XPostField_LoadingComplete()
{
	XPOSTCMD0(MC_FIELD_LOADING_COMPLETE);
}

inline void XPostRequestDialogSelect(int nSelect)
{
	XPOSTCMD1(MC_DIALOG_SELECT_REQ, MCommandParameterInt(nSelect));
}

inline void XPostTriggerTouchSensorEnter(unsigned long nSensorID, vec3& vPos)
{
	XPOSTCMD2(MC_TRIGGER_TOUCH_SENSOR_ENTER, MCommandParameterUInt(nSensorID), MCommandParameterVector(vPos.x, vPos.y, vPos.z));
}

inline void XPostTriggerTouchSensorLeave(unsigned long nSensorID, vec3& vPos)
{
	XPOSTCMD2(MC_TRIGGER_TOUCH_SENSOR_LEAVE, MCommandParameterUInt(nSensorID), MCommandParameterVector(vPos.x, vPos.y, vPos.z));
}


inline void XPostRequestWhoIs(MUID uidObject)
{
	XPOSTCMD1(MC_FIELD_WHO_IS_REQ, MCommandParameterUID(uidObject));
}

inline void XPostRequestWhoIsUIID(UIID nObjectUIID)
{
	XPOSTCMD1(MC_FIELD_WHO_IS_UIID_REQ, MCommandParameterUShort(nObjectUIID));
}

inline void XPostRequestBuffLost(int nBuffID)
{
	XPOSTCMD1(MC_BUFF_LOST_REQ, MCommandParameterInt(nBuffID));
}

inline void XPostRequestAutoPartyEnter(int nQuestID)
{
	XPOSTCMD1(MC_AUTOPARTY_ENQUEUE_REQ, MCommandParameterInt(nQuestID));
}

inline void XPostRequestAutoPartyLeave(int nQuestID)
{
	XPOSTCMD1(MC_AUTOPARTY_DEQUEUE_REQ, MCommandParameterInt(nQuestID));
}

//inline void XPostRequestAutoPartyCheckSensor(int nQuestID)
//{
//	XPOSTCMD1(MC_AUTOPARTY_CHECK_SENSOR_REQ, MCommandParameterInt(nQuestID));
//}

inline void XPostRequestAutoPartyChangeState(int nQuestID, int nState)
{
	XPOSTCMD2(MC_AUTOPARTY_CHANGE_STATE_REQ, MCommandParameterInt(nQuestID), MCommandParameterInt(nState));
}

#endif