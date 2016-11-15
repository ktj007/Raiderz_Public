#include "stdafx.h"
#include "GAIProfilingTest.h"
#include "GAIProfiler.h"
#include "GAIStatistics.h"
#include "GConst.h"
#include "GGlobal.h"
#include "GFieldMgr.h"
#include "GField.h"
#include "GMath.h"
#include "GNPCInfoMgr.h"
#include "GNPCInfo.h"
#include "MLocale.h"
#include <fstream>
#include "GTaskID.h"
#include "GTalentInfoMgr.h"
#include "GJobID.h"
#include "GReportTableSet.h"
#include "GAIEvent.h"

template <typename _S, typename _T>
float SafePercentage(_S sub, _T total)
{
	if (total == 0.f)
	{
		return 0.f;
	}

	return sub * 100.f / total;
}

GAIProfilingTest::GAIProfilingTest()
{
    // do nothing
}

GAIProfilingTest::~GAIProfilingTest()
{
    // do nothing
}

const char* GAIProfilingTest::GetName()
{
    return "AIProfilingTest";
}

void GAIProfilingTest::SaveTestResult(MMaietTestResult& outTestResult)
{
    std::wofstream outStream(L"AIProfileResult.txt");

    g_aiStatistics.MakeTotalStatistics();
    const map<int, CountProfile>* pMapProfile = g_aiProfiler.GetProfileMap();
	const WorldStat& rWorldStat = g_aiStatistics.GetStatatistics();

	
	// Test Summary
	{
		GReportTableSet tableSet;

		tableSet.SetTitle(L"Test Summary");
		tableSet.AddColumn(L"Name", 28);
		tableSet.AddColumn(L"Time", 8);
		tableSet.AddColumn(L"Av.Time", 8);
		tableSet.AddColumn(L"MaxTime", 8);
		tableSet.AddColumn(L"%", 8, 1, 8);
		tableSet.AddColumn(L"FieldCount", 12);
		tableSet.AddColumn(L"SectorCount", 12);
		tableSet.AddColumn(L"NPCCount", 8);
		tableSet.AddColumn(L"BattleCount", 12);

		float fTotalTestTime = outTestResult.timeElapsed / 1000.f;
		tableSet.AddRow(L"AiProfileTest");
		tableSet.SetItem(0, 1, fTotalTestTime);
		tableSet.SetItem(0, 2, fTotalTestTime / g_aiProfiler.GetFrameCount());
		tableSet.SetItem(0, 4, 100, true);
		tableSet.SetItem(0, 5, rWorldStat.m_nFieldCount);
		tableSet.SetItem(0, 6, rWorldStat.m_nSectorCount);
		tableSet.SetItem(0, 7, rWorldStat.m_nTotalNPC);
		tableSet.SetItem(0, 8, rWorldStat.m_nTotalBattle);		

		int aSummaryIdx[5] = {
			APT_ID_MODULE_AI, 
			APT_ID_HIT_COLLISION,
			APT_ID_STRAIGHT_MOVE,
			APT_ID_DEFINITE_MOVE,
			APT_ID_PATH_FIND
		};
		
		for(int i = 0; i <= 5; i++)
		{
			map<int, CountProfile>::const_iterator it = pMapProfile->find(aSummaryIdx[i]);

			if (it == pMapProfile->end())
			{
				continue;
			}

			int nRow = tableSet.AddRow(it->second.m_strName.c_str());
			tableSet.SetItem(nRow, 1, it->second.m_fTotalUpdateTime);
			tableSet.SetItem(nRow, 2, it->second.m_fAverageUpdateTime);
			tableSet.SetItem(nRow, 3, it->second.m_fMaxUpdateTime);
			tableSet.SetSubItem(nRow, 4, 0, SafePercentage(it->second.m_fTotalUpdateTime, fTotalTestTime), true);
		}

		tableSet.Sort(1, 0);
		tableSet.Report(outStream);
	}

	
	// LOD
	{
		GReportTableSet tableSet;

		tableSet.SetTitle(L"LOD Update");
		tableSet.AddColumn(L"Name",	28);
		tableSet.AddColumn(L"Time", 8);
		tableSet.AddColumn(L"Av.Time", 8);
		tableSet.AddColumn(L"MaxTime", 8);
		tableSet.AddColumn(L"CallCount", 12);
		tableSet.AddColumn(L"Av.Call", 12);
		tableSet.AddColumn(L"MaxCall", 12);

		for(int i = APT_ID_DUMMY; i <= APT_ID_LOD_LEVEL_5; i++)
		{
			map<int, CountProfile>::const_iterator it = pMapProfile->find(i);
			
			if (it == pMapProfile->end())
			{
				continue;
			}

			int nRow = tableSet.AddRow(it->second.m_strName.c_str());
			tableSet.SetItem(nRow, 1, it->second.m_fTotalUpdateTime);
			tableSet.SetItem(nRow, 2, it->second.m_fAverageUpdateTime);
			tableSet.SetItem(nRow, 3, it->second.m_fMaxUpdateTime);
			tableSet.SetItem(nRow, 4, it->second.m_nTotalUpdateCount);
			tableSet.SetItem(nRow, 5, it->second.m_nAverageUpdateCount);
			tableSet.SetItem(nRow, 6, it->second.m_nMaxUpdateCount);			
		}

		tableSet.Sort(1, 0);
		tableSet.Report(outStream);
	}


	// NPC AI Update
	{
		GReportTableSet tableSet;

		tableSet.SetTitle(L"NPC AI Update");
		tableSet.AddColumn(L"Name", 28);
		tableSet.AddColumn(L"Time", 8);
		tableSet.AddColumn(L"Av.Time", 8);
		tableSet.AddColumn(L"MaxTime", 8);
		tableSet.AddColumn(L"CallCount", 12);
		tableSet.AddColumn(L"ModuleAI", 20, 2, 8);
		tableSet.AddColumn(L"Job", 8, 1, 8);
		tableSet.AddColumn(L"State", 20, 2, 8);
		tableSet.AddColumn(L"Combat", 20, 2, 8);
		tableSet.AddColumn(L"COLT", 8, 1, 8);

		float fTotalModuleTime = 0.f;
		float fTotalJobTime = 0.f;
		float fTotalStateTime = 0.f;
		float fTotalCombatTime = 0.f;
		float fTotalCOLTTime = 0.f;

		for(int i = 0; i <= (APT_ID_JOB_OFFSET + GJOB_MAX); i++)
		{
			map<int, CountProfile>::const_iterator it = pMapProfile->find(i);

			if (it == pMapProfile->end())
			{
				continue;
			}

			int nRow = tableSet.AddRow(it->second.m_strName.c_str());
			tableSet.SetItem(nRow, 1, it->second.m_fTotalUpdateTime);
			tableSet.SetItem(nRow, 2, it->second.m_fAverageUpdateTime);
			tableSet.SetItem(nRow, 3, it->second.m_fMaxUpdateTime);
			tableSet.SetItem(nRow, 4, it->second.m_nTotalUpdateCount);

			if (i == APT_ID_MODULE_AI)
			{
				tableSet.SetItem(nRow, 5, 100, true);
				fTotalModuleTime = it->second.m_fTotalUpdateTime;
			}
			else if (i == APT_ID_TARGET_SELECT || i == APT_ID_FIND_ENEMY)
			{
				tableSet.SetSubItem(nRow, 5, 0, SafePercentage(it->second.m_fTotalUpdateTime, fTotalModuleTime), true);
				tableSet.SetSubItem(nRow, 5, 1, SafePercentage(it->second.m_fTotalUpdateTime, fTotalModuleTime), true);
			}
			else if (i == APT_ID_BEHAVIOR)
			{
				tableSet.SetSubItem(nRow, 5, 1, SafePercentage(it->second.m_fTotalUpdateTime, fTotalModuleTime), true);				
				tableSet.SetItem(nRow, 7, 100, true);
				fTotalStateTime = it->second.m_fTotalUpdateTime;
			}
			else if (i == APT_ID_JOB)
			{
				tableSet.SetSubItem(nRow, 5, 1, SafePercentage(it->second.m_fTotalUpdateTime, fTotalModuleTime), true);
				tableSet.SetItem(nRow, 6, 100, true);
				fTotalJobTime = it->second.m_fTotalUpdateTime;
			}
			else if (APT_ID_BEHAVIOR_DEAD <= i && i <= APT_ID_BEHAVIOR_PEACE)
			{
				tableSet.SetSubItem(nRow, 5, 0, SafePercentage(it->second.m_fTotalUpdateTime, fTotalModuleTime), true);
				tableSet.SetSubItem(nRow, 7, 0, SafePercentage(it->second.m_fTotalUpdateTime, fTotalStateTime), true);
				tableSet.SetSubItem(nRow, 7, 1, SafePercentage(it->second.m_fTotalUpdateTime, fTotalStateTime), true);
			}
			else if (i == APT_ID_BEHAVIOR_COMBAT)
			{
				tableSet.SetSubItem(nRow, 7, 1, SafePercentage(it->second.m_fTotalUpdateTime, fTotalStateTime), true);
				tableSet.SetItem(nRow, 8, 100, true);
				fTotalCombatTime = it->second.m_fTotalUpdateTime;
			}
			else if (APT_ID_COMBAT_FLEE <= i && i <= APT_ID_COMBAT_WEAKEN)
			{
				tableSet.SetSubItem(nRow, 5, 0, SafePercentage(it->second.m_fTotalUpdateTime, fTotalModuleTime), true);
				tableSet.SetSubItem(nRow, 7, 0, SafePercentage(it->second.m_fTotalUpdateTime, fTotalStateTime), true);
				tableSet.SetSubItem(nRow, 8, 0, SafePercentage(it->second.m_fTotalUpdateTime, fTotalCombatTime), true);
				tableSet.SetSubItem(nRow, 8, 1, SafePercentage(it->second.m_fTotalUpdateTime, fTotalCombatTime), true);
			}
			else if (i == APT_ID_COMBAT_COMBAT)
			{
				tableSet.SetSubItem(nRow, 8, 1, SafePercentage(it->second.m_fTotalUpdateTime, fTotalCombatTime), true);
				tableSet.SetItem(nRow, 9, 100, true);
				fTotalCOLTTime = it->second.m_fTotalUpdateTime;
			}
			else if (APT_ID_COLT_COMBAT <= i && i <= APT_ID_COLT_STRESS)
			{
				tableSet.SetSubItem(nRow, 5, 0, SafePercentage(it->second.m_fTotalUpdateTime, fTotalModuleTime), true);
				tableSet.SetSubItem(nRow, 7, 0, SafePercentage(it->second.m_fTotalUpdateTime, fTotalStateTime), true);
				tableSet.SetSubItem(nRow, 8, 0, SafePercentage(it->second.m_fTotalUpdateTime, fTotalCombatTime), true);
				tableSet.SetSubItem(nRow, 9, 0, SafePercentage(it->second.m_fTotalUpdateTime, fTotalCOLTTime), true);
			}
			else if (APT_ID_JOB_OFFSET <= i)
			{
				tableSet.SetSubItem(nRow, 5, 0, SafePercentage(it->second.m_fTotalUpdateTime, fTotalModuleTime), true);
				tableSet.SetSubItem(nRow, 6, 0, SafePercentage(it->second.m_fTotalUpdateTime, fTotalJobTime), true);
			}
		}

		tableSet.Sort(1, 0);
		tableSet.Report(outStream);
	}


	// Etc.
	{
		GReportTableSet tableSet;

		tableSet.SetTitle(L"Etc.h");
		tableSet.AddColumn(L"Name", 28);
		tableSet.AddColumn(L"Time", 8);
		tableSet.AddColumn(L"Av.Time", 8);
		tableSet.AddColumn(L"MaxTime", 8);		
		tableSet.AddColumn(L"CallCount", 12);

		for(int i = APT_ID_HIT_COLLISION; i <= APT_ID_PATH_FIND + 1; i++)
		{
			map<int, CountProfile>::const_iterator it = pMapProfile->find(i);

			if (it == pMapProfile->end())
			{
				continue;
			}

			int nRow = tableSet.AddRow(it->second.m_strName.c_str());
			tableSet.SetItem(nRow, 1, it->second.m_fTotalUpdateTime);
			tableSet.SetItem(nRow, 2, it->second.m_fAverageUpdateTime);
			tableSet.SetItem(nRow, 3, it->second.m_fMaxUpdateTime);
			tableSet.SetItem(nRow, 4, it->second.m_nTotalUpdateCount);
		}

		tableSet.Sort(1, 0);
		tableSet.Report(outStream);
	}


	// Task in Jobs
	{
		GReportTableSet tableSet;

		tableSet.SetTitle(L"Task in Jobs");
		tableSet.AddColumn(L"Name", 28);
		tableSet.AddColumn(L"Time", 8);
		tableSet.AddColumn(L"Av.Time", 8);
		tableSet.AddColumn(L"MaxTime", 8);
		tableSet.AddColumn(L"CallCount", 12);
		tableSet.AddColumn(L"Task", 8);

		tableSet.AddRow(L"Total");
		tableSet.SetItem(0, 5, 100, true);

		tableSet.AddRow(L"");
		tableSet.SetFillItem(1, 5, L'=');

		float fTotalTime = 0.f;
		int nTotalCallCount = 0;

		for(int i = 0; i < GTASK_TEST; i++)
		{
			map<int, CountProfile>::const_iterator it = pMapProfile->find(APT_ID_TASK_OFFSET + i);
			
			if (it == pMapProfile->end())
			{
				continue;
			}

			fTotalTime += it->second.m_fTotalUpdateTime;
			nTotalCallCount += it->second.m_nTotalUpdateCount;
		}

		tableSet.SetItem(0, 1, fTotalTime);
		tableSet.SetItem(0, 2, fTotalTime / g_aiProfiler.GetFrameCount());
		tableSet.SetItem(0, 4, nTotalCallCount);


		for(int i = 0; i < GTASK_TEST; i++)
		{
			map<int, CountProfile>::const_iterator it = pMapProfile->find(APT_ID_TASK_OFFSET + i);
			
			if (it == pMapProfile->end())
			{
				continue;
			}

			int nRow = tableSet.AddRow(it->second.m_strName.c_str());
			tableSet.SetItem(nRow, 1, it->second.m_fTotalUpdateTime);
			tableSet.SetItem(nRow, 2, it->second.m_fAverageUpdateTime);
			tableSet.SetItem(nRow, 3, it->second.m_fMaxUpdateTime);
			tableSet.SetItem(nRow, 4, it->second.m_nTotalUpdateCount);
			tableSet.SetItem(nRow, 5, SafePercentage(it->second.m_fTotalUpdateTime, fTotalTime), true);
		}

		tableSet.Sort(1, 2);
		tableSet.Report(outStream);
	}


	// Action in COLTs
	{
		GReportTableSet tableSet;

		tableSet.SetTitle(L"Action in COLTs");
		tableSet.AddColumn(L"Name", 28);
		tableSet.AddColumn(L"Time", 8);
		tableSet.AddColumn(L"Av.Time", 8);
		tableSet.AddColumn(L"MaxTime", 8);
		tableSet.AddColumn(L"CallCount", 12);
		tableSet.AddColumn(L"Act", 8);

		tableSet.AddRow(L"Total");
		tableSet.SetItem(0, 5, 100, true);

		tableSet.AddRow(L"");
		tableSet.SetFillItem(1, 5, L'=');

		float fTotalTime = 0.f;
		int nTotalCallCount = 0;

		for(int i = 0; i < CAT_SIZE; i++)
		{
			map<int, CountProfile>::const_iterator it = pMapProfile->find(APT_ID_ACTION_OFFSET + i);

			if (it == pMapProfile->end())
			{
				continue;
			}

			fTotalTime += it->second.m_fTotalUpdateTime;
			nTotalCallCount += it->second.m_nTotalUpdateCount;
		}

		tableSet.SetItem(0, 1, fTotalTime);
		tableSet.SetItem(0, 2, fTotalTime / g_aiProfiler.GetFrameCount());
		tableSet.SetItem(0, 4, nTotalCallCount);


		for(int i = 0; i < CAT_SIZE; i++)
		{
			map<int, CountProfile>::const_iterator it = pMapProfile->find(APT_ID_ACTION_OFFSET + i);

			if (it == pMapProfile->end())
			{
				continue;
			}

			int nRow = tableSet.AddRow(it->second.m_strName.c_str());
			tableSet.SetItem(nRow, 1, it->second.m_fTotalUpdateTime);
			tableSet.SetItem(nRow, 2, it->second.m_fAverageUpdateTime);
			tableSet.SetItem(nRow, 3, it->second.m_fMaxUpdateTime);
			tableSet.SetItem(nRow, 4, it->second.m_nTotalUpdateCount);
			tableSet.SetItem(nRow, 5, SafePercentage(it->second.m_fTotalUpdateTime, fTotalTime), true);
		}

		tableSet.Sort(1, 2);
		tableSet.Report(outStream);
	}


	// Talent / Phase
	{
		GReportTableSet tableSet;

		tableSet.SetTitle(L"Talent / Phase");
		tableSet.AddColumn(L"Name", 28);
		tableSet.AddColumn(L"Time", 8);
		tableSet.AddColumn(L"Av.Time", 8);
		tableSet.AddColumn(L"MaxTime", 8);
		tableSet.AddColumn(L"CallCount", 12);
		tableSet.AddColumn(L"Talent", 20, 2, 8);

		tableSet.AddRow(L"Total");
		tableSet.SetItem(0, 5, 100, true);

		// Total 구하기
		float fTotalTime = 0.0f;
		int nTotalCallCount = 0;

		for each(pair<int , CSTalentInfo*> pairTalent in *gmgr.pTalentInfoMgr)
		{			
			// Total 구할때, 하위 ACT 시간은 제외한다.
			int nKey = pairTalent.second->m_nID * 10;
			map<int, CountProfile>::const_iterator it = pMapProfile->find(APT_ID_TALENT_OFFSET + nKey);

			if (it == pMapProfile->end())
			{
				continue;
			}
			
			fTotalTime += it->second.m_fTotalUpdateTime;
			nTotalCallCount += it->second.m_nTotalUpdateCount;
		}

		tableSet.SetItem(0, 1, fTotalTime);
		tableSet.SetItem(0, 2, fTotalTime / g_aiProfiler.GetFrameCount());
		tableSet.SetItem(0, 4, nTotalCallCount);


		for each(pair<int , CSTalentInfo*> pairTalent in *gmgr.pTalentInfoMgr)
		{
			int nKey = pairTalent.second->m_nID * 10;

			for(int i = 0; i < TALENT_PHASE_MAX; i++)
			{
				map<int, CountProfile>::const_iterator it = pMapProfile->find(APT_ID_TALENT_OFFSET + nKey + i);

				if (it == pMapProfile->end())
				{
					continue;
				}

				int nRow = tableSet.AddRow(it->second.m_strName.c_str());
				tableSet.SetItem(nRow, 1, it->second.m_fTotalUpdateTime);
				tableSet.SetItem(nRow, 2, it->second.m_fAverageUpdateTime);
				tableSet.SetItem(nRow, 3, it->second.m_fMaxUpdateTime);
				tableSet.SetItem(nRow, 4, it->second.m_nTotalUpdateCount);

				if (i == 0)
				{
					tableSet.SetSubItem(nRow, 5, 1, SafePercentage(it->second.m_fTotalUpdateTime, fTotalTime), true);
				}
				else
				{					
					tableSet.SetSubItem(nRow, 5, 0, SafePercentage(it->second.m_nTotalUpdateCount, nTotalCallCount), true);
				}
			}
		}

		tableSet.Sort(1, 1);
		tableSet.Report(outStream);
	}


    // World Statistics - MaxNPC
    {
		GReportTableSet tableSet;

		tableSet.SetTitle(L"World Statistics - MaxNPC");
		tableSet.AddColumn(L"Name", 28);
		tableSet.AddColumn(L"MaxNPC", 12);
		tableSet.AddColumn(L"%", 8);

		tableSet.AddRow(L"Total");
		tableSet.SetItem(0, 1, rWorldStat.m_nTotalNPC);
		tableSet.SetItem(0, 2, 100, true);

		tableSet.AddRow(L"Average");
		tableSet.SetItem(1, 1, rWorldStat.m_nAverageNPC);
		tableSet.SetItem(1, 2, SafePercentage(rWorldStat.m_nAverageNPC, rWorldStat.m_nTotalNPC), true);

		tableSet.AddRow(L"MAX");
		tableSet.SetItem(2, 1, rWorldStat.m_nMaxNPC);
		tableSet.SetItem(2, 2, SafePercentage(rWorldStat.m_nMaxNPC, rWorldStat.m_nTotalNPC), true);

		tableSet.AddRow(L"");
		tableSet.SetFillItem(3, 0, L'-');
		tableSet.SetFillItem(3, 1, L'-');
		tableSet.SetFillItem(3, 2, L'-');

		for each(const pair<int, FieldStat>& pairFieldStat in rWorldStat.m_mapFieldStats)
		{
			const FieldStat& rFieldStat = pairFieldStat.second;

			int nRow = tableSet.AddRow(rFieldStat.m_strFieldName.c_str());
			tableSet.SetItem(nRow, 1, rFieldStat.m_nTotalNPC);
			tableSet.SetItem(nRow, 2, SafePercentage(rFieldStat.m_nTotalNPC, rWorldStat.m_nTotalNPC), true);
		}

		tableSet.Sort(1, 4);
		tableSet.Report(outStream);
	}


	// World Statistics - BattleCount
	{
		GReportTableSet tableSet;

		tableSet.SetTitle(L"World Statistics - BattleCount");
		tableSet.AddColumn(L"Name", 28);
		tableSet.AddColumn(L"BattleCount", 12);
		tableSet.AddColumn(L"%", 8);

		tableSet.AddRow(L"Total");
		tableSet.SetItem(0, 1, rWorldStat.m_nTotalBattle);
		tableSet.SetItem(0, 2, 100, true);

		tableSet.AddRow(L"Average");
		tableSet.SetItem(1, 1, rWorldStat.m_nAverageBattle);
		tableSet.SetItem(1, 2, SafePercentage(rWorldStat.m_nAverageBattle, rWorldStat.m_nTotalBattle), true);

		tableSet.AddRow(L"MAX");
		tableSet.SetItem(2, 1, rWorldStat.m_nMaxBattle);
		tableSet.SetItem(2, 2, SafePercentage(rWorldStat.m_nMaxBattle, rWorldStat.m_nTotalBattle), true);

		tableSet.AddRow(L"");
		tableSet.SetFillItem(3, 0, L'-');
		tableSet.SetFillItem(3, 1, L'-');
		tableSet.SetFillItem(3, 2, L'-');

		for each(const pair<int, FieldStat>& pairFieldStat in rWorldStat.m_mapFieldStats)
		{
			const FieldStat& rFieldStat = pairFieldStat.second;

			int nRow = tableSet.AddRow(rFieldStat.m_strFieldName.c_str());
			tableSet.SetItem(nRow, 1, rFieldStat.m_nTotalBattle);
			tableSet.SetItem(nRow, 2, SafePercentage(rFieldStat.m_nTotalBattle, rWorldStat.m_nTotalBattle), true);
		}

		tableSet.Sort(1, 4);
		tableSet.Report(outStream);
	}

	// World Statistics - SectorCount
	{
		GReportTableSet tableSet;

		tableSet.SetTitle(L"World Statistics - SectorCount");
		tableSet.AddColumn(L"Name", 28);
		tableSet.AddColumn(L"SectorCount", 12);
		tableSet.AddColumn(L"%", 8);

		tableSet.AddRow(L"Total");
		tableSet.SetItem(0, 1, rWorldStat.m_nSectorCount);
		tableSet.SetItem(0, 2, 100, true);
        
		tableSet.AddRow(L"Average");
		tableSet.SetItem(1, 1, rWorldStat.m_nAverageSectorCount);
		tableSet.SetItem(1, 2, SafePercentage(rWorldStat.m_nAverageSectorCount, rWorldStat.m_nSectorCount), true);

		tableSet.AddRow(L"MAX");
		tableSet.SetItem(2, 1, rWorldStat.m_nMaxSectorCount);
		tableSet.SetItem(2, 2, SafePercentage(rWorldStat.m_nMaxSectorCount, rWorldStat.m_nSectorCount), true);

		tableSet.AddRow(L"");
		tableSet.SetFillItem(3, 0, L'-');
		tableSet.SetFillItem(3, 1, L'-');
		tableSet.SetFillItem(3, 2, L'-');

		for each(const pair<int, FieldStat>& pairFieldStat in rWorldStat.m_mapFieldStats)
		{
			const FieldStat& rFieldStat = pairFieldStat.second;

			int nRow = tableSet.AddRow(rFieldStat.m_strFieldName.c_str());
			tableSet.SetItem(nRow, 1, rFieldStat.m_nSectorCount);
			tableSet.SetItem(nRow, 2, SafePercentage(rFieldStat.m_nSectorCount, rWorldStat.m_nSectorCount), true);
		}

		tableSet.Sort(1, 4);
		tableSet.Report(outStream);
    }
    

    // Field Statistics
    {
		GReportTableSet tableSet;

		tableSet.SetTitle(L"Field Statistics");
		tableSet.AddColumn(L"Name", 28);
		tableSet.AddColumn(L"  MaxNPC        Avg.         MAX", 32, 3, 8);
		tableSet.AddColumn(L"  Battle        Avg.         MAX", 32, 3, 8);
		tableSet.AddColumn(L"SectorCount", 12);

        for each(const pair<int, FieldStat>& pairFieldStat in rWorldStat.m_mapFieldStats)
        {
            const FieldStat& rFieldStat = pairFieldStat.second;

			int nRow = tableSet.AddRow(rFieldStat.m_strFieldName.c_str());
			tableSet.SetSubItem(nRow, 1, 0, rFieldStat.m_nTotalNPC);
			tableSet.SetSubItem(nRow, 1, 1, rFieldStat.m_nAverageNPC);
			tableSet.SetSubItem(nRow, 1, 2, rFieldStat.m_nMaxNPC);
			tableSet.SetSubItem(nRow, 2, 0, rFieldStat.m_nTotalBattle);
			tableSet.SetSubItem(nRow, 2, 1, rFieldStat.m_nAverageBattle);
			tableSet.SetSubItem(nRow, 2, 2, rFieldStat.m_nMaxBattle);
			tableSet.SetItem(nRow, 3, rFieldStat.m_nSectorCount);
        }

		tableSet.Report(outStream);
    }
}

void GAIProfilingTest::OnCreate()
{	
    g_aiProfiler.SetEnable(true);
}

void GAIProfilingTest::OnDestroy()
{
	g_aiProfiler.SetEnable(false);
}

void GAIProfilingTest::OnUpdate(float fDelta)
{    
}
