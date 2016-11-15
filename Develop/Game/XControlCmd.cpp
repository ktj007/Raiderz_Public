#include "stdafx.h"
#include "XControlCmd.h"

XControlCmd::XControlCmd( CMD_TYPE _eType, const wchar_t* pSzCmdName ) : m_eCmdType(_eType)
, m_strCmdName(pSzCmdName)
, m_pMovableGoParam(NULL)
, m_fElapsed(0.f)
, m_bDone(false)
{
	m_nTickNum = rs3::REngine::GetDevice().GetFrameCount();
}

XControlCmd::XControlCmd( CMD_TYPE _eType, XMovableGoParam* pMovableGoParam ) : m_eCmdType(_eType)
, m_fElapsed(0.f)
, m_bDone(false)
{
	m_nTickNum = rs3::REngine::GetDevice().GetFrameCount();
	m_pMovableGoParam = pMovableGoParam;
}

XControlCmd::~XControlCmd()
{
	delete m_pMovableGoParam;
}

const wchar_t* XControlCmd::GetDebugCmdInfo()
{
	if (m_strDebugInfo.empty())
	{
		std::wstring strType(m_strCmdName);
		std::wstring strMiddleMessage(L"IT IS MOVEMENT COMMAND : ");

		switch (m_eCmdType)
		{
		case E_DEBUG_CMD:
			{
				strMiddleMessage = L"just debug : ";
			}
			break;
		case E_MOVE:
			{
				strType = L"MOVE";
			}
			break;
		case E_MOVESTOP:
			{
				strType = L"MOVE_STOP";
			}
			break;
		case E_STOP:
			{
				strType = L"STOP";
			}
			break;
		}

		m_strDebugInfo = L"History Debug, " + strMiddleMessage + strType + L" command";
	}

	return m_strDebugInfo.c_str();
}
