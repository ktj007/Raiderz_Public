#include "stdafx.h"
#include "GDamageMODTable.h"

GPlayerDMODTable::GPlayerDMODTable()
{
	GLevelFactorTable level_factor_table;
	for (int level_diff = LEVELFACTOR_DIFF_MIN; level_diff <= LEVELFACTOR_DIFF_MAX; level_diff++)
	{
		int index = GetIndex(level_diff);
		int lf = level_factor_table.GetFactor(level_diff);

		if (lf >= 5)
		{
			m_fValues[index] = float(2 - 0.2 * lf);
		}
		else
		{
			m_fValues[index] = float(1.2 - 0.05 * lf);
		}
	}
}

float GPlayerDMODTable::GetValue( int nPCLevel, int nNPCLevel )
{
	int nLevelDiff = nPCLevel - nNPCLevel;
	return GetValue(nLevelDiff);
}

float GPlayerDMODTable::GetValue( int nLevelDiff )
{
	int index = GetIndex(nLevelDiff);
	return m_fValues[index];
}

TEST(TestPlayerDMODTable)
{
	// 공격 공식이 바뀌어 DMOD는 폐기함
	//GPlayerDMODTable dmod_table;

	//CHECK_CLOSE(dmod_table.GetValue(-10), 0.0f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(-9), 0.0f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(-8), 0.0f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(-7), 0.0f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(-6), 0.2f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(-5), 0.4f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(-4), 0.4f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(-3), 0.6f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(-2), 0.8f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(-1), 1.0f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(0), 1.0f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(1), 1.0f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(2), 1.0f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(3), 1.0f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(4), 1.05f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(5), 1.05f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(6), 1.1f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(7), 1.1f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(8), 1.15f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(9), 1.15f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(10), 1.2f, 0.0001f);

	//CHECK_CLOSE(dmod_table.GetValue(-100), 0.0f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(100), 1.2f, 0.0001f);

	//CHECK_CLOSE(dmod_table.GetValue(1, 11), 0.0f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(10, 10), 1.0f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(10, 5), 1.05f, 0.0001f);
}

TEST(TestNPC_DMODTable)
{
	// 공격 공식이 바뀌어 DMOD는 폐기함
	//GNPC_DMODTable dmod_table;

	//CHECK_CLOSE(dmod_table.GetValue(-10), 1.5f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(-9), 1.5f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(-8), 1.5f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(-7), 1.5f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(-6), 1.4f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(-5), 1.3f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(-4), 1.3f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(-3), 1.2f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(-2), 1.1f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(-1), 1.0f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(0), 1.0f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(1), 1.0f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(2), 0.95f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(3), 0.95f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(4), 0.9f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(5), 0.9f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(6), 0.85f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(7), 0.85f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(8), 0.8f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(9), 0.8f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(10), 0.75f, 0.0001f);

	//CHECK_CLOSE(dmod_table.GetValue(-100), 1.5f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(100), 0.75f, 0.0001f);

	//CHECK_CLOSE(dmod_table.GetValue(1, 11), 1.5f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(10, 10), 1.0f, 0.0001f);
	//CHECK_CLOSE(dmod_table.GetValue(10, 5), 0.9f, 0.0001f);
}

GNPC_DMODTable::GNPC_DMODTable()
{
	GLevelFactorTable level_factor_table;
	for (int level_diff = LEVELFACTOR_DIFF_MIN; level_diff <= LEVELFACTOR_DIFF_MAX; level_diff++)
	{
		int index = GetIndex(level_diff);
		int lf = level_factor_table.GetFactor(level_diff);

		if (lf >= 5)
		{
			m_fValues[index] = float(1 + 0.1 * (lf - 5));
		}
		else
		{
			m_fValues[index] = float(1 + 0.05 * (lf - 5));
		}
	}
}