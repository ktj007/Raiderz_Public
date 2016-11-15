#pragma once

#include "RMeshMgr.h"
#include "RResourceManager.h"
#include "RMesh.h"
#include "RResource.h"
using namespace rs3;

namespace rs3 {
	class RWeatherEffectSet;
}

class XResourceManager;

/// rs3 어댑터 및 엔진과 의존성이 높은 것들의 퍼사드 클래스
class XEngine
{
protected:
	rs3::RDevice*				m_pDevice;
	rs3::RWeatherEffectSet*		m_pWeatherEffectSet;
	XResourceManager*			m_pResourceManager;

	vector<RMesh*>				m_vecSharedMeshes;
public:
	XEngine();
	virtual ~XEngine();
	void Init(RDevice* pDevice, XResourceManager* pResourceManager);

	virtual void ReleaseMesh( RMesh* pMesh );
	virtual RMesh* CreateMesh( const TCHAR* szMeshName );

	virtual bool InitSharedResource(bool bPreloadPlayersMesh);
	virtual void DestroySharedResource();

	virtual RTexture* CreateTexture(const TCHAR *fileName, const RTEXTUREFILTERTYPE filter = RTF_LINEAR,
		DWORD flags = RCT_AUTOGENMIPMAP, RFORMAT pf = RFMT_NONE, bool bBackground = false, RResource::LoadingController* pController = NULL);
	virtual RTexture* CreateTexture(int nWidth, int nHeight, RFORMAT pf , const RTEXTUREFILTERTYPE filter = RTF_LINEAR, DWORD flags = 0, void *pixels = NULL, const TCHAR* szName = NULL);
	virtual void DeleteTexture(RTexture* pTexture);

	RWeatherEffectSet*		GetWeatherEffectSet();
};