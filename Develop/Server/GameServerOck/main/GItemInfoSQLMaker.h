#ifndef _GITEM_SQL_MAKER_H
#define _GITEM_SQL_MAKER_H


#include "GSQLMaker.h"


#include <string>


class GItemManager;


class GItemInfoSQLMaker : public GSQLMaker, public MTestMemPool<GItemInfoSQLMaker>
{
public :
	GItemInfoSQLMaker(GItemManager* pMgr);
	~GItemInfoSQLMaker();

	bool BuildSQL() override;


private :
	GItemManager* m_pMgr;
};


#endif