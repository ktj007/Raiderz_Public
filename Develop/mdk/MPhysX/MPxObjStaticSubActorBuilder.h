#pragma once

#include "MPhysXDef.h"

namespace physx {

//////////////////////////////////////////////////////////////////////////
// Static Shape Builder
//
class MPxObjStaticSubActorBuilder
{
public:
	MPxObjStaticSubActorBuilder() : m_bUseCubeSector(false), m_bUseSquareSector(false) {}
	virtual ~MPxObjStaticSubActorBuilder(){}


	//////////////////////////////////////////////////////////////////////////
	// Sector 정보
	//
	bool GetCubeSectorInfo( MBox& _rTotalBound, float& _fPieceCubeSize )
	{
		if (m_bUseCubeSector == false)
			return false;

		_rTotalBound = m_TotalBound;
		_fPieceCubeSize = m_fPieceCubeSize;
		return true;
	}

	void SetCubeSectorInfo( const MBox& _rTotalBound, float _fPieceCubeSize )
	{
		m_TotalBound = _rTotalBound;
		m_fPieceCubeSize = _fPieceCubeSize;
	}

	bool GetSquareSectorInfo( int& _nDivisionCount, int& _nRowCount, int& _nColCount, float& _fRowScale, float& _fColScale )
	{
		if (m_bUseSquareSector == false)
			return false;

		_nDivisionCount = m_nDivisionCount;
		_nRowCount = m_nRowCount;
		_nColCount = m_nColCount;
		_fRowScale = m_fRowScale;
		_fColScale = m_fColScale;
		return true;
	}

	void SetSquareSectorInfo( int _nDivisionCount, int _nRowCount, int _nColCount, float _fRowScale, float _fColScale )
	{
		m_nDivisionCount = _nDivisionCount;
		m_nRowCount = _nRowCount;
		m_nColCount = _nColCount;
		m_fRowScale = _fRowScale;
		m_fColScale = _fColScale;
	}


	//////////////////////////////////////////////////////////////////////////
	// Sector Mesh 정보
	//
	void GetCubeSectorMesh(std::vector<NxVec3>& _refVertices, std::vector<NxU32>& _refFaces, const MBox& _refPieceAABB)
	{
		_ASSERT( m_bUseCubeSector == true );
		OnGetCubeSectorMesh(_refVertices, _refFaces, _refPieceAABB);
	}

	void GetSquareSectorMesh(std::vector<NxVec3>& _refVertices, int x1, int y1, int x2, int y2)
	{
		_ASSERT( m_bUseSquareSector == true );
		OnGetSquareSectorMesh(_refVertices, x1, y1, x2, y2);
	}

protected:
	virtual void OnGetCubeSectorMesh(std::vector<NxVec3>& _refVertices, std::vector<NxU32>& _refFaces, const MBox& _refPieceAABB){}
	virtual void OnGetSquareSectorMesh(std::vector<NxVec3>& _refVertices, int x1, int y1, int x2, int y2){}

	bool				m_bUseCubeSector;
	bool				m_bUseSquareSector;

private:
	MBox				m_TotalBound;
	float				m_fPieceCubeSize;

	int					m_nDivisionCount;
	int					m_nRowCount;
	int					m_nColCount;
	float				m_fRowScale;
	float				m_fColScale;
};


} // namespace physx