#include "stdafx.h"
#include "MCommand.h"
#include "MCommandManager.h"
#include "MDebug.h"
#include <tchar.h>
#include <atlconv.h>
#include <atlexcept.h>


namespace minet {

void MCommand::Reset(void)
{
	m_pCommandDesc = NULL;
	m_Sender.SetZero();
	m_Receiver.SetZero();
	ClearParam();
}

void MCommand::ClearParam(void)
{
	const int nParamCount = GetParameterCount();
	for(int i=0; i<nParamCount; ++i)
	{
		delete m_Params[i];
		m_Params[i] = NULL;
	}
	m_Params.clear();
}

MCommand::MCommand(void)
{
	Reset();
}

MCommand::MCommand(const MCommandDesc* pCommandDesc, MUID Receiver, MUID Sender)
{
	Reset();
	SetID(pCommandDesc);
	m_Receiver = Receiver;
	m_Sender = Sender;
}

MCommand::MCommand(int nDescID, MUID Sender, MUID Receiver)
{
	Reset();
	SetID(nDescID);
	m_Sender = Sender;
	m_Receiver = Receiver;
}


MCommand::~MCommand(void)
{
	ClearParam();
}

void MCommand::SetID(const MCommandDesc* pCommandDesc)
{
	m_pCommandDesc = pCommandDesc;
	_ASSERT(m_pCommandDesc!=NULL);
	m_Params.reserve(pCommandDesc->GetParameterDescCount());	// 공간 확보
}


void MCommand::SetID(int nID)
{
	m_pCommandDesc = MGetCommandDescMap()->GetCommandDescByID(nID);
	_ASSERT(m_pCommandDesc!=NULL);
	m_Params.reserve(m_pCommandDesc->GetParameterDescCount());	// 공간 확보
}


bool MCommand::AddParameter(MCommandParameter* pParam)
{
	if (pParam == NULL)	return false;

	_ASSERT(m_Params.capacity()==m_pCommandDesc->GetParameterDescCount());	// 미리 공간이 확보되어 있어야 한다.

	size_t nCount = m_Params.size();
	size_t nParamDescCount = m_pCommandDesc->GetParameterDescCount();

	_ASSERT(nCount<nParamDescCount);				// Debug Mode 에서는 Assert로 엄격하게 체크한다.
	if(nCount>=nParamDescCount) return false;

	MCommandParameterDesc* pParamDesc = m_pCommandDesc->GetParameterDesc(nCount);
	_ASSERT(pParam->GetType()==pParamDesc->GetType());	// 명시된 파라미터여야 한다.
	if(pParam->GetType()!=pParamDesc->GetType()) return false;

	m_Params.push_back(pParam);

	return true;
}

int MCommand::GetParameterCount(void) const
{
	return (int)m_Params.size();
}

MCommandParameter* MCommand::GetParameter(int i) const
{
	if(i<0 || i>=(int)m_Params.size()) return NULL;

	return m_Params[i];
}

bool MCommand::GetParameter(void* pValue, int i, MCommandParameterType t, int nBufferSize) const
{
	if( 0 == pValue ) return false;

	MCommandParameter* pParam = GetParameter(i);
	if(pParam==NULL) return false;

	if(pParam->GetType()!=t) return false;

#ifdef _DEBUG
	// 스트링과 blob 은 버퍼오버플로우 체크를 할 필요가 있다.
	if((pParam->GetType()==MPT_STR || pParam->GetType()==MPT_WSTR) && nBufferSize < 0 ) {
		// string 파라미터는 꼭 buffer size를 적어주세요
		_ASSERT(FALSE);
	}
#endif

	if(pParam->GetType()==MPT_STR && nBufferSize>=0 ) {
		char *szParamString = *(char**)pParam->GetPointer();
		if( 0 == szParamString )
		{
//			assert( !"NULL 포인터 스트링" );
			strcpy_s( (char*)pValue, nBufferSize, "\0" );		
			return true;
		}

		int nLength = (int)strlen(szParamString);
		if(nLength>=nBufferSize-1) {
//			minet_logd("buffer overflow command id %d, sender uid(%I64u)\n", GetID(), m_Sender);
			strncpy_s((char*)pValue, nBufferSize, szParamString,nBufferSize-2);
			((char*)pValue)[nBufferSize-1]=0;
		}else{
			pParam->GetValue(pValue);
		}
	}
	else if (pParam->GetType()==MPT_WSTR && nBufferSize >=0 )
	{
		wchar_t *szParamString = *(wchar_t**)pParam->GetPointer();
		if( 0 == szParamString )
		{
			wcscpy_s( (wchar_t*)pValue, nBufferSize / sizeof(wchar_t), L"\0" );
			return true;
		}

		int nLength = (int)wcslen(szParamString);
		if(nLength>=nBufferSize-1) {
			wcsncpy_s((wchar_t*)pValue, nBufferSize / sizeof(wchar_t), szParamString, nBufferSize / sizeof(wchar_t) - 2);
			((wchar_t*)pValue)[nBufferSize / sizeof(wchar_t) - 1] = L'\0';
		}else{
			pParam->GetValue(pValue);
		}		
	}
	else
	{
		pParam->GetValue(pValue);
	}

	return true;
}

bool MCommand::GetParameter(string& str, int i, MCommandParameterType t) const
{
	if( 0 == str.c_str() ) return false;

	MCommandParameter* pParam = GetParameter(i);
	if(pParam==NULL) return false;

	if(pParam->GetType()!=t) return false;

	if(pParam->GetType()==MPT_STR) 
	{
		char *szParamString = *(char**)pParam->GetPointer();
		if( 0 == szParamString )
		{
			//			assert( !"NULL 포인터 스트링" );
			_ASSERT( 0 && "NULL 포인터 스트링" );
			str.assign("\0");
			return false;
		}

		int nLength = (int)strlen(szParamString);
		if(nLength >= 0) 
		{
			//			minet_logd("buffer overflow command id %d, sender uid(%I64u)\n", GetID(), m_Sender);
			str = szParamString;
		}
		else
		{
			_ASSERT(!"잘못된 스트링");
			str.assign("\0");
			return false;
		}
	}
	else
		_ASSERT(!"타입 체크 요망");

	return true;
}

bool MCommand::GetParameter(wstring& str, int i, MCommandParameterType t) const
{
	if( 0 == str.c_str() ) return false;

	MCommandParameter* pParam = GetParameter(i);
	if(pParam==NULL) return false;

	if(pParam->GetType()!=t) return false;

	if(pParam->GetType()==MPT_WSTR) 
	{
		wchar_t *szParamString = *(wchar_t**)pParam->GetPointer();
		if( 0 == szParamString )
		{
			_ASSERT( 0 && "NULL 포인터 스트링" );
			str.assign(L"\0");
			return false;
		}

		int nLength = (int)wcslen(szParamString);
		if(nLength >= 0) 
		{
			str = szParamString;
		}
		else
		{
			_ASSERT(!"잘못된 스트링");
			str.assign(L"\0");
			return false;
		}
	}
	else
		_ASSERT(!"타입 체크 요망");

	return true;
}

MCommand* MCommand::Clone(void) const
{
	if(m_pCommandDesc==NULL) return NULL;
	MCommand* pClone = new MCommand(m_pCommandDesc, m_Receiver, m_Sender);
	if( 0 == pClone ) return NULL;
	const int nParamCount = GetParameterCount();
	for(int i=0; i<nParamCount; ++i){
		MCommandParameter* pParameter = GetParameter(i);
		if(pClone->AddParameter(pParameter->Clone())==false)
		{
			delete pClone;
			pClone = NULL;
			return NULL;
		}
	}	

	return pClone;
}

bool MCommand::CheckRule(void)
{
	_ASSERT(m_pCommandDesc!=NULL);
	if(m_pCommandDesc==NULL) return false;

	int nCount = GetParameterCount();
	if(nCount!=m_pCommandDesc->GetParameterDescCount()) return false;

	for(int i=0; i<nCount; ++i){
		MCommandParameter* pParam = GetParameter(i);
		MCommandParameterDesc* pParamDesc = m_pCommandDesc->GetParameterDesc(i);
		if(pParam->GetType()!=pParamDesc->GetType()) return false;

		// 제약조건 체크
		if (pParamDesc->HasConditions())
		{
			for (int j = 0; j < pParamDesc->GetConditionCount(); j++)
			{
				MCommandParamCondition* pCondition = pParamDesc->GetCondition(j);
				if (!pCondition->Check(pParam)) 
				{
					minet_logd("Cmd Param Condition Check Error(CMID = %d)\n", m_pCommandDesc->GetID());
					return false;
				}
			}
		}
	}

	return true;
}

int MCommand::GetData(char* pData, uint16 nSize)
{
	if (NULL == m_pCommandDesc)	return 0;
	if (NULL == pData)			return 0;

	uint16 nDataIndex = sizeof(uint16);

	// Command id
	uint16 nCommandID = m_pCommandDesc->GetID();
	memcpy(pData + nDataIndex, &(nCommandID), sizeof(uint16));
	nDataIndex += sizeof(uint16);

	int nParamCount = GetParameterCount();
	for (int i = 0; i < nParamCount; ++i)
	{
		MCommandParameter* pParam = GetParameter(i);
		if (NULL == pParam)
		{
			minet_logd("Network Error! MCommand::GetData(), Cmd Param is NULL(CMD ID = %d)\n", m_pCommandDesc->GetID());
			return 0;
		}

		nDataIndex += pParam->GetData((pData + nDataIndex), (nSize - nDataIndex));
	}

	if (nSize != nDataIndex)
	{
		minet_logd("Network Error! MCommand::GetData(), Invalid data size(CMD ID = %d)\n", m_pCommandDesc->GetID());
		return 0;
	}

	// Write Total Size
	memcpy(pData, &nDataIndex, sizeof(uint16));

	return nDataIndex;
}

bool MCommand::SetData(char* pData, uint16 nDataLen)
{
	if (NULL == pData) return false;

	Reset();

	uint16 nDataIndex = 0;

	// Get Total Packet Size
	uint16 nTotalPacketSize = 0;
	memcpy(&nTotalPacketSize, pData, sizeof(uint16));
	nDataIndex += sizeof(uint16);

	if ((nDataLen != USHRT_MAX) && (nDataLen != nTotalPacketSize))
	{
		minet_logd("Network Error! MCommand::SetData(), Invalid Data Size!\n");
		return false;
	}

	// Command
	uint16 nCommandID = 0;
	memcpy(&nCommandID, pData + nDataIndex, sizeof(uint16));
	nDataIndex += sizeof(uint16);

	MCommandDesc* pDesc = MGetCommandDescMap()->GetCommandDescByID(nCommandID);
	if (pDesc == NULL)
	{
		minet_logd("Network Error! MCommand::SetData(), MCommandDesc is not exist!(CmdID = %d)\n", nCommandID);
		_ASSERT(0);
		return false;
	}
	SetID(pDesc);

	// Parameters
	int nParamCount = (int)pDesc->GetParameterDescCount();
	for (int i = 0; i < nParamCount; ++i)
	{
		MCommandParameterType nParamType = pDesc->GetParameterType(i);
		
		MCommandParameter* pParam = NewParameter(nParamType);
		if (pParam == NULL)
		{
			minet_logd("Network Error! MCommand::SetData(), Unknown Parameter Type!(CmdID = %d)(Param Type = %d)\n", nCommandID, (int)nParamType);
			return false;
		}
		
		uint16 nRemainSize = nTotalPacketSize - nDataIndex;
		uint16 nCopyedDataSize = (uint16)pParam->SetData(pData + nDataIndex, nRemainSize);
		if (nCopyedDataSize <= 0)
		{
			minet_logd("Network Error! MCommand::SetData(), Failed MCommandParameter::SetData()!(CmdID = %d)\n", nCommandID);
			return false;
		}
		
		nDataIndex += nCopyedDataSize;
		m_Params.push_back(pParam);
		
		if (nDataIndex > nTotalPacketSize)
		{
			minet_logd("Network Error! MCommand::SetData(), DataIndex is over TotalPacketSize!(CmdID = %d)\n", nCommandID);
			return false;
		}
	}
	
	if (nDataIndex != nTotalPacketSize)
	{
		minet_logd("Network Error! MCommand::SetData(), DataIndex is not equal TotalPacketSize!(CmdID = %d)\n", nCommandID);
		return false;
	}
	
	return true;
}

int MCommand::GetSize()
{
	if(m_pCommandDesc==NULL) return 0;

	int nSize = 0;

	// size + command id
	nSize = sizeof(unsigned short int) + sizeof(unsigned short int);

	int nParamCount = (int)m_Params.size();

	// Parameter Types
	//nSize += (sizeof(BYTE) * nParamCount);

	// Parameters
	for(int i=0; i<nParamCount; i++)
	{
		MCommandParameter* pParam = GetParameter(i);
		nSize += (pParam->GetSize());
	}

	return nSize;
}

void MCommand::AddReceiver(const MUID& uidReceiver)
{
	if (IsNoReceiver())
		SetReceiverUID(uidReceiver);
	else
		AddAdditiveReceiver(uidReceiver);
}

void MCommand::AddReceiver(const vector<MUID>& vecReceivers)
{
	for each (const MUID& uidReceiver in vecReceivers)
	{
		AddReceiver(uidReceiver);
	}
}

void MCommand::AddAdditiveReceiver(const MUID& uidReceiver)
{
	m_AdditiveReceiverList.push_back(uidReceiver);
}

int MCommand::GetReceiverCount()
{
	int ret = 0;
	if (!IsNoReceiver()) ret++;
	ret += (int)m_AdditiveReceiverList.size();
	return ret;
}

MCommandResult MCommand::Run()
{
	if (NULL == m_pCommandDesc) return CR_FALSE;
	if (NULL == m_pCommandDesc->m_pFunc) return CR_FALSE;

	return m_pCommandDesc->m_pFunc(this, m_pCommandDesc->m_pHandler);
}


} // namespace minet