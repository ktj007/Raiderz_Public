#ifndef _G_LOOT_INFO_VALIDATOR_H_
#define _G_LOOT_INFO_VALIDATOR_H_

#include "GValidatorLib.h"
#include "CSBaseValidator.h"

class GLootInfo;
struct LOOT_ITEM_INFO;
struct LOOT_MONEY_INFO;

class GVALIDATOR_API GLootInfoValidator : public CSBaseValidator
{
private:
	void CheckLoot(GLootInfo* pLootInfo);
	void CheckItem(LOOT_ITEM_INFO* pLootItemInfo);
	void CheckMoney(LOOT_MONEY_INFO* pLootMoneyInfo);

public:
	virtual bool Check();
};

#endif//_G_LOOT_INFO_VALIDATOR_H_
