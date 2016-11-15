#pragma once
#include "wx/panel.h"

#include "RLib.h"
#include "RRenderingCallbackInterface.h"

using namespace rs3;

namespace rs3
{
	class RDeviceD3D;
	class RSceneNode;
	class RActor;
	class RActorNode;
	class REffectSceneNode;
}

namespace EffectTool
{

class TSceneManager;
class TCamera;

class MainPanel : public wxPanel, RRenderingCallbackInterface
{
public:
	MainPanel(wxWindow* parent, wxWindowID winid);
	virtual ~MainPanel();

	void Init();

	virtual void DeviceSetting() override;
	virtual void PreOrderRender() override;
	virtual void PostOrderRender() override;

	bool CreateEffectSceneNode(const wxString& fileName);
	void CreateDefaultEffectSceneNode();
	void DeleteEffectSceneNode();

private:
	void OnIdle(wxIdleEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnMouseEvent(wxMouseEvent& event);

	void OnCreateEffectSceneNode();

	void InitDevice();
	void DestroyDevice();

	void UpdateKeys();
	void UpdateLightToCamera();
	void Update();
	void Render();

	bool CreateActor(const wxString& fileName);
	bool CreateDefaultActor();
	void DeleteActor();

	void AttachEffect(REffectSceneNode* pEffectSceneNode, RSceneNode* pSceneNode, RActorNode* pActorNode);

private:
	RDeviceD3D*			m_pDevice;
	TSceneManager*		m_pSceneManager;
	TCamera*			m_pCamera;

	// 액터
	wxString			m_actorFileName;
	RActor*				m_pActor;

	wxString			m_selectedActorNodeName;
	RActorNode*			m_pSelectedActorNode;

	wxString			m_selectedActorAnimationName;

	// 이펙트
	wxString			m_effectFileName; ///< 편집중인 파일 이름
	REffectSceneNode*	m_pEffectSceneNode;
	RSceneNode*			m_pSelectedChild;

	// 뷰 옵션
	bool				m_bDrawHelperGrid;
	bool				m_bDrawHelperAxis;
	bool				m_bLightToCameraDir;

	DECLARE_EVENT_TABLE()
};

}