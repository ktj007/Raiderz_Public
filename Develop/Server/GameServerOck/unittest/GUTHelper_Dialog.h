#pragma once

class GDialogInfo;
class GDialogSayInfo;
class GDialogSelectInfo;

class GUTHelper_Dialog
{
public:
	GDialogInfo* NewDialogInfo(int nID=0);
	GDialogSayInfo* NewDialogSayInfo();
	GDialogSelectInfo* NewDialogSelectInfo(int nExit=1);
};
