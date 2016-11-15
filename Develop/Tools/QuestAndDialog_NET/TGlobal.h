#ifndef _TGLOBAL_H
#define _TGLOBAL_H

#include "TStringResMgr.h"

// UI ฐüทร
enum _TreeNodeType
{
	TNT_NPC = 0,
	TNT_DIALOG,
	TNT_ON_SCREEN,
	TNT_OFF_SCREEN,
};

ref class TUIHelper
{
public:
	static _TreeNodeType GetTypeFromTreeNodeTag(Object^ Tag)
	{
		const char* szTag = MStringToCharPointer(Tag->ToString());

		if (0 == strcmp("NPC", szTag))
		{
			return TNT_NPC;
		}
		else if (0 == strcmp("Dialog", szTag))
		{
			return TNT_DIALOG;
		}
		else if (0 == strcmp("OnScreen", szTag))
		{
			return TNT_ON_SCREEN;
		}
		else if (0 == strcmp("OffScreen", szTag))
		{
			return TNT_OFF_SCREEN;
		}

		return TNT_NPC;
	}

	static int GetNPCIDFromTreeNodeText(Object^ Tag)
	{
		const char* szTag = MStringToCharPointer(Tag->ToString());
		strtok(const_cast<char*>(szTag), "(");
		const char* szNPCID = strtok(NULL, ")"); 

		int nNPCID = atoi(szNPCID);
		return nNPCID;
	}

	static void MakeTreeNodeTextFromDialogNode(TDialogNode* pDialogNode, char* szTreeNodeText)
	{
		switch(pDialogNode->m_nType)
		{
		case DNT_CHECK			:
			{
				TDialogCheckNode* pDialogChekNode = static_cast<TDialogCheckNode*>(pDialogNode);
				switch (pDialogChekNode->m_nCheckType)
				{
				case DCT_QUEST	:
					{
						if (0 > pDialogChekNode->m_nParam2)
							sprintf_s(szTreeNodeText, MAX_TREENODE_TEXT_LEN, "CHECK=Quest QuestID=%d", pDialogChekNode->m_nParam1);
						else
							sprintf_s(szTreeNodeText, MAX_TREENODE_TEXT_LEN, "CHECK=Quest QuestID=%d ObjectiveID=%d", pDialogChekNode->m_nParam1, pDialogChekNode->m_nParam2);
					}
					break;
				case DCT_FACTION:
					{
						sprintf_s(szTreeNodeText, MAX_TREENODE_TEXT_LEN, "");
					}
					break;
				};
			}
			break;

		case DNT_CASE_CHECKFAIL	:
			sprintf_s(szTreeNodeText, MAX_TREENODE_TEXT_LEN, "CHECKFAIL");
			break;
		case DNT_CASE_NOEXIST	:
			sprintf_s(szTreeNodeText, MAX_TREENODE_TEXT_LEN, "NOEXIST");
			break;
		case DNT_CASE_DOING		:	
			sprintf_s(szTreeNodeText, MAX_TREENODE_TEXT_LEN, "DOING");
			break;
		case DNT_CASE_FINAL		:		
			sprintf_s(szTreeNodeText, MAX_TREENODE_TEXT_LEN, "FINAL");
			break;
		case DNT_CASE_COMPLETED	:
			sprintf_s(szTreeNodeText, MAX_TREENODE_TEXT_LEN, "COMPLETED");
			break;
		case DNT_CASE_DEFAULT	:	
			sprintf_s(szTreeNodeText, MAX_TREENODE_TEXT_LEN, "DEFAULT");
			break;
		case DNT_SAY			:
			{
				TDialogSayNode* pDialogSayNode = static_cast<TDialogSayNode*>(pDialogNode);
				sprintf_s(szTreeNodeText, MAX_TREENODE_TEXT_LEN, "NPC=%s", TStr(pDialogSayNode->m_strText));
			}
			break;
		case DNT_SELECT			:
			{
				TDialogSelectNode* pDialogSelectNode = static_cast<TDialogSelectNode*>(pDialogNode);
				sprintf_s(szTreeNodeText, MAX_TREENODE_TEXT_LEN, "PC=%s EXIT=%d", TStr(pDialogSelectNode->m_strText), pDialogSelectNode->m_nExit);
			}
			break;

		case DNT_INFO			:
			{
				TDialogInfo* pDialogInfo = static_cast<TDialogInfo*>(pDialogNode);
				sprintf_s(szTreeNodeText, MAX_TREENODE_TEXT_LEN, "DialogID=%d", pDialogInfo->m_nID);
			}
			break;
		};
	}
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
		m_pDialogNode = NULL;
	}

};

#endif //_TGLOBAL_H