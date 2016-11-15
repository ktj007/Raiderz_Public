#pragma once

class GCrashReporter
{
public:
	static void CrashDumpHandler(const char* szFileName);

private:
	static void CrashLog();
};
