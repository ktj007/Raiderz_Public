#ifndef _MCOMMOBJECT_H
#define _MCOMMOBJECT_H

#include "MUID.h"
#include "MCommandManager.h"
#include "MPacketCrypter.h"
#include "MThread2.h"

namespace minet {

class MCommandStream;
class MCommandCommunicator;

/// 커뮤니케이터와의 접속을 위한 객체. MCommandCommunicator::Connect()의 파라미터로 들어간다.
class MCommObject {
protected:
	MUID					m_uid;

	MCommandStream*			m_pCommandStream;
	MPacketCrypter			m_PacketCrypter;

	DWORD					m_dwUserContext;		// 소켓 번호

	char					m_szIP[128];
	int						m_nPort;
	DWORD					m_dwIP;
	bool					m_bAllowed;
public:
	MCommObject(MCommandCommunicator* pCommunicator);
	virtual ~MCommObject();
	void Init(MUID uidServer, MUID uidComm, DWORD dwUserContext, const char* pszIP, int nPort);

	MUID GetUID()			{ return m_uid; }

	MCommandStream*		GetCommandStream()				{ return m_pCommandStream; }

	DWORD GetUserContext()								{ return m_dwUserContext; }
	char* GetIPString()									{ return m_szIP; }
	const char* GetIPString() const						{ return m_szIP; }
	DWORD GetIP()										{ return m_dwIP; }
	int GetPort()										{ return m_nPort; }
	void SetAllowed(bool bAllowed) { m_bAllowed = bAllowed; }
	bool IsAllowed() { return m_bAllowed; }
};


class MCommObjectManager
{
private:
	typedef map<MUID, MCommObject*>		MCommObjectMap;
	typedef map<DWORD, MCommObject*>	MCommObjectKeyMap;

	MCommObjectMap					m_CommObjectMap;
	MCommObjectKeyMap				m_CommObjectKeyMap;
	cml2::MCriticalSection			m_csLock;
public:
	MCommObjectManager();
	~MCommObjectManager();
	void Create();
	void Destroy();
	void Add(MCommObject* pCommObj);
	void Delete(const MUID& uid);
	void Delete(DWORD nKey);
	void DeleteAll();
	int GetCount();
	void Lock();
	void Unlock();
	DWORD GetCommObjectKey(const MUID& uid);
	MUID GetCommObjectUID(const DWORD nClientKey);

	// 쓰레드에 안전하지 않은 함수 - Lock, Unlock를 직접 걸어줘야 한다.
	MCommObject* GetCommObjectByKey(DWORD nClientKey);


	/** UID를 가지고 오는게 없어서 만듬.
	MCommObject를 가져오는것 까지는 쓰레드안전을 보장 하지만
	가져온 MCommObject에 대한 처리는 보장하지 못함.
	꼭 고려해서 사용하세요.
	*/
	MCommObject* GetCommObjectByUID(const MUID& uidComm);
};




} // namespace minet

#endif