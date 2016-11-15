#pragma once

#include "REngine.h"
#include "RDevice.h"
#include "XMovableGoParam.h"
#include "MemPool.h"

class XControlCmd : public MMemPool<XControlCmd>
{
public:
	enum CMD_TYPE
	{
		E_DEBUG_CMD = 0,

		// Move 관련 커맨드
		E_MOVE,
		E_MOVESTOP,
		E_STOP,

		E_INTEPOLATION_MOTION_STOP
	};

	XControlCmd(CMD_TYPE _eType, const wchar_t* pSzCmdName);
	XControlCmd(CMD_TYPE _eType, XMovableGoParam* pMovableGoParam);
	virtual ~XControlCmd();

	// 공용함수
	unsigned int			GetTickNum() const			{ return m_nTickNum; }
	CMD_TYPE				GetType() const				{ return m_eCmdType; }
	XMovableGoParam*		GetMovableGoParam() const	{ return m_pMovableGoParam; }

	void					AddElapsed(float fDelta)	{ m_fElapsed+= fDelta;}
	float					GetElapsed() const			{ return m_fElapsed; }

	void					SetDone() { m_bDone = true; }
	bool					IsDone() { return m_bDone; }

	// 디버깅 정보
	const wchar_t* GetDebugCmdInfo();

protected:
	CMD_TYPE				m_eCmdType;
	unsigned int			m_nTickNum;
	XMovableGoParam*		m_pMovableGoParam;
	bool					m_bDone;

	std::wstring				m_strCmdName;
	std::wstring				m_strDebugInfo;

	float					m_fElapsed;
};
