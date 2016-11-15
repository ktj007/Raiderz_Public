#pragma once
#include "M_LogicObject.h"
#include "M_EntityObject.h"

#include "../../Game/XCutSceneObjectController.h"

#include "RRenderHelper.h"


const rs3::RMatrix MAT_PI_RotForCorn(
									 1,  0,  0,  0,
									 0, -1,  0,  0,
									 0,  0, -1,  0,
									 0,  0,  0,  1
									 );

//////////////////////////////////////////////////////////////////////////

class XCutSceneObjectControllerEffect;
class XCutSceneObjectControllerMove;

class CCutSceneLogicObjectBase : public CLogicObject
{
protected:
	class CLogicObjectKey : public CLogicObject
	{
	public:
		CLogicObjectKey()
		{
			m_bSelected = false;
			m_bEditable = true;

			m_bLocationChanged = false;
		}

		virtual ~CLogicObjectKey()
		{
		}

		virtual void OnSelected_(bool bSelected)
		{
			m_bSelected = bSelected;
		}

		virtual void OnSetPos_(const rs3::RVector &_refUpdatePos)
		{
			CLogicObject::OnSetPos_(_refUpdatePos);
			m_bLocationChanged = true;
		}

		virtual void OnSetDirUp_(const rs3::RVector &_refUpdateDir, const rs3::RVector &_refUpdateUp)
		{
			CLogicObject::OnSetDirUp_(_refUpdateDir, _refUpdateUp);
			m_bLocationChanged = true;
		}

		void SetEnableUp(bool bEnable)
		{
			if (!bEnable)
			{
				m_pdUpX_ = NULL;
				m_pdUpY_ = NULL;
				m_pdUpZ_ = NULL;
			}
		}

		void SetAABB(const rs3::RBoundingBox& box){ m_aabb_ = box; }

		bool m_bSelected;
		bool m_bLocationChanged;

	protected:
		virtual void			OnRender() {}
		virtual CLogicObject*	OnPickForSelection_(float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to){ return NULL; }
		virtual void			OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum){ return; }
	};

	std::vector<CLogicObjectKey*>		m_vMoveKey;
	CLogicObjectKey*					m_pCurrentSelected;
	rs3::RBoundingBox					m_box;

	bool								m_bSelected;
	float								m_fHeight;
	bool								m_bUpEditable;

protected:
	virtual void						OnRender() {}

	virtual CLogicObject*				OnPickForSelection_(float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to);
	virtual void						OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum);
	virtual void						OnSelected_(bool bSelected);

public:
	CCutSceneLogicObjectBase();
	virtual ~CCutSceneLogicObjectBase();

	void								SetBaseInfo(float _fHeight, bool _bEditableUp);
	virtual	CLogicObject*				AddKey(SElementAtXML* pProerty, float _fTime, rs3::RVector3 _rPos, rs3::RVector3 _rDir = rs3::RVector3::AXISY, rs3::RVector3 _rUp = rs3::RVector3::AXISZ);

	bool								IsMoveKey(CLogicObject* _pLogicObject){ return (NULL != dynamic_cast<CLogicObjectKey*>(_pLogicObject)); }
	int									GetMoveKeyIdx(CLogicObject* _pLogicObject);
};