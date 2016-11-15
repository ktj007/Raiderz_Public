#ifndef _TDIALOG_INFO_H
#define _TDIALOG_INFO_H

enum DIALOG_CHECK_TYPE
{
	DCT_QUEST	= 0,	// 퀘스트
	DCT_FACTION	= 1,	// 팩션
};

enum DIALOG_NODE_TYPE
{
	DNT_CHECK			= 0,
	DNT_CASE_CHECKFAIL	= 1,
	DNT_CASE_NOEXIST	= 2,
	DNT_CASE_DOING		= 3,
	DNT_CASE_FINAL		= 4,
	DNT_CASE_COMPLETED	= 5,
	DNT_CASE_DEFAULT	= 6,
	DNT_SAY				= 7,
	DNT_SELECT			= 8,
	DNT_INFO			= 9
};

#define DIALOG_XML_TAG_MAIET					"maiet"

#define DIALOG_XML_TAG_DIALOG					"DIALOG"
#define DIALOG_XML_TAG_CHECK					"CHECK"

#define DIALOG_XML_TAG_CASE_CHECKFAIL			"CHECKFAIL"
#define DIALOG_XML_TAG_CASE_NOEXIST				"NOEXIST"
#define DIALOG_XML_TAG_CASE_DOING				"DOING"
#define DIALOG_XML_TAG_CASE_FINAL				"FINAL"
#define DIALOG_XML_TAG_CASE_COMPLETED			"COMPLETED"
#define DIALOG_XML_TAG_CASE_DEFAULT				"DEFAULT"

#define DIALOG_XML_TAG_SAY						"SAY"
#define DIALOG_XML_TAG_SELECT					"SELECT"

#define DIALOG_XML_ATTR_ID						"ID"
#define DIALOG_XML_ATTR_TYPE					"TYPE"
#define DIALOG_XML_ATTR_PARAM1					"PARAM1"
#define DIALOG_XML_ATTR_PARAM2					"PARAM2"
#define DIALOG_XML_ATTR_TEXT					"TEXT"
#define DIALOG_XML_ATTR_EXIT					"EXIT"

#define DIALOG_XML_VALUE_TYPE_QUEST				"QUEST"
#define DIALOG_XML_VALUE_TYPE_FACTION			"FACTION"


struct EXTRACTED_DIALOG_INFO;

/// 다이얼로그 트리를 구성하는 기본 노드
class TDialogNode
{
public:
	TDialogNode();
	virtual ~TDialogNode();

	TDialogNode(const TDialogNode& rDialogNode);
	virtual TDialogNode* Clone(TDialogNode* pParentDialogNode) = 0;

	void Clear();

	virtual void SaveToXml(MXmlElement* pElementMaietDialogInfo, MXmlElement* pElementMaietStrings, const int nNPCID, const int nDialogID, int& nCntSay, int& nCntSelect) = 0;
	virtual void GetInvalidDialogNode(vector<TDialogNode*>& outVecDialogNode) = 0;

	int m_nType;
	TDialogNode* m_pParentNode;
	vector<TDialogNode*> m_vecpChildNode;
};

/// 다이얼로그 xml의 <DIALOG> 정보가 들어있는 노드 이면서, 최상위 노드
class TDialogInfo : public TDialogNode
{
public:
	TDialogInfo(void);
	~TDialogInfo(void);

	TDialogInfo(const TDialogInfo& rDialogInfo);
	virtual TDialogInfo* Clone(TDialogNode* pParentDialogNode);

	virtual void SaveToXml(MXmlElement* pElementMaietDialogInfo, MXmlElement* pElementMaietStrings, const int nNPCID, const int nDialogID, int& nCntSay, int& nCntSelect);
	virtual void GetInvalidDialogNode(vector<TDialogNode*>& outVecDialogNode);

	int m_nID;
	vector<int> m_vecChildSelectNodeExit;				///< 자식 셀렉트 노드들의  exit값을 모아놨다가 새로운  exit값 들어올때 사용.
};

/// 다이얼로그 xml의 <CHECK> 정보가 들어있는 노드
class TDialogCheckNode : public TDialogNode
{
public:
	TDialogCheckNode();
	~TDialogCheckNode();

	TDialogCheckNode(const TDialogCheckNode& rDialogCheckNode);
	virtual TDialogCheckNode* Clone(TDialogNode* pParentDialogNode);

	virtual void SaveToXml(MXmlElement* pElementMaietDialogInfo, MXmlElement* pElementMaietStrings, const int nNPCID, const int nDialogID, int& nCntSay, int& nCntSelect);
	virtual void GetInvalidDialogNode(vector<TDialogNode*>& outVecDialogNode);

	int m_nCheckType;
	int m_nParam1;
	int m_nParam2;
};

/// 다이얼로그 xml의 <CHECKFAIL>, <NOEXIST>, <DOING>, <FINAL>, <COMPLETED> 정보가 들어있는 노드
class TDialogCaseNode : public TDialogNode
{
public:
	TDialogCaseNode();
	~TDialogCaseNode();

	TDialogCaseNode(const TDialogCaseNode& rDialogCaseNode);
	virtual TDialogCaseNode* Clone(TDialogNode* pParentDialogNode);

	virtual void SaveToXml(MXmlElement* pElementMaietDialogInfo, MXmlElement* pElementMaietStrings, const int nNPCID, const int nDialogID, int& nCntSay, int& nCntSelect);
	virtual void GetInvalidDialogNode(vector<TDialogNode*>& outVecDialogNode);
};

/// 다이얼로그 xml의 <SAY> 정보가 들어있는 노드
class TDialogSayNode : public TDialogNode
{
public:
	TDialogSayNode();
	~TDialogSayNode();

	TDialogSayNode(const TDialogSayNode& rDialogSayNode);
	virtual TDialogSayNode* Clone(TDialogNode* pParentDialogNode);

	virtual void SaveToXml(MXmlElement* pElementMaietDialogInfo, MXmlElement* pElementMaietStrings, const int nNPCID, const int nDialogID, int& nCntSay, int& nCntSelect);
	virtual void GetInvalidDialogNode(vector<TDialogNode*>& outVecDialogNode);

	string m_strText;
};

/// 다이얼로그 xml의 <SELCT> 정보가 들어있는정보 노드
class TDialogSelectNode : public TDialogNode
{
public:
	TDialogSelectNode();
	~TDialogSelectNode();

	TDialogSelectNode(const TDialogSelectNode& rDialogSelectNode);
	virtual TDialogSelectNode* Clone(TDialogNode* pParentDialogNode);

	virtual void SaveToXml(MXmlElement* pElementMaietDialogInfo, MXmlElement* pElementMaietStrings, const int nNPCID, const int nDialogID, int& nCntSay, int& nCntSelect);
	virtual void GetInvalidDialogNode(vector<TDialogNode*>& outVecDialogNode);

	string m_strText;
	int m_nExit;
};

/// 다이얼로그 트리에서 현제 플레이어의 상태에 따라 추출된 다이얼로그 정보
struct EXTRACTED_DIALOG_INFO
{
	int nDialogID;										///< 다이얼로그 ID
	MUID uidDialogNPC;									///< NPC ID

	TDialogSayNode* pDialogSayNode;						///< NPC가 말하는 정보가 들어 있는 노드
	vector<TDialogSelectNode*> vecpDiaolgSelectNode;	///< 플레이어가 선택할수 있는 선택지의 정보가 들어 있는 노드들
	void Init()
	{
		uidDialogNPC.Invalid();
		nDialogID = 0;
		pDialogSayNode = nullptr;
		vecpDiaolgSelectNode.clear();
		pSelectedDialogSelectNode = nullptr;
	}

	TDialogSelectNode* pSelectedDialogSelectNode;		///< 플레이어가 선택한 선택지의 정보가 들어있는 노드
};



#endif // _TDIALOG_INFO_H