#pragma once

class GField;
class GEntityNPC;
class GEntityPlayer;

class IBirdMonitorListener
{
public:
	virtual void OnOutput(int nLine, const wchar_t* szMsg) {}
};

// Bird가 테스트 용도로 사용하는 모니터
class GBirdMonitor
{
public:
	struct Entry
	{
		UIID nPlayerUIID;
		MUID uidField;
		UIID nTargetNPCUIID;
		set<NPCLogRecordType> setTypes;

		Entry()
		{
			setTypes.insert(NPCLOG_COMMON);
			setTypes.insert(NPCLOG_USETALENT);
			setTypes.insert(NPCLOG_MOVE);
			setTypes.insert(NPCLOG_COLT);
		}
	};

private:
	bool m_bReportToClient;
	vector<Entry> m_uidReportRequests;
	int m_nOutputLine;
	IBirdMonitorListener* m_pListener;

private:
	void ReportToClient( Entry entry, int nParam, const wchar_t* szText );
	void Output( Entry entry, const wchar_t* pFormat, ... );
	void FindEnemy( Entry entry, GField* pField );
	void OutputNPC( Entry entry, GEntityNPC* pEntityNPC );
	void OutputMyPlayer( Entry entry, GEntityPlayer* pEntityPlayer );

public:
	GBirdMonitor();
	~GBirdMonitor();

	void SetListener(IBirdMonitorListener* pListener) { m_pListener = pListener; }

	void Update();

	void AttachReportToClient(Entry entry);
	void DetachReportToClient(MUID uidRequest);
	bool IsAttached( MUID uidRequest );
	UIID GetTargetNPC(UIID nPlayerUIID);
	void SetTargetNPC( UIID nPlayerUIID, UIID nTargetNPCUIID );
	Entry* FindEntry(MUID uidRequest);

	static GBirdMonitor& GetInstance();
};

inline GBirdMonitor& BirdM()
{
	return GBirdMonitor::GetInstance();
}