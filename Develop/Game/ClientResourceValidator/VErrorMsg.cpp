#include "stdafx.h"
#include "VErrorMsg.h"

VErrorMsg::VErrorMsg()
{

}

VErrorMsg::~VErrorMsg()
{
	Clear();
}

void VErrorMsg::Add( ERROR_MSG_TYPE eType, string& strMsg )
{
	stERRORMSG st;
	st.eTYpe = eType;
	st.strErrorMsg = strMsg;

	m_strErrorMsg.push_back(st);
}

void VErrorMsg::Add( VErrorMsg& errorMsg )
{
	for(vector<stERRORMSG>::iterator it = errorMsg.m_strErrorMsg.begin(); it != errorMsg.m_strErrorMsg.end(); ++it)
	{
		Add(it->eTYpe, it->strErrorMsg);
	}
}

void VErrorMsg::Clear()
{
	m_strErrorMsg.clear();
}

void VErrorMsg::AddErrorXml( string& strXmlFileName )
{
	stERRORMSG st;
	st.eTYpe = EMT_FILE;
	st.strErrorMsg = strXmlFileName + " 파일 로딩에 문제가 생겼습니다. 확인바랍니다.";
	m_strErrorMsg.push_back(st);
}

void VErrorMsg::AddErrorFile( string& strFileName )
{
	stERRORMSG st;
	st.eTYpe = EMT_FILE;
	st.strErrorMsg = strFileName + " 파일이 존재하지 않습니다.";
	m_strErrorMsg.push_back(st);
}

bool VErrorMsg::IsError()
{
	return (m_strErrorMsg.size() > 0);
}