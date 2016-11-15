#pragma once

#include "RUVTransform.h"

namespace rs3 {

enum RTextureType {
	RTT_NONE = -1,
	RTT_UNKNOWN = 0,
	RTT_DIFFUSE,
	RTT_OPACITY,
	RTT_SPECULAR,
	RTT_NORMAL,
	RTT_SELFILLUMINATION,
	RTT_REFLECT,
	RTT_SHININESS,
	RTT_SPECULARLEVEL,
	RTT_MAX
};

// 런타임용 멤버 키 트래커에 복사생성
#define COPY_IF_EXIST( _dest, _src )	if(_src.size()) {	\
	_dest.Create( _src.size() );	\
	for(size_t i=0;i<_src.size(); i++)	\
	_dest.at(i) = _src[i];	\
	}




struct RTextureDesc
{
	RTextureDesc() : textureAddressU(RTADDRESS_WRAP), textureAddressV(RTADDRESS_WRAP), nMapChannel(0)
	{}

	void UpdateUVAnimation(UINT nFrame)
	{
		UVTransform = UVAniInfo.GetUVTransform(nFrame);
	}

	bool operator==(RTextureDesc& other)
	{
		return TextureName==other.TextureName &&
			textureAddressU==other.textureAddressU &&
			textureAddressV==other.textureAddressV &&
			nMapChannel==other.nMapChannel &&
			UVTransform==other.UVTransform &&
			UVAniInfo==other.UVAniInfo;
	}

	std::string TextureName;

	RTEXTUREADDRESS	textureAddressU;
	RTEXTUREADDRESS textureAddressV;

	int nMapChannel;

	RUVTransform UVTransform;
	RUVAnimationInfo UVAniInfo;	
};

class RS_API RMtrlTexInfo
{
public:
	RMtrlTexInfo();
	~RMtrlTexInfo();

	bool				Open(MXmlElement *pElement);
	void				Save(MXmlElement *pElement);

	RTextureType		GetTextureType() { return m_textureType; }
	RTexture*			GetTexture()	{ return m_hTexture; }
	void				SetTexture(RTexture* pTexture)	{ m_hTexture = pTexture; }
	DWORD				GetCreationFlag() { return dwCreationFlag; }
	bool				CheckCreationFlag(DWORD flag) { return (dwCreationFlag & flag) != 0; }

	void				UpdateUVAnimation(UINT nFrame) { m_TextureDesc.UpdateUVAnimation(nFrame); }


	RTextureDesc		GetDesc() const { return m_TextureDesc; }
	std::string			GetTextureName() { return m_TextureDesc.TextureName; }
	bool				IsDefaultUVTranform()	{ return m_TextureDesc.UVTransform.IsDefault(); }
	bool				IsUVAnimation() { return m_TextureDesc.UVAniInfo.IsHaveAnimationKey(); }
	RUVTransform		GetUVTranform()	{ return m_TextureDesc.UVTransform; }
	int					GetMapChannel()	{ return m_TextureDesc.nMapChannel; }
	RTEXTUREADDRESS		GetTextureAddressU() { return m_TextureDesc.textureAddressU; }
	RTEXTUREADDRESS		GetTextureAddressV() { return m_TextureDesc.textureAddressV; }
	RUVAnimationInfo&	GetUVAnimationRef() { return m_TextureDesc.UVAniInfo; }

	void				SetDesc(RTextureDesc desc)	{ m_TextureDesc=desc; }
	void				SetTextureName(std::string name) { m_TextureDesc.TextureName=name; }
	void				SetTextureType(RTextureType type)	{ m_textureType=type; }
	void				SetUVTransform(RUVTransform UVTrans) { m_TextureDesc.UVTransform =UVTrans; }
	void				SetMapChannel(int nMapChannel) { m_TextureDesc.nMapChannel=nMapChannel; }
	void				SetTextureAddressU(RTEXTUREADDRESS textureAddress) { m_TextureDesc.textureAddressU=textureAddress; }
	void				SetTextureAddressV(RTEXTUREADDRESS textureAddress) { m_TextureDesc.textureAddressV=textureAddress; }

	bool operator==(RMtrlTexInfo& other)
	{
		return this->GetDesc()==other.GetDesc() &&
			this->GetTextureType()==other.GetTextureType() &&
			this->GetTexture()==other.GetTexture() &&
			this->GetCreationFlag()==other.GetCreationFlag();
	}

private:
	RTextureDesc		m_TextureDesc;

	RTextureType		m_textureType;
	RTexture*			m_hTexture;
	DWORD				dwCreationFlag;	
};

}