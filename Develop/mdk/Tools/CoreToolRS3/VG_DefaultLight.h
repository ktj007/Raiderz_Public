#pragma once

#include "VL_ListenableView.h"

namespace rs3 {
	class RDirectionalLightSceneNode;
}


class CORETOOL_API VG_DefaultLight : public CListenableViewImpl_General<VG_DefaultLight>
{
public:
	VG_DefaultLight(void);
	virtual ~VG_DefaultLight(void);

	// Listenable View
	virtual void		Listen(CoreMessage& _message);			///< Listen
	virtual const char*	GetViewName(){ return "DefaultLight"; }	///< View Name

	bool				IsEnable();
	void				ToggleLight();
	bool				IsLightToCamera();
	void				ToggleLightToCamera();
	rs3::RDirectionalLightSceneNode*	GetDefaultLight();

protected:
	void				AddDefaultLight();
	void				RemoveDefaultLight();
	void				OnLogicRender();
	void				OnLogicDelete( CLogicObject* pLogicObject );

	bool								m_bLightToCamera;
	rs3::RDirectionalLightSceneNode*	m_pDefaultLight;
};

//////////////////////////////////////////////////////////////////////////
inline bool VG_DefaultLight::IsEnable()			{ return m_pDefaultLight != NULL; }
inline bool VG_DefaultLight::IsLightToCamera()	{ return m_bLightToCamera; }
inline rs3::RDirectionalLightSceneNode* VG_DefaultLight::GetDefaultLight() { return m_pDefaultLight; }