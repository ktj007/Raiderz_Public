#pragma once

#include "ZBaseApplication.h"
#include "MSingleton.h"
#include "ZServerInfo.h"
#include "ZGlobal.h"

class ZCmdHandler_App;

/// 콘솔 어플리케이션
class ZConsole : public ZBaseApplication, public MSingleton<ZConsole>
{
private:
	ZCmdHandler_App*	m_pAppCmdHandler;

	MRegulator			m_RegPrintServerTick;

protected:
	virtual	bool	OnCreate() override;

	virtual bool	OnUpdate(float fDelta) override;
	virtual void	OnDestroy() override;

	void UpdatePrintServerTick(float fDelta);
public:
					ZConsole();
	virtual			~ZConsole();

	static void		OutputLogProc(const char* szText, MLogLevel nLevel);
	void			OutputLog(const wchar_t* szText, MLogLevel nLevel);
};


