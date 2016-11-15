#include "stdafx.h"
#include "RToken.h"

namespace rs3 {

RMtrlTexInfo::RMtrlTexInfo() : 
m_textureType(RTT_UNKNOWN), 
m_hTexture(R_NONE), 
dwCreationFlag( RCT_AUTOGENMIPMAP)
{
}

RMtrlTexInfo::~RMtrlTexInfo()
{
}

// Texture Layer 정보에 대해 기술된 것들을 읽는다.

bool RMtrlTexInfo::Open(MXmlElement *pElement)
{
	MXmlElement* pChildElement;
	char szTagName[256],szContents[256];
	int nLayer = 0;

	// 텍스쳐 리덕션 옵션 정보
	bool bUnreducible = false;
	if( _Attribute( bUnreducible, pElement, RTOK_UNREDUCIBLE ) )
	{
		if( bUnreducible )
			dwCreationFlag |= RCT_UNREDUCIBLE;
	}

	pChildElement = pElement->FirstChildElement();
	for(pChildElement; pChildElement != NULL; pChildElement = pChildElement->NextSiblingElement())
	{
		strcpy(szTagName, pChildElement->Value());
		_Contents(szContents, pChildElement, "");

		if(_stricmp(szTagName,RTOK_DIFFUSEMAP)==0)	{ m_TextureDesc.TextureName = szContents; m_textureType = RTT_DIFFUSE; } else
		if(_stricmp(szTagName,RTOK_SPECULARMAP)==0)	{ m_TextureDesc.TextureName = szContents; m_textureType = RTT_SPECULAR; } else
		if(_stricmp(szTagName,RTOK_OPACITYMAP)==0)	{ m_TextureDesc.TextureName = szContents; m_textureType = RTT_OPACITY; } else
		if(_stricmp(szTagName,RTOK_NORMALMAP)==0)	{ m_TextureDesc.TextureName = szContents; m_textureType = RTT_NORMAL; } else
		if(_stricmp(szTagName,RTOK_REFLECTIONMAP)==0)		{ m_TextureDesc.TextureName = szContents; m_textureType = RTT_REFLECT; } else
		if(_stricmp(szTagName,RTOK_GLOSSINESS)==0)			{ m_TextureDesc.TextureName = szContents; m_textureType = RTT_SHININESS; } else
		if(_stricmp(szTagName,RTOK_SPECULAR_LEVEL)==0)		{ m_TextureDesc.TextureName = szContents; m_textureType = RTT_SPECULARLEVEL; } else
		if(_stricmp(szTagName,RTOK_SELFILLUMINATIONMAP)==0)	{ m_TextureDesc.TextureName = szContents; m_textureType = RTT_SELFILLUMINATION; } else

		if(_stricmp(szTagName,RTOK_TEX_ADDRESS_U)==0) 
		{
			if(_stricmp( szContents,RTOK_TEX_ADDRESS_MIRROR )==0) m_TextureDesc.textureAddressU = RTADDRESS_MIRROR; else 
			if(_stricmp( szContents,RTOK_TEX_ADDRESS_BORDER )==0) m_TextureDesc.textureAddressU = RTADDRESS_BORDER; 
		} else
		if(_stricmp(szTagName,RTOK_TEX_ADDRESS_V)==0) 
		{
			if(_stricmp( szContents,RTOK_TEX_ADDRESS_MIRROR )==0) m_TextureDesc.textureAddressV = RTADDRESS_MIRROR; 
			else if(_stricmp( szContents,RTOK_TEX_ADDRESS_BORDER )==0) m_TextureDesc.textureAddressV = RTADDRESS_BORDER; 
		} else
		if(_stricmp(szTagName,"MAPCHANNEL")==0) sscanf(szContents,"%d",&m_TextureDesc.nMapChannel); else
		if(_stricmp(szTagName,RTOK_TEX_OFFSET)==0)
		{
			RVector2 vOffset;
			sscanf(szContents,"%f %f",&vOffset.x,&vOffset.y);
			m_TextureDesc.UVTransform.SetOffset(vOffset);
		}
		else if(_stricmp(szTagName,RTOK_TEX_SCALE)==0)
		{
			RVector2 vScale;
			sscanf(szContents,"%f %f",&vScale.x,&vScale.y);
			m_TextureDesc.UVTransform.SetScale(vScale);
		}
		else if(_stricmp(szTagName,RTOK_TEX_ANIMATION)==0)
			m_TextureDesc.UVTransform = m_TextureDesc.UVAniInfo.Open(pChildElement);
	}

	return true;
}

void RMtrlTexInfo::Save(MXmlElement *pElement)
{
	if( CheckCreationFlag(RCT_UNREDUCIBLE ) )
		_SetAttribute(pElement, RTOK_UNREDUCIBLE, true );

	MXmlElement* pChildElement = NULL;

	switch(m_textureType)
	{
	case RTT_DIFFUSE : pChildElement = new MXmlElement(RTOK_DIFFUSEMAP);break;
	case RTT_OPACITY : pChildElement = new MXmlElement(RTOK_OPACITYMAP);break;
	case RTT_SPECULAR: pChildElement = new MXmlElement(RTOK_SPECULARMAP);break;
	case RTT_NORMAL: pChildElement = new MXmlElement(RTOK_NORMALMAP);break;
	case RTT_SELFILLUMINATION: pChildElement = new MXmlElement(RTOK_SELFILLUMINATIONMAP);break;
	case RTT_SHININESS: pChildElement = new MXmlElement(RTOK_GLOSSINESS);break;
	case RTT_SPECULARLEVEL: pChildElement = new MXmlElement(RTOK_SPECULAR_LEVEL);break;
	case RTT_REFLECT: pChildElement = new MXmlElement(RTOK_REFLECTIONMAP);break;
	default: pChildElement = new MXmlElement("Unknown");break;
	}
	pElement->LinkEndChild(pChildElement);
	pChildElement->LinkEndChild(new MXmlText(m_TextureDesc.TextureName));

	if(m_TextureDesc.textureAddressU == RTADDRESS_MIRROR || m_TextureDesc.textureAddressU == RTADDRESS_BORDER)
	{
		pChildElement = new MXmlElement(RTOK_TEX_ADDRESS_U);
		pElement->LinkEndChild(pChildElement);

		if(m_TextureDesc.textureAddressU == RTADDRESS_MIRROR)
			pChildElement->LinkEndChild(new MXmlText(RTOK_TEX_ADDRESS_MIRROR));
		else
			if(m_TextureDesc.textureAddressU == RTADDRESS_BORDER)
				pChildElement->LinkEndChild(new MXmlText(RTOK_TEX_ADDRESS_BORDER));
	}

	if(m_TextureDesc.textureAddressV == RTADDRESS_MIRROR || m_TextureDesc.textureAddressV == RTADDRESS_BORDER)
	{
		pChildElement = new MXmlElement(RTOK_TEX_ADDRESS_V);
		pElement->LinkEndChild(pChildElement);

		if(m_TextureDesc.textureAddressV == RTADDRESS_MIRROR)
			pChildElement->LinkEndChild(new MXmlText(RTOK_TEX_ADDRESS_MIRROR));
		else
			if(m_TextureDesc.textureAddressV == RTADDRESS_BORDER)
				pChildElement->LinkEndChild(new MXmlText(RTOK_TEX_ADDRESS_BORDER));
	}


	char buffer[256];

	if(m_TextureDesc.nMapChannel > 0)
	{
		pChildElement = new MXmlElement("MAPCHANNEL");
		sprintf(buffer,"%d", m_TextureDesc.nMapChannel);
		_SetContents(pChildElement, buffer);
		pElement->LinkEndChild(pChildElement);
	}

	if(m_TextureDesc.UVAniInfo.IsHaveAnimationKey())
		m_TextureDesc.UVAniInfo.Save(pElement);
	else
	{
		if(m_TextureDesc.UVTransform.IsDefaultOffset() == false)
		{
			pChildElement = new MXmlElement(RTOK_TEX_OFFSET);
			sprintf(buffer,"%f %f", m_TextureDesc.UVTransform.GetOffset().x, m_TextureDesc.UVTransform.GetOffset().y);
			_SetContents(pChildElement, buffer);
			pElement->LinkEndChild(pChildElement);
		}

		if(m_TextureDesc.UVTransform.IsDefaultScale() == false)
		{
			pChildElement = new MXmlElement(RTOK_TEX_SCALE);
			sprintf(buffer,"%f %f", m_TextureDesc.UVTransform.GetScale().x, m_TextureDesc.UVTransform.GetScale().y);
			_SetContents(pChildElement, buffer);
			pElement->LinkEndChild(pChildElement);
		}
	}
}

}
