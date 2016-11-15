#pragma once

class GPresetInfo : public MTestMemPool<GPresetInfo>
{
public:
	GPresetInfo(void);
	~GPresetInfo(void);

	wstring strID;
	int		nLevel;
	int		nTP;

	set<int> setTalentID;
	set<int> setItemID;
};
