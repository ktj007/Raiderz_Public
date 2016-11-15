#include "stdafx.h"
#include "STrayIcon.h"

STrayIcon::STrayIcon()
{

}

STrayIcon::~STrayIcon()
{

}

BOOL STrayIcon::Create(HWND hWnd, UINT uCallbackMessage, const wchar_t* szToolTip, HICON icon, UINT uID)
{
	m_tnd.cbSize = sizeof(NOTIFYICONDATA);
	m_tnd.hWnd = hWnd;
	m_tnd.uID = uID;
	m_tnd.hIcon = icon;
	m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_tnd.uCallbackMessage = uCallbackMessage;
	wcscpy_s(m_tnd.szTip, szToolTip);

	return Shell_NotifyIcon(NIM_ADD, &m_tnd);
}

BOOL STrayIcon::SetIcon(HICON hIcon)
{
	m_tnd.uFlags = NIF_ICON;
	m_tnd.hIcon = hIcon;

	return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

BOOL STrayIcon::SetTooltipText(LPCTSTR pszTip)
{
	m_tnd.uFlags = NIF_TIP;
	wcscpy_s(m_tnd.szTip, pszTip);

	return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

void STrayIcon::RemoveIcon()
{
	m_tnd.uFlags = 0;
	Shell_NotifyIcon(NIM_DELETE, &m_tnd);
}