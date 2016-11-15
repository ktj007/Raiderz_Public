#pragma once

#include "RDevice.h"
#include "RTypes.h"
#include "D3dx9shader.h"

namespace rs3
{

class RShaderFX;

enum VALUE_TYPE {
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_VECTOR2,
	TYPE_VECTOR3,
	TYPE_VECTOR4,
	TYPE_MATRIX,
	TYPE_BOOL,
	TYPE_UNKNOWN,
};

class RS_API RShaderConstant
{
public:
		RShaderConstant();
		~RShaderConstant();

		bool				SetInt(int x);
		bool				SetIntArray(const int* pArray, int nSize, int nOffset =0);
		bool				SetFloat(float x);
		bool				SetVector2(float x, float y);
		bool				SetVector2(const RVector2& v);
		bool				SetVector3(float x, float y, float z);
		bool				SetVector3(const RVector& v);
		bool				SetVector4(float x, float y, float z, float w);
		bool                SetVector4(const RVector4& v);
		bool                SetVector4Array(const RVector4* pArray, int nSize, int nOffset = 0);
		bool                SetMatrix(const RMatrix& rMatrix, bool bRepetitionCheck = true);
		bool                SetMatrixTranspose(const RMatrix& rMatrix);
		bool                SetMatrixIndex(const RMatrix& rMatrix, int nIndex);
		bool                SetMatrixTransposeIndex(const RMatrix& rMatrix, int nIndex);
		bool                SetMatrixArray(const RMatrix* pMatrix,int nCount);
		bool                SetDQuaternionArray(const RDQuaternion* pDQ,int nCount);	
		bool                SetQuaternionArray(const RQuaternion* pDQ,int nCount);		
		bool                SetVector3Array(const RVector3* pDQ,int nCount);				

		bool				SetTexture(RTexture* hTex, const RDevice* pDevice);

		bool				IsValid(void);
		string				GetName(void) { return m_strName; }


		LPD3DXEFFECT		m_pEffect;
		D3DXHANDLE			m_hParameter;

		string				m_strName;

		void*				m_pValue;
		VALUE_TYPE			m_eValueType;
		bool				m_bChkValue;
		/// 두번 이상 값이 설정 되었는지의 체크를 위한 카운터. 그러므로 무한히 올라갈필요는 없다. 고로 사이즈도 클 필요 없음.
		BYTE				m_uiCounter;

		void				SetValue( VALUE_TYPE eType, const void* pValue);
		bool				IsSame( VALUE_TYPE eType, const void* pValue);
		bool				CreateValue( VALUE_TYPE eType);
		void				Destroy();
};


};