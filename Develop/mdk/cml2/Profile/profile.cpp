/***************************************************************************************************
**
** profile.cpp
**
** Real-Time Hierarchical Profiling for Game Programming Gems 3
**
** by Greg Hjelstrom & Byon Garrabrant
**
***************************************************************************************************/
#include "stdafx.h"
#include "profile.h"

inline void Profile_Get_Ticks(_int64 * ticks)
{
	QueryPerformanceCounter((LARGE_INTEGER*)ticks);
	return;

	__asm
	{
		push eax;
		push edx;
		push ecx;
		mov ecx,ticks;
		_emit 0Fh
		_emit 31h
		mov [ecx],eax;
		mov [ecx+4],edx;
		pop ecx;
		pop edx;
		pop eax;
	}
}

inline float Profile_Get_Tick_Rate(void)
{
	static float _CPUFrequency = -1.0f;
	
	if (_CPUFrequency == -1.0f) {
		__int64 curr_rate = 0;
		::QueryPerformanceFrequency ((LARGE_INTEGER *)&curr_rate);
		_CPUFrequency = (float)curr_rate;
	} 
	
	return _CPUFrequency;
}

/***************************************************************************************************
**
** CProfileNode
**
***************************************************************************************************/

/***********************************************************************************************
 * INPUT:                                                                                      *
 * name - pointer to a static string which is the name of this profile node                    *
 * parent - parent pointer                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * The name is assumed to be a static pointer, only the pointer is stored and compared for     *
 * efficiency reasons.                                                                         *
 *=============================================================================================*/
CProfileNode::CProfileNode(int inID, const char * name, CProfileNode * parent,int inDepth ) :
	Name( name ),
	TotalCalls( 0 ),
	CurrentFrameCalls(0),
	TotalTime( 0 ),
	Current_Time( 0 ),
	Current_GPUTime(0),
	n64_CPU_StartTime( 0 ),
	RecursionCounter( 0 ),
	Parent( parent ),
	Child( NULL ),
	ChildEnd( NULL ),
	Sibling( NULL ),
	ID(inID),
	Depth(inDepth)
{
	Reset();
}


CProfileNode::~CProfileNode( void )
{
	delete Child;
	delete Sibling;
}


CProfileNode * CProfileNode::Find_Child( const char* name )
{
	// Try to find this sub node
	CProfileNode * child = Child;
	while ( child ) 
	{
		if ( child->Name == name ) 
		{
			return child;
		}
		child = child->Sibling;
	}
	return NULL;
}

void CProfileNode::Add_Child( CProfileNode* node )
{
	if( NULL == ChildEnd )
	{
		ChildEnd = node;
		Child = node;
	}
	else
	{
		ChildEnd->Sibling = node;
		ChildEnd = node;
	}
}

void	CProfileNode::Reset( void )
{
	TotalCalls = 0;
	CurrentFrameCalls = 0;
	TotalTime = 0.0f;

	Current_Time	= 0.0f;
	Current_GPUTime = 0.0f;

	if ( Child ) {
		Child->Reset();
	}
	if ( Sibling ) {
		Sibling->Reset();
	}
}


void CProfileNode::ResetCurrentInfo( void )
{
	Current_Time	= 0.0f;
	Current_GPUTime = 0.0f;
	CurrentFrameCalls = 0;
	if ( Child ) {
		Child->ResetCurrentInfo();
	}
	if ( Sibling ) {
		Sibling->ResetCurrentInfo();
	}
}

void	CProfileNode::Call( void )
{
	TotalCalls++;
	CurrentFrameCalls++;
	if (RecursionCounter++ == 0) {
		Profile_Get_Ticks(&n64_CPU_StartTime);
	}
}


bool	CProfileNode::Return( void )
{
	if ( --RecursionCounter == 0 && TotalCalls != 0 ) { 
		__int64 time;
		Profile_Get_Ticks(&time);
		time-=n64_CPU_StartTime;
		Current_Time += (float)time / Profile_Get_Tick_Rate();
		TotalTime += (float)time / Profile_Get_Tick_Rate();
	}
	return ( RecursionCounter == 0 );
}

void CProfileNode::Traverse( CProfileVisitor* pVisitor )
{
	pVisitor->PreOrderVisit(this);
	{
		CProfileNode* pChild = Get_Child();

		while(pChild!=NULL)
		{
			pChild->Traverse(pVisitor);
			pChild = pChild->Get_Sibling();
		}
	}
	pVisitor->PostOrderVisit(this);
}

/***************************************************************************************************
**
** CProfileIterator
**
***************************************************************************************************/
/*
CProfileIterator::CProfileIterator( CProfileNode * start )
{
	CurrentParent = start;
	CurrentChild = CurrentParent->Get_Child();
}


void	CProfileIterator::First(void)
{
	CurrentChild = CurrentParent->Get_Child();
}


void	CProfileIterator::Next(void)
{
	CurrentChild = CurrentChild->Get_Sibling();
}


bool	CProfileIterator::Is_Done(void)
{
	return CurrentChild == NULL;
}


void	CProfileIterator::Enter_Child( int index )
{
	CurrentChild = CurrentParent->Get_Child();
	while ( (CurrentChild != NULL) && (index != 0) ) {
		index--;
		CurrentChild = CurrentChild->Get_Sibling();
	}

	if ( CurrentChild != NULL ) {
		CurrentParent = CurrentChild;
		CurrentChild = CurrentParent->Get_Child();
	}
}


void	CProfileIterator::Enter_Parent( void )
{
	if ( CurrentParent->Get_Parent() != NULL ) {
		CurrentParent = CurrentParent->Get_Parent();
	}
	CurrentChild = CurrentParent->Get_Child();
}
*/

#ifdef VALIDATOR_THREAD_SAFE_PROFILE_TREE
int get_current_thread_ID()
{
	//보면 알겠지만, 굉장히 위험한 코드. os가 사용하는 구조체의 구조를 예측하여 , 원하는 변수의 포인터를 직접 셋팅하여, 
	//그 포인터가 가리키는 값을 직접 가지고옴. 당연히 os패치같은게 일어나서 구조체의 구조가 바뀌면 동작안하는 함수
	//2010.07.07, windows xp에서는 정상작동하는거 같음
	//함수가 호출될때 해당함수가 호출되는 쓰레드의 ID를 얻을수 있는 다른 방법이 있으면, 이 함수를 그 함수로 대체하여야 함
	int ID = 0;
	__asm
	{
		// TEB의 0x20 번째 CLIENT_ID 구조체주소를 얻는다.
		// CLIENT_ID 구조체의 첫번째는 UniqueProcess와 두번째 UniqueThread로 이루어져 있기때문에
		// 두번째 값을 구하기 위해 4byte 이동한다.
		mov  eax, FS:[0x24]
		mov  ID, eax
	}
	return ID;

}
#endif

CProfileManager::CProfileManager( void )
:m_pCallStackProfileRootNode(NULL),m_pCallStackProfileCurrentNode(NULL),m_FrameCounter(0),m_ResetTime(0),m_ResetCurrentTime(0),m_ID(1),
	m_pNoCallStackProfileRootNode(NULL),m_NumberOfCallStackProfileNode(0),m_NumberOfNoCallStackProfileNode(0)
{
	//Call Stack용
	m_pCallStackProfileRootNode = Create_CallStackProfileNode("CallStackRoot",NULL);
	m_pCallStackProfileCurrentNode = m_pCallStackProfileRootNode;
	
	//No Call Stack용
	m_pNoCallStackProfileRootNode = Create_NoCallStackProfileNode("NoCallStackRoot",NULL);

#ifdef VALIDATOR_THREAD_SAFE_PROFILE_TREE
	m_ThreadID = get_current_thread_ID();
#endif
}

CProfileManager::~CProfileManager( void )
{
	if( NULL != m_pCallStackProfileRootNode )
	{
		delete m_pCallStackProfileRootNode;
		m_pCallStackProfileRootNode = NULL;
	}

	if( NULL != m_pNoCallStackProfileRootNode )
	{
		delete m_pNoCallStackProfileRootNode;
		m_pNoCallStackProfileRootNode = NULL;
	}
}

CProfileNode* CProfileManager::Create_ProfileNode( const char * name, CProfileNode * parent )
{
	int ID = GetCurrentID();
	IncreaseCurrentID();

	int Depth = 0;
	if( parent ) Depth = parent->Get_Depth() + 1;

	return new CProfileNode(ID, name, parent,Depth);
}

CProfileNode* CProfileManager::Create_CallStackProfileNode( const char* name,CProfileNode* parent )
{
	m_NumberOfCallStackProfileNode++;
	return Create_ProfileNode(name,parent);
}

CProfileNode* CProfileManager::Create_NoCallStackProfileNode( const char* name,CProfileNode* parent )
{
	m_NumberOfNoCallStackProfileNode++;
	return Create_ProfileNode(name,parent);
}

/***********************************************************************************************
 * CProfileManager::Start_CallStackProfile -- Begin a named profile                                    *
 *                                                                                             *
 * Steps one level deeper into the tree, if a child already exists with the specified name     *
 * then it accumulates the profiling; otherwise a new child node is added to the profile tree. *
 *                                                                                             *
 * INPUT:                                                                                      *
 * name - name of this profiling record                                                        *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * The string used is assumed to be a static string; pointer compares are used throughout      *
 * the profiling code for efficiency.                                                          *
 *=============================================================================================*/
CProfileNode*	CProfileManager::Start_CallStackProfile( const char * name)
{
#ifdef VALIDATOR_THREAD_SAFE_PROFILE_TREE	
	int tid = get_current_thread_ID();
	int id = GetThreadId();
	if( id !=  tid)
	{
		_ASSERT(false);
		//mlog("\tThread ID : %i , Thread ID : %i , Name : %s \n",id,tid,name);
	}
#endif	
	
	if (name != m_pCallStackProfileCurrentNode->Get_Name()) 
	{
		CProfileNode * node = NULL;
		node = m_pCallStackProfileCurrentNode->Find_Child(name);

		if( NULL == node ) 
		{
			node = Create_CallStackProfileNode(name,m_pCallStackProfileCurrentNode);
			m_pCallStackProfileCurrentNode->Add_Child(node);
		}
		m_pCallStackProfileCurrentNode = node;
	} 
	
	m_pCallStackProfileCurrentNode->Call();

	return m_pCallStackProfileCurrentNode;
}


/***********************************************************************************************
 * CProfileManager::Stop_CallStackProfile -- Stop timing and record the results.                       *
 *=============================================================================================*/
void	CProfileManager::Stop_CallStackProfile( void )
{
	// Return will indicate whether we should back up to our parent (we may
	// be profiling a recursive function)
	if (m_pCallStackProfileCurrentNode->Return()) {
		m_pCallStackProfileCurrentNode = m_pCallStackProfileCurrentNode->Get_Parent();
	}
}


void CProfileManager::Start_NoCallStackProfile( const char* name )
{
	m_FunctionNameToProfileNode;

	CProfileNode * node = NULL;
	std::map<const char*,CProfileNode*>::iterator iter;	
	iter = m_FunctionNameToProfileNode.find(name);

	if( m_FunctionNameToProfileNode.end() == iter ) 
	{
		node = Create_NoCallStackProfileNode(name,m_pNoCallStackProfileRootNode);
		m_pNoCallStackProfileRootNode->Add_Child(node);
		m_FunctionNameToProfileNode.insert(std::pair<const char*,CProfileNode*>(name,node));
	}
	else
	{
		node = iter->second;
	}
	
	
	node->Call();
}

void CProfileManager::Stop_NoCallStackProfile( const char* name )
{
	m_FunctionNameToProfileNode;

	CProfileNode * node = NULL;
	std::map<const char*,CProfileNode*>::iterator iter;	
	iter = m_FunctionNameToProfileNode.find(name);

	if( m_FunctionNameToProfileNode.end() == iter ) 
	{
		return;
	}
	else
	{
		node = iter->second;
	}

	node->Return();
}

/***********************************************************************************************
 * CProfileManager::Reset -- Reset the contents of the profiling system                       *
 *                                                                                             *
 *    This resets everything except for the tree structure.  All of the timing data is reset.  *
 *=============================================================================================*/
void	CProfileManager::Reset( void )
{ 
	m_pCallStackProfileRootNode->Reset(); 
	m_FrameCounter = 0;
	Profile_Get_Ticks(&m_ResetTime);
}

/***********************************************************************************************
 * CProfileManager::ResetCurrentInfo -- Reset the contents of the profiling system  current information                     *
 *                                                                                             *
 *=============================================================================================*/
void CProfileManager::ResetCurrentInfo( void )
{
	m_pCallStackProfileRootNode->ResetCurrentInfo();
	m_pNoCallStackProfileRootNode->ResetCurrentInfo();
	Profile_Get_Ticks(&m_ResetCurrentTime);
}

/***********************************************************************************************
 * CProfileManager::Increment_Frame_Counter -- Increment the frame counter                    *
 *=============================================================================================*/
void CProfileManager::Increment_Frame_Counter( void )
{
	m_FrameCounter++;
}


/***********************************************************************************************
 * CProfileManager::Get_Time_Since_Reset -- returns the elapsed time since last reset         *
 *=============================================================================================*/
float CProfileManager::Get_Time_Since_Reset( void )
{
	__int64 time;
	Profile_Get_Ticks(&time);
	time -= m_ResetTime;
	return (float)time / Profile_Get_Tick_Rate();
}

float CProfileManager::Get_Time_Since_CurrentReset( void )
{
	__int64 time;
	Profile_Get_Ticks(&time);
	time -= m_ResetCurrentTime;
	return (float)time / Profile_Get_Tick_Rate();
}

void CProfileManager::Traverse( CProfileVisitor* pVisitor ,CProfileNode* pRootNode)
{
	pVisitor->PreTraverse(this);
	pRootNode->Traverse(pVisitor);
	pVisitor->PostTraverse(this);
}

void CProfileManager::MakeBinary( MBuffer* pProfileBuffer ,MBuffer* pTempBuffer1,MBuffer* pTempBuffer2)
{
	MBuffer* pCallStackProfileBuffer = pTempBuffer1;
	MBuffer* pNoCallStackProfileBuffer = pTempBuffer2;

	CBinarySavingProfileVisitor CallStackProfileBinaryVisitor(this,pCallStackProfileBuffer);
	Traverse(&CallStackProfileBinaryVisitor,GetCallStackProfileRootNode());

	CBinarySavingProfileVisitor NoCallStackProfileBinaryVisitor(this,pNoCallStackProfileBuffer);
	Traverse(&NoCallStackProfileBinaryVisitor,GetNoCallStackProfileRootNode());

	int CallStackProfileSize	= pCallStackProfileBuffer->GetDataSize();
	int NoCallStackProfileSize	= pNoCallStackProfileBuffer->GetDataSize();

	pProfileBuffer->Append((char*)&CallStackProfileSize,sizeof(CallStackProfileSize));
	pProfileBuffer->Append((char*)&NoCallStackProfileSize,sizeof(NoCallStackProfileSize));
	pProfileBuffer->Append(pCallStackProfileBuffer->GetBufferPointer(),pCallStackProfileBuffer->GetDataSize());
	pProfileBuffer->Append(pNoCallStackProfileBuffer->GetBufferPointer(),pNoCallStackProfileBuffer->GetDataSize());
}

void CProfileManager::MakeFromBinary( MBuffer* pProfileBuffer)
{
	int CallStackProfileSize	= 0;
	int NoCallStackProfileSize	= 0;

	pProfileBuffer->Read((char*)&CallStackProfileSize,sizeof(CallStackProfileSize));
	pProfileBuffer->Read((char*)&NoCallStackProfileSize,sizeof(NoCallStackProfileSize));

	MBuffer CallStackProfileBuffer;
	MBuffer NoCallStackProfileBuffer;
	CallStackProfileBuffer.AllocateBuffer(CallStackProfileSize);
	NoCallStackProfileBuffer.AllocateBuffer(NoCallStackProfileSize);

	char* BufferPointer = pProfileBuffer->GetBufferPointer() + sizeof(CallStackProfileSize) + sizeof(NoCallStackProfileSize);
	CallStackProfileBuffer.Append(BufferPointer,CallStackProfileSize);
	BufferPointer+=CallStackProfileSize;
	NoCallStackProfileBuffer.Append(BufferPointer,NoCallStackProfileSize);

	MakeRootFromBinary(&CallStackProfileBuffer,GetCallStackProfileRootNode());
	MakeRootFromBinary(&NoCallStackProfileBuffer,GetNoCallStackProfileRootNode());
}

void CProfileManager::MakeRootFromBinary( MBuffer* pProfileBuffer,CProfileNode* pRootNode )
{
	int depth = 0;
	int ID = 0;
	float Current_Time		= 0.0f;
	float Current_GPUTime	= 0.0f;
	int	CurrentFrameCalls	= 0;

	CProfileNode* pParentNode = NULL;
	CProfileNode* pPreNode = NULL;
	CProfileNode* pNewNode = NULL;


	//Current time셋팅
	float TimeSinceCurrentReset;
	pProfileBuffer->Read((char*)&TimeSinceCurrentReset,sizeof(TimeSinceCurrentReset));
	this->Set_Time_Since_ResetCurrentInfoCall(TimeSinceCurrentReset);

	//Root노드 셋팅
	pProfileBuffer->Read((char*)&depth,sizeof(depth));
	pProfileBuffer->Read((char*)&ID,sizeof(ID));
	pProfileBuffer->Read((char*)&Current_Time,sizeof(Current_Time));
	pProfileBuffer->Read((char*)&Current_GPUTime,sizeof(Current_GPUTime));
	pProfileBuffer->Read((char*)&CurrentFrameCalls,sizeof(CurrentFrameCalls));

	pRootNode->Set_ID(ID);
	pRootNode->Set_Current_Time(Current_Time);
	pRootNode->Set_Current_GPUTime(Current_GPUTime);
	pRootNode->Set_Current_Calls(CurrentFrameCalls);
	pPreNode = pRootNode;

	while( pProfileBuffer->GetRemainReadSize() > 0 )
	{
		pProfileBuffer->Read((char*)&depth,sizeof(depth));
		pProfileBuffer->Read((char*)&ID,sizeof(depth));
		pProfileBuffer->Read((char*)&Current_Time,sizeof(Current_Time));
		pProfileBuffer->Read((char*)&Current_GPUTime,sizeof(Current_GPUTime));
		pProfileBuffer->Read((char*)&CurrentFrameCalls,sizeof(CurrentFrameCalls));

		if( Current_GPUTime > 0)
			int aa = 0;

		if( pPreNode->Get_Depth() == depth )
		{//같은 깊이이면 같은 부모
			pParentNode = pParentNode;
		}
		else if( (pPreNode->Get_Depth()+1) == depth )
		{//한 깊이 만큼 더 들어갔을경우
			pParentNode = pPreNode;
		}
		else 
		{
			_ASSERT( pPreNode->Get_Depth() > depth );//깊이가 줄어들어든 경우

			while(true)
			{
				pParentNode = pParentNode->Get_Parent();
				if( pParentNode->Get_Depth() == (depth-1) ) break;
			}
		}

		pNewNode = new CProfileNode(ID,NULL,pParentNode,depth);
		pNewNode->Set_Current_Time(Current_Time);
		pNewNode->Set_Current_GPUTime(Current_GPUTime);
		pNewNode->Set_Current_Calls(CurrentFrameCalls);
		if(NULL != pParentNode) pParentNode->Add_Child(pNewNode);

	
		pPreNode = pNewNode;
	}
}

void CProfileManager::MakeNameBinary( MBuffer* pProfileBuffer )
{
	CNameBinarySavingProfileVisitor ProfileNameBinaryVisitor(pProfileBuffer);
	Traverse(&ProfileNameBinaryVisitor,m_pCallStackProfileRootNode);
	Traverse(&ProfileNameBinaryVisitor,m_pNoCallStackProfileRootNode);
}



void SaveFileProfileCallBack(const char* szString, void* pUserPointer)
{
	FILE* pFile = (FILE*)pUserPointer;
	fprintf(pFile,"%s",szString);
}

void CProfileManager::SaveFile( const char* szFileName )
{
	FILE *pFile = fopen(szFileName,"w");
	CProfileInfoVisitor ProfileInfoVisitor(this,(void*)pFile,&SaveFileProfileCallBack);
	Traverse(&ProfileInfoVisitor,m_pCallStackProfileRootNode);
	fclose(pFile);
}

CML2_API CProfileManager& GetProfileManager( void )
{
	static CProfileManager ProfileManager;
	return ProfileManager;
}


void CProfileIDToName::MakeFromNameBinary( MBuffer *pProfileBuffer )
{
	int ID = 0;
	int NameSize = 0;
	char buffer[255];

	while( pProfileBuffer->GetRemainReadSize() > 0 )
	{
		pProfileBuffer->Read((char*)&ID,sizeof(ID));
		pProfileBuffer->Read((char*)&NameSize,sizeof(NameSize));
		pProfileBuffer->Read(buffer,NameSize);		buffer[NameSize] = 0; //NULL끝처리 안해주면 버그 !

		std::string NameString(buffer);

		m_IDToNameMap.insert(std::pair<int,std::string>(ID,NameString));
	}
}

std::string CProfileIDToName::GetNameFromIDToNameMap( int ID )
{
	std::map<int,std::string>::iterator iter;
	iter = m_IDToNameMap.find(ID);

	if( iter == m_IDToNameMap.end() )	return	std::string("None");

	return iter->second;	
}