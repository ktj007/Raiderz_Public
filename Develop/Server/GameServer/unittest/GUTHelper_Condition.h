#pragma once

class GConditionsInfo;
class GCondition_Leaf;

class GUTHelper_Condition
{
public:
	static GConditionsInfo* NewCondI(GCondition_Leaf* pCondL=NULL, bool bItemCondition=false);
	
	static GCondition_Leaf* NewCondL();
	static GCondition_Leaf* NewCondLForLevel(int nMinLevel=0, int nMaxLevel=0);	
	static GCondition_Leaf* NewCondLForFail();
};
