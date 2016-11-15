#pragma once

//////////////////////////////////////////////////////////////////////////

#pragma push_macro("min")
#pragma push_macro("max")
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include "NxPhysics.h"

#pragma pop_macro("min")
#pragma pop_macro("max")

//////////////////////////////////////////////////////////////////////////


#include <vector>
#include <map>
#include <list>
#include <string>
#include <set>
#include <map>

#include "RPhysXActorController.h"

namespace rs3 {

//////////////////////////////////////////////////////////////////////////
//
// PhysX Cloth SceneNode Controller
//
//////////////////////////////////////////////////////////////////////////

class RPhysXClothActorNode;
class RActorCollisionObject;

class RPhysXClothController : public RPhysXActorController
{
public:

	static const char* CONTROLLER_NAME(){ return "rphysx_cloth_controller"; }
	static RSNCTRL_TYPE CONTROLLER_TYPE(){ return RSNCTRL_POSTUPDATE; }

	RPhysXClothController(NxPhysicsSDK* pSDK, NxScene* pScene, NxCookingInterface* pCookingNxScene, int nCollsionGroup = 2)
		: RPhysXActorController(pScene, CONTROLLER_TYPE(), CONTROLLER_NAME())
		, m_nRefCount(0), m_pSDK(pSDK), m_pScene(pScene), m_pCooking(pCookingNxScene)
		, m_nCollsionGroup(nCollsionGroup)
	{
	}

	virtual ~RPhysXClothController()
	{
		_ASSERT(m_mapClothNodesAdded.empty());
		_ASSERT(m_mapAnchorNxActors.empty());

		ClearCollsionNxActors();
		ClearExternAnchorActorNodes();
	}

	virtual void			Update(RCameraSceneNode* pCamera) override;

	int						Grap()	{ return ++m_nRefCount; }
	int						Drop()	{ return --m_nRefCount; }

	virtual void			AddClothNode(const std::string& _rNodeName);
	virtual void			RemoveClothNode(const std::string& _rNodeName);


protected:

	virtual void			ReNew();
	virtual void			OnBuilderAdded(){ _ASSERT(!"빌더를 이용하는 컨트롤러 아님"); }

	int						m_nRefCount;
	int						m_nCollsionGroup;

	NxPhysicsSDK*			m_pSDK;
	NxScene*				m_pScene;
	NxCookingInterface*		m_pCooking;

	struct SNxCloth
	{
		SNxCloth()
			: m_pCloth(NULL)
			, m_pClothMesh(NULL)
			, m_pClothDesc(NULL)
		{
		}

		~SNxCloth()
		{
			delete m_pClothDesc;
		}

		NxCloth*					m_pCloth;			// cloth instance
		NxClothMesh*				m_pClothMesh;		// cloth mesh instance
		std::vector<std::string>	m_vecAnchorNodes;	// cloth anchor

		NxMeshData					m_MeshData;
		NxClothDesc*				m_pClothDesc;
	};

	typedef std::map< std::string, SNxCloth > CLOTH_MAP;
	typedef std::map< std::string, NxActor* > NXACTOR_MAP;
	typedef std::map< std::string, std::pair<RMeshNode*, RActorNode*> > ANCHOR_CANDIDATE_MAP;
	typedef std::map<std::string, int> CLOTH_CANDIDATE_MAP;

	CLOTH_MAP				m_mapClothNodesAdded;
	NXACTOR_MAP				m_mapAnchorNxActors;
	NXACTOR_MAP				m_mapCollsionNxActors;

	CLOTH_CANDIDATE_MAP		m_mapClothCandidates;
	ANCHOR_CANDIDATE_MAP	m_mapExternAnchorActorNodeCandidates;

	std::set<RActorNode*>	m_setExternAnchorNodes;


	void					CreateClothAnchor(RActorNode* _pClothActorNode, NXACTOR_MAP& _rAnchorOut);
	virtual void			CreateClothCollision();
	NxActor*				CreateNxActorFromCapsuleCollision(const RCapsule& _rCapsule);

	void					CreateCloth(RPhysXClothActorNode* pActorNode, SNxCloth& _rCloth);
	virtual void			FillClothDesc(NxClothDesc& _rDesc, SNxCloth& _rCloth, const RActorNode* _pClothActorNode);

	void					ClearNxCloth(SNxCloth& _rCloth);
	void					ClearCollsionNxActors();
	void					ClearExternAnchorActorNodes();
	void					RemoveNxActorUpdateInfo(NxActor* pActor);

	void					UpdateExternAnchorActorNodesCreation();	//DO NOT CALL in ReNew!!
	virtual void			UpdateClothCreationCandidates();		//DO NOT CALL in ReNew!!

};

}
