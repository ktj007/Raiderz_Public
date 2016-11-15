#pragma once

struct GDBCACHEDATA_CHARACTER
{
	GDBCACHEDATA_CHARACTER()
	: nXP(0), nMoney(0)
	{

	}

	GDBCACHEDATA_CHARACTER(int nXP, int nMoney)
	:nXP(nXP), nMoney(nMoney)
	{

	}

	int nXP;
	int nMoney;
};