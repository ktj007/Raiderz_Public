#ifndef _XPOST_CHANNEL_H
#define _XPOST_CHANNEL_H

#include "XNetClient.h"
#include "XCommandTable.h"

inline void XPostChannel_GetChannelList()
{
	XPOSTCMD0(MC_FIELD_REQUEST_CHANNEL_LIST);
}

inline void XPostChannel_ChangeChannelPrepare(int nChannelID)
{
	XPOSTCMD1(MC_FIELD_PREPARE_CHANGE_CHANNEL_REQ, MCommandParameterInt(nChannelID));
}

inline void XPostChannel_ChangeChannel()
{
	XPOSTCMD0(MC_FIELD_REQUEST_CHANGE_CHANNEL);
}

inline void XPostChannel_ChangeChannelCancel()
{
	XPOSTCMD0(MC_FIELD_CANCEL_CHANGE_CHANNEL);	
}

inline void XPostChannel_LeaveDynamicField()
{
	XPOSTCMD0(MC_DYNAMIC_FIELD_LEAVE_REQ);	
}

#endif