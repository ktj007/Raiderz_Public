#include "stdafx.h"
#include "REffectBase.h"
#include "RPostEffectDistortion.h"
#include "RXMLTemplate.h"
#include "RParticleShader.h"
#include "RShaderCompositeManager.h"
#include "RLODProcessorBase.h"

namespace rs3 {

///////////////////////////////////////////////////////////////////////////////
// TODO: 호환성 유지. 제거해야 한다. - charmke
struct RParticleBlendTypes
{
	rs3::RBLEND eBlend;
	TCHAR *pcName;
};
RParticleBlendTypes tableBlendTypes [] = 
{
	RBLEND_ZERO				, _T("ZERO"),
	RBLEND_ONE				, _T("ONE"),					//1
	RBLEND_SRCCOLOR			, _T("SRCCOLOR"),				//2
	RBLEND_INVSRCCOLOR		, _T("ONE_MINUS_SRCCOLOR"),		//3
	RBLEND_SRCALPHA			, _T("SRCALPHA"),				//4
	RBLEND_INVSRCALPHA		, _T("ONE_MINUS_SRCALPHA"),		//5
	RBLEND_DESTALPHA		, _T("DESTALPHA"),
	RBLEND_DESTCOLOR		, _T("DESTCOLOR"),
	RBLEND_INVDESTCOLOR		, _T("ONE_MINUS_DESTCOLOR"),
	RBLEND_NONE				, _T("NONE")
};

TCHAR* BlendToString(RBLEND blend)
{
	for ( int i = 0; tableBlendTypes[i].pcName != NULL; ++i )
	{
		if ( blend == tableBlendTypes[i].eBlend )
			return tableBlendTypes[i].pcName;
	}

	return NULL;
}

RBLEND StringToBlend(const TCHAR* tString)
{
	for ( int i = 0; tableBlendTypes[i].pcName != NULL; ++i )
	{
		if ( _tcsicmp(tString,tableBlendTypes[i].pcName) == 0 )
			return tableBlendTypes[i].eBlend;
	}

	return RBLEND_NONE;
}
///////////////////////////////////////////////////////////////////////////////

MImplementRTTI(REffectBase, RSceneNode);

REffectBase::REffectBase(RRENDERPASS eRenderPass)
: RSceneNode(eRenderPass)
, m_pParentEffectSceneNode(NULL)
, m_nRenderOrder(0)
, m_bVisibleSwitch(true)
, m_bDistortion(false)
, m_bAlphaTest(false)
, m_fAlphaRef(128.f)
, m_SrcBlend(RBLEND_NONE)
, m_DstBlend(RBLEND_NONE)
, m_hTexture(NULL)
, m_hSITexture(NULL)
, m_fSelfIlluminationScale(1.f)
{
	/// LOD 사용
	CreateLODContext( LODP_FOG | LODP_PROJECTION);
}

REffectBase::~REffectBase()
{
	DeleteTexture();
	DeleteSITexture();
}

float REffectBase::GetResultScale(float fInitValue, RSceneNode* pInitParent)
{
	while (pInitParent)
	{
		fInitValue *= pInitParent->GetScale().x;
		pInitParent = pInitParent->GetParent();
	}

	return fInitValue;
}

bool REffectBase::IsBlendModeAdd()
{
	return ( RBLEND_SRCALPHA == m_SrcBlend && RBLEND_ONE == m_DstBlend );
}

void REffectBase::SetBlendModeAdd()
{
	m_SrcBlend = RBLEND_SRCALPHA;
	m_DstBlend = RBLEND_ONE;
}

bool REffectBase::IsBlendModeAlpha()
{
	return ( RBLEND_SRCALPHA == m_SrcBlend && RBLEND_INVSRCALPHA == m_DstBlend );
}

void REffectBase::SetBlendModeAlpha()
{
	m_SrcBlend = RBLEND_SRCALPHA;
	m_DstBlend = RBLEND_INVSRCALPHA;
}

bool REffectBase::IsBlendModeOpaque()
{
	return ( RBLEND_NONE == m_SrcBlend && RBLEND_NONE == m_DstBlend );
}

void REffectBase::SetBlendModeOpaque()
{
	m_SrcBlend = RBLEND_NONE;
	m_DstBlend = RBLEND_NONE;
}

bool REffectBase::IsBlendModeInverse()
{
	return ( RBLEND_INVSRCALPHA == m_SrcBlend && RBLEND_INVDESTCOLOR == m_DstBlend );
}

void REffectBase::SetBlendModeInverse()
{
	m_SrcBlend = RBLEND_INVSRCALPHA;
	m_DstBlend = RBLEND_INVDESTCOLOR;
}

void REffectBase::SetUseAlphaTest(bool b)
{
	m_bAlphaTest = b;
}

bool REffectBase::IsUseAlphaTest() const
{
	return m_bAlphaTest;
}

void REffectBase::SetAlphaRef(float ref)
{
	m_fAlphaRef = ref;
}

float REffectBase::GetAlphaRef() const
{
	return m_fAlphaRef;
}

void REffectBase::LoadAllTexture(bool bBackground)
{
	LoadTexture( bBackground );
	LoadSITexture( bBackground );
}

void REffectBase::LoadTexture(bool bBackground)
{
	DeleteTexture();

	if( m_strTextureName.size() )
	{
		_ASSERT( REngine::GetDevicePtr() );
		m_hTexture = REngine::GetDevice().CreateTextureForBuildLater(m_strTextureName.c_str(),
			RTF_LINEAR, RCT_AUTOGENMIPMAP | RCT_UNREDUCIBLE, RFMT_NONE, bBackground);
	}
}

void REffectBase::LoadSITexture(bool bBackground)
{
	DeleteSITexture();

	if( m_strSITextureName.size() )
	{
		_ASSERT( REngine::GetDevicePtr() );
		m_hSITexture = REngine::GetDevice().CreateTextureForBuildLater(m_strSITextureName.c_str(),
			RTF_LINEAR, RCT_AUTOGENMIPMAP | RCT_UNREDUCIBLE, RFMT_NONE, bBackground);
	}
}

void REffectBase::DeleteTexture()
{
	if( m_hTexture != R_NONE )
	{
		_ASSERT( REngine::GetDevicePtr() );
		REngine::GetDevice().DeleteTexture( m_hTexture );
		m_hTexture = R_NONE;
	}
}

void REffectBase::DeleteSITexture()
{
	if( m_hSITexture != R_NONE )
	{
		_ASSERT( REngine::GetDevicePtr() );
		REngine::GetDevice().DeleteTexture( m_hSITexture );
		m_hSITexture = R_NONE;
	}
}

void REffectBase::ReplaceTexture(const char *const pName)
{
	_ASSERT( pName );
	m_strTextureName = pName;
	LoadTexture( false );
}

void REffectBase::ReplaceSITexture(const char *const pName)
{
	_ASSERT( pName );
	m_strSITextureName = pName;
	LoadSITexture( false );
}

void REffectBase::SetRenderState( RCameraSceneNode* pCamera, RParticleShader* pParticleShader, bool bDistortion )
{
	_ASSERT( pCamera );
	_ASSERT( pParticleShader );

	//Set Shader Constant
	if( IsUseAlphaTest() )
	{
		pParticleShader->SetAlphaRefConstant( GetAlphaRef() / 255.0f );
	}

	if( IsBlendModeAdd() )
	{
		pParticleShader->SetFogColorConstant( 0x00000000 );
	}

	if( m_hSITexture )
	{
		pParticleShader->SetSelfIlluminationScaleConstant( GetSelfIlluminationScale() );
	}

	if( bDistortion )
	{
		pParticleShader->SetDistortionAttenuationBeginConstant( GetDistortionParam()->fAttenuationBegin );
	}

	//Set Render State
	REngine::GetDevice().SetFvF( PVERTEXFORMAT::FVF );
	REngine::GetDevice().SetBlending(GetSrcBlendMode(), GetDestBlendMode());
	REngine::GetDevice().SetCullMode(RCULL_NONE);
	REngine::GetDevice().SetDepthEnable(true, false);
	//Set Texture
	REngine::GetDevice().SetTexture( AS_DIFFUSE, GetTexture() );
	REngine::GetDevice().SetTextureAddressClamp(AS_DIFFUSE);
	REngine::GetDevice().SetTextureFilter(AS_DIFFUSE, RTF_BILINEAR);
	//Set SI Texture
	REngine::GetDevice().SetTexture( AS_SELFILLUMINATION, GetSITexture() );
	REngine::GetDevice().SetTextureAddressClamp(AS_SELFILLUMINATION);
	REngine::GetDevice().SetTextureFilter(AS_SELFILLUMINATION, RTF_BILINEAR);
}

void REffectBase::RestoreRenderState( RCameraSceneNode* pCamera, RParticleShader* pParticleShader )
{
	_ASSERT( pCamera );
	_ASSERT( pParticleShader );

	if( IsBlendModeAdd() )
	{
		pParticleShader->SetFogColorConstant( pCamera->GetFogColor() );
	}

	REngine::GetDevice().SetDepthEnable(true, true);
	REngine::GetDevice().SetBlending(RBLEND_NONE);
	REngine::GetDevice().SetCullMode(RCULL_CW);
}

bool REffectBase::BeginDistortion(RCameraSceneNode* pCamera)
{
	if(IsDistortion() == false)
		return false;

	RPostEffectDistortion* pPostEffectDistortion = (RPostEffectDistortion*)REngine::GetSceneManager().GetPostEffect(PE_DISTORTION);
	bool bDistortion = pPostEffectDistortion && pPostEffectDistortion->IsValid() 
		&& pPostEffectDistortion->IsCulledVisible( this, pCamera->GetWorldPosition());	/// 거리에 따라 디스토션 적용 유무 판단
	
	if(bDistortion==false)
		return false;

	if(pPostEffectDistortion->CheckRenderTargetFormat() == false)
		return false;
	
	
	pPostEffectDistortion->ClearMaskBuffer();
	if( !m_DistortionParam.bRender)
		REngine::GetDevice().SetColorWriteEnable(false);

	pPostEffectDistortion->BeginMasking(1);

	return true;
}

void REffectBase::EndDistortion(RCameraSceneNode* pCamera, bool bTriagleStrip)
{
	RPFC_THISFUNC;
	RPostEffectDistortion* pPostEffectDistortion = (RPostEffectDistortion*)REngine::GetSceneManager().GetPostEffect(PE_DISTORTION);

	if( pPostEffectDistortion)
		pPostEffectDistortion->EndMasking();

	REngine::GetDevice().SetColorWriteEnable(true);
	if( pPostEffectDistortion)
		pPostEffectDistortion->RenderEffectDistortion(pCamera, m_DistortionParam, bTriagleStrip);
}

void REffectBase::CopyEffectBaseParameter( const REffectBase* pSrc )
{
	SetNodeName( pSrc->GetNodeName() );

	SetTransform( pSrc->GetLocalTransform() );
	SetScale( pSrc->GetScale() );

	SetVisibleSwitch( pSrc->GetVisibleSwitch() );

	m_DistortionParam = pSrc->GetDistortionParam();
	SetDistortion( pSrc->IsDistortion() );

	m_bAlphaTest = pSrc->IsUseAlphaTest();
	m_fAlphaRef = pSrc->GetAlphaRef();
	m_SrcBlend = pSrc->GetSrcBlendMode();
	m_DstBlend = pSrc->GetDestBlendMode();

	SetTextureName( pSrc->GetTextureName().c_str() );
	SetSITextureName( pSrc->GetSITextureName().c_str() );
	SetSelfIlluminationScale( pSrc->GetSelfIlluminationScale() );
	LoadAllTexture( false );
}

bool REffectBase::LoadXMLBlendInfo( MXmlElement* pElement )
{
	_ASSERT( pElement );

	if( pElement->FirstChildElement("BLENDMODE_ADD") )
	{
		SetBlendModeAdd();
	}
	else
	if( pElement->FirstChildElement("BLENDMODE_ALPHA") )
	{
		SetBlendModeAlpha();
	}
	else
	if( pElement->FirstChildElement("BLENDMODE_OPAQUE") )
	{
		SetBlendModeOpaque();
	}
	else
	if( pElement->FirstChildElement("BLENDMODE_INVERSE") )
	{
		SetBlendModeInverse();
	}
	// TODO: 호환성 유지. 제거해야 한다. - charmke
	else
	if( pElement->FirstChildElement("SRCBLEND") )
	{
		int srcBlend = RBLEND_NONE;
		int destBlend = RBLEND_NONE;

		// 이미터 블렌딩 모드 호환
		if( pElement->FirstChildElement("DSTBLEND") )
		{
			char pcBuffer[128];
			pcBuffer[0] = '\0';
			if( _Contents( pcBuffer, pElement->FirstChildElement("SRCBLEND") ) )
				srcBlend = StringToBlend(pcBuffer);
			if( _Contents( pcBuffer, pElement->FirstChildElement("DSTBLEND") ) )
				destBlend = StringToBlend(pcBuffer);

			if( RBLEND_SRCALPHA == srcBlend && RBLEND_ONE == destBlend )
			{
				SetBlendModeAdd();
			}
			else
			if( RBLEND_SRCALPHA == srcBlend && RBLEND_INVSRCALPHA == destBlend )
			{
				SetBlendModeAlpha();
			}
			else
			if( ( RBLEND_NONE == srcBlend || RBLEND_ONE == srcBlend ) &&
				( RBLEND_NONE == destBlend || RBLEND_ONE == destBlend ) )
			{
				SetBlendModeOpaque();
			}
			else
			if( RBLEND_INVSRCALPHA == srcBlend && RBLEND_INVDESTCOLOR == destBlend )
			{
				SetBlendModeInverse();
			}
			else
			{
				_ASSERT( false );
				SetBlendModeAlpha();
			}
		}
		// 트레일 블렌딩 모드 호환
		else
		if( pElement->FirstChildElement("DESTBLEND") )
		{
			srcBlend = TXML::_GETElement<int>::Value( pElement, "SRCBLEND", "int" );
			destBlend = TXML::_GETElement<int>::Value( pElement, "DESTBLEND", "int" );

			if( srcBlend == RBLEND_SRCALPHA && destBlend == RBLEND_INVSRCALPHA )
				SetBlendModeAlpha();
			else
				SetBlendModeAdd();
		}
		else
		{
			_ASSERT( false );
		}
	}
	else
	{
		_ASSERT( false );
		SetBlendModeAlpha();
	}

	if( pElement->FirstChildElement("ALPHA_TEST") )
	{
		m_bAlphaTest = TXML::_GETElement<int>::Value( pElement, "ALPHA_TEST", "int" ) ? true : false;
	}
	// TODO: 호환성 유지. 제거해야 한다. - charmke
	else
	{
		m_bAlphaTest = IsBlendModeOpaque();
	}

	if( pElement->FirstChildElement("ALPHAREF") )
	{
		m_fAlphaRef = TXML::_GETElement<float>::Value( pElement, "ALPHAREF", "float" );
	}

	return true;
}

bool REffectBase::SaveXMLBlendInfo( MXmlElement* pElement )
{
	_ASSERT( pElement );

	if( IsBlendModeAdd() )
	{
		MXmlElement *pBlendModeElement = new MXmlElement("BLENDMODE_ADD");
		pElement->LinkEndChild(pBlendModeElement);
	}
	else
	if( IsBlendModeAlpha() )
	{
		MXmlElement *pBlendModeElement = new MXmlElement("BLENDMODE_ALPHA");
		pElement->LinkEndChild(pBlendModeElement);
	}
	else
	if( IsBlendModeOpaque() )
	{
		MXmlElement *pBlendModeElement = new MXmlElement("BLENDMODE_OPAQUE");
		pElement->LinkEndChild(pBlendModeElement);
	}
	else
	if( IsBlendModeInverse() )
	{
		MXmlElement *pBlendModeElement = new MXmlElement("BLENDMODE_INVERSE");
		pElement->LinkEndChild(pBlendModeElement);
	}
	else
	{
		_ASSERT( false );
	}

	TXML::_SetElementValue( pElement, "ALPHA_TEST", "int", m_bAlphaTest ? 1 : 0 );
	TXML::_SetElementValue( pElement, "ALPHAREF", "float", m_fAlphaRef );

	return true;
}

};