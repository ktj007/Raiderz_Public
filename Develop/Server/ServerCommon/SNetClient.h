#pragma once

#include "ServerCommonLib.h"
#include "MNetClient.h"
#include "MCommandHandler.h"
using namespace minet;

class SCmdHandler_Net;

class SCOMMON_API SNetClient : public MNetClient
{
private:
protected:
	SCmdHandler_Net*			m_pCmdHandler;
protected:
	virtual void			OnReplyConnect(MUID& uidHost, MUID& uidAlloc, unsigned int nTimeStamp);
	virtual MCommandResult	OnCommand(MCommand* pCommand);
	virtual void			OnPrepareCommand(MCommand* pCommand);
	virtual void			SendCommand(MCommand* pCommand);
	void					LogCommand(MCommand* pCommand);
public:
	SNetClient(const MNetClientDesc& desc);
	virtual ~SNetClient();
	virtual bool Post(MCommand* pCommand);
};

bool SPostCommand(MCommand* pCmd);
MCommand* SNewCmd(int nCmdID);

// Post Command Macro For Convenience
#define SPOSTCMD0(_ID)										{ MCommand* pC=SNewCmd(_ID); SPostCommand(pC); }
#define SPOSTCMD1(_ID, _P0)									{ MCommand* pC=SNewCmd(_ID); pC->AP(_P0); SPostCommand(pC); }
#define SPOSTCMD2(_ID, _P0, _P1)							{ MCommand* pC=SNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); SPostCommand(pC); }
#define SPOSTCMD3(_ID, _P0, _P1, _P2)						{ MCommand* pC=SNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); SPostCommand(pC); }
#define SPOSTCMD4(_ID, _P0, _P1, _P2, _P3)					{ MCommand* pC=SNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); SPostCommand(pC); }
#define SPOSTCMD5(_ID, _P0, _P1, _P2, _P3, _P4)				{ MCommand* pC=SNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); SPostCommand(pC); }
#define SPOSTCMD6(_ID, _P0, _P1, _P2, _P3, _P4, _P5)		{ MCommand* pC=SNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); pC->AP(_P5); SPostCommand(pC); }
#define SPOSTCMD7(_ID, _P0, _P1, _P2, _P3, _P4, _P5, _P6)	{ MCommand* pC=SNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); pC->AP(_P5); pC->AP(_P6); SPostCommand(pC); }

inline MCommand* SPOSTCMD(int nCmdID, MCommandParameter* pCmdParam, ... )
{
	MCommand* pC=SNewCmd(nCmdID);

	va_list marker;
	MCommandParameter* pParam = pCmdParam;

	va_start( marker, pCmdParam );     /* Initialize variable arguments. */
	while( pParam->GetType() < MPT_END )
	{
		pC->AddParameter(pParam);
		pParam = va_arg( marker, MCommandParameter*);
	}
	va_end( marker );              /* Reset variable arguments.      */

	SPostCommand(pC);
}

