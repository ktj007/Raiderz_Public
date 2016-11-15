#pragma once

template <class Type> 
class MInstanceChanger
{
protected:
	Type*		m_pOriginal;
	Type*		m_pTester;
public:
	MInstanceChanger() : m_pOriginal(NULL)
	{
		m_pTester = new Type();
	}
	virtual ~MInstanceChanger()
	{
		SAFE_DELETE(m_pTester);
	}
	Type* Get() { return m_pTester; }
};


template <class TypeOriginal, class TypeDerived> class MInstanceChanger2
{
protected:
	TypeOriginal*	m_pOriginal;
	TypeDerived*		m_pTester;
public:
	MInstanceChanger2() : m_pOriginal(NULL)
	{
		m_pTester = new TypeDerived();
	}
	virtual ~MInstanceChanger2()
	{
		SAFE_DELETE(m_pTester);
	}
	TypeDerived* Get() { return m_pTester; }
};


