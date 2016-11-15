#pragma once

#include "RPreDefine.h"
#include "RResource.h"
#include "RMaterial.h"

namespace rs3 {

class RMaterialResourceMgr;

const int MAX_TEXTURE_LAYER = 6;

class RS_API RMaterialResource : public RResource
{
public:

	RMaterialResource(const char* _pSzResourceName, RSceneManager* pSceneManager);
	virtual ~RMaterialResource();
	DECLARE_USING_RES_MGR(RMaterialResourceMgr);

	RSceneManager*				m_pSceneManager;

	vector<RTexture*>	m_textures[MAX_TEXTURE_LAYER];	///< 로딩중인 텍스쳐들
	RMaterial*			m_pMaterial;

	int					m_nUsingSystemMemorySize;
	int					m_nUsingVideoMemorySize;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 매트리얼 리소스는 fill을 사용하지 않는다.
	//
	bool							Open(MXmlElement *pElement, bool bBackground);
	bool							CreateAsDefaultMaterial();

	void							Destroy();

	int			GetUsingSystemMemory()	{ return m_nUsingVideoMemorySize; }
	int			GetUsingVideoMemory()	{ return m_nUsingSystemMemorySize; }

//	virtual void				SetLoadingPriority(int nPriority);

protected:
	virtual bool		Fill() override;
	virtual LOAD_RESULT Load() override;
};

}	//namespace rs3