#pragma once

#include "MiNetLib.h"
#include "MLink.h"
#include "MCommand.h"
#include "MSimpleCommandBinder.h"
#include "MNetClient.h"

namespace minet {

class MTestNetAgent;

class MINET_API MMockNetClient : public MNetClient
{
	friend class MTestNetAgent;
private:
	bool m_bIsConnected;

protected:
	MTestNetAgent*				m_pAgent;

	virtual void OnReplyConnect(MUID& uidHost, MUID& uidAlloc, unsigned int nTimeStamp){}
	virtual void OnPrepareCommand(MCommand* pCommand){}
	virtual void SendCommand(MCommand* pCommand) {}
	virtual MCommandResult OnCommand(MCommand* pCommand);

public:
	MMockNetClient(const MNetClientDesc& desc);
	virtual ~MMockNetClient() {}
	virtual bool Post(MCommand* pCommand);
	virtual int Connect(const char* szRemoteIP, int nPort, const char* szMyNetworkCardIP = "") override { m_bIsConnected = true; return 1; }
	virtual void Disconnect() { m_bIsConnected = false; }
	virtual bool IsConnected() override { return m_bIsConnected; }

	void LinkToAgent(MTestNetAgent* pAgent);
	void UnlinkToAgent();

};

class MINET_API MTestNetAgent
{
private:
	friend class MMockNetClient;

	MMockNetClient*		m_pMockNetClient;

	class Logger
	{
		struct _LogItem
		{
			bool				bSend;
			minet::MCommand*	pCommand;
		};

		vector<_LogItem>		m_Items;
	public:
		~Logger();
		void LogCommand(bool bSend, minet::MCommand* pCommand);
		void OutputLog(const char* szTitle);
		void Clear();
	};

	Logger						m_Logger;
	vector<minet::MCommand*>	m_vecSentCommands;
	minet::MCommand				m_NullCommand;
	set<int>					m_setIgnoreCmdID;

	void OnSend(MCommand* pCommand);
	bool IsIgnoreCmdID( int nCmdID )
	{
		return (m_setIgnoreCmdID.find(nCmdID) != m_setIgnoreCmdID.end());
	}
public:
	MTestNetAgent(MNetClient* pNetClient=NULL);
	~MTestNetAgent();
	minet::MCommand& GetSendCommand( size_t nIndex );
	size_t GetSendCommandCount();
	int GetSendCommandID(size_t nIndex);
	bool IsExistSendCommand( int nCmdID );
	void ResetSendCommands();
	void OutputLog(const char* szTitle=NULL);


	template<typename T>
	T GetSendParam(size_t nCmdIndex, int nParamIndex)
	{
		return cmd_binder::param<T>(&GetSendCommand(nCmdIndex), nParamIndex);
	}

	MCommandResult OnRecv(int nCmdID, int nParamCount, minet::MCommandParameter* pCmdParam, ... );
	MCommandResult OnRecv(MCommand* pCommand);

	void IgnoreCmd( int nCmdID );
	void ResetIgnoreCmd();

	MUID GetUID();
	MUID GetServerUID();

};



} // namespace minet