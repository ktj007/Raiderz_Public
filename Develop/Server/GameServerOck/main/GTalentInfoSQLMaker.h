#ifndef _GTALENT_SQL_MAKER_H
#define _GTALENT_SQL_MAKER_H


#include "GSQLMaker.h"


class GTalentInfoMgr;


class GTalentInfoSQLMaker : public GSQLMaker, public MTestMemPool<GTalentInfoSQLMaker>
{
public :
	GTalentInfoSQLMaker(GTalentInfoMgr* pMgr);
	~GTalentInfoSQLMaker();

	bool BuildSQL() override;	

private :
	GTalentInfoMgr* m_pMgr;	
};

#endif