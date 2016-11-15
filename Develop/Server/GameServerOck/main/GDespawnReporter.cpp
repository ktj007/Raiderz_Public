#include "StdAfx.h"
#include "GDespawnReporter.h"
#include "GDef.h"

GDebugReporter::GDebugReporter(void)
: m_nRecordLimit(0)
{
}

GDebugReporter::~GDebugReporter(void)
{
}

void GDebugReporter::SetReportName( wstring strName )
{
	m_strName = strName;
}

void GDebugReporter::SetRecordLimit( int nLimit )
{
	m_nRecordLimit = nLimit;
}

void GDebugReporter::Push( Record record )
{
	VALID(m_nRecordLimit > 0);

	if (m_vecRecords.size() >= (size_t)m_nRecordLimit)
	{
		m_vecRecords.pop_front();
	}

	m_vecRecords.push_back(record);

	DCHECK(m_vecRecords.size() <= (size_t)m_nRecordLimit);
}

size_t GDebugReporter::GetRecordQty()
{
	return m_vecRecords.size();
}

GDebugReporter::Record GDebugReporter::GetRecord( size_t nIndex )
{
	GDebugReporter::Record r;
	VALID_RET(nIndex >= 0, r);
	VALID_RET(nIndex < m_vecRecords.size(), r);

	r = m_vecRecords.at(nIndex);
	DCHECK(r.IsRecorded());

	return r;
}

void GDebugReporter::Log( MUID uidNPC, wstring strText )
{
	Record* pRecord = FindRecord(uidNPC);
	if (!pRecord)
		return;

	pRecord->vecLogs.push_back(strText);
}

GDebugReporter::Record* GDebugReporter::FindRecord( MUID uidNPC )
{
	for (deque<Record>::iterator it = m_vecRecords.begin();
		 it != m_vecRecords.end();
		 ++it)
	{
		if (it->uidNPC == uidNPC)
			return &(*it);
	}
	
	return NULL;
}

void GDebugReporter::FlushToFile()
{
	mlog("NPC %s Report ------------\n", MLocale::ConvUTF16ToAnsi(m_strName.c_str()).c_str());
	mlog("===============================\n");
	for each (const Record& each in m_vecRecords)
	{
		mlog("Name | %s\n", MLocale::ConvUTF16ToAnsi(each.strName.c_str()).c_str());
		mlog("UID  | %I64d\n", each.uidNPC);
		mlog("Logs>\n");

		for each (const std::wstring& log in each.vecLogs)
		{
			mlog("\t%s %s\n", MLocale::ConvUTF16ToAnsi(WriteTimeStamp().c_str()).c_str(), MLocale::ConvUTF16ToAnsi(log.c_str()).c_str());
		}

		mlog("\n");
	}
}

std::wstring GDebugReporter::WriteTimeStamp() const
{
	SYSTEMTIME timeStamp;
	GetLocalTime(&timeStamp);

	static wchar_t szTime[256] = L"";
	swprintf_s(szTime, L"[%02d:%02d:%02d.%03d]"
		, timeStamp.wHour
		, timeStamp.wMinute
		, timeStamp.wSecond
		, timeStamp.wMilliseconds);
	return std::wstring(szTime);
}