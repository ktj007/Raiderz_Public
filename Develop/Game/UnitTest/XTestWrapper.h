#pragma once

#include "MInstanceChanger.h"


template <class Type>
class XTestGlobalWrapper : public MInstanceChanger<Type>
{
public:
	XTestGlobalWrapper() : MInstanceChanger()
	{
		m_pOriginal = global.Change(m_pTester);
	}
	~XTestGlobalWrapper()
	{
		global.Change(m_pOriginal);
	}
};

template <class Type>
class XTestInfoWrapper : public MInstanceChanger<Type>
{
public:
	XTestInfoWrapper() : MInstanceChanger()
	{
		m_pOriginal = info.Change(m_pTester);
	}
	~XTestInfoWrapper()
	{
		info.Change(m_pOriginal);
	}
};

//////////////////////////////////////////////////////////////////////////

template <class TypeOriginal, class TypeTest> 
class XTestGlobalWrapper2 : public MInstanceChanger2<TypeOriginal, TypeTest>
{
public:
	XTestGlobalWrapper2() : MInstanceChanger2()
	{
		m_pOriginal = global.Change(m_pTester);
	}
	~XTestGlobalWrapper2()
	{
		global.Change(m_pOriginal);
	}
};

template <class TypeOriginal, class TypeTest> 
class XTestInfoWrapper2 : public MInstanceChanger2<TypeOriginal, TypeTest>
{
public:
	XTestInfoWrapper2() : MInstanceChanger2()
	{
		m_pOriginal = info.Change(m_pTester);
	}
	~XTestInfoWrapper2()
	{
		info.Change(m_pOriginal);
	}
};
