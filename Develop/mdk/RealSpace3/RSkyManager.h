#pragma once

#include "RTypes.h"
#include "RPType.h"
#include "RSceneNodeID.h"
#include "RSceneNode.h"
#include <vector>

namespace rs3 {

class RSceneSky;

struct RSkyInfo
{
	int nOrder;
	float fUVAniSpeed;
	float fFogFactor;
	DWORD dwColor;

	std::string strFilename;
	RSceneSky *pSky;
	int skyRefCount;

	RSkyInfo()
		: nOrder(-1)
		, fFogFactor(1.0f)
		, fUVAniSpeed(1.0f)
		, dwColor(0xffffffff)
		, pSky(NULL)
		, skyRefCount(0)
	{
	}
};
typedef std::vector<RSkyInfo> SKYARRAY;

class RS_API RSkyManager : public RSceneNode 
{
public:
	enum eRenderOrder
	{
		eRENDERORDER_FIRST,
		eRENDERORDER_SECOND,
		eRENDERORDER_NUM
	};

private:
	DECLARE_NODE_ID_STRING(RSID_SKY);

	static const char* ELEMENT_FILENAME;	
	static const char* ELEMENT_SPEED;
	static const char* ELEMENT_FOGFACTOR;
	static const char* ELEMENT_COLOR;

	SKYARRAY *m_pSkyArray;

	virtual void OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod) override;
	virtual void OnRenderGStage( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod) override;
	virtual void OnUpdate();

	void RenderSky( RCameraSceneNode* pCurrentCamera, RRENDERINGMETHOD renderMothod);

	void ReorderNumber();
	void Destroy();

	RVector4		m_vMultiplyColor;
	eRenderOrder	m_nRenderOrder;
protected:
	static const UINT LAYER_NUM = 1;

public:
	RSkyManager();
	virtual ~RSkyManager();

	void SetAnimationSpeed();
	void SetMultiplyColor(RVector4 &vec4);
	const RVector4& GetMutiplyColor();

	bool CreateFromXML(MXmlElement &element, bool bBackgroundCreation);
	
	bool CreateScenes(bool bBackgroundCreation);
	void ReleaseScenes();

	// 모두 로드 되었으면 LOADED, 그렇지 않으면 EMPTY, 하나라도 실패했으면 FAILED ( Scene & Material )
	RRSTATE GetScenesState();

	void SaveToXML(MXmlElement &element);

	bool AddSky(int nLayer, RSkyInfo &info);
	void DelSky(int nLayer, int nIndex);

	int FindSky(int nLayer, RSceneSky *pSkyNode);
	RSkyInfo* GetSkyInfo(int nLayer, int nIndex); 

	void UpdateSkyLayer();
	SKYARRAY* GetArray(int nLayer);
	int GetLayerCount();

	eRenderOrder GetRenderOrder() const		{ return m_nRenderOrder; }
	void SetRenderOrder( eRenderOrder val )	{ m_nRenderOrder = val; }
};

inline int RSkyManager::GetLayerCount() { return m_pSkyArray->size(); }
};