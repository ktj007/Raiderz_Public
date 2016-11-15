#pragma once

#include "MCommandTable.h"
#include "CCommandTable.h"
#include "CCommandTable_Master.h"
#include "SMinetInitScope.h"

// 커맨드 선언 ////////////////////////////////////////////////////////////////
// 커맨드 번호 20001 ~ 30000 까지 사용

// 로컬 ///////////////////////////////////////////////////////////////////////

// 프로세스가 통신
enum LCommandTable
{

};

void LAddCommandTable();


class LMinetInitScope : public SMinetInitScope
{
public:
	LMinetInitScope() : SMinetInitScope()
	{
		LAddCommandTable();
	}
};
