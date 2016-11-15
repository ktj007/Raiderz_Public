#pragma once

#include "RDevice.h"
#include "RTypes.h"
#include <D3dx9shader.h>

namespace rs3
{

enum TECHNIQUE_PRESERVE_STATE
{
	TECH_PRESERVE_NOT = 0,
	TECH_PRESERVE_ALL = 1,
	TECH_PRESERVE_EXCEPT_SAMPLER,
	TECH_PRESERVE_EXCEPT_SHADER,
	TECH_PRESERVE_COUNT
};

class RS_API RTechnique
{
public:
		RTechnique( );
		~RTechnique( );

static	RDevice*			m_pDevice;
static	bool				m_bUsing;
static	TECHNIQUE_PRESERVE_STATE m_ePreserveStates;	// 상태를 저장해야 하는지
static	RTechnique*			m_pUsingTechnique;
static	UINT				m_nUsingPass;
		
        bool				Begin(UINT* pPasses = 0, enum TECHNIQUE_PRESERVE_STATE ePreserveStates = TECH_PRESERVE_ALL);
		bool				BeginPass(UINT nPass = 0);
		bool				EndPass(void);
		bool				End(void);

		void				CommitChanges();

		D3DXHANDLE			GetPass(UINT nPass);
		bool				GetPassDesc(UINT nPass, D3DXPASS_DESC* pDesc);
		bool				GetPassDescName(UINT nPass, char *pPassName);
		unsigned int		GetSamplerIndex( int nPass , const char* szConstant);

		bool				IsEqual(const RTechnique& technique) const;

        LPD3DXEFFECT		m_pEffect;
        D3DXHANDLE			m_hTechnique;
};

typedef std::map< int, RTechnique> TECHNIQUE_MAP;

};