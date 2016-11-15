#include "stdafx.h"
#include "XFeedbackSender.h"
#include <ShellAPI.h>


#define FEEDBACK_LOCAL_FILENAME			"feedback.txt"

void XFeedbackSender::SaveFile()
{
	uint32 nNowTime = timeGetTime();

	FILE* fp = fopen(FEEDBACK_LOCAL_FILENAME, "wt");
	if (fp == NULL) return;


	fprintf(fp, "%s\n", MLocale::ConvUTF16ToAnsi(m_InputData.strUserID.c_str()).c_str());
	fprintf(fp, "%s\n", MLocale::ConvUTF16ToAnsi(m_InputData.strCharName.c_str()).c_str());
	fprintf(fp, "%d\n", m_InputData.nFieldID);
	fprintf(fp, "%.1f %.f %.1f\n", m_InputData.vPos.x, m_InputData.vPos.y, m_InputData.vPos.z);
	fprintf(fp, "%.1f %.f %.1f\n", m_InputData.vDir.x, m_InputData.vDir.y, m_InputData.vDir.z);

	fprintf(fp, "%s\n", MLocale::ConvUTF16ToAnsi(m_InputData.strTag.c_str()).c_str());
	fprintf(fp, "%s\n", MLocale::ConvUTF16ToAnsi(m_InputData.strText.c_str()).c_str());

	fclose(fp);	
}

void XFeedbackSender::Send( const wchar_t* szUserID,
						   const wchar_t* szCharName,
						   int nFieldID,
						   vec3 vPos,
						   vec3 vDir,
						   const wchar_t* szTag,
						   const wchar_t* szText )
{
	m_InputData.strUserID = szUserID;
	m_InputData.strCharName = szCharName;
	m_InputData.nFieldID = nFieldID;
	m_InputData.vPos = vPos;
	m_InputData.vDir = vDir;
	m_InputData.strTag = szTag;
	m_InputData.strText = szText;

	SaveFile();
	SendFile();
}

void XFeedbackSender::SendFile()
{
	time_t currtime;
	time(&currtime);
	struct tm* pTM = localtime(&currtime);

	wchar_t szPlayer[MAX_COMPUTERNAME_LENGTH];
	wsprintf(szPlayer, L"%s", L"test");

	// BAReport ½ÇÇà
	wchar_t szCmd[4048];
	wchar_t szRemoteFileName[_MAX_DIR];

	swprintf_s(szRemoteFileName, L"feedback/%04d%02d%02d_%02d%02d%02d_%s.txt",
		pTM->tm_year + 1900,
		pTM->tm_mon+1, pTM->tm_mday, pTM->tm_hour, pTM->tm_min, pTM->tm_sec, szPlayer);


	swprintf_s(szCmd, L"app=%s;addr=%s;port=21;id=dev;passwd=dev;user=%s;localfile=%s;remotefile=%s;srcdelete=1;agree=1",
		GAME_NAME_STR_LOWER, L"icecream", szPlayer, FEEDBACK_LOCAL_FILENAME, szRemoteFileName);


	::ShellExecute(0, NULL, L"BAReport.exe", szCmd, NULL, SW_HIDE);

}