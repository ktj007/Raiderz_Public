#ifndef MUID_H
#define MUID_H

#include <map>
#include <vector>
#include <crtdbg.h>
#include <iostream>
#include "RLib.h"
#include "MTypes.h"

using namespace std;

/// MAIET Unique ID
struct CML2_API MUID{
	union {
		struct 
		{
			unsigned long int	Low;	///< Low 4 Byte
			unsigned long int	High;	///< High 4 Byte
		};
		uint64 Value;
	};

	MUID(void) { Value = 0; }
	MUID(uint64 i) : Value(i) { }
	MUID(unsigned long int h, unsigned long int l)
	{
		High = h;
		Low = l;
	}

	/// MUID 0으로 초기화
	void SetZero(void){
		Value = 0;
	}
	void SetInvalid(void)
	{
		SetZero();
	}

	/// MUID를 nSize만큼 증가
	MUID Increase(uint64 nSize=1){
		Value += nSize;
		return *this;
	}

	/// MUID가 유효한지 검사, 0:0이면 Invalid
	bool IsInvalid(void) const {
		// 0이면 Invalid
		if(Value == 0) return true;
		return false;
	}
	/// MUID가 유효한지 검사, 0:0이면 Invalid
	bool IsValid(void) const {
		// 0이면 Invalid
		if(Value == 0) return false;
		return true;
	}

	/// 크기 비교를 위한 오퍼레이터 오버로딩
	inline friend bool operator > (const MUID& a, const MUID& b){
		if(a.Value > b.Value) return true;
		return false;
	}
	/// 크기 비교를 위한 오퍼레이터 오버로딩
	inline friend bool operator >= (const MUID& a, const MUID& b){
		if(a.Value >= b.Value) return true;
		return false;
	}
	/// 크기 비교를 위한 오퍼레이터 오버로딩
	inline friend bool operator < (const MUID& a, const MUID& b){
		if(a.Value < b.Value) return true;
		return false;
	}
	/// 크기 비교를 위한 오퍼레이터 오버로딩
	inline friend bool operator <= (const MUID& a, const MUID& b){
		if(a.Value <= b.Value) return true;
		return false;
	}

	/// Stream에 대입 오퍼레이터 오버로딩
	inline friend ostream& operator<<(ostream& stream, const MUID& rhs){
		stream << rhs.Value;
		return stream;
	}
	
	inline friend wostream& operator<<(wostream& stream, const MUID& rhs){
		stream << rhs.Value;
		return stream;
	}

	/// 대입 오퍼레이터 오버로딩
	inline MUID& operator=(uint64 v){
		Value = v;
		return *this;
	}
	/// 대입 오퍼레이터 오버로딩
	inline MUID& operator=(const MUID& a){
		Value = a.Value;
		return *this;
	}

	/// 비교 오퍼레이터 오버로딩
	inline friend bool operator==(const MUID& a, const MUID& b){
		if(a.Value==b.Value) return true;
		return false;
	}
	/// 비교 오퍼레이터 오버로딩
	inline friend bool operator!=(const MUID& a, const MUID& b){
		if(a.Value!=b.Value) return true;
		return false;
	}

	MUID operator+() const { return MUID(Value);	}
	MUID operator+(const MUID& a) const 
	{ 
		return MUID(Value + a.Value);	
	}
	MUID& operator+=(const MUID& a)	
	{ 
		Value += a.Value;
		return *this; 
	}
	MUID& operator++()		{ Value++; return (*this); }
	const MUID operator++(int)
	{ 
		const MUID oldValue = *this; 
		++(*this);
		return oldValue; 
	}

	MUID operator-() const { return MUID(Value);  }
	MUID operator-(const MUID& a) const 
	{ 
		return MUID(Value - a.Value);	
	}
	MUID& operator-=(const MUID& a)	
	{ 
		Value -= a.Value;
		return *this; 
	}
	MUID& operator--()		{ Value--; return (*this); }
	const MUID operator--(int)
	{ 
		const MUID oldValue = *this; 
		--(*this);
		return oldValue; 
	}

	/// Invalid한 UID 얻기
		static MUID Invalid(void);

	static const MUID ZERO;
};

/// MUID 영역
struct MUIDRANGE{
	MUID	Start;
	MUID	End;

	bool IsEmpty(void){
		return (Start==End);
	}
	void Empty(void){
		SetZero();
	}
	void SetZero(void){
		Start.SetZero();
		End.SetZero();
	}
};

/// MUID Reference Map
/// - 순차적으로 1씩 증가하는 ID를 얻어낼 수 있는 MUID 집합
class MUIDRefMap : protected map<MUID, void*>{
	MUID	m_CurrentMUID;		///< 현재 발급된 MUID
public:
	MUIDRefMap(void);
	virtual ~MUIDRefMap(void);

	/// 레퍼런스를 MUID하나에 할당한다.
	/// @param pRef	레퍼런스 포인터
	/// @return		할당된 MUID
	MUID Generate(void* pRef);

	/// MUID를 통해 레퍼런스 포인터를 얻어낸다.
	/// @param uid	MUID
	/// @return		레퍼런스 포인터, MUID가 존재하지 않으면 NULL을 리턴
	void* GetRef(MUID& uid);

	/// 등록된 MUID 삭제.
	/// @param uid	MUID
	/// @return		등록되었던 레퍼런스 포인터, MUID가 존재하지 않으면 NULL을 리턴
	void* Remove(MUID& uid);

	/// nSize만큼 확보
	MUIDRANGE Reserve(int nSize);

	/// 지금까지 발급된 UID 구간
	MUIDRANGE GetReservedCount(void);
};

/// MUID Reference Map Cache
/// - 전체 UID의 검색 부하를 줄이기 위한 캐쉬 클래스
class MUIDRefCache : public map<MUID, void*>{
public:
	MUIDRefCache(void);
	virtual ~MUIDRefCache(void);

	/// 레퍼런스를 MUID하나에 할당한다.
	/// @param pRef	레퍼런스 포인터
	/// @return		할당된 MUID
	void Insert(const MUID& uid, void* pRef);
	/// MUID를 통해 레퍼런스 포인터를 얻어낸다.
	/// @param uid	MUID
	/// @return		레퍼런스 포인터, MUID가 존재하지 않으면 NULL을 리턴
	void* GetRef(const MUID& uid);
	/// 등록된 MUID 삭제.
	/// @param uid	MUID
	/// @return		등록되었던 레퍼런스 포인터, MUID가 존재하지 않으면 NULL을 리턴
	void* Remove(const MUID& uid);
};

/// MUID 생성기
class CML2_API MUIDGenerator
{
private:
	MUID	m_CurrentMUID;
public:
	MUIDGenerator(MUID uidSeed=MUID::ZERO) : m_CurrentMUID(uidSeed) {}
	~MUIDGenerator() {}
	void SetSeed(MUID uidSeed) { m_CurrentMUID = uidSeed; }
	MUID Generate()
	{
		m_CurrentMUID.Increase();
		return m_CurrentMUID;
	}
};

/// Random UID Generator
class CML2_API MUIDRandomGenerator
{
private:
	uint32 m_nSeed;
public:
	MUIDRandomGenerator(uint32 nSeed) : m_nSeed(nSeed) {}
	MUID Generate();
};

/// MUID보다는 작은(4바이트) ID
typedef unsigned long int		MID;


#endif