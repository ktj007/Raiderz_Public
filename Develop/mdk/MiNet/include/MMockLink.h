#pragma once

#include "MiNetLib.h"
#include "MLink.h"
#include "MCommand.h"
#include "MSimpleCommandBinder.h"

namespace minet {

class MNetServer;

class MINET_API MMockLink : public MLink
{
protected:
	class Logger
	{
		struct _LogItem
		{
			bool				bSend;
			MCommand*			pCommand;
		};

		vector<_LogItem>		m_Items;
	public:
		~Logger();
		void LogCommand(bool bSend, MCommand* pCommand);
		void OutputLog(const char* szTitle);
		void Clear();
	};

private:
	bool IsIgnoredID(int nID);
	bool IsAllowedID(int nID);
protected:
	MCommand					m_NullCommand;
	vector<MCommand*>			m_vecRecevedCommands;
	set<int>					m_vecIgnoredID;
	set<int>					m_vecAllowedID;
	bool						m_bIgnoreAll;
	MNetServer*					m_pNetServer;
	bool						m_bCallDisconnect;
	bool						m_bCallHardDisconnect;

	Logger						m_Logger;
public:
	MMockLink(MUID uidNew, MNetServer* pNetServer, MServerTcpSocket& tcp_socket, MCommandManager* pCM);
	virtual ~MMockLink();
	virtual bool Disconnect() override		{ m_bCallDisconnect = true; return true; }
	virtual void HardDisconnect() override	{ m_bCallHardDisconnect = true; }
	virtual void Send(BYTE* buf, int buflen) { }
	virtual void OnSendCommand(MCommand& command);
	virtual MCommandResult OnRecvCommand(MCommand& command);

	size_t				GetCommandCount();
	MCommand&			GetCommand(size_t nIndex);
	int					GetCommandID( size_t nIndex);
	void				AddIgnoreID(int nID);
	void				EraseIgnoreID(int nID);
	void				IgnoreAll();
	void				ReleaseIgnoreAll();
	void				AddAllowedID(int nID);
	bool				IsCallDisconnect() { return m_bCallDisconnect; }
	bool				IsCallHardDisconnect() { return m_bCallHardDisconnect; }

	template<typename T>
	T GetParam(size_t nCmdIndex, int nParamIndex)
	{
		return cmd_binder::param<T>(&GetCommand(nCmdIndex), nParamIndex);
	}

	MCommandResult OnRecv(int nCmdID, int nParamCount, MCommandParameter* pCmdParam, ... );

	void OutputLog(const char* szTitle=NULL);

	bool IsExistCommand(int nCmdID);
	void ResetCommands();
	void PopFrontCommand();
};

} // namespace minet