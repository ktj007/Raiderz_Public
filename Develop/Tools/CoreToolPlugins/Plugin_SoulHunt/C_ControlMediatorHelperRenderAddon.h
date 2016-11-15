#pragma once

#include "C_ControlMediatorHelperRender.h"

namespace rs3
{
	class RDeviceD3D;
	class RFont;
}

class CControlMediator;

class CORETOOL_SOULHUNT_API CControlMediatorHelperObjectRendererImpl : public CControlMediatorHelperObjectRenderer
{
public:
	CControlMediatorHelperObjectRendererImpl();
	virtual ~CControlMediatorHelperObjectRendererImpl(){}

	virtual void OnInitDevice();
	virtual void OnDestroyDevice();

	virtual void OnRenderNavigationMesh();

	virtual void OnRenderObject()
	{
		// Entity Object Rendering ( black is invalid entity )
		RenderHelper_Spawn();
		RenderHelper_Marker();
		RenderHelper_Sensor();
		RenderHelper_Sound();
		RenderHelper_Area();
		RenderHelper_Room();
	}

	virtual void OnRenderSelection();
	virtual void OnRenderObjectTextInfo();

private:
	void RenderHelper_Spawn();
	void RenderHelper_Marker();
	void RenderHelper_Sensor();
	void RenderHelper_Sound();
	void RenderHelper_Area();
	void RenderHelper_Room();
	//랜더링시에 헬퍼 객체를 이동시키는 matrix
	rs3::RMatrix			m_matTranslation_;
	const rs3::RMatrix		m_mat_PI_RotForCorn_;
	rs3::RTexture*			m_pSoundIcon;
	rs3::RTexture*			m_pSensorIcon;

	struct Draw_Icon_Data 
	{
		rs3::RVector position;
		float fSize;
	};

	void Render_DrawIcon(rs3::RDevice *pDevice, vector<Draw_Icon_Data>& vecPositions, rs3::RTexture *pTexture, DWORD dwColor);
};
