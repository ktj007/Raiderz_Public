#pragma once

#include "XGuideMsgEnum.h"

class XGuideMsg
{
private:
	enum GUIDE_MSG_STATE
	{
		PRESENTATION_LOWER = 0,
		IDLE,

		LOOT_GUIDE,
		NPC_INTERACTION_GUIDE,

	} m_CurState;

	unsigned int m_nStateChangedTime;

	struct XGuideMsgDesc
	{
		bool				bPollingCheck;
		bool				bLevelCheck;
		GUIDE_MSG_STATE		nState;

		XGuideMsgDesc(bool pollingCheck, bool levelCheck, GUIDE_MSG_STATE state) : bPollingCheck(pollingCheck), bLevelCheck(levelCheck), nState(state) {}
	};

	typedef map<XGuideMsgEnum, XGuideMsgDesc>	XGuideMsgDescTable;
	XGuideMsgDescTable		m_DescTableMap;

	bool m_bEntered;
	MUID m_uidPollingTarget;

	void OnGuideMsg( XGuideMsgEnum nMsg, XPRESENTATIONICON_TYPE nIconType );

	bool IsEnabled();

	void OnReleaseMsg();

	bool UpdateStateAndCheck( XGuideMsgEnum nMsg, bool bEnter, MUID uidTarget );
	bool UpdatePollingGuideStateAndCheck( const GUIDE_MSG_STATE _targetState, bool bEnter, MUID uidTarget );
	bool UpdateEventGuideStateAndCheck( bool bEnter );

	bool CheckLevel();

	XGuideMsgDesc* GetMsgDesc(XGuideMsgEnum nMsg);
public:
	XGuideMsg();

	void InitDesc();

	void OutputMsg(XGuideMsgEnum nMsgEnum, MUID uidTarget = MUID::ZERO);
	void ReleaseMsg(XGuideMsgEnum nMsgEnum = GUIDE_MSG_NA, MUID uidTarget=MUID::ZERO);
	void NotifyOnPresentationLower();
};