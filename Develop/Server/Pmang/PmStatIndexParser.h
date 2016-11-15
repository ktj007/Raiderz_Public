#pragma once

#include "PmLib.h"
#include "PmDef.h"

class PMANG_API PmStatIndexParser
{
public:
	PmStatIndexParser(wstring strStatIndex);

	bool Parser();

	PmStatIndex GetStatIndex() const;

private:
	wstring m_strStatIndex;
	PmStatIndex m_statIndex;
};
