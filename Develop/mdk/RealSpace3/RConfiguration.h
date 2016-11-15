#pragma once

#include <string>
#include "RTypes.h"
#include "RPathSplitter.h"

using namespace std;

namespace rs3 {

class RS_API RConfiguration
{
private:
	string  m_strDataPath;

	float	m_fSpeedTreeCompositeTextureMipmapLodBias;	///< SpeedTree Texture Mipmap bias
	float	m_fSpeedGrassTextureMipmapLodBias;			///< SpeedGrass Texture Mipmap bias

	RTEXTUREFILTERTYPE	m_TextureFilter;
	DWORD				m_dwTextureMaxAnisotropy;

	string MakePath(const char *szName);

	void SetVideo(MXmlElement *pElement);
public:
	RConfiguration();

	bool	m_bFullScreen;
	int		m_nInitX;				///< 윈도우모드에서 윈도우 초기 X위치
	int		m_nInitY;				///< 윈도우모드에서 윈도우 초기 Y위치
	int		m_nWidth;
	int		m_nHeight;
	int		m_nTextureReduction;	///< 텍스쳐해상도 강제 낮춤 옵션
	float	m_fGlobalTextureMipmapLodBias;	///< 전역 텍스쳐 Mipmap bias
	bool	m_bUsingShader;

	bool	m_bLogResource;						///< 리소스 로딩 시간을 체크할 지 여부
	bool	m_bUsingCompiledShader;				///< 만약 컴파일된 쉐이더가 있으면 컴파일된 쉐이더로부터 읽어올지 여부
	bool	m_bUsingCompiledShaderCaching;		///< 컴파일된 쉐이더 파일 캐슁할지 여부
	string	m_strCompiledShaderCachePath;

	bool	m_bBackgroundCreateShaderComposite;	///< 셰이더 조합 리소스를 백그라운드 생성 할 것인가.

	RFORMAT	pixelFormat;
	RFORMAT	depthFormat;

	void SetDataPath(const char* szPath);
 	string GetDataPath( const char *DataFilename);

	float	GetSpeedTreeCompositeTextureMipmapLodBias() const { return m_fSpeedTreeCompositeTextureMipmapLodBias; }
	void	SetSpeedTreeCompositeTextureMipmapLodBias(float val) { m_fSpeedTreeCompositeTextureMipmapLodBias = val; }

	float	GetSpeedGrassTextureMipmapLodBias() const { return m_fSpeedGrassTextureMipmapLodBias; }
	void	SetSpeedGrassTextureMipmapLodBias(float val) { m_fSpeedGrassTextureMipmapLodBias = val; }

	void				SetTextureFilter( RTEXTUREFILTERTYPE filter)	{ m_TextureFilter = filter; }
	RTEXTUREFILTERTYPE	GetTextureFilter()	{ return m_TextureFilter; }

	void	SetTextureMaxAnisotropy( DWORD maxAnisotropy)	{ m_dwTextureMaxAnisotropy = maxAnisotropy; }
	DWORD	GetTextureMaxAnisotropy()	{ return m_dwTextureMaxAnisotropy; }

	bool LoadFromXMLFile(const char *filename);
};

#define COMPILED_SHADER_FOLDER_PATH			"obj/"
#define COMPILED_SHADER_FILENAME_EXT		".fxo"

}
