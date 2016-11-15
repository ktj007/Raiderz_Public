#pragma once

#include "MPhysXObject.h"
#include "MPxObjStaticSubActorBuilder.h"

namespace physx {

//////////////////////////////////////////////////////////////////////////
//
// MPxObjStatic
//  - Static Object (정적인 SubActors를 가지는 오브젝트)
//  - 한번 만들어지면 이동하지 않는다.
//  - 매 틱마다 ActivateSubActor을 MBox 영역을 가지고 수행해주어야 한다.
//    (정해진 영역만 활성화 하는 일종의 컬링이다)
//
//////////////////////////////////////////////////////////////////////////

class MPxObjStatic : public MPhysXObject
{

public:

	MPxObjStatic();
	virtual ~MPxObjStatic();


	//////////////////////////////////////////////////////////////////////////
	// Static SubActor 추가하기
	//
	bool AddStaticSubActorsFromBuilder(MPxObjStaticSubActorBuilder& _rStaticSubActorBuilder);


	//////////////////////////////////////////////////////////////////////////
	// Static SubActor 활성화 하기
	//
	void ActivateStaticSubActorsByAreaList(const std::vector< const MBox* >& _rAreaList);
	void ActivateStaticSubActorsAll();


	//////////////////////////////////////////////////////////////////////////
	//
	// 사용되지 않는 Virtual 함수 처리
	//
	//////////////////////////////////////////////////////////////////////////

	virtual void NotifyResultOnSimulated()
	{
		_ASSERT(0);	// Rigid Static Object는 이 기능을 사용하지 않으므로 불리면 안된다.
	}

	virtual const MBox* GetAreaBox()
	{
		_ASSERT(0); // Rigid Static Object는 이 기능을 사용하지 않으므로 불리면 안된다.
		return NULL;
	}


protected:

	virtual void OnDoPreUpdateSubActorTransform() { _ASSERT(0); } //안불려야함
	virtual void OnDoPostUpdateSubActorTransform(){ _ASSERT(0); } //안불려야함


private:

	//////////////////////////////////////////////////////////////////////////
	// Sector를 가지는 오브젝트를 위한 정보
	//  - 오브젝트가 너무 크면 속도를 위해 섹터로 쪼갠다.
	//  - 그리드 / 큐브를 병행해서 사용할수 있다.
	//
	struct SECTOR_INFO
	{
		SECTOR_INFO()
			: m_NXInfo(PS_STATIC_TRIANGLE, MMatrix::IDENTITY, 0.0f, NULL)
			, m_nRefCount(0)
			, m_pVertices(NULL), m_pFaces(NULL)
			, m_bHeightField(false)
		{
		}

		~SECTOR_INFO()
		{
			SAFE_DELETE(m_pVertices);
			SAFE_DELETE(m_pFaces);
		}

		int						m_nRefCount;
		MBox					m_PieceBox;

		std::vector<NxVec3>*	m_pVertices;
		std::vector<NxU32>*		m_pFaces;
		bool					m_bHeightField;

		NX_INFO					m_NXInfo;
	};

	int							m_nUsingSquareGridMeshCount;
	int							m_nUsingCubeGridMeshCount;

	// cube sector
	int							m_nCubeXCount;
	int							m_nCubeYCount;
	int							m_nCubeZCount;

	float						m_fCubeXStart;
	float						m_fCubeYStart;
	float						m_fCubeZStart;

	float						m_fCubeXInterval;
	float						m_fCubeYInterval;
	float						m_fCubeZInterval;

	// square sector
	int							m_nSquareDivision;
	float						m_fSquareXInterval;
	float						m_fSquareYInterval;

	// sector buffer
	SECTOR_INFO**				m_pArraySectorCubeGrid;
	SECTOR_INFO**				m_pArraySectorSquareGrid;

	std::vector<SECTOR_INFO*>	m_vecSectorBuff_0;
	std::vector<SECTOR_INFO*>	m_vecSectorBuff_1;
	std::vector<SECTOR_INFO*>*	m_pSectorBuff_Old;
	std::vector<SECTOR_INFO*>*	m_pSectorBuff_Current;
	std::vector<SECTOR_INFO*>*	m_pSectorBuffTemp;

	void						CreateSectorActor(SECTOR_INFO* pSector, std::vector<SECTOR_INFO*>* pVecSectorBufferToAdd);
};

} // namespace physx