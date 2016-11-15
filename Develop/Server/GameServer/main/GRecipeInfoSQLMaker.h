#ifndef _GRECIPE_SQL_MAKER_H
#define _GRECIPE_SQL_MAKER_H


#include "GSQLMaker.h"


class GRecipeInfoMgr;


class GRecipeInfoSQLMaker : public GSQLMaker, public MTestMemPool<GRecipeInfoSQLMaker>
{
public :
	GRecipeInfoSQLMaker(GRecipeInfoMgr* pRecpInfoMgr) : GSQLMaker(false, true), m_pMgr(pRecpInfoMgr) {}
	~GRecipeInfoSQLMaker() {}

	bool BuildSQL() override;	


private :
	GRecipeInfoMgr* m_pMgr;
};


#endif