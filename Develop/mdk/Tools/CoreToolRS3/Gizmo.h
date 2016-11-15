#pragma once

namespace rs3{
	class RCameraSceneNode;
	class RFont;
}

class GizmoComponent;

class Gizmo
{
public:
	Gizmo(CControlMediator* pControlMediator_);
	virtual ~Gizmo(void);

	void			SetTransform(rs3::RMatrix& matTM);

	AXIS_STATE		GetGizmoState();
	float			GetSizeFactor();
	rs3::RVector	GetPosition();

	void			SetEnable(bool bEnable);
	bool			GetEnable();

	void			UpdateStates();
	void			Render();

	rs3::RCameraSceneNode* m_pCamera;
	CControlMediator* m_pControlMediator_;
	rs3::RDeviceD3D* m_pDeviceD3D_;
	rs3::RFont*		m_pFont;
	rs3::RMatrix	m_matTM;
	AXIS_STATE		m_state;

	vector<GizmoComponent*>	m_vecComponents;
};

inline void Gizmo::SetTransform(rs3::RMatrix& matTM) { m_matTM = matTM; }

inline AXIS_STATE	Gizmo::GetGizmoState() { return m_state; }
inline rs3::RVector	Gizmo::GetPosition() { return rs3::RVector(m_matTM._41,m_matTM._42,m_matTM._43); }

class CControlMediatorHelperGizmo_Position : public Gizmo
{
public:
	CControlMediatorHelperGizmo_Position( CControlMediator* pControlMediator_ );
};

class CControlMediatorHelperGizmo_Rotation : public Gizmo
{
public:
	CControlMediatorHelperGizmo_Rotation( CControlMediator* pControlMediator_ );
};