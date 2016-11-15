#include "StdAfx.h"
#include "RMaterialDecal.h"

namespace rs3 {

RMaterialDecal::~RMaterialDecal()
{
	DecalLayerMap::iterator itr =m_DecalLayerMap.begin();
	for(; itr!=m_DecalLayerMap.end(); ++itr)
	{
		DestoryLayer(itr->second);
	}

	m_DecalLayerMap.clear();
}

void RMaterialDecal::SetDecalTexInfo(RMtrlTexInfo* pTexInfo, RTextureDesc desc)
{
	if(pTexInfo==NULL)
	{
		_ASSERT(0);
		return;
	}	

	pTexInfo->SetDesc(desc);

	SetTextureToTexInfo(pTexInfo, desc.TextureName);
}

COMPOSITE_LAYER_INFO RMaterialDecal::CreateDecalLayer(RTextureDesc* pDiffuseDesc, RTextureDesc* pMaskDesc)
{
	COMPOSITE_LAYER_INFO layer;

	SetDecalLayer(&layer, pDiffuseDesc, pMaskDesc);

	return layer;
}

void RMaterialDecal::AddDecalLayer(std::string MaterialName, RTextureDesc* pDiffuseDesc, RTextureDesc* pMaskDesc)
{
	if( COMPOSITE_LAYER_INFO* pLayer = GetDecalLayer(MaterialName) )
		SetDecalLayer(pLayer, pDiffuseDesc, pMaskDesc);
	else
	{
		COMPOSITE_LAYER_INFO layer = CreateDecalLayer(pDiffuseDesc, pMaskDesc);
		m_DecalLayerMap.insert(DecalLayer(MaterialName, layer));
	}
}

void RMaterialDecal::SetDecalLayer(std::string MaterialName, RTextureDesc* pDiffuseDesc, RTextureDesc* pMaskDesc)
{
	COMPOSITE_LAYER_INFO* pLayer = GetDecalLayer(MaterialName);
	if(pLayer==NULL)
		return;

	SetDecalLayer(pLayer, pDiffuseDesc, pMaskDesc);
}

void RMaterialDecal::SetDecalLayer(COMPOSITE_LAYER_INFO* pLayer, RTextureDesc* pDiffuseDesc, RTextureDesc* pMaskDesc)
{
	_ASSERT(pLayer);

	if(pDiffuseDesc)
	{
		if(pLayer->pLayerMap == NULL)
			pLayer->pLayerMap = new RMtrlTexInfo;

		SetDecalTexInfo(pLayer->pLayerMap, *pDiffuseDesc);
	}

	if(pMaskDesc)
	{
		if(pLayer->pMaskMap == NULL)
			pLayer->pMaskMap = new RMtrlTexInfo;

		SetDecalTexInfo(pLayer->pMaskMap, *pMaskDesc);
	}
}

COMPOSITE_LAYER_INFO* RMaterialDecal::GetDecalLayer(std::string MaterialName)
{
	if(MaterialName.empty())
		return NULL;

	DecalLayerMap::iterator itr =m_DecalLayerMap.find(MaterialName);
	if(itr==m_DecalLayerMap.end())
		return NULL;
	else
		return &itr->second;
}

bool RMaterialDecal::HaveDecalLayer(std::string MaterialName)
{
	return m_DecalLayerMap.find(MaterialName) != m_DecalLayerMap.end();
}

void RMaterialDecal::RemoveDecalLayer(std::string MaterialName)
{
	DecalLayerMap::iterator itr =m_DecalLayerMap.find(MaterialName);
	if(itr==m_DecalLayerMap.end())
		return;

	DestoryLayer(itr->second);
	m_DecalLayerMap.erase(itr);
}

void RMaterialDecal::DestoryLayer(COMPOSITE_LAYER_INFO& layer)
{
	if(layer.pLayerMap)
	{
		ReleaseTexture(layer.pLayerMap);
		SAFE_DELETE(layer.pLayerMap);
	}

	if(layer.pMaskMap)
	{
		ReleaseTexture(layer.pMaskMap);
		SAFE_DELETE(layer.pMaskMap);
	}
}

void RMaterialDecal::SetTextureToTexInfo(RMtrlTexInfo* pTexInfo, std::string TextureName)
{
	if(TextureName.empty())
		return;

	_ASSERT(pTexInfo);

	if(pTexInfo->GetTexture())
	{
		if( pTexInfo->GetTexture()->GetName() == TextureName )
			return;
	}

	ReleaseTexture(pTexInfo);
	
	RTexture* pTexture = REngine::GetDevice().CreateTextureForBuildLater(TextureName.c_str(), RTF_LINEAR, RCT_AUTOGENMIPMAP | RCT_UNREDUCIBLE, RFMT_NONE, true);
	pTexInfo->SetTexture(pTexture);
}

void RMaterialDecal::ReleaseTexture(RMtrlTexInfo* pTexInfo)
{
	if(pTexInfo==NULL)
	{
		_ASSERT(0);
		return;
	}
	
	if(pTexInfo->GetTexture()==NULL)
		return;

	REngine::GetDevice().DeleteTexture(pTexInfo->GetTexture());
	pTexInfo->SetTexture(NULL);
}

}