#ifndef MCOMMAND_H
#define MCOMMAND_H

#include "MiNetLib.h"
#include "MUID.h"
#include "MCommandParameter.h"
#include "MCommandDesc.h"

#include <vector>
#include <list>
#include <set>
#include <deque>
using namespace std;

namespace minet {

class MCommandManager;

/// 머신과 머신 혹은 로컬에 전달되는 커멘드
class MINET_API MCommand
{
public:
	MUID						m_Sender;				///< 메세지를 보내는 머신(혹은 오브젝트) UID
	MUID						m_Receiver;				///< 메세지를 받는 머신(혹은 오브젝트) UID
	const MCommandDesc*			m_pCommandDesc;			///< 해당 커맨드의 Description
	vector<MCommandParameter*>	m_Params;				///< 파라미터
protected:
	list<MUID>					m_AdditiveReceiverList;	///< 메세지를 받는 머신이 여러개일 경우 여기다 추가해서 넣는다. 서버만 사용.

	/// 초기화
	void Reset(void);
	/// 파라미터 초기화
	void ClearParam(void);

	MCommand(const MCommandDesc* pCommandDesc, MUID Receiver, MUID Sender);
public:
	MCommand(void);
	MCommand(int nDescID, MUID Sender, MUID Receiver);
	virtual ~MCommand(void);

	/// ID 얻기
	int GetID(void) const { return (m_pCommandDesc) ? m_pCommandDesc->GetID() : 0; }
	/// 설명 얻기
	const char* GetDescription(void){ return (m_pCommandDesc) ? m_pCommandDesc->GetDescription() : ""; }

	/// 파라미터 추가
	bool AddParameter(MCommandParameter* pParam);
	/// 파라미터 갯수 얻기
	int GetParameterCount(void) const;
	/// 파라미터 얻기
	MCommandParameter* GetParameter(int i) const;

	/// 인덱스로 파라미터를 얻는다. 파라미터 타입을 정확하게 명시하지 않으면 false를 리턴한다.
	/// @brief 파라미터 얻기
	/// @param pValue	[out] 파라미터 값
	/// @param i		[in] 파라미터 인덱스
	/// @param t		[in] 파라미터 타입, 정확한 타입을 명시해줘야 한다.
	bool GetParameter(void* pValue, int i, MCommandParameterType t, int nBufferSize=-1) const;
    bool GetParameter(string& str, int i, MCommandParameterType t) const;	
	bool GetParameter(wstring& str, int i, MCommandParameterType t) const;
	
	template<typename T>
	bool GetBlob(T*& pPointer, int& nCount, int i) const
	{
		MCommandParameter* pParam = GetParameter(i);
		if(pParam->GetType()!=MPT_BLOB) return false;
		pPointer = static_cast<T*>(pParam->GetPointer());
		nCount = pParam->GetBlobCount();

		_ASSERT(NULL != pPointer || 0 >= nCount);
		if (NULL == pPointer && 0 < nCount) return false;

		return true;
	};

	template<typename T>
	bool GetBlob(vector<T>& vecT, int i) const
	{
		MCommandParameter* pParam = GetParameter(i);
		if(pParam->GetType()!=MPT_BLOB) return false;
		T* pPointer = static_cast<T*>(pParam->GetPointer());
		int nCount = pParam->GetBlobCount();

		_ASSERT(NULL != pPointer || 0 >= nCount);
		if (NULL == pPointer && 0 < nCount) return false;

		vecT.insert(vecT.end(), pPointer, pPointer + nCount);

		return true;
	};

	template<typename T>
	bool GetSingleBlob(T*& pPointer, int i) const
	{
		MCommandParameter* pParam = GetParameter(i);
		if(pParam->GetType()!=MPT_SINGLE_BLOB) return false;
		pPointer = static_cast<T*>(pParam->GetPointer());

		_ASSERT(NULL != pPointer);
		if (NULL == pPointer) return false;

		return true;
	};

	template<typename T>
	bool GetSingleBlob(T& data, int i) const
	{
		MCommandParameter* pParam = GetParameter(i);
		if(pParam->GetType()!=MPT_SINGLE_BLOB) return false;
		T* pPointer = static_cast<T*>(pParam->GetPointer());

		_ASSERT(NULL != pPointer);
		if (NULL == pPointer) return false;

		if (0 < pParam->GetSingleBlobValueSize())
		{
			data = (*pPointer);
		}		

		return true;
	};

	// 리시버를 알아서 추가
	void AddReceiver(const MUID& uidReceiver);
	void AddReceiver(const vector<MUID>& vecReceivers);
	/// 리시버를 부가적으로 더 추가한다.
	void AddAdditiveReceiver(const MUID& uidReceiver);

	MUID GetSenderUID(void)	const			{ return m_Sender; }
	void SetSenderUID(const MUID &uid)		{ m_Sender = uid; }
	MUID GetReceiverUID(void) const			{ return m_Receiver; }
	void SetReceiverUID(const MUID& uid)	{ m_Receiver = uid; }
	bool IsNoReceiver()						{ return (m_Receiver.IsInvalid()); }

	bool IsLocalCommand(void)				{ return (m_Sender==m_Receiver); }
	bool HasAdditiveReceivers()				{ return (!m_AdditiveReceiverList.empty()); }
	int GetReceiverCount();
	list<MUID>::iterator GetAdditiveReceiverListBegin()	{ return m_AdditiveReceiverList.begin(); }
	list<MUID>::iterator GetAdditiveReceiverListEnd()	{ return m_AdditiveReceiverList.end(); }

	/// 같은 내용을 가진 커맨드 복제
	MCommand* Clone(void) const;

	/// Description에 맞게끔 설정되었는가?
	bool CheckRule(void);	

	/// 커맨드 메모리 블럭 데이터로 얻어내기
	/// @param pData	[out] 커맨드 데이터 블럭
	/// @param nSize	[in] 커맨드 데이터 블럭 최대 사이즈
	/// @return			블럭 사이즈
	int GetData(char* pData, uint16 nSize);
	/// 커맨드 메모리 블럭 데이터로부터 저장
	/// @param pData	[in] 커맨드 데이터 블럭
	/// @param pPM		[in] 커맨드 매니져(MCommandDesc를 enum할 수 있다.)
	/// @return			성공 여부
	bool SetData(char* pData, uint16 nDataLen=USHRT_MAX);

	int GetSize();

	/// MCommandDesc으로 ID 지정
	void SetID(const MCommandDesc* pCommandDesc);
	/// ID 지정
	void SetID(int nID);

	virtual MCommandResult Run();
};





// 라인수 줄이기 위한 매크로
#define NEWCMD(_ID)		(new MCommand(_ID))
#define AP(_P)			AddParameter(new _P)
#define MKCMD(_C, _ID)									{ _C = NEWCMD(_ID); }
#define MKCMD1(_C, _ID, _P0)							{ _C = NEWCMD(_ID); _C->AP(_P0); }
#define MKCMD2(_C, _ID, _P0, _P1)						{ _C = NEWCMD(_ID); _C->AP(_P0); _C->AP(_P1); }
#define MKCMD3(_C, _ID, _P0, _P1, _P2)					{ _C = NEWCMD(_ID); _C->AP(_P0); _C->AP(_P1); _C->AP(_P2); }
#define MKCMD4(_C, _ID, _P0, _P1, _P2, _P3)				{ _C = NEWCMD(_ID); _C->AP(_P0); _C->AP(_P1); _C->AP(_P2); _C->AP(_P3); }
#define MKCMD5(_C, _ID, _P0, _P1, _P2, _P3, _P4)		{ _C = NEWCMD(_ID); _C->AP(_P0); _C->AP(_P1); _C->AP(_P2); _C->AP(_P3); _C->AP(_P4); }
#define MKCMD6(_C, _ID, _P0, _P1, _P2, _P3, _P4, _P5)	{ _C = NEWCMD(_ID); _C->AP(_P0); _C->AP(_P1); _C->AP(_P2); _C->AP(_P3); _C->AP(_P4); _C->AP(_P5); }


// Short Name
typedef MCommand				MCmd;
typedef MCommandDesc			MCmdDesc;

} // namespace minet

#endif