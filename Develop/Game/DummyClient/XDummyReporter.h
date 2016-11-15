#pragma once

class XDummyReporter;
class XBirdDummyAgentMgr;

class XDummyReporter
{
public:
	class XDummyTotalInfo
	{
		friend class XDummyReporter;
	private:
		uint32		m_nLastCheckTime;
	public:
		uint32		m_nTotalRunningTime;
		int64		m_nTotalSendBytes;
		int64		m_nTotalRecvBytes;
		uint32		m_nSecSendBytes;
		uint32		m_nSecRecvBytes;
		XDummyTotalInfo() : m_nLastCheckTime(0), m_nTotalRunningTime(0), m_nTotalSendBytes(0),
			m_nTotalRecvBytes(0), m_nSecSendBytes(0), m_nSecRecvBytes(0)
		{

		}
	};

	struct XDummyAgentInfo
	{
		wstring strAgentName;
		int m_nAgentCount;
		int m_nRunningAgentCount;
		uint32 m_nTotalRunningCount;
		uint32 m_nTotalSendBytes;
		uint32 m_nTotalRecvBytes;
		XDummyAgentInfo() { Clear(); }
		void Clear()
		{
			m_nAgentCount = 0;
			m_nRunningAgentCount = 0;
			m_nTotalRunningCount = 0;
			m_nTotalSendBytes = 0;
			m_nTotalRecvBytes = 0;
		}
	};
private:
	uint32				m_nLastUpdateTime;
	XBirdDummyAgentMgr*	m_pAgentMgr;
	XDummyTotalInfo		m_TotalInfo;
	vector<XDummyAgentInfo>	m_vecAgentInfo;
	void UpdateTotalInfo();
	void UpdateTotalPacketBytes();
	void UpdateAgentInfo();
public:
	XDummyReporter(XBirdDummyAgentMgr* pAgentMgr);
	~XDummyReporter();
	void Update();
	XDummyTotalInfo& GetTotalInfo() { return m_TotalInfo; }
	vector<XDummyAgentInfo>& GetAgentInfo() { return m_vecAgentInfo; }
};