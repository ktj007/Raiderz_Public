#pragma once

class GDebugReporter
{
public:
	struct Record
	{
		MUID			uidNPC;
		wstring			strName;
		vector<wstring>	vecLogs;

		bool IsRecorded()	{ return uidNPC.IsValid(); }
	};
public:
	GDebugReporter(void);
	~GDebugReporter(void);
	
	// 레코드 관리 -----------------------
	Record* FindRecord(MUID uidNPC);
	void SetReportName( wstring strName );
	void SetRecordLimit(int nLimit);
	void Push(Record record);
	size_t GetRecordQty();
	Record GetRecord(size_t nIndex);

	// 로그 ------------------------------
	void Log(MUID uidNPC, wstring strText);
	void FlushToFile();
private:
	// 현재 시간 반환
	std::wstring WriteTimeStamp() const;
	
private:
	deque<Record>	m_vecRecords;
	int				m_nRecordLimit;
	wstring			m_strName;
};
