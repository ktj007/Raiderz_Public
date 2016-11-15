#pragma once

#include "RDeviceD3D.h"
#include "RFont.h"

class CControlMediator;

namespace rs3
{
	class RToolSceneManager;
	class RTreeSystem;
	class RCameraSceneNode;
	class RSceneNode;
};

class CControlPreviewSceneManager
{
private:
	rs3::RTexture* m_hRenderTarget;
	rs3::RTexture* m_hDepthBuffer;

	rs3::RToolSceneManager	*m_pPreviewSceneManager;
	rs3::RCameraSceneNode	*m_pPreviewCamera;
	rs3::RDeviceD3D			*m_pDevice;

	CControlMediator*		m_pControlMediator;

public:
	bool Init(CControlMediator* pControlMediator, rs3::RDeviceD3D *pDevice, rs3::RToolSceneManager *pSceneManager);
	void Release();

	rs3::RSceneNode* LoadSceneNode(const char *szFilename, const char *szNodeName);
	void UpdateRender(rs3::RSceneNode *pNode);

	void OnUpdate();

	rs3::RTexture* GetRenderTexture();
	
	static CControlPreviewSceneManager* GetInstance()
	{
		if ( m_pPtr == NULL )
			m_pPtr = new CControlPreviewSceneManager();
		
		return m_pPtr;
	}
private:
	static CControlPreviewSceneManager *m_pPtr;
	
	CControlPreviewSceneManager();
	virtual ~CControlPreviewSceneManager();
};

inline rs3::RTexture* CControlPreviewSceneManager::GetRenderTexture()
{
	return m_hRenderTarget;
}
