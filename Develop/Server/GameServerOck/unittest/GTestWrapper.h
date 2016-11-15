#pragma once

#include "MInstanceChanger.h"


template <class Type>
class GTestSysWrapper : public MInstanceChanger<Type>
{
public:
	GTestSysWrapper() : MInstanceChanger()
	{
		m_pOriginal = gsys.Change(m_pTester);
	}
	~GTestSysWrapper()
	{
		gsys.Change(m_pOriginal);
	}
};

template <class Type>
class GTestMgrWrapper : public MInstanceChanger<Type>
{
public:
	GTestMgrWrapper() : MInstanceChanger()
	{
		m_pOriginal = gmgr.Change(m_pTester);
	}
	~GTestMgrWrapper()
	{
		gmgr.Change(m_pOriginal);
	}
};

//////////////////////////////////////////////////////////////////////////

template <class TypeOriginal, class TypeTest> 
class GTestMgrWrapper2 : public MInstanceChanger2<TypeOriginal, TypeTest>
{
public:
	GTestMgrWrapper2() : MInstanceChanger2()
	{
		m_pOriginal = gmgr.Change(m_pTester);
	}
	~GTestMgrWrapper2()
	{
		gmgr.Change(m_pOriginal);
	}
};

template <class TypeOriginal, class TypeTest> 
class GTestSysWrapper2 : public MInstanceChanger2<TypeOriginal, TypeTest>
{
public:
	GTestSysWrapper2() : MInstanceChanger2()
	{
		m_pOriginal = gsys.Change(m_pTester);
	}
	~GTestSysWrapper2()
	{
		gsys.Change(m_pOriginal);
	}
};
