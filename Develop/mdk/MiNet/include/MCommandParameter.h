#pragma once

#include "MiNetLib.h"
#include "MUID.h"
#include "MTypes.h"
#include "MBlob.h"
#include "MMemPool.h"

namespace minet {

class MCommandParamCondition;
class MCommandParamConditionMinMax;

/// 커맨드 파라미터 타입
enum MCommandParameterType{
	MPT_INT			= 0,
	MPT_UINT		= 1,
	MPT_FLOAT		= 2,
	MPT_DOUBLE		= 3,
	MPT_CHAR		= 4,
	MPT_UCHAR		= 5,
	MPT_SHORT		= 6,
	MPT_USHORT		= 7,
	MPT_INT64		= 8,
	MPT_UINT64		= 9,
	MPT_BOOL		= 10,
	MPT_STR			= 11,
	MPT_VECTOR		= 12,
	MPT_SVEC		= 13,
	MPT_SVEC2		= 14,
	MPT_UID			= 15,
	MPT_ID			= 16,
	MPT_BLOB		= 17,
	MPT_SINGLE_BLOB	= 18,
	MPT_WSTR		= 19,

	// 숫자는 고정된 값이다. (확장은 가능하되, 수정은 불가)
	MPT_END,		///< 파라미터 타입 총 갯수
};

#define MAX_BLOB_SIZE		(0x100000)			// 1메가바이트
#define MAX_SINGLE_BLOB_SIZE	1024			// 1kb

/// Command Parameter Description
class MINET_API MCommandParameterDesc {
protected:
	MCommandParameterType			m_nType;
	char							m_szDescription[64];
	vector<MCommandParamCondition*>	m_Conditions;				///< 파라메타의 제약조건들
	void							InitializeConditions();
public:
									MCommandParameterDesc(MCommandParameterType nType, char* szDescription);
	virtual							~MCommandParameterDesc(void);
	MCommandParameterType			GetType(void)		{ return m_nType; }
	const char*						GetDescription(void){ return m_szDescription; }
	void							AddCondition(MCommandParamCondition* pCondition);
	bool							HasConditions()		{ return (!m_Conditions.empty()); }
	int								GetConditionCount() { return (int)m_Conditions.size(); }
	MCommandParamCondition*			GetCondition(int n) { return m_Conditions[n]; }

};


/// Command Parameter Abstract Class
class MINET_API MCommandParameter {
protected:
	MCommandParameterType	m_nType;
public:
							MCommandParameter(MCommandParameterType nType){ m_nType = nType; }
	virtual					~MCommandParameter(void){}

	MCommandParameterType		GetType(void)	{ return m_nType; }
	/// 같은 파라미티 타입으로 복제
	virtual MCommandParameter*	Clone(void) = 0;
	/// 값 얻어내기
	virtual	void				GetValue(void* p) = 0;
	/// 메모리 블럭으로 저장
	virtual int					GetData(char* pData, const int nSize) = 0;
	/// 메모리 블럭으로 지정
	virtual int					SetData(char* pData, const int nSize) = 0;
	/// 첫 포인터 얻기
	virtual void*				GetPointer(void) = 0;
	/// 타입 이름 얻기
	virtual const char*			GetClassName(void) = 0;
	/// 스트링으로 값 얻기
	virtual void				GetString(char* szValue, int nBufferSize) = 0;
	/// 사이즈 얻기
	virtual int					GetSize() = 0;

	virtual int					GetBlobCount()				{ _ASSERT(!"MCommandParameterBlob 전용."); return -1; }
	virtual int					GetBlobSize()				{ _ASSERT(!"MCommandParameterBlob 전용."); return -1; }
	virtual void*				GetBlobElement(int i)		{ _ASSERT(!"MCommandParameterBlob 전용."); return NULL; }
	virtual int					GetSingleBlobValueSize()	{ _ASSERT(!"MCommandParameterSingleBlob 전용."); return -1; }

	// 새 커맨드 핸들러 테스트를 위한 데이터 직접 얻기 함수
	virtual void*				GetPointerRaw(void) = 0;
};

/// 정수 파라미터
class MINET_API MCommandParameterInt : public MCommandParameter
{
public:
	int m_Value;
public:
								MCommandParameterInt(void);
								MCommandParameterInt(int Value);
	virtual	MCommandParameter*	Clone(void)			override;
	virtual void				GetValue(void* p)	override;
	virtual int					GetData(char* pData, const int nSize) override;
	virtual int					SetData(char* pData, const int nSize) override;
	virtual void*				GetPointer()		override { return &m_Value; }
	virtual const char*			GetClassName(void)	override { return "Int"; }
	virtual int					GetSize()			override { return sizeof(int); }
	virtual void*				GetPointerRaw()		override { return &m_Value; }
	virtual void				GetString(char* szValue, int nBufferSize) override { sprintf_s(szValue, nBufferSize, "%d", m_Value); }
};

/// 양수
class MINET_API MCommandParameterUInt : public MCommandParameter
{
public:
	unsigned int m_Value;
public:
								MCommandParameterUInt(void);
								MCommandParameterUInt(unsigned int Value);
	virtual MCommandParameter*	Clone(void)			override;
	virtual void				GetValue(void* p)	override;
	virtual int					GetData(char* pData, const int nSize) override;
	virtual int					SetData(char* pData, const int nSize) override;
	virtual void*				GetPointer()		override { return &m_Value; }
	virtual const char*			GetClassName(void)	override { return "UInt"; }
	virtual int					GetSize()			override { return sizeof(unsigned int); }
	virtual void*				GetPointerRaw()		override { return &m_Value; }
	virtual void				GetString(char* szValue, int nBufferSize) override { sprintf_s(szValue, nBufferSize,  "%u", m_Value); }
};

/// 소수 파라미터
class MINET_API MCommandParameterFloat : public MCommandParameter
{
public:
	float m_Value;
public:
								MCommandParameterFloat(void);
								MCommandParameterFloat(float Value);
	virtual MCommandParameter*	Clone(void)			override;
	virtual void				GetValue(void* p)	override;
	virtual int					GetData(char* pData, const int nSize) override;
	virtual int					SetData(char* pData, const int nSize) override;
	virtual void*				GetPointer()		override { return &m_Value; }
	virtual const char*			GetClassName(void)	override { return "Float"; }
	virtual int					GetSize()			override { return sizeof(float); }
	virtual void*				GetPointerRaw()		override { return &m_Value; }
	virtual void				GetString(char* szValue, int nBufferSize) override { sprintf_s(szValue, nBufferSize, "%f", m_Value); }
};

/// double형 파라메터
class MINET_API MCommandParameterDouble : public MCommandParameter
{
public:
	double m_Value;
public:
								MCommandParameterDouble(void);
								MCommandParameterDouble(double Value);
	virtual MCommandParameter*	Clone(void)			override;
	virtual void				GetValue(void* p)	override;
	virtual int					GetData(char* pData, const int nSize) override;
	virtual int					SetData(char* pData, const int nSize) override;
	virtual void*				GetPointer()		override { return &m_Value; }
	virtual const char*			GetClassName(void)	override { return "Double"; }
	virtual int					GetSize()			override { return sizeof(double); }
	virtual void*				GetPointerRaw()		override { return &m_Value; }
	virtual void				GetString(char* szValue, int nBufferSize) override { sprintf_s(szValue, nBufferSize, "%f", m_Value); }
};

/// 스트링 파라미터(65533이하의 문자)
class MINET_API MCommandParameterString : public MCommandParameter
{
public:
	char* m_Value;
public:
								MCommandParameterString(void);
								MCommandParameterString(const char* Value);								
	virtual						~MCommandParameterString(void);
	virtual MCommandParameter*	Clone(void)			override;
	virtual void				GetValue(void* p)	override;
	virtual int					GetData(char* pData, const int nSize) override;
	virtual int					SetData(char* pData, const int nSize) override;
	virtual void*				GetPointer()		override { return &m_Value; }
	virtual const char*			GetClassName(void)	override { return "String"; }
	virtual int					GetSize()			override { if(m_Value==NULL) return 0;
															   return ((int)strlen(m_Value)+2 + sizeof(unsigned short)); }
	virtual void*				GetPointerRaw()		override { return &m_Value; }
	virtual void				GetString(char* szValue, int nBufferSize) override;
};

/// 3D 벡터 파라미터
class MINET_API MCommandParameterVector : public MCommandParameter
{
public:
	float	m_fX;
	float	m_fY;
	float	m_fZ;
public:
								MCommandParameterVector(void);
								MCommandParameterVector(const vec3& vec);
								MCommandParameterVector(float x ,float y, float z);
	virtual						~MCommandParameterVector(void);
	virtual MCommandParameter*	Clone(void)			override;
	virtual void				GetValue(void* p)	override;
	virtual int					GetData(char* pData, const int nSize) override;
	virtual int					SetData(char* pData, const int nSize) override;
	virtual void*				GetPointer()		override { return &m_fX; }
	virtual const char*			GetClassName(void)	override { return "Vector"; }
	virtual int					GetSize()			override { return (sizeof(float)*3); }
	virtual void*				GetPointerRaw()		override { return &m_fX; }
	virtual void				GetString(char* szValue, int nBufferSize) override { sprintf_s(szValue, nBufferSize, "%.2f,%.2f,%.2f", m_fX, m_fY, m_fZ); }
};


/// Bool 파라미터
class MINET_API MCommandParameterBool : public MCommandParameter
{
	bool	m_Value;
public:
								MCommandParameterBool(void) : MCommandParameter(MPT_BOOL) { }
								MCommandParameterBool(bool bValue) : MCommandParameter(MPT_BOOL) { m_Value = bValue; }
	virtual MCommandParameter*	Clone(void)			override;
	virtual void				GetValue(void* p)	override;
	virtual int					GetData(char* pData, const int nSize) override;
	virtual int					SetData(char* pData, const int nSize) override;
	virtual void*				GetPointer(void)	override { return &m_Value; }
	virtual const char*			GetClassName(void)	override { return "Bool"; }
	virtual int					GetSize()			override { return sizeof(bool); }
	virtual void*				GetPointerRaw()		override { return &m_Value; }
	virtual void				GetString(char* szValue, int nBufferSize) override { strcpy_s(szValue, nBufferSize, m_Value ? "true" : "false"); }
};

/// MUID 파라미터
class MINET_API MCommandParameterUID : public MCommandParameter
{
public:
	MUID	m_Value;
public:
									MCommandParameterUID(void);
									MCommandParameterUID(const MUID& uid);
	virtual							~MCommandParameterUID(void);
	virtual MCommandParameterUID*	Clone(void)			override;
	virtual void					GetValue(void* p)	override;
	virtual int						GetData(char* pData, const int nSize) override;
	virtual int						SetData(char* pData, const int nSize) override;
	virtual void*					GetPointer()		override { return &m_Value; }
	virtual const char*				GetClassName(void)	override { return "UID"; }
	virtual int						GetSize()			override { return sizeof(MUID); }
	virtual void*					GetPointerRaw()		override { return &m_Value; }
	virtual void					GetString(char* szValue, int nBufferSize) override { sprintf_s(szValue, nBufferSize, "%I64u", m_Value.Value); }
};

/// MID 파라미터
class MINET_API MCommandParameterID : public MCommandParameter
{
public:
	MID		m_Value;
public:
								MCommandParameterID(void);
								MCommandParameterID(MID Value);
	virtual MCommandParameter*	Clone(void)			override;
	virtual void				GetValue(void* p)	override;
	virtual int					GetData(char* pData, const int nSize) override;
	virtual int					SetData(char* pData, const int nSize) override;
	virtual void*				GetPointer()			override { return &m_Value; }
	virtual const char*			GetClassName(void)		override { return "ID"; }
	virtual int					GetSize()				override { return sizeof(MID); }
	virtual void*				GetPointerRaw()			override { return &m_Value; }
	virtual void				GetString(char* szValue, int nBufferSize) override { sprintf_s(szValue, nBufferSize, "%u", m_Value); }
};

template<typename T>
const T* _VectorToArray(const vector<T>& vec)
{
	if (vec.empty()) 
		return NULL;

	return &(const T&)vec.front();
}

class MINET_API MCommandParameterBlob : public MCommandParameter
{
	MBlob m_Value;
	MBlob* m_pValue;

public:
	MCommandParameterBlob(void);
	template<typename T>
	MCommandParameterBlob(const vector<T>& vector)
	: MCommandParameter(MPT_BLOB), m_Value(_VectorToArray(vector), sizeof(T), static_cast<unsigned short>(vector.size())), m_pValue(&m_Value)
	{
	}
	MCommandParameterBlob(const void* pData, unsigned short nTypeSize, unsigned short nBlobCount);
	MCommandParameterBlob(MBlob& blob);

	virtual	MCommandParameterBlob*	Clone(void)			override;
	virtual void					GetValue(void* p)	override;
	virtual int						GetData(char* pData, const int nSize) override;
	virtual int						SetData(char* pData, const int nSize) override;
	virtual void*					GetPointer()		override { return m_Value.GetBlobPointer(); }
	virtual int						GetSize()			override { return m_Value.GetBlobTotalSize(); }
	virtual const char*				GetClassName(void)	override { return "Blob"; }
	virtual void*					GetPointerRaw()		override { return &m_pValue; }
	virtual void					GetString(char* szValue, int nBufferSize)	override;

	virtual int						GetBlobCount()		override { return m_Value.GetBlobCount(); }
	virtual int						GetBlobSize()		override { return m_Value.GetBlobSize(); }
	virtual void*					GetBlobElement( int i )	override { return m_Value.GetBlobElement( i ); }
			bool					SetBlobElement( int i, const void* pData, const int nTypeSize ) { return m_Value.SetBlobElement( i, pData, nTypeSize ); }
};

class MINET_API MCommandParameterSingleBlob : public MCommandParameter
{
	unsigned char	m_Value[MAX_SINGLE_BLOB_SIZE];
	unsigned short	m_nValueSize;
public:
	MCommandParameterSingleBlob(void);
	MCommandParameterSingleBlob(const void* pData, unsigned short nDataSize);

	template<typename T>
	MCommandParameterSingleBlob(const T& val)
	: MCommandParameter(MPT_SINGLE_BLOB)
	{
		unsigned short nValueSize = sizeof(val);		

		if (nValueSize < MAX_SINGLE_BLOB_SIZE)
		{
			m_nValueSize = nValueSize; 
			memcpy_s(m_Value, MAX_SINGLE_BLOB_SIZE, &val, nValueSize);
		}
	}

	virtual	MCommandParameterSingleBlob*	Clone(void)			override;
	virtual void							GetValue(void* p)	override;
	virtual int								GetData(char* pData, const int nSize) override;
	virtual int								SetData(char* pData, const int nSize) override;
	virtual void*							GetPointer()		override { return ((unsigned char*)&m_Value[0]); }
	virtual int								GetSize()			override { return ((int)m_nValueSize + sizeof(unsigned short)); }
	virtual const char*						GetClassName(void)	override { return "SingleBlob"; }
	virtual void*							GetPointerRaw()		override { return &(m_Value[0]); }
	virtual void							GetString(char* szValue, int nBufferSize) override;
	virtual int								GetSingleBlobValueSize() override { return m_nValueSize; }
};

/// char형 파라미터
class MINET_API MCommandParameterChar : public MCommandParameter
{
public:
	char	m_Value;
public:
								MCommandParameterChar(void);
								MCommandParameterChar(char Value);
	virtual MCommandParameter*	Clone(void)			override;
	virtual void				GetValue(void* p)	override;
	virtual int					GetData(char* pData, const int nSize) override;
	virtual int					SetData(char* pData, const int nSize) override;
	virtual void*				GetPointer()		override { return &m_Value; }
	virtual const char*			GetClassName(void)	override { return "Char"; }
	virtual int					GetSize()			override { return sizeof(char); }
	virtual void*				GetPointerRaw()		override { return &m_Value; }
	virtual void				GetString(char* szValue, int nBufferSize) override { sprintf_s(szValue, nBufferSize, "%d", m_Value); }
};


/// unsigned char형 파라미터
class MINET_API MCommandParameterUChar : public MCommandParameter
{
public:
	unsigned char	m_Value;
public:
								MCommandParameterUChar(void);
								MCommandParameterUChar(unsigned char Value);
	virtual MCommandParameter*	Clone(void)			override;
	virtual void				GetValue(void* p)	override;
	virtual int					GetData(char* pData, const int nSize) override;
	virtual int					SetData(char* pData, const int nSize) override;
	virtual void*				GetPointer()		override { return &m_Value; }
	virtual const char*			GetClassName(void)	override { return "UChar"; }
	virtual int					GetSize()			override { return sizeof(unsigned char); }
	virtual void*				GetPointerRaw()		override { return &m_Value; }
	virtual void				GetString(char* szValue, int nBufferSize) override { sprintf_s(szValue, nBufferSize, "%u", m_Value); }
};


/// short형 파라미터
class MINET_API MCommandParameterShort : public MCommandParameter
{
public:
	short	m_Value;
public:
								MCommandParameterShort(void);
								MCommandParameterShort(short Value);
	virtual MCommandParameter*	Clone(void)			override;
	virtual void				GetValue(void* p)	override;
	virtual int					GetData(char* pData, const int nSize) override;
	virtual int					SetData(char* pData, const int nSize) override;
	virtual void*				GetPointer()		override { return &m_Value; }
	virtual const char*			GetClassName(void)	override { return "Short"; }
	virtual int					GetSize()			override { return sizeof(short); }
	virtual void*				GetPointerRaw()		override { return &m_Value; }
	virtual void				GetString(char* szValue, int nBufferSize) override { sprintf_s(szValue, nBufferSize, "%d", m_Value); }
};

/// unsigned short형 파라미터
class MINET_API MCommandParameterUShort : public MCommandParameter
{
public:
	unsigned short	m_Value;
public:
								MCommandParameterUShort(void);
								MCommandParameterUShort(unsigned short Value);
	virtual MCommandParameter*	Clone(void)			override;
	virtual void				GetValue(void* p)	override;
	virtual int					GetData(char* pData, const int nSize) override;
	virtual int					SetData(char* pData, const int nSize) override;
	virtual void*				GetPointer()		override { return &m_Value; }
	virtual const char*			GetClassName(void)	override { return "UShort"; }
	virtual int					GetSize()			override { return sizeof(unsigned short); }
	virtual void*				GetPointerRaw()		override { return &m_Value; }
	virtual void				GetString(char* szValue, int nBufferSize) override { sprintf_s(szValue, nBufferSize, "%u", m_Value); }
};


/// int64형 파라미터
class MINET_API MCommandParameterInt64 : public MCommandParameter
{
public:
	int64	m_Value;
public:
								MCommandParameterInt64(void);
								MCommandParameterInt64(int64 Value);
	virtual MCommandParameter*	Clone(void)			override;
	virtual void				GetValue(void* p)	override;
	virtual int					GetData(char* pData, const int nSize) override;
	virtual int					SetData(char* pData, const int nSize) override;
	virtual void*				GetPointer()		override { return &m_Value; }
	virtual const char*			GetClassName(void)	override { return "Int64"; }
	virtual int					GetSize()			override { return sizeof(int64); }
	virtual void*				GetPointerRaw()		override { return &m_Value; }
	virtual void				GetString(char* szValue, int nBufferSize) override { sprintf_s(szValue, nBufferSize, "%lld", m_Value); }
};

/// unsigned int64형 파라미터
class MINET_API MCommandParameterUInt64 : public MCommandParameter
{
public:
	uint64	m_Value;
public:
								MCommandParameterUInt64(void);
								MCommandParameterUInt64(uint64 Value);
	virtual MCommandParameter*	Clone(void)			override;
	virtual void				GetValue(void* p)	override;
	virtual int					GetData(char* pData, const int nSize) override;
	virtual int					SetData(char* pData, const int nSize) override;
	virtual void*				GetPointer()		override { return &m_Value; }
	virtual const char*			GetClassName(void)	override { return "UInt64"; }
	virtual int					GetSize()			override { return sizeof(uint64); }
	virtual void*				GetPointerRaw()		override { return &m_Value; }
	virtual void				GetString(char* szValue, int nBufferSize) override { sprintf_s(szValue, nBufferSize, "%llu", m_Value); }
};


/// short형 3D 벡터 파라미터
class MINET_API MCommandParameterShortVector : public MCommandParameter
{
public:
	MSVector	m_Value;
public:
								MCommandParameterShortVector(void);
								MCommandParameterShortVector(const MSVector& svec);
								MCommandParameterShortVector(float x ,float y, float z);	///< 내부에서 short로 변환해준다.
								MCommandParameterShortVector(const vec3& vec);
	virtual						~MCommandParameterShortVector(void);
	virtual MCommandParameter*	Clone(void)			override;
	virtual void				GetValue(void* p)	override;
	virtual int					GetData(char* pData, const int nSize) override;
	virtual int					SetData(char* pData, const int nSize) override;
	virtual void*				GetPointer()		override { return &m_Value; }
	virtual const char*			GetClassName(void)	override { return "ShortVector"; }
	virtual int					GetSize()			override { return (sizeof(MSVector)); }
	virtual void*				GetPointerRaw()		override { return &m_Value; }
	virtual void				GetString(char* szValue, int nBufferSize)	{ sprintf_s(szValue, nBufferSize, "%.1f,%.1f,%.1f", m_Value.X(), m_Value.Y(), m_Value.Z()); }
};

/// short형 2D 벡터 파라미터
class MINET_API MCommandParameterShortVector2 : public MCommandParameter
{
public:
	MSVector2	m_Value;
public:
	MCommandParameterShortVector2(void);
	MCommandParameterShortVector2(const MSVector2& svec);
	MCommandParameterShortVector2(float x ,float y);	///< 내부에서 short로 변환해준다.
	MCommandParameterShortVector2(const vec3& vec);		///< vec의 z값은 무시된다.
	virtual						~MCommandParameterShortVector2(void);
	virtual MCommandParameter*	Clone(void)			override;
	virtual void				GetValue(void* p)	override;
	virtual int					GetData(char* pData, const int nSize) override;
	virtual int					SetData(char* pData, const int nSize) override;
	virtual void*				GetPointer()		override { return &m_Value; }
	virtual const char*			GetClassName(void)	override { return "ShortVector2"; }
	virtual int					GetSize()			override { return (sizeof(MSVector2)); }
	virtual void*				GetPointerRaw()		override { return &m_Value; }
	virtual void				GetString(char* szValue, int nBufferSize)	{ sprintf_s(szValue, nBufferSize, "%.1f,%.1f", m_Value.X(), m_Value.Y()); }
};


/// 스트링 파라미터(65533 / 2 이하의 문자)
class MINET_API MCommandParameterWideString : public MCommandParameter
{
public:
	wchar_t* m_Value;
public:
	MCommandParameterWideString(void);
	MCommandParameterWideString(const wchar_t* Value);
	virtual						~MCommandParameterWideString(void);
	virtual MCommandParameter*	Clone(void)			override;
	virtual void				GetValue(void* p)	override;
	virtual int					GetData(char* pData, const int nSize) override;
	virtual int					SetData(char* pData, const int nSize) override;
	virtual void*				GetPointer()		override { return &m_Value; }
	virtual const char*			GetClassName(void)	override { return "WideString"; }
	virtual int					GetSize()			override { if(m_Value==NULL) return 0; return ((int)wcslen(m_Value) * sizeof(wchar_t) + 2 + sizeof(unsigned short)); }
	virtual void*				GetPointerRaw()		override { return &m_Value; }
	virtual void				GetString(char* szValue, int nBufferSize) override;
};


/// Command Parameter Condition Abstract Class
class MCommandParamCondition
{
public:
					MCommandParamCondition(void) {}
	virtual			~MCommandParamCondition(void) {}
	virtual bool	Check(MCommandParameter* pCP) = 0;
};

/// 파라메타의 최대 최소를 설정할 수 있는 제약조건
class MCommandParamConditionMinMax : public MCommandParamCondition
{
private:
	int m_nMin;
	int m_nMax;
public:
					MCommandParamConditionMinMax(int nMin, int nMax) : m_nMin(nMin), m_nMax(nMax) {}
	virtual			~MCommandParamConditionMinMax(void) {}
	virtual bool	Check(MCommandParameter* pCP);
};

/// 타입에 맞는 파라미터 생성
MINET_API MCommandParameter* NewParameter(MCommandParameterType nParamType);


// Short Name
typedef MCommandParameterBlob				MCmdParamBlob;
typedef MCommandParameterSingleBlob			MCmdParamSingleBlob;
typedef MCommandParameterUID				MCmdParamUID;
typedef MCommandParameter					MCmdParam;
typedef MCommandParameterDesc				MCmdParamDesc;
typedef MCommandParameterInt				MCmdParamInt;
typedef MCommandParameterUInt				MCmdParamUInt;
typedef MCommandParameterFloat				MCmdParamFloat;
typedef MCommandParameterDouble				MCmdParamDouble;
typedef MCommandParameterString				MCmdParamStr;
typedef MCommandParameterVector				MCmdParamVector;
typedef MCommandParameterShortVector		MCmdParamShortVector;
typedef MCommandParameterShortVector2		MCmdParamShortVector2;
typedef MCommandParameterBool				MCmdParamBool;
typedef MCommandParameterChar				MCmdParamChar;
typedef MCommandParameterUChar				MCmdParamUChar;
typedef MCommandParameterShort				MCmdParamShort;
typedef MCommandParameterUShort				MCmdParamUShort;
typedef MCommandParameterInt64				MCmdParamInt64;
typedef MCommandParameterUInt64				MCmdParamUInt64;
typedef MCommandParameterWideString			MCmdParamWideString;

#define NEW_BLOB							new MCommandParameterBlob
#define NEW_SINGLE_BLOB						new MCommandParameterSingleBlob
#define NEW_UID								new MCommandParameterUID
#define NEW_INT								new MCommandParameterInt
#define NEW_UINT							new MCommandParameterUInt
#define NEW_FLOAT							new MCommandParameterFloat		
#define NEW_DOUBLE							new	MCommandParameterDouble		
#define NEW_STR								new	MCommandParameterString		
#define NEW_VEC								new	MCommandParameterVector		
#define NEW_SVEC							new	MCommandParameterShortVector
#define NEW_SVEC2							new	MCommandParameterShortVector2
#define NEW_BOOL							new	MCommandParameterBool		
#define NEW_CHAR							new	MCommandParameterChar		
#define NEW_UCHAR							new	MCommandParameterUChar		
#define NEW_SHORT							new	MCommandParameterShort		
#define NEW_USHORT							new	MCommandParameterUShort		
#define NEW_INT64							new	MCommandParameterInt64		
#define NEW_UINT64							new	MCommandParameterUInt64		
#define NEW_WSTR							new MCommandParameterWideString


// 나중에 define을 없애고 MPT_VECTOR가 MPT_VEC로 변경할 예정입니다. - bird(2006.05.10)
#define MPT_VEC		MPT_VECTOR

} // namespace minet


