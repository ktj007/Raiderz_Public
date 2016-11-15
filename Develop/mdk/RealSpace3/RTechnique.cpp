#include "stdafx.h"
#include "RTechnique.h"

namespace rs3
{

RDevice* RTechnique::m_pDevice = NULL;
bool RTechnique::m_bUsing = false;
TECHNIQUE_PRESERVE_STATE RTechnique::m_ePreserveStates = TECH_PRESERVE_NOT;
RTechnique* RTechnique::m_pUsingTechnique = NULL;
UINT RTechnique::m_nUsingPass = 0;

///////////////////////////////////////////////////////////////////////  
//  RTechnique::RTechnique

RTechnique::RTechnique( ) 
: m_pEffect(NULL) 
, m_hTechnique(NULL)
{
}


///////////////////////////////////////////////////////////////////////  
//  RTechnique::~RTechnique

RTechnique::~RTechnique( )
{
}


///////////////////////////////////////////////////////////////////////  
//  RTechnique::Begin
// bPreserveStates : device 의 state 를 보존해 줄것인지 여부
bool RTechnique::Begin(UINT* pPasses, enum TECHNIQUE_PRESERVE_STATE ePreserveStates )
{
	bool bSuccess = false;
	DWORD dwFlags = 0;
	switch( ePreserveStates)
	{
	case TECH_PRESERVE_NOT: dwFlags = D3DXFX_DONOTSAVESTATE; break;
	case TECH_PRESERVE_ALL: dwFlags = 0; break;
	case TECH_PRESERVE_EXCEPT_SAMPLER: dwFlags = D3DXFX_DONOTSAVESAMPLERSTATE; break;
	case TECH_PRESERVE_EXCEPT_SHADER: dwFlags = D3DXFX_DONOTSAVESHADERSTATE; break;
	}

	m_ePreserveStates = ePreserveStates;

	_ASSERT(m_bUsing == false);

	if (SUCCEEDED(m_pEffect->SetTechnique(m_hTechnique)))
	{
		if (SUCCEEDED(m_pEffect->Begin(pPasses, dwFlags)))
		{
			m_bUsing = true;
			m_pUsingTechnique = this;
			bSuccess = true;
		}
	}

	_ASSERT(bSuccess);
	return bSuccess;
}
 
bool RTechnique::BeginPass(UINT nPass)
{
	m_nUsingPass = nPass;

	bool bSuccess = false;

	if (SUCCEEDED(m_pEffect->BeginPass(nPass)))
	{
		bSuccess = true;
	}

	_ASSERT(bSuccess);
	return bSuccess;
}

bool RTechnique::EndPass()
{
	bool bSuccess = false;

	if (SUCCEEDED(m_pEffect->EndPass()))
	{
		bSuccess = true;
	}

	_ASSERT(bSuccess);
	return bSuccess;
}

///////////////////////////////////////////////////////////////////////  
//  RTechnique::Begin

bool RTechnique::End(void)
{
	bool bSuccess = false;

	if (SUCCEEDED(m_pEffect->End( )))
	{
		m_bUsing = false;
		m_pUsingTechnique = NULL;
		bSuccess = true;
	}

	_ASSERT(bSuccess);
	return bSuccess;
}

void RTechnique::CommitChanges()
{
	if (m_pEffect)
	{
		m_pEffect->CommitChanges();
	}
}

// Gets the handle of a pass.
D3DXHANDLE RTechnique::GetPass(UINT nPass)
{
	return m_pEffect->GetPass(m_hTechnique, nPass);
}

// Gets a pass description.
// description 정보에는 PassName과 주석의 개수(?) VS, PS의 FucntionPointer 등의 정보가 있다
bool RTechnique::GetPassDesc(UINT nPass, D3DXPASS_DESC* pDesc)
{
	bool bSuccess = false;

	if (SUCCEEDED(m_pEffect->GetPassDesc( GetPass(nPass), pDesc )))
	{
		bSuccess = true;
	}

	_ASSERT(bSuccess);
	return bSuccess;
}

// D3DXPASS_DESC 의 PassName을 얻는다.
bool RTechnique::GetPassDescName(UINT nPass, char *pPassName)
{
	D3DXPASS_DESC description;
	
	bool bSuccess = GetPassDesc(nPass, &description);

	strcpy(pPassName, description.Name);

	return bSuccess;
}


/// sampler index 를 얻어온다, 없으면 -1 리턴
unsigned int RTechnique::GetSamplerIndex( int nPass , const char* szConstant)
{
	D3DXPASS_DESC descPass;

	HRESULT hr;
	D3DXHANDLE hPass = m_pEffect->GetPass( m_hTechnique, nPass );
	hr = m_pEffect->GetPassDesc( hPass, &descPass);
	LPD3DXCONSTANTTABLE pConstTable;
	D3DXGetShaderConstantTable(descPass.pPixelShaderFunction, &pConstTable);

	/* debug 용 const enum 방법
	D3DXCONSTANTTABLE_DESC descConstTable;
	pConstTable->GetDesc(&descConstTable);
	for(unsigned int i=0;i<descConstTable.Constants;i++)
	{
		D3DXHANDLE hConst = pConstTable->GetConstant(NULL,i);
		UINT nCount = 10;
		D3DXCONSTANT_DESC descConst[10];
		pConstTable->GetConstantDesc( hConst , descConst, &nCount);
	}
	//*/


	D3DXHANDLE hConst = pConstTable->GetConstantByName( NULL, szConstant );
	if(!hConst)
		return R_SAMPLER_INDEX_INVALID;
	unsigned int nIndex = pConstTable->GetSamplerIndex(hConst);
	return nIndex;
}

bool RTechnique::IsEqual( const RTechnique& technique ) const
{
	return ( m_hTechnique == technique.m_hTechnique );
}

};