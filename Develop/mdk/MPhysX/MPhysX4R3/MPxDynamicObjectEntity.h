#pragma once

#include "../MPxObjDynamic.h"
#include "MPxTransformController.h"

namespace rs3
{
	class RActor;
	class RActorNode;
	class RPhysXActorController;
}

namespace physx
{

//////////////////////////////////////////////////////////////////////////
//
// Dynamic Object Entity
//
//////////////////////////////////////////////////////////////////////////

class MPxDynamicObjectEntity : public MPxObjDynamic::DynamicObjectEntity
{
public:

									MPxDynamicObjectEntity(MPxTcEntity* _pPxTcEntity);
	virtual							~MPxDynamicObjectEntity(){}

	void							ResetDynamicBodyEntity(rs3::RActor* pActor, const std::vector<std::string>& _rActorNodeName, const char* _pSzDeletionChunck);
	bool							IsAllShapesDeActive(){ return ( (m_nActorNodesDeActive > 0) && (m_nActorNodesDeActive == m_vecNxInfoHandles.size()) ); }

	void							ClearHandles(){ m_pActor = NULL; m_ActorNodeInfos.Clear(); m_vecNxInfoHandles.clear(); m_nActorNodesDeActive = 0; }
	void							DeleteActorNodes();

	void							SetKinematicMode(bool bKinematic, MPxObjDynamic* pDynamic);
	void							SetFadeOutFactor(float _fTime){ m_ActorNodeInfos.m_fFadeOutFactor = _fTime; }
	void							SetCustomReferenceNode(const std::string& _rRefNodePostfix){ m_strCustomReferenceNodePostfix = _rRefNodePostfix; }

	void							GrabSubActorShape();
	void							ReleaseSubActorShape();


	rs3::RPhysXActorController*		GetPhysXActorController(){ return m_pPhysXActorController; }
	int								GetMassRank(const std::string& _rActorNodeName);


	vector<NxActor*>				GetActors();

	void							CheckEndSubActorSimulate(std::vector<std::string>& vecEndList);

protected:

	virtual int						GetSubActorCount();
	virtual bool					GetSubActorInfo(int _nIndex, std::string& _rShapeName, std::vector<NxVec3>& _refVertices, std::vector<NxU32>& _refFaces, MMatrix& _rMatLocal, bool& _rIsKinematic);

	virtual void					OnSubActorAdded( int _nSubActorIndex, physx::MSUBACTORHANDLE _hAddedSubActorHandle );
	virtual void					OnActivatedSubActorsAll(MMatrix& _rOutEntityTM);
	virtual void					OnDeActivatedSubActorsAll();

	virtual void					OnSubActorSimulated( NxActor* pNxActor );
	virtual void					OnSubActorCollisionStarted(NxActor* pNxActor);

	virtual const MBox&				GetAreaBox();
	virtual void					OnSimulated();
	
	bool							IsStopSubActorSimulated(NxActor* pNxActor);


private:

	rs3::RPhysXActorController*		m_pPhysXActorController;
	rs3::RActor*					m_pActor;
	MPxTcEntity*					m_pPxTcEntity;

	std::string						m_strCustomReferenceNodePostfix;
	bool							m_bKinematicMode;
	std::vector< MSUBACTORHANDLE >	m_vecNxInfoHandles;
	size_t							m_nActorNodesDeActive;

	std::vector< std::string >		m_vecShapeNames;


	//////////////////////////////////////////////////////////////////////////
	//
	// ActorNode Info
	//
	//////////////////////////////////////////////////////////////////////////

	struct SDynamicActorNodeInfo
	{
		void Clear()
		{
			m_vecActorNodesList.clear();
			m_strDeletionChunck.clear();

			m_mapActorNodeDisapper.clear();
			m_mapActorNodeMassRank.clear();
		}

		bool IsFadeOutEnabled()
		{
			return (m_fFadeOutFactor >= 0.f);
		}

		bool IsFadeOutStartedNode(const std::string& _rStrNodeName)
		{
			std::map< std::string, float >::iterator itr = m_mapActorNodeDisapper.find(_rStrNodeName);
			if ( itr == m_mapActorNodeDisapper.end() )
				return false;

			return true;
		}

		bool CheckFadeOutStart(const std::string& _rStrNodeName, float _fDelta)
		{
			if (false == IsFadeOutStartedNode(_rStrNodeName))
			{
				m_mapActorNodeDisapper[_rStrNodeName] = m_fFadeOutFactor; //10√ 
				return false;
			}

			float& rTime = m_mapActorNodeDisapper[_rStrNodeName];
			rTime -= _fDelta;
			if (rTime < 0)
				return true;

			return false;
		}

		template < typename T >
		T GetActorNodesCount()
		{
			return (T)m_vecActorNodesList.size();
		}

		const char* GetActorNodeName(int _nIndex)
		{
			return m_vecActorNodesList[_nIndex].c_str();
		}

		std::vector< std::string >		m_vecActorNodesList;
		std::string						m_strDeletionChunck;

		std::map< std::string, float >	m_mapActorNodeDisapper;
		float							m_fFadeOutFactor;

		std::map< std::string, int >	m_mapActorNodeMassRank;

	} m_ActorNodeInfos;
};

}
