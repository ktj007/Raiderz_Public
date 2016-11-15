#pragma once

#include "MNetClient.h"
#include "MCommandHandler.h"
#include "MCommand.h"

using namespace minet;

class XRecorder;
class XCmdHandlerGroup;
class XRequestCommandFilter;
class XWhoIsRequester;
class XControlNetCmdState;

class XNetClient : public MNetClient
{
private:
	friend class XReplay;

	XRecorder*					m_pRecorder;
	XControlNetCmdState*		m_pControlNetCmdState;
protected:
	XCmdHandlerGroup*			m_pCmdHandlerGroup;
	XRequestCommandFilter*		m_pRequestCommandFilter;
	XWhoIsRequester*			m_pWhoIsRequester;
protected:
	virtual void			OnReplyConnect(MUID& uidHost, MUID& uidAlloc, unsigned int nTimeStamp);
	virtual MCommandResult OnCommand(MCommand* pCommand);
	virtual void OnPrepareCommand(MCommand* pCommand);
	virtual void			SendCommand(MCommand* pCommand);
	void LogCommand(MCommand* pCommand);
public:
	XNetClient(const MNetClientDesc& desc);
	virtual ~XNetClient();
	virtual bool Post(MCommand* pCommand);
	void SetRecorder(XRecorder* pRecorder)	{ m_pRecorder = pRecorder; }
	void ReleaseRecorder()					{ m_pRecorder = NULL; }
	XRecorder* GetRecorder()				{ return m_pRecorder; }
	XWhoIsRequester* GetWhoIsRequester()	{ return m_pWhoIsRequester; }
	void SetControlNetCmdState(XControlNetCmdState* pCmdState);
};

bool XPostCommand(MCommand* pCmd);
MCommand* XNewCmd(int nCmdID);

// Post Command Macro For Convenience
#define XPOSTCMD0(_ID)										{ MCommand* pC=XNewCmd(_ID); XPostCommand(pC); }
#define XPOSTCMD1(_ID, _P0)									{ MCommand* pC=XNewCmd(_ID); pC->AP(_P0); XPostCommand(pC); }
#define XPOSTCMD2(_ID, _P0, _P1)							{ MCommand* pC=XNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); XPostCommand(pC); }
#define XPOSTCMD3(_ID, _P0, _P1, _P2)						{ MCommand* pC=XNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); XPostCommand(pC); }
#define XPOSTCMD4(_ID, _P0, _P1, _P2, _P3)					{ MCommand* pC=XNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); XPostCommand(pC); }
#define XPOSTCMD5(_ID, _P0, _P1, _P2, _P3, _P4)				{ MCommand* pC=XNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); XPostCommand(pC); }
#define XPOSTCMD6(_ID, _P0, _P1, _P2, _P3, _P4, _P5)		{ MCommand* pC=XNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); pC->AP(_P5); XPostCommand(pC); }
#define XPOSTCMD7(_ID, _P0, _P1, _P2, _P3, _P4, _P5, _P6)	{ MCommand* pC=XNewCmd(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); pC->AP(_P5); pC->AP(_P6); XPostCommand(pC); }


inline MCommand* XMakeCMD0(int _ID) 
{ MCommand* pC=XNewCmd(_ID); return pC; };

inline MCommand* XMakeCMD1(int _ID, MCommandParameter* _P0)
{ MCommand* pC=XNewCmd(_ID); pC->AddParameter(_P0); return pC; }

inline MCommand* XMakeCMD2(int _ID, MCommandParameter* _P0, MCommandParameter* _P1)
{ MCommand* pC=XNewCmd(_ID); pC->AddParameter(_P0); pC->AddParameter(_P1); return pC; }

inline MCommand* XMakeCMD3(int _ID, MCommandParameter* _P0, MCommandParameter* _P1, MCommandParameter* _P2)						
{ MCommand* pC=XNewCmd(_ID); pC->AddParameter(_P0); pC->AddParameter(_P1); pC->AddParameter(_P2); return pC; }

inline MCommand* XMakeCMD4(int _ID, MCommandParameter* _P0, MCommandParameter* _P1, MCommandParameter* _P2, MCommandParameter* _P3)					
{ MCommand* pC=XNewCmd(_ID); pC->AddParameter(_P0); pC->AddParameter(_P1); pC->AddParameter(_P2); pC->AddParameter(_P3); return pC; }

inline MCommand* XMakeCMD5(int _ID, MCommandParameter* _P0, MCommandParameter* _P1, MCommandParameter* _P2, MCommandParameter* _P3, MCommandParameter* _P4)				
{ MCommand* pC=XNewCmd(_ID); pC->AddParameter(_P0); pC->AddParameter(_P1); pC->AddParameter(_P2); pC->AddParameter(_P3); pC->AddParameter(_P4); return pC; }

inline MCommand* XMakeCMD6(int _ID, MCommandParameter* _P0, MCommandParameter* _P1, MCommandParameter* _P2, MCommandParameter* _P3, MCommandParameter* _P4, MCommandParameter* _P5)		
{ MCommand* pC=XNewCmd(_ID); pC->AddParameter(_P0); pC->AddParameter(_P1); pC->AddParameter(_P2); pC->AddParameter(_P3); pC->AddParameter(_P4); pC->AddParameter(_P5); return pC; }

inline MCommand* XMakeCMD7(int _ID, MCommandParameter* _P0, MCommandParameter* _P1, MCommandParameter* _P2, MCommandParameter* _P3, MCommandParameter* _P4, MCommandParameter* _P5, MCommandParameter* _P6)	
{ MCommand* pC=XNewCmd(_ID); pC->AddParameter(_P0); pC->AddParameter(_P1); pC->AddParameter(_P2); pC->AddParameter(_P3); pC->AddParameter(_P4); pC->AddParameter(_P5); pC->AddParameter(_P6); return pC; }

inline MCommand* XPOSTCMD(int nCmdID, MCommandParameter* pCmdParam, ... )
{
	MCommand* pC=XNewCmd(nCmdID);

	va_list marker;
	MCommandParameter* pParam = pCmdParam;

	va_start( marker, pCmdParam );     /* Initialize variable arguments. */
	while( pParam->GetType() < MPT_END )
	{
		pC->AddParameter(pParam);
		pParam = va_arg( marker, MCommandParameter*);
	}
	va_end( marker );              /* Reset variable arguments.      */

	XPostCommand(pC);
}

