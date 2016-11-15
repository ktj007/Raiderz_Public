#ifndef _MINT4GAME_H
#define _MINT4GAME_H

#include "Mint4R3.h"

using namespace mint3;

class Mint4Game : public Mint4R3 {
friend class XMintLoader;
private:
	MWidget*				m_pMainInput;

public:
	Mint4Game(void);
	virtual ~Mint4Game(void);

	virtual MWidget* NewWidget(const char* szClass, const char* szName, MWidget* pParent, MListener* pListener);	

	//MWidget* GetMainInput(void);

	DWORD	m_dwStartTime;
	virtual void BeginDrawWidget(const wchar_t* szClassName);
	virtual void EndDrawWidget(const wchar_t* szClassName);
};







#endif