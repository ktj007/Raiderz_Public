#ifndef _GTRADE_SYSTEM_H_
#define _GTRADE_SYSTEM_H_


class GTradeRequester;
class GTradeAcceptor;		
class GTradeItemPutUper;
class GTradeItemPutDowner;
class GTradeMoneyPutUper;
class GTradeCanceler;	
class GTradeConfirmer;
class GTradeExchanger;
class GTradeChecker;

class GTradeExchangerForDBTask;

class GTradeSystem : public MTestMemPool<GTradeSystem>
{
public:
	GTradeSystem();
	~GTradeSystem();

	GTradeRequester&		GetRequester();
	GTradeAcceptor&			GetAcceptor();
	GTradeItemPutUper&		GetItemPutUper();
	GTradeItemPutDowner&	GetItemPutDowner();
	GTradeMoneyPutUper&	GetMoneyPutUper();
	GTradeCanceler&			GetCanceler();
	GTradeConfirmer&		GetConfirmer();
	GTradeExchanger&		GetExchanger();	
	GTradeChecker&			GetChecker();

	GTradeExchangerForDBTask&	GetExchangerForDBTask();	

private:
	GTradeRequester*		m_pTradeRequester;
	GTradeAcceptor*			m_pTradeAcceptor;	
	GTradeItemPutUper*		m_pTradeItemPutUper;
	GTradeItemPutDowner*	m_pTradeItemPutDowner;
	GTradeMoneyPutUper*	m_pTradeMoneyPutUper;
	GTradeCanceler*			m_pTradeCanceler;
	GTradeConfirmer*		m_pTradeConfirmer;
	GTradeExchanger*		m_pTradeExchanger;	
	GTradeChecker*			m_pTradeChecker;

	GTradeExchangerForDBTask*	m_pTradeExchangerForDBTask;
};

#endif //_GTRADE_SYSTEM_H_
