#ifndef _GPLAYER_DB_H
#define _GPLAYER_DB_H

#include "GDef.h"
#include "GAccountInfo.h"

class GPlayerObject;
class GTalentInfoMgr;

// 실제 DB 작업시작하면 없어질 클래스.
// 일단 플레이어 정보를 파일에 저장해둔다.
class GPlayerDB
{
private:
	string GetFullFileName(string strUserID);
public:
	GPlayerDB() {}
	virtual ~GPlayerDB() {}

	virtual bool Save(GPlayerObject* pPlayerObject);	
	virtual bool Load(GPlayerObject* poutPlayerObject);
	virtual bool LoadForAccountCharList(GAccountCharList& outCharList);

	virtual bool Delete(const char* szName);
};





#endif