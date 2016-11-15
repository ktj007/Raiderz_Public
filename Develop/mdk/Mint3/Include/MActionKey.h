#pragma once

#include "MEdit.h"
#include <vector>

using namespace std;

namespace mint3 {

typedef vector<int> ActionKeyForbidVector;
/// ActionKey
class MActionKey : public MEdit{
protected:
	DECLARE_LOOK(MEditLook)	// Edit Look을 그대로 쓴다.
	DECLARE_LOOK_CLIENT()

protected:
	unsigned int		m_nKey;

public:

	static ActionKeyForbidVector m_ForbidKey;

protected:
	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);

public:
	MActionKey(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL );

#define MINT_ACTIONKEY	"ActionKey"
	virtual const char* GetClassName(void){ return MINT_ACTIONKEY; }

	void GetActionKeyName(char* szActionKeyName);
	void GetActionKey(unsigned int* pKey);

	void SetActionKey(unsigned int nKey);

	bool RegisterActionKey(int nActionID);
	bool UnregisterActionKey(void);
	
	static void RegisterForbidKey(int nkey);
	bool IsForbidKey(int nkey);
};

} // namespace mint3