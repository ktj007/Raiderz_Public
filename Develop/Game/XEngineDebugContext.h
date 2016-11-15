#pragma once

class XEngineDebugContext
{
public:
	friend class  XEngineDebugger;

	// Debugging Context Type
	enum E_DEBUG_CONTEXT_TYPE
	{
		DCT_MY_CHARACTER_WORLD = 0,
		DCT_FREE_WORLD
	};

	XEngineDebugContext(E_DEBUG_CONTEXT_TYPE _eType)
		: m_bContextUsable(false)
		, m_eDebugContextType(_eType)
	{
	}
	virtual ~XEngineDebugContext(){}


	//////////////////////////////////////////////////////////////////////////
	//
	// XEngineDebugContext Interface
	//
	//////////////////////////////////////////////////////////////////////////

	virtual void			OnUpdate(float fDelta) = 0;
	virtual void			OnCmd( const std::vector< std::wstring >& _refConsoleCmd ) = 0;


protected:
	XEngineDebugger*		m_pEngineDebugger;
	bool					m_bContextUsable;

private:
	E_DEBUG_CONTEXT_TYPE	m_eDebugContextType;
	void Update(float fDelta)
	{
		if (m_bContextUsable)
		{
			OnUpdate(fDelta);
		}
	}
};
