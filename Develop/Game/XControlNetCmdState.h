#ifndef _XCONTROL_NET_COMMAND_STATE_H
#define _XCONTROL_NET_COMMAND_STATE_H

#pragma once

class XControlNetCmdState
{
private:
	vector<int>		m_vecCmdID;

private:
	bool			CheckCompareCmdID(int nID);

public:
	XControlNetCmdState();
	virtual ~XControlNetCmdState()	{}

	bool			CheckStateUsableCommand(MCommand* pCommand);

	void			SetUsableCmdID(int nID);
};

#endif