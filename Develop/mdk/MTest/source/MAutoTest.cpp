#include "StdAfx.h"
#include "MAutoTest.h"

MAutoTest::MAutoTest()
{
}

MAutoTest::~MAutoTest()
{

}

void MAutoTest::Create()
{
	OnCreate();
}

void MAutoTest::Destroy()
{
	OnDestroy();
}

void MAutoTest::Update(float fDelta)
{
	OnUpdate(fDelta);
}