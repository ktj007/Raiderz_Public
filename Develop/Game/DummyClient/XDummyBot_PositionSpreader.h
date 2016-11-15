#pragma once

#include "XDummyBot.h"

class XBirdClient;
class XFieldInfo;

class XDummyBot_PositionSpreader : public XDummyBot
{
	//////////////////////////////////////////////////////////////////////////
	/// 테스트를 위한 정적 초기값들
	static int			m_nDestFieldID;
	static float		m_fFieldMin_x;
	static float		m_fFieldMin_y;
	static float		m_fFieldMax_x;
	static float		m_fFieldMax_y;
	static float		m_fSpaceLength;
	static vec3			m_vTargetPos;
	//////////////////////////////////////////////////////////////////////////

	enum BOT_PHASE
	{
		PHASE_0_STARTING,
		PHASE_1_CHECK,
		PHASE_2_COMPLETE,
	};

	struct RESULT_INFO
	{
		int		nFieldID;
		vec3	vTargetPos;

		RESULT_INFO() { Reset(); }
		void Reset()
		{
			nFieldID = INVALID_ID;
			vTargetPos.Set(0, 0, 0);
		}
	};

	BOT_PHASE			m_ePhase;
	RESULT_INFO			m_stExpectedDestInfo;
	MRegulator			m_rgltTimeout;

private:

	bool InitSpreader(XBirdDummyAgent* pAgent);

	void OnRun(float fDelta) override;

	minet::MCommandResult OnFieldStartGame(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);

public:
	XDummyBot_PositionSpreader(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	virtual ~XDummyBot_PositionSpreader() {}

	virtual MCommandResult	OnCommand(XBirdDummyAgent* pAgent, MCommand* pCmd) override;
	static const wchar_t*	GetName() { return L"PositionSpreader"; }
	void					OnError(string strLog);
};
