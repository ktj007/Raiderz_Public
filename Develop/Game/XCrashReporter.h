#pragma once

#include "MCrashReporter.h"

class XCrashReporter
{
public:
	static void LogInfo();
	static bool bAutoReport;

	static void Report(const std::string &strLocalDumpName, const std::string &strAppName);
};