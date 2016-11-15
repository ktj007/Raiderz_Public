#pragma once

#include "MCommand.h"
#include "XBirdClient.h"
#include "XDummyHandler.h"
#include "XConnectionManager.h"

class XDummyBot;
class XDummyCollision;
class XBirdDummyAgentMgr;
struct XDummyAgentInfo;

class XBirdClientWrapper : public XNetClientWrapper
{
protected:
	XBirdClient*		m_pBirdClient;
	XBirdDummyAgent*	m_pAgent;
public:
	XBirdClientWrapper(XBirdClient* pNetClient, XBirdDummyAgent* pAgent) : m_pBirdClient(pNetClient), m_pAgent(pAgent), XNetClientWrapper() {}

	virtual minet::MCommand* NewCmd(int nCmdID)
	{
		MUID uidTarget= m_pBirdClient->GetServerUID();
		return m_pBirdClient->NewCommand(nCmdID, uidTarget);
	}
	virtual bool Post(minet::MCommand* pCommand)
	{
		return m_pBirdClient->Post(pCommand);
	}
	virtual void Disconnect()
	{
		m_pBirdClient->Disconnect();
	}
	virtual const wchar_t* GetLoginID();
	virtual const wchar_t* GetPassword();
	virtual int GetDirectLoginFieldID();
	virtual vec3 GetStartPos();
};

class XBirdDummyAgent
{
	friend class XBirdClient;
	typedef list<XDummyBot*> BOTS;
public:
	enum XDummyAgentStatus
	{
		DAS_NOTCONNECTED=0,
		DAS_RUNNING,
		DAS_ERROR,
		DAS_COMPLETE,
	};
private:
protected:
	wstring				m_strName;		// 이 Agent의 이름
	int					m_nID;			// 이 Agent의 ID
	vec3				m_vCenter;		// 이 Agent의 시작 위치

	XBirdClient*		m_pClient;
	XConnectionManager*	m_pConnectionManager;
	XDummyCollision*	m_pCollision;
	XBirdDummyAgentMgr* m_pAgentMgr;

	wstring				m_strLoginID;

	wstring				m_strGameServer;
	wstring				m_strLoginServer;
	int					m_nLoginFieldID;
	int					m_nLoginChannelID;
	bool				m_bDirectLoginToGameServer;
	int					m_nPreStayFieldID;	// 필드이동 시, 방금전 머문 필드

	bool				m_bCreated;
	XDummyAgentStatus	m_eStatus;
	uint32				m_nConnCount;
	unsigned long int	m_nLastCommandTime;

	UIID				m_nUIID;
	MUID				m_UID;

	bool				m_bIsFirstAgent;	// 제일 처음 생성된 Agent 인지 체크. 최초 Agent 일 경우 커맨드 로그를 찍습니다.

	vector<XDummyHandler*>		m_vecHandler;
	BOTS				m_bots;
	BOTS				m_deletedBots;

	uint32				m_nLastCheckConnection;
	
	bool				m_bDisconnectForTest;
public:
	enum CON_STATE
	{
		REQUEST_CONNECT,
		CONNECTING,
		DISCONNECTING,
		DISCONNECTING_REQUEST_FAILED,
		MOVING_FROM_SERVER_TO_SERVER,
	};
protected:
	CON_STATE			m_eConState;

	void CheckConnection();
	void Connect(const wchar_t* const szIP, int nPort);
	void CommonResponseLogin( MCommand* pCommand);
	void OnCommMoveGameServer(MCommand* pCommand);

	friend class XBirdDummyAgentMgr;
	void OnConnect(MServerTcpSocket& tcp_socket);
	void OnDisconnect();
	void OnRecv(const BYTE* const data,const DWORD& dataLen);
	void OnSend(DWORD bytes_transferred);

public:
	XBirdDummyAgent();
	virtual ~XBirdDummyAgent();
	void Create(const XDummyAgentInfo& agentInfo, int nAgnetID, const wchar_t* szLoginID, const wchar_t* szGameServer, const wchar_t* szLoginServer, XBirdDummyAgentMgr* pAgentMgr);
	MCommandResult OnCommand(XBirdClient* pClient, MCommand* pCmd);
	void Run(float fDelta);

	void PostConnectToGameServer();
	void PostConnectToLoginServer();
	void PostDisconnect();


	void AddCommandHandler(XDummyHandler* pHandler);
	void AddBot(XDummyBot* const pBot);
	void DelBot(XDummyBot* const pBot);

	bool IsStayTrialField(void);

	const wchar_t* GetName()		{ return m_strName.c_str(); }
	const wchar_t* GetLoginID()		{ return m_strLoginID.c_str(); }
	void SetCenter(vec3 vPos)		{ m_vCenter = vPos; }
	vec3 GetCenter()				{ return m_vCenter; }
	XBirdClient* GetClient()		{ return m_pClient; }
	size_t GetHandlerCount()		{ return m_vecHandler.size(); }
	XDummyAgentStatus GetStatus()	{ return m_eStatus; }
	void SetStatusNotConnected()	{ m_eStatus = DAS_NOTCONNECTED; }
	void SetStatusError()			{ m_eStatus = DAS_ERROR; }
	void SetStatusRunning()			{ m_eStatus = DAS_RUNNING; }
	void SetStatusComplete()		{ m_eStatus = DAS_COMPLETE; }
	int GetID()						{ return m_nID; }
	uint32 GetConnCount()			{ return m_nConnCount; }

	MUID GetUID()					{ return m_UID; }
	UIID GetUIID()					{ return m_nUIID; }
	XConnectionManager* GetConnectionManager() { return m_pConnectionManager; }
	void SetLoginField(int nFieldID, int nChannelID, vec3 vStartPos);
	int GetLoginChannelID()					{ return m_nLoginChannelID; }
	int GetLoginFieldID()					{ return m_nLoginFieldID; }
	vec3 GetLoginFieldStartPos()			{ return m_vCenter; }
	void SetDirectLoginToGameServer(bool b)	{ m_bDirectLoginToGameServer = b; }	
	int GetPreStayFieldID(void)				{ return m_nPreStayFieldID;	}
	void SetChannelID(int nChannelID)		{ m_nLoginChannelID = nChannelID; }

	wstring GetGameServer()					{ return m_strGameServer; }

	void SetUID(MUID uidNew)		{ m_UID = uidNew; }
	void SetUIID(UIID nUIID)		{ m_nUIID = nUIID; }


	XDummyCollision* GetCollision() { return m_pCollision; }
	float GetGroundZ(vec3 vPos);

	void SetFirstAgent()			{ m_bIsFirstAgent = true; }
	bool IsFirstAgent()				{ return m_bIsFirstAgent; }

	XBirdDummyAgentMgr* GetAgentMgr()	{ return m_pAgentMgr; }
	XDummyBot* GetFrontBot();

	void SetDisconnectForTest(bool bFlag=true) { m_bDisconnectForTest = bFlag; }
	bool IsDisconnectForTest() { return m_bDisconnectForTest; }

	bool IsOddNumberID();
	bool IsEvenNumberID();

	void SetConnectionState(CON_STATE state) { m_eConState = state; }
};


class XBirdDummyAgentMgr
{
private:
	typedef std::map<MUID,XBirdDummyAgent*>					AGENTS;
	typedef std::map<MServerTcpSocket*,XBirdDummyAgent*>	CONNECTING_AGENTS;

	vector<XBirdDummyAgent*>	m_vecAgents;
	AGENTS						m_mapAgents;
	int							m_nStartingID;
	CONNECTING_AGENTS			m_pendingConnectAgents;
	cml2::MCriticalSection		m_cs;

public:
	XBirdDummyAgentMgr();
	~XBirdDummyAgentMgr();
	void Run(float fDelta);
	void SetStartingID(int nStartingID) { m_nStartingID = nStartingID; }
	XBirdDummyAgent* AddAgent(const XDummyAgentInfo& agentInfo, const wchar_t* szLoginID, int nAgentID, const wchar_t* szGameServer, const wchar_t* szLoginServer);
	void Clear();
	void PostConnectToLoginServer();
	size_t GetSize() { return m_vecAgents.size(); }
	XBirdDummyAgent* GetAgent(size_t index) { assert(m_vecAgents.size() > index); return m_vecAgents[index]; }
	XBirdDummyAgent* GetAgentByID(int nID);
	
	bool Connect(const char* const addr,const char* const port,XBirdDummyAgent& agent);
	void OnConnect(MServerTcpSocket& tcp_socket);
	void OnDisconnect(const MUID& uid);
	void OnRecv(const MUID& uid,const BYTE* const data,const DWORD& dataLen);
	void OnSend(const MUID& uid,DWORD bytes_transferred);
};