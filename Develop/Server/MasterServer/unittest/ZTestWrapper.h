#pragma once

#include "MInstanceChanger.h"
#include "ZGlobal.h"

template <class Type>
class ZTestSysWrapper : public MInstanceChanger<Type>
{
public:
	ZTestSysWrapper() : MInstanceChanger()
	{
		m_pOriginal = gsys.Change(m_pTester);
	}
	~ZTestSysWrapper()
	{
		gsys.Change(m_pOriginal);
	}
};

template <class Type>
class ZTestMgrWrapper : public MInstanceChanger<Type>
{
public:
	ZTestMgrWrapper() : MInstanceChanger()
	{
		m_pOriginal = gmgr.Change(m_pTester);
	}
	~ZTestMgrWrapper()
	{
		gmgr.Change(m_pOriginal);
	}
};

//////////////////////////////////////////////////////////////////////////

template <class TypeOriginal, class TypeTest> 
class ZTestMgrWrapper2 : public MInstanceChanger2<TypeOriginal, TypeTest>
{
public:
	ZTestMgrWrapper2() : MInstanceChanger2()
	{
		m_pOriginal = gmgr.Change(m_pTester);
	}
	~ZTestMgrWrapper2()
	{
		gmgr.Change(m_pOriginal);
	}
};

template <class TypeOriginal, class TypeTest> 
class ZTestSysWrapper2 : public MInstanceChanger2<TypeOriginal, TypeTest>
{
public:
	ZTestSysWrapper2() : MInstanceChanger2()
	{
		m_pOriginal = gsys.Change(m_pTester);
	}
	~ZTestSysWrapper2()
	{
		gsys.Change(m_pOriginal);
	}
};
