#pragma once

#include "CTradeMarketConstant.h"

class GEntityPlayer;

class GTradeMarket
{
public:
	static bool PutOn(const MUID& playerUID,const int& slot,const int& price,const int& quantity,const int& expiryDate);
	static void Search(const MUID& playerUID,const int& itemUID);
};