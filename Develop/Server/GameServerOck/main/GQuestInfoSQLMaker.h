#ifndef _GQUEST_SQL_MAKER_H
#define _GQUEST_SQL_MAKER_H


#include "GSQLMaker.h"


#include <string>


class GQuestInfoMgr;


class GQuestInfoSQLMaker : public GSQLMaker, public MTestMemPool<GQuestInfoSQLMaker>
{
public:
	GQuestInfoSQLMaker(GQuestInfoMgr* mgr);
	~GQuestInfoSQLMaker();

	bool BuildSQL() override;	

private:
	GQuestInfoMgr* m_pMgr;
};

#endif