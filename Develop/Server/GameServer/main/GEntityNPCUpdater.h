#pragma once

#include "GNPCAILod.h"

class GEntityNPC;

class GEntityNPCUpdater
{
protected:
	void Update_ScriptTimer( GEntityNPC* pOwner, float fDelta );
	void Update_NPCDecay( GEntityNPC* pOwner, float fDelta );
	void Update_HateTable( GEntityNPC* pOwner, float fDelta, AI_LOD_LEVEL nLODLevel );
	void ResetTickVars( GEntityNPC* pOwner );

	//
	void Update_Invisibility( GEntityNPC* pOwner, float fDelta, AI_LOD_LEVEL nLODLevel );
	void Update_CooltimeChecker( GEntityNPC* pOwner, float fDelta, AI_LOD_LEVEL nLODLevel );
	void Update_NPCAssist( GEntityNPC* pOwner, float fDelta, AI_LOD_LEVEL nLODLevel );
	void Update_NPCSensorChecker( GEntityNPC* pOwner, float fDelta );
	void Update_NPCStress( GEntityNPC* pOwner, float fDelta );
	void Update_NPCFirstStrike( GEntityNPC* pOwner, float fDelta, AI_LOD_LEVEL nLODLevel );
	void Update_NPCBParts( GEntityNPC* pOwner, float fDelta );
	void Update_NPCMarkPos( GEntityNPC* pOwner, float fDelta );
public:
	void UpdatePre(GEntityNPC* pOwner, float fDelta);

	AI_LOD_LEVEL GetAILodLevel( GEntityNPC* pOwner );
	void UpdatePost(GEntityNPC* pOwner, float fDelta);

};