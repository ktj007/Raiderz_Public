#ifndef _TDIALOG_CONTROLLER_H
#define _TDIALOG_CONTROLLER_H

class TStringResMgr;
class TNPCInfoMgr;
class TQuestController;

using namespace System;
using namespace System::Windows::Forms;

typedef set<pair<int,pair<bool,int>>> SET_MODIFIED_DIALOG;

public class TDialogController
{
private:
	TQuestController*	m_pQc;
	TNPCInfoMgr*		m_pNim;
	TStringResMgr*		m_pSrm;
	set<int>			m_setHidedNPCID;
	SET_MODIFIED_DIALOG	m_setModifiedDialogID;

	int					m_nSelectedNPCID;
	bool				m_bSelectedOnScreen;
	int					m_nSelectedDialogID;
	TDialogNode*		m_pDialogNodeClip;

	TDialogNode*		m_pSelectedDialogNode;

	unsigned int m_nTempkeyDefault;
	unsigned int m_nTempkeyQuest;
	unsigned int m_nTempkeyOnscreenDialog;
	unsigned int m_nTempkeyOffscreenDialog;

	void SetModifiedDialogID(int nNPCID, bool bOnScreen, int nDialogID);

public:
	TDialogController(TNPCInfoMgr* pNim, TStringResMgr* pSrm);
	~TDialogController();

	bool Begin(TQuestController* pQc, int nNPCID = -1);
	void End();
	bool CreateInstance();
	void DeCreateInstance();

	void ShowDialogTool();
	void ReloadDialog();

	void SerializeTreeView_DialogList();
	void SerializeTreeView_DialogInfo();

	String^ MakeTreeView_DilaogListNPCText(int nNPCID);
	TreeNode^ SearchTreeNode(TreeNodeCollection^ inNodes, String^ str);
	void CutAndPasteTreeNode(TreeNode^ tnDestination, TreeNode^ tnSource);

	void RunQuestTool();
	
	bool IsValidQuest(int nQuestID);
	bool IsValidQuestObjective(int nQuestID, int nObjectiveID);

	Object^ GetDialogForm();
	void SetDialogForm(Object^ obj);
	int  GetSelectedNPCID();
	void SetSelectedNPCID(int nNPCID);
	bool GetSelectedOnScreen();
	void SetSelectedOnScreen( bool bOnScreen);
	int  GetSelectedDialogID();
	void SetSelectedDialogID(int nDialogID);
	TDialogNode* GetSelectedDialogNode();
	void SetSelectedDialogNode(TDialogNode* pDn);
	TDialogNode* GetDialogNodeClip();
	void SetDialogNodeClip(TDialogNode* pDn);
	String^ GetString(String^ strKey);
	void SetStringToStrOnScreenDialogTable(String^ strKey, String^ strValue);
	void SetStringToStrOffScreenDialogTable(String^ strKey, String^ strValue);

	bool IsHidedNPC(int nNPCID);
	void SetHidedNPCIDByName(String^ strName);
	void SetHidedNPCIDByID(int nID);
	bool IsModifiedDialog(int nNPCID, bool bOnScreen, int nDialogID);
	void SetModifiedDialogID();
	bool IsEmptyModifiedDialogID();



	String^ MakeOnScreenDialogTempKey();
	String^ MakeOffScreenDialogTempKey();

	void SetPreSelectedDialogListTreeNode(TreeNode^ tn);
	TreeNode^ GetPreSelectedDialogListTreeNode();

	bool AddDialog(int nDialogID, bool bOnScreen);
	void DeleteDialog();

	void DeleteDialogNode(TDialogNode* pDialogNode);
	void CutDialogNode(TDialogNode* pDialogNode);
	void CopyDialogNode(TDialogNode* pDialogNode);
	TDialogNode* PasteDialogNode(TDialogNode* pDialogNode);

	TDialogCheckNode* AddDialogCheckNode(TDialogNode* pDialogNode);
	TDialogCaseNode* AddDialogCaseNode(TDialogNode* pDialogNode, DIALOG_NODE_TYPE dnt);
	TDialogSayNode* AddDialogSayNode(TDialogNode* pDialogNode);
	TDialogSelectNode* AddDialogSelectNode(TDialogNode* pDialogNode);

	TDialogInfo* GetSelectedDialogRootNode();

	void SaveDialogToXml();

	bool HaveOnScreenDialog();
	bool HaveOffScreenDialog();

	void GetInvalidDialogNode(TDialogNode* const pDialogNode, vector<TDialogNode*>& outVecDialogNode);
};

ref class TDialogNodeAdapter : public System::Object
{
public :
	TDialogNode* m_pDialogNode;
public : 
	TDialogNodeAdapter(TDialogNode* pDialogNode)
	{
		m_pDialogNode = pDialogNode;
	}
	virtual ~TDialogNodeAdapter()
	{
		m_pDialogNode = nullptr;
	}

};

#endif //_TDIALOG_CONTROLLER_H
