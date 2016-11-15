#pragma once

#include "XCmdParam.h"

class XCmdParam__MC_COMM_RESPONSE_ACCOUNT_CHAR_LIST : public XCmdParam
{
public:
	// value
	TD_AccountCharInfo*		m_pTDAccountCharInfo;
	int						m_nBlobCount;
public:
	XCmdParam__MC_COMM_RESPONSE_ACCOUNT_CHAR_LIST() : m_nBlobCount(0), m_pTDAccountCharInfo(NULL) {}

	virtual bool Parse(MCommand* pCommand);
	void AssignTo(vector<XAccountCharInfo>& outvecAccountCharInfo);

};


//////////////////////////////////////////////////////////////////////////
class XCmdParam__MC_COMM_RESPONSE_LOGIN : public XCmdParam
{
public:
	// Value
	int		m_nResult;
	MUID	m_uidAlloc;
	uint8	m_nServerMode; ///< CSServerMode (CSDef.h)
public:
	XCmdParam__MC_COMM_RESPONSE_LOGIN() : m_nResult(0), m_uidAlloc(MUID::ZERO), m_nServerMode(SERVER_MODE_NORMAL) {}
	virtual bool Parse(MCommand* pCommand)
	{
		if( !pCommand->GetParameter(&m_nResult,		0, MPT_INT) )	return false;
		if( !pCommand->GetParameter(&m_uidAlloc,	1, MPT_UID) )	return false;
		if( !pCommand->GetParameter(&m_nServerMode,	2, MPT_UCHAR) )	return false;

		return true;
	}
};

//////////////////////////////////////////////////////////////////////////
class XCmdParam__MC_CHAR_RESPONSE_SEL_MYCHAR : public XCmdParam_IntResult
{
public:
	XCmdParam__MC_CHAR_RESPONSE_SEL_MYCHAR()	{}
};


class XCmdParam__MC_COMM_MOVE_TO_GAME_SERVER: public XCmdParam
{
public:
	// Value
	TD_LOGIN_GAME_SERVER_INFO*	m_pLoginGameServerInfo;

public:
	XCmdParam__MC_COMM_MOVE_TO_GAME_SERVER(): m_pLoginGameServerInfo(NULL) {}

	virtual bool Parse(MCommand* pCommand)
	{
		if (GetBlobParameter<TD_LOGIN_GAME_SERVER_INFO>(pCommand, 0, &m_pLoginGameServerInfo, NULL) == false)
		{
			return false;
		}

		return true;
	}
};


//////////////////////////////////////////////////////////////////////////
class XCmdParam__MC_CHAR_RESPONSE_INSERT_CHAR : public XCmdParam_IntResult, public XCmdParam
{
public:
	XCmdParam__MC_CHAR_RESPONSE_INSERT_CHAR() {}

	virtual bool Parse(MCommand* pCommand)
	{
		if (!XCmdParam_IntResult::Parse(pCommand))	return false;

		return true;
	}

};


//////////////////////////////////////////////////////////////////////////
class XCmdParam__MC_CHAR_RESPONSE_DELETE_CHAR : public XCmdParam_IntResult, public XCmdParam
{
public:
	uint8 m_nCharIndex;

	XCmdParam__MC_CHAR_RESPONSE_DELETE_CHAR() : m_nCharIndex(0) {}

	virtual bool Parse(MCommand* pCommand)
	{
		if (!XCmdParam_IntResult::Parse(pCommand))	return false;
		if (!pCommand->GetParameter(&m_nCharIndex,	1, MPT_CHAR) )	return false;

		return true;
	}
};
