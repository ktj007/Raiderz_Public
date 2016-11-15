#include "StdAfx.h"
#include "XYouthSelfRegulation.h"
#include "XStrings.h"

#include "XController.h"

XYouthSelfRegulation::XYouthSelfRegulation(void)
: m_nHour(0)
{
}

XYouthSelfRegulation::~XYouthSelfRegulation(void)
{
}

void XYouthSelfRegulation::OnUpadate()
{
	unsigned int nTime = XGetNowTime();
	int nHour = nTime / (60*60*1000); //1½Ã°£
	if(m_nHour == nHour) return;
	m_nHour = nHour;

	wchar_t text[1024]=L"";
	swprintf_s(text, L"YOUTHSELFREGULATION_%d", min(nHour,7));
	
	wstring str = FormatString(XGetStr(text), FSParam(nHour));	
	global.ui->OnSystemMsg(str.c_str());
}
