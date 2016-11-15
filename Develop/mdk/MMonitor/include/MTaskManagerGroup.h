#ifndef _MMONITORTASKMANAGERGROUP
#define _MMONITORTASKMANAGERGROUP


#include <map>
#include <string>
#include <vector>


using std::map;
using std::string;
using std::vector;


namespace mmonitor {

#define MMONITOR_TASK_MAX_INDEX 4200000000


class MTask;
class MTaskManager;


class MTaskManagerGroup
{
protected :
	string					m_strName;
	vector< MTaskManager* >	m_TaskManagerList;	// TaskManager는 Type, IP, Port로 구분이 되어 저장된다.
												// 모든 Task는 xml에 정의된 Type, IP, Port와 일치하는 TaskManager에 등록이 된다.

	map< string, MTask* >	m_TaskMap;			// Task의 빠른 검색을 위해서 이름으로 북마크를 해놓는다.
												// 모든 Task는 TaskManager에 등록될때 반드시 북마크되어야 한다.
	bool					m_IsUsableNameIndex;

	map< DWORD, MTask* >	m_TaskIndexMap;		// Index를 이용한 검색.

	DWORD					m_nIndexCache;		// Task를 만들때 고유 Index를 셋팅할때 사용된다. Index검색을 위해서 사용.
												// Index는 1부터 시작. 0은 사용하지 않음.

private :
	bool			AddTaskManager( MTaskManager* pTaskManager );
	bool			AddTaskManager( const string& strType, const string& strIP, const unsigned short nPort );

protected :
	bool			AddTaskToMap( MTask* pTask );
	void			DeleteTaskFromMap( MTask* pTask );
	DWORD			MakeIndex();

public :
	MTaskManagerGroup();
	~MTaskManagerGroup();

	const string&	GetName() const								{ return m_strName; }
	const DWORD		GetTaskManagerCount() const 				{ return static_cast<DWORD>(m_TaskManagerList.size()); }
	const DWORD		GetTaskCount() const						{ return static_cast<DWORD>(m_TaskIndexMap.size()); }
	MTaskManager*	GetTaskManager( const DWORD dwIndex );
	MTaskManager*	GetTaskManager( const string& strTaskManagerName );
	MTaskManager*	GetTaskManager( const string& strType, const string& strIP, const unsigned short nPort );
	MTask*			GetTask( const string& strTaskName );
	MTask*			GetTask( const DWORD dwIndex );

	bool			AddTask( const string& strType, const string& strIP, const unsigned short nPort, MTask* pTask );
	
	void			Release();
};


}

#endif