#include "RSpeedGrass.h"

#include "RShaderConstant.h"

// 16비트 인덱스 버퍼를 사용 하고, 풀 개당 버텍스가 4개가 사용.
#define GRASS_LIMIT_PER_REGION		(16384)	


namespace rs3
{

class RShaderComposite;
class RShaderFX;
class RTechnique;
class RSpeedGrassShader;

class RSpeedGrassGPUInstance : public RSpeedGrass
{

protected:
	RSpeedGrassShader*		m_pSpeedGrassShader;
	HVERTEXFORMAT			m_hGrassVertex;

public:
	RSpeedGrassGPUInstance(RDevice* dev);
	virtual ~RSpeedGrassGPUInstance();

	virtual bool Create();
	virtual void Destroy();

	virtual void SetBasicRenderState( RGrassSystem *pGrassSystem, RCameraSceneNode* pCamera);
	virtual void ResetRenderState();
	virtual int Render( RShaderFX* pShader, RTechnique* pTechnique);
	virtual int RenderTillingEffect();
	virtual int RenderGStage(  RRENDERINGMETHOD method);
	virtual int RenderMStage();
	virtual void RenderRaw();
	virtual void UpdateDXBuffers( RTerrain* pTerrain);

private:
	
};

};