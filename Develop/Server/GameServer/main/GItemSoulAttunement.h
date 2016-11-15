#pragma once

class GEntityPlayer;
class GItem;

class GItemSoulAttunement : public MTestMemPool<GItemSoulAttunement>
{
public:
	GItemSoulAttunement(void);
	~GItemSoulAttunement(void);

private:
	int val;

};