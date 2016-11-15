#pragma once

class SServerApplication;
enum SServerAppType;

class SServiceAdapter
{
public:
	SServiceAdapter(SServerApplication* pApplication, SServerAppType m_applicationType, wstring strServerName);
	virtual ~SServiceAdapter();

	bool Run();

protected:
	virtual bool StartServiceThread();

	bool Install();
	bool Uninstall();
	bool Start();
	
	static bool BeginApplicationService();
	static unsigned int WINAPI RunApplication(void* pArg);
	static void WINAPI RunService(DWORD dwArgc, LPTSTR *lpszArgv);
	static void WINAPI ServiceHandler(DWORD dwControl);
};
