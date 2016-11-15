#pragma once

class GEntityPlayer;
class GItemAddRemoveInfo;
class GItemFillInfo;
class GItemAddInfo;
class GItemRemoveInfo;
class GItemRemoveAllInfo;

class GItemAddAndRemoverForInfo : public MTestMemPool<GItemAddAndRemoverForInfo>
{
public:
	bool AddAndRemove(GEntityPlayer* pPlayer, GItemAddRemoveInfo* pItemAddRemoveInfo);
	bool Check(GEntityPlayer* pPlayer, const vector<GItemAddInfo*>& vecAddInfo);

private:
	void Fill(GEntityPlayer* pPlayer, GItemFillInfo* pFillInfo);
	void Add(GEntityPlayer* pPlayer, GItemAddInfo* pAddInfo);
	void Remove(GEntityPlayer* pPlayer, GItemRemoveInfo* pRemoveInfo);
	void RemoveAll(GEntityPlayer* pPlayer, GItemRemoveAllInfo* pRemoveAllInfo);	
};
