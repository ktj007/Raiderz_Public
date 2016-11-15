#ifndef _MCOMMAND_DESC_H
#define _MCOMMAND_DESC_H

#include "MiNetLib.h"
#include "MSingleton.h"
#include "MCommandParameter.h"
#include "MCommandHandler.h"

namespace minet {

// Command Description Flag
enum MCDFlags
{
	MCDT_NOTINITIALIZED			= 0x0000,	///< 초기화가 안된 상태

	MCDT_LOCAL					= 0x0001,	///< 로컬로 전달되는 커맨드
	MCDT_MACHINE2MACHINE		= 0x0002,	///< 머신에서 머신으로 전달되는 커맨드
	MCDT_PEER2PEER				= 0x0004,	///< Peer에서 Peer로 전달되는 커맨드, MACHINE2MACHINE와는 따로 구분한다.
	MCDT_NON_ENCRYPTED			= 0x0008,	///< 암호화하지 않는 커맨드

	MCDT_FILTER_CROWDED			= 0x0010,	///< 섹터 혼잡도 필터옵션 적용
	MCDT_FILTER_IGNORE			= 0x0020,	///< 어떤 경우라도 필터에 적용받지 않는다
	
};

// 서버 대 서버 커맨드는 기본적으로 암호화하지 않는다.
#define MCDT_SERVER2SERVER (MCDT_MACHINE2MACHINE | MCDT_NON_ENCRYPTED)

enum MCommandFlow
{
	MCF_ANY	= 0,	///< 사용하지 않거나 C<->S
	MCF_C2S	= 1,	///< Client -> Server
	MCF_S2C	= 2		///< Server -> Client
};

/// 커맨드의 속성을 정의
/// - MCommandDesc::m_nID 와 같은 ID를 가진 MCommand는 MCommandDesc::m_ParamDescs에 정의된 파라미터를 가지게 된다.
class MINET_API MCommandDesc {
protected:
	int				m_nID;					///< Command ID
	char			m_szName[256];			///< Name for Parsing
	char			m_szDescription[256];	///< Description of Command
	unsigned int	m_nFlag;				///< Command Description Flag
	MCommandFlow	m_nFlow;				///< Flow

	vector<MCommandParameterDesc*>	m_ParamDescs;	///< Parameters
public:
	MCommandHandler*	m_pHandler;		///< 커맨드 핸들러
	MCommandHanderFunc* m_pFunc;		///< 커맨드 핸들러 함수 포인터
public:
	/// @param nID				커맨드 ID
	/// @param nFlag			커맨드 플래그, MCDT_NOTINITIALIZED
	/// @param szName			커맨드 이름
	/// @param szDescription	커맨드 설명
	MCommandDesc(int nID, int nFlag, MCommandFlow nFlow, const char* szName, const char* szDescription);
	virtual ~MCommandDesc(void);

	/// MCommandParameterDesc 추가
	void AddParamDesc(MCommandParameterDesc* pParamDesc);

	/// 플래그 검사
	bool IsFlag(int nFlag) const;
	/// 아이디 얻기
	int GetID(void) const { return m_nID; }
	/// 이름 얻기
	const char* GetName(void) const { return m_szName; }
	/// 설명 얻기
	const char* GetDescription(void) const { return m_szDescription; }
	MCommandFlow GetFlow() const { return m_nFlow; }
	/// Parameter Description 얻기
	MCommandParameterDesc* GetParameterDesc(size_t i) const {
		if(i<0 || i>=m_ParamDescs.size()) return NULL;
		return m_ParamDescs[i];
	}
	/// Parameter Description 갯수 얻기
	size_t GetParameterDescCount(void) const 
	{
		return m_ParamDescs.size();
	}
	MCommandParameterType GetParameterType(size_t i) const
	{
		if(i<0 || i>=m_ParamDescs.size()) return MPT_END;
		return m_ParamDescs[i]->GetType();
	}
};

///< Command Description Map
class MINET_API MCommandDescMap
{
private:
	typedef map<int, MCommandDesc*>	MDescMap;
	MDescMap						m_DescMap;
public:
	MCommandDescMap();
	~MCommandDescMap();
	static MCommandDescMap* GetInstance();
	void Clear();

	int GetCount(void) const		{ return (int)m_DescMap.size(); }
	MCommandDesc* GetCommandDesc(int i);
	MCommandDesc* GetCommandDescByID(int nID);
	MCommandDesc* GetCommandDescByName(const char* chName);
	void AddCommandDesc(MCommandDesc* pCD);
};

inline MCommandDescMap* MGetCommandDescMap()
{
	return MCommandDescMap::GetInstance();
}

// Command Description Add Macro For Convenience
#define BEGIN_CMD_DESC()				{ MCommandDesc* pCD4m; 
#define END_CMD_DESC()					}
#define C(_a, _b, _c, _d)				{ pCD4m = new MCommandDesc(_a, _b, _c, #_a, _d); MGetCommandDescMap()->AddCommandDesc(pCD4m); }
#define P(_a, _b)						{ pCD4m->AddParamDesc(new MCommandParameterDesc(_a, _b)); }
#define CA(_a, _b)						{ pCM4m->AddAlias(_a, _b); }
#define P_MINMAX(_a, _b, _min, _max)	{ MCommandParameterDesc* pNewDesc = new MCommandParameterDesc(_a, _b);  \
	                                      pNewDesc->AddCondition(new MCommandParamConditionMinMax(_min, _max)); \
										  pCD4m->AddParamDesc(pNewDesc); }

} // namespace minet

#endif