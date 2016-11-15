#pragma once


#include <map>
using namespace std;


namespace mint3
{

class MLookBase;


/// class : MLookManager
class MLookManager
{
/// Member variables
protected:
	map< string, MLookBase*>		m_Look;



/// Member functions
public:
	MLookManager();
	virtual ~MLookManager();

	bool Add( const string& strName, MLookBase* pLook);
	MLookBase* Get( const string& strName);
	void Clear();
};



} // namespace mint3

