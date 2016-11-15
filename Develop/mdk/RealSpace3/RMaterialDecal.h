#pragma once

namespace rs3 {

class RS_API RMaterialDecal
{
public:
	RMaterialDecal() {}
	~RMaterialDecal();

	void					AddDecalLayer(std::string MaterialName, RTextureDesc* pDiffuseDesc, RTextureDesc* pMaskDesc);
	void					SetDecalLayer(std::string MaterialName, RTextureDesc* pDiffuseDesc, RTextureDesc* pMaskDesc);
	COMPOSITE_LAYER_INFO*	GetDecalLayer(std::string MaterialName);
	bool					HaveDecalLayer(std::string MaterialName);
	void					RemoveDecalLayer(std::string MaterialName);

	size_t					GetDecalLayerCount() { return m_DecalLayerMap.size(); }

	COMPOSITE_LAYER_INFO	CreateDecalLayer(RTextureDesc* pDiffuseDesc, RTextureDesc* pMaskDesc);
	void					DestoryLayer(COMPOSITE_LAYER_INFO& layer);

private:
	void					SetDecalLayer(COMPOSITE_LAYER_INFO* pLayer, RTextureDesc* pDiffuseDesc, RTextureDesc* pMaskDesc);
	void					SetDecalTexInfo(RMtrlTexInfo* pTexInfo, RTextureDesc desc);
	void					SetTextureToTexInfo(RMtrlTexInfo* pTexInfo, std::string TextureName);
	void					ReleaseTexture(RMtrlTexInfo* pTexInfo);	


	typedef std::map<std::string, COMPOSITE_LAYER_INFO> DecalLayerMap;
	typedef std::pair<std::string, COMPOSITE_LAYER_INFO> DecalLayer;
	DecalLayerMap			m_DecalLayerMap;
};

}