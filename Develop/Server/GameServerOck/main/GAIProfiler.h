#pragma once

#define APT_ID_TARGET_SELECT		1
#define APT_ID_FIND_ENEMY			2
#define APT_ID_BEHAVIOR				3
#define APT_ID_BEHAVIOR_DEAD		4
#define APT_ID_BEHAVIOR_MISSION		5
#define APT_ID_BEHAVIOR_PEACE		6
#define APT_ID_BEHAVIOR_COMBAT		7
#define APT_ID_COMBAT_FLEE			8
#define APT_ID_COMBAT_VICTORY		9
#define APT_ID_COMBAT_WEAKEN		10
#define APT_ID_COMBAT_COMBAT		11
#define APT_ID_COLT_COMBAT			12
#define APT_ID_COLT_REACTION		13
#define APT_ID_COLT_AGGRO			14
#define APT_ID_COLT_STRESS			15
#define APT_ID_COLT_IDLE			16
#define APT_ID_JOB					17
#define APT_ID_JOB_OFFSET			18

#define APT_ID_TASK_OFFSET			101

#define APT_ID_HIT_COLLISION		200
#define APT_ID_STRAIGHT_MOVE		201
#define APT_ID_DEFINITE_MOVE		202
#define APT_ID_PATH_FIND			203

#define APT_ID_ACTION_OFFSET		301

#define APT_ID_DUMMY				400

#define APT_ID_NPC_AI				500
#define APT_ID_LOD_DEFAULT			501
#define APT_ID_LOD_EMPTY_FIELD		502
#define APT_ID_LOD_LEVEL_1			503
#define APT_ID_LOD_LEVEL_2			504
#define APT_ID_LOD_LEVEL_3			505
#define APT_ID_LOD_LEVEL_4			506
#define APT_ID_LOD_LEVEL_5			507

#define APT_ID_MODULE_AI			600
#define APT_ID_MODULE_BUFF			601
#define APT_ID_MODULE_COMBAT		602

#define APT_ID_TALENT_OFFSET		1000

#ifndef _PUBLISH
	#define AI_PFI_B(idx,name)				g_aiProfiler.Begin(idx,name)
	#define AI_PFI_E(idx)					g_aiProfiler.End(idx)
	#define AI_PFI_BLOCK(idx,name)			GAIProfilerScopeObj aiProfilerScopObj(idx,name)
	#define AI_PFI_JOB_BLOCK(idx,name)		GAIProfilerScopeObj aiProfilerScopObj(APT_ID_JOB_OFFSET + idx,name)
	#define AI_PFI_TASK_BLOCK(idx,name)		GAIProfilerScopeObj aiProfilerScopObj(APT_ID_TASK_OFFSET + idx,name)
	#define AI_PFI_TALENT_BLOCK(idx,name)	GAIProfilerScopeObj aiProfilerScopObj(APT_ID_TALENT_OFFSET + idx,name)
	#define AI_PFI_ACTION_BLOCK(idx,name)	GAIProfilerScopeObj aiProfilerScopObj(APT_ID_ACTION_OFFSET + idx,name)
	#define AI_PFI_FRAME_UPDATE()			g_aiProfiler.Update()			// 매프레임 끝에 한번 호출
#else
	#define AI_PFI_B(idx,name)
	#define AI_PFI_E(idx)
	#define AI_PFI_BLOCK(idx,name)
	#define AI_PFI_JOB_BLOCK(idx,name)
	#define AI_PFI_TASK_BLOCK(idx,name)
	#define AI_PFI_TALENT_BLOCK(idx,name)
	#define AI_PFI_ACTION_BLOCK(idx,name)
	#define AI_PFI_FRAME_UPDATE()
#endif

struct CountProfile
{
	wstring m_strName;
	int m_nTotalUpdateCount;
	int m_nCurrentUpdateCount;
	int m_nMaxUpdateCount;
	int m_nAverageUpdateCount;
	float m_fTotalUpdateTime;
	float m_fBeginUpdateTime;
	float m_fCurrentUpdateTime;
	float m_fMaxUpdateTime;
	float m_fAverageUpdateTime;
	int64 m_llTick;

	CountProfile()
		: m_nTotalUpdateCount(0)
		, m_nCurrentUpdateCount(0)
		, m_nMaxUpdateCount(0)
		, m_nAverageUpdateCount(0)
		, m_fTotalUpdateTime(0)
		, m_fBeginUpdateTime(0)
		, m_fCurrentUpdateTime(0)
		, m_fMaxUpdateTime(0)
		, m_fAverageUpdateTime(0)
		, m_llTick(0)
	{}
};

class GAIProfiler
{
public:
	struct ProfileUpdater
	{
		int m_nFrameCount;
		ProfileUpdater(int nFrameCount);
		template <typename T> void operator() (T& profile) const;
	};

public:
	GAIProfiler();
	virtual ~GAIProfiler();

	void Update(void);
	void Begin(int nAiProfilingId, wstring strName);
	void End(int nAiProfilingId);

	void SetEnable(bool bEnable);
	int GetFrameCount(void);
	const map<int, CountProfile>* GetProfileMap(void);	

private:		
	bool m_isEnable;
	map<int, CountProfile> m_mapCountProfile;
	CountProfile m_aCountProfile[APT_ID_TALENT_OFFSET];
	int m_nFrameCount;
};

extern GAIProfiler g_aiProfiler;


class GAIProfilerScopeObj
{	
public:
	GAIProfilerScopeObj(int nIndex, wstring strName) : m_nIndex(nIndex)
	{
		g_aiProfiler.Begin(m_nIndex, strName);
	}
	~GAIProfilerScopeObj()
	{
		g_aiProfiler.End(m_nIndex);
	}	

private:	
	int m_nIndex;
};
