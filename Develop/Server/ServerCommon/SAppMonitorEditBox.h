#pragma once

#include "MTime.h"
#include "MTstring.h"

// 모니터 에디트 박스
class SAppMonitorEditBox
{
private:
	int				m_nMaxLineCount;
	bool			m_bVisible;
	vector<wstring>	m_Lines;
	HWND			m_hRichEdMonitor;
	MRegulator		m_UpdateRglt;
public:
	SAppMonitorEditBox(HWND nHwnd, int nMaxLineCount=18) : m_hRichEdMonitor(nHwnd), m_nMaxLineCount(nMaxLineCount)
	{
		for (int i = 0; i < m_nMaxLineCount; i++)
		{
			m_Lines.push_back(L"");
		}

		const float MONITOR_TICK_TIME = 0.5f;
		m_UpdateRglt.SetTickTime(MONITOR_TICK_TIME);
	}
	void Update(float fDelta)
	{
		if (!m_UpdateRglt.IsReady(fDelta)) return;

		wchar_t szBuf[65535] = L"";

		for (int i = 0; i < m_nMaxLineCount; i++)
		{
			wcscat_s(szBuf, L" ");
			wcscat_s(szBuf, m_Lines[i].c_str());
			wcscat_s(szBuf, L"\r\n");
		}

		int nLength = GetWindowTextLength(m_hRichEdMonitor);
		SendMessage( m_hRichEdMonitor, EM_SETSEL, 0, nLength );
		SendMessage( m_hRichEdMonitor, EM_REPLACESEL, FALSE, (LPARAM)szBuf );
	}
	void SetText(int nLine, const wchar_t* szText)
	{
		if (nLine < 0 || nLine >= m_nMaxLineCount) return;

		m_Lines[nLine] = wstring(szText);
	}
};