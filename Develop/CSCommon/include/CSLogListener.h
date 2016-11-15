#pragma once

class CSLogListener
{
public:
	virtual void OnLog(const char* szLog) = 0;
};
