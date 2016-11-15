#ifndef _GQUEST_INFO_H
#define _GQUEST_INFO_H

#include "CSQuestInfo.h"
#include "MXml.h"

class GItemAddRemoveInfo;
class GQObjectiveInfo;
class CSQObjectiveInfo;

/// 퀘스트 정보
class GQuestInfo : public CSQuestInfo, public MTestMemPool<GQuestInfo>
{
public:
	GQuestInfo();
	virtual ~GQuestInfo();	

	GQObjectiveInfo* Get(int nQuestID);

	vector<GQObjectiveInfo*> GetAllQObjectiveInfo();
	vector<GQObjectiveInfo*> GetActQObjectiveInfo();
	vector<GQObjectiveInfo*> GetEscortQObjectiveInfo();
	vector<GQObjectiveInfo*> GetQObjectiveInfo(QOBJECTIVE_TYPE nType);

private:
	vector<GQObjectiveInfo*> ToGQObjectiveInfo(vector<CSQObjectiveInfo*> vecInfo);

public:		
	GItemAddRemoveInfo*	pItemAddRemoveInfo;	///< 퀘스트 시작할때 지급받거나 제거되는 아이템

	bool				bChallengerQuest;	///< 도전자퀘스트인지 여부

};


#endif // _GQUEST_INFO_H