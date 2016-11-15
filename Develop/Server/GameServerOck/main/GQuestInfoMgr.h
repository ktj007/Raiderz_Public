#ifndef _GQUEST_INFO_MGR_H
#define _GQUEST_INFO_MGR_H

#include "CSQuestInfoMgr.h"

class CSQuestInfo;
class GQuestInfo;
class GQObjectiveInfo;
class CSQObjectiveInfo;

class GQuestInfoMgr : public CSQuestInfoMgr, public MTestMemPool<GQuestInfoMgr>
{
public:
	GQuestInfoMgr();
	virtual ~GQuestInfoMgr();

	virtual CSQuestInfo* NewQuestInfo();
	virtual CSQObjectiveInfo* NewQObjectiveInfo();
	GQuestInfo*	Get(int nQuestID);
	vector<GQObjectiveInfo*> GetQObjectiveByParam1(int nQObjectiveType, int nQObjectiveParam1);

	bool IsChallengerQuestID(int nQuestID);
	vector<GQuestInfo*> GetAllQuestInfo();
	virtual void Cooking() override;

private:
	virtual void ParseQuest(MXmlElement* pElement, MXml* pXml, CSQuestInfo* pQuestInfo);
	virtual void ParseQRewards(MXmlElement* pElement, MXml* pXml, CSQuestInfo* pQuestInfo);
	void ParseItem(MXmlElement* pElement, MXml* pXml, GQuestInfo* pQuestInfo);	

	vector<GQObjectiveInfo*> ToGQObjectiveInfo(vector<CSQObjectiveInfo*> vecInfo);
};

#endif // _GQUEST_INFO_MGR_H