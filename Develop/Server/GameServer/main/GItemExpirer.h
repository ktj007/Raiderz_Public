#pragma once

#include "GTickRegulator.h"

class GEntityPlayer;
class GItem;


typedef	vector<GItem*> GExprieItemVec;


class GItemExpirer : public MTestMemPool<GItemExpirer>
{
public:
	GItemExpirer(GEntityPlayer* pPlayer);
	~GItemExpirer(void);

	void Init();

	void Run(float fDelta);
	void RunImmediately();

	GExprieItemVec& GetExpiredItemList();		///< 기간 만료 삭제된 아이템 목록. Run(), RunImmediately()를 호출 한 후에 사용해야 한다.

private :
	void ExpireItems();
	void BuildExpiredItemList();
	void ExpireEachItem();

private:
	GEntityPlayer*	m_pOwnerPlayer;
	GTickRegulator	m_CheckTimeRegulator;
	GExprieItemVec	m_vecExpiredItem;
};
