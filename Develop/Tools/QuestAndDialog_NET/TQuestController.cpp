#include "StdAfx.h"
#include "TQuestController.h"
#include "TQuestConfig.h"
#include "TDBManager.h"
#include "TStringResMgr.h"

TQuestController::TQuestController(TDBManager* pDBm, TStringResMgr* pSrm)
{
	this->m_pDBm = pDBm;
	this->m_pSrm = pSrm;

	if (false == this->CreateInstance()) _ASSERT(0);
}

TQuestController::~TQuestController()
{
	this->DeCreateInstance();

	if (nullptr != this->m_pDBm)
		delete this->m_pDBm;
}

bool TQuestController::Begin(TDialogController* pDc)
{
	this->m_nSelectedQuestID = -1;
	this->m_nCuttedQuestID = -1;
	this->m_pDc = pDc;

	if (false == this->m_pDBm->Begin()) return false;
	if (false == this->m_pSrm->BeginQuestTool(FILENAME_STRINGS_QUESTTOOL)) return false;
	if (false == TQuestControllerGlobal::m_QuestForm->Begin()) return false;

	return true;
}

void TQuestController::End()
{
	if (nullptr != TQuestControllerGlobal::m_QuestForm)
	{
		TQuestControllerGlobal::m_QuestForm->End();
	}

	this->m_pSrm->EndQuestTool();
	this->m_pDBm->End();
	this->m_pDc = nullptr;

	this->m_setModifiedQuestID.clear();
	this->m_setHidedQuestID.clear();
}

bool TQuestController::CreateInstance()
{
	TQuestControllerGlobal::m_QuestForm = gcnew QuestForm(this);
	if (nullptr == TQuestControllerGlobal::m_QuestForm) return false;

	return true;
}

void TQuestController::DeCreateInstance()
{
	if (nullptr != TQuestControllerGlobal::m_QuestForm)
	{
		TQuestControllerGlobal::m_QuestForm->Close();
		TQuestControllerGlobal::m_QuestForm = nullptr;
	}
}

String^ TQuestController::MakeQuestTitleKey(int nQuestID)
{
	String^ strKey = String::Format("Q{0}d_T", nQuestID);

	return strKey;
}

String^ TQuestController::MakeQuestDescKey(int nQuestID)
{
	String^ strKey = String::Format("Q{0}d_D", nQuestID);

	return strKey;
}

String^ TQuestController::MakeOObjectiveTitleKey(int nQuestID, int nObjectiveID)
{
	String^ strKey = String::Format("Q{0}d_OO{1}d_T", nQuestID, nObjectiveID);

	return strKey;
}

String^ TQuestController::MakeOObjectiveDescKey(int nQuestID, int nObjectiveID)
{
	String^ strKey = String::Format("Q{0}d_OO{1}d_D", nQuestID, nObjectiveID);
	
	return strKey;
}

String^ TQuestController::MakeUObjectiveTitleKey(int nQuestID, int nObjectiveID)
{
	String^ strKey = String::Format("Q{0}d_UO{1}d_T", nQuestID, nObjectiveID);

	return strKey;
}

String^ TQuestController::MakeUObjectiveDescKey(int nQuestID, int nObjectiveID)
{
	String^ strKey = String::Format("Q{0}d_UO{1}d_D", nQuestID, nObjectiveID);

	return strKey;
}

void TQuestController::SaveQuestsToXML()
{
	MXml xQuest;
	MXml xQuestStrings;

	MXmlDeclaration* pXdQuest = xQuest.CreateDeclaration("1.0", "UTF-8", "");
	xQuest.Doc()->LinkEndChild(pXdQuest);
	MXmlElement* pXeQuest_MAIET = new MXmlElement(QUEST_XML_TAG_MAIET);
	xQuest.Doc()->LinkEndChild(pXeQuest_MAIET);

	MXmlDeclaration* pXdQuestStrings = xQuestStrings.CreateDeclaration("1.0", "euc-kr", "");
	xQuestStrings.Doc()->LinkEndChild(pXdQuestStrings);
	MXmlElement* pXeQuestStrings_MAIET = new MXmlElement(STRINGS_XML_TAG_MAIET);
	xQuestStrings.Doc()->LinkEndChild(pXeQuestStrings_MAIET);

	DataTable^ dtQuestBaseInfo = this->m_pDBm->GetQuestBaseInfo();

	for (int i = 0; dtQuestBaseInfo->Rows->Count > i; i++)
	{
		int nQuestID = MStringToInt(dtQuestBaseInfo->Rows[i][QUEST_DB_TABLE_QUEST_COLUMN_QID]->ToString());
		String^ strFilter = String::Format("{0}={1}", QUEST_DB_TABLE_QUEST_COLUMN_QID, nQuestID);

		this->SaveQuestToXML(pXeQuest_MAIET, pXeQuestStrings_MAIET, strFilter, nQuestID);
	}


	String^ strPath = String::Format("{0}{1}{2}", PATH_GAMECLIENT, PATH_SYSTEM, FILENAME_QUEST_INFO);
	xQuest.Doc()->SaveFile(MStringToCharPointer(strPath));

	strPath = String::Format("{0}{1}{2}", PATH_GAMECLIENT, PATH_STRINGS, FILENAME_STRINGS_QUEST);
	xQuestStrings.Doc()->SaveFile(MStringToCharPointer(strPath));

	strPath = String::Format("{0}{1}{2}", PATH_GAMESERVER, PATH_SYSTEM, FILENAME_QUEST_INFO);
	xQuest.Doc()->SaveFile(MStringToCharPointer(strPath));

	strPath = String::Format("{0}{1}{2}", PATH_GAMESERVER, PATH_STRINGS, FILENAME_STRINGS_QUEST);
	xQuestStrings.Doc()->SaveFile(MStringToCharPointer(strPath));
}

void TQuestController::SaveQuestToXML(MXmlElement* pXeQuest_MAIET, MXmlElement* pXeQuestStrings_MAIET, String^ strFilter, int nQuestID)
{
	DataTable^ dtQuestBaseInfo = this->m_pDBm->GetQuestBaseInfo();
	array<DataRow^>^ arrDrQuestBastInfo = dtQuestBaseInfo->Select(strFilter);
	DataRow^ drQuestBaseInfo = arrDrQuestBastInfo[0];

	MXmlElement* pXeQuest_QUEST = new MXmlElement(QUEST_XML_TAG_QUEST);
	pXeQuest_MAIET->LinkEndChild(pXeQuest_QUEST);

	pXeQuest_QUEST->SetAttribute(QUEST_XML_ATTR_ID, nQuestID);

	String^ strQuestTitleKey = this->MakeQuestTitleKey(nQuestID);
	String^ strQuestTitleValue = drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_TITLE]->ToString();
	pXeQuest_QUEST->SetAttribute(QUEST_XML_ATTR_TITLE, MStringToCharPointer(strQuestTitleKey));
	this->SaveQuestStringsToXML(pXeQuestStrings_MAIET, strQuestTitleKey, strQuestTitleValue);


	bool bRaidable = MStringToBool(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_RAIDABLE]->ToString());
	string strRaidable = (true == bRaidable) ? QUEST_XML_VALUE_TYPE_TRUE : QUEST_XML_VALUE_TYPE_FALSE;
	pXeQuest_QUEST->SetAttribute(QUEST_XML_ATTR_RAIDABLE, strRaidable);

	int nQA = MStringToInt(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_QA]->ToString());
	pXeQuest_QUEST->SetAttribute(QUEST_XML_ATTR_QA, nQA);

	bool bRepeatable = MStringToBool(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_REPEATABLE]->ToString());
	string strRepeatable = (true == bRepeatable) ? QUEST_XML_VALUE_TYPE_TRUE : QUEST_XML_VALUE_TYPE_FALSE;
	pXeQuest_QUEST->SetAttribute(QUEST_XML_ATTR_REPEATABLE, strRepeatable);

	QUEST_TYPE nType = static_cast<QUEST_TYPE>(MStringToInt(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_TYPE]->ToString()));
	string strType;
	switch (nType)
	{
	case QT_REGULAR:
		{
			strType = QUEST_XML_VALUE_TYPE_REGULAR;
		}
		break;
	case QT_FACTION:
		{
			strType = QUEST_XML_VALUE_TYPE_FACTION;
		}
		break;
	case QT_GUILD:
		{
			strType = QUEST_XML_VALUE_TYPE_GUILD;
		}
		break;
	case QT_MAIN:
		{
			strType = QUEST_XML_VALUE_TYPE_MAIN;
		}
		break;
	case QT_EPIC:
		{
			strType = QUEST_XML_VALUE_TYPE_EPIC;
		}
		break;
	}
	pXeQuest_QUEST->SetAttribute(QUEST_XML_ATTR_TYPE, strType);

	bool bHideNextObjInfo = MStringToBool(drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_HIDETHENEXTSTEP]->ToString());
	string strHideNextObjInfo = (true == bHideNextObjInfo) ? QUEST_XML_VALUE_TYPE_TRUE : QUEST_XML_VALUE_TYPE_FALSE;
	pXeQuest_QUEST->SetAttribute(QUEST_XML_ATTR_HIDE_NEXT_OBJ_INFO, strHideNextObjInfo);


	MXmlElement* pXeQuest_DESC = new MXmlElement(QUEST_XML_TAG_DESC);
	pXeQuest_QUEST->LinkEndChild(pXeQuest_DESC);
	String^ strQuestDescKey = this->MakeQuestDescKey(nQuestID);
	String^ strQuestDescValue = drQuestBaseInfo[QUEST_DB_TABLE_QUEST_COLUMN_DESCRIPTION]->ToString();
	_SetContents(pXeQuest_DESC, MStringToCharPointer(strQuestDescKey));
	this->SaveQuestStringsToXML(pXeQuestStrings_MAIET, strQuestDescKey, strQuestDescValue);

	this->SavePrerequisitesToXML(pXeQuest_QUEST, strFilter);
	this->SaveOObjectiveToXML(pXeQuest_QUEST, pXeQuestStrings_MAIET, strFilter, nQuestID);
	this->SaveUObjectiveToXML(pXeQuest_QUEST, pXeQuestStrings_MAIET, strFilter, nQuestID);
}

void TQuestController::SaveQuestStringsToXML(MXmlElement* pXeQuestStrings_MAIET, String^ strKey, String^ strValue)
{
	MXmlElement* pXeQuestStrings_STR = new MXmlElement(STRINGS_XML_TAG_STR);
	pXeQuestStrings_MAIET->LinkEndChild(pXeQuestStrings_STR);
	pXeQuestStrings_STR->SetAttribute(STRINGS_XML_ATTR_KEY, MStringToCharPointer(strKey));
	pXeQuestStrings_STR->SetAttribute(STRINGS_XML_ATTR_STRING, MStringToCharPointer(strValue));
}

void TQuestController::SavePrerequisitesToXML(MXmlElement* pXeQuest_QUEST, String^ strFilter)
{
	MXmlElement* pXeQuest_PR = new MXmlElement(QUEST_XML_TAG_PREREQUISITES);
	pXeQuest_QUEST->LinkEndChild(pXeQuest_PR);

	DataTable^ dtQuestPR = this->m_pDBm->GetQuestPR();
	array<DataRow^>^ arrDrQuestPR = dtQuestPR->Select(strFilter);
	DataRow^ drQuestPR = arrDrQuestPR[0];

	int nMinLevel = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_MINLEVEL]->ToString());
	pXeQuest_PR->SetAttribute(QUEST_XML_ATTR_MINLEVEL, nMinLevel);

	int nMaxLevel = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_MAXLEVEL]->ToString());
	pXeQuest_PR->SetAttribute(QUEST_XML_ATTR_MAXLEVEL, nMaxLevel);

	int nPRQuestID = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_QUEST]->ToString());
	pXeQuest_PR->SetAttribute(QUEST_XML_ATTR_QUEST, nPRQuestID);

	int nPRUncompletedQuestID = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_UNCOMPLETEDQUEST]->ToString());
	pXeQuest_PR->SetAttribute(QUEST_XML_ATTR_UNCOMPLETEDQUEST, nPRUncompletedQuestID);

	int nPRItemID = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_ITEMID]->ToString());
	pXeQuest_PR->SetAttribute(QUEST_XML_ATTR_ITEM, nPRItemID);

	int nEquipItemID = MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_EQUIPITEM]->ToString());
	pXeQuest_PR->SetAttribute(QUEST_XML_ATTR_EQUIP, nEquipItemID);

	RACE nPRRace = static_cast<RACE>(MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_RACE]->ToString()));
	string strPRRace;
	switch (nPRRace)
	{
	case RACE_NA :
		{
			strPRRace = QUEST_XML_VALUE_TYPE_NA;
		}
		break;
	case RACE_HUMAN :
		{
			strPRRace = QUEST_XML_VALUE_TYPE_HUMAN;
		}
		break;
	case RACE_VOLO :
		{
			strPRRace = QUEST_XML_VALUE_TYPE_VOLO;
		}
		break;
	case RACE_NEBIT :
		{
			strPRRace = QUEST_XML_VALUE_TYPE_NEBIT;
		}
		break;
	}
	pXeQuest_PR->SetAttribute(QUEST_XML_ATTR_RACE, strPRRace);

	SEX nPRSex = static_cast<SEX>(MStringToInt(drQuestPR[QUEST_DB_TABLE_QUEST_COLUMN_PR_SEX]->ToString()));
	string strPRSex;
	switch (nPRSex)
	{
	case SEX_NA :
		{
			strPRSex = QUEST_XML_VALUE_TYPE_NA;
		}
		break;
	case SEX_MALE :
		{
			strPRSex = QUEST_XML_VALUE_TYPE_MALE;
		}
		break;
	case SEX_FEMALE :
		{
			strPRSex = QUEST_XML_VALUE_TYPE_FEMALE;
		}
		break;
	}
	pXeQuest_PR->SetAttribute(QUEST_XML_ATTR_SEX, strPRSex);

	this->SaveFactionToXML(pXeQuest_PR, strFilter);
}

void TQuestController::SaveFactionToXML(MXmlElement* pXeQuest_PR, String^ strFilter)
{
	DataTable^ dtQuestPRFaction = this->m_pDBm->GetQuestPRFaction();
	array<DataRow^>^ arrDrQuestPRFaction = dtQuestPRFaction->Select(strFilter);

	for (int i = 0; arrDrQuestPRFaction->Length > i; i++)
	{
		MXmlElement* pXeQuest_FACTION = new MXmlElement(QUEST_XML_TAG_FACTION);
		pXeQuest_PR->LinkEndChild(pXeQuest_FACTION);

		int nFactionID = MStringToInt(arrDrQuestPRFaction[i][QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_FID]->ToString());
		pXeQuest_FACTION->SetAttribute(QUEST_XML_ATTR_ID, nFactionID);

		int nMinFaction = MStringToInt(arrDrQuestPRFaction[i][QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_MINFACTION]->ToString());
		pXeQuest_FACTION->SetAttribute(QUEST_XML_ATTR_MIN, nMinFaction);

		int nMaxFaction = MStringToInt(arrDrQuestPRFaction[i][QUEST_DB_TABLE_QUESTPRFACTION_COLUMN_MAXFACTION]->ToString());
		pXeQuest_FACTION->SetAttribute(QUEST_XML_ATTR_MAX, nMaxFaction);
	}
}

void TQuestController::SaveOObjectiveToXML(MXmlElement* pXeQuest_QUEST, MXmlElement* pXeQuestStrings_MAIET, String^ strFilter, int nQuestID)
{
	MXmlElement* pXeQuest_ORDERED = new MXmlElement(QUEST_XML_TAG_ORDERED);
	pXeQuest_QUEST->LinkEndChild(pXeQuest_ORDERED);

	DataTable^ dtQuestOObjective = this->m_pDBm->GetQuestOObjective();
	array<DataRow^>^ arrDrQuestOObjective = dtQuestOObjective->Select(strFilter);

	for (int i = 0; arrDrQuestOObjective->Length > i; i++)
	{
		MXmlElement* pXeQuest_OBJECTIVE = new MXmlElement(QUEST_XML_TAG_OBJECTIVE);
		pXeQuest_ORDERED->LinkEndChild(pXeQuest_OBJECTIVE);

		int nObjectiveID = MStringToInt(arrDrQuestOObjective[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->ToString());
		pXeQuest_OBJECTIVE->SetAttribute(QUEST_XML_ATTR_ID, nObjectiveID);

		String^ strObjectiveTitleKey = this->MakeOObjectiveTitleKey(nQuestID, nObjectiveID);
		String^ strObjectiveTitleValue = arrDrQuestOObjective[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_TITLE]->ToString();
		pXeQuest_OBJECTIVE->SetAttribute(QUEST_XML_ATTR_TITLE, MStringToCharPointer(strObjectiveTitleKey));
		this->SaveQuestStringsToXML(pXeQuestStrings_MAIET, strObjectiveTitleKey, strObjectiveTitleValue);

		QUEST_OBJECTIVE_TYPE nQOT = static_cast<QUEST_OBJECTIVE_TYPE>(MStringToInt(arrDrQuestOObjective[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_TYPE]->ToString()));
		string strType;
		switch (nQOT)
		{
		case QOT_DESTROY:
			{
				strType = QUEST_XML_VALUE_TYPE_DESTROY;
			}
			break;
		case QOT_COLLECT:
			{
				strType = QUEST_XML_VALUE_TYPE_COLLECT;
			}
			break;
		case QOT_ACT:
			{
				strType = QUEST_XML_VALUE_TYPE_ACT;
			}
			break;
		case QOT_BIT_OP:
			{
				strType = QUEST_XML_VALUE_TYPE_BIT_OP;
			}
			break;
		}
		pXeQuest_OBJECTIVE->SetAttribute(QUEST_XML_ATTR_TYPE, strType);

		int nParam1 = MStringToInt(arrDrQuestOObjective[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_PARAM1]->ToString());
		pXeQuest_OBJECTIVE->SetAttribute(QUEST_XML_ATTR_PARAM1, nParam1);

		int nParam2 = MStringToInt(arrDrQuestOObjective[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_PARAM2]->ToString());
		pXeQuest_OBJECTIVE->SetAttribute(QUEST_XML_ATTR_PARAM2, nParam2);

		MXmlElement* pXeQuest_DESC = new MXmlElement(QUEST_XML_TAG_DESC);
		pXeQuest_OBJECTIVE->LinkEndChild(pXeQuest_DESC);

		String^ strObjectiveDescKey = this->MakeOObjectiveDescKey(nQuestID, nObjectiveID);
		String^ strObjectiveDescValue = arrDrQuestOObjective[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_DESCRIPTION]->ToString();
		_SetContents(pXeQuest_DESC, MStringToCharPointer(strObjectiveDescKey));
		this->SaveQuestStringsToXML(pXeQuestStrings_MAIET, strObjectiveDescKey, strObjectiveDescValue);
	}
}

void TQuestController::SaveUObjectiveToXML(MXmlElement* pXeQuest_QUEST, MXmlElement* pXeQuestStrings_MAIET, String^ strFilter, int nQuestID)
{
	MXmlElement* pXeQuest_UNORDERED = new MXmlElement(QUEST_XML_TAG_UNORDERED);
	pXeQuest_QUEST->LinkEndChild(pXeQuest_UNORDERED);

	DataTable^ dtQuestUObjective = this->m_pDBm->GetQuestUObjective();
	array<DataRow^>^ arrDrQuestUObjective = dtQuestUObjective->Select(strFilter);

	for (int i = 0; arrDrQuestUObjective->Length > i; i++)
	{
		MXmlElement* pXeQuest_OBJECTIVE = new MXmlElement(QUEST_XML_TAG_OBJECTIVE);
		pXeQuest_UNORDERED->LinkEndChild(pXeQuest_OBJECTIVE);

		int nObjectiveID = MStringToInt(arrDrQuestUObjective[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_OID]->ToString());
		pXeQuest_OBJECTIVE->SetAttribute(QUEST_XML_ATTR_ID, nObjectiveID);

		String^ strObjectiveTitleKey = this->MakeUObjectiveTitleKey(nQuestID, nObjectiveID);
		String^ strObjectiveTitleValue = arrDrQuestUObjective[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_TITLE]->ToString();
		pXeQuest_OBJECTIVE->SetAttribute(QUEST_XML_ATTR_TITLE, MStringToCharPointer(strObjectiveTitleKey));
		this->SaveQuestStringsToXML(pXeQuestStrings_MAIET, strObjectiveTitleKey, strObjectiveTitleValue);

		QUEST_OBJECTIVE_TYPE nQOT = static_cast<QUEST_OBJECTIVE_TYPE>(MStringToInt(arrDrQuestUObjective[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_TYPE]->ToString()));
		string strType;
		switch (nQOT)
		{
		case QOT_DESTROY:
			{
				strType = QUEST_XML_VALUE_TYPE_DESTROY;
			}
			break;
		case QOT_COLLECT:
			{
				strType = QUEST_XML_VALUE_TYPE_COLLECT;
			}
			break;
		case QOT_ACT:
			{
				strType = QUEST_XML_VALUE_TYPE_ACT;
			}
			break;
		case QOT_BIT_OP:
			{
				strType = QUEST_XML_VALUE_TYPE_BIT_OP;
			}
			break;
		}
		pXeQuest_OBJECTIVE->SetAttribute(QUEST_XML_ATTR_TYPE, strType);

		int nParam1 = MStringToInt(arrDrQuestUObjective[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_PARAM1]->ToString());
		pXeQuest_OBJECTIVE->SetAttribute(QUEST_XML_ATTR_PARAM1, nParam1);

		int nParam2 = MStringToInt(arrDrQuestUObjective[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_PARAM2]->ToString());
		pXeQuest_OBJECTIVE->SetAttribute(QUEST_XML_ATTR_PARAM2, nParam2);

		MXmlElement* pXeQuest_DESC = new MXmlElement(QUEST_XML_TAG_DESC);
		pXeQuest_OBJECTIVE->LinkEndChild(pXeQuest_DESC);

		String^ strObjectiveDescKey = this->MakeUObjectiveDescKey(nQuestID, nObjectiveID);
		String^ strObjectiveDescValue = arrDrQuestUObjective[i][QUEST_DB_TABLE_OBJECTIVE_COLUMN_DESCRIPTION]->ToString();
		_SetContents(pXeQuest_DESC, MStringToCharPointer(strObjectiveDescKey));
		this->SaveQuestStringsToXML(pXeQuestStrings_MAIET, strObjectiveDescKey, strObjectiveDescValue);
	}
}

void TQuestController::ShowQuestTool()
{
	TQuestControllerGlobal::m_QuestForm->Show();
	TQuestControllerGlobal::m_QuestForm->Focus();
}

void TQuestController::ReloadQuest()
{
	TDialogController* pDc = this->m_pDc;
	this->End();
	this->Begin(this->m_pDc);
}

void TQuestController::SerializeTreeView_QuestList()
{
	DataTable^ dtPR = this->m_pDBm->GetQuestPR();
	if (nullptr ==  dtPR) return;
	DataTable^ dtBaseInfo = this->m_pDBm->GetQuestBaseInfo();
	if (nullptr ==  dtBaseInfo) return;

	this->m_nSelectedQuestID = -1;

	TQuestControllerGlobal::m_QuestForm->SerializeTreeView_QuestList(dtPR, dtBaseInfo);
}

void TQuestController::SerializeDataGridView()
{	
	this->SerializeDataGridView_QuestBaseInfo();
	this->SerializeDataGridView_QuestPR();
	this->SerializeDataGridView_QuestPRFaction();
	this->SerializeDataGridView_QuestOObjective();
	this->SerializeDataGridView_QuestUObjective();
}

void TQuestController::SerializeDataGridView_QuestBaseInfo()
{
	DataTable^ dt = this->m_pDBm->CopyQuestBaseInfo(this->m_nSelectedQuestID);
	if (nullptr == dt) return;
	TQuestControllerGlobal::m_QuestForm->SerializeDataGridView_QuestBaseInfo(dt);
}

void TQuestController::SerializeDataGridView_QuestPR()
{
	DataTable^ dt = this->m_pDBm->CopyQuestPR(this->m_nSelectedQuestID);
	if (nullptr == dt) return;
	TQuestControllerGlobal::m_QuestForm->SerializeDataGridView_QuestPR(dt);
}

void TQuestController::SerializeDataGridView_QuestPRFaction()
{
	DataTable^ dt = this->m_pDBm->CopyQuestPRFaction(this->m_nSelectedQuestID);
	if (nullptr == dt) return;
	TQuestControllerGlobal::m_QuestForm->SerializeDataGridView_QuestPRFaction(dt);
}

void TQuestController::SerializeDataGridView_QuestOObjective()
{
	DataTable^ dt = this->m_pDBm->CopyQuestOObjective(this->m_nSelectedQuestID);
	if (nullptr == dt) return;
	TQuestControllerGlobal::m_QuestForm->SerializeDataGridView_QuestOObjective(dt);
}

void TQuestController::SerializeDataGridView_QuestUObjective()
{
	DataTable^ dt = this->m_pDBm->CopyQuestUObjective(this->m_nSelectedQuestID);
	if (nullptr == dt) return;
	TQuestControllerGlobal::m_QuestForm->SerializeDataGridView_QuestUObjective(dt);
}

TreeNode^ TQuestController::SearchTreeNode(TreeNodeCollection^ inNodes, String^ str)
{
	TreeNode^ tn = nullptr;
	System::Collections::IEnumerator^ myNodes = (safe_cast<System::Collections::IEnumerable^>(inNodes))->GetEnumerator();
	while ( myNodes->MoveNext() )
	{
		tn = safe_cast<TreeNode^>(myNodes->Current);
		System::String^ tnstr = (safe_cast<TreeNode^>(myNodes->Current))->Text;
		if( str->StartsWith(tnstr))
		{
			return tn;
		}
		else
		{
			tn = SearchTreeNode( tn->Nodes, str);
			if (nullptr != tn)
				return tn;
		}
	}

	return nullptr;
}

void TQuestController::CutAndPasteTreeNode(TreeNode^ tnDestination, TreeNode^ tnSource)
{
	System::String^ strText = gcnew System::String(tnSource->Text);
	tnSource->Text = "";
	TreeNode^ tnChild = tnDestination->Nodes->Add(strText);

	System::Collections::IEnumerator^ etor = tnSource->Nodes->GetEnumerator();
	while(etor->MoveNext())
	{
		TreeNode^ tnCurrent = safe_cast<TreeNode^>(etor->Current);
		CutAndPasteTreeNode(tnChild, tnCurrent);
	}
}

System::String^ TQuestController::MakeTreeView_QuestListText(int nQuestID)
{
	DataTable^ dt = this->m_pDBm->GetQuestBaseInfo();
	if (nullptr == dt) _ASSERT(0);

	String^ strFilter = String::Format("QID={0}", nQuestID);
	array<DataRow^>^ arrDr = dt->Select(strFilter);
	if (nullptr == arrDr) _ASSERT(0);
	if (1 != arrDr->Length) return nullptr;

	DataRow^ dr = arrDr[0];
	String^ strTilte = dr[QUEST_DB_TABLE_QUEST_COLUMN_TITLE]->ToString();
	String^ strText = TQuestControllerGlobal::m_QuestForm->MakeTreeView_QuestListText(strTilte, nQuestID);

	return strText;
}

void TQuestController::RunDialogTool()
{
	if (nullptr == this->m_pDc->GetDialogForm())
	{
		this->m_pDc->End();
		this->m_pDc->CreateInstance();
		this->m_pDc->Begin(this);
	}

	this->m_pDc->ShowDialogTool();
}

Object^ TQuestController::GetQuestForm()
{
	return TQuestControllerGlobal::m_QuestForm;
}

void TQuestController::SetQuestForm(Object^ obj)
{
	TQuestControllerGlobal::m_QuestForm = safe_cast<QuestForm^>(obj);
}

int TQuestController::GetSelectedQuestID()
{
	return this->m_nSelectedQuestID;
}


void TQuestController::SetSelectedQuestID(int nQuestID)
{
	this->m_nSelectedQuestID = nQuestID;
}

int TQuestController::GetCuttedQuestID()
{
	return this->m_nCuttedQuestID;
}


void TQuestController::SetCuttedQuestID(int nQuestID)
{
	this->m_nCuttedQuestID = nQuestID;
}

void TQuestController::GetSelectedCell(DataGridViewCell^& dgvc)
{
	dgvc = TQuestControllerGlobal::m_dgvcSelected;
}

void TQuestController::SetSelectedCell(DataGridViewCell^ dgvc)
{
	TQuestControllerGlobal::m_dgvcSelected = dgvc;
}

Object^ TQuestController::GetPrevSelectedCellValue()
{
	return TQuestControllerGlobal::m_dgvcPrevSelectedValue;
}

void TQuestController::SetPrevSelectedCellValue(Object^ obj)
{
	TQuestControllerGlobal::m_dgvcPrevSelectedValue = obj;
}

String^  TQuestController::GetStringFromStrQuestToolTable(String^ strKey)
{
	return this->m_pSrm->GetStringFromStrQuestToolTable(strKey);
}

bool TQuestController::IsModifiedQuest(int nQuestID)
{
	set<int>::iterator itor = this->m_setModifiedQuestID.find(nQuestID);
	if (this->m_setModifiedQuestID.end() == itor) return false;

	return true;
}

void TQuestController::SetModifiedQuestID(int nQuestID)
{
	set<int>::iterator itor = this->m_setModifiedQuestID.find(nQuestID);
	if (this->m_setModifiedQuestID.end() == itor)
	{
		this->m_setModifiedQuestID.insert(set<int>::value_type(nQuestID));
	}
}

bool TQuestController::IsEmptyModifiedQuestID()
{
	return this->m_setModifiedQuestID.empty();
}

bool TQuestController::IsHidedQuest(int nQuestID)
{
	set<int>::iterator itor = this->m_setHidedQuestID.find(nQuestID);
	if (this->m_setHidedQuestID.end() == itor) return false;

	return true;
}

void TQuestController::SetHidedQuestIDByTitle(String^ strName)
{
	this->m_setHidedQuestID.clear();
	if (0 >= strName->Length) return;

	DataTable^ dt = this->m_pDBm->GetQuestBaseInfo();
	String^ strFilter = String::Format("{0} LIKE '%%{1}%%'", QUEST_DB_TABLE_QUEST_COLUMN_TITLE, strName);

	array<DataRow^>^ arrDr = dt->Select(strFilter);

	vector<int> vecVisibleQuestID;
	for (int i = 0; arrDr->Length > i; i++)
	{
		int nQuestID = MStringToInt(arrDr[i][QUEST_DB_TABLE_QUEST_COLUMN_QID]->ToString());
		this->CollectParentQuestID(nQuestID, vecVisibleQuestID);
		this->CollectChildQuestID(nQuestID, vecVisibleQuestID);
	}

	for (int i = 0; dt->Rows->Count > i; i++)
	{
		int nAllQuestID = MStringToInt(dt->Rows[i][QUEST_DB_TABLE_QUEST_COLUMN_QID]->ToString());
		int nSize = vecVisibleQuestID.size();
		bool bHide = true;
		for (int j = 0; nSize > j; j++)
		{
			if (nAllQuestID == vecVisibleQuestID[j])
			{
				bHide = false;
			}
		}

		if (true == bHide)
		{
			this->m_setHidedQuestID.insert(nAllQuestID);
		}
	}

	this->m_setHidedQuestID;
}

void TQuestController::SetHidedQuestIDByID(int nQuestID)
{
	this->m_setHidedQuestID.clear();
	if (0 > nQuestID) return;

	DataTable^ dt = this->m_pDBm->GetQuestBaseInfo();
	String^ strFilter = String::Format("{0} = {1}", QUEST_DB_TABLE_QUEST_COLUMN_QID, nQuestID);

	array<DataRow^>^ arrDr = dt->Select(strFilter);

	vector<int> vecVisibleQuestID;
	for (int i = 0; arrDr->Length > i; i++)
	{
		int nQuestID = MStringToInt(arrDr[i][QUEST_DB_TABLE_QUEST_COLUMN_QID]->ToString());
		this->CollectParentQuestID(nQuestID, vecVisibleQuestID);
		this->CollectChildQuestID(nQuestID, vecVisibleQuestID);
	}

	for (int i = 0; dt->Rows->Count > i; i++)
	{
		int nAllQuestID = MStringToInt(dt->Rows[i][QUEST_DB_TABLE_QUEST_COLUMN_QID]->ToString());
		int nSize = vecVisibleQuestID.size();
		bool bHide = true;
		for (int j = 0; nSize > j; j++)
		{
			if (vecVisibleQuestID[j] == nAllQuestID)
			{
				bHide = false;
			}
		}

		if (true == bHide)
		{
			this->m_setHidedQuestID.insert(nAllQuestID);
		}
	}
}

void TQuestController::UpdateQuestBaseInfo(int nQuestID, String^ strColumnName, Object^ objValue)
{
	this->m_pDBm->UpdateQuestBaseInfo(nQuestID, strColumnName, objValue);
}

void TQuestController::UpdateQuestPR(int nQuestID, String^ strColumnName, Object^ objValue)
{
	this->m_pDBm->UpdateQuestPR(nQuestID, strColumnName, objValue);
}

void TQuestController::InsertQuestPRFaction(int nQRFID)
{
	this->m_pDBm->InsertQuestPRFaction(nQRFID, this->m_nSelectedQuestID);
}

void TQuestController::UpdateQuestPRFaction(int nQRFID, String^ strColumnName, Object^ objValue)
{
	this->m_pDBm->UpdateQuestPRFaction(nQRFID, strColumnName, objValue);
}

void TQuestController::DeleteQuestPRFaction(int nQRFID)
{
	this->m_pDBm->DeleteQuestPRFaction(nQRFID, this->m_nSelectedQuestID);
}

void TQuestController::InsertQuestOObjective(int nObjectiveID)
{
	this->m_pDBm->InsertQuestOObjective(this->m_nSelectedQuestID, nObjectiveID);
}

void TQuestController::UpdateQuestOObjective(int nQuestID, int nObjectiveID, String^ strColumnName, Object^ objValue)
{
	this->m_pDBm->UpdateQuestOObjective(nQuestID, nObjectiveID, strColumnName, objValue);
}

void TQuestController::DeleteQuestOObjective(int nObjectiveID)
{
	this->m_pDBm->DeleteQuestOObjective(this->m_nSelectedQuestID, nObjectiveID);
}

void TQuestController::InsertQuestUObjective(int nObjectiveID)
{
	this->m_pDBm->InsertQuestUObjective(this->m_nSelectedQuestID, nObjectiveID);
}

void TQuestController::UpdateQuestUObjective(int nQuestID, int nObjectiveID, String^ strColumnName, Object^ objValue)
{
	this->m_pDBm->UpdateQuestUObjective(nQuestID, nObjectiveID, strColumnName, objValue);
}

void TQuestController::DeleteQuestUObjective(int nObjectiveID)
{
	this->m_pDBm->DeleteQuestUObjective(this->m_nSelectedQuestID, nObjectiveID);
}

void TQuestController::CutQuests()
{
	if (0 >= this->m_nSelectedQuestID) return;

	if (0 < this->m_nCuttedQuestID)
	{
		this->m_pDBm->DeleteQuests(this->m_nCuttedQuestID);
	}

	this->m_nCuttedQuestID = this->m_nSelectedQuestID;
}

void TQuestController::PasteQuests()
{
	if (0 >= this->m_nSelectedQuestID) return;

	int nPRQuestID = this->m_nSelectedQuestID;
	this->m_pDBm->UpdateQuestPR(this->m_nCuttedQuestID, QUEST_DB_TABLE_QUEST_COLUMN_PR_QUEST, nPRQuestID);

	this->m_nCuttedQuestID = -1;
}

bool TQuestController::InsertQuests(int nQuestID)
{
	if (0 >= nQuestID || true == this->m_pDBm->HaveQuest(nQuestID)) return false;

	int nPRQuestID = this->m_nSelectedQuestID;
	this->m_pDBm->InsertQuests(nQuestID , nPRQuestID);

	return true;
}

void TQuestController::DeleteQuests()
{
	this->m_pDBm->DeleteQuests(this->m_nSelectedQuestID);
}

void TQuestController::SaveQuests()
{
	this->m_pDBm->SetQuestsToDB();
	this->m_pDBm->End();
	this->m_pDBm->Begin();

	this->SaveQuestsToXML();

	this->m_setModifiedQuestID.clear();

	this->SerializeTreeView_QuestList();
	this->SerializeDataGridView();
}


void TQuestController::CollectParentQuestID(int nQuestID, vector<int>& dstVecQuestID)
{
	dstVecQuestID.push_back(nQuestID);

	DataTable^ dt = this->m_pDBm->GetQuestPR();
	String^ strFilter = String::Format("{0} = {1}", QUEST_DB_TABLE_QUEST_COLUMN_QID, nQuestID);

	array<DataRow^>^ arrDr = dt->Select(strFilter);

	for (int i = 0; arrDr->Length > i; i++)
	{		
		int nParentQuestID = MStringToInt(arrDr[i][QUEST_DB_TABLE_QUEST_COLUMN_PR_QUEST]->ToString());
		if (0 < nParentQuestID)
		{
			this->CollectParentQuestID(nParentQuestID, dstVecQuestID);
		}
	}
}

void TQuestController::CollectChildQuestID(int nQuestID, vector<int>& dstVecQuestID)
{
	dstVecQuestID.push_back(nQuestID);

	DataTable^ dt = this->m_pDBm->GetQuestPR();
	String^ strFilter = String::Format("{0} = {1}", QUEST_DB_TABLE_QUEST_COLUMN_PR_QUEST, nQuestID);

	array<DataRow^>^ arrDr = dt->Select(strFilter);

	for (int i = 0; arrDr->Length > i; i++)
	{		
		int nChildQuestID = MStringToInt(arrDr[i][QUEST_DB_TABLE_QUEST_COLUMN_QID]->ToString());
		this->CollectChildQuestID(nChildQuestID, dstVecQuestID);
	}
}

bool TQuestController::HaveQuest(int nQuestID)
{
	return this->m_pDBm->HaveQuest(nQuestID);
}

bool TQuestController::HaveQuestObjective(int nQuestID, int nObjectiveID)
{
	return this->m_pDBm->HaveQuestObjective(nQuestID, nObjectiveID);
}