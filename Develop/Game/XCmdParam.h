#pragma once

#include "MCommand.h"
#include "MCommandParameter.h"
#include "CTransData.h"

using namespace minet;

class XCmdParam
{
public:
	virtual bool Parse(MCommand* pCommand) = 0;

	template <typename T_DATA>
	bool GetBlobParameter(MCommand* pCommand, int nParamIndex, T_DATA** outpTransData, int* outnBlobCount)
	{
		MCommandParameter* pParam = pCommand->GetParameter(nParamIndex);
		if(pParam->GetType()!=MPT_BLOB) return false;

		if (outpTransData)
		{
			*outpTransData = (T_DATA*)pParam->GetPointer();
		}

		if (outnBlobCount)
		{
			*outnBlobCount = pParam->GetBlobCount();
		}

		return true;
	}
};

class XCmdParam_IntResult
{
public:
	int m_nResult;
public:
	XCmdParam_IntResult() : m_nResult(0) {}
	virtual bool Parse(MCommand* pCommand)
	{
		if( !pCommand->GetParameter(&m_nResult,		0, MPT_INT) )	return false;

		return true;
	}
};