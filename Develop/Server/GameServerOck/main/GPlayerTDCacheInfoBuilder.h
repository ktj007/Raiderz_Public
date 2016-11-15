#pragma once

#include "CTransData.h"

class GEntityPlayer;

TEST_FORWARD_FT(PlayerTDCacheInfoBuilder, FPlayerTDCacheInfoBuilder, TestPlayerTDCacheInfoBuilder_BuildBaseInfo);

class GPlayerTDCacheInfoBuilder
{
TEST_FRIEND_FT(PlayerTDCacheInfoBuilder, FPlayerTDCacheInfoBuilder, TestPlayerTDCacheInfoBuilder_BuildBaseInfo);

protected:
	void BuildBaseInfo(GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& out);
public:
	GPlayerTDCacheInfoBuilder() {}
	void Build(GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& out);

	void BuildStatusFlag(GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& out);
	void BuildEquipment(GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& out);
	void BuildFeature( GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& out );
};
