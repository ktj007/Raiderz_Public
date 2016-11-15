#include "stdafx.h"
#include <vector>
#include "TDatabase.h"
#include "TDBManager.h"
#include "TQuestConfig.h"

TDBManager::TDBManager(TDatabase* pDB)
{
	m_pDB = pDB;
	m_pDB->SetLogCallback( LogCallback );
	TDBManagerGlobal::m_dsQuest = gcnew DataSet();
}

TDBManager::~TDBManager()
{
	if (nullptr != this->m_pDB)
		delete this->m_pDB;
}

bool TDBManager::Begin()
{
	if (false == this->GetQuestsFromDB()) return false;
	this->SaveStateOfGetQuestsFromDB();
	this->DBUOIDtoRealUOID();

	return true;
}

void TDBManager::End()
{
	this->RealUOIDtoDBUOID();
	this->ClearStateOfGetQuests();

	TDBManagerGlobal::m_dsQuest->Clear();
}


void TDBManager::LogCallback( const string& strLog )
{
	mlog( strLog.c_str() );
}

bool TDBManager::Execute(String^ strSQL)
{
	try 
	{
		mlog0("DB SQL - %s\n", MStringToCharPointer(strSQL));
		m_pDB->ExecuteSQL(strSQL);
	} 
	catch(OleDbException^ e)
	{
		mlog("DB Query Failed( %s ) - [ %s ]\n", MStringToCharPointer(strSQL), e->Errors);
		return false;
	}

	return true;
}


bool TDBManager::Execute(String^ strSQL, DataSet^ dstDs, String^ strTable)
{
	try 
	{
		mlog0("DB SQL - %s\n", MStringToCharPointer(strSQL));
		m_pDB->ExecuteSQL(strSQL, dstDs, strTable);
	} 
	catch(OleDbException^ e)
	{
		mlog("DB Query Failed( %s ) - [ %s ]\n", MStringToCharPointer(strSQL), e->Errors);
		return false;
	}

	return true;
}

bool TDBManager::GetQuestsFromDB(int nQuestID)
{
	if (false == this->GetQuestBaseInfoFromDB(nQuestID)) return false;
	if (false == this->GetQuestPRFromDB(nQuestID)) return false;
	if (false == this->GetQuestOObjectiveFromDB(nQuestID)) return false;
	if (false == this->GetQuestUObjectiveFromDB(nQuestID)) return false;
	if (false == this->GetQuestPRFactionFromDB(nQuestID)) return false;

	return true;
}

bool TDBManager::GetQuestBaseInfoFromDB(int nQuestID)
{
	String^ strSQL = String::Format("{{CALL spGetQuestBaseInfo({0})}}", nQuestID);

	return this->Execute(strSQL, TDBManagerGlobal::m_dsQuest, QUEST_DB_VTABLE_QUESTBASEINFO);
}


bool TDBManager::GetQuestPRFromDB(int nQuestID)
{
	String^ strSQL = String::Format("{{CALL spGetQuestPR({0})}}", nQuestID);

	return this->Execute(strSQL, TDBManagerGlobal::m_dsQuest, QUEST_DB_VTABLE_QUESTPR);
}


bool TDBManager::GetQuestOObjectiveFromDB(int nQuestID)
{
	String^ strSQL = String::Format("{{CALL spGetQuestOObjective({0})}}", nQuestID);

	return this->Execute(strSQL, TDBManagerGlobal::m_dsQuest, QUEST_DB_VTABLE_OOBJECTIVE);
}

bool TDBManager::GetQuestUObjectiveFromDB(int nQuestID)
{
	String^ strSQL = String::Format("{{CALL spGetQuestUObjective({0})}}", nQuestID);

	return this->Execute(strSQL, TDBManagerGlobal::m_dsQuest, QUEST_DB_VTABLE_UOBJECTIVE);
}

bool TDBManager::GetQuestPRFactionFromDB(int nQuestID, int nFactionID)
{
	String^ strSQL = String::Format("{{CALL spGetQuestRequiredFaction({0}, {1})}}", nQuestID, nFactionID);

	return this->Execute(strSQL, TDBManagerGlobal::m_dsQuest, QUEST_DB_TABLE_QUESTPRFACTION);
}

void TDBManager::SetQuestsToDB()
{
	this->RealUOIDtoDBUOID();
	this->SetQuestToDB();
	this->SetQuestOObjectiveToDB();
	this->SetQuestUObjectiveToDB();
	this->SetQuestPRFactionToDB();
}

void TDBManager::SetQuestToDB()
{
	DataTable^ dtQuestBaseInfo = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTBASEINFO];
	if (nullptr == dtQuestBaseInfo) return;

	for (int i = 0; dtQuestBaseInfo->Rows->Count > i; i++)
	{
		DataRow^ drQuestBaseInfo = dtQuestBaseInfo->Rows[i];
		int nPresentQuestID = MStringToInt(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_QID]->ToString());
		bool bInsert = true;

		for each(int nPastQuestID in this->m_vecQIDOfGetQuestFromDB)
		{
			if (nPresentQuestID == nPastQuestID)
			{
				bInsert = false;
				break;
			}
		}

		if (true == bInsert)
		{
			if (false == this->InsertQuestToDB(nPresentQuestID))
			{
				this->UpdateQuestToDB(nPresentQuestID);
			}
		}
		else
		{
			this->UpdateQuestToDB(nPresentQuestID);
		}
	}


	for each(int nPastQuestID in this->m_vecQIDOfGetQuestFromDB)
	{
		bool bDelete = true;

		for (int i = 0; dtQuestBaseInfo->Rows->Count > i; i++)
		{
			DataRow^ drQuestBaseInfo = dtQuestBaseInfo->Rows[i];
			int nPresentQuestID = MStringToInt(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_QID]->ToString());

			if (nPastQuestID == nPresentQuestID)
			{
				bDelete = false;
				break;
			}
		}
		if (true == bDelete)
		{
			this->DeleteQuestToDB(nPastQuestID);
		}
		else
		{
			this->UpdateQuestToDB(nPastQuestID);
		}
	}
}

//char g_szDB_INSERT_QUEST[] = "{CALL spInsertQuest(%d, '%s', %d, %d, %d, %d, %s, %d, '%s', %s, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)}";

bool TDBManager::InsertQuestToDB(int nQuestID)
{
	String^ strFilter = String::Format("{0}={1}", QUEST_DB_TABLE_QUEST_COLUMN_QID, nQuestID);

	DataTable^ dtQuestBaseInfo = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTBASEINFO];
	if (nullptr == dtQuestBaseInfo) return false;
	array<DataRow^>^ arrDrQuestBaseInfo = dtQuestBaseInfo->Select(strFilter);
	if (1 != arrDrQuestBaseInfo->Length) return false;

	DataRow^ drQuestBaseInfo = arrDrQuestBaseInfo[0];
	String^ strTitle = drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_TITLE]->ToString();
	int nRaidable = MStringToInt(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_RAIDABLE]->ToString());
	int nQA = MStringToInt(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_QA]->ToString());
	int nRepeatable = MStringToInt(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_REPEATABLE]->ToString());
	int nType = MStringToInt(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_TYPE]->ToString());
	String^ strZoneDesc = drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_ZONEDESCRIPTION]->ToString();
	int nHideTheNextStep = MStringToInt(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_HIDETHENEXTSTEP]->ToString());
	String^ strDesc = drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_DESCRIPTION]->ToString();
	String^ strDeadLine = this->ConvertToDBDateTimeFormat(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_DEADLINE]->ToString()) ;
	String^ strQuotedDeadLine = nullptr;
	if (nullptr ==  strDeadLine)
	{
		strQuotedDeadLine = "NULL";
	}
	else
	{
		strQuotedDeadLine = String::Format("'{0}'", strDeadLine);
	}
	int nZoneLifeTime = MStringToInt(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_ZONELIFETIME]->ToString());

	DataTable^ dtQuestPR = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTPR];
	if (nullptr == dtQuestPR) return false;
	array<DataRow^>^ arrDrQuestPR = dtQuestPR->Select(strFilter);
	if (1 != arrDrQuestPR->Length) return false;

	DataRow^ drQuestPR = arrDrQuestPR[0];
	int nPRMinLevel = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_MINLEVEL]->ToString());
	int nPRMaxLevel = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_MAXLEVEL]->ToString());
	int nPRQuest = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_QUEST]->ToString());
	int nPRUncompletedQuest = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_UNCOMPLETEDQUEST]->ToString());
	int nPRItemID = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_ITEMID]->ToString());
	int nPREqupItem = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_EQUIPITEM]->ToString());
	int nPRRace = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_RACE]->ToString());
	int nPRSex = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_SEX]->ToString());
	int nPRHaveFID = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_HaveFID]->ToString());

	String^ strSQL = 
		String::Format(
		"{{CALL spInsertQuest({0}, '{1}', {2}, {3}, {4}, {5}, '{6}', {7}, '{8}', {9}, {10}, {11}, {12}, {13}, {14}, {15}, {16}, {17}, {18}, {19})}}",
		nQuestID, strTitle, nRaidable, nQA, nRepeatable, nType, strZoneDesc, nHideTheNextStep, strDesc, strQuotedDeadLine, nZoneLifeTime,
		nPRMinLevel, nPRMaxLevel, nPRQuest, nPRUncompletedQuest, nPRItemID, nPREqupItem, nPRRace, nPRSex, nPRHaveFID);

	return this->Execute(strSQL);
}

char g_szDB_UPDATE_QUEST[] = "{CALL spUpdateQuest(%d, '%s', %d, %d, %d, %d, %s, %d, '%s', %s, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)}";

bool TDBManager::UpdateQuestToDB(int nQuestID)
{
	char szFilter[512];
	sprintf_s(szFilter, 512, "%s=%d", QUEST_DB_TABLE_QUEST_COLUMN_QID, nQuestID);
	String^ strFilter = gcnew String(szFilter);

	DataTable^ dtQuestBaseInfo = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTBASEINFO];
	if (nullptr == dtQuestBaseInfo) return false;
	array<DataRow^>^ arrDrQuestBaseInfo = dtQuestBaseInfo->Select(strFilter);
	if (1 != arrDrQuestBaseInfo->Length) return false;

	DataRow^ drQuestBaseInfo = arrDrQuestBaseInfo[0];
	String^ strTitle = drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_TITLE]->ToString();
	int nRaidable = MStringToInt(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_RAIDABLE]->ToString());
	int nQA = MStringToInt(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_QA]->ToString());
	int nRepeatable = MStringToInt(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_REPEATABLE]->ToString());
	int nType = MStringToInt(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_TYPE]->ToString());
	String^ strZoneDesc = drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_ZONEDESCRIPTION]->ToString();
	int nHideTheNextStep = MStringToInt(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_HIDETHENEXTSTEP]->ToString());
	String^ strDesc = drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_DESCRIPTION]->ToString();
	String^ strDeadLine =  this->ConvertToDBDateTimeFormat(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_DEADLINE]->ToString());
	String^ strQuotedDeadLine = nullptr;
	if (nullptr ==  strDeadLine)
	{
		strQuotedDeadLine = "NULL";
	}
	else
	{
		strQuotedDeadLine = String::Format("'{0}'", strDeadLine);
	}
	int nZoneLifeTime = MStringToInt(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_ZONELIFETIME]->ToString());

	DataTable^ dtQuestPR = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTPR];
	if (nullptr == dtQuestPR) return false;
	array<DataRow^>^ arrDrQuestPR = dtQuestPR->Select(strFilter);
	if (1 != arrDrQuestPR->Length) return false;

	DataRow^ drQuestPR = arrDrQuestPR[0];
	int nPRMinLevel = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_MINLEVEL]->ToString());
	int nPRMaxLevel = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_MAXLEVEL]->ToString());
	int nPRQuest = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_QUEST]->ToString());
	int nPRUncompletedQuest = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_UNCOMPLETEDQUEST]->ToString());
	int nPRItemID = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_ITEMID]->ToString());
	int nPREqupItem = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_EQUIPITEM]->ToString());
	int nPRRace = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_RACE]->ToString());
	int nPRSex = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_SEX]->ToString());
	int nPRHaveFID = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_HaveFID]->ToString());

	String^ strSQL = 
		String::Format(
		"{{CALL spUpdateQuest({0}, '{1}', {2}, {3}, {4}, {5}, '{6}', {7}, '{8}', {9}, {10}, {11}, {12}, {13}, {14}, {15}, {16}, {17}, {18}, {19})}}",
		nQuestID, strTitle, nRaidable, nQA, nRepeatable, nType, strZoneDesc, nHideTheNextStep, strDesc, strQuotedDeadLine, nZoneLifeTime,
		nPRMinLevel, nPRMaxLevel, nPRQuest, nPRUncompletedQuest, nPRItemID, nPREqupItem, nPRRace, nPRSex, nPRHaveFID);

	return this->Execute(strSQL);
}

char g_szDB_DELETE_QUEST[] = "{CALL spDeleteQuest(%d)}";
bool TDBManager::DeleteQuestToDB(int nQuestID)
{
	String^ strSQL = String::Format("{{CALL spDeleteQuest({0})}}", nQuestID);

	return this->Execute(strSQL);
}

void TDBManager::SetQuestOObjectiveToDB()
{
	DataTable^ dtQuestOObjective = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_OOBJECTIVE];
	if (nullptr == dtQuestOObjective) return;

	for (int i = 0; dtQuestOObjective->Rows->Count > i; i++)
	{
		DataRow^ drQuestOObjective = dtQuestOObjective->Rows[i];
		int nPresentQuestID = MStringToInt(drQuestOObjective[QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID]->ToString());
		int nPresentObjectiveID = MStringToInt(drQuestOObjective[QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->ToString());

		bool bInsert = true;
		for each(pair<int,int> itor in this->m_vecQIDANDOIDOfGetOObjectiveFromDB)
		{
			int nPastQuestID = itor.first;
			int nPastObjectiveID = itor.second;
			if (nPresentQuestID == nPastQuestID && nPresentObjectiveID == nPastObjectiveID)
			{
				bInsert = false;
				break;
			}
		}

		if (true == bInsert)
		{
			if (false == this->InsertQuestObjectiveToDB(nPresentQuestID, nPresentObjectiveID))
			{
				this->UpdateQuestObjectiveToDB(nPresentQuestID, nPresentObjectiveID);
			}
		}
		else
		{
			this->UpdateQuestObjectiveToDB(nPresentQuestID, nPresentObjectiveID);
		}
	}


	for each(pair<int,int> itor in this->m_vecQIDANDOIDOfGetOObjectiveFromDB)
	{
		int nPastQuestID = itor.first;
		int nPastObjectiveID = itor.second;

		bool bDelete = true;
		for (int i = 0; dtQuestOObjective->Rows->Count > i; i++)
		{
			DataRow^ drQuestOObjective = dtQuestOObjective->Rows[i];
			int nPresentQuestID = MStringToInt(drQuestOObjective[QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID]->ToString());
			int nPresentObjectiveID = MStringToInt(drQuestOObjective[QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->ToString());

			if (nPastQuestID == nPresentQuestID && nPastObjectiveID == nPresentObjectiveID)
			{
				bDelete = false;
				break;
			}
		}

		if (true == bDelete)
		{
			this->DeleteQuestObjectiveToDB(nPastQuestID, nPastObjectiveID);
		}
		else
		{
			this->UpdateQuestObjectiveToDB(nPastQuestID, nPastObjectiveID);
		}
	}
}

void TDBManager::SetQuestUObjectiveToDB()
{
	DataTable^ dtQuestUObjective = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_UOBJECTIVE];
	if (nullptr == dtQuestUObjective) return;

	for (int i = 0; dtQuestUObjective->Rows->Count > i; i++)
	{
		DataRow^ drQuestUObjective = dtQuestUObjective->Rows[i];
		int nPresentQuestID = MStringToInt(drQuestUObjective[QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID]->ToString());
		int nPresentObjectiveID = MStringToInt(drQuestUObjective[QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->ToString());

		bool bInsert = true;
		for each(pair<int,int> itor in this->m_vecQIDANDOIDOfGetUObjectiveFromDB)
		{
			int nPastQuestID = itor.first;
			int nPastObjectiveID = itor.second;
			if (nPresentQuestID == nPastQuestID && nPresentObjectiveID == nPastObjectiveID)
			{
				bInsert = false;
				break;
			}
		}

		if (true == bInsert)
		{
			if (false == this->InsertQuestObjectiveToDB(nPresentQuestID, nPresentObjectiveID))
			{
				this->UpdateQuestObjectiveToDB(nPresentQuestID, nPresentObjectiveID);
			}
		}
		else
		{
			this->UpdateQuestObjectiveToDB(nPresentQuestID, nPresentObjectiveID);
		}
	}


	for each(pair<int,int> itor in this->m_vecQIDANDOIDOfGetUObjectiveFromDB)
	{
		int nPastQuestID = itor.first;
		int nPastObjectiveID = itor.second;
		int nPresentQuestID;
		int nPresentObjectiveID;

		bool bDelete = true;
		for (int i = 0; dtQuestUObjective->Rows->Count > i; i++)
		{
			DataRow^ drQuestUObjective = dtQuestUObjective->Rows[i];
			nPresentQuestID = MStringToInt(drQuestUObjective[QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID]->ToString());
			nPresentObjectiveID = MStringToInt(drQuestUObjective[QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->ToString());

			if (nPastQuestID == nPresentQuestID && nPastObjectiveID == nPresentObjectiveID)
			{
				bDelete = false;
				break;
			}
		}

		if (true == bDelete)
		{
			this->DeleteQuestObjectiveToDB(nPastQuestID, nPastObjectiveID);
		}
		else
		{
			this->UpdateQuestObjectiveToDB(nPastQuestID, nPastObjectiveID);
		}
	}
}



//char g_szDB_INSERT_QUEST_OBJECTIVE[] = "{CALL spInsertQuestObjective(%d,%d,'%s','%s',%d,%d,%d)}";

bool TDBManager::InsertQuestObjectiveToDB(int nQuestID, int nObjectiveID)
{
	char szFilter[512];
	sprintf_s(szFilter, 512, "%s=%d AND %s=%d", QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID, nQuestID, QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID, nObjectiveID);
	String^ strFilter = gcnew String(szFilter);

	DataTable^ dtQuestObjective = nullptr;
	if (OFFSET_UOBJECTIVE > nObjectiveID)
	{
		dtQuestObjective = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_OOBJECTIVE];
	}
	else
	{
		dtQuestObjective = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_UOBJECTIVE];
	}
	if (nullptr == dtQuestObjective) return false;

	array<DataRow^>^ arrDrQuestObjective = dtQuestObjective->Select(strFilter);
	if (1 != arrDrQuestObjective->Length) return false;

	DataRow^ drQuestObjective = arrDrQuestObjective[0];
	String^ strTitle = drQuestObjective[QUEST_DB_TABLE_OBJECTIVE_COLUMN_TITLE]->ToString();
	String^ strDesc = drQuestObjective[QUEST_DB_TABLE_OBJECTIVE_COLUMN_DESCRIPTION]->ToString();
	int nType = MStringToInt(drQuestObjective[QUEST_DB_TABLE_OBJECTIVE_COLUMN_TYPE]->ToString());
	int nParam1 = MStringToInt(drQuestObjective[QUEST_DB_TABLE_OBJECTIVE_COLUMN_PARAM1]->ToString());
	int nParam2 = MStringToInt(drQuestObjective[QUEST_DB_TABLE_OBJECTIVE_COLUMN_PARAM2]->ToString());

	String^ strSQL = String::Format(
		"{{CALL spInsertQuestObjective({0}, {1}, '{2}', '{3}', {4}, {5}, {6})}}", 
		nQuestID, nObjectiveID, strTitle, strDesc, nType, nParam1, nParam2) ;

	return this->Execute(strSQL);
}

//char g_szDB_UPDATE_QUEST_OBJECTIVE[] = "{CALL spUpdateQuestObjective(%d,%d,'%s','%s',%d,%d,%d)}";

bool TDBManager::UpdateQuestObjectiveToDB(int nQuestID, int nObjectiveID)
{
	char szFilter[512];
	sprintf_s(szFilter, 512, "%s=%d AND %s=%d", QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID, nQuestID, QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID, nObjectiveID);
	String^ strFilter = gcnew String(szFilter);

	DataTable^ dtQuestObjective = nullptr;
	if (OFFSET_UOBJECTIVE > nObjectiveID)
	{
		dtQuestObjective = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_OOBJECTIVE];
	}
	else
	{
		dtQuestObjective = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_UOBJECTIVE];
	}
	if (nullptr == dtQuestObjective) return false;

	array<DataRow^>^ arrDrQuestObjective = dtQuestObjective->Select(strFilter);
	if (1 != arrDrQuestObjective->Length) return false;

	DataRow^ drQuestObjective = arrDrQuestObjective[0];
	String^ strTitle = drQuestObjective[QUEST_DB_TABLE_OBJECTIVE_COLUMN_TITLE]->ToString();
	String^ strDesc = drQuestObjective[QUEST_DB_TABLE_OBJECTIVE_COLUMN_DESCRIPTION]->ToString();
	int nType = MStringToInt(drQuestObjective[QUEST_DB_TABLE_OBJECTIVE_COLUMN_TYPE]->ToString());
	int nParam1 = MStringToInt(drQuestObjective[QUEST_DB_TABLE_OBJECTIVE_COLUMN_PARAM1]->ToString());
	int nParam2 = MStringToInt(drQuestObjective[QUEST_DB_TABLE_OBJECTIVE_COLUMN_PARAM2]->ToString());

	String^ strSQL = String::Format(
		"{{CALL spUpdateQuestObjective({0}, {1},'{2}','{3}',{4}, {5}, {6})}}",
		nQuestID, nObjectiveID, strTitle, strDesc, nType, nParam1, nParam2);
	return this->Execute(strSQL);
}

char g_szDB_DELETE_OBJECTIVE[] = "{CALL spDeleteObjective(%d,%d)}";

bool TDBManager::DeleteQuestObjectiveToDB(int nQuestID, int nObjectiveID)
{
	String^ strSQL = String::Format("{{CALL spDeleteObjective({0}, {1})}}", nQuestID, nObjectiveID);

	return this->Execute(strSQL);
}

void TDBManager::SetQuestPRFactionToDB()
{
	DataTable^ dt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_TABLE_QUESTPRFACTION];
	if (nullptr == dt) return;

	for (int i = 0; dt->Rows->Count > i; i++)
	{
		DataRow^ dr = dt->Rows[i];
		int nPresentQuestID = MStringToInt(dr[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_QID]->ToString());
		int nPresentFactionID = MStringToInt(dr[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_FID]->ToString());

		bool bInsert = true;
		for each(pair<int,int> itor in this->m_vecQIDANDFIDOfGetQuestPRFactionFromDB)
		{
			int nPastQuestID = itor.first;
			int nPastFactionID = itor.second;
			if (nPresentQuestID == nPastQuestID && nPresentFactionID == nPastFactionID)
			{
				bInsert = false;
				break;
			}
		}

		if (true == bInsert)
		{
			if (false == this->InsertQuestPRFactionToDB(nPresentQuestID, nPresentFactionID))
			{
				this->UpdateQuestPRFactionToDB(nPresentQuestID, nPresentFactionID);
			}
		}
		else
		{
			this->UpdateQuestPRFactionToDB(nPresentQuestID, nPresentFactionID);
		}
	}


	for each(pair<int,int> itor in this->m_vecQIDANDFIDOfGetQuestPRFactionFromDB)
	{
		int nPastQuestID = itor.first;
		int nPastFactionID = itor.second;

		bool bDelete = true;
		for (int i = 0; dt->Rows->Count > i; i++)
		{
			DataRow^ dr = dt->Rows[i];
			int nPresentQuestID = MStringToInt(dr[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_QID]->ToString());
			int nPresentFactionID = MStringToInt(dr[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_FID]->ToString());

			if (nPastQuestID == nPresentQuestID && nPastFactionID == nPresentFactionID)
			{
				bDelete = false;
				break;
			}
		}

		if (true == bDelete)
		{
			this->DeleteQuestPRFactionToDB(nPastQuestID, nPastFactionID);
		}
		else
		{
			this->UpdateQuestPRFactionToDB(nPastQuestID, nPastFactionID);
		}
	}
}

char g_szDB_INSERT_QUEST_PRFACTION[] = "{CALL spInsertQuestRequiredFaction(%d,%d,%d,%d)}";

bool TDBManager::InsertQuestPRFactionToDB(int nQuestID, int nFactionID)
{
	char szFilter[512];
	sprintf_s(szFilter, 512, "%s=%d AND %s=%d", QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_QID, nQuestID, QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_FID, nFactionID);
	String^ strFilter = gcnew String(szFilter);

	DataTable^ dt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_TABLE_QUESTPRFACTION];
	if (nullptr == dt) return false;

	array<DataRow^>^ arrDr = dt->Select(strFilter);
	if (1 != arrDr->Length) return false;

	DataRow^ dr = arrDr[0];
	int nMinFaction = MStringToInt(dr[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_MINFACTION]->ToString());
	int nMaxFaction = MStringToInt(dr[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_MAXFACTION]->ToString());

	String^ strSQL = String::Format("{{CALL spInsertQuestRequiredFaction({0}, {1}, {2}, {3})}}", nQuestID, nMinFaction, nMaxFaction, nFactionID);

	return this->Execute(strSQL);
}

char g_szDB_UPDATE_QUEST_PRFACTION[] = "{CALL spUpdateQuestRequiredFaction(%d,%d,%d,%d)}";

bool TDBManager::UpdateQuestPRFactionToDB(int nQuestID, int nFactionID)
{
	char szFilter[512];
	sprintf_s(szFilter, 512, "%s=%d AND %s=%d", QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_QID, nQuestID, QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_FID, nFactionID);
	String^ strFilter = gcnew String(szFilter);

	DataTable^ dt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_TABLE_QUESTPRFACTION];
	if (nullptr == dt) return false;

	array<DataRow^>^ arrDr = dt->Select(strFilter);
	if (1 != arrDr->Length) return false;

	DataRow^ dr = arrDr[0];
	int nMinFaction = MStringToInt(dr[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_MINFACTION]->ToString());
	int nMaxFaction = MStringToInt(dr[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_MAXFACTION]->ToString());

	String^ strSQL = String::Format("{{CALL spUpdateQuestRequiredFaction({0}, {1}, {2}, {2})}}", nQuestID, nMinFaction, nMaxFaction, nFactionID);

	return this->Execute(strSQL);
}

char g_szDB_DELETE_QUEST_PRFACTION[] = "{CALL spDeleteQuestRequiredFaction(%d,%d)}";

bool TDBManager::DeleteQuestPRFactionToDB(int nQuestID, int nFactionID)
{
	String^ strSQL = String::Format("{{CALL spDeleteQuestRequiredFaction({0}, {1})}}", nQuestID, nFactionID);

	return this->Execute(strSQL);
}

void TDBManager::SaveStateOfGetQuestsFromDB()
{
	this->SaveStateOfGetQuestFromDB();
	this->SaveStateOfGetQuestObejctiveFromDB();
	this->SaveStateOfGetQuestPRFactionFromDB();
}

void TDBManager::SaveStateOfGetQuestFromDB()
{
	DataTable^ dt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTBASEINFO];
	if(nullptr == dt) return;

	for (int i = 0; dt->Rows->Count > i; i++)
	{
		int nQuestID = MStringToInt(dt->Rows[i][QUEST_DB_TABLE_QUEST_COLUMN_QID]->ToString());
		this->m_vecQIDOfGetQuestFromDB.push_back(nQuestID);
	}
}

void TDBManager::SaveStateOfGetQuestObejctiveFromDB()
{
	DataTable^ dtOObjective = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_OOBJECTIVE];
	if(nullptr == dtOObjective) return;

	for (int i = 0; dtOObjective->Rows->Count > i; i++)
	{
		int nQuestID = MStringToInt(dtOObjective->Rows[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID]->ToString());
		int nObjectiveID = MStringToInt(dtOObjective->Rows[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->ToString());
		this->m_vecQIDANDOIDOfGetOObjectiveFromDB.push_back(make_pair(nQuestID, nObjectiveID));
	}

	DataTable^ dtUObjective = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_UOBJECTIVE];
	if(nullptr == dtUObjective) return;

	for (int i = 0; dtUObjective->Rows->Count > i; i++)
	{
		int nQuestID = MStringToInt(dtUObjective->Rows[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID]->ToString());
		int nObjectiveID = MStringToInt(dtUObjective->Rows[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->ToString());
		this->m_vecQIDANDOIDOfGetUObjectiveFromDB.push_back(make_pair(nQuestID, nObjectiveID));
	}
}

void TDBManager::SaveStateOfGetQuestPRFactionFromDB()
{
	DataTable^ dt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_TABLE_QUESTPRFACTION];
	if(nullptr == dt) return;

	for (int i = 0; dt->Rows->Count > i; i++)
	{
		int nQuestID = MStringToInt(dt->Rows[i][QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_QID]->ToString());
		int nFactionID = MStringToInt(dt->Rows[i][QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_FID]->ToString());
		this->m_vecQIDANDFIDOfGetQuestPRFactionFromDB.push_back(make_pair(nQuestID, nFactionID));
	}
}

void TDBManager::ClearStateOfGetQuests()
{
	this->m_vecQIDANDFIDOfGetQuestPRFactionFromDB.clear();
	this->m_vecQIDANDOIDOfGetOObjectiveFromDB.clear();
	this->m_vecQIDANDOIDOfGetUObjectiveFromDB.clear();
	this->m_vecQIDOfGetQuestFromDB.clear();
}

void TDBManager::DBUOIDtoRealUOID()
{
	DataTable^ dt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_UOBJECTIVE];
	if (nullptr == dt) return;

	for (int i = 0; dt->Rows->Count > i; i++)
	{
		int nObjectiveID = MStringToInt(dt->Rows[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->ToString());
		dt->Rows[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID] = nObjectiveID - OFFSET_UOBJECTIVE;
	}
}

void TDBManager::RealUOIDtoDBUOID()
{
	DataTable^ dt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_UOBJECTIVE];
	if (nullptr == dt) return;

	for (int i = 0; dt->Rows->Count > i; i++)
	{
		int nObjectiveID = MStringToInt(dt->Rows[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->ToString());
		dt->Rows[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID] = nObjectiveID + OFFSET_UOBJECTIVE;
	}
}

String^ TDBManager::ConvertToDBDateTimeFormat(String^ strDate)
{
	String^ strDBDateTime = nullptr;

	if (true == strDate->Contains("오전"))
	{
		array<String^>^ strsForeNoon = {"오전"};
		array<String^>^ strsSplittedResultByNoon = strDate->Split(strsForeNoon, StringSplitOptions::None);

		strDBDateTime = String::Format("{0}{1}", strsSplittedResultByNoon[0],  strsSplittedResultByNoon[1]);
	}
	else if (true == strDate->Contains("오후"))
	{
		array<String^>^ strsAfterNoon = {"오후"};
		array<String^>^ strsSplittedResultByNoon = strDate->Split(strsAfterNoon, StringSplitOptions::None);
		String^ strYearMonthDay = strsSplittedResultByNoon[0];
		String^ strHourMinuteSecond = strsSplittedResultByNoon[1];

		array<String^>^ strsColon = {":"};
		array<String^>^ strsSplittedResultByColon = strHourMinuteSecond->Split(strsColon, StringSplitOptions::None);
		String^ strHour = strsSplittedResultByColon[0];
		String^ strMinute = strsSplittedResultByColon[1];
		String^ strSeconde = strsSplittedResultByColon[2];

		int nHour = MStringToInt(strHour);
		
		strDBDateTime = String::Format("{0} {1}:{2}:{3}", 
			strsSplittedResultByNoon[0],  nHour + 12, strMinute, strSeconde) ;
	}

	_ASSERT(0);

	return strDBDateTime;
}

DataTable^ TDBManager::GetQuestBaseInfo()
{
	DataTable^ dt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTBASEINFO];

	return dt;
}

DataTable^ TDBManager::GetQuestPR()
{
	DataTable^ dt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTPR];

	return dt;
}

DataTable^ TDBManager::GetQuestPRFaction()
{
	DataTable^ dt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_TABLE_QUESTPRFACTION];

	return dt;
}

DataTable^ TDBManager::GetQuestOObjective()
{
	DataTable^ dt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_OOBJECTIVE];

	return dt;
}

DataTable^ TDBManager::GetQuestUObjective()
{
	DataTable^ dt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_UOBJECTIVE];

	return dt;
}


DataTable^ TDBManager::CopyQuestBaseInfo(int nQuestID)
{
	DataTable^ srcDt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTBASEINFO];
	if (nullptr == srcDt) return nullptr;

	DataTable^ dstDt = srcDt->Copy();

	for (int i = 0; dstDt->Rows->Count > i; i++)
	{
		DataRow^ dstDr = dstDt->Rows[i];
		if (nQuestID != MStringToInt(dstDr[QUEST_DB_TABLE_QUEST_COLUMN_QID]->ToString()))
		{
			dstDr->Delete();
		}
	}

	dstDt->AcceptChanges();

	return dstDt;
}

void TDBManager::InsertQuestBaseInfo(int nQuestID)
{
	DataTable^ dt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTBASEINFO];
	if (nullptr == dt) return ;

	DataRow^ drNew = dt->NewRow();
	drNew[QUEST_DB_TABLE_QUEST_COLUMN_QID] = nQuestID;
	drNew[QUEST_DB_TABLE_OBJECTIVE_COLUMN_DESCRIPTION] = "NewDesc";
	drNew[QUEST_DB_TABLE_OBJECTIVE_COLUMN_TITLE] = "NewTitle";

	dt->Rows->Add(drNew);

	dt->AcceptChanges();
}

void TDBManager::UpdateQuestBaseInfo(int nQuestID, String^ strColumnName, Object^ objValue)
{
	DataTable^ dstDt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTBASEINFO];
	if (nullptr == dstDt) _ASSERT(0);

	char szFilter[512];
	sprintf_s(szFilter, 512, "QID=%d", nQuestID);
	String^ strFilter = gcnew String(szFilter);

	array<DataRow^>^ dstArrDr = dstDt->Select(strFilter);

	if (nullptr == dstArrDr) return;

	for (int i = 0; dstArrDr->Length > i; i++) 
	{
		DataRow^ dstDr = dstArrDr[i];
		dstDr[strColumnName] = objValue;
	}

	dstDt->AcceptChanges();
}

void TDBManager::DeleteQuestBaseInfo(int nQuestID)
{
	char szFilter[512];
	sprintf_s(szFilter, 512, "QID=%d", nQuestID);
	String^ strFilter = gcnew String(szFilter);

	DataTable^ dtQuestBaseInfo = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTBASEINFO];
	if (nullptr == dtQuestBaseInfo) _ASSERT(0);

	array<DataRow^>^ arrDrQuestBaseInfo = dtQuestBaseInfo->Select(strFilter);

	for (int i = 0; arrDrQuestBaseInfo->Length > i; i++)
	{
		arrDrQuestBaseInfo[i]->Delete();
	}

	dtQuestBaseInfo->AcceptChanges();
}

DataTable^ TDBManager::CopyQuestPR(int nQuestID)
{
	DataTable^ srcDt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTPR];
	if (nullptr == srcDt) return nullptr;

	DataTable^ dstDt = srcDt->Copy();

	for (int i = 0; dstDt->Rows->Count > i; i++)
	{
		DataRow^ dstDr = dstDt->Rows[i];
		if (nQuestID != MStringToInt(dstDr[QUEST_DB_TABLE_QUEST_COLUMN_QID]->ToString()))
		{
			dstDr->Delete();
		}
	}

	dstDt->AcceptChanges();
	
	return dstDt;
}

void TDBManager::InsertQuestPR(int nQuestID, int nPRQuestID)
{
	DataTable^ dt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTPR];
	if (nullptr == dt) return ;

	DataRow^ drNew = dt->NewRow();
	drNew[QUEST_DB_TABLE_QUEST_COLUMN_QID] = nQuestID;
	drNew[QUEST_DB_TABLE_QUEST_COLUMN_PR_QUEST] = nPRQuestID;

	dt->Rows->Add(drNew);
	dt->AcceptChanges();
}

void TDBManager::UpdateQuestPR(int nQuestID, String^ strColumnName, Object^ objValue)
{
	DataTable^ dstDt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTPR];
	if (nullptr == dstDt) _ASSERT(0);

	char szFilter[512];
	sprintf_s(szFilter, 512, "QID=%d", nQuestID);
	String^ strFilter = gcnew String(szFilter);

	array<DataRow^>^ dstArrDr = dstDt->Select(strFilter);

	if (nullptr == dstArrDr) return;

	for (int i = 0; dstArrDr->Length > i; i++) 
	{
		DataRow^ dstDr = dstArrDr[i];
		dstDr[strColumnName] = objValue;
	}

	dstDt->AcceptChanges();
}

void TDBManager::DeleteQuestPR(int nQuestID)
{
	char szFilter[512];
	sprintf_s(szFilter, 512, "QID=%d", nQuestID);
	String^ strFilter = gcnew String(szFilter);

	DataTable^ dtQuestPR = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTPR];
	if (nullptr == dtQuestPR) _ASSERT(0);

	array<DataRow^>^ arrDrQuestPR = dtQuestPR->Select(strFilter);

	for (int i = 0; arrDrQuestPR->Length > i; i++)
	{
		arrDrQuestPR[i]->Delete();
	}

	dtQuestPR->AcceptChanges();
}

DataTable^ TDBManager::CopyQuestPRFaction(int nQuestID)
{
	DataTable^ srcDt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_TABLE_QUESTPRFACTION];
	if (nullptr == srcDt) return nullptr;

	DataTable^ dstDt = srcDt->Copy();

	for (int i = 0; dstDt->Rows->Count > i; i++)
	{
		DataRow^ dstDr = dstDt->Rows[i];
		if (nQuestID != MStringToInt(dstDr[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_QID]->ToString()))
		{
			dstDr->Delete();
		}
	}

	dstDt->AcceptChanges();

	return dstDt;
}

void TDBManager::InsertQuestPRFaction(int nQRFID, int nQuestID)
{
	DataTable^ dstDt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_TABLE_QUESTPRFACTION];
	if (nullptr == dstDt) _ASSERT(0);

	DataRow^ newDr = dstDt->NewRow();
	newDr[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_QRFID] = nQRFID;
	newDr[QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_QID] = nQuestID;

	dstDt->Rows->Add(newDr);

	dstDt->AcceptChanges();
}

void TDBManager::UpdateQuestPRFaction(int nQRFID, String^ strColumnName, Object^ objValue)
{
	DataTable^ dstDt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_TABLE_QUESTPRFACTION];
	if (nullptr == dstDt) _ASSERT(0);

	char szFilter[512];
	sprintf_s(szFilter, 512, "QRFID=%d", nQRFID);
	String^ strFilter = gcnew String(szFilter);

	array<DataRow^>^ dstArrDr = dstDt->Select(strFilter);

	if (nullptr == dstArrDr) return;

	for (int i = 0; dstArrDr->Length > i; i++) 
	{
		DataRow^ dstDr = dstArrDr[i];
		dstDr[strColumnName] = objValue;
	}

	dstDt->AcceptChanges();
}

void TDBManager::DeleteQuestPRFaction(int nQRFID, int nQuestID)
{
	DataTable^ dtQuestPRFaction = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_TABLE_QUESTPRFACTION];
	if (nullptr == dtQuestPRFaction) _ASSERT(0);

	char szFilter[512];
	sprintf_s(szFilter, 512, "%s=%d AND %s=%d",QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_QRFID, nQRFID, QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_QID, nQuestID);
	String^ strFilter = gcnew String(szFilter);

	array<DataRow^>^ arrDrQuestPRFaction = dtQuestPRFaction->Select(strFilter);

	for (int i = 0; arrDrQuestPRFaction->Length > i; i++)
	{
		arrDrQuestPRFaction[i]->Delete();
	}

	dtQuestPRFaction->AcceptChanges();
}

void TDBManager::DeleteQuestPRFaction(int nQuestID)
{
	DataTable^ dtQuestPRFaction = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_TABLE_QUESTPRFACTION];
	if (nullptr == dtQuestPRFaction) _ASSERT(0);

	char szFilter[512];
	sprintf_s(szFilter, 512, "QID=%d", nQuestID);
	String^ strFilter = gcnew String(szFilter);


	array<DataRow^>^ arrDrQuestPRFaction = dtQuestPRFaction->Select(strFilter);

	for (int i = 0; arrDrQuestPRFaction->Length > i; i++)
	{
		arrDrQuestPRFaction[i]->Delete();
	}

	dtQuestPRFaction->AcceptChanges();
}

DataTable^ TDBManager::CopyQuestOObjective(int nQuestID)
{
	DataTable^ srcDt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_OOBJECTIVE];
	if (nullptr == srcDt) return nullptr;

	DataTable^ dstDt = srcDt->Copy();

	for (int i = 0; dstDt->Rows->Count > i; i++)
	{
		DataRow^ dstDr = dstDt->Rows[i];
		if (nQuestID != MStringToInt(dstDr[QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID]->ToString()))
		{
			dstDr->Delete();
		}
	}

	dstDt->AcceptChanges();

	return dstDt;
}

void TDBManager::InsertQuestOObjective(int nQuestID, int nObjectiveID)
{
	DataTable^ dstDt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_OOBJECTIVE];
	if (nullptr == dstDt) _ASSERT(0);

	DataRow^ newDr = dstDt->NewRow();
	newDr[QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID] = nQuestID;
	newDr[QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID] = nObjectiveID;
	newDr[QUEST_DB_TABLE_OBJECTIVE_COLUMN_TITLE] = "NewTitle";
	newDr[QUEST_DB_TABLE_OBJECTIVE_COLUMN_DESCRIPTION] = "NewDesc";

	dstDt->Rows->Add(newDr);

	dstDt->AcceptChanges();
}

void TDBManager::UpdateQuestOObjective(int nQuestID, int nObjectiveID, String^ strColumnName, Object^ objValue)
{
	DataTable^ dstDt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_OOBJECTIVE];
	if (nullptr == dstDt) _ASSERT(0);

	char szFilter[512];
	sprintf_s(szFilter, 512, "QID=%d AND OID=%d", nQuestID, nObjectiveID);
	String^ strFilter = gcnew String(szFilter);

	array<DataRow^>^ dstArrDr = dstDt->Select(strFilter);

	if (nullptr == dstArrDr) return;

	for (int i = 0; dstArrDr->Length > i; i++) 
	{
		DataRow^ dstDr = dstArrDr[i];
		dstDr[strColumnName] = objValue;
	}

	dstDt->AcceptChanges();
}

void TDBManager::DeleteQuestOObjective(int nQuestID, int nObjectiveID)
{
	char szFilter[512];
	sprintf_s(szFilter, 512, "%s=%d AND %s=%d", QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID, nQuestID, QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID, nObjectiveID);
	String^ strFilter = gcnew String(szFilter);

	DataTable^ dt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_OOBJECTIVE];
	if (nullptr == dt) _ASSERT(0);

	array<DataRow^>^ arrDr = dt->Select(strFilter);

	for (int i = 0; arrDr->Length > i; i++)
	{
		arrDr[i]->Delete();
	}

	dt->AcceptChanges();
}

void TDBManager::DeleteQuestOObjective(int nQuestID)
{
	char szFilter[512];
	sprintf_s(szFilter, 512, "QID=%d", nQuestID);
	String^ strFilter = gcnew String(szFilter);

	DataTable^ dtQuestOObjective = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_OOBJECTIVE];
	if (nullptr == dtQuestOObjective) _ASSERT(0);

	array<DataRow^>^ arrDrQuestOObjective = dtQuestOObjective->Select(strFilter);

	for (int i = 0; arrDrQuestOObjective->Length > i; i++)
	{
		arrDrQuestOObjective[i]->Delete();
	}

	dtQuestOObjective->AcceptChanges();
}

DataTable^ TDBManager::CopyQuestUObjective(int nQuestID)
{
	DataTable^ srcDt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_UOBJECTIVE];
	if (nullptr == srcDt) return nullptr;

	DataTable^ dstDt = srcDt->Copy();

	for (int i = 0; dstDt->Rows->Count > i; i++)
	{
		DataRow^ dstDr = dstDt->Rows[i];

		if (nQuestID != MStringToInt(dstDr[QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID]->ToString()))
		{
			dstDt->Rows[i]->Delete();
		}
	}

	dstDt->AcceptChanges();

	return dstDt;
}

void TDBManager::InsertQuestUObjective(int nQuestID, int nObjectiveID)
{
	DataTable^ dstDt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_UOBJECTIVE];
	if (nullptr == dstDt) _ASSERT(0);

	DataRow^ newDr = dstDt->NewRow();
	newDr[QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID] = nQuestID;
	newDr[QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID] = nObjectiveID;
	newDr[QUEST_DB_TABLE_OBJECTIVE_COLUMN_TITLE] = "NewTitle";
	newDr[QUEST_DB_TABLE_OBJECTIVE_COLUMN_DESCRIPTION] = "NewDesc";

	dstDt->Rows->Add(newDr);

	dstDt->AcceptChanges();
}

void TDBManager::UpdateQuestUObjective(int nQuestID, int nObjectiveID, String^ strColumnName, Object^ objValue)
{
	DataTable^ dstDt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_UOBJECTIVE];
	if (nullptr == dstDt) _ASSERT(0);

	char szFilter[512];
	sprintf_s(szFilter, 512, "QID=%d AND OID=%d", nQuestID, nObjectiveID);
	String^ strFilter = gcnew String(szFilter);

	array<DataRow^>^ dstArrDr = dstDt->Select(strFilter);

	if (nullptr == dstArrDr) return;

	for (int i = 0; dstArrDr->Length > i; i++) 
	{
		DataRow^ dstDr = dstArrDr[i];
		dstDr[strColumnName] = objValue;
	}

	dstDt->AcceptChanges();
}

void TDBManager::DeleteQuestUObjective(int nQuestID, int nObjectiveID)
{
	char szFilter[512];
	sprintf_s(szFilter, 512, "%s=%d AND %s=%d", QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID, nQuestID, QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID, nObjectiveID);
	String^ strFilter = gcnew String(szFilter);

	DataTable^ dtQuestUObjective = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_UOBJECTIVE];
	if (nullptr == dtQuestUObjective) _ASSERT(0);

	array<DataRow^>^ arrDrQuestUObjective = dtQuestUObjective->Select(strFilter);

	for (int i = 0; arrDrQuestUObjective->Length > i; i++)
	{
		arrDrQuestUObjective[i]->Delete();
	}

	dtQuestUObjective->AcceptChanges();
}

void TDBManager::DeleteQuestUObjective(int nQuestID)
{
	char szFilter[512];
	sprintf_s(szFilter, 512, "%s=%d", QUEST_DB_TABLE_OBJECTIVE_COLUMN_QID, nQuestID);
	String^ strFilter = gcnew String(szFilter);

	DataTable^ dtQuestUObjective = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_UOBJECTIVE];
	if (nullptr == dtQuestUObjective) _ASSERT(0);

	array<DataRow^>^ arrDrQuestUObjective = dtQuestUObjective->Select(strFilter);

	for (int i = 0; arrDrQuestUObjective->Length > i; i++)
	{
		arrDrQuestUObjective[i]->Delete();
	}

	dtQuestUObjective->AcceptChanges();
}

void TDBManager::InsertQuests(int nQuestID, int nPRQuestID)
{
	this->InsertQuestBaseInfo(nQuestID);
	this->InsertQuestPR(nQuestID, nPRQuestID);
}

void TDBManager::DeleteQuests(int nQuestID)
{
	this->DeleteQuestBaseInfo(nQuestID);
	this->DeleteQuestPR(nQuestID);
	this->DeleteQuestPRFaction(nQuestID);
	this->DeleteQuestOObjective(nQuestID);
	this->DeleteQuestUObjective(nQuestID);

	char szFilter[512];
	sprintf_s(szFilter, 512, "%s=%d", QUEST_DB_TABLE_QUEST_COLUMN_PR_QUEST, nQuestID);
	String^ strFilter = gcnew String(szFilter);

	DataTable^ dtQuestPR = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTPR];
	if (nullptr == dtQuestPR) _ASSERT(0);

	array<DataRow^>^ arrDrQuestPR = dtQuestPR->Select(strFilter);

	for (int i = 0; arrDrQuestPR->Length > i; i++)
	{
		int nDeletedQuestID = MStringToInt(arrDrQuestPR[i][QUEST_DB_TABLE_QUEST_COLUMN_QID]->ToString());
		this->DeleteQuests(nDeletedQuestID);
	}
}


String^ TDBManager::GetStringsKeyFromQuestBaseInfo(int nQuestID, String^ strColumn)
{
	if (nullptr == strColumn) _ASSERT(0);

	DataTable^ srcDt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTBASEINFO];
	if (nullptr == srcDt) _ASSERT(0);

	String^ strFilter = String::Format("QID={0}", nQuestID);

	array<DataRow^>^ arrDr = srcDt->Select(strFilter);
	if (1 != arrDr->Length) _ASSERT(0);

	String^ strKey = arrDr[0][strColumn]->ToString();

	return strKey;
}

String^ TDBManager::GetStringsKeyFromQuestOObjective(int nQuestID, int nObjectiveID, String^ strColumn)
{
	if (nullptr == strColumn) _ASSERT(0);

	DataTable^ srcDt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_OOBJECTIVE];
	if (nullptr == srcDt) _ASSERT(0);

	String^ strFilter = String::Format("QID={0} AND OID={1}", nQuestID, nObjectiveID);

	array<DataRow^>^ arrDr = srcDt->Select(strFilter);
	if (1 != arrDr->Length) _ASSERT(0);

	String^ strKey = arrDr[0][strColumn]->ToString();

	return strKey;
}

String^ TDBManager::GetStringsKeyFromQuestUObjective(int nQuestID, int nObjectiveID, String^ strColumn)
{
	if (nullptr == strColumn) _ASSERT(0);

	DataTable^ srcDt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_UOBJECTIVE];
	if (nullptr == srcDt) _ASSERT(0);

	String^ strFilter = String::Format("QID={0} AND OID={1}", nQuestID, nObjectiveID);

	array<DataRow^>^ arrDr = srcDt->Select(strFilter);
	if (1 != arrDr->Length) _ASSERT(0);

	String^ strKey = arrDr[0][strColumn]->ToString();

	return strKey;
}

bool TDBManager::HaveQuest(int nQuestID)
{
	DataTable^ dt = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_QUESTBASEINFO];
	if (nullptr == dt) return false;

	char szFilter[512];
	sprintf_s(szFilter, 512, "%s=%d", QUEST_DB_TABLE_QUEST_COLUMN_QID, nQuestID);
	String^ strFilter = gcnew String(szFilter);

	array<DataRow^>^ arrDr = dt->Select(strFilter);

	if (0 < arrDr->Length) return true;

	return false;
}

bool TDBManager::HaveQuestObjective(int nQuestID, int nObjectiveID)
{
	DataTable^ dtOobjective = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_OOBJECTIVE];
	DataTable^ dtUobjective = TDBManagerGlobal::m_dsQuest->Tables[QUEST_DB_VTABLE_UOBJECTIVE];
	
	char szFilter[512];
	sprintf_s(szFilter, 512, "%s=%d AND %s=%d", QUEST_DB_TABLE_QUEST_COLUMN_QID, nQuestID, QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID, nObjectiveID);
	String^ strFilter = gcnew String(szFilter);

	array<DataRow^>^ arrDrOobjective = dtOobjective->Select(strFilter);
	if (0 < arrDrOobjective->Length) return true;

	array<DataRow^>^ arrDrUobjective = dtUobjective->Select(strFilter);
	if (0 < arrDrUobjective->Length) return true;

	return false;
}