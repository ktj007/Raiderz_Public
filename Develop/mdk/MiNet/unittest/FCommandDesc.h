#pragma once

#include "MCommandDesc.h"
#include "MCommand.h"

using namespace minet;

class FCommandDesc
{
public:
	FCommandDesc()
	{

	}
	virtual ~FCommandDesc()
	{

	}
	void AddLoginCommandDesc()
	{
		BEGIN_CMD_DESC();

		C(MC_COMM_REQUEST_LOGIN,			MCDT_MACHINE2MACHINE , MCF_C2S, "Request Login")
			P(MPT_STR, "UserID");
		P(MPT_STR, "Password");
		P(MPT_INT, "CommandVersion");

		END_CMD_DESC();
	}

	void AddTestCommandDesc_Int()
	{
		BEGIN_CMD_DESC();

		C(MC_COMM_TEST_INT,			MCDT_MACHINE2MACHINE , MCF_C2S, "Test Command")
		P(MPT_INT, "Value1");

		END_CMD_DESC();
	}

	MCommand* NewCmd_TestInt1(int nValue)
	{
		MCommand* pCommand = new MCommand(MC_COMM_TEST_INT, MUID::ZERO, MUID::ZERO);
		pCommand->AddParameter(new MCommandParameterInt(nValue));
		return pCommand;
	}
	static const int MC_COMM_REQUEST_LOGIN = 2001;
	static const int MC_COMM_TEST_INT = 5001;
};