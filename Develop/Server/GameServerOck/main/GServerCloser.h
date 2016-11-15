#pragma once

class GServerCloser : public MTestMemPool<GServerCloser>
{
public:
	enum CloseState
	{
		SLEEP,
		BEGIN,
		WAIT,
		END
	};

public:
	GServerCloser();

	void CloseServer(void);
	bool Update(void);

	bool IsRunServerCloser(void);

private:
	void LogoutAllPlayer(void) const;
	bool IsComplateLogout(void) const;

private:
	CloseState	m_state;
};
