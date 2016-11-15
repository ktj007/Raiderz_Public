#pragma once

#include "CTransData.h"

class GEntityPlayer;

TEST_FORWARD_FT(PlayerTDCacheInfoBuilder, FPlayerTDCacheInfoBuilder, TestPlayerTDCacheInfoBuilder_BuildBaseInfo);

class GPlayerTDCacheInfoBuilder
{
TEST_FRIEND_FT(PlayerTDCacheInfoBuilder, FPlayerTDCacheInfoBuilder, TestPlayerTDCacheInfoBuilder_BuildBaseInfo);

protected:
	void BuildBaseInfo(GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& cache);
public:
	GPlayerTDCacheInfoBuilder() {}
	void Build(GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& cache, TD_PLAYER_FEATURE_TATTOO& TattooCache);

	void BuildStatusFlag(GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& cache);
	void BuildEquipment(GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& cache);
	void BuildFeature(GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& cache, TD_PLAYER_FEATURE_TATTOO& TattooCache);
};
