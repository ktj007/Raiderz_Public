#pragma once

#include "MInstanceChanger.h"
#include "PGlobal.h"

template <class Type>
class PTestSysWrapper : public MInstanceChanger<Type>
{
public:
	PTestSysWrapper() : MInstanceChanger()
	{
		m_pOriginal = gsys.Change(m_pTester);
	}
	~PTestSysWrapper()
	{
		gsys.Change(m_pOriginal);
	}
};

template <class Type>
class PTestMgrWrapper : public MInstanceChanger<Type>
{
public:
	PTestMgrWrapper() : MInstanceChanger()
	{
		m_pOriginal = gmgr.Change(m_pTester);
	}
	~PTestMgrWrapper()
	{
		gmgr.Change(m_pOriginal);
	}
};

//////////////////////////////////////////////////////////////////////////

template <class TypeOriginal, class TypeTest> 
class PTestMgrWrapper2 : public MInstanceChanger2<TypeOriginal, TypeTest>
{
public:
	PTestMgrWrapper2() : MInstanceChanger2()
	{
		m_pOriginal = gmgr.Change(m_pTester);
	}
	~PTestMgrWrapper2()
	{
		gmgr.Change(m_pOriginal);
	}
};

template <class TypeOriginal, class TypeTest> 
class PTestSysWrapper2 : public MInstanceChanger2<TypeOriginal, TypeTest>
{
public:
	PTestSysWrapper2() : MInstanceChanger2()
	{
		m_pOriginal = gsys.Change(m_pTester);
	}
	~PTestSysWrapper2()
	{
		gsys.Change(m_pOriginal);
	}
};
