#ifndef _CS_QUEST_INFO_H
#define _CS_QUEST_INFO_H

#include <string>
#include "MTstring.h"
#include <vector>
#include "MTypes.h"
#include "CSCommonLib.h"

using namespace std;
enum QUEST_SHARE_TYPE;
enum QUEST_TRIAL_TYPE;
enum QUEST_REWARD_TYPE;
enum QOBJECTIVE_TYPE;

class CSQObjectiveInfo;
struct QUEST_REWARD;

typedef vector<QUEST_REWARD>		VEC_QUEST_REWARD;
typedef map<int, CSQObjectiveInfo*>	MAP_QOBJECTIVEINFO;

/// 퀘스트 정보
class CSCOMMON_API CSQuestInfo
{
public:
	CSQuestInfo();
	virtual ~CSQuestInfo();	

	bool Insert(CSQObjectiveInfo* pQObjectiveInfo);
	CSQObjectiveInfo* Get(int nQObjectiveID);	

	bool IsTrialQuest();
	bool IsEscortQuest();
	bool IsEmptyQObjective();
	bool IsAutoParty()			{ return bAutoParty;}
	bool CheckRequireLevel_ForChallengerQuest(int nLevel) const;

	vector<CSQObjectiveInfo*> GetQObjectiveInfo(QOBJECTIVE_TYPE nType);

	vector<CSQObjectiveInfo*> GetAllQObjectiveInfo();
	vector<CSQObjectiveInfo*> GetActQObjectiveInfo();
	vector<CSQObjectiveInfo*> GetEscortQObjectiveInfo();
	vector<CSQObjectiveInfo*> GetCollectQObjectiveInfo();	

	int GetQObejctiveInfoCount();

public:
	int			nID;				///< 퀘스트 ID	
	tstring		strTitle;			///< 퀘스트 제목
	tstring		strDesc;			///< 퀘스트 설명
	tstring		strCategory;		///< 인터페이스를 위해 사용
	int			nTimeLimit;			///< 수행 제한시간 (초단위)
	int			nLevel;				///< 퀘스트 레벨
	bool		bRepeatable;		///< 반복 수행이 가능한지 여부
	bool		bCancelable;		///< 취소 가능한지 여부
	QUEST_SHARE_TYPE		nShareType;			///< 공유 타입
	int			nLimitPersonnel;				///< 제한 인원수

	QUEST_TRIAL_TYPE	nTrialType;			///< 트라이얼 타입	
	int					nTrialFieldGroupID;	///< 트라이얼 퀘스트를 수행하는 필드그룹	

	int				nGiveDialogID;			///< 퀘스트를 줄 때 사용하는 다이얼로그
	int				nIncompleteDialogID;	///< 퀘스트를 아직 완료하지 못했을때 사용하는 다이얼로그	
	int				nShareConditionsID;		///< 퀘스트 공유가 가능한지 체크하는 컨디션

	int					nRewardDialogID;			///< 보상에 사용하는 다이얼로그	
	int					nRewardNPCID;				///< 보상해주는 NPCID
	int					nRewardFieldID;				///< 보상 필드 ID
	vec3				vRewardPos;					///< 보상 좌표
	VEC_QUEST_REWARD	vecReward;					///< 퀘스트 보상
	VEC_QUEST_REWARD	vecSelectableReward;		///< 선택 가능한 퀘스트 보상

	vector<int>			vecDestroyItemID;			///< 퀘스트 완료와 취소시 파괴되는 아이템	

	int					nPartyFieldID;				///< 자동파티매칭 퀘스트의 필드ID
	int					nPartySensorID;				///< 자동파티매칭 퀘스트의 센서ID
	int					nAutoPartyMemberLimit;		///< 자동파티 최소인원

	bool				bAutoParty;					///< 자동파티매칭이 되는 퀘스트인지 여부
	bool				bPublicVar;					///< var 변경시 파티원도 적용z
	bool				bValidation;				///< ResourceValidator 유효성검사를 할지, 말지 결정 - 기본값 true


	int			nRequiredMinLevelForChallengerQuest;	///< 도전자 퀘스트 수락시 필요 최소레벨
	int			nRequiredMaxLevelForChallengerQuest;	///< 도전자 퀘스트 수락시 필요 최대레벨
	int			nRequiredConditionIDForChallengerQuest;	///< 도전자 퀘스트 수락시 필요 조건


protected:
	MAP_QOBJECTIVEINFO	mapQObjectiveInfo;	///< 퀘스트 목적
};

/// 퀘스트 보상
struct QUEST_REWARD
{
public:
	QUEST_REWARD_TYPE	nType;
	int					nParam1;
	int					nParam2;

public:
	QUEST_REWARD();

	QUEST_REWARD(QUEST_REWARD_TYPE	nType, int nParam1, int	nParam2);
	QUEST_REWARD(QUEST_REWARD_TYPE	nType, int nParam1);
};


#endif // _CS_QUEST_INFO_H
