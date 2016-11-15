#ifndef _G_TRADE_VALIDATOR_H_
#define _G_TRADE_VALIDATOR_H_

#include "GValidatorLib.h"
#include "CSBaseValidator.h"

class GNPCShopInfo;
class GItemData;

class GVALIDATOR_API GNPCShopInfoValidator : public CSBaseValidator
{
private:
	void CheckNPCShop(GNPCShopInfo* pNPCShopInfo);
	bool CheckItemSellPriceIsHigherThanBuyPrice(GNPCShopInfo* pNPCShopInfo, GItemData* pItemData);	

public:
	virtual bool Check();
};

#endif//_G_TRADE_VALIDATOR_H_
