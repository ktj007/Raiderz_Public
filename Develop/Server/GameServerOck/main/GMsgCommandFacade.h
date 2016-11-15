#pragma once

#include "CSMsgCommandFacade.h"

class GEntityPlayer;
class GField;
enum CCommandResultTable;

class GMsgCommandFacade: public CSMsgCommandFacade, public MTestMemPool<GMsgCommandFacade>
{
public:
	GMsgCommandFacade();
	virtual ~GMsgCommandFacade();

	bool RouteSystemMsg(MUID uidPlayer, CCommandResultTable nResult);
	bool RouteSystemMsg(GEntityPlayer* pPlayer, CCommandResultTable nResult);		
	
	bool RouteNoticeMsgToAll(wstring strMsg);
	bool RouteNoticeMsgToField(GField* pField, wstring strMsg);

	bool RouteAdviceMsg(MUID uidPlayer, CCommandResultTable nResult);
	bool RouteAdviceMsg(GEntityPlayer* pPlayer, CCommandResultTable nResult);	
	bool RouteNoticeMsgToMe(GEntityPlayer* pPlayer, wstring strMsg);
};
