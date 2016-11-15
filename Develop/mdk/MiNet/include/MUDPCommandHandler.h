#pragma once


namespace minet {


class IUDPCommandHandler
{
public:
	virtual void OnDo(int nIP, int nPort, MCommand* pCommand) = 0;
};


template <typename Delegator>
class MUDPCommandHandler: public IUDPCommandHandler
{
private:
	typedef bool (*CommandHandler)(DWORD dwIP, WORD wRawPort, MCommand* pCommand, Delegator* pHandler);
	typedef map<int, CommandHandler> map_handler;

public:
	MUDPCommandHandler(Delegator* pDelegator)		{ m_pDelegator = pDelegator;	}
	virtual ~MUDPCommandHandler()					{}
	
	virtual void OnDo(int nIP, int nPort, MCommand* pCommand)
	{
		if (pCommand == NULL)	return;

		int nID = pCommand->GetID();

		map_handler::iterator it = m_mapCommandHandler.find(nID);

		if (it != m_mapCommandHandler.end())
		{
			CommandHandler handlerFn = it->second;
			handlerFn(nIP, nPort, pCommand, m_pDelegator);
		}
	}

	void AddHandler(int nCommandID, CommandHandler pHandler)
	{
		m_mapCommandHandler.insert(map_handler::value_type(nCommandID, pHandler));
	}

private:
	map_handler m_mapCommandHandler;	
	Delegator* m_pDelegator;
};

}
