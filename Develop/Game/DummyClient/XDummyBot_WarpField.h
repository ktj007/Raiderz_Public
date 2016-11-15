#pragma once

#include "XDummyBot.h"

class XBirdClient;

class XDummyBot_WarpField : public XDummyBot
{
	enum BOT_PHASE
	{
		PHASE_0_STARTING,
		PHASE_1_WARP,
		PHASE_2_CHECK,
	};

	struct RESULT_INFO
	{
		int nFieldID;

		RESULT_INFO() { Reset(); }
		void Reset()
		{
			nFieldID = INVALID_ID;
		}
	};

public:
	XDummyBot_WarpField(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	virtual ~XDummyBot_WarpField() {}

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"WarpField"; }

protected:
	virtual void	OnRun(float fDelta) override;

	void			OnRun_Check(float fDelta);
	void			OnRun_Warp(float fDelta);

private:
	BOT_PHASE			m_ePhase;
	bool				m_bPostGMInit;
	RESULT_INFO			m_stExpectedDestInfo;

	std::vector<int>	m_vecTestFieldID;
	MRegulator			m_rgltTimeout;
	MRegulator			m_rgltWarpDelay;

	BOT_PHASE			GetPhase() { return m_ePhase; }
	void				SetPhase(BOT_PHASE ePhase);

	bool InitTestFieldList();
	void InitTestDelayTime();
	void StartTimeoutTimer();
	void StopTimeoutTimer();

	void OnTestStarter();
	void OnTestChecker(RESULT_INFO* pResultInfo);
	void OnError(string strLog="");

	/// Command Handler
	MCommandResult OnFieldStartGame(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);
	MCommandResult OnFieldChangeField(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);
	MCommandResult OnFieldCancelChangeField(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);

};
