#include "stdafx.h"
#include "RCompositeLayer.h"
#include "RToken.h"

namespace rs3 {


RCompositeLayer::RCompositeLayer() : m_textureType(RTT_NONE)
{
}

RCompositeLayer::~RCompositeLayer()
{	
}

void RCompositeLayer::Open(MXmlElement *pElement, RMaterial* pMaterial)
{
	char szContents[256];

	int nIndex =0;
	pElement->Attribute(RTOK_INDEX, &nIndex);
	m_textureType =(RTextureType)nIndex;

	MXmlElement* pLayerElement = pElement->FirstChildElement("LAYER");
	for(; pLayerElement != NULL; pLayerElement = pLayerElement->NextSiblingElement("LAYER"))
	{
		COMPOSITE_LAYER_INFO LayerInfo;

		MXmlElement* pChildElement = NULL;

		if(pChildElement = pLayerElement->FirstChildElement("LAYERMAP"))
		{
			_Contents(szContents, pChildElement);
			LayerInfo.pLayerMap = pMaterial->GetTexLayerByNameFromType(m_textureType, szContents);
		}

		if(pChildElement = pLayerElement->FirstChildElement("MASKMAP"))
		{
			_Contents(szContents, pChildElement);
			LayerInfo.pMaskMap = pMaterial->GetTexLayerByNameFromType(m_textureType, szContents);
		}

		if(pChildElement = pLayerElement->FirstChildElement("OPACITY"))
			_Contents(&LayerInfo.fOpacity, pChildElement);

		if(pChildElement = pLayerElement->FirstChildElement("BLENDMODE"))
			_Contents((int*)&LayerInfo.BlendMode, pChildElement);

		m_LayerInfo.push_back(LayerInfo);
	}
}

void RCompositeLayer::Save(MXmlElement *pElement)
{
	for(size_t i=0; i<m_LayerInfo.size(); ++i)
	{
		MXmlElement* pLayerElement = new MXmlElement("LAYER");
		_SetAttribute(pLayerElement, RTOK_INDEX, (int)i);

		COMPOSITE_LAYER_INFO LayerInfo = m_LayerInfo[i];

		if(LayerInfo.pLayerMap)
		{
			MXmlElement* pLayerMapElement = new MXmlElement("LAYERMAP");
			pLayerMapElement->LinkEndChild(new MXmlText(LayerInfo.pLayerMap->GetTextureName()));
			pLayerElement->LinkEndChild(pLayerMapElement);
		}

		if(LayerInfo.pMaskMap)
		{
			MXmlElement* pMaskMapElement = new MXmlElement("MASKMAP");
			pMaskMapElement->LinkEndChild(new MXmlText(LayerInfo.pMaskMap->GetTextureName()));
			pLayerElement->LinkEndChild(pMaskMapElement);
		}

		MXmlElement* pOpacityElement = new MXmlElement("OPACITY");
		_SetContents(pOpacityElement, LayerInfo.fOpacity);
		pLayerElement->LinkEndChild(pOpacityElement);

		MXmlElement* pBlendModeElement = new MXmlElement("BLENDMODE");
		_SetContents(pBlendModeElement, (int)LayerInfo.BlendMode);
		pLayerElement->LinkEndChild(pBlendModeElement);


		pElement->LinkEndChild(pLayerElement);
	}
}

}