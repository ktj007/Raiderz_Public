#include "stdafx.h"
#include "ZMemory.h"
#include "ZDBTaskSQL.h"


ZScopeMempoolReleaser::~ZScopeMempoolReleaser()
{
	LAccountDBTaskQuery::release();
	LGameDBTaskQuery::release();
	LLogDBTaskQuery::release();
}
