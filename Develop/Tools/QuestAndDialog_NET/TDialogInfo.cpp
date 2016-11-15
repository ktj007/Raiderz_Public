#include "StdAfx.h"
#include "TDialogInfo.h"
#include "TStringResMgr.h"

TDialogNode::TDialogNode() : m_nType(-1), m_pParentNode(nullptr)
{

}

TDialogNode::~TDialogNode()
{
	Clear();
}

TDialogNode::TDialogNode(const TDialogNode& rDialogNode)
{
	m_nType = rDialogNode.m_nType;
	m_pParentNode = rDialogNode.m_pParentNode;

	int nCnt = rDialogNode.m_vecpChildNode.size();

	for (int i = 0; nCnt > i; i++)
	{
		TDialogNode* pChildDialogNode = rDialogNode.m_vecpChildNode[i];
		TDialogNode* pNewDialogNode = pChildDialogNode->Clone(this);

		m_vecpChildNode.push_back(pNewDialogNode);
	}
}

void TDialogNode::Clear()
{
	while (false == m_vecpChildNode.empty())
	{
		vector<TDialogNode*>::iterator itor = m_vecpChildNode.begin();
		(*itor)->Clear();
		delete (*itor);
		m_vecpChildNode.erase(itor);
	}
}


TDialogInfo::TDialogInfo() : m_nID(-1)
{
}

TDialogInfo::~TDialogInfo()
{
	m_vecChildSelectNodeExit.clear();
}

TDialogInfo::TDialogInfo(const TDialogInfo& rDialogInfo)
{
	m_nType = rDialogInfo.m_nType;
	m_pParentNode = rDialogInfo.m_pParentNode;

	int nCnt = rDialogInfo.m_vecpChildNode.size();

	for (int i = 0; nCnt > i; i++)
	{
		TDialogNode* pChildDialogNode = rDialogInfo.m_vecpChildNode[i];
		TDialogNode* pNewDialogNode = pChildDialogNode->Clone(this);

		m_vecpChildNode.push_back(pNewDialogNode);
	}

	m_nID = rDialogInfo.m_nID;
}

TDialogInfo* TDialogInfo::Clone(TDialogNode* pParentDialogNode)
{
	TDialogInfo* pNewDialogInfo = new TDialogInfo(*this);
	pNewDialogInfo->m_pParentNode = pParentDialogNode;

	return pNewDialogInfo;
}

void TDialogInfo::SaveToXml(MXmlElement* pElementMaietDialogInfo, MXmlElement* pElementMaietStrings, const int nNPCID, const int nDialogID, int& nCntSay, int& nCntSelect)
{
	MXmlElement* pNewElementDialogInfo = new MXmlElement(DIALOG_XML_TAG_DIALOG);
	pElementMaietDialogInfo->LinkEndChild(pNewElementDialogInfo);

	pNewElementDialogInfo->SetAttribute(DIALOG_XML_ATTR_ID, m_nID);

	for (vector<TDialogNode*>::iterator itor = m_vecpChildNode.begin(); itor != m_vecpChildNode.end(); ++itor)
	{
		(*itor)->SaveToXml(pNewElementDialogInfo, pElementMaietStrings, nNPCID, m_nID, nCntSay, nCntSelect);
	}
}

void TDialogInfo::GetInvalidDialogNode(vector<TDialogNode*>& outVecDialogNode)
{
	for (vector<TDialogNode*>::iterator itor = m_vecpChildNode.begin(); itor != m_vecpChildNode.end(); ++itor)
	{
		(*itor)->GetInvalidDialogNode(outVecDialogNode);
	}
}

TDialogCheckNode::TDialogCheckNode() : m_nCheckType(-1), m_nParam1(-1), m_nParam2(-1)
{

}

TDialogCheckNode::~TDialogCheckNode()
{

}

TDialogCheckNode::TDialogCheckNode(const TDialogCheckNode& rDialogCheckNode)
{
	m_nType = rDialogCheckNode.m_nType;
	m_pParentNode = rDialogCheckNode.m_pParentNode;

	int nCnt = rDialogCheckNode.m_vecpChildNode.size();

	for (int i = 0; nCnt > i; i++)
	{
		TDialogNode* pChildDialogNode = rDialogCheckNode.m_vecpChildNode[i];
		TDialogNode* pNewDialogNode = pChildDialogNode->Clone(this);

		m_vecpChildNode.push_back(pNewDialogNode);
	}

	m_nCheckType = rDialogCheckNode.m_nCheckType;
	m_nParam1 = rDialogCheckNode.m_nParam1;
	m_nParam2 = rDialogCheckNode.m_nParam2;
}

TDialogCheckNode* TDialogCheckNode::Clone(TDialogNode* pParentDialogNode)
{
	TDialogCheckNode* pNewDialogCheckNode = new TDialogCheckNode(*this);
	pNewDialogCheckNode->m_pParentNode = pParentDialogNode;

	return pNewDialogCheckNode;
}

void TDialogCheckNode::SaveToXml(MXmlElement* pElementMaietDialogInfo, MXmlElement* pElementMaietStrings, const int nNPCID, const int nDialogID, int& nCntSay, int& nCntSelect)
{
	MXmlElement* pNewElementDialogInfo = new MXmlElement(DIALOG_XML_TAG_CHECK);
	pElementMaietDialogInfo->LinkEndChild(pNewElementDialogInfo);

	switch (m_nCheckType)
	{
	case DCT_QUEST:
		{
			pNewElementDialogInfo->SetAttribute(DIALOG_XML_ATTR_TYPE, DIALOG_XML_VALUE_TYPE_QUEST);
		}
		break;
	case DCT_FACTION:
		{
			pNewElementDialogInfo->SetAttribute(DIALOG_XML_ATTR_TYPE, DIALOG_XML_VALUE_TYPE_FACTION);
		}
		break;
	}

	pNewElementDialogInfo->SetAttribute(DIALOG_XML_ATTR_PARAM1, m_nParam1);
	pNewElementDialogInfo->SetAttribute(DIALOG_XML_ATTR_PARAM2, m_nParam2);

	for (vector<TDialogNode*>::iterator itor = m_vecpChildNode.begin(); itor != m_vecpChildNode.end(); ++itor)
	{
		(*itor)->SaveToXml(pNewElementDialogInfo, pElementMaietStrings, nNPCID,nDialogID, nCntSay, nCntSelect);
	}
}

void TDialogCheckNode::GetInvalidDialogNode(vector<TDialogNode*>& outVecDialogNode)
{
	for (vector<TDialogNode*>::iterator itor = m_vecpChildNode.begin(); itor != m_vecpChildNode.end(); ++itor)
	{
		(*itor)->GetInvalidDialogNode(outVecDialogNode);
	}
}

TDialogCaseNode::TDialogCaseNode()
{

}

TDialogCaseNode::~TDialogCaseNode()
{

}

TDialogCaseNode::TDialogCaseNode(const TDialogCaseNode& rDialogCaseNode)
{
	m_nType = rDialogCaseNode.m_nType;
	m_pParentNode = rDialogCaseNode.m_pParentNode;

	int nCnt = rDialogCaseNode.m_vecpChildNode.size();

	for (int i = 0; nCnt > i; i++)
	{
		TDialogNode* pChildDialogNode = rDialogCaseNode.m_vecpChildNode[i];
		TDialogNode* pNewDialogNode = pChildDialogNode->Clone(this);

		m_vecpChildNode.push_back(pNewDialogNode);
	}
}

TDialogCaseNode* TDialogCaseNode::Clone(TDialogNode* pParentDialogNode)
{
	TDialogCaseNode* pNewDialogCaseNode = new TDialogCaseNode(*this);
	pNewDialogCaseNode->m_pParentNode = pParentDialogNode;

	return pNewDialogCaseNode;
}

void TDialogCaseNode::SaveToXml(MXmlElement* pElementMaietDialogInfo, MXmlElement* pElementMaietStrings, const int nNPCID, const int nDialogID, int& nCntSay, int& nCntSelect)
{
	MXmlElement* pNewElementDialogInfo = nullptr;

	switch (m_nType)
	{
	case DNT_CASE_CHECKFAIL:
		{
			pNewElementDialogInfo = new MXmlElement(DIALOG_XML_TAG_CASE_CHECKFAIL);
		}
		break;
	case DNT_CASE_NOEXIST:
		{
			pNewElementDialogInfo = new MXmlElement(DIALOG_XML_TAG_CASE_NOEXIST);
		}
		break;
	case DNT_CASE_DOING:
		{
			pNewElementDialogInfo = new MXmlElement(DIALOG_XML_TAG_CASE_DOING);
		}
		break;
	case DNT_CASE_FINAL:
		{
			pNewElementDialogInfo = new MXmlElement(DIALOG_XML_TAG_CASE_FINAL);
		}
		break;
	case DNT_CASE_COMPLETED:
		{
			pNewElementDialogInfo = new MXmlElement(DIALOG_XML_TAG_CASE_COMPLETED);
		}
		break;
	case DNT_CASE_DEFAULT:
		{
			pNewElementDialogInfo = new MXmlElement(DIALOG_XML_TAG_CASE_DEFAULT);
		}
		break;
	}

	if (nullptr ==  pNewElementDialogInfo) return;

	pElementMaietDialogInfo->LinkEndChild(pNewElementDialogInfo);
	for (vector<TDialogNode*>::iterator itor = m_vecpChildNode.begin(); itor != m_vecpChildNode.end(); ++itor)
	{
		(*itor)->SaveToXml(pNewElementDialogInfo, pElementMaietStrings, nNPCID, nDialogID, nCntSay, nCntSelect);
	}
}

void TDialogCaseNode::GetInvalidDialogNode(vector<TDialogNode*>& outVecDialogNode)
{
	for (vector<TDialogNode*>::iterator itor = m_vecpChildNode.begin(); itor != m_vecpChildNode.end(); ++itor)
	{
		(*itor)->GetInvalidDialogNode(outVecDialogNode);
	}
}


TDialogSayNode::TDialogSayNode()
{

}

TDialogSayNode::~TDialogSayNode()
{

}

TDialogSayNode::TDialogSayNode(const TDialogSayNode& rDialogSayNode)
{
	m_nType = rDialogSayNode.m_nType;
	m_pParentNode = rDialogSayNode.m_pParentNode;

	int nCnt = rDialogSayNode.m_vecpChildNode.size();

	for (int i = 0; nCnt > i; i++)
	{
		TDialogNode* pChildDialogNode = rDialogSayNode.m_vecpChildNode[i];
		TDialogNode* pNewDialogNode = pChildDialogNode->Clone(this);

		m_vecpChildNode.push_back(pNewDialogNode);
	}

	m_strText = rDialogSayNode.m_strText;
}

TDialogSayNode* TDialogSayNode::Clone(TDialogNode* pParentDialogNode)
{
	TDialogSayNode* pNewDialogSayNode = new TDialogSayNode(*this);
	pNewDialogSayNode->m_pParentNode = pParentDialogNode;

	return pNewDialogSayNode;
}

void TDialogSayNode::SaveToXml(MXmlElement* pElementMaietDialogInfo, MXmlElement* pElementMaietStrings, const int nNPCID, const int nDialogID, int& nCntSay, int& nCntSelect)
{
	char szNPCID[32];
	sprintf_s(szNPCID, 32, "%05d", nNPCID);
	char szDialogID[32];
	sprintf_s(szDialogID, 32, "%d", nDialogID);
	char szCntSay[32];
	sprintf_s(szCntSay, 32, "%d", nCntSay);
	char szCntSelect[32];
	sprintf_s(szCntSelect, 32, "%d", nCntSelect);

	string strKey;
	strKey = "N";
	strKey += szNPCID;
	strKey += "_OFFD";
	strKey += szDialogID;
	strKey += "_SAY";
	strKey += szCntSay;

	nCntSay++;

	MXmlElement* pNewElementStrings = new MXmlElement(STRINGS_XML_TAG_STR);
	pElementMaietStrings->LinkEndChild(pNewElementStrings);
	pNewElementStrings->SetAttribute(STRINGS_XML_ATTR_KEY, strKey);

	String^ strValue = gcnew String(TStringResMgr::GetInstance()->GetString(this->m_strText));
	pNewElementStrings->SetAttribute(STRINGS_XML_ATTR_STRING, MStringToCharPointer(strValue));

	MXmlElement* pNewElementDialogInfo = new MXmlElement(DIALOG_XML_TAG_SAY);
	pElementMaietDialogInfo->LinkEndChild(pNewElementDialogInfo);
	pNewElementDialogInfo->SetAttribute(DIALOG_XML_ATTR_TEXT, strKey);


	for (vector<TDialogNode*>::iterator itor = m_vecpChildNode.begin(); itor != m_vecpChildNode.end(); ++itor)
	{
		(*itor)->SaveToXml(pNewElementDialogInfo, pElementMaietStrings, nNPCID, nDialogID, nCntSay, nCntSelect);
	}

}

void TDialogSayNode::GetInvalidDialogNode(vector<TDialogNode*>& outVecDialogNode)
{
	outVecDialogNode.push_back(this);

	for (vector<TDialogNode*>::iterator itor = m_vecpChildNode.begin(); itor != m_vecpChildNode.end(); ++itor)
	{
		(*itor)->GetInvalidDialogNode(outVecDialogNode);
	}
}


TDialogSelectNode::TDialogSelectNode() : m_nExit(-1)
{

}

TDialogSelectNode::~TDialogSelectNode()
{

}

TDialogSelectNode::TDialogSelectNode(const TDialogSelectNode& rDialogSelectNode)
{
	m_nType = rDialogSelectNode.m_nType;
	m_pParentNode = rDialogSelectNode.m_pParentNode;

	int nCnt = rDialogSelectNode.m_vecpChildNode.size();

	for (int i = 0; nCnt > i; i++)
	{
		TDialogNode* pChildDialogNode = rDialogSelectNode.m_vecpChildNode[i];
		TDialogNode* pNewDialogNode = pChildDialogNode->Clone(this);

		m_vecpChildNode.push_back(pNewDialogNode);
	}

	m_strText = rDialogSelectNode.m_strText;
	m_nExit = rDialogSelectNode.m_nExit;
}

TDialogSelectNode* TDialogSelectNode::Clone(TDialogNode* pParentDialogNode)
{
	TDialogSelectNode* pNewDialogSelectNode = new TDialogSelectNode(*this);
	pNewDialogSelectNode->m_pParentNode = pParentDialogNode;

	return pNewDialogSelectNode;
}

void TDialogSelectNode::SaveToXml(MXmlElement* pElementMaietDialogInfo, MXmlElement* pElementMaietStrings, const int nNPCID, const int nDialogID, int& nCntSay, int& nCntSelect)
{
	char szNPCID[32];
	sprintf_s(szNPCID, 32, "%05d", nNPCID);
	char szDialogID[32];
	sprintf_s(szDialogID, 32, "%d", nDialogID);
	char szCntSay[32];
	sprintf_s(szCntSay, 32, "%d", nCntSay);
	char szCntSelect[32];
	sprintf_s(szCntSelect, 32, "%d", nCntSelect);

	string strKey;
	strKey = "N";
	strKey += szNPCID;
	strKey += "_OFFD";
	strKey += szDialogID;
	strKey += "_SAY";
	strKey += szCntSay;
	strKey += "_SEL";
	strKey += szCntSelect;
	nCntSelect++;

	MXmlElement* pNewElementStrings = new MXmlElement(STRINGS_XML_TAG_STR);
	pElementMaietStrings->LinkEndChild(pNewElementStrings);
	pNewElementStrings->SetAttribute(STRINGS_XML_ATTR_KEY, strKey);

	String^ strValue = gcnew String(TStringResMgr::GetInstance()->GetString(this->m_strText));
	pNewElementStrings->SetAttribute(STRINGS_XML_ATTR_STRING, MStringToCharPointer(strValue));

	MXmlElement* pNewElementDialogInfo = new MXmlElement(DIALOG_XML_TAG_SELECT);
	pElementMaietDialogInfo->LinkEndChild(pNewElementDialogInfo);
	pNewElementDialogInfo->SetAttribute(DIALOG_XML_ATTR_TEXT, strKey);
	pNewElementDialogInfo->SetAttribute(DIALOG_XML_ATTR_EXIT, m_nExit);

	for (vector<TDialogNode*>::iterator itor = m_vecpChildNode.begin(); itor != m_vecpChildNode.end(); ++itor)
	{
		(*itor)->SaveToXml(pNewElementDialogInfo, pElementMaietStrings, nNPCID, nDialogID, nCntSay, nCntSelect);
	}
}

void TDialogSelectNode::GetInvalidDialogNode(vector<TDialogNode*>& outVecDialogNode)
{
	TDialogNode* pDialogNodeParent = m_pParentNode;

	while (nullptr != pDialogNodeParent)
	{
		if (DNT_SAY == pDialogNodeParent->m_nType)
		{
			for (vector<TDialogNode*>::iterator itor = outVecDialogNode.begin(); outVecDialogNode.end() != itor; ++itor)
			{
				if (pDialogNodeParent == (*itor))
				{
					itor = outVecDialogNode.erase(itor);
					if (outVecDialogNode.end() == itor)
						break;
				}
			}
		}

		if (nullptr == pDialogNodeParent->m_pParentNode)
			break;
		else
			pDialogNodeParent = pDialogNodeParent->m_pParentNode;
	}


	for (vector<TDialogNode*>::iterator itor = m_vecpChildNode.begin(); itor != m_vecpChildNode.end(); ++itor)
	{
		(*itor)->GetInvalidDialogNode(outVecDialogNode);
	}
}
