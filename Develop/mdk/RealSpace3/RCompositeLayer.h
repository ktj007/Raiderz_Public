#pragma once

namespace rs3 {

enum COMPOSITE_LAYER_BLEND_MODE
{
	LAYER_BLEND_NORMAL =0,
	LAYER_BLEND_AVERAGE,
	LAYER_BLEND_ADD,
	LAYER_BLEND_SUBTRACT,
	LAYER_BLEND_DARKEN,
	LAYER_BLEND_MULTIPLY,
	LAYER_BLEND_COLORBURN,
	LAYER_BLEND_LINEARBURN,
	LAYER_BLEND_LIGHTEN,
	LAYER_BLEND_SCREEN,
	LAYER_BLEND_COLORDODGE,
	LAYER_BLEND_LINEARDODGE,
	LAYER_BLEND_SPOTLIGHT,
	LAYER_BLEND_SPOTLIGHTBLEND,
	LAYER_BLEND_OVERLAY,
	LAYER_BLEND_SOFTLIGHT,
	LAYER_BLEND_HARDLIGHT,
	LAYER_BLEND_PINLIGHT,
	LAYER_BLEND_HARDMIX,
	LAYER_BLEND_DIFFERENCE,
	LAYER_BLEND_EXCLUSION,
	LAYER_BLEND_HUE,
	LAYER_BLEND_SATURATION,
	LAYER_BLEND_COLOR,
	LAYER_BLEND_VALUE,
	LAYER_BLEND_MAX
};

struct COMPOSITE_LAYER_INFO
{
	COMPOSITE_LAYER_INFO()
	{
		pLayerMap = pMaskMap = NULL;
		fOpacity = 1.0f;
		BlendMode = LAYER_BLEND_NORMAL;
	}

	bool operator==(const COMPOSITE_LAYER_INFO& other)
	{
		return (*this->pLayerMap == *other.pLayerMap) &&
			(*this->pMaskMap == *other.pMaskMap) &&
			(this->fOpacity == other.fOpacity) &&
			(this->BlendMode == other.BlendMode);
	}

	RMtrlTexInfo* pLayerMap;
	RMtrlTexInfo* pMaskMap;

	float fOpacity;

	COMPOSITE_LAYER_BLEND_MODE BlendMode;
};

class RCompositeLayer
{
public:
	RCompositeLayer();
	~RCompositeLayer();

	void Open(MXmlElement *pElement, RMaterial* pMaterial);
	void Save(MXmlElement *pElement);

	void AddLayer(COMPOSITE_LAYER_INFO info)				{ m_LayerInfo.push_back(info); }
	void SetTextureType(RTextureType type)					{ m_textureType = type; }


	RTextureType GetTextureType()							{ return m_textureType; }
	int GetLayerCount()										{ return m_LayerInfo.size(); }
	const COMPOSITE_LAYER_INFO& GetLayerInfo(int nIndex)	{ return m_LayerInfo[nIndex]; }
	COMPOSITE_LAYER_BLEND_MODE GetBlendingMode(int nIndex)	{ return m_LayerInfo[nIndex].BlendMode; }

private:
	RTextureType m_textureType;

	std::vector<COMPOSITE_LAYER_INFO> m_LayerInfo;
};

}