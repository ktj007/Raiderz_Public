#pragma once

#include "GAIEventHandler.h"

enum AI_LOD_LEVEL 
{
	AI_LOD_INVALID_LEVEL = -1,
	AI_LOD_LEVEL_1 = 0,				///< Combat				(전투중)
	AI_LOD_LEVEL_2,					///< Neighbor Sector	(주변 섹터에 플레이어 있음)
	AI_LOD_LEVEL_3,					///< Major Agent		(LOD에 상관없이 항상 AI를 돌려야 하는 상태 - 현재 사용안함)
	AI_LOD_LEVEL_4,					///< Field				(필드에 플레이어가 있음)
	AI_LOD_LEVEL_5,					///< out of sight		(필드에 플레이어가 없음)

	MAX_AI_LOD_LEVEL
};


class GNPCAILod: public GAIEventHandler, public MTestMemPool<GNPCAILod>
{
public:
	GNPCAILod();
	virtual ~GNPCAILod();

	virtual void OnNotify(GAIEvent aiEvent);
	AI_LOD_LEVEL GetLodLevel(void) const;
	wstring GetName(AI_LOD_LEVEL aiLod) const;

private:	
	AI_LOD_LEVEL m_aiLod;	
	AI_LOD_LEVEL m_aiOldLod;
};
