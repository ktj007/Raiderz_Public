#pragma once

#include "RDevice.h"
#include "RTypes.h"
#include "RShaderConstant.h"
#include "RTechnique.h"
#include "RResource.h"
#include <D3dx9shader.h>

namespace rs3
{

class RS_API RShaderFX : public RResource
{
public:
		RShaderFX( LPDIRECT3DDEVICE9 pDevice );
		~RShaderFX( );

		bool SetFromString(const string& strShader, const vector<string>& vShaderDefines, RShaderFXPool *pShaderPool, const char* szCachingFile=NULL);
		bool SetFromFile(const string& strFilename, const vector<string>& vShaderDefines, RShaderFXPool *pShaderPool);

		RTechnique			GetTechniqueByIndex(unsigned int index) const;
        RTechnique			GetTechnique(const char* pName) const;
        RShaderConstant		GetConstant(const char* pName) const;

        bool                CommitChanges(void) const;

		LPDIRECT3DDEVICE9	m_pd3dDevice;
        LPD3DXEFFECT        m_pEffect;

		void OnLostDevice();
		void OnResetDevice();

		virtual const char*			GetDebugName();
protected:
	virtual bool		Fill() override;
	virtual LOAD_RESULT Load() override;

	void					SaveCacheFile();
	bool					LoadShaderFile();
	void					LoadCachingFile();
	void					SetupShaderMacros();
	void					SetupShaderFlags();

	bool					OpenShaderFile( MFile& file );
	void					MakeShaderStringFromFile( MFile& file );

	string					m_strFilename;
	string					m_strFullFilename;
	string					m_strShaderString;
	vector<string>			m_vShaderDefines;
	RShaderFXPool*			m_pShaderPool;

	LPD3DXBUFFER			m_pCompiledEffectBuffer;
	LPD3DXBUFFER			m_pErrorMsgs;
	D3DXMACRO*				m_pShaderMacros;
	DWORD					m_dwShaderFlags;

	// Compiled
	bool					m_bOpenCompiledShaderFile;
	char*					m_pCompiledEffectFileBuffer;
	unsigned int			m_nCompiledEffectFileBufferSize;
	string					m_strCachingFileName;

	bool					CompileEffect();
	bool					IsSettingFromFile();
};

class RS_API RShaderFXPool
{
protected:
	LPD3DXEFFECTPOOL m_pEffectPool;
public:
	RShaderFXPool();
	~RShaderFXPool();

	LPD3DXEFFECTPOOL GetEffectPool();
};

};