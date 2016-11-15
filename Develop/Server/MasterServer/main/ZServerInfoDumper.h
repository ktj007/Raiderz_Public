#pragma once

#include "MDebug.h"

class ZServerInfoDumper
{
public:
	ZServerInfoDumper();
	~ZServerInfoDumper();

	void Dump(wstring strPlayerName);

private:
	MLogger m_Logger;

	void DumpFields();
	void DumpPlayers();

	void Log(const wchar_t* pFormat, ...);
	std::wstring MakeFileName(std::wstring strPlayerName);
};
