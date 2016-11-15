#pragma once

#include "MPhysXDef.h"
#include "MPhysXObject.h"
#include "MPhysXShape.h"
#include "MSingleton.h"

namespace physx {

//////////////////////////////////////////////////////////////////////////
//
// MPhysX
//  - MPhysXObject 관리 / 오브젝트 Factory
//  - NxActor등 Nx리소스 관리
//
//////////////////////////////////////////////////////////////////////////

class MPhysX : public MSingletonRestricted< MPhysX >
{
	friend class MPhysXObject;
	friend class MPhysXShape;

public:

	MPhysX();
	virtual ~MPhysX();


	//////////////////////////////////////////////////////////////////////////
	// PhysX Global Getter
	//
	static MPhysX*					GetInstance(){ return m_pInitedGlobalPhysX; }


	//////////////////////////////////////////////////////////////////////////
	// Init / Destroy / Update
	//
	enum PHYSX_INIT_CODE			{ PIC_SUCCESS = 0, PIC_NOT_INSTALLED, PIC_FAILD };
	PHYSX_INIT_CODE					Init(float fScale = 10.f, const char* _pSzVisualRemoteDebuggerAddress = NULL, bool bUseCustomAllocator = false);
	void							Destroy();
	void							Update(float _fElapsed);
	float							GetCurrentElapsed(){ return m_fCurrentElapsed; }

	int								gAppData;
	void							connectToVRD();

	//////////////////////////////////////////////////////////////////////////
	// 씬 관련
	//
	void							SetSceneGravity(float _fGravity);
	void							SetSceneGravity( const MVector3& _rGravity );
	const MVector3&					GetSceneGravity(){ return m_vecSceneGravity; }


	//////////////////////////////////////////////////////////////////////////
	// MPhysX Object 관련
	//
	template< typename T > T*		CreatePxObject();
	void							RemovePxObject(MPhysXObject* pPxObject);


	//////////////////////////////////////////////////////////////////////////
	// MPhysX Shape 관련
	//
	MPhysXShape*					CreatePxShape(MPhysXShape::PHYSX_SHAPE_TYPE _eShapeType, const std::string& _strShapeResourceName);
	MPhysXShape*					CreatePxShape(NxShapeDesc* _pShapeDesc);
	MPhysXShape*					FindPxShape(const std::string& _strShapeResourceName);
	void							RemovePxShape(MPhysXShape* _pShape);
	void							RemovePxShape(const std::string& _strShapeResourceName);


	//////////////////////////////////////////////////////////////////////////
	// MPhysX Information XML 관련
	//
	bool							CreatePxInfoXML(const std::string& strInfoFileName, bool bReCreate);
	bool							IsCreatedPxInfoXML(const std::string& strInfoFileName);
	void							ClearPxInfoXML();

	template< typename T >
	inline MXmlElement*				GetPxInfoXML(const std::string& strFileName, const std::string& strContext);

	typedef							std::map< std::string, MXmlElement* > PX_INFO_XML;
	template< typename T >
	inline PX_INFO_XML*				GetPxInfoXMLAllContext(const std::string& strFileName);
						


	//////////////////////////////////////////////////////////////////////////
	// MPhysX 충돌그룹관리
	//
	int								AddCollisionGroup(const char* _pSzCollsionGroup, bool _bWithDefaultCollision, bool _bWithSelfGroupCollision, bool _bWithOtherGroupCollision);
	int								GetCollsionGroupId(const std::string& _rStrGroupName);
	int								GetDefaultDynamicObjectCollisionGroupId(bool bSelfGroupCollision);


	//////////////////////////////////////////////////////////////////////////
	// 피직스 LOD
	// Default일 경우 PxObject 가 있는 곳에서만 PO_STATIC_BODY 가 활성화된다.
	//
	enum PHYSX_LOD					{ PL_NONE = 0, PL_DEFAULT, PL_NO_STATIC };
	void							SetLOD(PHYSX_LOD _eLod){ m_eMPhysXLOD = _eLod; }
	PHYSX_LOD						GetLOD(){ return m_eMPhysXLOD; }


	//////////////////////////////////////////////////////////////////////////
	// Nx리소스 관련
	//
	NxPhysicsSDK*					GetNxSDK() { return m_pSDK; }
	NxScene*						GetNxScene() { return m_pScene; }
	NxCookingInterface*				GetNxCookingInterface() { return m_pCooking; }


	//////////////////////////////////////////////////////////////////////////
	// 에러코드
	//
	NxSDKCreateError				GetLastError() const { return m_eLastErrorCode; }


protected:

	//////////////////////////////////////////////////////////////////////////
	// Time Step 관련
	//
	enum E_ELAPSED_TIME_GROUP
	{
		E_1_PER_35 = 0,	// 기본값
		E_1_PER_65,
		E_1_PER_120,
		E_1_PER_180,

		E_ELAPSED_NUM
	};
	struct SElapsedCalc
	{
		float	m_fElapsedTime;
		int		m_nElapsedGroup;
	};

	SElapsedCalc					m_Elapsed[E_ELAPSED_NUM];
	float							m_fCurrentElapsed;
	float							m_fTimestep;
	void							SetTimeStep(float _fElapsed);
	void							SetTiming( float _fTimeStep, int _nIteration );


	//////////////////////////////////////////////////////////////////////////
	// Nx계열 리소스
	//
	NxPhysicsSDK*					m_pSDK;						// SDK
	NxScene*						m_pScene;					// Scene
	NxCookingInterface*				m_pCooking;					// Cooking

	NxRemoteDebugger*				m_pRemoteDebugger;			// VRD
	MPhysXUserAllocator*			m_pMyAllocator;				// Allocator


	//////////////////////////////////////////////////////////////////////////
	// MPhysXObject 계열
	//
	PHYSX_LOD						m_eMPhysXLOD;
	typedef std::list<MPhysXObject*> PX_OBJ_LIST;
	PX_OBJ_LIST						m_PxObjects[MPhysXObject::PO_TOTAL_NUM];
	std::vector< const MBox* >		m_vecActiveAreas;

	std::vector< NxActor* >			m_vecNxActorToRelease;		// 삭제할 액터
	MVector3						m_vecSceneGravity;			// MPhysX Gravity

	inline void						InitPxObject(MPhysXObject::PHYSX_OBJECT_TYPE _eType, MPhysXObject* pNewObject);
	void							ClearReleaseReservedNxActor();

	//////////////////////////////////////////////////////////////////////////
	// MPhysXShape
	//
	typedef std::map< std::string, MPhysXShape* > PX_SHAPE_MAP;
	PX_SHAPE_MAP					m_mapShapes;


	//////////////////////////////////////////////////////////////////////////
	// PxInfoXML 계열
	//
	typedef std::map< std::string, PX_INFO_XML> PX_INFO_OBJ;
	struct PX_INFO_FILE { PX_INFO_OBJ m_FileInfo; };
	typedef std::map< std::string, PX_INFO_FILE > PX_INFO;
	PX_INFO							m_mapPxInfo;

	//////////////////////////////////////////////////////////////////////////
	// 충돌그룹
	//
	struct SCollisionGroup
	{
		SCollisionGroup(int _nGroup, bool bWithDefault, bool bWithSelfGroup, bool bWithOtherGroup)
		{
			m_nGroupID = _nGroup;

			m_bColWithDefault = bWithDefault;
			m_bColWithSelfGroup = bWithSelfGroup;
			m_bColWithOtherGroup = bWithOtherGroup;
		}
		int					m_nGroupID;
		bool				m_bColWithDefault;
		bool				m_bColWithSelfGroup;
		bool				m_bColWithOtherGroup;
	};
	typedef							std::map< std::string, SCollisionGroup > PX_COL;
	PX_COL							m_mapCollisionGroups;
	int								m_nLastGroup;
	MPhysXCollisionReport*			m_pCollisionReport;

	//////////////////////////////////////////////////////////////////////////
	// 전역 접근 객체
	//
	static MPhysX*					m_pInitedGlobalPhysX;

	//////////////////////////////////////////////////////////////////////////
	// 에러 코드
	//
	NxSDKCreateError		m_eLastErrorCode;
};

//----------------------------------------------------------------------------------------------------
template< typename T > T* MPhysX::CreatePxObject()
{
	_ASSERT(m_pScene != NULL);

	T* pPxObject = new T;
	InitPxObject( (MPhysXObject::PHYSX_OBJECT_TYPE)MPhysXObject::PxType2Id<T>::INDEX, pPxObject);

	return pPxObject;
}
//----------------------------------------------------------------------------------------------------
template< typename T >
inline MXmlElement* MPhysX::GetPxInfoXML(const std::string& strFileName, const std::string& strContext)
{
	PX_INFO_XML* pPxInfoXml = GetPxInfoXMLAllContext<T>(strFileName);
	if (!pPxInfoXml)
		return NULL;

	PX_INFO_XML::iterator itr_xml = pPxInfoXml->find(strContext);
	if (itr_xml == pPxInfoXml->end())
		return NULL;

	return itr_xml->second;
}
//----------------------------------------------------------------------------------------------------
template< typename T >
inline MPhysX::PX_INFO_XML* MPhysX::GetPxInfoXMLAllContext(const std::string& strFileName)
{
	PX_INFO::iterator itr_info = m_mapPxInfo.find(strFileName);
	if(itr_info == m_mapPxInfo.end())
		return NULL;

	PX_INFO_OBJ& objInfo = itr_info->second.m_FileInfo;
	PX_INFO_OBJ::iterator itr_obj = objInfo.find(MPhysXObject::PxType2Id<T>::GetString());
	if(itr_obj == objInfo.end())
		return NULL;

	return &itr_obj->second;
}
//----------------------------------------------------------------------------------------------------
inline void MPhysX::InitPxObject(MPhysXObject::PHYSX_OBJECT_TYPE _eType, MPhysXObject* pNewObject)
{
	pNewObject->m_eObjectType = _eType;
	pNewObject->m_pPhysX = this;
	m_PxObjects[_eType].push_back(pNewObject);
}
//----------------------------------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
//
// MPhysXHelper
//
//////////////////////////////////////////////////////////////////////////
class MPhysXHelper
{
public:
	// Matrix 변환
	static void DecomposeMMatrix( const MMatrix& _rInputMMat, NxMat33& _rOrient, NxVec3& _rPosition)
	{
		MVector3 vScale;
		MVector3 vPos;
		MQuat qOrient;
		MMatrix matOrient;

		_rInputMMat.Decompose(vScale, vPos, qOrient);
		qOrient.SetMatrixRotation(matOrient);

		_rOrient.setColumn(0, NxVec3(matOrient._11, matOrient._12, matOrient._13));
		_rOrient.setColumn(1, NxVec3(matOrient._21, matOrient._22, matOrient._23));
		_rOrient.setColumn(2, NxVec3(matOrient._31, matOrient._32, matOrient._33));

		_rPosition.set(vPos.x, vPos.y, vPos.z);
	}

	// 두 백터의 각도 구해오기
	static float GetAngleByPoint(const NxVec3& lpos, const NxVec3& rpos)
	{
		NxVec3 rbPos = NxVec3(rpos.x, rpos.y, lpos.z);

		float fLPosDis = lpos.distance(rbPos);
		float fRPosDis = rpos.distance(rbPos);

		float fRadian = atan(fRPosDis / fLPosDis);

		return fRadian * 180 / 3.1414f;
	}

	template <bool bTurnOnAssert = true >
	class CUpdatableChecker
	{
	public:
		CUpdatableChecker(NxScene* pScene){ m_pScene = pScene; }
		bool checkData()
		{    
			NxU32 nbActors = m_pScene->getNbActors();
			NxActor** actors = m_pScene->getActors();
			while(nbActors--)
			{
				if (!checkInternalData(*actors++))
					return false;
			}

			return true;
		}

	private:
		bool checkInternalData(NxActor* actor)
		{
			NxVec3 v, p, w;
			v = actor->getLinearVelocity();
			p = actor->getGlobalPosition();
			w = actor->getAngularVelocity();
			if (!v.isFinite() || !w.isFinite() || !p.isFinite() )
			{
				if (bTurnOnAssert)
				{
					_ASSERT(0);
				}
				return false;
			}
			return true;
		}

		NxScene* m_pScene;
	};

};

} // namespace physx
