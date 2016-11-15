#ifndef _RCAMERA_DISTORTION_CONTROLLER_H
#define _RCAMERA_DISTORTION_CONTROLLER_H

#include "RCameraSceneNodeController.h"
#include "RShaderFX.h"
#include "RGlareDef.h"
#include "RListener.h"

namespace rs3 {

class RActorNode;

#define DISTORTION_DEFAUL_NORMALMAP "Distortion_nm.dds"

	class RS_API RDistortionParameter
	{
	public:
		RDistortionParameter();
		~RDistortionParameter();

		void LoadNormalMap(bool bBackground);
		void ReplaceNormalMap();
		void CreateToXML(MXmlElement &element);
		void SaveToXML(MXmlElement &element);

		RDistortionParameter& operator=(const RDistortionParameter& param);

		float fDir0X;
		float fDir0Y;
		float fDir1X;
		float fDir1Y;
		float fScale;
		float fSpeed0;
		float fSpeed1;
		bool bRender;
		char strNormalMapFileName[50];
		RTexture* pNormalMap;
		float fRange;
		float fVisibility;
		float fAttenuationBegin;
	};

	class RS_API RCameraDistortionController : public RCameraSceneNodeController, public RListener
	{
	public:
		RCameraDistortionController( RCameraSceneNode* pCameraSceneNode, bool& bValid );
		virtual ~RCameraDistortionController();

		virtual void	UpdateCameraController(RMatrix& _rMatApplyToCamera) {}

		virtual void	OnPreRender( RInferredRenderingStrategy* pInferredRenderingStrategy);
		virtual void	OnClear();
		virtual void	OnPostRender();
				
		bool 			Create();
		void 			Destroy();
		void			Restore();

		void			RenderEmitterDistortion(int nParticleCount, RDistortionParameter& param);
		void			RenderEffectActorDistortion(const vector<RActorNode*>& ActorNode, RDistortionParameter& param);
		void			RenderFullScreenDistortion(float fDistortionScale);
				
		RTexture*		GetMaskingMap()	{ return m_pMaskingMap; }
		
		void			BeginMasking(RDistortionParameter& parameter);
		void			EndMasking();

		bool			IsValid()	{ return m_bValid; }

	private:
		void 			DrawFullScreenQuad();
		void			CheckRenderTargetFormat();
		void			SetFog();
		void			SetShaderConstant(const RDistortionParameter& param);
		void			RenderTextureToFullScreen(RTexture* pTexture);
		void			RenderEffecActorNode(RActorNode* pActorNode, RTechnique technique);
		void			RenderResultToRenderTarget();

		RCameraSceneNode*		m_pCameraSceneNode;

		RShaderFX*		m_pDistortionShader;	
		
		RTexture* 		m_pRenderTarget;		//∑ª¥ı ≈∏∞Ÿ0
		RTexture* 		m_pRenderTargetOrg;		//±‚¡∏ ∑ª¥ı ≈∏∞Ÿ0 
		RTexture* 		m_pDistortredRT;		//ø÷∞Óµ» ¿ÃπÃ¡ˆ∏¶ ¿˙¿Â«œ¥¬ ∑ª¥ı≈∏∞Ÿ
		RTexture* 		m_pNormalMap;			//π˝º±∏ 
		RTexture* 		m_pMaskingMap;			//∏∂Ω∫≈∑∏ 
		RTexture* 		m_pMaskingMapOrg;		//±‚¡∏ ∑ª¥ı ≈∏∞Ÿ1

		bool			m_bValid;

		RListener*		m_pEventListener;			///< OnRestoreDevice / OnLostDevice ¿Ã∫•∆Æ ºˆΩ≈∞¥√º
	};

}

#endif
