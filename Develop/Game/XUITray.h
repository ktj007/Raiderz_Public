
#pragma once

#include "MWidget.h"

using namespace std;
using namespace mint3;



/// class : XUITray
class XUITray : public MWidget
{
/// Member variables
protected:
	string				m_strScriptInstance;	///< Script instance
	DWORD				m_dwUpdateTimer;		///< Update timer
	unsigned int		m_nHotKey;				///< HotKey



/// Member functions
public:
	XUITray( const char* szName, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~XUITray();

	// 위젯의 상태를 구한다
	virtual const char* GetState();
	virtual int GetStateNum();

	/// 스크립트 인스턴스 바인딩
	void BindScriptInstance( const char* szScriptInstance);
	const char* GetScriptInstance() const;

	/// HotKey 설정
	void SetHotKey( unsigned int nHotKey)							{ m_nHotKey = nHotKey;						}
	unsigned int GetHotKey() const									{ return m_nHotKey;							}



/// Event handler
protected:
	virtual bool OnQueryHotKey( unsigned int nHotKey) override;
	virtual void OnHotKey( unsigned int nHotKey) override;
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnDrawBorder( MDrawContext* pDC) override;
	virtual void OnClick( MEvent* pEvent) override;
	virtual void OnRClick( MEvent* pEvent) override;
	virtual MWidget* OnToolTip() override;
	virtual void OnEnter() override;
	virtual void OnLeave() override;



/// Lua script
public:
	void glueBindScriptInstance( const char* szScriptInstance)		{ BindScriptInstance( szScriptInstance);	}
	const char* glueGetScriptInstance()								{ return GetScriptInstance();				}
	void glueSetHotKey( int nHotKey)								{ SetHotKey( nHotKey);						}
	int glueGetHotKey()												{ return GetHotKey();						}



#define MINT_TRAY		"Tray"
	virtual const char* GetClassName() override						{ return MINT_TRAY;							}
};
