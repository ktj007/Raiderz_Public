#pragma once

#include "MInstanceChanger.h"
#include "ZGlobal.h"

template <class Type>
class LTestSysWrapper : public MInstanceChanger<Type>
{
public:
	LTestSysWrapper() : MInstanceChanger()
	{
		m_pOriginal = gsys.Change(m_pTester);
	}
	~LTestSysWrapper()
	{
		gsys.Change(m_pOriginal);
	}
};

template <class Type>
class LTestMgrWrapper : public MInstanceChanger<Type>
{
public:
	LTestMgrWrapper() : MInstanceChanger()
	{
		m_pOriginal = gmgr.Change(m_pTester);
	}
	~LTestMgrWrapper()
	{
		gmgr.Change(m_pOriginal);
	}
};

//////////////////////////////////////////////////////////////////////////

template <class TypeOriginal, class TypeTest> 
class LTestMgrWrapper2 : public MInstanceChanger2<TypeOriginal, TypeTest>
{
public:
	LTestMgrWrapper2() : MInstanceChanger2()
	{
		m_pOriginal = gmgr.Change(m_pTester);
	}
	~LTestMgrWrapper2()
	{
		gmgr.Change(m_pOriginal);
	}
};

template <class TypeOriginal, class TypeTest> 
class LTestSysWrapper2 : public MInstanceChanger2<TypeOriginal, TypeTest>
{
public:
	LTestSysWrapper2() : MInstanceChanger2()
	{
		m_pOriginal = gsys.Change(m_pTester);
	}
	~LTestSysWrapper2()
	{
		gsys.Change(m_pOriginal);
	}
};
