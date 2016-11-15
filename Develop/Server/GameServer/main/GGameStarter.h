#pragma once

#include "MTypes.h"
#include "GPlayerGameStart_State.h"

class GEntityPlayer;
class GPlayerGameStart_FieldGater;


class GGameStarter
{
private:

public:
	enum _Result
	{
		RESULT_SUCCESS = 0,					// 성공
		RESULT_FAILED_TO_FIND_PLAYER,		// 플레이어가 없다
		RESULT_FAILED_TO_GATE				// 필드 진입 실패
	};
	_Result		m_nResult;
private:
	void SetResult(_Result nResult) { m_nResult = nResult; }

	bool GateToField( GEntityPlayer* pPlayer, GPlayerGameStart_State& playerGameStartState, int nReqFieldID, vec3 vReqPos );
public:
	GGameStarter() : m_nResult(RESULT_SUCCESS) {}
	virtual ~GGameStarter() {}
	void Start(MUID uidPlayer, int nReqFieldID=-1, vec3 vReqPos=vec3::ZERO);
	_Result GetResult() { return m_nResult; }
};