#ifndef _TRAYICON_H
#define _TRAYICON_H

#include <shellapi.h>

/// 어플리케이션 트레이 아이콘
class CTrayIcon
{
private:
protected:
	NOTIFYICONDATA	m_tnd;
public:
	CTrayIcon();
	virtual ~CTrayIcon();

	BOOL Create(HWND hWnd, UINT uCallbackMessage, LPCTSTR szToolTip, HICON icon, UINT uID);
	BOOL SetIcon(HICON hIcon);
	BOOL SetTooltipText(LPCTSTR pszTip);
	void RemoveIcon();
};
#endif
