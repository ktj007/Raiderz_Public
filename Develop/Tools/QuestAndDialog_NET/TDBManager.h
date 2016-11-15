#ifndef _TDBMANAGER_H
#define _TDBMANAGER_H


class TDatabase;

using namespace System;

/// DB 관리자
class TDBManager 
{
private:
	TDatabase*					m_pDB;
	std::string					m_strDSNConnect;

	vector<int>					m_vecQIDOfGetQuestFromDB;					///<DB로 부터 Quest 테이블을 불러올때의 QuestID
	vector<pair<int, int>>		m_vecQIDANDOIDOfGetOObjectiveFromDB;		///<DB로 부터 Objective 테이블을 불러올때 순서형 QuestID와 ObjectiveID
	vector<pair<int, int>>		m_vecQIDANDOIDOfGetUObjectiveFromDB;		///<DB로 부터 Objective 테이블을 불러올때 비순서형 QuestID와 ObjectiveID
	vector<pair<int, int>>		m_vecQIDANDFIDOfGetQuestPRFactionFromDB;	///<DB로 부터 QeustRequiredFaction 테이블을 불러올때의 QuestID, FactionID

	bool GetQuestsFromDB(int nQuestID = -1);
	bool GetQuestBaseInfoFromDB(int nQuestID = -1);
	bool GetQuestPRFromDB(int nQuestID = -1);
	bool GetQuestOObjectiveFromDB(int nQuestID = -1);
	bool GetQuestUObjectiveFromDB(int nQuestID = -1);
	bool GetQuestPRFactionFromDB(int nQuestID = -1, int nFactionID = -1);

	void SetQuestToDB();
	bool InsertQuestToDB(int nQuestID);
	bool UpdateQuestToDB(int nQuestID);
	bool DeleteQuestToDB(int nQuestID = -1);

	void SetQuestOObjectiveToDB();
	void SetQuestUObjectiveToDB();
	bool InsertQuestObjectiveToDB(int nQuestID, int nObjectiveID);
	bool UpdateQuestObjectiveToDB(int nQuestID, int nObjectiveID);
	bool DeleteQuestObjectiveToDB(int nQuestID = -1, int nObjectiveID = -1);

	void SetQuestPRFactionToDB();
	bool InsertQuestPRFactionToDB(int nQuestID, int nFactionID);
	bool UpdateQuestPRFactionToDB(int nQuestID, int nFactionID);
	bool DeleteQuestPRFactionToDB(int nQuestID = -1, int nFactionID = -1);

	void SaveStateOfGetQuestsFromDB();
	void SaveStateOfGetQuestFromDB();								///< DB로 부터 Quest 테이블을 불러올때의 상태를 저장했다가, DB에 저장할때 참고한다.
	void SaveStateOfGetQuestObejctiveFromDB();						///< DB로 부터 Objective 테이블을 불러올때의 상태를 저장했다가, DB에 저장할때 참고한다.
	void SaveStateOfGetQuestPRFactionFromDB();						///< DB로 부터 QeustRequiredFaction 테이블을 불러올때의 상태를 저장했다가, DB에 저장할때 참고한다.
	void ClearStateOfGetQuests();

	void DBUOIDtoRealUOID();										///< DB에서 불러올때 +100되어 있는 비순서형 Objective를 -100 해준다.
	void RealUOIDtoDBUOID();										///< DB에 저장할때 비순서형 Objective를 +100 해준다.

	String^ ConvertToDBDateTimeFormat(String^ strDate);				///< 오전, 오후를 1~12, 1~24시로 바꾼다.


	
public:
	TDBManager(TDatabase* pDB);
	~TDBManager();
	bool Begin();
	void End();

	static void LogCallback( const string& strLog );

	bool Execute(String^ strSQL);
	bool Execute(String^ strSQL, DataSet^ dstDs, String^ strTable);

	void SetQuestsToDB();

	DataTable^ GetQuestBaseInfo();
	DataTable^ GetQuestPR();
	DataTable^ GetQuestPRFaction();
	DataTable^ GetQuestOObjective();
	DataTable^ GetQuestUObjective();


	DataTable^ CopyQuestBaseInfo(int nQuestID);
	void InsertQuestBaseInfo(int nQuestID);
	void UpdateQuestBaseInfo(int nQuestID, String^ strColumnName, Object^ objValue);
	void DeleteQuestBaseInfo(int nQuestID);
	
	DataTable^ CopyQuestPR(int nQuestID);
	void InsertQuestPR(int nQuestID, int nPRQuestID);
	void UpdateQuestPR(int nQuestID, String^ strColumnName, Object^ objValue);
	void DeleteQuestPR(int nQuestID);

	DataTable^ CopyQuestPRFaction(int nQuestID);
	void InsertQuestPRFaction(int nQRFID, int nQuestID);
	void UpdateQuestPRFaction(int nQRFID, String^ strColumnName, Object^ objValue);
	void DeleteQuestPRFaction(int nQRFID, int nQuestID);
	void DeleteQuestPRFaction(int nQuestID);

	DataTable^ CopyQuestOObjective(int nQuestID);
	void InsertQuestOObjective(int nQuestID, int nObjectiveID);
	void UpdateQuestOObjective(int nQuestID, int nObjectiveID, String^ strColumnName, Object^ objValue);
	void DeleteQuestOObjective(int nQuestID, int nObjectiveID);
	void DeleteQuestOObjective(int nQuestID);

	DataTable^ CopyQuestUObjective(int nQuestID);
	void InsertQuestUObjective(int nQuestID, int nObjectiveID);
	void UpdateQuestUObjective(int nQuestID, int nObjectiveID, String^ strColumnName, Object^ objValue);
	void DeleteQuestUObjective(int nQuestID, int nObjectiveID);
	void DeleteQuestUObjective(int nQuestID);

	void InsertQuests(int nQuestID, int nPRQuestID);
	void DeleteQuests(int nQuestID);

	String^ GetStringsKeyFromQuestBaseInfo(int nQuestID, String^ strColumn);
	String^ GetStringsKeyFromQuestOObjective(int nQuestID, int nObjectiveID, String^ strColumn);
	String^ GetStringsKeyFromQuestUObjective(int nQuestID, int nObjectiveID, String^ strColumn);

	bool HaveQuest(int nQuestID);
	bool HaveQuestObjective(int nQuestID, int nObjectiveID);

public:
};


#endif //_TDBMANAGER_H
