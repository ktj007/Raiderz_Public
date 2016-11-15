#include "stdafx.h"
#include "MCommandProfiler.h"
#include "MDebug.h"
#include "MCommand.h"
#include "MPacket.h"

namespace minet {

MCommandProfiler::MCommandProfiler() : m_bInited(false)
{
	m_Items.resize(MAX_COMMANDPROFILER_CMD_COUNT);

	m_pCM = NULL;
	Reset();
}

MCommandProfiler::~MCommandProfiler()
{

}

void MCommandProfiler::Reset()
{
	 m_fTotalOnProcessTime = 0.0f;

	m_nTotalCmdCount[_PIT_SEND] = 0;
	m_nTotalCmdCount[_PIT_RECV] = 0;
	m_nTotalBytes[_PIT_SEND] = 0;
	m_nTotalBytes[_PIT_RECV] = 0;

	 for (size_t i = 0; i < m_Items.size(); ++i)
	 {
		 m_Items[i].Clear();
	 }
}

void MCommandProfiler::Analysis(const char* szFolder, const char* szFileName, bool bSaveCSV)
{
	if (!IsInited()) return;

	char szFullFileName[_MAX_PATH];
	sprintf_s(szFullFileName, "%s%s.txt", szFolder, szFileName);

	FILE* fp;
	errno_t err = fopen_s(&fp, szFullFileName, "wt");
	if ( 0 != err ) return;

	FILE* fpCsv = NULL;
	if (bSaveCSV)
	{
		sprintf_s(szFullFileName, "%s%s.csv", szFolder, szFileName);
		fopen_s(&fpCsv, szFullFileName, "wt");
	}

	PrintTitle(fp);
	PrintSummary(fp);
	PrintCmdItems(fp, fpCsv);

	fclose(fp);
	if (fpCsv) fclose(fpCsv);

	Reset();
}

void MCommandProfiler::PrintTitle(FILE* fp)
{
	fputs("----------------------------------------------------------------------------------------------------------------------------------------\n", fp);
	fputs("| Command Profile                                                                                                                      |\n", fp);
	fputs("----------------------------------------------------------------------------------------------------------------------------------------\n", fp);
}

static char* AddCommaToNum(int num, char *buf, int buflen)
{ 
	char strNum[128];
	_itoa_s(num, strNum, 10);
	int len = (int)strlen(strNum); 
	char* str = strNum;

	char* saved = buf;

//	assert(len > 0); 
//	assert(buflen >= len + ((len - 1) / 3) + 1); 

	switch ((len - 1) % 3) { 
		case 3: /* fake label to make gcc happy */ 
			while (*str) { 
					*buf++ = ','; 
				case 2: *buf++ = *str++; 
				case 1: *buf++ = *str++; 
				case 0: *buf++ = *str++; 
			} 
	} 
	*buf = '\0'; 

	return saved;
} 

void MCommandProfiler::PrintSummary(FILE* fp)
{
	// 측정 시간
	// 총 Send 커맨드 개수, 초당 개수, bytes, 비율, 초당 bytes
	// 총 Recv 커맨드 개수, 초당 개수, bytes, 비율, 초당 bytes

	unsigned int nTotalTimeSec = (unsigned int)(m_timeTotal.Check()*0.001f);
	if (nTotalTimeSec == 0) nTotalTimeSec=1;

	unsigned int nTotalCmdCount = m_nTotalCmdCount[_PIT_SEND] + m_nTotalCmdCount[_PIT_RECV];
	float fSendCmdCountRate=0.0f, fRecvCmdCountRate=0.0f;
	if (nTotalCmdCount != 0)
	{
		fSendCmdCountRate = (m_nTotalCmdCount[_PIT_SEND] / ((float)(nTotalCmdCount))) * 100.0f;
		fRecvCmdCountRate = (m_nTotalCmdCount[_PIT_RECV] / ((float)(nTotalCmdCount))) * 100.0f;
	}

	int nSendCmdCountPerSec = m_nTotalCmdCount[_PIT_SEND] / nTotalTimeSec;
	int nRecvCmdCountPerSec = m_nTotalCmdCount[_PIT_RECV] / nTotalTimeSec;

	unsigned int nTotalBytes = m_nTotalBytes[_PIT_SEND] + m_nTotalBytes[_PIT_RECV];
	float fSendByteRate=0.0f, fRecvByteRate=0.0f;
	if (nTotalBytes!=0)
	{
		fSendByteRate = (m_nTotalBytes[_PIT_SEND] / ((float)(nTotalBytes))) * 100.0f;
		fRecvByteRate = (m_nTotalBytes[_PIT_RECV] / ((float)(nTotalBytes))) * 100.0f;
	}

	int nSendBytePerSec = m_nTotalBytes[_PIT_SEND] / nTotalTimeSec;
	int nRecvBytePerSec = m_nTotalBytes[_PIT_RECV] / nTotalTimeSec;

	char szTotalSendCmdCount[128]="", szTotalRecvCmdCount[128]="";
	char szTotalSendBytes[128]="", szTotalRecvBytes[128]="";

	AddCommaToNum(m_nTotalCmdCount[_PIT_SEND], szTotalSendCmdCount, 256);
	AddCommaToNum(m_nTotalBytes[_PIT_SEND], szTotalSendBytes, 256);
	AddCommaToNum(m_nTotalCmdCount[_PIT_RECV], szTotalRecvCmdCount, 256);
	AddCommaToNum(m_nTotalBytes[_PIT_RECV], szTotalRecvBytes, 256);

	fprintf(fp, "  측정 시간 = %u초\n", nTotalTimeSec);
	fprintf(fp, "  Send> Count=%s(%4.1f%%) , Count/Sec = %d \t, Bytes = %s(%4.1f%%) , Bytes/Sec = %d\n", 
				szTotalSendCmdCount,
				fSendCmdCountRate, 
				nSendCmdCountPerSec,
				szTotalSendBytes,
				fSendByteRate, 
				nSendBytePerSec);

	fprintf(fp, "  Recv> Count=%s(%4.1f%%) , Count/Sec = %d \t, Bytes = %s(%4.1f%%) , Bytes/Sec = %d\n", 
				szTotalRecvCmdCount, 
				fRecvCmdCountRate, 
				nRecvCmdCountPerSec,
				szTotalRecvBytes, 
				fRecvByteRate, 
				nRecvBytePerSec);
}

void MCommandProfiler::PrintCmdItems(FILE* fp, FILE* fpCSV)
{
	// send: 개수(비율), byte(비율), byte/sec

	if (fpCSV)
	{
		fputs("ID,Size,Count,CountRate,Count/Sec,Bytes/Sec,B/S Rate,AvgTime, AvgTime Rate,MinTime,MaxTime,설명\n", fpCSV);
	}

	for (int itemType = 0; itemType < _PIT_MAX; itemType++)
	{

		if (itemType == _PIT_SEND)
		{
			fputs("-Send-----------------------------------------------------------------------------------------------------------------------------------\n", fp);
		}
		else
		{
			fputs("-Recv-----------------------------------------------------------------------------------------------------------------------------------\n", fp);
		}

		fputs("  ID     |  Size  ||     Count      |  Count/Sec  |     Bytes/Sec     |     AvgTime     |MinTime(ms)|MaxTime(ms)|  설명\n", fp);
		fputs("----------------------------------------------------------------------------------------------------------------------------------------\n", fp);

		// 측정시간(초)
		unsigned int nTotalTimeSec = (unsigned int)(m_timeTotal.Check()*0.001f);
		if (nTotalTimeSec == 0) nTotalTimeSec=1;

		char text[512];
		for (int i = 0; i < MAX_COMMANDPROFILER_CMD_COUNT; i++)
		{
			if (m_Items[i].nCount[itemType] <= 0) continue;

			int nID = i;
			char szDesc[256] = "";
			if (m_pCM)
			{
				MCommandDesc* pDesc = MGetCommandDescMap()->GetCommandDescByID(i);
				if (pDesc)
					strcpy_s(szDesc, pDesc->GetName());
			}
			
			int nAvgSize = (m_Items[i].nBytes[itemType]) / (m_Items[i].nCount[itemType]);


			float fCountRate=0.0f;
			float fBytesRate=0.0f;

			// Count Rate
			if (m_nTotalCmdCount[itemType] != 0)
				fCountRate = (m_Items[i].nCount[itemType] / (float)m_nTotalCmdCount[itemType]) * 100.0f;

			// Count/Sec
			float fCountPerSec=0.0f;
			fCountPerSec = m_Items[i].nCount[itemType] / (float)nTotalTimeSec;

			// Bytes/Sec
			float fBytesPerSec=0.0f;
			fBytesPerSec = m_Items[i].nBytes[itemType] / (float)nTotalTimeSec;

			// Bytes Rate
			if (m_nTotalBytes[itemType] != 0)
				fBytesRate = (m_Items[i].nBytes[itemType] / (float)m_nTotalBytes[itemType]) * 100.0f;


			float nAvgTime = 0.0f;
			float nMinTime = 0.0f;
			float nMaxTime = 0.0f;
			float fTimeRate = 0.0f;

			if (m_Items[i].nCount[itemType] != 0)
				nAvgTime = m_Items[i].fTotalTime / m_Items[i].nCount[itemType];

			if (itemType == _PIT_RECV)
			{
				nMinTime = m_Items[i].fMinTime;
				nMaxTime = m_Items[i].fMaxTime;
			}

			if (m_fTotalOnProcessTime != 0.0f)
				fTimeRate = (m_Items[i].fTotalTime / m_fTotalOnProcessTime) * 100.0f;


			sprintf_s(text, "  %5d  |  %4d  || %6d(%4.1f%%)  |     %4.1f    |    %6.1f(%4.1f%%)  |   %6.4f(%4.1f%%) |  %6.4f   |  %6.4f   |%s\n",
				nID, 
				nAvgSize,
				m_Items[i].nCount[itemType], 
				fCountRate, 
				fCountPerSec,
				fBytesPerSec, 
				fBytesRate,
				nAvgTime, 
				fTimeRate, 
				nMinTime, 
				nMaxTime,
				szDesc);

			fputs(text, fp);
			fputs("----------------------------------------------------------------------------------------------------------------------------------------\n", fp);


			if (fpCSV)
			{
				sprintf_s(text, "%5d,%3d,%6d,%4.1f,%4.1f,%6.1f,%4.1f,%6.4f,%4.1f,%6.4f,%6.4f,%s %s\n",
					nID, 
					nAvgSize,
					m_Items[i].nCount[itemType], 
					fCountRate, 
					fCountPerSec,
					fBytesPerSec, 
					fBytesRate,
					nAvgTime, 
					fTimeRate, 
					nMinTime, 
					nMaxTime,
					(itemType == _PIT_SEND) ? "(S)" : "(R)",
					szDesc );

				fputs(text, fpCSV);
			}

		}
	} // for
}

void MCommandProfiler::OnCommandBegin(MCommand* pCmd)
{
	if (!IsInited()) return;
	
	if (!m_timeTotal.IsRunning())
	{
		m_timeTotal.Start();
	}

	int nCmdID = pCmd->GetID();
	if ((nCmdID <0) || (nCmdID >= MAX_COMMANDPROFILER_CMD_COUNT)) return;

	m_Items[nCmdID].TimeChecker.Start();
}

void MCommandProfiler::OnCommandEnd(MCommand* pCmd)
{
	if (!IsInited()) return;

	int nCmdID = pCmd->GetID();
	if ((nCmdID <0) || (nCmdID >= MAX_COMMANDPROFILER_CMD_COUNT)) return;

	float fPlayTime = (float)m_Items[nCmdID].TimeChecker.Check();
	m_Items[nCmdID].fTotalTime += fPlayTime;

	m_Items[nCmdID].fMaxTime = max(fPlayTime, m_Items[nCmdID].fMaxTime);
	m_Items[nCmdID].fMinTime = min(fPlayTime, m_Items[nCmdID].fMinTime);

	m_fTotalOnProcessTime += fPlayTime;
}

void MCommandProfiler::OnSend(MCommand* pCmd)
{
	if (!IsInited()) return;

	int nSize = pCmd->GetSize() + sizeof(MPacketHeader);
	m_nTotalBytes[_PIT_SEND] += (unsigned int)nSize;
	m_nTotalCmdCount[_PIT_SEND]++;

	int nCmdID = pCmd->GetID();
	if ((nCmdID <0) || (nCmdID >= MAX_COMMANDPROFILER_CMD_COUNT)) return;
	m_Items[nCmdID].nBytes[_PIT_SEND] += nSize;
	m_Items[nCmdID].nCount[_PIT_SEND]++;
}

void MCommandProfiler::OnRecv(MCommand* pCmd)
{
	if (!IsInited()) return;

	int nSize = pCmd->GetSize() + sizeof(MPacketHeader);
	m_nTotalBytes[_PIT_RECV] += (unsigned int)nSize;
	m_nTotalCmdCount[_PIT_RECV]++;

	int nCmdID = pCmd->GetID();
	if ((nCmdID <0) || (nCmdID >= MAX_COMMANDPROFILER_CMD_COUNT)) return;
	m_Items[nCmdID].nBytes[_PIT_RECV] += nSize;
	m_Items[nCmdID].nCount[_PIT_RECV]++;
}


void MCommandProfiler::Init(MCommandManager* pCM)
{
	m_pCM = pCM;

	Reset();

	m_bInited = true;

}

} // namespace minet