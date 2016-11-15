#pragma once

namespace rs3
{

class RActorNode;
class RMaterial;
class RShaderFX;


class RActorNodeRenderStrategy
{
public:
	virtual ~RActorNodeRenderStrategy() {}

	virtual void SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial);
	virtual void SetNodeRenderState( RCameraSceneNode* pCurrentCamera, RActorNode* pActorNode, RMaterial* pMaterial );
	virtual void ResetNodeRenderState() {}
	virtual void Begin() {}
	virtual void End();

	virtual bool IsRender(RCameraSceneNode* pCamera, RActorNode* pActorNode, int nMtrIndex) { return true; }
	virtual RShaderFX* GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing) { return NULL; }

	virtual void ApplyDistortion(RCameraSceneNode* pCamera, RActorNode* pActorNode, int nMtrIndex) {}

	virtual bool CheckInstancing(RActorNode* pActorNode, RMaterial* pMaterial);

protected:
	RShaderFX* GetShaderForStage(RActorNode* pActorNode, int nMtrIndex, bool bMstage);

	void SetDiffuseStatePerNode(RActorNode* pActorNode, RMaterial* pMaterial);
};





class RActorNodeRenderStrategyAlphaDepthOnly : public RActorNodeRenderStrategy
{
public:
	virtual void SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial) override;
	virtual void Begin() override;
	virtual void End() override;

	virtual RShaderFX* GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing) override;
};


class RActorNodeRenderStrategyAlpha2PassMStage : public RActorNodeRenderStrategy
{
public:
	virtual void SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial) override;
	virtual void Begin() override;
	virtual void End() override;

	virtual RShaderFX* GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing) override;

	virtual void ApplyDistortion(RCameraSceneNode* pCamera, RActorNode* pActorNode, int nMtrIndex) override;
};



class RActorNodeRenderStrategyShadowMap : public RActorNodeRenderStrategy
{
public:
	RActorNodeRenderStrategyShadowMap() :m_pShadowStrategy(NULL) {}
	virtual void SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial) override;
	virtual void SetNodeRenderState(  RCameraSceneNode* pCurrentCamera, RActorNode* pActorNode, RMaterial* pMaterial ) override;
	virtual void Begin() override;

	virtual bool IsRender(RCameraSceneNode* pCamera, RActorNode* pActorNode, int nMtrIndex);
	virtual RShaderFX* GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing) override;

	virtual bool CheckInstancing(RActorNode* pActorNode, RMaterial* pMaterial) override { return pActorNode->CanInstancing(); }

	void SetShadowStrategy(RShadowStrategy* pShadowStrategy) { m_pShadowStrategy=pShadowStrategy; }

protected:
	virtual bool IsRenderReceiver(bool bReceiver);

private:
	RShadowStrategy* m_pShadowStrategy;
};



class RActorNodeRenderStrategyProjectionShadowMap : public RActorNodeRenderStrategyShadowMap
{
public:
	virtual void SetNodeRenderState(  RCameraSceneNode* pCurrentCamera, RActorNode* pActorNode, RMaterial* pMaterial ) override;
	virtual void ResetNodeRenderState() override;

	virtual bool CheckInstancing(RActorNode* pActorNode, RMaterial* pMaterial) override;

protected:
	virtual bool IsRenderReceiver(bool bReceiver);
};




class RActorNodeRenderStrategyGStageDeferred: public RActorNodeRenderStrategy
{
private:
	void SetTextureByEnvironment( RActorNode* _pActorNode, RMaterial* _pMaterial );
public:
	virtual void SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial) override;
	virtual void SetNodeRenderState( RCameraSceneNode* pCurrentCamera, RActorNode* pActorNode, RMaterial* pMaterial ) override;
	virtual void ResetNodeRenderState() override;
	virtual void End() override;

	virtual RShaderFX* GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing) override;

	virtual bool CheckInstancing(RActorNode* pActorNode, RMaterial* pMaterial);
};


class RActorNodeRenderStrategyDiffuseOnly: public RActorNodeRenderStrategy
{
public:
	virtual void SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial) override;
	virtual RShaderFX* GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing) override;
};



class RActorNodeRenderStrategyAlpha: public RActorNodeRenderStrategy
{
public:
	virtual void SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial) override;
	virtual void Begin() override;
	virtual void End() override;

	virtual RShaderFX* GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing) override;

	virtual void ApplyDistortion(RCameraSceneNode* pCamera, RActorNode* pActorNode, int nMtrIndex) override;
};



class RActorNodeRenderStrategyMStageAddColor: public RActorNodeRenderStrategy
{
public:
	virtual void SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial) override;
	virtual void SetNodeRenderState(  RCameraSceneNode* pCurrentCamera, RActorNode* pActorNode, RMaterial* pMaterial ) override;
	virtual void Begin() override;
	virtual void End() override;

	virtual bool IsRender(RCameraSceneNode* pCamera, RActorNode* pActorNode, int nMtrIndex);
	virtual RShaderFX* GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing) override;
};



class RActorNodeRenderStrategyGStageInferred: public RActorNodeRenderStrategy
{
public:
	virtual void SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial) override;
	virtual void SetNodeRenderState(  RCameraSceneNode* pCurrentCamera, RActorNode* pActorNode, RMaterial* pMaterial ) override;
	virtual void ResetNodeRenderState() override;
	virtual void End() override;

	virtual RShaderFX* GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing) override;

	virtual bool CheckInstancing(RActorNode* pActorNode, RMaterial* pMaterial) override;
};



class RActorNodeRenderStrategyMStageInferred: public RActorNodeRenderStrategy
{
public:
	virtual void SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial) override;
	virtual void Begin() override;
	virtual void End() override;

	virtual RShaderFX* GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing) override;
};



class RActorNodeRenderStrategySky: public RActorNodeRenderStrategy
{
public:
	virtual void SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial) override;
	virtual void Begin() override;
	virtual void End() override;

	virtual RShaderFX* GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing) override;
};


}