#pragma once

#include "XDummyBot.h"

class XBirdClient;

class XDummyBot_MoveChannel : public XDummyBot
{
	enum PHASE
	{
		PHASE_NONE,

		PHASE_1_WAITING_RECV_CHANNEL_LIST,
		PHASE_2_WAITING_RECV_RESPONSE_PREPARE_CHANGE_CHANNEL,
		PHASE_3_WAITING_PREPARE_TIME,
		PHASE_4_WAITING_RECV_CHANGE_FIELD,
	};

	struct RESULT_INFO
	{
		RESULT_INFO() : nFieldID(INVALID_ID), nChannelID(INVALID_CHANNELID) {}
		int nFieldID;
		int nChannelID;
	};

public:
	XDummyBot_MoveChannel(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_MoveChannel() {}

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"MoveChannel"; }

protected:
	virtual void OnRun(float fDelta) override;

private:
	PHASE		m_ePhase;
	RESULT_INFO	m_stExpectedDestInfo;

	std::vector<int> m_vecChannelID;
	std::vector<int> m_vecChannelStateList;

	MRegulator	m_rgltTimeout;
	MRegulator	m_rgltWaitingForChangeChannel;

	bool IsPhase(PHASE ePhase)	{ return (m_ePhase == ePhase); }
	void SetPhase(PHASE ePhase)	{ m_ePhase = ePhase; }
	void StartTimeoutTimer();
	
	void OnTestStarter();	///< RequestChannelList
	void OnTestChecker(RESULT_INFO* pResultInfo);
	void OnError(string strLog="");

	/// Command Handler
	MCommandResult OnFieldStartGame(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);

	MCommandResult OnFieldChannelList(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);
	MCommandResult OnFieldPrepareChangeChannelRes(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);
	MCommandResult OnFieldChangeField(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);
};
