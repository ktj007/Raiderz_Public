#ifndef _CTRANS_DATA_LOG_H
#define _CTRANS_DATA_LOG_H

#include "MTypes.h"
#include "CSDef.h"

// ### 주의 ###
//
// 1byte로 byte alignment가 설정되어 있기 때문에
// 이 아래 이후로 include 절대 하면 안됩니다. 
// - birdkr(2007-06-29)
//
// ### 주의 ###

#pragma pack(push, old)
#pragma pack(1)

struct TDL_DATE
{
	tm date;
};

struct TDL_LOG_ITEM
{
	int64	nIUID;
	int		nQuantity;
};


#pragma pack(pop, old)


#endif