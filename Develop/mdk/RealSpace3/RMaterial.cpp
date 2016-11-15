#include "stdafx.h"
#include "MDebug.h"
#include "RMaterial.h"
#include "mmsystem.h"
#include "RToken.h"
#include "RShaderCompositeManager.h"
#include "RShaderComposer.h"

namespace rs3 {

	
char *FormatColor(char *buffer,RVector3 &v)
{
	int r = int(v.x*255);
	int g = int(v.y*255);
	int b = int(v.z*255);

	sprintf(buffer,"%d %d %d",r,g,b);
	return buffer;
}

void ReadColor(char* szBuf,RVector& v)
{
	int r = 0,g = 0,b = 0;

	sscanf(szBuf,"%d %d %d",&r,&g,&b);

	if( r <0 || 255 < r ||
		g <0 || 255 < g ||
		b <0 || 255 < b 	)
	{
		mlog("Color value is invalid. \n");
		_ASSERT(false);
	}

	v.x = (float)r / 255.0f;
	v.y = (float)g / 255.0f;
	v.z = (float)b / 255.0f;
}


/////////////////////////////////////////////////////////

RMaterial::RMaterial()
: m_nMaxTexLayer(0),
m_Ambient(RVector::ZERO),
m_Diffuse(RVector::ZERO),
m_Specular(RVector::ZERO),
m_fSelfIlluminationScale(0.0f),
m_fSpecularLevel(0.0f),
m_fGlossiness(0.0f),
m_dwFlags(0),
m_AlphaCmpFunc(RCMP_GREATEREQUAL),
m_AlphaRefValue(128),
m_bAnimation(false),
m_bUseSpecular(false),
m_TexInfo(NULL)
{
	CreateTexInfo(1);
	AddMtrlTexInfo(0, RMtrlTexInfo());

	for(int i=0; i<ALPHAUSAGE_MAX; ++i)
		m_TextureTypeForAlphaUsage[i] = RTT_UNKNOWN;
}

RMaterial::~RMaterial()
{
	SAFE_DELETE_ARRAY(m_TexInfo);
}

void RMaterial::CreateTexInfo(int nLayer)
{
	if (nLayer <= 0) 
	{
		return;
	}

	SAFE_DELETE_ARRAY(m_TexInfo);

	m_TexInfo = new std::vector<RMtrlTexInfo>[nLayer];
	m_nMaxTexLayer = nLayer;
}

RMtrlTexInfo* RMaterial::GetTexLayer(int nLayer)
{
	if(nLayer<0 || nLayer > m_nMaxTexLayer-1)
		return NULL;

	if( GetTextureCountOfLayer(nLayer) == 0 )
		return NULL;

	return &m_TexInfo[nLayer][0];
}

RMtrlTexInfo* RMaterial::GetTexLayerByType(RTextureType nType)
{
	for(int i=0; i<m_nMaxTexLayer; ++i)
	{
		if(m_TexInfo[i][0].GetTextureType()==nType)
			return &m_TexInfo[i][0];
	}

    return NULL;
}

RMtrlTexInfo* RMaterial::GetTexLayerByNameFromType(RTextureType nType, const std::string& TexName)
{
	for(int i=0; i<m_nMaxTexLayer; ++i)
	{
		if(m_TexInfo[i][0].GetTextureType()!=nType)
			continue;

		for(size_t iTex=0; iTex<GetTextureCountOfLayer(i); ++iTex)
		{
			if(m_TexInfo[i][iTex].GetTextureName() == TexName)
				return &m_TexInfo[i][iTex];
		}
	}

	return NULL;
}

bool RMaterial::IsUseExtraUV()
{
	for( int i = 0; i < m_nMaxTexLayer; ++i )
	{
		for( size_t iTex = 0; iTex < GetTextureCountOfLayer(i); ++iTex )
		{
			if( m_TexInfo[i][iTex].GetMapChannel() > 0 )
				return true;
		}
	}

	return false;
}


RCompositeLayer* RMaterial::GetCompositeLayer(RTextureType textureType)
{
	for(size_t i=0; i<m_CompositeLayer.size(); ++i)
	{
		if(m_CompositeLayer[i].GetTextureType() == textureType)
			return &m_CompositeLayer[i];
	}

	return NULL;
}

bool RMaterial::SearchFromTextureName( RTextureType _Search, RTextureType _From )
{
	RMtrlTexInfo* pSearchTexInpo	= GetTexLayerByType(_Search);
	RMtrlTexInfo* pFromTexInpo		= GetTexLayerByType(_From);
	
	if( !pSearchTexInpo || !pFromTexInpo )
		return false;

	if( pSearchTexInpo->GetTextureName() == pFromTexInpo->GetTextureName() )
	{
		switch( _Search )
		{
			case RTT_SHININESS:
				m_TextureTypeForAlphaUsage[ALPHAUSAGE_SHINIESS] = _From;
				return true;
			break;
		}
	}
	return false;
}


bool RMaterial::Open(MXmlElement *pElement)
{
	MXmlElement* pChildElement;
	char szTagName[256],szContents[256];

	m_dwFlags = 0;

	m_Name = pElement->Attribute(RTOK_NAME);

	pChildElement = pElement->FirstChildElement();

	for(pChildElement; pChildElement != NULL; pChildElement = pChildElement->NextSiblingElement())
	{
		strcpy(szTagName, pChildElement->Value());
		_Contents(szContents, pChildElement);

		if(_stricmp(szTagName,RTOK_AMBIENT)==0)		ReadColor(szContents,m_Ambient); else
		if(_stricmp(szTagName,RTOK_DIFFUSE)==0)		ReadColor(szContents,m_Diffuse); else
		if(_stricmp(szTagName,RTOK_SPECULAR)==0)		ReadColor(szContents,m_Specular); else
		if(_stricmp(szTagName,RTOK_SPECULAR_LEVEL)==0)
		{
			sscanf(szContents,"%f",&m_fSpecularLevel);
			m_fSpecularLevel = m_fSpecularLevel * 0.01f;		// Max 수치에서 ShaderConst 수치로 변환
		} else
		if(_stricmp(szTagName,RTOK_GLOSSINESS)==0)
		{
			sscanf(szContents,"%f",&m_fGlossiness); 
		} else
		if(_stricmp(szTagName,RTOK_SELFILLUSIONSCALE)==0)		sscanf(szContents,"%f",&m_fSelfIlluminationScale); else

		if(_stricmp(szTagName,RTOK_TEXTURE_LIST)==0) 
		{
			pChildElement->Attribute(RTOK_COUNT,&m_nMaxTexLayer);

			CreateTexInfo(m_nMaxTexLayer);	// 배열에 메모리 할당.

			MXmlElement* pTexElement = pChildElement->FirstChildElement(RTOK_TEXTURE_LAYER);
			for(pTexElement; pTexElement != NULL; pTexElement = pTexElement->NextSiblingElement(RTOK_TEXTURE_LAYER))
			{
				strcpy(szTagName, pTexElement->Value());
				_Contents(szContents, pTexElement);
			
				if(_stricmp(szTagName,RTOK_TEXTURE_LAYER)==0) 
				{	
					int nLayer(0);
					pTexElement->Attribute(RTOK_INDEX,&nLayer);
					if( nLayer >= 0 )
					{
						RMtrlTexInfo MtrlTexInfo;
						MtrlTexInfo.Open(pTexElement);

						AddMtrlTexInfo(nLayer, MtrlTexInfo);

						if(MtrlTexInfo.GetMapChannel() > 0)
							m_dwFlags |= RM_FLAG_EXTRA_UV;

						m_bAnimation |=MtrlTexInfo.IsUVAnimation();						
					}
				} 
			}
			
 			MXmlElement *pAnimation = pChildElement->FirstChildElement(RTOK_TEX_ANIMATION);
			if ( pAnimation )
			{
				m_UVAniInfo.Open(pAnimation);

				m_bAnimation= true;
			}
			else
			{
				memset(szContents, 0, sizeof(szContents));
				memset(szTagName, 0, sizeof(szTagName));

				bool bResult;

				MXmlElement *pDefaultOffset = pChildElement->FirstChildElement(RTOK_TEX_OFFSET);
				if ( pDefaultOffset )
				{
					bResult = _Contents(szContents, pDefaultOffset);
					_ASSERT(bResult);

					RVector2 vOffset;
					sscanf(szContents,"%f %f",&vOffset.x,&vOffset.y);
					m_vUVTransform.SetOffset(vOffset);
				}

				MXmlElement *pDefaultScale = pChildElement->FirstChildElement(RTOK_TEX_SCALE);
				if ( pDefaultScale )
				{
					bResult = _Contents(szContents, pDefaultScale);
					_ASSERT(bResult);

					RVector2 vScale;
					sscanf(szContents,"%f %f",&vScale.x,&vScale.y);
					m_vUVTransform.SetScale(vScale);
				}

				if(pDefaultOffset || pDefaultScale)
				{
					SetAllLayerUVTransform(m_vUVTransform);
				}
			}
		}

		if(_stricmp(szTagName,"COMPOSITELAYERLIST")==0)
		{
			MXmlElement* pCompositeElement = pChildElement->FirstChildElement("COMPOSITELAYER");
			for(pCompositeElement; pCompositeElement != NULL; pCompositeElement = pCompositeElement->NextSiblingElement("COMPOSITELAYER"))
			{
				RCompositeLayer CompositeLayer;
				CompositeLayer.Open(pCompositeElement, this);
				m_CompositeLayer.push_back(CompositeLayer);
			}

		} else
					
		if(_stricmp(szTagName,RTOK_ADDITIVE)==0)		m_dwFlags|=RM_FLAG_ADDITIVE; else
		if(_stricmp(szTagName,RTOK_USEOPACITY)==0)	m_dwFlags|=RM_FLAG_USEOPACITY; else
		if(_stricmp(szTagName,RTOK_TWOSIDED)==0)		m_dwFlags|=RM_FLAG_TWOSIDED; else
		if(_stricmp(szTagName,RTOK_USEALPHATEST)==0)
		{
			m_dwFlags|=RM_FLAG_USEALPHATEST;
			// 알파 테스트를 사용하면 opacity 는 사용하지 않는다
			m_dwFlags &= (0xffffffff ^ RM_FLAG_USEOPACITY);
			MXmlElement* pAlphaTestValueElement = pChildElement->FirstChildElement(RTOK_ALPHATESTVALUE);
			if(pAlphaTestValueElement)
			{
				int nAlphaRef;
				_Contents(&nAlphaRef,pAlphaTestValueElement);
				m_AlphaRefValue = nAlphaRef;
			}
			MXmlElement* pAlphaCompareFunctionElement = pChildElement->FirstChildElement(RTOK_ALPHATESTFUNC);
			if(pAlphaCompareFunctionElement)
			{
				string strCmpFunc;
				_Contents(strCmpFunc,pAlphaCompareFunctionElement);
				for(int ic = 0; ic< RCMP_END; ic++)
				{
					if(strCmpFunc==RTOK_COMPARE_FUNCTION[ic])
						m_AlphaCmpFunc = (RCMPFUNC)ic;
				}
			}
		}else
		if(_stricmp(szTagName,RTOK_USEPARTSCOLOR)==0)		m_dwFlags|=RM_FLAG_USEPARTSCOLOR;

		// Special Effect Shader Flag
		if( _stricmp( szTagName, RTOK_SHADEREFFECTS ) == 0 )
		{
			m_rscShaderComponent = RSCID();

			MXmlElement* pShaderEffectElement = pChildElement->FirstChildElement( RTOK_SHADEREFFECT );
			for(pShaderEffectElement; pShaderEffectElement != NULL; pShaderEffectElement = pShaderEffectElement->NextSiblingElement( RTOK_SHADEREFFECT ) )
			{
				string shaderComponentName;
				_Contents( shaderComponentName, pShaderEffectElement );

				m_rscShaderComponent |= RShaderCompositeManager::GetShaderComponentIDByName( shaderComponentName.c_str() );
			}
		}
	}

	if(m_UVAniInfo.IsHaveAnimationKey())
		m_dwFlags |= RM_FLAG_TEXTURE_TRANSFORM;


	if(GetTexLayerByType(RTT_REFLECT) != NULL)
	{
		RTextureType check[] = { RTT_SPECULAR, RTT_DIFFUSE, RTT_OPACITY, RTT_NORMAL };

		for( int i = 0 ; i < sizeof( check ) / sizeof( RTextureType ) ; ++i )
		{
			if( SearchFromTextureName( RTT_SHININESS, check[i] ) )
				break;
		}
	}

	if(GetTexLayerByType(RTT_SPECULAR)!=NULL)
	{
		// 디퓨즈맵의 알파 채널을 스페큘러로 쓰는 경우
		if(GetTexLayerByType(RTT_DIFFUSE)->GetTextureName() == GetTexLayerByType(RTT_SPECULAR)->GetTextureName())
			m_TextureTypeForAlphaUsage[ALPHAUSAGE_SPECULAR] = RTT_DIFFUSE;
		else
			m_TextureTypeForAlphaUsage[ALPHAUSAGE_SPECULAR] = RTT_SPECULAR;

		m_bUseSpecular = true;
	}
	else if( m_fSpecularLevel > 0.0f &&  m_fGlossiness > 0.0f ) 
	{
		m_TextureTypeForAlphaUsage[ALPHAUSAGE_SPECULAR] = RTT_NONE;
		m_bUseSpecular = true;
	}
	else
	{
		m_TextureTypeForAlphaUsage[ALPHAUSAGE_SPECULAR] = RTT_NONE;
	}
///---------------------------------------------------------------------------------


	// specluar level이 존재하면 specluar를 사용 하는 것으로 인식하고, 항상 glossniess가 1 이상이 되도록 강제화를 셰이더 단계에서 처리
	if(  m_fSpecularLevel > 0 &&  m_fGlossiness < 1)		
		 m_fGlossiness = 1;


	m_TextureTypeForAlphaUsage[ALPHAUSAGE_ALPHA] = RTT_UNKNOWN;
	if(GetTexLayerByType(RTT_OPACITY)!=NULL)
	{
		if(GetTexLayerByType(RTT_DIFFUSE)!=NULL)
		{
			// 디퓨즈맵의 알파채널을 오파서티맵으로 쓰는 경우
			if( GetTexLayerByType(RTT_DIFFUSE)->GetTextureName() == GetTexLayerByType(RTT_OPACITY)->GetTextureName() ) 
				m_TextureTypeForAlphaUsage[ALPHAUSAGE_ALPHA] = RTT_DIFFUSE;
		}

		// 디퓨즈맵과 같이 쓰는게 아니면 스페큘러 맵 채널을 사용
		if( m_TextureTypeForAlphaUsage[ALPHAUSAGE_ALPHA] == RTT_UNKNOWN &&
			GetTexLayerByType(RTT_SPECULAR)!=NULL )
		{
			// 스페큘러맵의 알파채널을 오파서티맵으로 쓰는 경우
			if( GetTexLayerByType(RTT_SPECULAR)->GetTextureName() == GetTexLayerByType(RTT_OPACITY)->GetTextureName() )
				m_TextureTypeForAlphaUsage[ALPHAUSAGE_ALPHA] = RTT_SPECULAR;
		}

		// 디퓨즈맵, 스페큘러맵과 같이 쓰는게 아니면 노말 맵 채널을 사용
		if( m_TextureTypeForAlphaUsage[ALPHAUSAGE_ALPHA] == RTT_UNKNOWN &&
			GetTexLayerByType(RTT_NORMAL)!=NULL )
		{
			// 스페큘러맵의 알파채널을 오파서티맵으로 쓰는 경우
			if( GetTexLayerByType(RTT_NORMAL)->GetTextureName() == GetTexLayerByType(RTT_OPACITY)->GetTextureName() )
				m_TextureTypeForAlphaUsage[ALPHAUSAGE_ALPHA] = RTT_NORMAL;
		}

		if( m_TextureTypeForAlphaUsage[ALPHAUSAGE_ALPHA] == RTT_UNKNOWN )
		{
			//_ASSERT( 0 ); // 처리할수 없는 opacity map, 디퓨즈 알파 채널도 아니고, 스페큘러의 알파 채널도 아닌경우
			static bool bWarned = false;
			if(!bWarned)
			{
				bWarned = true;
				mlog(" %s material 처리할수 없는 opacity map, 디퓨즈 알파 채널도 아니고, 스페큘러의 알파 채널도 아니고 노말의 알파 채널도 아닌경우\n", m_Name.c_str() );
			}
		}
	}

// ----------------------------------------------------------------------------------

	return true;
}

void RMaterial::Save(MXmlElement* pElement)
{
	char buffer[256];

	_SetAttribute(pElement, RTOK_NAME,m_Name);


	//------------------------------------------------------------------------
	//Save Property for lighting
	MXmlElement* pChildElement = new MXmlElement(RTOK_DIFFUSE);
	pElement->LinkEndChild(pChildElement);
	_SetContents(pChildElement, FormatColor(buffer, m_Diffuse));

	pChildElement = new MXmlElement(RTOK_AMBIENT);
	pElement->LinkEndChild(pChildElement);
	pChildElement->LinkEndChild(new MXmlText(FormatColor(buffer, m_Ambient)));

	pChildElement = new MXmlElement(RTOK_SPECULAR);
	pElement->LinkEndChild(pChildElement);
	pChildElement->LinkEndChild(new MXmlText(FormatColor(buffer, m_Specular)));

	pChildElement = new MXmlElement(RTOK_SPECULAR_LEVEL);
	pElement->LinkEndChild(pChildElement);
	sprintf(buffer,"%5.3f",  m_fSpecularLevel);
	_SetContents(pChildElement, buffer);

	pChildElement = new MXmlElement(RTOK_GLOSSINESS);
	pElement->LinkEndChild(pChildElement);
	sprintf(buffer,"%5.3f",  m_fGlossiness);
	_SetContents(pChildElement, buffer);

	pChildElement = new MXmlElement(RTOK_SELFILLUSIONSCALE);
	pElement->LinkEndChild(pChildElement);
	sprintf(buffer,"%5.3f",  m_fSelfIlluminationScale);
	_SetContents(pChildElement, buffer);
	//Save Property for lighting
	//------------------------------------------------------------------------



	//------------------------------------------------------------------------
	//Save Texture List
	MXmlElement* pTexListElement = new MXmlElement(RTOK_TEXTURE_LIST);
	{
		OptimizeUVTransform();

		int nLayerCount=0;
		RTextureType texType = RTT_UNKNOWN;

		for(int nLayer=0;nLayer<m_nMaxTexLayer;++nLayer) 
		{
			MXmlElement* pLayerElement = new MXmlElement(RTOK_TEXTURE_LAYER);

			RMtrlTexInfo *pTexLayer = GetTexLayer(nLayer);
			pTexLayer->Save(pLayerElement);

			if(pTexLayer->GetTextureType() != texType)
			{
				++nLayerCount;
				texType = pTexLayer->GetTextureType();
			}

			_SetAttribute(pLayerElement, RTOK_INDEX, nLayerCount-1);
			pTexListElement->LinkEndChild(pLayerElement);
		}
		pElement->LinkEndChild(pTexListElement);
		_SetAttribute(pTexListElement, RTOK_COUNT,nLayerCount);
	}
	//Save Texture List
	//------------------------------------------------------------------------



	//------------------------------------------------------------------------
	//Save Composite Layer
	if(!m_CompositeLayer.empty())
	{
		MXmlElement* pCompositeListElement = new MXmlElement("COMPOSITELAYERLIST");

		for(size_t i=0; i<m_CompositeLayer.size(); ++i)
		{
			MXmlElement* pComposite = new MXmlElement("COMPOSITELAYER");
			_SetAttribute(pComposite, RTOK_INDEX, m_CompositeLayer[i].GetTextureType());
			_SetAttribute(pComposite, RTOK_COUNT, m_CompositeLayer[i].GetLayerCount());

			m_CompositeLayer[i].Save(pComposite);

			pCompositeListElement->LinkEndChild(pComposite);
		}

		pElement->LinkEndChild(pCompositeListElement);
	}
	//Save Composite Layer
	//------------------------------------------------------------------------
	


	//------------------------------------------------------------------------
	//Save UV Animation/Transform
	if(m_UVAniInfo.IsHaveAnimationKey())
		m_UVAniInfo.Save(pTexListElement);
	else
	{
		buffer[0] = '\0';

		if(m_vUVTransform.IsDefaultOffset() == false)
		{
			MXmlElement *pDefaultOffset = new MXmlElement(RTOK_TEX_OFFSET);
			pTexListElement->LinkEndChild(pDefaultOffset);
			sprintf(buffer,"%5.3f %5.3f", m_vUVTransform.GetOffset().x, m_vUVTransform.GetOffset().y);
			_SetContents(pDefaultOffset, buffer);
		}

		if( m_vUVTransform.IsDefaultScale() == false )
		{
			MXmlElement *pDefaultScale = new MXmlElement(RTOK_TEX_SCALE);
			pTexListElement->LinkEndChild(pDefaultScale);
			sprintf(buffer,"%5.3f %5.3f", m_vUVTransform.GetScale().x, m_vUVTransform.GetScale().y);
			_SetContents(pDefaultScale, buffer);
		}
	}
	//Save UV Animation/Transform
	//------------------------------------------------------------------------



	//------------------------------------------------------------------------
	//Save Flag
	if( CheckFlag(RM_FLAG_ADDITIVE) )
	{
		pChildElement = new MXmlElement(RTOK_ADDITIVE);
		pElement->LinkEndChild(pChildElement);
	}
	if( CheckFlag(RM_FLAG_TWOSIDED) )
	{
		pChildElement = new MXmlElement(RTOK_TWOSIDED);
		pElement->LinkEndChild(pChildElement);
	}
	if( CheckFlag(RM_FLAG_USEOPACITY) )
	{
		pChildElement = new MXmlElement(RTOK_USEOPACITY);
		pElement->LinkEndChild(pChildElement);
	}
	if( CheckFlag(RM_FLAG_USEALPHATEST) )
	{
		pChildElement = new MXmlElement(RTOK_USEALPHATEST);
		pElement->LinkEndChild(pChildElement);

		MXmlElement* pValueElement = new MXmlElement(RTOK_ALPHATESTVALUE);
		_SetContents(pValueElement,(int)m_AlphaRefValue);
		pChildElement->LinkEndChild(pValueElement);

		if(m_AlphaCmpFunc!=RCMP_GREATEREQUAL)
		{
			MXmlElement* pCmpFuncElement = new MXmlElement(RTOK_ALPHATESTFUNC);
			_SetContents(pCmpFuncElement,RTOK_COMPARE_FUNCTION[m_AlphaCmpFunc]);
			pChildElement->LinkEndChild(pCmpFuncElement);
		}
	}
	if( CheckFlag(RM_FLAG_USEPARTSCOLOR) )
	{
		pChildElement = new MXmlElement(RTOK_USEPARTSCOLOR);
		pElement->LinkEndChild(pChildElement);
	}
	//Save Flag
	//------------------------------------------------------------------------



	//------------------------------------------------------------------------
	//Save Shader Effect
	if(m_rscShaderComponent.IsNonZero())
	{
		pChildElement = new MXmlElement(RTOK_SHADEREFFECTS);
		pElement->LinkEndChild(pChildElement);

		vector<string> componentNameList;
		RShaderCompositeManager::GetComponentsNameList( m_rscShaderComponent, componentNameList );
		for( size_t i = 0; i < componentNameList.size(); ++i )
		{
			MXmlElement* pChildChildElement = new MXmlElement(RTOK_SHADEREFFECT);
			pChildElement->LinkEndChild(pChildChildElement);
			_SetContents(pChildChildElement,componentNameList[i]);
		}
	}
	//Save Shader Effect
	//------------------------------------------------------------------------
}

void RMaterial::GetTextureTransform(RMatrix *pOut, const RVector vOffset, const RVector &vScale)
{
	RMatrix matT, matS, matA, matB, matC, matD;
	matT.MakeIdentity();
	matS.MakeIdentity();
	matA.MakeIdentity();
	matB.MakeIdentity();
	matC.MakeIdentity();
	matD.MakeIdentity();

	matC._22 = -1.f;
	matC._42 = 1.0f;

	matA.SetTranslation(RVector(.5f, 0.5f, 0.f));
	matB.SetTranslation(RVector(-0.5f,-0.5f, 0.f));
	matS.SetScale(vScale);
	matT.SetTranslation(-vOffset);

	_ASSERT(pOut!=NULL);
	*pOut = matC * matT * matB * matS * matA * matC;
}

RMatrix RMaterial::GetTextureTransform()
{
	RVector3 vOffset(m_vUVTransform.GetOffset().x, m_vUVTransform.GetOffset().y,0);
	RVector3 vScale(m_vUVTransform.GetScale().x, m_vUVTransform.GetScale().y,0);

	RMatrix matResult;
	GetTextureTransform(&matResult, vOffset, vScale);
	return matResult;
}

bool RMaterial::IsUsePerLayerTransform(RTextureType textureType)
{
	if(IsUVAnimation())
		return false;

	RMtrlTexInfo* pTextureLayer =GetTexLayerByType(textureType);
	return pTextureLayer->IsUVAnimation() || !pTextureLayer->IsDefaultUVTranform();
}

void RMaterial::SetAllLayerUVTransform(RUVTransform UVTrans)
{
	for(int iTexType=0; iTexType<m_nMaxTexLayer; ++iTexType)
	{
		for(size_t iLayer=0; iLayer<GetTextureCountOfLayer(iTexType); ++iLayer)
		{
			m_TexInfo[iTexType][iLayer].SetUVTransform(UVTrans);
		}
	}
}

void RMaterial::UpdateUVAnimation(UINT nFrame)
{
	//모든 텍스쳐 레이어가 같은 애니메이션을 사용할 경우
	if(IsUVAnimation())
	{
		m_vUVTransform = m_UVAniInfo.GetUVTransform(nFrame);
		return;
	}

	//텍스쳐 레이어별로 별도의 애니메이션 사용
	for(int iTexType=0; iTexType<m_nMaxTexLayer; ++iTexType)
	{
		for(size_t iLayer=0; iLayer<GetTextureCountOfLayer(iTexType); ++iLayer)
		{
			m_TexInfo[iTexType][iLayer].UpdateUVAnimation(nFrame);
		}
	}
}

//모든 텍스쳐 레이어가 같은 UV-transform이나 UV-animation을 사용하면 이를 매테리얼에 저장
void RMaterial::OptimizeUVTransform()
{
	if(m_nMaxTexLayer<=0)
		return;

	if(m_TexInfo[0].empty())
		return;

	RUVAnimationInfo& UVAni = m_TexInfo[0][0].GetUVAnimationRef();
	RUVTransform UVTransform = m_TexInfo[0][0].GetUVTranform();
	bool bCommonAni =true;
	bool bCommonTrans =true;


	for(int iTexType=0; iTexType<m_nMaxTexLayer; ++iTexType)
	{
		for(size_t iLayer=0; iLayer<GetTextureCountOfLayer(iTexType); ++iLayer)
		{
			if( !(UVAni==m_TexInfo[iTexType][iLayer].GetUVAnimationRef()) )
				bCommonAni =false;

			if( UVTransform!=m_TexInfo[iTexType][iLayer].GetUVTranform() )
				bCommonTrans =false;
		}
	}

	if(bCommonAni)
	{
		COPY_IF_EXIST(m_UVAniInfo.GetOffsetKeyTrack(), m_TexInfo[0][0].GetUVAnimationRef().GetOffsetKeyTrack());
		COPY_IF_EXIST(m_UVAniInfo.GetScaleKeyTrack(), m_TexInfo[0][0].GetUVAnimationRef().GetScaleKeyTrack());
		COPY_IF_EXIST(m_UVAniInfo.GetRotateKeyTrack(), m_TexInfo[0][0].GetUVAnimationRef().GetRotateKeyTrack());
	}

	if(bCommonTrans)
		m_vUVTransform =UVTransform;

	for(int iTexType=0; iTexType<m_nMaxTexLayer; ++iTexType)
	{
		for(size_t iLayer=0; iLayer<GetTextureCountOfLayer(iTexType); ++iLayer)
		{
			if(bCommonAni)
				m_TexInfo[iTexType][iLayer].GetUVAnimationRef().ClearAnimationKey();

			if(bCommonTrans)
				m_TexInfo[iTexType][iLayer].GetUVTranform().SetDefault();
		}
	}	
}

}
