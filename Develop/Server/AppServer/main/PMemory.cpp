#include "stdafx.h"
#include "PMemory.h"
#include "PDBTaskSQL.h"


PScopeMempoolReleaser::~PScopeMempoolReleaser()
{
	PGameDBTaskQuery::release();
	PLogDBTaskQuery::release();
}