#ifndef _G_ITEM_INFO_VALIDATOR_H_
#define _G_ITEM_INFO_VALIDATOR_H_

#include "GValidatorLib.h"
#include "CSBaseValidator.h"

class GItemData;

class GVALIDATOR_API GItemInfoValidator : public CSBaseValidator
{
public:
	typedef int	talent_id;
	typedef int item_id;

public:
	virtual bool Check();

private:
	void CheckStackAmount(GItemData* pItemData);
	void CheckSoul(GItemData* pItemData);
	void CheckTier(GItemData* pItemData);
	void CheckSellable(GItemData* pItemData);
	void CheckUsableParam(GItemData* pItemData);
	void CheckGather(GItemData* pItemData);
	void CheckEnchant(GItemData* pItemData);
	void CheckArmorType(GItemData* pItemData);

private:
	map<talent_id, item_id>	m_mapTrainTalent;
};

#endif// _G_ITEM_INFO_VALIDATOR_H_
