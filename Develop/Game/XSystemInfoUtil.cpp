#include "stdafx.h"
#include "XSystemInfoUtil.h"
#include <dos.h>
#include "SVNRevision.h"
#include "App/version.h"
#include "CAppHelper.h"

#define SM_SERVERR2		89
#define STRING_BUFF_SIZE 128
#define MEMORY_DIVIDER	1024.0f  

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);


void XSystemInfoUtil::SetLogSystemInfo()
{
	wchar_t chOSVer[STRING_BUFF_SIZE] = {0, };
	wstring strRntime;
	wstring strCPUName;
	wstring strMemory;
	wstring strVideo;
	wstring strVideoMemory;

	GetRuntimeVersion(strRntime);
	GetOSVersion(chOSVer);
	GetCPUInfo(strCPUName);
	GetMemoryInfo(strMemory);
	GetVideoInfo(strVideo);
	GetVideoMemoryInfo(strVideoMemory);

	mlog("RunTime : %s\n\n", MLocale::ConvUTF16ToAnsi(strRntime.c_str()).c_str());
	mlog("OS : %s\n", MLocale::ConvUTF16ToAnsi(chOSVer).c_str());
	mlog("CPU : %s\n", MLocale::ConvUTF16ToAnsi(strCPUName.c_str()).c_str());
	mlog("Memory : %s\n", MLocale::ConvUTF16ToAnsi(strMemory.c_str()).c_str());
	mlog("Video : %s\n", MLocale::ConvUTF16ToAnsi(strVideo.c_str()).c_str());
	mlog("Video Memory : %s\n", MLocale::ConvUTF16ToAnsi(strVideoMemory.c_str()).c_str());

	mlog("\n");
}

int XSystemInfoUtil::GetOSVersion( wchar_t* pszBuff)
{
	int nMajorID = -1;

	memset( pszBuff, 0, sizeof( pszBuff));

	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	BOOL bOsVersionInfoEx;

	ZeroMemory( &si, sizeof( SYSTEM_INFO));
	ZeroMemory( &osvi, sizeof( OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFOEX);
	if ( !(bOsVersionInfoEx = GetVersionEx( (OSVERSIONINFO*) &osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO);
		if ( !GetVersionEx( (OSVERSIONINFO*)&osvi)) 
			return -1;
	}

	pGNSI = (PGNSI)GetProcAddress( GetModuleHandle( TEXT( "kernel32.dll")), "GetNativeSystemInfo");
	if ( NULL != pGNSI)		pGNSI( &si);
	else					GetSystemInfo( &si);

	nMajorID = osvi.dwMajorVersion;

	switch ( osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
		{
			if ( osvi.wProductType == VER_NT_WORKSTATION)					wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Windows 7 ");
			else															wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Windows Server 2008 \"R2\" ");
		}
		if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
		{
			if ( osvi.wProductType == VER_NT_WORKSTATION)					wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Windows Vista ");
			else															wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Windows Server 2008 ");
		}
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
		{
			if ( GetSystemMetrics( SM_SERVERR2))							wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Microsoft Windows Server 2003 \"R2\" ");
			else if ( osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
				wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Microsoft Windows XP Professional x64 Edition ");
			else															wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Microsoft Windows Server 2003, ");
		}
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)			wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Microsoft Windows XP ");
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)			wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Microsoft Windows 2000 ");
		if ( osvi.dwMajorVersion <= 4)										wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Microsoft Windows NT ");

		if ( bOsVersionInfoEx)
		{
			if ( osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture!=PROCESSOR_ARCHITECTURE_AMD64)
			{
				if ( osvi.dwMajorVersion == 4)								wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Workstation 4.0 ");
				else if ( osvi.wSuiteMask & VER_SUITE_PERSONAL)				wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Home Edition ");
				else														wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Professional ");
			}

			else if ( osvi.wProductType == VER_NT_SERVER || osvi.wProductType == VER_NT_DOMAIN_CONTROLLER)
			{
				if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
				{
					if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
					{
						if ( osvi.wSuiteMask & VER_SUITE_DATACENTER)		wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Datacenter Edition for Itanium-based Systems ");
						else if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE)	wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Enterprise Edition for Itanium-based Systems ");
					}
					else if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					{
						if ( osvi.wSuiteMask & VER_SUITE_DATACENTER)		wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Datacenter x64 Edition ");
						else if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE)	wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Enterprise x64 Edition ");
						else												wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Standard x64 Edition ");
					}
					else
					{
						if ( osvi.wSuiteMask & VER_SUITE_DATACENTER)		wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Datacenter Edition ");
						else if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE)	wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Enterprise Edition ");
						else if ( osvi.wSuiteMask & VER_SUITE_BLADE)		wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Web Edition ");
						else												wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Standard Edition ");
					}
				}
				else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
				{
					if ( osvi.wSuiteMask & VER_SUITE_DATACENTER)			wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Datacenter Server ");
					else if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE)		wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Advanced Server ");
					else													wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Server ");
				}
				else  // Windows NT 4.0 
				{
					if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE)			wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Server 4.0, Enterprise Edition ");
					else													wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Server 4.0 ");
				}
			}
		}

#define BUFSIZE		80
		else  
		{
			HKEY hKey;
			wchar_t szProductType[ BUFSIZE];
			DWORD dwBufLen = BUFSIZE * sizeof(wchar_t);
			LONG lRet;

			lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT( "SYSTEM\\CurrentControlSet\\Control\\ProductOptions"), 0, KEY_QUERY_VALUE, &hKey);
			if ( lRet != ERROR_SUCCESS)
				return -1;

			lRet = RegQueryValueEx( hKey, TEXT( "ProductType"), NULL, NULL, (LPBYTE) szProductType, &dwBufLen);
			RegCloseKey( hKey);

			if ( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE * sizeof(wchar_t)))
				return -1;

			if ( lstrcmpi( L"WINNT", szProductType) == 0)				wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Workstation ");
			if ( lstrcmpi( L"LANMANNT", szProductType) == 0)			wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Server ");
			if ( lstrcmpi( L"SERVERNT", szProductType) == 0)			wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Advanced Server ");


			wchar_t szTmp[ STRING_BUFF_SIZE];
			swprintf_s( szTmp, L"%d.%d ", osvi.dwMajorVersion, osvi.dwMinorVersion);
			wcscat_s( pszBuff, STRING_BUFF_SIZE, szTmp);
		}

		// Display service pack (if any) and build number.
		if ( osvi.dwMajorVersion == 4 && lstrcmpi( osvi.szCSDVersion, TEXT( "Service Pack 6")) == 0)
		{
			HKEY hKey;
			LONG lRet;

			// Test for SP6 versus SP6a.
			lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT( "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009"), 0, KEY_QUERY_VALUE, &hKey);
			if ( lRet == ERROR_SUCCESS)
			{
				wchar_t szTmp[ STRING_BUFF_SIZE];
				swprintf_s( szTmp, L"Service Pack 6a (Build %d)", osvi.dwBuildNumber & 0xFFFF);
				wcscat_s( pszBuff, STRING_BUFF_SIZE, szTmp);
			}
			else // Windows NT 4.0 prior to SP6a
			{
				wchar_t szTmp[ STRING_BUFF_SIZE];
				swprintf_s( szTmp, L"%s (Build %d)", osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
				wcscat_s( pszBuff, STRING_BUFF_SIZE, szTmp);
			}

			RegCloseKey( hKey);
		}
		else // not Windows NT 4.0 
		{
			wchar_t szTmp[ STRING_BUFF_SIZE];
			swprintf_s( szTmp, L"%s (Build %d)", osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
			wcscat_s( pszBuff, STRING_BUFF_SIZE, szTmp);
		}

		break;


		// Test for the Windows Me/98/95.
	case VER_PLATFORM_WIN32_WINDOWS:
		if ( osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Microsoft Windows 95 ");
			if ( osvi.szCSDVersion[ 1] == 'C' || osvi.szCSDVersion[ 1] == 'B')
				wcscat_s( pszBuff, STRING_BUFF_SIZE, L"OSR2");
		} 
		if ( osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Microsoft Windows 98 ");
			if ( osvi.szCSDVersion[ 1] == 'A' || osvi.szCSDVersion[ 1] == 'B')
				wcscat_s( pszBuff, STRING_BUFF_SIZE, L"SE");
		} 
		if ( osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
			wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Microsoft Windows Millennium Edition");
		break;


	case VER_PLATFORM_WIN32s:
		wcscat_s( pszBuff, STRING_BUFF_SIZE, L"Microsoft Win32s");
		break;
	}

	return nMajorID;
}

void XSystemInfoUtil::GetCPUInfo( wstring &strBuff )
{
	LONG lResult;
	HKEY hKey;

	lResult = RegOpenKeyEx (HKEY_LOCAL_MACHINE, L"Hardware\\Description\\System\\CentralProcessor\\0", 0, KEY_QUERY_VALUE , &hKey);
	if (lResult != ERROR_SUCCESS) 
	{
		strBuff = L"CPU Info Error";
		return;
	}

	DWORD dwSize = 0;
	strBuff.resize(256);
	dwSize = strBuff.size();

	lResult = RegQueryValueEx(hKey, L"ProcessorNameString", NULL, NULL, (LPBYTE)((LPCTSTR)strBuff.c_str()), &dwSize);
	if (lResult != ERROR_SUCCESS) 
	{
		strBuff = L"CPU Value Info Error";
		return;
	}
}

void XSystemInfoUtil::GetMemoryInfo( wstring &strBuff )
{
	MEMORYSTATUSEX memex;
	ZeroMemory(&memex, sizeof(MEMORYSTATUS));
	memex.dwLength = sizeof(memex);

	GlobalMemoryStatusEx(&memex);

	wchar_t chBuff[256] = {0,};
	swprintf_s(chBuff, L"%.2f GB", memex.ullTotalPhys / MEMORY_DIVIDER / MEMORY_DIVIDER / MEMORY_DIVIDER);
	strBuff = chBuff;
}

void XSystemInfoUtil::GetVideoInfo( wstring& strBuff )
{
	int i = 0;
	wstring id;
	DISPLAY_DEVICE dd;
	ZeroMemory(&dd, sizeof(DISPLAY_DEVICE));
	dd.cb = sizeof(DISPLAY_DEVICE);

	while(EnumDisplayDevices(NULL, i, &dd, 0))
	{
		if(dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
		{
			id = dd.DeviceID;
			break;
		}

		++i;
	}

	if(id == L"")
	{
		strBuff = L"Vedio Info Error";
		return;
	}

	strBuff = dd.DeviceString;
}

void XSystemInfoUtil::GetVideoMemoryInfo( wstring& strBuff )
{
	DWORD totalMem = RDeviceD3D::GetVideoMemorySize();

	wchar_t chBuff[256] = {0,};
	swprintf_s(chBuff, L"%.2f MB", totalMem / MEMORY_DIVIDER / MEMORY_DIVIDER);
	strBuff = chBuff;
}

void XSystemInfoUtil::GetRuntimeVersion( wstring & strBuff )
{
	strBuff = CAppHelper::MakeTitleCaption(APPLICATION_NAME,
		MLocale::ConvAnsiToUTF16(PRODUCT_VERSION).c_str(),
		MLocale::ConvAnsiToUTF16(SVN_REVISION).c_str(),
		MLocale::ConvAnsiToUTF16(SVN_DATE).c_str());
}