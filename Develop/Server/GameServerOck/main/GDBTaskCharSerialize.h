#ifndef _GDBTASK_SERIALIZESELECTCHARACTER_H
#define _GDBTASK_SERIALIZESELECTCHARACTER_H


#include "GDBAsyncTask.h"
#include "GDBTaskData.h"
#include "CSDef.h"
#include "MMemPool.h"
#include "GDBTaskItem.h"


class PLAYER_INFO;


class GEntityPlayer;
class GItem;
class GPlayerQuest;

struct GDBT_CHARINFO
{
	SEX						nSex;
	int64					nCID;
	int						nLevel;
	int						nXP;
	int						nRemaindTP;
	int						nMoney;
	int						nStorageMoney;
	int						nHP;
	int						nEN;
	int						nSTA;
	int						nGID;
	SH_ITEM_SWITCH_WEAPON	nWeaponSet;
	int8					nFeatureHair;
	int8					nFeatureFace;
	short					nFeatureHairColor;
	short					nFeatureSkinColor;
	SBID					nSoulBindingID;
	int8					nInnRoomID;
	PALETTE_NUM				ActivePaletteID;
	PALETTE_NUM				PrimaryPaletteID;
	PALETTE_NUM				SecondaryPaletteID;
	wstring					strName;
	short					nFatigueQuantity;
	int						nBlankTimeSec;
	float					fPosX;
	float					fPosY;
	float					fPosZ;
	int						nSharedFieldID;
	float					fEnterPosX;
	float					fEnterPosY;
	float					fEnterPosZ;
	MUID					uidDynamicFieldGroup;
	int						nDynamicFieldID;
	int						nCheckPointFieldID;
	int						nCheckPointMarkerID;
	MUID					uidParty;
	MUID					uidArena;
	MUID					uidArenaTeam;
	uint8					nPlayerGrade;
	int						nPlayTimeSec;
	int						nElapsedDeadTimeSec;
};

struct GDBT_EQUIPSLOT
{
	int		nType;
	int64	nIUID;
};

struct GDBT_FACTION
{
	uint8	nFactionID;
	uint16	nFaction;
};

struct GDBT_PALETTE
{
	uint8					nOrderID;
	PALETTE_ITEM_TYPE		nType;
	int						nID;
};

struct GDBT_QUESTINSTANCE
{
	uint8	nSlotID;
	int		nQID;
	int		nVar;
	uint8	nDone;
	DWORD	dwElapsedSec;	///<퀘스트를 받은 후 지난 시간. 밀리초.
	wstring strAcceptDt;
	wstring strExpiDt;
	int16	nObj1Progress;
	int16	nObj2Progress;
	int16	nObj3Progress;
	int16	nObj4Progress;
	int16	nObj5Progress;
};

struct GDBT_QUESTHISTORY
{
	int nQID;
	int nDoneCnt;
};

struct GDBT_MAIL_SUMMARY_INFO
{
	int64	nTopMailUID;
	int64	nBottomMailUID;
	int		nUnreadMailCount;
	int		nTotalMailCount;
};

typedef deque<GDBT_FACTION>			FactionQ;
typedef deque<GDBT_PALETTE>			PaletteQ;
typedef vector<REMAIN_BUFF_TIME>	BuffRemaindSecVec;
typedef deque<int>					TalentQ;
typedef vector<pair<int, float>>	TalentRemaindSecVec;
typedef vector<int>					SawnCutsceneVec;
typedef deque<int>					RecipeQ;
typedef deque<GDBT_QUESTINSTANCE>	QuestQ;
typedef deque<GDBT_QUESTHISTORY>	QusetHistoryQ;
typedef deque<int>					QualifyQ;

struct GCHAR_SERIAL_DATA
{
	MUID					uidPlayer;
	UIID					nUIID;
	int64					nCID;
	bool					bReload;

	GDBT_CHARINFO			CharInfo;
	GDBT_ITEMINSTANCEQ		qItemInst;
	TalentQ					qTalent;
	PaletteQ				qPalette;
	TalentRemaindSecVec		vecTalentCollTimeSec;
	BuffRemaindSecVec		vecBuffRemaindSec;
	QuestQ					qQuestInstance;
	QusetHistoryQ			qQuestInstanceDontDoing;
	FactionQ				qFaction;
	SawnCutsceneVec			vecSawnCutscene;
	RecipeQ					qRecipe;
	QualifyQ				qEmblem;
	GDBT_MAIL_SUMMARY_INFO	MailSummary;
};

class GDBTaskCharSerialize : public GDBAsyncTask, public MMemPool<GDBTaskCharSerialize>
{
public :
	GDBTaskCharSerialize(const MUID& uidReqPlayer);
	~GDBTaskCharSerialize();

	

	enum
	{
		CHAR_INFO = 0
		, ITEM
		, TALENT
		, PALETTE
		, REMAIND_TIME
		, QLIST_DOING
		, QLIST_DONTDOING
		, FACTION_LIST
		, SAWN_CUTSCENE_LIST
		, RECIPE_LIST
		, QUALIFY_LIST
		, MAIL_SUMMARY
		, END
	};


	void					Input(const int64 nCID, const UIID& nUIID, bool bReload);


	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT _OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;

	
private :
	bool					GetCharInfo(mdb::MDatabase& rfDB);
	bool					GetCharItemList(mdb::MDatabase& rfDB);
	bool					GetCharTalent(mdb::MDatabase& rfDB);
	bool					GetCharPalette(mdb::MDatabase& rfDB);
	bool					GetEffectRemaindSecondsList(mdb::MDatabase& rfDB);
	bool					GetQuestListDoing(mdb::MDatabase& rfDB);
	bool					GetQuestListDontDoing(mdb::MDatabase& rfDB);
	bool					GetFactionList(mdb::MDatabase& rfDB);
	bool					GetSawnCutsceneList(mdb::MDatabase& rfDB);
	bool					GetRecipeList(mdb::MDatabase& rfDB);
	bool					GetEmblem(mdb::MDatabase& rfDB);
	bool					GetMailSummaryInfo(mdb::MDatabase& rfDB);


protected :
	class Completer
	{
	public :
		Completer(GCHAR_SERIAL_DATA& Data);
		void Do();

	private :
		void Clear(GEntityPlayer* pEntityPlayer);

		void SetInfo(GEntityPlayer* pEntityPlayer);
		void SetPlayerInfo(GEntityPlayer* pEntityPlayer);
		void SetArenaUID(GEntityPlayer* pEntityPlayer);
		void SetState(GEntityPlayer* pEntityPlayer);
		void SetPalette(GEntityPlayer* pEntityPlayer);
		void AddPalette(GEntityPlayer* pEntityPlayer);
		void SetGateInfo(GEntityPlayer* pEntityPlayer);

		void AddItemList(GEntityPlayer* pEntityPlayer);
		void RemoveExpiredItem(GEntityPlayer* pEntityPlayer);
		void SetActiveWeaponSet(GEntityPlayer* pEntityPlayer);

		void AddTalentList(GEntityPlayer* pEntityPlayer);
		void AddEffectRemainSecList(GEntityPlayer* pEntityPlayer);

		void AddDoingQuestList(GEntityPlayer* pEntityPlayer);
		void AddQuestObjective(GEntityPlayer* pEntityPlayer, GPlayerQuest* pPlayerQuest, int nObjectiveID, int nDBProgress);

		void AddDoneCount(GEntityPlayer* pEntityPlayer);

		void AddFaction(GEntityPlayer* pEntityPlayer);
		void AddSawnCutscene(GEntityPlayer* pEntityPlayer);
		void AddRepice(GEntityPlayer* pEntityPlayer);
		void AddEmblem(GEntityPlayer* pEntityPlayer);
		void SetMailSummaryInfo(GEntityPlayer* pEntityPlayer);
		
		void SerializeGuld(GEntityPlayer* pEntityPlayer);

		void PostResponseLoginGameServer();

	private :
		GCHAR_SERIAL_DATA& m_Data;
	};


protected :
	GCHAR_SERIAL_DATA m_Data;
};


#endif