#pragma once

#include "../RLib.h"
#include <map>
#include <string>
#include "profileVisitor.h"

// gpg 3권의 실시간 계층 프로파일러를 거의 그대로 쓴다.
/***************************************************************************************************
**
** profile.h
**
** Real-Time Hierarchical Profiling for Game Programming Gems 3
**
** by Greg Hjelstrom & Byon Garrabrant
**
***************************************************************************************************/

class CProfileVisitor;	

//#define VALIDATOR_THREAD_SAFE_PROFILE_TREE

/*
** A node in the Profile Hierarchy Tree
*/
class CML2_API CProfileNode 
{
	friend CBinarySavingProfileVisitor;
	friend CNameBinarySavingProfileVisitor;

public:
	CProfileNode(int inID, const char * name, CProfileNode * parent ,int inDepth);
	~CProfileNode( void );

	CProfileNode * Find_Child( const char* name );
	void		   Add_Child(CProfileNode* node);

	CProfileNode * Get_Parent( void )			{ return Parent; }
	CProfileNode * Get_Sibling( void )			{ return Sibling; }
	CProfileNode * Get_Child( void )			{ return Child; }

	void			Reset( void );
	void			ResetCurrentInfo( void );
	void			Call( void );
	bool			Return( void );

	const CProfileNode * Get_Parent( void )	const		{ return Parent; }
	const CProfileNode * Get_Sibling( void ) const		{ return Sibling; }
	const CProfileNode * Get_Child( void ) const		{ return Child; }

	int				Get_Depth(void)	const			{ return Depth;}
	const char *	Get_Name( void ) const			{ return Name; }
	int				Get_ID( void )	const			{ return ID; }
	int				Get_Total_Calls( void )	const	{ return TotalCalls; }
	int				Get_Current_Calls(void) const	{ return CurrentFrameCalls;	}
	float			Get_Total_Time( void ) const	{ return TotalTime; }

	float			Get_Current_Time(void) const	{ return Current_Time;	}
	float			Get_Current_GPUTime(void) const { return Current_GPUTime; }

	void			Set_Current_Time(float t)		{	Current_Time = t; }
	void			Set_Current_GPUTime(float t)	{	Current_GPUTime = t; }
	void			Set_Current_Calls(int i)	{	CurrentFrameCalls = i;	}
	void			Set_Depth(int d)			{	Depth = d;	}
	void			Set_ID(int i)				{	ID = i;	}

public:
	void			Traverse(CProfileVisitor* pVisitor);

protected:
	int				Depth;
	const char *	Name;
	int				ID;
	int				TotalCalls;
	int				CurrentFrameCalls;
	float			TotalTime;

	float			Current_Time;		// CPU + GPU 의 현재 프레임에서 걸린시간
	float			Current_GPUTime;	// GPU

	__int64			n64_CPU_StartTime;

	int				RecursionCounter;

	CProfileNode *	Parent;
	CProfileNode *	Child;
	CProfileNode *  ChildEnd;
	CProfileNode *	Sibling;

};

class CML2_API CProfileIDToName
{
public:
	CProfileIDToName(void){}
	~CProfileIDToName(void){}

public:
	void			Clear(void)	{	m_IDToNameMap.clear();	}
	void			MakeFromNameBinary(MBuffer *pProfileBuffer);
	std::string		GetNameFromIDToNameMap(int ID);

private:
	std::map<int,std::string>	m_IDToNameMap;
};

/*
** The Manager for the Profile system
*/
class CML2_API CProfileManager 
{
public:
	CProfileManager(void);
	~CProfileManager(void);

public://CallStack버전 프로파일함수
	CProfileNode*				Start_CallStackProfile(const char* name);		//call stack을 쓰는 버전
	void						Stop_CallStackProfile(void);

	CProfileNode*				GetCallStackProfileRootNode()		{	return m_pCallStackProfileRootNode;}
	CProfileNode *				GetCallStackProfileCurrentNode()	{	return m_pCallStackProfileCurrentNode; }
	int							GetNumberOfCallStackProfileNode()	{	return m_NumberOfCallStackProfileNode;	}

public://NoCallStack버전 프로파일 함수
	void						Start_NoCallStackProfile(const char* name);	//call stack을 안쓰는 버전
	void						Stop_NoCallStackProfile(const char* name);

	CProfileNode*				GetNoCallStackProfileRootNode()		{	return m_pNoCallStackProfileRootNode;	}
	int							GetNumberOfNoCallStackProfileNode()	{	return m_NumberOfNoCallStackProfileNode;}

public://시간관련 함수
	void						Reset( void );
	void						ResetCurrentInfo( void );
	void						Increment_Frame_Counter( void );
	int							Get_Frame_Count_Since_Reset( void )		{ return m_FrameCounter; }
	float						Get_Time_Since_Reset( void );
	float						Get_Time_Since_CurrentReset( void );

	void						Set_Time_Since_ResetCurrentInfoCall(float t)	{	m_TimeSinceResetCurrentInfoCall = t;	}
	float						Get_Time_Since_ResetCurrentInfoCall(void)	{	return m_TimeSinceResetCurrentInfoCall;}

public://탐색
	void						Traverse(CProfileVisitor* pVisitor,CProfileNode* pRootNode);

public://정보 저장/읽기 함수들
	void						MakeBinary(MBuffer* pProfileBuffer,MBuffer* pTempBuffer1,MBuffer* pTempBuffer2);
	void						MakeFromBinary(MBuffer* pProfileBuffer);
	void						MakeRootFromBinary(MBuffer* pProfileBuffer,CProfileNode* pRootNode);

	void						MakeNameBinary(MBuffer* pProfileBuffer);
	void						MakeNameFromNameBinary(MBuffer *pProfileBuffer);

	void						SaveFile(const char* szFileName);

#ifdef	VALIDATOR_THREAD_SAFE_PROFILE_TREE
public:
	int							GetThreadId(void)	{	return m_ThreadID;	}
#endif

private:
	CProfileNode*				Create_ProfileNode(const char * name, CProfileNode * parent);
	CProfileNode*				Create_CallStackProfileNode(const char* name,CProfileNode* parent);
	CProfileNode*				Create_NoCallStackProfileNode(const char* name,CProfileNode* parent);

	int							GetCurrentID(void)	{	return m_ID;	}
	void						IncreaseCurrentID(void)	{	m_ID++;	}

private://시간관련 변수
	int						m_FrameCounter;		//프레임카운터. Increment_Frame_Counter()함수로 직접 증가시켜주어야 한다
	__int64					m_ResetTime;		//Reset()함수가 호출된 시점의 시각
	__int64					m_ResetCurrentTime;	//ResetCurrentInfo()함수가 호출된 시점의 시각

	float					m_TimeSinceResetCurrentInfoCall;	//ResetCurrentInfo()함수가 호출된 이후로 지난 시간
																//이 변수는 디버깅정보를 RS3Analyzer에 줄때에만 쓰인다.
private://ID
	int							m_ID;

protected://CallStack버전 관련 변수
	CProfileNode *			m_pCallStackProfileRootNode;
	CProfileNode *			m_pCallStackProfileCurrentNode;
	int						m_NumberOfCallStackProfileNode;		//컬스택쓰는 프로파일노드 갯수

private://call stack안쓰는 프로파일노드
	CProfileNode*							m_pNoCallStackProfileRootNode;	
	std::map<const char*,CProfileNode*>		m_FunctionNameToProfileNode;	
	int						m_NumberOfNoCallStackProfileNode;	//컬스택안쓰는 프로파일노드 갯수

#ifdef	VALIDATOR_THREAD_SAFE_PROFILE_TREE
private:
	int		m_ThreadID;
#endif
};

CML2_API CProfileManager&	GetProfileManager(void);




/*
** ProfileSampleClass is a simple way to profile a function's scope
** Use the PROFILE macro at the start of scope to time
*/
class	CCallStackProfileSample 
{
public:
	CCallStackProfileSample( const char * name )
	{ 
		GetProfileManager().Start_CallStackProfile( name ); 
	}
	
	~CCallStackProfileSample( void )					
	{ 
		GetProfileManager().Stop_CallStackProfile(); 
	}
};


class	CNoCallStackProfileSample 
{
public:
	CNoCallStackProfileSample( const char * name )
		:m_Name(name)
	{ 
		GetProfileManager().Start_NoCallStackProfile(m_Name); 
	}

	~CNoCallStackProfileSample( void )					
	{ 
		GetProfileManager().Stop_NoCallStackProfile(m_Name); 
	}
private:
	const char* m_Name;
};

// 이터레이터가 좀 안좋아서 봉인
/*
** An iterator to navigate through the tree
*/
/*
class CProfileIterator
{
public:
	// Access all the children of the current parent
	void				First(void);
	void				Next(void);
	bool				Is_Done(void);

	void				Enter_Child( int index );		// Make the given child the new parent
	void				Enter_Largest_Child( void );	// Make the largest child the new parent
	void				Enter_Parent( void );			// Make the current parent's parent the new parent

	// Access the current child
	const char *	Get_Current_Name( void )			{ return CurrentChild->Get_Name(); }
	int				Get_Current_Total_Calls( void )	{ return CurrentChild->Get_Total_Calls(); }
	float				Get_Current_Total_Time( void )	{ return CurrentChild->Get_Total_Time(); }

	// Access the current parent
	const char *	Get_Current_Parent_Name( void )			{ return CurrentParent->Get_Name(); }
	int				Get_Current_Parent_Total_Calls( void )	{ return CurrentParent->Get_Total_Calls(); }
	float				Get_Current_Parent_Total_Time( void )	{ return CurrentParent->Get_Total_Time(); }

protected:

	CProfileNode *	CurrentParent;
	CProfileNode *	CurrentChild;

	CProfileIterator( CProfileNode * start );
	friend	class		CProfileManager;
};
*/
