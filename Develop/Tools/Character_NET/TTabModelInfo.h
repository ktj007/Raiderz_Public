#pragma once

class TTabModelInfo
{
private:
	struct stBoneNodeData
	{	
		RActorNode*			m_pParentsNode;
		RActorNode*			m_pMeNode;
		float				m_fScale;

		stBoneNodeData()
		{
			m_pParentsNode	= NULL;
			m_pMeNode		= NULL;

			m_fScale		= 1.0f;
		}
	};	

public:
	vector<RActorNode*>		m_NodeList;
	vector<RActorNode*>		m_PartsColorTestList;

	vector<stBoneNodeData>	m_vecBoneNodeList;

	// 선택한 노드
	stBoneNodeData			m_SelectNodeOtherList;
	stBoneNodeData*			m_pSelectNode;
	stBoneNodeData*			m_pTempSelectNode;
	float					m_fSelectNodeDrawBoxSize;

private:
	void			InitSelectNode();
	void			SetModelBipNode(RActorNode* pActorNode);

	void			DrawBoneBox(RActorNode* actorNode, float fSize, float fScale, DWORD dwColor = 0xffffffff);
	void			DrawBoneLinkLine(stBoneNodeData& boneData);
	void			DrawBoneName(RActorNode* actorNode, DWORD dwColor);

	void			CalSelectNodeDrawBoxSize(TCharacter* pActor);

	bool			CheckSameBoneNode(RActorNode* pNode, int& nOutCount);
	bool			CheckDelDummyBoneNode(RActorNode* pNode, RActorNode* pParentNode);
	bool			CheckExistNodeByAnimation(string strNodeName);

	stBoneNodeData*	GetBoneNodeData(RActorNode* pActorNode);

	void			SetPartsColorTestActorNode(TCharacter* pActor, vector<string>& vecActorNodeList);

public:
	TTabModelInfo(void);
	virtual ~TTabModelInfo(void);

	bool			SetBipTreeView();
	void			SetSelectedNodeInfo(String^ nodeName);

	void			NodeNoneSelectedButtonClick();
	void			SkinNodesSelectButtonClick();
	void			ItemNodesSelectButtonClick();

	bool			ReadySkinColorTest();
	bool			ReadyHairColorTest();
	bool			ReadyItemColorTest();

	void			OnBoneRander();

	void			CheckMousePick(RVector2& pos);
	RActorNode*		BoneSelect();

	string			GetSelectNodeName();
	RActorNode*		GetSelectNode();
};
