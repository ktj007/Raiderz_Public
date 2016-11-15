#pragma once

class XBaseUnitTestHelper
{
public:
	XBaseUnitTestHelper() {}
	virtual ~XBaseUnitTestHelper() {}

	static TD_UPDATE_CACHE_NPC Make_TD_UPDATE_CACHE_NPC(MUID uidNPC, int nNPCID, vec3 vPos=vec3::ZERO, vec3 vDir=vec3::AXISY, uint32 nStatusFlag=0);
	static TD_UPDATE_CACHE_PLAYER MakeSamle_TD_UPDATE_CACHE_PLAYER(MUID uidPlayer);
};