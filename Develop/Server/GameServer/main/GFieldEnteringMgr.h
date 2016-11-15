#pragma once

#include <queue>

class GFieldEnteringMgr : public MTestMemPool<GFieldEnteringMgr>
{
private:	typedef std::queue<MUID>	GEnteringPlayerQueue;
public:		static const float TIME_UPDATE_SEC;
			static const int LIMIT_COUNT_ENTER_PLAYER_A_UPDATE;
public:
	GFieldEnteringMgr();
	virtual ~GFieldEnteringMgr();

	void			Update(float fDelta);
	virtual void	Add(MUID uidPlayer);

	int				GetQueueSize()	{ return (int)m_qEnteringPlayer.size(); }

protected:
	void			EnterPlayerWhoFieldLoadingComplete(MUID uidPlayer);

private:
	MRegulator				m_rgltUpdate;
	GEnteringPlayerQueue	m_qEnteringPlayer;

};
