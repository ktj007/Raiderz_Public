#include "stdafx.h"
#include "RShaderConstant.h"
#include "RDeviceD3D.h"
#include "RTechnique.h"


#define CHECK_VALUE 1

namespace rs3
{


RShaderConstant::RShaderConstant() : m_pEffect(NULL), m_hParameter(NULL), m_pValue(NULL), m_eValueType(TYPE_UNKNOWN), m_bChkValue(true), m_uiCounter(0)
{
}

RShaderConstant::~RShaderConstant()
{
	Destroy();
}

void RShaderConstant::Destroy()
{
	SAFE_DELETE(m_pValue);
}

void RShaderConstant::SetValue( VALUE_TYPE eType, const void* pValue)
{
#ifndef CHECK_VALUE
	return;
#endif
	if(!m_bChkValue)
		return;

	/// 카운터가 1 미만. 즉, 처음 값이 셋팅 되었을 경우는 이전 값을 저장해놓지 않는다.
	/// 로컬 블럭 내에서 GetConstant().Set()시 마다 매번 new가 일어 나는 것을 방지하기 위함.
	if( m_uiCounter < 1)
	{
		m_uiCounter = 1;
		return;
	}

	if(!m_pValue)
		CreateValue( eType);

	if( m_eValueType != eType)
		return;

	switch( eType) 
	{
	case TYPE_INT:		*((int*)m_pValue) = *((int*)pValue);			break;
	case TYPE_FLOAT:	*((float*)m_pValue) = *((float*)pValue);		break;
	case TYPE_VECTOR2:	*((RVector2*)m_pValue) = *((RVector2*)pValue);	break;
	case TYPE_VECTOR3:	*((RVector3*)m_pValue) = *((RVector3*)pValue);	break;
 	case TYPE_VECTOR4:	*((RVector4*)m_pValue) = *((RVector4*)pValue);	break;
	case TYPE_MATRIX:	*((RMatrix*)m_pValue) = *((RMatrix*)pValue);	break;
	case TYPE_BOOL:		*((BOOL*)m_pValue) = *((BOOL*)pValue);	break;
	default: return;
	}

}


bool RShaderConstant::IsSame( VALUE_TYPE eType, const void* pValue)
{
#ifndef CHECK_VALUE
	return false;
#endif

	if(!m_bChkValue || m_eValueType != eType || !m_pValue)
		return false;

	switch( eType) 
	{
	case TYPE_INT:		return (memcmp( m_pValue, pValue, sizeof(int)) == 0);
	case TYPE_FLOAT:	return (memcmp( m_pValue, pValue, sizeof(float)) == 0);
	case TYPE_VECTOR2:	return (memcmp( m_pValue, pValue, sizeof(RVector2)) == 0);
	case TYPE_VECTOR3:	return (memcmp( m_pValue, pValue, sizeof(RVector3)) == 0);
 	case TYPE_VECTOR4:	return (memcmp( m_pValue, pValue, sizeof(RVector4)) == 0);
	case TYPE_MATRIX:	return (memcmp( m_pValue, pValue, sizeof(RMatrix)) == 0);
	default: return false;
	}
}


bool RShaderConstant::CreateValue( VALUE_TYPE eType)
{
#ifndef CHECK_VALUE
	return false;
#endif

	if(!m_bChkValue)
		return false;

	switch( eType) 
	{
	case TYPE_INT:		m_pValue = new int;			break;
	case TYPE_FLOAT:	m_pValue = new float;		break;
	case TYPE_VECTOR2:	m_pValue = new RVector2;	break;
	case TYPE_VECTOR3:	m_pValue = new RVector3;	break;
 	case TYPE_VECTOR4:	m_pValue = new RVector4;	break;
	case TYPE_MATRIX:	m_pValue = new RMatrix;		break;
	default: return false;
	}
	m_eValueType = eType;

	return true;
}


bool RShaderConstant::SetInt(int x)
{
	bool returnVal = true;
	if(!IsSame( TYPE_INT, &x))
	{
		RPFN_B( "call SetShaderConstant");
		returnVal = SUCCEEDED(m_pEffect->SetInt(m_hParameter, x));
		RPFN_E( "call SetShaderConstant");	
		if( returnVal)
			SetValue( TYPE_INT, &x);
	}

	return returnVal;
}

bool RShaderConstant::SetIntArray(const int* pArray, int nSize, int nOffset)
{
	m_bChkValue = false;	// 이전 값과의 중복 체크를 더이상 하지 않는다. 
	if(nOffset == 0)
	{
		return SUCCEEDED(m_pEffect->SetIntArray(m_hParameter, pArray, nSize));
	}
	else
	{
		for(int i=0; i<nSize; ++i)
		{
			D3DXHANDLE hParameter = m_pEffect->GetParameterElement(m_hParameter, nOffset+i);
			if( !SUCCEEDED(m_pEffect->SetInt(hParameter, pArray[i])) )
				return false;
		}

		return true;
	}
}

bool RShaderConstant::SetFloat(float x)
{
	bool returnVal = true;
	if(!IsSame( TYPE_FLOAT, &x))
	{
		RPFN_B( "call SetShaderConstant");
		returnVal = SUCCEEDED(m_pEffect->SetFloat(m_hParameter, x));
		RPFN_E( "call SetShaderConstant");	
		if( returnVal)
			SetValue( TYPE_FLOAT, &x);
	}

	return returnVal;
}

bool RShaderConstant::SetVector2(float x, float y)
{
	D3DXVECTOR4 vec;
	vec.x = x;
	vec.y = y;
	vec.z = 0;
	vec.w = 0;

	bool returnVal = true;
	if(!IsSame( TYPE_VECTOR4, &vec))
	{
		RPFN_B( "call SetShaderConstant");
		returnVal = SUCCEEDED(m_pEffect->SetVector(m_hParameter, &vec));
		RPFN_E( "call SetShaderConstant");	
		if( returnVal)
			SetValue( TYPE_VECTOR4, &vec);
	}

	return returnVal;
}

bool RShaderConstant::SetVector2(const RVector2& v)
{
	return SetVector2(v.x, v.y);		
}

bool RShaderConstant::SetVector3(float x, float y, float z)
{
	D3DXVECTOR4 vec;
	vec.x = x;
	vec.y = y;
	vec.z = z;
	vec.w = 0;

	bool returnVal = true;
	if(!IsSame( TYPE_VECTOR4, &vec))
	{
		RPFN_B( "call SetShaderConstant");
		returnVal = SUCCEEDED(m_pEffect->SetVector(m_hParameter, &vec));
		RPFN_E( "call SetShaderConstant");	
		if( returnVal)
			SetValue( TYPE_VECTOR4, &vec);
	}

	return returnVal;
}

bool RShaderConstant::SetVector3(const RVector& v)
{
	RPFC_THISFUNC;
	return SetVector3(v.x, v.y, v.z);
}

bool RShaderConstant::SetVector4(float x, float y, float z, float w)
{
	D3DXVECTOR4 vec;
	vec.x = x;
	vec.y = y;
	vec.z = z;
	vec.w = w;

	bool returnVal = true;
	if(!IsSame( TYPE_VECTOR4, &vec))
	{
		RPFN_B( "call SetShaderConstant");
		returnVal = SUCCEEDED(m_pEffect->SetVector(m_hParameter, &vec));
		RPFN_E( "call SetShaderConstant");	
		if( returnVal)
			SetValue( TYPE_VECTOR4, &vec);
	}

	return returnVal;
}

bool RShaderConstant::SetVector4(const RVector4& v)
{
	return SetVector4(v.x, v.y, v.z, v.w);
}

bool RShaderConstant::SetVector4Array(const RVector4* pArray, int nSize, int nOffset)
{
	m_bChkValue = false;	// 이전 값과의 중복 체크를 더이상 하지 않는다. 
	if(nOffset == 0)
	{
		return SUCCEEDED(m_pEffect->SetVectorArray(m_hParameter, (const D3DXVECTOR4*) pArray, nSize));
	}
	else
	{
		for(int i=0; i<nSize; ++i)
		{
			D3DXHANDLE hParameter = m_pEffect->GetParameterElement(m_hParameter, nOffset+i);
			if( !SUCCEEDED(m_pEffect->SetVector(hParameter, (const D3DXVECTOR4*) pArray)) )
				return false;
		}

		return true;
	}
}


bool RShaderConstant::SetMatrix(const RMatrix& rMatrix, bool bRepetitionCheck)
{
	bool returnVal = true;
	if( !bRepetitionCheck || !IsSame( TYPE_MATRIX, &rMatrix))
	{
		RPFN_B( "call SetShaderConstant");
		returnVal = SUCCEEDED(m_pEffect->SetMatrix(m_hParameter, (const D3DXMATRIX*)&rMatrix));
		RPFN_E( "call SetShaderConstant");	
		if( returnVal)
			SetValue( TYPE_MATRIX, &rMatrix);
	}

	return returnVal;
}


bool RShaderConstant::SetMatrixTranspose(const RMatrix& rMatrix)
{
	m_bChkValue = false;	// 이전 값과의 중복 체크를 더이상 하지 않는다. 
	return SUCCEEDED(m_pEffect->SetMatrixTranspose(m_hParameter, (const D3DXMATRIX*)&rMatrix));
}


bool RShaderConstant::SetMatrixIndex(const RMatrix& rMatrix, int nIndex)
{
	m_bChkValue = false;	// 이전 값과의 중복 체크를 더이상 하지 않는다. 
	D3DXHANDLE hElement = m_pEffect->GetParameterElement(m_hParameter, nIndex);
	_ASSERT(hElement);
	return SUCCEEDED(m_pEffect->SetMatrix(hElement, (const D3DXMATRIX*)&rMatrix)); 
}


bool RShaderConstant::SetMatrixTransposeIndex(const RMatrix& rMatrix, int nIndex)
{
	m_bChkValue = false;	// 이전 값과의 중복 체크를 더이상 하지 않는다. 
	D3DXHANDLE hElement = m_pEffect->GetParameterElement(m_hParameter, nIndex);
	_ASSERT(hElement);
	return SUCCEEDED(m_pEffect->SetMatrixTranspose(hElement, (const D3DXMATRIX*)&rMatrix)); 
}

bool RShaderConstant::SetMatrixArray(const RMatrix* pMatrix,int nCount)
{
	RPFC_THISFUNC;
	m_bChkValue = false;	// 이전 값과의 중복 체크를 더이상 하지 않는다. 
	return SUCCEEDED(m_pEffect->SetMatrixArray(m_hParameter, (const D3DXMATRIX*)pMatrix, nCount));
}

bool RShaderConstant::SetDQuaternionArray(const RDQuaternion* pDQ,int nCount)	
{
	m_bChkValue = false;	// 이전 값과의 중복 체크를 더이상 하지 않는다. 
	return SUCCEEDED(m_pEffect->SetValue( m_hParameter, pDQ, D3DX_DEFAULT));
}

bool RShaderConstant::SetQuaternionArray(const RQuaternion* pQ,int nCount)	
{
	m_bChkValue = false;	// 이전 값과의 중복 체크를 더이상 하지 않는다. 
	return SUCCEEDED(m_pEffect->SetValue( m_hParameter, pQ, D3DX_DEFAULT));
}

bool RShaderConstant::SetVector3Array(const RVector3* pV,int nCount)	
{
	m_bChkValue = false;	// 이전 값과의 중복 체크를 더이상 하지 않는다. 
	return SUCCEEDED(m_pEffect->SetValue( m_hParameter, pV, D3DX_DEFAULT));
}

bool RShaderConstant::SetTexture(RTexture* hTex, const RDevice* pDevice)
{
	m_bChkValue = false;	// 이전 값과의 중복 체크를 더이상 하지 않는다. 
	_ASSERT( pDevice );

	LPDIRECT3DBASETEXTURE9 lpTexture = NULL;
	if ( hTex == R_NONE )
	{
		lpTexture = NULL;
	}
	else
	{
		 lpTexture = ((RDeviceD3D*)pDevice)->GetD3DTexture( hTex );
	}
	return SUCCEEDED(m_pEffect->SetTexture(m_hParameter, lpTexture));
}

//// by pok, 새로운 텍스쳐에 대응하는 쉐이더
//bool RShaderConstant::SetTexture( RTexture_x* _pTexture)
//{
//	return SUCCEEDED(m_pEffect->SetTexture(m_hParameter, _pTexture ? static_cast< RDeviceD3D::RTextureD3D_* >( _pTexture )->texture : NULL ) );
//}

};