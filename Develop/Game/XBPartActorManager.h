#pragma once
#include "XCommonData.h"
#include "XEffectTrace.h"
#include "XEffectInvoker.h"

class XBPartActorManager
{
private:
	struct stBPartActorNodeInfo
	{
		TD_USABLE_BPART tdBPartInfo;

		string strActorNodeName;

		bool bLootable;

		stBPartActorNodeInfo()
		{
			bLootable = false;
		}

		void setData(TD_USABLE_BPART* pBPart, string& strNodeName)
		{
			tdBPartInfo = *pBPart;

			strActorNodeName = strNodeName;
		}

		void setEffectTrace(RActor* pActor)
		{
			RActorNode* pActorNode = pActor->GetActorNode(strActorNodeName.c_str());
			if(pActorNode == NULL)
				return;

			vec3 vStart = pActorNode->GetPosition();
			pActorNode->GetWorldTransform().TransformVect(pActorNode->m_pMeshNode->m_boundingBox.GetCenter(), vStart);
		}
	};

	struct stDelBPart
	{
		MUID						m_uidNPC;
		string						m_strDelActorNodeName;
	};

	vector<stBPartActorNodeInfo>	m_vecBPartActorNodeInfo;
	set<int>						m_setIndex;

	XObject*						m_pOwner;

	vector<stDelBPart>				m_vecDel;

private:
	MUID					CreateBPartNpc(string& strActorNodeName, XCharacter* pActor, int nBPartID);
	void					UpdateNewBPartNpc(float fDelta);

	bool					CheckActorNodeNameDuplication(string strActorNodeName);
	float					CheckBoundBoxFromGroundZ(MMatrix& matWorld, RBoundingBox& boundingBox);

public:
	XBPartActorManager();
	virtual ~XBPartActorManager();

	void					Init(XObject* pObject);
	void					Update(float fDelta);

	void					AddIndex(int nIndex);
	void					GetBPartActorNodeList(rs3::RActorNode* _pParentActorNode, TD_USABLE_BPART* pUsableBpart, int nUsableBpartCount, std::vector<std::string>& _rBPartActorNodeInfo, std::vector<std::string>& _rFragActorNodes);
	void					RemoveSimulationActorNodes(std::vector<std::string>& _rFragActorNodes, std::vector<BPART_ACTOR_NODE_INFO>& _rBPartActorNodeInfo);

	void					SetLootableBPartNPC(MUID uid, bool bLootable);

};