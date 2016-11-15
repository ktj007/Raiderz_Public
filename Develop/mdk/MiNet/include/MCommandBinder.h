#pragma once

/*
커맨드 바인딩 예제

* 일단 파라미터 5개까지만 만들어놨습니다.

* 첫번째 파라미터는 항상 MCommand* 이다.
* 각 파라미터 타입에 대해 다음의 함수 파라미터 형식으로 받아야 한다
* 같은 타입의 클래스에 두개이상의 인스턴스를 바인딩하면 안됩니다. ( 오동작할수 있어 어서션 달아놨습니다 )

MPT_INT		-> int
MPT_UINT	-> unsigned int
MPT_FLOAT	-> float
MPT_DOUBLE	-> double
MPT_CHAR	-> char
MPT_UCHAR	-> unsigned char
MPT_SHORT	-> short
MPT_USHORT	-> unsigned short
MPT_INT64	-> int64
MPT_UINT64	-> unsigned int64
MPT_BOOL	-> bool
MPT_STR		-> const char*
MPT_VECTOR	-> MVector3
MPT_SVECTOR	-> MShortVector
MPT_UID		-> MUID
MPT_ID		-> MID
MPT_BLOB	-> MBlob*


// 커맨드 디스크립션
	C(MC_MATCH_LOGIN, MCDT_MACHINE2MACHINE, "Login Match Server" )
		P(MPT_STR, "UserID");
		P(MPT_STR, "Password");
		P(MPT_INT, "CommandVersion");
		P(MPT_UINT, "nChecksumPack");
		P(MPT_BLOB,	"MD5Value");


	C(ZC_TEST_TEST, MCDT_LOCAL, "testCommand");
		P( MPT_STR, "UserID" )

// 위의 커맨드에 대한 핸들러 작성 예제
class GunzCommandHandler : public MHandlerBase
{
public:
	MCommandResult OnLogin(MCommand* pCommand, const char* szUserID, const char* szPassword, int nCommandVersion, unsigned int nCheckSum, MBlob blob);
	MCommandResult OnTestTest(MCommand* pCommand, const char* szBlah);
} g_gunzCommandHandler;

// 커뮤니케이터
class GunzServer : public MCommandBinderable
{
public:
} g_gunzServer;


main()
{
	MCommandBinder<GunzCommandHandler> binder(&g_gunzCommandHandler,&g_gunzServer);
	binder.bind(MC_MATCH_LOGIN,	&MMatchServer::OnLogin);
	binder.bind(MC_TEST_TEST,	&MMatchServer::OnTestTest);

	//	or

	//	MCommandBinder<GunzCommandHandler>(&g_gunzCommandHandler,&g_gunzServer)
	//		.bind(MC_MATCH_LOGIN,	&MMatchServer::OnLogin);
	//		.bind(MC_TEST_TEST,		&MMatchServer::OnTestTest);
	//

}



*/

namespace minet {

class MCommand;
class MCommandHandlerCaller;
class MHandlerBase;

// MCommandBinder 로 바인딩할수 있는 클래스
class MCommandDistributer
{
protected:
	map<int,MCommandHandlerCaller> m_cmdHandlers;	///< map : command id -> 커맨드 핸들러

public:
	void			BindCommandHandler(int nCmdID, MCommandHandlerCaller& info);
	MHandlerBase*	GetCommandHandlerInstance(int nCmdID);
	MCommandResult	Distribute(MCommand* pCommand);
};

/// Command Handler의 베이스 클래스
class MHandlerBase
{
public:
	// 커맨드를 검증한다, 괜찮으면 true, 문제가 있으면 false 를 리턴해야 한다
	virtual bool Validate(MCommand* pCommand) { return true; }
};

/// MHandlerFunctionTable 의 베이스클래스, MHandlerFunctionTable은 같은 파라미터 형을 가지는 핸들러 펑션들의 테이블을 관리한다
class MHandlerFunctionTableBase
{
	friend MCommandHandlerCaller;

protected:
	MHandlerBase*	m_pInstance;			// 핸들러의 인스턴스

	virtual MCommandResult OnCommand(MCommand* pCommand, int nHandlerID) { return CR_FALSE; }
};

/// 실제 커맨드 핸들러를 불러주는 클래스
class MCommandHandlerCaller
{
	MHandlerFunctionTableBase*	m_pHandlerFunctionTable;
	int							m_nHandlerID;

public:
	MCommandHandlerCaller(MHandlerFunctionTableBase* pHandlerFunctionTable, int	nHandlerID) 
		: m_pHandlerFunctionTable(pHandlerFunctionTable), m_nHandlerID(nHandlerID) { }
	MCommandHandlerCaller() { }

	inline MCommandResult OnCommand(MCommand* pCommand) { 
		return m_pHandlerFunctionTable->OnCommand(pCommand, m_nHandlerID);
	}

	MHandlerBase* GetHandlerInstance() { return m_pHandlerFunctionTable->m_pInstance; }
	
};

/// MCommunicator 와 Handler 펑션을 바인딩해주는 클래스
template<typename T_HANDLER_CLASS>
class MCommandBinder {
	T_HANDLER_CLASS*			m_pInstance;
	MCommandDistributer*		m_pDistributer;
public:
	/// pInstance : 커맨드 핸들러, T 형식의 instance, pCommunicator : 커맨드 커뮤니케이터 ( 커맨드가 들어오는 곳 )
	MCommandBinder(T_HANDLER_CLASS* pInstance, MCommandDistributer* pDistributer) 
		: m_pInstance(pInstance), m_pDistributer(pDistributer) { }

	template< typename _FPT > struct Type2Enum { enum { Result = -1 }; };
	template<> struct Type2Enum< int >				{ enum { Result = MPT_INT }; };
	template<> struct Type2Enum< unsigned int >		{ enum { Result = MPT_UINT }; };
	template<> struct Type2Enum< float >			{ enum { Result = MPT_FLOAT }; };
	template<> struct Type2Enum< double >			{ enum { Result = MPT_DOUBLE }; };
	template<> struct Type2Enum< char >				{ enum { Result = MPT_CHAR }; };
	template<> struct Type2Enum< unsigned char >	{ enum { Result = MPT_UCHAR }; };
	template<> struct Type2Enum< short >			{ enum { Result = MPT_SHORT }; };
	template<> struct Type2Enum< unsigned short>	{ enum { Result = MPT_USHORT }; };
	template<> struct Type2Enum< int64 >			{ enum { Result = MPT_INT64 }; };
	template<> struct Type2Enum< uint64 >			{ enum { Result = MPT_UINT64 }; };
	template<> struct Type2Enum< bool >				{ enum { Result = MPT_BOOL }; };
	template<> struct Type2Enum< const char* >		{ enum { Result = MPT_STR }; };
	template<> struct Type2Enum< MVector3>			{ enum { Result = MPT_VECTOR }; };
	template<> struct Type2Enum< MSVector >			{ enum { Result = MPT_SVECTOR }; };
	template<> struct Type2Enum< MUID >				{ enum { Result = MPT_UID }; };
	template<> struct Type2Enum< MID >				{ enum { Result = MPT_ID }; };
	template<> struct Type2Enum< MBlob* >			{ enum { Result = MPT_BLOB }; };
	template<> struct Type2Enum< const wchar_t* >	{ enum { Result = MPT_WSTR }; };


	/// MHandlerFunctionTable 같은 파라미터 형을 가지는 핸들러 펑션들의 테이블을 관리한다
	template<typename typename T_PFUNC>
	class MHandlerFunctionTable : public MHandlerFunctionTableBase {
	protected:
//		T_HANDLER_CLASS*	m_pInstance;			// 핸들러의 인스턴스
		vector<T_PFUNC>		m_handlerFunctionTable;	// 핸들러의 (더해진) 펑션테이블

	public:
		MHandlerFunctionTable(T_HANDLER_CLASS* pInstance) { m_pInstance = pInstance; }

		/// 핸들러를 등록한다, 리턴값은 nHandlerID (= m_handlerFuncTable의 인덱스)
		unsigned int AddHandler(T_PFUNC pFunc)
		{
			m_handlerFunctionTable.push_back(pFunc);
			return (unsigned int)m_handlerFunctionTable.size()-1;
		}

		T_PFUNC GetHandler( int nIndex)
		{
			return m_handlerFunctionTable[nIndex];
		}

		T_HANDLER_CLASS* GetHandlerInstance() { return static_cast<T_HANDLER_CLASS*> (m_pInstance); }
	};

	/// 파라미터가 0개인 커맨드 핸들러 펑션을 불러주는 함수
	class MHandlerCaller0 : public MHandlerFunctionTable< MCommandResult (T_HANDLER_CLASS::*)(MCommand*) >
	{
		typedef MCommandResult (T_HANDLER_CLASS::*PFUNC)(MCommand*);

	public:
		MHandlerCaller0(T_HANDLER_CLASS* pInstance) : MHandlerFunctionTable(pInstance) { }

		MCommandResult OnCommand(MCommand* pCommand, int nHandlerID)
		{
			PFUNC func = GetHandler(nHandlerID);
			return (GetHandlerInstance()->*func)(pCommand);
		}
	};

	/// 파라미터가 1개인 커맨드 핸들러 펑션을 불러주는 함수
	template<typename T1>
	class MHandlerCaller1 : public MHandlerFunctionTable< MCommandResult (T_HANDLER_CLASS::*)(MCommand*,T1) >
	{
		typedef MCommandResult (T_HANDLER_CLASS::*PFUNC)(MCommand*,T1);

	public:
		MHandlerCaller1(T_HANDLER_CLASS* pInstance) : MHandlerFunctionTable(pInstance) { }

		MCommandResult OnCommand(MCommand* pCommand, int nHandlerID)
		{
			// 펑션의 파라미터 타입과 커맨드의 파라미터 타입 맞는지 검사
			_ASSERT(Type2Enum<T1>::Result == pCommand->GetParameter(0)->GetType());

			T1 p1 = *(T1*)pCommand->GetParameter(0)->GetPointerRaw();

			PFUNC func = GetHandler(nHandlerID);
			return (GetHandlerInstance()->*func)(pCommand,p1);
		}
	};

	/// 파라미터가 2개인 커맨드 핸들러 펑션을 불러주는 함수
	template<typename T1,typename T2>
	class MHandlerCaller2 : public MHandlerFunctionTable< MCommandResult (T_HANDLER_CLASS::*)(MCommand*,T1,T2) >
	{
		typedef MCommandResult (T_HANDLER_CLASS::*PFUNC)(MCommand*,T1,T2);

	public:
		MHandlerCaller2(T_HANDLER_CLASS* pInstance) : MHandlerFunctionTable(pInstance) { }

		MCommandResult OnCommand(MCommand* pCommand, int nHandlerID)
		{
			// 펑션의 파라미터 타입과 커맨드의 파라미터 타입 맞는지 검사
			_ASSERT(Type2Enum<T1>::Result == pCommand->GetParameter(0)->GetType());
			_ASSERT(Type2Enum<T2>::Result == pCommand->GetParameter(1)->GetType());

			T1 p1 = *(T1*)pCommand->GetParameter(0)->GetPointerRaw();
			T2 p2 = *(T2*)pCommand->GetParameter(1)->GetPointerRaw();

			PFUNC func = GetHandler(nHandlerID);
			return (GetHandlerInstance()->*func)(pCommand,p1,p2);
		}
	};

	/// 파라미터가 3개인 커맨드 핸들러 펑션을 불러주는 함수
	template<typename T1,typename T2,typename T3>
	class MHandlerCaller3 : public MHandlerFunctionTable< MCommandResult (T_HANDLER_CLASS::*)(MCommand*,T1,T2,T3) >
	{
		typedef MCommandResult (T_HANDLER_CLASS::*PFUNC)(MCommand*,T1,T2,T3);

	public:
		MHandlerCaller3(T_HANDLER_CLASS* pInstance) : MHandlerFunctionTable(pInstance) { }

		MCommandResult OnCommand(MCommand* pCommand, int nHandlerID)
		{
			// 펑션의 파라미터 타입과 커맨드의 파라미터 타입 맞는지 검사
			_ASSERT(Type2Enum<T1>::Result == pCommand->GetParameter(0)->GetType());
			_ASSERT(Type2Enum<T2>::Result == pCommand->GetParameter(1)->GetType());
			_ASSERT(Type2Enum<T3>::Result == pCommand->GetParameter(2)->GetType());

			T1 p1 = *(T1*)pCommand->GetParameter(0)->GetPointerRaw();
			T2 p2 = *(T2*)pCommand->GetParameter(1)->GetPointerRaw();
			T3 p3 = *(T3*)pCommand->GetParameter(2)->GetPointerRaw();

			PFUNC func = GetHandler(nHandlerID);
			return (GetHandlerInstance()->*func)(pCommand,p1,p2,p3);
		}
	};

	/// 파라미터가 4개인 커맨드 핸들러 펑션을 불러주는 함수
	template<typename T1,typename T2,typename T3,typename T4>
	class MHandlerCaller4 : public MHandlerFunctionTable< MCommandResult (T_HANDLER_CLASS::*)(MCommand*,T1,T2,T3,T4) >
	{
		typedef MCommandResult (T_HANDLER_CLASS::*PFUNC)(MCommand*,T1,T2,T3,T4);

	public:
		MHandlerCaller4(T_HANDLER_CLASS* pInstance) : MHandlerFunctionTable(pInstance) { }

		MCommandResult OnCommand(MCommand* pCommand, int nHandlerID)
		{
			// 펑션의 파라미터 타입과 커맨드의 파라미터 타입 맞는지 검사
			_ASSERT(Type2Enum<T1>::Result == pCommand->GetParameter(0)->GetType());
			_ASSERT(Type2Enum<T2>::Result == pCommand->GetParameter(1)->GetType());
			_ASSERT(Type2Enum<T3>::Result == pCommand->GetParameter(2)->GetType());
			_ASSERT(Type2Enum<T4>::Result == pCommand->GetParameter(3)->GetType());

			T1 p1 = *(T1*)pCommand->GetParameter(0)->GetPointerRaw();
			T2 p2 = *(T2*)pCommand->GetParameter(1)->GetPointerRaw();
			T3 p3 = *(T3*)pCommand->GetParameter(2)->GetPointerRaw();
			T4 p4 = *(T4*)pCommand->GetParameter(3)->GetPointerRaw();

			PFUNC func = GetHandler(nHandlerID);
			return (GetHandlerInstance()->*func)(pCommand,p1,p2,p3,p4);
		}
	};

	/// 파라미터가 5개인 커맨드 핸들러 펑션을 불러주는 함수
	template<typename T1,typename T2,typename T3,typename T4,typename T5>
	class MHandlerCaller5 : public MHandlerFunctionTable< MCommandResult (T_HANDLER_CLASS::*)(MCommand*,T1,T2,T3,T4,T5) >
	{
		typedef MCommandResult (T_HANDLER_CLASS::*PFUNC)(MCommand*,T1,T2,T3,T4,T5);

	public:
		MHandlerCaller5(T_HANDLER_CLASS* pInstance) : MHandlerFunctionTable(pInstance) { }

		MCommandResult OnCommand(MCommand* pCommand, int nHandlerID)
		{
			// 펑션의 파라미터 타입과 커맨드의 파라미터 타입 맞는지 검사
			_ASSERT(Type2Enum<T1>::Result == pCommand->GetParameter(0)->GetType());
			_ASSERT(Type2Enum<T2>::Result == pCommand->GetParameter(1)->GetType());
			_ASSERT(Type2Enum<T3>::Result == pCommand->GetParameter(2)->GetType());
			_ASSERT(Type2Enum<T4>::Result == pCommand->GetParameter(3)->GetType());
			_ASSERT(Type2Enum<T5>::Result == pCommand->GetParameter(4)->GetType());

			T1 p1 = *(T1*)pCommand->GetParameter(0)->GetPointerRaw();
			T2 p2 = *(T2*)pCommand->GetParameter(1)->GetPointerRaw();
			T3 p3 = *(T3*)pCommand->GetParameter(2)->GetPointerRaw();
			T4 p4 = *(T4*)pCommand->GetParameter(3)->GetPointerRaw();
			T5 p5 = *(T5*)pCommand->GetParameter(4)->GetPointerRaw();

			PFUNC func = GetHandler(nHandlerID);
			return (GetHandlerInstance()->*func)(pCommand,p1,p2,p3,p4,p5);
		}
	};

	/// 파라미터가 0개인 함수를 바인딩(핸들라와 communicator)해주는 함수
	MCommandBinder<T_HANDLER_CLASS>& Bind(int nID, MCommandResult (T_HANDLER_CLASS::*func)(MCommand*)) 
	{ 
		static MHandlerCaller0 handlerCaller( m_pInstance );
		_ASSERT(handlerCaller.GetHandlerInstance()==m_pInstance);
		MCommandHandlerCaller info(&handlerCaller,handlerCaller.AddHandler(func));
		m_pDistributer->BindCommandHandler(nID,info);
		return *this;
	};

	/// 파라미터가 1개인 함수를 바인딩해주는 함수
	template<typename T1>
	MCommandBinder<T_HANDLER_CLASS>& Bind(int nID, MCommandResult (T_HANDLER_CLASS::*func)(MCommand*,T1)) 
	{ 
		static MHandlerCaller1<T1> handlerCaller( m_pInstance );
		_ASSERT(handlerCaller.GetHandlerInstance()==m_pInstance);	///< 같은 바인딩이 두번되면 오동작합니다.
		MCommandHandlerCaller info(&handlerCaller,handlerCaller.AddHandler(func));
		m_pDistributer->BindCommandHandler(nID,info);
		return *this;
	};

	/// 파라미터가 2개인 함수를 바인딩해주는 함수
	template<typename T1,typename T2>
	MCommandBinder<T_HANDLER_CLASS>& Bind(int nID, MCommandResult (T_HANDLER_CLASS::*func)(MCommand*,T1,T2)) 
	{ 
		static MHandlerCaller2<T1,T2> handlerCaller( m_pInstance );
		_ASSERT(handlerCaller.GetHandlerInstance()==m_pInstance);	///< 같은 바인딩이 두번되면 오동작합니다.
		MCommandHandlerCaller info(&handlerCaller,handlerCaller.AddHandler(func));
		m_pDistributer->BindCommandHandler(nID,info);
		return *this;
	};

	/// 파라미터가 3개인 함수를 바인딩해주는 함수
	template<typename T1,typename T2,typename T3>
	MCommandBinder<T_HANDLER_CLASS>& Bind(int nID, MCommandResult (T_HANDLER_CLASS::*func)(MCommand*,T1,T2,T3)) 
	{ 
		static MHandlerCaller3<T1,T2,T3> handlerCaller( m_pInstance );
		_ASSERT(handlerCaller.GetHandlerInstance()==m_pInstance);	///< 같은 바인딩이 두번되면 오동작합니다.
		MCommandHandlerCaller info(&handlerCaller,handlerCaller.AddHandler(func));
		m_pDistributer->BindCommandHandler(nID,info);
		return *this;
	};

	/// 파라미터가 4개인 함수를 바인딩해주는 함수
	template<typename T1,typename T2,typename T3,typename T4>
	MCommandBinder<T_HANDLER_CLASS>& Bind(int nID, MCommandResult (T_HANDLER_CLASS::*func)(MCommand*,T1,T2,T3,T4)) 
	{ 
		static MHandlerCaller4<T1,T2,T3,T4> handlerCaller( m_pInstance );
		_ASSERT(handlerCaller.GetHandlerInstance()==m_pInstance);	///< 같은 바인딩이 두번되면 오동작합니다.
		MCommandHandlerCaller info(&handlerCaller,handlerCaller.AddHandler(func));
		m_pDistributer->BindCommandHandler(nID,info);
		return *this;
	};

	/// 파라미터가 5개인 함수를 바인딩해주는 함수
	template<typename T1,typename T2,typename T3,typename T4,typename T5>
	MCommandBinder<T_HANDLER_CLASS>& Bind(int nID, MCommandResult (T_HANDLER_CLASS::*func)(MCommand*,T1,T2,T3,T4,T5)) 
	{ 
		static MHandlerCaller5<T1,T2,T3,T4,T5> handlerCaller( m_pInstance );
		_ASSERT(handlerCaller.GetHandlerInstance()==m_pInstance);	///< 같은 바인딩이 두번되면 오동작합니다.
		MCommandHandlerCaller info(&handlerCaller,handlerCaller.AddHandler(func));
		m_pDistributer->BindCommandHandler(nID,info);
		return *this;
	};
};

} // namespace minet
