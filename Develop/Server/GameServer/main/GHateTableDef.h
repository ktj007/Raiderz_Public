#pragma once

typedef long	HatePoint;
typedef map<MUID, HatePoint>						HatePointMap;
typedef map<HatePoint, MUID, greater<HatePoint>>	HateReverseMap;

enum HATE_TARGET_SWAP_EVENT
{
	HATESWAP_NONE=0,
	HATESWAP_FLIP,
	HATESWAP_RANDOM,
	HATESWAP_NEAR,
	HATESWAP_FAR,
	HATESWAP_SHORT,
};