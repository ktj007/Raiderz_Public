#include "stdafx.h"
#include "LMemory.h"
#include "LDBTaskHeaderList.h"


LScopeMempoolReleaser::~LScopeMempoolReleaser()
{
	LDBTaskPmangAccountInsert::release();
	LDBTaskAccountInsert::release();
	LDBTaskCharDelete::release();
	LDBTaskCharGetLookList::release();
	LDBTaskCharInsert::release();
	LDBTaskLogin::release();
	LDBTaskPmangLogin::release();
	LDBTaskServerGetStatus::release();
	LAccountDBTaskQuery::release();
	LGameDBTaskQuery::release();
	LLogDBTaskQuery::release();
	LDBTaskGetWorldInfo::release();
}
