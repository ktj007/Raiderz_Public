#include "StdAfx.h"
#include "TDialogController.h"
#include "TQuestController.h"
#include "TStringResMgr.h"

TDialogController::TDialogController(TNPCInfoMgr* pNim, TStringResMgr* pSrm) : m_pDialogNodeClip(nullptr), m_nSelectedNPCID(-1), m_nSelectedDialogID(-1), m_bSelectedOnScreen(false), m_pSelectedDialogNode(nullptr)
{
	this->m_pNim = pNim;
	this->m_pSrm = pSrm;

	this->CreateInstance();
}

TDialogController::~TDialogController(void)
{
	this->DeCreateInstance();

	if (nullptr != this->m_pNim)
		delete this->m_pNim;
}

bool TDialogController::Begin(TQuestController* pQc, int nNPCID)
{
	String^ strServerStringsPath = String::Format("{0}{1}", PATH_GAMESERVER, PATH_STRINGS);
	String^ strNPCFilename = String::Format("{0}{1}{2}", PATH_GAMESERVER, PATH_SYSTEM, FILENAME_NPCINFO);

	this->m_nSelectedNPCID = -1;
	this->m_nSelectedDialogID = -1;
	this->m_bSelectedOnScreen = false;
	this->m_pSelectedDialogNode = nullptr;
	this->m_pDialogNodeClip = nullptr;
	TDialogControllerGlobal::m_tnPreSelectedDialogList = nullptr;

	this->m_pQc = pQc;
	this->m_pNim->Load(MStringToCharPointer(strNPCFilename));
	this->m_pSrm->BeginDialog(strServerStringsPath);
	this->SetHidedNPCIDByID(nNPCID);
	
	TDialogControllerGlobal::m_DialogForm->Begin();

	return true;
}

void TDialogController::End()
{
	if (nullptr != TDialogControllerGlobal::m_DialogForm)
	{
		TDialogControllerGlobal::m_DialogForm->End();
	}

	this->m_pSrm->EndDialog();
	this->m_pNim->Clear();
	this->m_pQc = nullptr;

	this->m_setHidedNPCID.clear();
	this->m_setModifiedDialogID.clear();
}

bool TDialogController::CreateInstance()
{
	TDialogControllerGlobal::m_DialogForm = gcnew DialogForm(this);
	if (nullptr == TDialogControllerGlobal::m_DialogForm) return false;

	return true;
}

void TDialogController::DeCreateInstance()
{
	if (nullptr != TDialogControllerGlobal::m_DialogForm)
	{		
		TDialogControllerGlobal::m_DialogForm->Close();
		TDialogControllerGlobal::m_DialogForm = nullptr;
	}
}

void TDialogController::ShowDialogTool()
{
	TDialogControllerGlobal::m_DialogForm->Show();
	TDialogControllerGlobal::m_DialogForm->Focus();
}

void TDialogController::ReloadDialog()
{
	TQuestController* pQc = this->m_pQc;
	this->End();
	this->Begin(pQc);
}

void TDialogController::SerializeTreeView_DialogList()
{
	TDialogControllerGlobal::m_DialogForm->SerializeTreeView_DialogList(this->m_pNim);
}

void TDialogController::SerializeTreeView_DialogInfo()
{
	TDialogControllerGlobal::m_DialogForm->SerializeTreeView_DialogInfo(this->m_pNim);
}

String^ TDialogController::MakeTreeView_DilaogListNPCText(int nQuestID)
{
	TNPCInfoMgr::iterator itor = this->m_pNim->find(nQuestID);
	if (this->m_pNim->end() == itor) return nullptr;

	TNPCInfo* pNi = (*itor).second;
	String^ strNPCName = gcnew String(pNi->strName.c_str());
	int nNPCID = pNi->nID;

	String^ strNPCText = TDialogControllerGlobal::m_DialogForm->MakeNPCTextOfTreeView_DialogList(nNPCID, strNPCName);

	return strNPCText;
}

TreeNode^ TDialogController::SearchTreeNode(TreeNodeCollection^ inNodes, String^ str)
{
	TreeNode^ tn = nullptr;
	System::Collections::IEnumerator^ myNodes = (safe_cast<System::Collections::IEnumerable^>(inNodes))->GetEnumerator();
	while ( myNodes->MoveNext() )
	{
		tn = safe_cast<TreeNode^>(myNodes->Current);
		System::String^ tnstr = (safe_cast<TreeNode^>(myNodes->Current))->Text;
		if(0 == String::Compare(str, tnstr))
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

void TDialogController::CutAndPasteTreeNode(TreeNode^ tnDestination, TreeNode^ tnSource)
{
	String^ strText = gcnew System::String(tnSource->Text);
	tnSource->Text = "";
	TreeNode^ tnChild = tnDestination->Nodes->Add(strText);

	System::Collections::IEnumerator^ etor = tnSource->Nodes->GetEnumerator();
	while(etor->MoveNext())
	{
		TreeNode^ tnCurrent = safe_cast<TreeNode^>(etor->Current);
		CutAndPasteTreeNode(tnChild, tnCurrent);
	}
}

void TDialogController::RunQuestTool()
{
	if (nullptr == this->m_pQc->GetQuestForm())
	{
		this->m_pQc->End();
		this->m_pQc->CreateInstance();
		this->m_pQc->Begin(this);
	}

	this->m_pQc->ShowQuestTool();
}

bool TDialogController::IsValidQuest(int nQuestID)
{
	if (0 >= nQuestID) return false;
	return this->m_pQc->HaveQuest(nQuestID);
}

bool TDialogController::IsValidQuestObjective(int nQuestID, int nObjectiveID)
{
	return this->m_pQc->HaveQuestObjective(nQuestID, nObjectiveID);
}

Object^ TDialogController::GetDialogForm()
{
	return TDialogControllerGlobal::m_DialogForm;
}

void TDialogController::SetDialogForm(Object^ obj)
{
	TDialogControllerGlobal::m_DialogForm = safe_cast<DialogForm^>(obj);
}

int TDialogController::GetSelectedNPCID()
{
	return this->m_nSelectedNPCID;
}

void TDialogController::SetSelectedNPCID(int nNPCID)
{
	this->m_nSelectedNPCID = nNPCID;
}

bool TDialogController::GetSelectedOnScreen()
{
	return this->m_bSelectedOnScreen;
}

void TDialogController::SetSelectedOnScreen(bool bOnScreen)
{
	this->m_bSelectedOnScreen = bOnScreen;
}

int TDialogController::GetSelectedDialogID()
{
	return this->m_nSelectedDialogID;
}

void TDialogController::SetSelectedDialogID(int nDialogID)
{
	this->m_nSelectedDialogID = nDialogID;
}

TDialogNode* TDialogController::GetSelectedDialogNode()
{
	return this->m_pSelectedDialogNode;
}

void TDialogController::SetSelectedDialogNode(TDialogNode* dn)
{
	this->m_pSelectedDialogNode = dn;
}

TDialogNode* TDialogController::GetDialogNodeClip()
{
	return this->m_pDialogNodeClip;
}

void TDialogController::SetDialogNodeClip(TDialogNode* dn)
{
	this->m_pDialogNodeClip = dn;
}

String^ TDialogController::GetString(String^ strKey)
{
	string key = MStringToCharPointer(strKey);
	return gcnew String(this->m_pSrm->GetString(key));
}

void TDialogController::SetStringToStrOnScreenDialogTable(String^ strKey, String^ strValue)
{
	if (false == this->m_pSrm->SetStringToStrOnScreenDialogTable(MStringToCharPointer(strKey), MStringToCharPointer(strValue)))
	{
		this->m_pSrm->InsertStringToStrOnScreenDialogTable(MStringToCharPointer(strKey), MStringToCharPointer(strValue));
	}
}

void TDialogController::SetStringToStrOffScreenDialogTable(String^ strKey, String^ strValue)
{
	if (false == this->m_pSrm->SetStringToStrOffScreenDialogTable(MStringToCharPointer(strKey), MStringToCharPointer(strValue)))
	{
		this->m_pSrm->InsertStringToStrOffScreenDialogTable(MStringToCharPointer(strKey), MStringToCharPointer(strValue));
	}
}

bool TDialogController::IsHidedNPC(int nNPCID)
{
	for each (int nHidedNPCID in this->m_setHidedNPCID)
	{
		if (nNPCID == nHidedNPCID)
			return true;
	}

	return false;
}

void TDialogController::SetHidedNPCIDByName(String^ strName)
{
	this->m_setHidedNPCID.clear();
	if (0 >= strName->Length) return;

	vector<int> vecFindedNPCID;
	this->m_pNim->FindNPCByName(MStringToCharPointer(strName), vecFindedNPCID);

	for each (pair<int, TNPCInfo*> itor in (*this->m_pNim))
	{
		int nAllNPCID = itor.first;
		bool bPushBack = true;
		for each(int nFindedNPCID in vecFindedNPCID)
		{
			if (nAllNPCID == nFindedNPCID)
			{
				bPushBack = false;
			}
		}

		if (true == bPushBack)
		{
			this->m_setHidedNPCID.insert(nAllNPCID);
		}
	}
}

void TDialogController::SetHidedNPCIDByID(int nNPCID)
{
	this->m_setHidedNPCID.clear();
	if (0 > nNPCID) return;

	for each (pair<int, TNPCInfo*> itor in (*this->m_pNim))
	{
		int nAllNPCID = itor.first;
		if (nNPCID == nAllNPCID) continue;

		this->m_setHidedNPCID.insert(nAllNPCID);
	}
}

bool TDialogController::IsModifiedDialog(int nNPCID, bool bOnScreen, int nDialogID)
{
	SET_MODIFIED_DIALOG::iterator itor = this->m_setModifiedDialogID.find(SET_MODIFIED_DIALOG::value_type(nNPCID, pair<bool,int>(bOnScreen, nDialogID)));
	if (this->m_setModifiedDialogID.end() == itor) return false;

	return true;
}

void TDialogController::SetModifiedDialogID()
{
	int nNPCID = this->GetSelectedNPCID();
	bool bOnScreen = this->GetSelectedOnScreen();
	int nDialogID = this->GetSelectedDialogID();

	this->SetModifiedDialogID(nNPCID, bOnScreen, nDialogID);
}

bool TDialogController::IsEmptyModifiedDialogID()
{
	return this->m_setModifiedDialogID.empty();
}



void TDialogController::SetModifiedDialogID(int nNPCID, bool bOnScreen, int nDialogID)
{
	SET_MODIFIED_DIALOG::iterator itor = this->m_setModifiedDialogID.find(SET_MODIFIED_DIALOG::value_type(nNPCID, pair<bool,int>(bOnScreen, nDialogID)));
	if (this->m_setModifiedDialogID.end() == itor)
	{
		this->m_setModifiedDialogID.insert(SET_MODIFIED_DIALOG::value_type(nNPCID, pair<bool,int>(bOnScreen, nDialogID)));
	}
}

String^ TDialogController::MakeOnScreenDialogTempKey() 
{
	String^ strTempKey = String::Format("{0}{1}", gcnew String(TEMPKEY_ONSCREENDIALOG), this->m_nTempkeyOnscreenDialog);
	this->m_nTempkeyOnscreenDialog++;

	return strTempKey;
}

String^ TDialogController::MakeOffScreenDialogTempKey() 
{
	String^ strTempKey = String::Format("{0}{1}", gcnew String(TEMPKEY_OFFSCREENDIALOG), this->m_nTempkeyOffscreenDialog);
	this->m_nTempkeyOffscreenDialog++;

	return strTempKey;
}


void TDialogController::SetPreSelectedDialogListTreeNode(TreeNode^ tn)
{
	TDialogControllerGlobal::m_tnPreSelectedDialogList = tn;
}

TreeNode^ TDialogController::GetPreSelectedDialogListTreeNode()
{
	return TDialogControllerGlobal::m_tnPreSelectedDialogList;
}

bool TDialogController::AddDialog(int nDialogID, bool bOnScreen)
{
	if (0 >= nDialogID) return false;

	this->SetModifiedDialogID(m_nSelectedNPCID, bOnScreen, nDialogID);
	return this->m_pNim->AddDialog(m_nSelectedNPCID, bOnScreen, nDialogID);
}

void TDialogController::DeleteDialog()
{
	this->SetModifiedDialogID(m_nSelectedNPCID, m_bSelectedOnScreen, m_nSelectedDialogID);
	this->m_pNim->DeleteDialog(m_nSelectedNPCID, m_bSelectedOnScreen, m_nSelectedDialogID);
}

void TDialogController::DeleteDialogNode(TDialogNode* pDialogNode)
{
	if (nullptr == pDialogNode) return;

	if (DNT_SELECT == pDialogNode->m_nType)
	{
		TDialogSelectNode* pDialogSelectNode = static_cast<TDialogSelectNode*>(pDialogNode);
		if (0 < pDialogSelectNode->m_nExit)
		{
			TDialogInfo* pSelectedDialogInfo = GetSelectedDialogRootNode();
			if (nullptr == pSelectedDialogInfo) return;

			for (vector<int>::iterator itor = pSelectedDialogInfo->m_vecChildSelectNodeExit.begin(); pSelectedDialogInfo->m_vecChildSelectNodeExit.end() != itor; ++itor)
			{
				if (pDialogSelectNode->m_nExit == (*itor))
				{
					pSelectedDialogInfo->m_vecChildSelectNodeExit.erase(itor);
					break;
				}
			}
		}
	}
	this->m_pNim->DeleteDialogNode(pDialogNode);
}

void TDialogController::CutDialogNode(TDialogNode* pDialogNode)
{
	if (nullptr == pDialogNode) return;

	TDialogNode* pParentDialogNode = pDialogNode->m_pParentNode;

	for(vector<TDialogNode*>::iterator itor = pParentDialogNode->m_vecpChildNode.begin(); pParentDialogNode->m_vecpChildNode.end() != itor; ++itor)
	{
		if (pDialogNode == (*itor))
		{
			pParentDialogNode->m_vecpChildNode.erase(itor);

			m_pDialogNodeClip = pDialogNode;
			break;
		}
	}

	return;
}

void TDialogController::CopyDialogNode(TDialogNode* pDialogNode)
{
	if (nullptr == pDialogNode) return;

	m_pDialogNodeClip = pDialogNode;

	return;
}

TDialogNode* TDialogController::PasteDialogNode(TDialogNode* pDialogNode)
{
	if (nullptr == pDialogNode) return nullptr;
	if (nullptr == m_pDialogNodeClip) return nullptr;

	TDialogNode* pNewDialogNode = m_pDialogNodeClip->Clone(pDialogNode);

	if (DNT_SELECT == pNewDialogNode->m_nType)
	{
		TDialogSelectNode* pNewDialogSelectNode = static_cast<TDialogSelectNode*>(pNewDialogNode);
		pNewDialogSelectNode->m_nExit = 0;
	}

	pDialogNode->m_vecpChildNode.push_back(pNewDialogNode);

	return pNewDialogNode;
}

TDialogCheckNode* TDialogController::AddDialogCheckNode(TDialogNode* pDialogNode)
{
	if (nullptr == pDialogNode) return nullptr;

	TDialogCheckNode* pNewDialogCheckNode = new TDialogCheckNode();
	pNewDialogCheckNode->m_nType = DNT_CHECK;
	pNewDialogCheckNode->m_pParentNode = pDialogNode;

	pNewDialogCheckNode->m_nCheckType = DCT_QUEST;

	pDialogNode->m_vecpChildNode.push_back(pNewDialogCheckNode);

	return pNewDialogCheckNode;
}

TDialogCaseNode* TDialogController::AddDialogCaseNode(TDialogNode* pDialogNode, DIALOG_NODE_TYPE dnt)
{
	if (nullptr == pDialogNode) return nullptr;

	TDialogCaseNode* pNewDialogCaseNode = new TDialogCaseNode();
	pNewDialogCaseNode->m_nType = dnt;
	pNewDialogCaseNode->m_pParentNode = pDialogNode;

	pDialogNode->m_vecpChildNode.push_back(pNewDialogCaseNode);

	return pNewDialogCaseNode;
}

TDialogSayNode* TDialogController::AddDialogSayNode(TDialogNode* pDialogNode)
{
	if (nullptr == pDialogNode) return nullptr;

	TDialogSayNode* pNewDialogSayNode = new TDialogSayNode();
	pNewDialogSayNode->m_nType = DNT_SAY;
	pNewDialogSayNode->m_pParentNode = pDialogNode;

	if (true == m_bSelectedOnScreen)
	{
		pNewDialogSayNode->m_strText = MStringToCharPointer(this->MakeOnScreenDialogTempKey());
		this->m_pSrm->InsertStringToStrOnScreenDialogTable(pNewDialogSayNode->m_strText, "");
	}
	else
	{
		pNewDialogSayNode->m_strText = MStringToCharPointer(this->MakeOffScreenDialogTempKey());
		this->m_pSrm->InsertStringToStrOffScreenDialogTable(pNewDialogSayNode->m_strText, "");
	}

	pDialogNode->m_vecpChildNode.push_back(pNewDialogSayNode);

	return pNewDialogSayNode;
}

TDialogSelectNode* TDialogController::AddDialogSelectNode(TDialogNode* pDialogNode)
{
	if (nullptr == pDialogNode) return nullptr;

	TDialogSelectNode* pNewDialogSelectNode = new TDialogSelectNode();
	pNewDialogSelectNode->m_nType = DNT_SELECT;
	pNewDialogSelectNode->m_pParentNode = pDialogNode;
	pNewDialogSelectNode->m_nExit = 0;

	if (true == m_bSelectedOnScreen)
	{
		pNewDialogSelectNode->m_strText = MStringToCharPointer(this->MakeOnScreenDialogTempKey());
		this->m_pSrm->InsertStringToStrOnScreenDialogTable(pNewDialogSelectNode->m_strText, "");
	}
	else
	{
		pNewDialogSelectNode->m_strText = MStringToCharPointer(this->MakeOffScreenDialogTempKey());
		this->m_pSrm->InsertStringToStrOffScreenDialogTable(pNewDialogSelectNode->m_strText, "");
	}

	pDialogNode->m_vecpChildNode.push_back(pNewDialogSelectNode);

	return pNewDialogSelectNode;
}

TDialogInfo* TDialogController::GetSelectedDialogRootNode()
{
	TNPCInfo* pNPCInfo = this->m_pNim->Get(m_nSelectedNPCID);
	if (nullptr == pNPCInfo) return nullptr;

	TDialogInfo* pDialogRootNode = nullptr;
	if (true == m_bSelectedOnScreen)
	{
		pDialogRootNode = pNPCInfo->FindOnScreenDialogNode(m_nSelectedDialogID);
	}
	else
	{
		pDialogRootNode = pNPCInfo->FindOffScreenDialogNode(m_nSelectedDialogID);
	}

	return pDialogRootNode;
}

void TDialogController::SaveDialogToXml()
{
	this->m_setModifiedDialogID.clear();
	this->m_pNim->SaveDialogToXml();
}

bool TDialogController::HaveOnScreenDialog()
{
	TNPCInfo * pNPCInfo = this->m_pNim->Get(m_nSelectedNPCID);
	if (nullptr == pNPCInfo) return false;

	if (true == pNPCInfo->m_mapOnScreenDialog.empty())
		return false;
	else
		return true;
}

bool TDialogController::HaveOffScreenDialog()
{
	TNPCInfo * pNPCInfo = this->m_pNim->Get(m_nSelectedNPCID);
	if (nullptr == pNPCInfo) return false;

	if (true == pNPCInfo->m_mapOffScreenDialog.empty())
		return false;
	else
		return true;
}

void TDialogController::GetInvalidDialogNode(TDialogNode* const pDialogNode, vector<TDialogNode*>& outVecDialogNode)
{
	pDialogNode->GetInvalidDialogNode(outVecDialogNode);
}

