#ifndef _G_DIALOG_INFO_VALIDATOR_H_
#define _G_DIALOG_INFO_VALIDATOR_H_

#include "GValidatorLib.h"
#include "CSBaseValidator.h"

class GDialogInfo;
class GDialogSayInfo;
class GDialogSelectInfo;
class GItemAddRemoveInfo;

class GVALIDATOR_API GDialogInfoValidator : public CSBaseValidator
{
public:
	void CheckDialog(GDialogInfo* pDialogInfo);
	void CheckSay(GDialogSayInfo* pDialogSayInfo);
	void CheckSelect(GDialogSelectInfo* pDialogSelectInfo);
	void CheckItem(GItemAddRemoveInfo* pItemInfo);

public:
	virtual bool Check();	
};

#endif//_G_DIALOG_INFO_VALIDATOR_H_
