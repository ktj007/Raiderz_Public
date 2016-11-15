#pragma once

#include "XBirdDummyAgent.h"
#include "XDummyPattern.h"

class XDummyPattern;
class XDummyFileLoader;
class XDummyReporter;
class XDummyLazyCommandPoster;

/// 더미 클라이언트 Config 클래스
struct XDummyConfig
{
	wstring	strGameServer;
	wstring	strLoginServer;
	wstring	strID;
	int		nStartingID;		///< TestID 를 몇번 ID 부터 시작할 것인지
	int		nRunningTime;		///< 더미 클라이언트 수행 시간(초 단위)
	bool	bDirectLoginToGameServer;
	bool	bAutoID;			///< 자동으로 ID를 만들지 여부
	XDummyConfig() : nRunningTime(60000000), bDirectLoginToGameServer(false), bAutoID(false)
	{

	}
};

struct XDummyAgentInfo
{
	wstring		strName;
	int			nCount;
	int			nFieldID;
	vec3		vCenter;
	float		fRadius;
	XDummyCollision* pCollision;
	vector<wstring>				strXmlPatterns;
	vector<wstring>				strGluePatterns;
	vector<wstring>				strBotPatterns;
	multimap<wstring, wstring>	mapValues;

	XDummyAgentInfo() : nCount(0), nFieldID(0), vCenter(vec3::ZERO), fRadius(1000.0f), pCollision(NULL) {}
	wstring GetValue(wstring strKey)
	{
		multimap<wstring, wstring>::iterator itor = mapValues.find(strKey);
		if (itor != mapValues.end()) return (*itor).second;
		return L"";
	}

	float GetFloatValue(wstring strKey)
	{
		wstring strValue = GetValue(strKey);
		float fValue = (float)_wtof(strValue.c_str());
		return fValue;
	}

	int GetIntValue(wstring strKey)
	{
		wstring strValue = GetValue(strKey);
		int fValue = (int)_wtoi(strValue.c_str());
		return fValue;
	}

	bool GetBoolValue(wstring strKey)
	{
		bool bValue = false;
		wstring strValue = GetValue(strKey);

		if (!strValue.empty())
		{
			if (!wcsicmp(strValue.c_str(), L"true"))
			{
				bValue = true;
			}
		}

		return bValue;
	}
};


class XDummyData
{
public:
	// info
	XDummyConfig					m_Config;
	vector<XDummyAgentInfo>			m_AgentInfos;
	XDummyReporter*					m_pReporter;

	// actor
	XBirdDummyAgentMgr				m_AgentMgr;
	XDummyHandlerMgr				m_GlueHandlerMgr;
	XDummyHandlerMgr				m_XmlHandlerMgr;
	XDummyPatternMgr				m_DummyPatternMgr;
private:
	void InitAgents();
	void InitGlueHandlers();
	void InitXmlHandlers();
	void InitGlobalInfo();
	XDummyBot* NewDummyBot(const wchar_t* szName, XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);

	void AttachHandlerToAgent();
public:
	XDummyData();
	~XDummyData();

	void InitFromInfo()
	{
#ifdef _PUBLISH
		return;
#endif

		InitAgents();
		InitGlueHandlers();
		InitXmlHandlers();
		InitGlobalInfo();
		AttachHandlerToAgent();		
	}

	XDummyAgentInfo* GetAgentInfo(const wchar_t* szName);
};

class XDummyConnectListener;
class XDummyDisconnectListener;
class XDummySendListener;
class XDummyRecvListener;
class XDummyLazyCommandPoster;

class XDummyMaster
{
private:
	XDummyData					m_Data;
	uint32						m_nStartTime;
	XDummyLazyCommandPoster*	m_pLazyCommmandPoster;
	XDummyConnectListener*		m_pConnectListener;
	XDummyDisconnectListener*	m_pDisconnectListener;
	XDummySendListener*			m_pSendListener;
	XDummyRecvListener*			m_pRecvListener;
	XDummyLazyCommandPoster*	m_pLazyCommandPoster;

	void Report();
public:
	XDummyMaster();
	~XDummyMaster();
	void Init(const wchar_t* szDummyFileName);
	void Start();
	void End();
	void Run(float fDelta);
	XDummyData* GetData() { return &m_Data; }

private:
	// io 처리
	friend class XDummyConnectListener;
	friend class XDummyDisconnectListener;
	friend class XDummySendListener;
	friend class XDummyRecvListener;
	void OnConnect(MServerTcpSocket& tcp_socket);
	void OnDisconnect(const MUID& uid);
	void OnRecv(const MUID& uid,const BYTE* const data,const DWORD& dataLen);
	void OnSend(const MUID& muid,DWORD bytes_transferred);
};
