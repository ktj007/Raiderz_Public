#pragma once

#include "RSceneNode.h"
#include "RLODContext.h"
#include "RPostEffectDistortionParameter.h"
#include "RDevice.h"

namespace rs3 {

	class RParticleShader;

	/**
	 Effect 로서 기본 요소들을 가지는 Layer 클래스
	*/
	class RS_API REffectBase : public RSceneNode
	{
	public:
		MDeclareRTTI;
		DECLARE_NODE_ID_STRING("REFFECTBASE");

	public:
		REffectBase( RRENDERPASS eRenderPass = RRP_PARTICLE);
		virtual ~REffectBase();

		virtual void Start() = 0;
		virtual void Stop() = 0;
		virtual void Pause() = 0;
		virtual bool IsAlive() = 0;

		virtual float GetStartTime() {	return 0.0f; }
		virtual void SetLooping(bool loop) = 0;
		virtual bool IsLooping() = 0;

		void SetDistortion(bool bDistortion) { m_bDistortion = bDistortion; }
		bool IsDistortion() const { return m_bDistortion; }

		virtual int GetCurrentCount() = 0;

		// result scale
		float GetResultScale(float fInitValue, RSceneNode* pInitParent);

		void SetParentEffectSceneNode( RSceneNode* pParentEffectSceneNode ) { m_pParentEffectSceneNode = pParentEffectSceneNode; }
		RSceneNode* GetParentEffectSceneNode() { return m_pParentEffectSceneNode; }

		void SetRenderOrder( int nRenderOrder ) { m_nRenderOrder = nRenderOrder; }
		int GetRenderOrder() { return m_nRenderOrder; }

		bool GetVisibleSwitch() const { return m_bVisibleSwitch; }
		void SetVisibleSwitch( bool bFlag ) { m_bVisibleSwitch = bFlag; }
		virtual bool GetVisible() const override { return ( RSceneNode::GetVisible() && m_bVisibleSwitch ); }

		RDistortionParameter*	GetDistortionParam()	{ return &m_DistortionParam; }
		const RDistortionParameter&	GetDistortionParam() const { return m_DistortionParam; }

		bool IsBlendModeAdd();
		void SetBlendModeAdd();

		void SetBlendModeAlpha();
		bool IsBlendModeAlpha();

		bool IsBlendModeOpaque();
		void SetBlendModeOpaque();

		bool IsBlendModeInverse();
		void SetBlendModeInverse();

		RBLEND GetSrcBlendMode() const { return m_SrcBlend; }
		RBLEND GetDestBlendMode() const { return m_DstBlend; }

		void SetUseAlphaTest(bool b);
		bool IsUseAlphaTest() const;

		void SetAlphaRef(float ref);
		float GetAlphaRef() const;

		void		SetSelfIlluminationScale(float scale) { _ASSERT( scale >= 0.f && scale <= 4.f ); m_fSelfIlluminationScale = scale; }
		const float	GetSelfIlluminationScale() const { return m_fSelfIlluminationScale; }

		const string& GetTextureName() const { return m_strTextureName; }
		void SetTextureName( const char *szName ) { m_strTextureName = szName; }

		const string& GetSITextureName() const { return m_strSITextureName; }
		void SetSITextureName( const char *szName ) { m_strSITextureName = szName; }

		RTexture*	GetTexture() { return m_hTexture; }
		RTexture*	GetSITexture() { return m_hSITexture; }

		void LoadAllTexture(bool bBackground);

		void LoadTexture(bool bBackground);
		void LoadSITexture(bool bBackground);

		void DeleteTexture();
		void DeleteSITexture();

		// 툴을 위한 함수
		void ReplaceTexture(const char *const pName);
		void ReplaceSITexture(const char *const pName);

	protected:
		void SetRenderState( RCameraSceneNode* pCamera, RParticleShader* pParticleShader, bool bDistortion );
		void RestoreRenderState( RCameraSceneNode* pCamera, RParticleShader* pParticleShader );

		bool BeginDistortion(RCameraSceneNode* pCamera);
		void EndDistortion(RCameraSceneNode* pCamera, bool bTriagleStrip);

		void CopyEffectBaseParameter( const REffectBase* pSrc );

		bool LoadXMLBlendInfo( MXmlElement* pElement );
		bool SaveXMLBlendInfo( MXmlElement* pElement );

		RSceneNode* m_pParentEffectSceneNode;
		int m_nRenderOrder;

		bool m_bVisibleSwitch;

		RDistortionParameter	m_DistortionParam;

	private:
		bool				m_bDistortion;

		bool				m_bAlphaTest;
		float				m_fAlphaRef;
		RBLEND				m_SrcBlend;
		RBLEND				m_DstBlend;

		RTexture*			m_hTexture;
		string				m_strTextureName;

		RTexture*			m_hSITexture; 			// Self illumination 텍스쳐
		string				m_strSITextureName;
		float				m_fSelfIlluminationScale;
	};
};