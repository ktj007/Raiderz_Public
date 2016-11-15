#include "stdafx.h"
#include "MCommandParameter.h"
#include "MLocale.h"

namespace minet {

MCommandParameter* NewParameter(MCommandParameterType nParamType)
{
	MCommandParameter* pNewParam = NULL;

	switch(nParamType)
	{
	case MPT_INT:
		pNewParam = new MCommandParameterInt();
		break;
	case MPT_UINT:
		pNewParam = new MCommandParameterUInt();
		break;
	case MPT_FLOAT:
		pNewParam = new MCommandParameterFloat();
		break;
	case MPT_STR:
		pNewParam = new MCommandParameterString();
		break;
	case MPT_VECTOR:
		pNewParam = new MCommandParameterVector();
		break;
	case MPT_BOOL:
		pNewParam = new MCommandParameterBool();
		break;
	case MPT_UID:
		pNewParam = new MCommandParameterUID();
		break;
	case MPT_ID:
		pNewParam = new MCommandParameterID();
		break;
	case MPT_BLOB:
		pNewParam = new MCommandParameterBlob();
		break;
	case MPT_SINGLE_BLOB:
		pNewParam = new MCommandParameterSingleBlob();
		break;
	case MPT_CHAR:
		pNewParam = new MCommandParameterChar();
		break;
	case MPT_UCHAR:
		pNewParam = new MCommandParameterUChar();
		break;
	case MPT_SHORT:
		pNewParam = new MCommandParameterShort();
		break;
	case MPT_USHORT:
		pNewParam = new MCommandParameterUShort();
		break;
	case MPT_INT64:
		pNewParam = new MCommandParameterInt64();
		break;
	case MPT_UINT64:
		pNewParam = new MCommandParameterUInt64();
		break;
	case MPT_SVEC:
		pNewParam = new MCommandParameterShortVector();
		break;
	case MPT_SVEC2:
		pNewParam = new MCommandParameterShortVector2();
		break;
	case MPT_WSTR:
		pNewParam = new MCommandParameterWideString();
		break;
	default:
		_ASSERT(false);		// Unknow Parameter!!!
		break;
	}

	return pNewParam;

}

bool MCommandParamConditionMinMax::Check(MCommandParameter* pCP)
{
	switch (pCP->GetType())
	{
	case MPT_INT:
		{
			int nValue;
			pCP->GetValue(&nValue);
			if ((nValue < m_nMin) || (nValue > m_nMax)) return false;
		}
		break;
	case MPT_UINT:
		{
			unsigned int nValue;
			pCP->GetValue(&nValue);
			if ((nValue < (unsigned int)m_nMin) || (nValue > (unsigned int)m_nMax)) return false;
		}
		break;
	case MPT_CHAR:
		{
			char nValue;
			pCP->GetValue(&nValue);
			if ((nValue < (char)m_nMin) || (nValue > (char)m_nMax)) return false;
		}
		break;
	case MPT_UCHAR:
		{
			unsigned char nValue;
			pCP->GetValue(&nValue);
			if ((nValue < (unsigned char)m_nMin) || (nValue > (unsigned char)m_nMax)) return false;
		}
		break;
	case MPT_SHORT:
		{
			short nValue;
			pCP->GetValue(&nValue);
			if ((nValue < (short)m_nMin) || (nValue > (short)m_nMax)) return false;
		}
		break;
	case MPT_USHORT:
		{
			unsigned short nValue;
			pCP->GetValue(&nValue);
			if ((nValue < (unsigned short)m_nMin) || (nValue > (unsigned short)m_nMax)) return false;
		}
		break;
	default:
		{
			_ASSERT(0); // 없는 제약조건이다.
		}
	};

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterDesc ////////////////////////////////////////////////////////////////////////
MCommandParameterDesc::MCommandParameterDesc(MCommandParameterType nType, char* szDescription)
{
	m_nType = nType;
	strcpy_s(m_szDescription, szDescription);
}

void MCommandParameterDesc::InitializeConditions()
{
	for(int i=0; i<(int)m_Conditions.size(); i++){
		SAFE_DELETE(m_Conditions[i]);
	}
	m_Conditions.clear();
}

MCommandParameterDesc::~MCommandParameterDesc(void)
{
	InitializeConditions();
}

void MCommandParameterDesc::AddCondition(MCommandParamCondition* pCondition)
{
	m_Conditions.push_back(pCondition);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterInt /////////////////////////////////////////////////////////////////////////
MCommandParameterInt::MCommandParameterInt(void)
 : MCommandParameter(MPT_INT)
{
	m_Value = 0;
}
MCommandParameterInt::MCommandParameterInt(int Value)
 : MCommandParameter(MPT_INT)
{
	m_Value = Value;
}
MCommandParameter* MCommandParameterInt::Clone(void)
{
	return (new MCommandParameterInt(m_Value));
}
void MCommandParameterInt::GetValue(void* p)
{
	*(int*)p = m_Value;
}
int MCommandParameterInt::GetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int MCommandParameterInt::SetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterUInt ////////////////////////////////////////////////////////////////////////
MCommandParameterUInt::MCommandParameterUInt(void)
: MCommandParameter(MPT_UINT)
{
	m_Value = 0;
}
MCommandParameterUInt::MCommandParameterUInt(unsigned int Value)
: MCommandParameter(MPT_UINT)
{
	m_Value = Value;
}
MCommandParameter* MCommandParameterUInt::Clone(void)
{
	return (new MCommandParameterUInt(m_Value));
}
void MCommandParameterUInt::GetValue(void* p)
{
	*(unsigned int*)p = m_Value;
}
int MCommandParameterUInt::GetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int MCommandParameterUInt::SetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterFloat ///////////////////////////////////////////////////////////////////////
MCommandParameterFloat::MCommandParameterFloat(void)
 : MCommandParameter(MPT_FLOAT)
{
	m_Value = 0;
}
MCommandParameterFloat::MCommandParameterFloat(float Value)
 : MCommandParameter(MPT_FLOAT)
{
	m_Value = Value;
}
MCommandParameter* MCommandParameterFloat::Clone(void)
{
	return (new MCommandParameterFloat(m_Value));
}
void MCommandParameterFloat::GetValue(void* p)
{
	*(float*)p = m_Value;
}
int MCommandParameterFloat::GetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int MCommandParameterFloat::SetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterDouble //////////////////////////////////////////////////////////////////////
MCommandParameterDouble::MCommandParameterDouble(void)
 : MCommandParameter(MPT_DOUBLE)
{
	m_Value = 0;
}
MCommandParameterDouble::MCommandParameterDouble(double Value)
 : MCommandParameter(MPT_DOUBLE)
{
	m_Value = Value;
}
MCommandParameter* MCommandParameterDouble::Clone(void)
{
	return (new MCommandParameterDouble(m_Value));
}
void MCommandParameterDouble::GetValue(void* p)
{
	*(double*)p = m_Value;
}
int MCommandParameterDouble::GetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int MCommandParameterDouble::SetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterString //////////////////////////////////////////////////////////////////////
MCommandParameterString::MCommandParameterString(void)
 : MCommandParameter(MPT_STR)
{
	m_Value = 0;
}
MCommandParameterString::MCommandParameterString(const char* Value)
 : MCommandParameter(MPT_STR)
{
	int nLen = (int)strlen(Value)+2;

	if (nLen > (USHRT_MAX-2))
	{
		m_Value = 0;
		return;
	}

	size_t nBuffer = strlen(Value)+2;
	m_Value = new char[nBuffer];
	strcpy_s(m_Value, nBuffer, Value);
}

MCommandParameterString::~MCommandParameterString(void)
{
	if(m_Value!=NULL){
		delete m_Value;
		m_Value=NULL;
	}
}
MCommandParameter* MCommandParameterString::Clone(void)
{
	return (new MCommandParameterString(m_Value));
}
void MCommandParameterString::GetValue(void* p)
{
	strcpy((char*)p, m_Value);
}
int MCommandParameterString::GetData(char* pData, const int nSize)
{
	if(m_Value==NULL) 
	{
		unsigned short nEmptySize = 0;
		memcpy( pData, &nEmptySize, sizeof(nEmptySize) );
		return sizeof(nEmptySize);
	}

	unsigned short nValueSize = (unsigned short)strlen(m_Value)+2;
	if((int)nValueSize+(int)sizeof(nValueSize) > nSize) return 0;

	memcpy(pData, &nValueSize, sizeof(nValueSize));
	memcpy(pData+sizeof(nValueSize), m_Value, nValueSize);

	return nValueSize+sizeof(nValueSize);
}
int MCommandParameterString::SetData(char* pData, const int nSize)
{
	if (NULL == pData)		return 0;

	if(m_Value != NULL) 
	{
		delete[] m_Value;
		m_Value = 0;
	}

	unsigned short nValueSize = 0;
	memcpy(&nValueSize, pData, sizeof(nValueSize));
	if (nValueSize > nSize)	return 0;

	if( (nValueSize > (USHRT_MAX-2)) || (0 == nValueSize) )
	{
//		assert( 0 && "비정상 길이의 문자." );
		return sizeof(nValueSize);
	}

	m_Value = new char[nValueSize];

	memcpy(m_Value, pData + sizeof(nValueSize), nValueSize);
	return nValueSize + sizeof(nValueSize);
}

void MCommandParameterString::GetString(char* szValue,int nBufferSize)
{ 
	if( 0 != szValue )
	{
		if( 0 != m_Value )
			strncpy_s(szValue, nBufferSize - 1, m_Value, min(nBufferSize - 1, sizeof(m_Value) - 1));
		else 
			strcpy_s(szValue, nBufferSize, "\0" );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterVector //////////////////////////////////////////////////////////////////////
MCommandParameterVector::MCommandParameterVector(void)
 : MCommandParameter(MPT_VECTOR)
{
	m_fX = m_fY = m_fZ = 0;
}
MCommandParameterVector::MCommandParameterVector(float x ,float y, float z)
 : MCommandParameter(MPT_VECTOR)
{
	m_fX = x;
	m_fY = y;
	m_fZ = z;
}

MCommandParameterVector::MCommandParameterVector(const vec3& vec)
 : MCommandParameter(MPT_VECTOR)
{
	m_fX = vec.x;
	m_fY = vec.y;
	m_fZ = vec.z;
}
MCommandParameterVector::~MCommandParameterVector(void)
{
}
MCommandParameter* MCommandParameterVector::Clone(void)
{
	return (new MCommandParameterVector(m_fX, m_fY, m_fZ));
}
void MCommandParameterVector::GetValue(void* p)
{
	((float*)p)[0] = m_fX;
	((float*)p)[1] = m_fY;
	((float*)p)[2] = m_fZ;
}
int MCommandParameterVector::GetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_fX) * 3;
	if (nValueSize > nSize) return 0;
	float v[3] = {m_fX, m_fY, m_fZ};
	memcpy(pData, v, nValueSize);
	return nValueSize;
}
int MCommandParameterVector::SetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_fX) * 3;
	if (nValueSize > nSize) return 0;
	float v[3];
	memcpy(v, pData, nValueSize);
	m_fX = v[0];
	m_fY = v[1];
	m_fZ = v[2];
	return nValueSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterBool ////////////////////////////////////////////////////////////////////////
MCommandParameter* MCommandParameterBool::Clone(void)
{
	return (new MCommandParameterBool(m_Value));
}
void MCommandParameterBool::GetValue(void* p)
{
	*(bool*)p = m_Value;
}
int MCommandParameterBool::GetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int MCommandParameterBool::SetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterUID /////////////////////////////////////////////////////////////////////////
MCommandParameterUID::MCommandParameterUID(void)
 : MCommandParameter(MPT_UID)
{
}
MCommandParameterUID::MCommandParameterUID(const MUID& uid)
 : MCommandParameter(MPT_UID)
{
	m_Value = uid;
}
MCommandParameterUID::~MCommandParameterUID(void)
{
}
MCommandParameterUID* MCommandParameterUID::Clone(void)
{
	return (new MCommandParameterUID(m_Value));
}
void MCommandParameterUID::GetValue(void* p)
{
	*(MUID*)p = m_Value;
}
int MCommandParameterUID::GetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int MCommandParameterUID::SetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterID //////////////////////////////////////////////////////////////////////////
MCommandParameterID::MCommandParameterID(void)
: MCommandParameter(MPT_ID)
{
	m_Value = 0;
}
MCommandParameterID::MCommandParameterID(MID Value)
: MCommandParameter(MPT_ID)
{
	m_Value = Value;
}
MCommandParameter* MCommandParameterID::Clone(void)
{
	return (new MCommandParameterID(m_Value));
}
void MCommandParameterID::GetValue(void* p)
{
	*(unsigned int*)p = m_Value;
}
int MCommandParameterID::GetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int MCommandParameterID::SetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterBlob ////////////////////////////////////////////////////////////////////////
MCommandParameterBlob::MCommandParameterBlob(void)
: MCommandParameter(MPT_BLOB), m_pValue(&m_Value)
{
}

MCommandParameterBlob::MCommandParameterBlob(const void* pData, unsigned short nTypeSize, unsigned short nBlobCount)
: MCommandParameter(MPT_BLOB), m_Value(pData, nTypeSize, nBlobCount), m_pValue(&m_Value)
{
}

MCommandParameterBlob::MCommandParameterBlob(MBlob& blob)
: MCommandParameter(MPT_BLOB), m_Value(blob), m_pValue(&m_Value)
{
}

MCommandParameterBlob* MCommandParameterBlob::Clone(void)
{ 
	return new MCommandParameterBlob(m_Value); 
}

void MCommandParameterBlob::GetValue(void* p)
{
	memcpy(p, m_Value.GetBlobPointer(), m_Value.GetBlobTotalSize());
}

int MCommandParameterBlob::GetData(char* pData, const int nMaxSize)
{
	return m_Value.GetData(pData, nMaxSize);
}

int MCommandParameterBlob::SetData(char* pData, const int nSize)
{	
	return m_Value.SetData(pData, nSize);
}

void MCommandParameterBlob::GetString(char* szValue, int nBufferSize)
{ 
	if (m_Value.GetBlobPointer()==NULL) 
	{
		strcpy_s(szValue, nBufferSize, "");
	}
	else
	{
		sprintf_s(szValue, nBufferSize, "%02X%02X..", *((unsigned char*)m_Value.GetBlobPointer()), *((unsigned char*)m_Value.GetBlobPointer() + 1)); 
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterSingleBlob //////////////////////////////////////////////////////////////////
MCommandParameterSingleBlob::MCommandParameterSingleBlob(void)
: MCommandParameter(MPT_SINGLE_BLOB), m_nValueSize(0)
{
}

MCommandParameterSingleBlob::MCommandParameterSingleBlob(const void* pData, unsigned short nDataSize)
: MCommandParameter(MPT_SINGLE_BLOB), m_nValueSize(nDataSize)
{
	if (pData == NULL)
	{
		m_nValueSize = 0;
	}
	else
	{
		if (nDataSize < MAX_SINGLE_BLOB_SIZE)
		{
			memcpy_s(m_Value, MAX_SINGLE_BLOB_SIZE, pData, nDataSize);
		}
	}
}

MCommandParameterSingleBlob* MCommandParameterSingleBlob::Clone(void)
{
	const void* pData = m_Value;
	unsigned short nDataCount = m_nValueSize;

	return new MCommandParameterSingleBlob(pData, nDataCount); 
}

void MCommandParameterSingleBlob::GetValue(void* p)
{ 
	memcpy(p, m_Value, m_nValueSize);
}

int MCommandParameterSingleBlob::GetData(char* pData, const int nMaxSize)
{
	int nValueSize = (int)m_nValueSize;
	if (nValueSize > (int)(nMaxSize + sizeof(unsigned short))) return 0;

	memcpy_s(pData, nMaxSize, &m_nValueSize, sizeof(unsigned short));
	memcpy_s(pData + sizeof(unsigned short), nMaxSize-sizeof(unsigned short), m_Value, nValueSize);

	return (sizeof(unsigned short) + nValueSize);
}

int MCommandParameterSingleBlob::SetData(char* pData, const int nSize)
{
	m_nValueSize = 0;

	unsigned short nValueSize = 0;
	memcpy(&nValueSize, pData, sizeof(unsigned short));

	if (nValueSize > (nSize - sizeof(unsigned short)) ||
		nValueSize > MAX_SINGLE_BLOB_SIZE)
	{
		_ASSERT(!"Data가 너무 크다");
		return -1;
	}
	else if (nValueSize==0)
	{
		return sizeof(unsigned short);
	}

	m_nValueSize = nValueSize;
	memcpy_s(&m_Value[0], MAX_SINGLE_BLOB_SIZE, pData + sizeof(unsigned short), nValueSize);

	return (sizeof(unsigned short) + nValueSize);
}

void MCommandParameterSingleBlob::GetString(char* szValue, int nBufferSize)
{ 
	if (m_nValueSize > 0)
	{
		sprintf_s(szValue, nBufferSize, "%02X%02X..", *((unsigned char*)(&m_Value[0])), *((unsigned char*)(&m_Value[0])) + 1);
	}
	else
	{
		strcpy_s(szValue, nBufferSize, "");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterChar ////////////////////////////////////////////////////////////////////////
MCommandParameterChar::MCommandParameterChar(void)
 : MCommandParameter(MPT_CHAR)
{
	m_Value = 0;
}
MCommandParameterChar::MCommandParameterChar(char Value)
 : MCommandParameter(MPT_CHAR)
{
	m_Value = Value;
}

MCommandParameter* MCommandParameterChar::Clone(void)
{
	return (new MCommandParameterChar(m_Value));
}

void MCommandParameterChar::GetValue(void* p)
{
	*(char*)p = m_Value;
}
int MCommandParameterChar::GetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int MCommandParameterChar::SetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterUChar ///////////////////////////////////////////////////////////////////////
MCommandParameterUChar::MCommandParameterUChar(void)
 : MCommandParameter(MPT_UCHAR)
{
	m_Value = 0;
}
MCommandParameterUChar::MCommandParameterUChar(unsigned char Value)
 : MCommandParameter(MPT_UCHAR)
{
	m_Value = Value;
}

MCommandParameter* MCommandParameterUChar::Clone(void)
{
	return (new MCommandParameterUChar(m_Value));
}

void MCommandParameterUChar::GetValue(void* p)
{
	*(unsigned char*)p = m_Value;
}
int MCommandParameterUChar::GetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int MCommandParameterUChar::SetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterShort ///////////////////////////////////////////////////////////////////////
MCommandParameterShort::MCommandParameterShort(void)
 : MCommandParameter(MPT_SHORT)
{
	m_Value = 0;
}
MCommandParameterShort::MCommandParameterShort(short Value)
 : MCommandParameter(MPT_SHORT)
{
	m_Value = Value;
}

MCommandParameter* MCommandParameterShort::Clone(void)
{
	return (new MCommandParameterShort(m_Value));
}

void MCommandParameterShort::GetValue(void* p)
{
	*(short*)p = m_Value;
}
int MCommandParameterShort::GetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int MCommandParameterShort::SetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterUShort //////////////////////////////////////////////////////////////////////
MCommandParameterUShort::MCommandParameterUShort(void)
 : MCommandParameter(MPT_USHORT)
{
	m_Value = 0;
}
MCommandParameterUShort::MCommandParameterUShort(unsigned short Value)
 : MCommandParameter(MPT_USHORT)
{
	m_Value = Value;
}

MCommandParameter* MCommandParameterUShort::Clone(void)
{
	return (new MCommandParameterUShort(m_Value));
}

void MCommandParameterUShort::GetValue(void* p)
{
	*(short*)p = m_Value;
}
int MCommandParameterUShort::GetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int MCommandParameterUShort::SetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterInt64 ///////////////////////////////////////////////////////////////////////
MCommandParameterInt64::MCommandParameterInt64(void)
 : MCommandParameter(MPT_INT64)
{
	m_Value = 0;
}
MCommandParameterInt64::MCommandParameterInt64(int64 Value)
 : MCommandParameter(MPT_INT64)
{
	m_Value = Value;
}

MCommandParameter* MCommandParameterInt64::Clone(void)
{
	return (new MCommandParameterInt64(m_Value));
}

void MCommandParameterInt64::GetValue(void* p)
{
	*(int64*)p = m_Value;
}
int MCommandParameterInt64::GetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int MCommandParameterInt64::SetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterUInt64 //////////////////////////////////////////////////////////////////////
MCommandParameterUInt64::MCommandParameterUInt64(void)
 : MCommandParameter(MPT_UINT64)
{
	m_Value = 0;
}
MCommandParameterUInt64::MCommandParameterUInt64(uint64 Value)
 : MCommandParameter(MPT_UINT64)
{
	m_Value = Value;
}

MCommandParameter* MCommandParameterUInt64::Clone(void)
{
	return (new MCommandParameterUInt64(m_Value));
}

void MCommandParameterUInt64::GetValue(void* p)
{
	*(uint64*)p = m_Value;
}
int MCommandParameterUInt64::GetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int MCommandParameterUInt64::SetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterShortVector /////////////////////////////////////////////////////////////////
MCommandParameterShortVector::MCommandParameterShortVector(void)
 : MCommandParameter(MPT_SVEC)
{
	m_Value = MSVector(0.0f, 0.0f, 0.0f);
}

MCommandParameterShortVector::MCommandParameterShortVector(float x ,float y, float z)
: MCommandParameter(MPT_SVEC)
{
	m_Value.SetX(x);
	m_Value.SetY(y);
	m_Value.SetZ(z);
}

MCommandParameterShortVector::MCommandParameterShortVector(const vec3& vec)
: MCommandParameter(MPT_SVEC)
{
	m_Value.SetX(vec.x);
	m_Value.SetY(vec.y);
	m_Value.SetZ(vec.z);
}

MCommandParameterShortVector::MCommandParameterShortVector(const MSVector& svec)
: MCommandParameter(MPT_SVEC)
{
	m_Value = svec;
}

MCommandParameterShortVector::~MCommandParameterShortVector(void)
{
}
MCommandParameter* MCommandParameterShortVector::Clone(void)
{
	return (new MCommandParameterShortVector(m_Value));
}
void MCommandParameterShortVector::GetValue(void* p)
{
	*(MSVector*)p = m_Value;
}
int MCommandParameterShortVector::GetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int MCommandParameterShortVector::SetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterShortVector2 ////////////////////////////////////////////////////////////////
MCommandParameterShortVector2::MCommandParameterShortVector2(void)
: MCommandParameter(MPT_SVEC2)
{
	m_Value = MSVector2(0.0f, 0.0f);
}

MCommandParameterShortVector2::MCommandParameterShortVector2(float x ,float y)
: MCommandParameter(MPT_SVEC2)
{
	m_Value.SetX(x);
	m_Value.SetY(y);
}

MCommandParameterShortVector2::MCommandParameterShortVector2(const vec3& vec)
: MCommandParameter(MPT_SVEC2)
{
	m_Value.SetX(vec.x);
	m_Value.SetY(vec.y);
}

MCommandParameterShortVector2::MCommandParameterShortVector2(const MSVector2& svec)
: MCommandParameter(MPT_SVEC2)
{
	m_Value = svec;
}

MCommandParameterShortVector2::~MCommandParameterShortVector2(void)
{
}
MCommandParameter* MCommandParameterShortVector2::Clone(void)
{
	return (new MCommandParameterShortVector2(m_Value));
}
void MCommandParameterShortVector2::GetValue(void* p)
{
	*(MSVector2*)p = m_Value;
}
int MCommandParameterShortVector2::GetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(pData, &m_Value, nValueSize);
	return nValueSize;
}
int MCommandParameterShortVector2::SetData(char* pData, const int nSize)
{
	int nValueSize = sizeof(m_Value);
	if (nValueSize > nSize) return 0;
	memcpy(&m_Value, pData, nValueSize);
	return nValueSize;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
// MCommandParameterWideString //////////////////////////////////////////////////////////////////////
MCommandParameterWideString::MCommandParameterWideString(void)
: MCommandParameter(MPT_WSTR)
{
	m_Value = 0;
}

MCommandParameterWideString::MCommandParameterWideString(const wchar_t* Value)
: MCommandParameter(MPT_WSTR)
{
	int nLen = (int)wcslen(Value) * sizeof(wchar_t) + 2;

	if (nLen > (USHRT_MAX - 2))
	{
		m_Value = 0;
		return;
	}

	size_t nBuffer = wcslen(Value) * sizeof(wchar_t) + 2;
	m_Value = new wchar_t[nBuffer];
	wcscpy_s(m_Value, nBuffer, Value);
}

MCommandParameterWideString::~MCommandParameterWideString(void)
{
	if(m_Value!=NULL){
		delete m_Value;
		m_Value=NULL;
	}
}
MCommandParameter* MCommandParameterWideString::Clone(void)
{
	return (new MCommandParameterWideString(m_Value));
}
void MCommandParameterWideString::GetValue(void* p)
{
	wcscpy((wchar_t*)p, m_Value);	
}
int MCommandParameterWideString::GetData(char* pData, const int nSize)
{
	if(m_Value==NULL)
	{
		unsigned short nEmptySize = 0;
		memcpy( pData, &nEmptySize, sizeof(nEmptySize) );
		return sizeof(nEmptySize);
	}

	unsigned short nValueSize = (unsigned short)wcslen(m_Value) * sizeof(wchar_t) + 2;
	if((int)nValueSize+(int)sizeof(nValueSize) > nSize) return 0;

	memcpy(pData, &nValueSize, sizeof(nValueSize));
	memcpy(pData+sizeof(nValueSize), m_Value, nValueSize);

	return nValueSize+sizeof(nValueSize);
}
int MCommandParameterWideString::SetData(char* pData, const int nSize)
{
	if (NULL == pData)		return 0;

	if(m_Value != NULL) 
	{
		delete[] m_Value;
		m_Value = 0;
	}

	unsigned short nValueSize = 0;
	memcpy(&nValueSize, pData, sizeof(nValueSize));
	if (nValueSize > nSize)	return 0;

	if( (nValueSize > (USHRT_MAX-2)) || (0 == nValueSize) )
	{
		//		assert( 0 && "비정상 길이의 문자." );
		return sizeof(nValueSize);
	}

	m_Value = new wchar_t[nValueSize];

	memcpy(m_Value, pData + sizeof(nValueSize), nValueSize);
	return nValueSize + sizeof(nValueSize);
}

void MCommandParameterWideString::GetString(char* szValue,int nBufferSize)
{ 
	if( 0 != szValue )
	{
		if( 0 != m_Value )
		{			
			strncpy_s(szValue, nBufferSize - 1, MLocale::ConvUTF16ToAnsi(m_Value).c_str(), _TRUNCATE);
		}
		else 
		{
			strcpy_s(szValue, nBufferSize, "\0" );
		}
	}
}

} // namespace minet
