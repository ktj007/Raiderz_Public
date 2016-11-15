#pragma once

#include "CSChrInfo.h"

/// NPCInfo 관련 헬퍼
class CSNPCInfoHelper
{
public:
	/// 중력에 영향을 받는 NPC인지 여부
	static bool IsUsingGravityNPC(NPC_TYPE nNPCType);
};