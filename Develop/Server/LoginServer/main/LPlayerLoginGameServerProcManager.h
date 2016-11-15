#pragma once

#include <queue>

class LPlayerLoginGameServerProcManager
{
public:
	static const float TIME_UPDATE_SEC;
	static const int LIMIT_COUNT_PLAYER_LOGIN_GAMESERVER_A_UPDATE;
	struct SEL_MY_CHAR_NODE
	{
		MUID uidPlayer;
		int nCharIndex;

		SEL_MY_CHAR_NODE() : uidPlayer(MUID::ZERO), nCharIndex(-1) {}
		SEL_MY_CHAR_NODE(MUID player, int char_index) : uidPlayer(player), nCharIndex(char_index) {}
	};

private:	typedef std::queue<SEL_MY_CHAR_NODE>	GSelMyCharQueue;
public:
	LPlayerLoginGameServerProcManager();
	~LPlayerLoginGameServerProcManager();

	void			Clear();

	void			Update(float fDelta);
	virtual void	Add(MUID uidPlayer, int nCharIndex);

	int				GetQueueSize()	{ return (int)m_qSelMyChar.size(); }

private:
	MRegulator		m_rgltUpdate;
	GSelMyCharQueue	m_qSelMyChar;

	bool			LoginGameServer(MUID uidPlayer, int nCharIndex);
	void			PostEnterWorldMsg(MUID uidPlayer, int nResult);
};
