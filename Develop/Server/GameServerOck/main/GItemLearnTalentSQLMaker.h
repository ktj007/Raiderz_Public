#ifndef _GITEMLEARNTALENT_SQL_MAKER_H
#define _GITEMLEARNTALENT_SQL_MAKER_H


#include "GSQLMaker.h"


#include <string>


class GItemManager;


class GItemLearnTalentMaker : public GSQLMaker, public MTestMemPool<GItemLearnTalentMaker>
{
public :
	GItemLearnTalentMaker(GItemManager* pMgr);
	~GItemLearnTalentMaker();

	bool BuildSQL() override;


private :
	GItemManager* m_pMgr;
};


#endif