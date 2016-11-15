#pragma once

#include "MCommand.h"

class FCmdParam
{
public:
	FCmdParam() : m_pCommand(NULL)
	{

	}
	~FCmdParam()
	{
		SAFE_DELETE(m_pCommand);
	}

	minet::MCommand* m_pCommand;
};

