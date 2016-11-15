#pragma once

#include "RActor.h"
#include "RActorController.h"
#include "CSItemData.h"
#include "XPartsParam.h"
#include "XCoverMesh.h"
#include "MLocale.h"

using namespace rs3;

class XCharacter;
class XPartsLoadingController;
class XCharacterParts;

/// 파츠 노드 정보
struct XPartsNodeInfo
{
	RActorNode*		pActorNode;
	tstring			strNodeName;
	tstring			strActorNodeName;
	tstring			strParentName;
	unsigned int	dwColor;

	XPartsNodeInfo() : pActorNode(NULL), dwColor(0xFFFFFFFF) {}
	void SetActorNode(RActorNode* actorNode)
	{
		pActorNode = actorNode;
		if (actorNode)
		{
			strActorNodeName = MLocale::ConvAnsiToTCHAR(actorNode->GetNodeName().c_str());
		}
	}
};

/// 파트 상태
enum XPartState
{
	PART_STATE_INVALID = -1,
	PART_STATE_READY_TO_CREATE_MESH = 0,
	PART_STATE_READY_TO_CREATE_PARTS_NODE,
	PART_STATE_LOADING_COMPLETED,
	PART_STATE_LOADING_CANCELED
};

/// 파트 정보
class XPartInfo
{
private:
	XPartState				m_nState;
public:
	int						nPartsID;
	tstring					strMeshName;
	RMesh*					pMesh;
	vector<XPartsNodeInfo>	m_vecNodes;

	XPartInfo();
	void AddNode(tstring& nodeName, tstring& parentName, tstring& actorNodeName, unsigned int nColor=0xFFFFFFFF, RActorNode* actorNode=NULL);
	void Assign(XPartsParam* pPartsParam);
	bool IsEqualTo(const XPartsParam* pPartsParam);

	XPartState GetState() { return m_nState; }
	void SetState(XPartState st) { m_nState = st; }
};

/// 캐릭터 파츠
class XModelPartsInfo
{
public:
	typedef map<int, XPartInfo*>	ModelPartsMap;
	typedef list<XPartInfo*>		ModelPartsList;
private:
	ModelPartsMap					m_PartsMap;
	ModelPartsList					m_LoadingQueue;

	void AddPartInfo( XPartInfo* pParts );
public:
	XModelPartsInfo() {}
	virtual ~XModelPartsInfo() { Clear(); }

	void Clear();
	void DeletePartInfo( int nPartsID );
	void Reserve( XPartsParam* pPartsParam );
	void UpdateQueue();

	XPartInfo* Find( int nPartsID );

	tstring GetReservedPartsMeshName( int nPartsID );
	bool IsExistActorNode( RActorNode* pPartsNode );
	bool IsLoadingQueueEmpty();

	ModelPartsMap& GetParts() { return m_PartsMap; }
	ModelPartsList& GetLoadingQueue() { return m_LoadingQueue; }
};
