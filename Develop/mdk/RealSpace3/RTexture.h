#pragma once

#include "RPreDefine.h"
#include "RTypes.h"
#include "RResource.h"

namespace rs3 {

struct RS_API RImage {
	int		nWidth;
	int		nHeight;
	RFORMAT pixelFormat;

	virtual ~RImage() {}

	virtual bool Save(const char* szFileName) { return false; }
};

struct RS_API RTextureInfo {
	int nTextureWidth, nTextureHeight;
	RFORMAT pixelFormat;
	bool bMipmap;

	RTextureInfo() : nTextureWidth(0), nTextureHeight(0), pixelFormat(RFMT_NONE), bMipmap(false) { }

	unsigned long GetSize() const;
};

class RTextureManager;

class RS_API RTexture : public RResource
{
	friend RTextureManager;
	friend RResourceProvider;
	friend RDevice;
	friend RDeviceD3D;

public:
	RTexture(void);
	virtual ~RTexture(void);


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 텍스쳐가 Load되어있지않을때의 기본픽셀 설정
	//
	virtual void		SetDefaultPixel( RTexture* pSrcTexture )	{}


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// filter / flags setter and getter
	//
	void					SetFilter( RTEXTUREFILTERTYPE _filter)	{_ASSERT( GetState() == RRS_EMPTY ); filter = _filter;}
	void					SetFlags( DWORD _flags )				{_ASSERT( GetState() == RRS_EMPTY ); flags = _flags;}
	RTEXTUREFILTERTYPE		GetFilter()								{ return filter; }
	DWORD					GetFlags()								{ return flags; }
	bool					CheckFlags(DWORD flag)					{ return (flags & flag)!=NULL; }
	const RTextureInfo&		GetInfo() const							{ return info; }

	/// 텍스쳐를 파일로 저장한다. 현재 dds만 지원
	virtual bool			SaveToFile(const char* szFileName, RFORMAT format = RFMT_NONE);

protected:
	bool				m_bCreateFromFile;
	RTEXTUREFILTERTYPE	filter;
	DWORD				flags;
	RTextureInfo		info;
};


class RS_API RTextureCreationInfo : public RAbstractResourceCreationInfo
{
public:
	RTextureCreationInfo(void)
		:m_Width(0),m_Height(0),m_Format(RFMT_NONE),m_Filter(RTF_POINT),m_Flags(0),m_bCreationFromFile(false),m_Pixels(NULL)
	{}
public:
	virtual int GetResourceType(void) override { return RRESOURCETYPE::RR_TEXTURE; }

protected:
	virtual	RAbstractResourceCreationInfo*	AllocateCreationInfo(void)	override { return new RTextureCreationInfo; }
	virtual void					OnClone(RAbstractResourceCreationInfo* pResourceCreationInfoToFill) override
	{
		_ASSERT(NULL != pResourceCreationInfoToFill);
		RTextureCreationInfo* pTextureIdentifierToFill = static_cast<RTextureCreationInfo*>(pResourceCreationInfoToFill);
		pTextureIdentifierToFill->m_Width = this->m_Width;
		pTextureIdentifierToFill->m_Height = this->m_Height;
		pTextureIdentifierToFill->m_Format = this->m_Format;
		pTextureIdentifierToFill->m_Filter = this->m_Filter;
		pTextureIdentifierToFill->m_Flags = this->m_Flags;
		pTextureIdentifierToFill->m_bCreationFromFile = this->m_bCreationFromFile;
		pTextureIdentifierToFill->m_Pixels = this->m_Pixels;
	}

public:
	int					m_Width;
	int					m_Height;
	RFORMAT				m_Format;
	RTEXTUREFILTERTYPE	m_Filter;
	DWORD				m_Flags;
	bool				m_bCreationFromFile;
	void*				m_Pixels;

};

}
