#pragma once

template <typename Derive>
class FSFsm
{
public:
	typedef void (Derive::*FSState)(wchar_t token);

public:
	FSFsm(): m_state(NULL)
	{
		// do nothing
	}

	virtual ~FSFsm()	
	{
		// do nothing
	}

	void Dispatch(wchar_t token)
	{
		_ASSERT(m_state != NULL);
		(((Derive*)this)->*m_state)(token);
	}

	void Trans(FSState state)
	{
		m_state = state;
	}

private:
	FSState m_state;
};
