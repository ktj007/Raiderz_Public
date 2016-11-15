#include "stdafx.h"
#include "WS_CutSceneLogicObjectBase.h"
#include "../../Game/XCutSceneObjectControllerMove.h"

CCutSceneLogicObjectBase::CCutSceneLogicObjectBase()
{
	m_bSelected = false;
	m_bEditable = true;
	m_bUpEditable = false;

	m_pCurrentSelected = NULL;

	m_fHeight = 0.0f;
	m_box.vmax.Set(10.f, 10.f, 10.f);
	m_box.vmin.Set(-10.f,-10.f,-10.f);
}

CCutSceneLogicObjectBase::~CCutSceneLogicObjectBase()
{
	DestroyLogicObject();
	for (std::vector<CLogicObjectKey*>::iterator itr = m_vMoveKey.begin(); itr != m_vMoveKey.end(); ++itr)
	{
		CLogicObjectKey* pMoveKey = *itr;
		pMoveKey->DestroyLogicObject();
		delete pMoveKey;
	}
}

CLogicObject* CCutSceneLogicObjectBase::AddKey( SElementAtXML* pProerty, float _fTime, rs3::RVector3 _rPos, rs3::RVector3 _rDir /*= rs3::RVector3::AXISY*/, rs3::RVector3 _rUp /*= rs3::RVector3::AXISZ*/ )
{
	CLogicObjectKey* pMoveKey = new CLogicObjectKey;
	pMoveKey->LoadPropertyFromXML(pProerty);
	pMoveKey->SetPosition(_rPos);
	pMoveKey->SetDirectionUp(_rDir, _rUp);
	pMoveKey->SetEnableUp(m_bUpEditable);
	pMoveKey->SetRootElementAttributeValue("time", _fTime);

	pMoveKey->m_bLocationChanged = false;
	m_vMoveKey.push_back(pMoveKey);
	return pMoveKey;
}

void CCutSceneLogicObjectBase::OnSelected_( bool bSelected )
{
	m_bSelected = bSelected;
}

void CCutSceneLogicObjectBase::SetBaseInfo( float _fHeight, bool _bUpEditable )
{
	m_fHeight = _fHeight;
	m_bUpEditable = _bUpEditable;
}

CLogicObject* CCutSceneLogicObjectBase::OnPickForSelection_( float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to )
{
	using namespace rs3;

	CLogicObject* pPickLogicObject = NULL;
	size_t count = m_vMoveKey.size();
	RVector3 vPos, vDir;
	RMatrix mat;
	RBoundingBox box;
	for(size_t i = 0; i < count; ++i)
	{
		CLogicObjectKey* pMoveKeyObj = m_vMoveKey[i];
		pMoveKeyObj->GetPosition(vPos);
		pMoveKeyObj->GetDirection(vDir);

		mat.SetLocalMatrix(vPos, vDir, RVector3::AXISZ);
		TransformBox(&box, m_box, mat);
		pMoveKeyObj->SetAABB(box);

		if (box.ContainsLineSegment(origin, to))
		{
			float fLSq = (origin - box.GetCenter()).LengthSq();
			if ( fLSq < _rPrevPickedSqLength )
			{
				pPickLogicObject = pMoveKeyObj;
				_rPrevPickedSqLength = fLSq;
			}
		}
	}

	return pPickLogicObject;
}

void CCutSceneLogicObjectBase::OnPickForSelection_( std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum )
{
	using namespace rs3;

	size_t count = m_vMoveKey.size();
	RVector3 vPos, vDir;
	RMatrix mat;
	RBoundingBox box;
	for(size_t i = 0; i < count; ++i)
	{
		CLogicObjectKey* pMoveKeyObj = m_vMoveKey[i];
		pMoveKeyObj->GetPosition(vPos);
		pMoveKeyObj->GetDirection(vDir);

		mat.SetLocalMatrix(vPos, vDir, RVector3::AXISZ);
		TransformBox(&box, m_box, mat);
		pMoveKeyObj->SetAABB(box);

		if(_refFrustum.TestBox(box))
		{
			_refVecLogicObject.push_back(pMoveKeyObj);
		}
	}
}

int CCutSceneLogicObjectBase::GetMoveKeyIdx( CLogicObject* _pLogicObject )
{
	CLogicObjectKey* pMoveKey = (CLogicObjectKey*)_pLogicObject;
	int cnt = (int)m_vMoveKey.size();
	for (int i = 0; i < cnt; ++i)
	{
		if (m_vMoveKey[i] == pMoveKey)
			return i;
	}

	return -1;
}