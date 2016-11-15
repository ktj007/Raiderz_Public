#ifndef _MBLOB_H
#define _MBLOB_H

#pragma warning( disable : 4251 )

#include <vector>
#include "RLib.h"
using namespace std;

class CML2_API MBlob
{
private:
	unsigned short			m_nTypeSize;	///< Type size는 0xffff를 넘을 수 없다.
	unsigned short			m_nBlobCount;	///< Blob Count는 0xffff를 넘을 수 없다.
	vector<unsigned char>	m_vecData;		///< m_vecBlob에는 순수 데이터만 들어간다.
public:
	MBlob(void);
	/// nTypeSize크기로 nBlobCount갯수만큼 배열한 블럭 만들기
	MBlob(const void* pData, unsigned short nTypeSize, unsigned short nBlobCount);
		
	int SetData(const char* pData, const int nDataSize);	///< Data로 Blob을 세팅
	int GetData(char* pBuffer, const int nBufferSize);		///< Blob값으로 data를 세팅

	/// 블럽 사이즈 얻어내기
	inline int GetBlobSize();

	/// 블럽의 갯수 얻어내기
	inline int GetBlobCount();

	/// 블럽 전체 사이즈 얻어내기
	int GetBlobTotalSize();

	/// 블럽에서 각 원소 얻어내기
	void* GetBlobElement(int i);

	/// 블럽에 원소단위로 데이터 넣기
	bool SetBlobElement(int i, const void* pData, const int nTypeSize);

	/// Array Block으로 보고 맨 처음 포인터 얻어내기
	void* GetBlobPointer();

	void Clear();
};

inline int MBlob::GetBlobSize()
{
	return m_nTypeSize;
}

inline int MBlob::GetBlobCount()
{
	return m_nBlobCount;
}

#endif // _MBLOB_H