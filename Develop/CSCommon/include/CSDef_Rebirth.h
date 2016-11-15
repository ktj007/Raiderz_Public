#pragma once

enum REBIRTH_TYPE
{
	REBT_NONE,

	REBT_PLACE_IN_BATTLEGROUD,	// 투기장에서 제자리 부활
	REBT_SOULBINDING,			// 영혼석 위치로 부활
	REBT_NEAR_SOULBINDING,		// 가장 가까운 영혼석 위치로 부활
	REBT_CHECKPOINT,			// 안전지점 위치로 부활
	REBT_QPVP,					// QPVP부활지점으로 부활	

	REBIRTH_MAX
};