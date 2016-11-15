#ifndef _XSYSTME_INFO_UTIL_H
#define _XSYSTME_INFO_UTIL_H

class XSystemInfoUtil
{
private:
	void			GetRuntimeVersion(wstring & strBuff);

protected:
	int				GetOSVersion(wchar_t* pszBuff);
	void			GetCPUInfo(wstring &strBuff);
	void			GetMemoryInfo(wstring &strBuff);
	void			GetVideoInfo(wstring& strBuff);
	void			GetVideoMemoryInfo(wstring& strBuff);

public:
	XSystemInfoUtil() {}
	virtual ~XSystemInfoUtil() {}

	void			SetLogSystemInfo();
};


#endif // #ifndef _XSYSTME_INFO_UTIL_H