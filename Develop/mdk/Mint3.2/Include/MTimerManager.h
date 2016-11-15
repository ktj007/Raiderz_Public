#pragma once

#include <string>
#include <vector>
using namespace std;


namespace mint3
{

class MWidget;



// MTimerNode
struct MTimerNode
{
	MWidget*		pOwner;
	int				nElapse;
	DWORD			dwEventData;
	DWORD			dwNextEventTime;
	bool			bDelete;
};




/// class : MTimerManager
class MTimerManager
{
protected:
	list< MTimerNode*>		m_vTimer;


public:
	MTimerManager();
	virtual ~MTimerManager();

	bool Add( MWidget* pWidget, int nElapse, DWORD dwEventData);
	bool Remove( MWidget* pWidget);
	void Clear();

	void Update( DWORD dwCurrTime);
};





} // namespace mint3
