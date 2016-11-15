#pragma once

#include "VL_ListenableView.h"

class Gizmo;

class CORETOOL_API VG_Gizmo : public CListenableViewImpl_General<VG_Gizmo>
{
public:
	VG_Gizmo(void);
	virtual ~VG_Gizmo(void);

	// Listenable View
	virtual void			Listen(CoreMessage& _message);		///< Listen
	virtual const char*		GetViewName(){ return "Gizmo"; }	///< View Name

	AXIS_STATE				GetGizmoState();
	rs3::RVector			GetGizmoPosition();

	void					SetLockState(bool bLock);
	bool					GetStateLocked();

	bool					GetEnable();
	Gizmo*					GetCurrentGizmo();

protected:
	
	void					SetCurrentGizmo(Gizmo* pGizmo);

	void					OnInitialize();
	void					OnLogicObjectRender();
	void					OnLogicObjectSelected();
	void					OnWBStateChanged();

	bool					m_bEnabled;			///< 활성화 여부
	bool					m_bStateLocked;		///< state 변경 lock
	Gizmo*					m_pGizmoPosition_;	///< position
	Gizmo*					m_pGizmoRotation_;	///< rotation
	Gizmo*					m_pCurrentGizmo_;	///< 현재 사용중인 기즈모
};

//////////////////////////////////////////////////////////////////////////
inline Gizmo*	VG_Gizmo::GetCurrentGizmo()				{ return m_pCurrentGizmo_; }
inline void VG_Gizmo::SetCurrentGizmo(Gizmo* pGizmo)	{ m_pCurrentGizmo_ = pGizmo; }

inline bool VG_Gizmo::GetStateLocked() { return m_bStateLocked; }
inline void VG_Gizmo::SetLockState(bool bLock) { m_bStateLocked = bLock; }

inline bool VG_Gizmo::GetEnable() { return m_bEnabled; }