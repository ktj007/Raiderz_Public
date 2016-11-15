#ifndef _TQUEST_CONTROLLER_H
#define _TQUEST_CONTROLLER_H

class TDBManager;
class TDialogController;

using namespace System;
using namespace System::Windows::Forms;

public class TQuestController
{
private:
	TDialogController*		m_pDc;
	TDBManager*				m_pDBm;
	TStringResMgr*			m_pSrm;

	set<int>				m_setModifiedQuestID;
	set<int>				m_setHidedQuestID;
	int						m_nSelectedQuestID;
	int						m_nCuttedQuestID;
	
	String^	MakeQuestTitleKey(int nQuestID);
	String^	MakeQuestDescKey(int nQuestID);
	String^	MakeOObjectiveTitleKey(int nQuestID, int nObjectiveID);
	String^	MakeOObjectiveDescKey(int nQuestID, int nObjectiveID);
	String^	MakeUObjectiveTitleKey(int nQuestID, int nObjectiveID);
	String^	MakeUObjectiveDescKey(int nQuestID, int nObjectiveID);

	void SaveQuestsToXML();
	void SaveQuestToXML(MXmlElement* pXeQuest_MAIET, MXmlElement* pXeQuestStrings_MAIET, String^ strFilter,int nQuestID);
	void SaveQuestStringsToXML(MXmlElement* pXeQuestStrings_MAIET, String^ strKey, String^ strValue);
	void SavePrerequisitesToXML(MXmlElement* pXeQuest_QUEST, String^ strFilter);
	void SaveFactionToXML(MXmlElement* pXeQuest_PR, String^ strFilter);
	void SaveOObjectiveToXML(MXmlElement* pXeQuest_QUEST, MXmlElement* pXeQuestStrings_MAIET, String^ strFilter, int nQuestID);
	void SaveUObjectiveToXML(MXmlElement* pXeQuest_QUEST, MXmlElement* pXeQuestStrings_MAIET, String^ strFilter, int nQuestID);

public:
	TQuestController(TDBManager* pDBm, TStringResMgr* pSrm);
	virtual ~TQuestController(void);

	bool Begin(TDialogController* pDc);
	void End();
	bool CreateInstance();
	void DeCreateInstance();

	void ShowQuestTool();
	void ReloadQuest();

	void SerializeTreeView_QuestList();
	void SerializeDataGridView();
	void SerializeDataGridView_QuestBaseInfo();
	void SerializeDataGridView_QuestPR();
	void SerializeDataGridView_QuestPRFaction();
	void SerializeDataGridView_QuestOObjective();
	void SerializeDataGridView_QuestUObjective();

	System::Windows::Forms::TreeNode^ SearchTreeNode(TreeNodeCollection^ inNodes, System::String^ str);
	void CutAndPasteTreeNode(System::Windows::Forms::TreeNode^ tnDestination, TreeNode^ tnSource);
	System::String^ MakeTreeView_QuestListText(int nQuestID);

	void RunDialogTool();

	Object^ GetQuestForm();
	void SetQuestForm(Object^ obj);
	int  GetSelectedQuestID();
	void SetSelectedQuestID(int nQuestID);
	int  GetCuttedQuestID();
	void SetCuttedQuestID(int nQuestID);
	void GetSelectedCell(DataGridViewCell^& dgvc);
	void SetSelectedCell(DataGridViewCell^ dgvc);
	Object^ GetPrevSelectedCellValue();
	void SetPrevSelectedCellValue(Object^ obj);
	String^ GetStringFromStrQuestToolTable(String^ strKey);

	bool IsModifiedQuest(int nQuestID);
	void SetModifiedQuestID(int nQuestID);
	bool IsEmptyModifiedQuestID();
	bool IsHidedQuest(int nQuestID);
	void SetHidedQuestIDByTitle(String^ strName);
	void SetHidedQuestIDByID(int nQuestID);
	
	void UpdateQuestBaseInfo(int nQuestID, String^ strColumnName, Object^ objValue);
	void UpdateQuestPR(int nQuestID, String^ strColumnName, Object^ objValue);

	void InsertQuestPRFaction(int nQRFID);
	void UpdateQuestPRFaction(int nQRFID, String^ strColumnName, Object^ objValue);
	void DeleteQuestPRFaction(int nQRFID);

	void InsertQuestOObjective(int nObjectiveID);
	void UpdateQuestOObjective(int nQuestID, int nObjectiveID, String^ strColumnName, Object^ objValue);
	void DeleteQuestOObjective(int nObjectiveID);

	void InsertQuestUObjective(int nObjectiveID);
	void UpdateQuestUObjective(int nQuestID, int nObjectiveID, String^ strColumnName, Object^ objValue);
	void DeleteQuestUObjective(int nObjectiveID);

	void CutQuests();
	void PasteQuests();
	bool InsertQuests(int nQuestID);
	void DeleteQuests();

	void SaveQuests();
	
	void CollectParentQuestID(int nQuestID, vector<int>& dstVecQuestID);
	void CollectChildQuestID(int nQuestID, vector<int>& dstVecQuestID);
	
	bool HaveQuest(int nQuestID);
	bool HaveQuestObjective(int nQuestID, int nObjectiveID);
};



#endif //_TQUEST_CONTROLLER_H
