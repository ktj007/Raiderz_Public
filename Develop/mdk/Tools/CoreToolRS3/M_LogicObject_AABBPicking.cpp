#include "StdAfx.h"
#include "M_LogicObject_AABBPicking.h"
#include "C_ControlSceneProperty.h"

//////////////////////////////////////////////////////////////////////////
// CLogicObjectRenderer_SceneNode

CLogicObject_AABBPicking::CLogicObject_AABBPicking()
{
	m_bEditable = true;
}

CLogicObject*		CLogicObject_AABBPicking::OnPickForSelection_(float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to)
{
	if( GetAABB().ContainsLineSegment(origin, to) )
	{
		float fLSq = (origin - GetAABB().GetCenter() ).LengthSq();
		if ( fLSq < _rPrevPickedSqLength )
		{
			_rPrevPickedSqLength = fLSq;
			return this;
		}
	}

	return NULL;
}

void				CLogicObject_AABBPicking::OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum)
{
	if( _refFrustum.TestBox( GetAABB()) )
	{
		_refVecLogicObject.push_back( this );
	}
}

void CLogicObject_AABBPicking::SetAABB( rs3::RBoundingBox& aabb )
{
	m_aabb_ = aabb;
}
