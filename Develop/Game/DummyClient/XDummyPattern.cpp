#include "stdafx.h"
#include "XBirdClient.h"
#include "XDummyPattern.h"
#include "MXml.h"

#define DUMMYCLIENT_XML_TAG_ONCOMMAND				"OnCommand"
#define DUMMYCLIENT_XML_TAG_POST					"Post"
#define DUMMYCLIENT_XML_TAG_PARAM					"Param"
#define DUMMYCLIENT_XML_TAG_IF						"IF"
#define DUMMYCLIENT_XML_TAG_THEN					"THEN"
#define DUMMYCLIENT_XML_TAG_ELSE					"ELSE"
#define DUMMYCLIENT_XML_TAG_DATA					"Data"
#define DUMMYCLIENT_XML_TAG_END						"End"

#define DUMMYCLIENT_XML_ATTR_NAME					"name"
#define DUMMYCLIENT_XML_ATTR_ID						"ID"
#define DUMMYCLIENT_XML_ATTR_ID_BASE				"BaseID"
#define DUMMYCLIENT_XML_ATTR_COMPARE				"Compare"
#define DUMMYCLIENT_XML_ATTR_PARAMINDEX_1			"ParamIndex"
#define DUMMYCLIENT_XML_ATTR_VALUE_2				"Value"
#define DUMMYCLIENT_XML_ATTR_TYPE					"Type"

#define DUMMYCLIENT_ID_BASE							"DC_"

#define XDUMMYPOSTCMD0(_CLIENT, _ID)									{ MCommand* pC=XNewCmd(_ID); XBirdPostCommand(_CLIENT, pC); }
#define XDUMMYPOSTCMD1(_CLIENT, _ID, _P0)								{ MCommand* pC=XNewCmd(_ID); pC->AddParameter(_P0); XBirdPostCommand(_CLIENT, pC); }
#define XDUMMYPOSTCMD2(_CLIENT, _ID, _P0, _P1)							{ MCommand* pC=XNewCmd(_ID); pC->AddParameter(_P0); pC->AddParameter(_P1); XBirdPostCommand(_CLIENT, pC); }
#define XDUMMYPOSTCMD3(_CLIENT, _ID, _P0, _P1, _P2)						{ MCommand* pC=XNewCmd(_ID); pC->AddParameter(_P0); pC->AddParameter(_P1); pC->AddParameter(_P2); XBirdPostCommand(_CLIENT, pC); }
#define XDUMMYPOSTCMD4(_CLIENT, _ID, _P0, _P1, _P2, _P3)				{ MCommand* pC=XNewCmd(_ID); pC->AddParameter(_P0); pC->AddParameter(_P1); pC->AddParameter(_P2); pC->AddParameter(_P3); XBirdPostCommand(_CLIENT, pC); }
#define XDUMMYPOSTCMD5(_CLIENT, _ID, _P0, _P1, _P2, _P3, _P4)			{ MCommand* pC=XNewCmd(_ID); pC->AddParameter(_P0); pC->AddParameter(_P1); pC->AddParameter(_P2); pC->AddParameter(_P3); pC->AddParameter(_P4); XBirdPostCommand(_CLIENT, pC); }
#define XDUMMYPOSTCMD6(_CLIENT, _ID, _P0, _P1, _P2, _P3, _P4, _P5)		{ MCommand* pC=XNewCmd(_ID); pC->AddParameter(_P0); pC->AddParameter(_P1); pC->AddParameter(_P2); pC->AddParameter(_P3); pC->AddParameter(_P4); pC->AddParameter(_P5); XBirdPostCommand(_CLIENT, pC); }
#define XDUMMYPOSTCMD7(_CLIENT, _ID, _P0, _P1, _P2, _P3, _P4, _P5, _P6)	{ MCommand* pC=XNewCmd(_ID); pC->AddParameter(_P0); pC->AddParameter(_P1); pC->AddParameter(_P2); pC->AddParameter(_P3); pC->AddParameter(_P4); pC->AddParameter(_P5); pC->AddParameter(_P6); XBirdPostCommand(_CLIENT, pC); }

//////////////////////////////////////////////////////////////////////////
bool IsSameTag(MXmlElement* pElement, const char* szTagName)
{
	if (!_stricmp(pElement->Value(), szTagName)) return true;
	return false;
}

bool ParseOnCommandChild(MXmlElement* pElement, vector<XDummy_Cmd *>* vecDummyNext)
{
	for(MXmlElement* pOnCommand = pElement->FirstChildElement(DUMMYCLIENT_XML_TAG_ONCOMMAND); pOnCommand != NULL; pOnCommand = pOnCommand->NextSiblingElement())
	{
		if(IsSameTag(pOnCommand, DUMMYCLIENT_XML_TAG_ONCOMMAND))
		{
			XDummy_OnCommand * pDummy_Command = new XDummy_OnCommand;

			if(pDummy_Command->ParseOnCommand(pOnCommand))
			{
				vecDummyNext->push_back(pDummy_Command);
			}
			else
			{
				SAFE_DELETE(pDummy_Command);
			}
		}
	}

	return true;
}

bool ParsePostChild(MXmlElement* pElement, vector<XDummy_Cmd *>* vecDummyNext)
{
	for(MXmlElement* pPost = pElement->FirstChildElement(DUMMYCLIENT_XML_TAG_POST); pPost != NULL; pPost = pPost->NextSiblingElement())
	{
		if(IsSameTag(pPost, DUMMYCLIENT_XML_TAG_POST))
		{
			XDummy_Post * pDummy_Post = new XDummy_Post;

			if(pDummy_Post->ParsePost(pPost))
			{
				vecDummyNext->push_back(pDummy_Post);
			}
			else
			{
				SAFE_DELETE(pDummy_Post);
			}
		}
	}

	return true;
}

int GetParameterTypeSize(minet::MCommandParameterType nType)
{
	if(nType == MPT_INT)			return sizeof(int);
	if(nType == MPT_UINT)			return sizeof(unsigned long int);
	if(nType == MPT_FLOAT)			return sizeof(float);
	if(nType == MPT_DOUBLE)			return sizeof(double);
	if(nType == MPT_CHAR)			return sizeof(char);
	if(nType == MPT_UCHAR)			return sizeof(unsigned char);
	if(nType == MPT_SHORT)			return sizeof(short);
	if(nType == MPT_USHORT)			return sizeof(unsigned short);
	if(nType == MPT_INT64)			return sizeof(int64);
	if(nType == MPT_UINT64)			return sizeof(uint64);
	if(nType == MPT_BOOL)			return sizeof(bool);
	if(nType == MPT_STR)			return sizeof(char);
	if(nType == MPT_VECTOR)			return sizeof(vec3);
	if(nType == MPT_SVEC)			return sizeof(MSVector);
	if(nType == MPT_SVEC2)			return sizeof(MSVector2);
	if(nType == MPT_UID)			return sizeof(MUID);
	if(nType == MPT_ID)				return sizeof(MID);
	//if(nType == MPT_BLOB)			return sizeof(int);

	return 0;
}

MCommandParameter * GetParameterTransform(XDummy_ParamValue * pParamValue)
{
	switch (pParamValue->m_nType)
	{
	case MPT_INT:
		{
			int n;
			sscanf(pParamValue->m_strValue.c_str(), "%d", &n);
			return new MCommandParameterInt(n);
		}
		break;
	case MPT_UINT:
		{
			unsigned long int unsingn;
			sscanf(pParamValue->m_strValue.c_str(), "%d", &unsingn);
			return new MCommandParameterUInt(unsingn);
		}
		break;
	case MPT_FLOAT:
		{
			float f;
			sscanf(pParamValue->m_strValue.c_str(), "%f", &f);
			return new MCommandParameterFloat(f);
		}
		break;
	case MPT_DOUBLE:
		{
			double db;
			sscanf(pParamValue->m_strValue.c_str(), "%d", &db);
			return new MCommandParameterDouble(db);
		}
		break;
	case MPT_CHAR:
		{
			char ch;
			sscanf(pParamValue->m_strValue.c_str(), "%c", &ch);
			return new MCommandParameterChar(ch);
		}
		break;
	case MPT_UCHAR:
		{
			unsigned char uch;
			sscanf(pParamValue->m_strValue.c_str(), "%c", &uch);
			return new MCommandParameterUChar(uch);
		}
		break;
	case MPT_SHORT:
		{
			short sh;
			sscanf(pParamValue->m_strValue.c_str(), "%d", &sh);
			return new MCommandParameterShort(sh);
		}
		break;
	case MPT_USHORT:
		{
			unsigned short ush;
			sscanf(pParamValue->m_strValue.c_str(), "%d", &ush);
			return new MCommandParameterUShort(ush);
		}
		break;
	case MPT_INT64:
		{
			int64 n64;
			sscanf(pParamValue->m_strValue.c_str(), "%d", &n64);
			return new MCommandParameterInt64(n64);
		}
		break;
	case MPT_UINT64:
		{
			uint64 un64;
			sscanf(pParamValue->m_strValue.c_str(), "%d", &un64);
			return new MCommandParameterUInt64(un64);
		}
		break;
	case MPT_BOOL:
		{
			bool b;
			sscanf(pParamValue->m_strValue.c_str(), "%d", &b);
			return new MCommandParameterBool(b);
		}
		break;
	case MPT_STR:
		{
			return new MCommandParameterString(pParamValue->m_strValue.c_str());
		}
		break;
	case MPT_VECTOR:
		{
			vec3 v;
			sscanf(pParamValue->m_strValue.c_str(), "%f %f %f", &v.x, &v.y, &v.z);
			return new MCommandParameterVector(v);
		}
		break;
	case MPT_SVEC:
		{
			vec3 v;
			sscanf(pParamValue->m_strValue.c_str(), "%f %f %f", &v.x, &v.y, &v.z);
			return new MCommandParameterShortVector(v);
		}
		break;
	case MPT_SVEC2:
		{
			vec3 v;
			sscanf(pParamValue->m_strValue.c_str(), "%f %f", &v.x, &v.y);
			return new MCommandParameterShortVector2(v);
		}
		break;
	case MPT_UID:
		{
			MUID uid;
			sscanf(pParamValue->m_strValue.c_str(), "%I64u", &uid);
			return new MCommandParameterUID(uid);
		}
		break;
	case MPT_ID:
		{
			MID id;
			sscanf(pParamValue->m_strValue.c_str(), "%d", &id);
			return new MCommandParameterID(id);
		}
		break;
	case MPT_BLOB:
		{
			if(pParamValue->m_nBlobDataSize > 0)
			{
				char * pData = new char[pParamValue->m_nBlobDataSize];
				vector<XDummy_ParamValue>::iterator itBlob = pParamValue->m_vecBlob.begin();
				while(itBlob != pParamValue->m_vecBlob.end())
				{
					XDummy_ParamValue tempParam;
					tempParam.m_nType		= (*itBlob).m_nType;
					tempParam.m_strValue	= (*itBlob).m_strValue;
					MCommandParameter * pParamData = GetParameterTransform(&tempParam);

					void * p = NULL;
					pParamData->GetValue(p);
					memcpy(pData, p, pParamData->GetSize());
					pData += pParamData->GetSize();

					SAFE_DELETE(pParamData);
				}

				MCommandParameterBlob * pMCommandParamBlob = new MCommandParameterBlob(pData, pParamValue->m_nBlobDataSize, pParamValue->m_vecBlob.size());

				SAFE_DELETE(pData);
				return pMCommandParamBlob;
			}	
		}
		break;
	}

	return NULL;
}

minet::MCommandParameterType GetParamType( string strType )
{
	if(strType == "int")			return MPT_INT;
	if(strType == "uint")			return MPT_UINT;
	if(strType == "flaot")			return MPT_FLOAT;
	if(strType == "double")			return MPT_DOUBLE;
	if(strType == "char")			return MPT_CHAR;
	if(strType == "uchar")			return MPT_UCHAR;
	if(strType == "short")			return MPT_SHORT;
	if(strType == "ushort")			return MPT_USHORT;
	if(strType == "int64")			return MPT_INT64;
	if(strType == "uint64")			return MPT_UINT64;
	if(strType == "bool")			return MPT_BOOL;
	if(strType == "string")			return MPT_STR;
	if(strType == "vector")			return MPT_VECTOR;
	if(strType == "svec")			return MPT_SVEC;
	if(strType == "svec2")			return MPT_SVEC2;
	if(strType == "uid")			return MPT_UID;
	if(strType == "id")				return MPT_ID;
	if(strType == "blob")			return MPT_BLOB;

	return MPT_END;
}

template<class tType> 
bool IfCompare( tType& first, tType& second, XDummy_If::DUMMY_IF_COMPARE nCompare)
{
	switch(nCompare)
	{
	case XDummy_If::EQUAL:
		{
			if(first == second)
			{
				return true;
			}

			return false;
		}
	case XDummy_If::LARGE:
		{
			if(first > second)
			{
				return true;
			}

			return false;
		}
	case XDummy_If::SMALL:
		{
			if(first < second)
			{
				return true;
			}

			return false;
		}
	case XDummy_If::LARGE_EQUAL:
		{
			if(first >= second)
			{
				return true;
			}

			return false;
		}
	case XDummy_If::SMALL_EQUAL:
		{
			if(first <= second)
			{
				return true;
			}

			return false;
		}
	}

	return false;
}

bool IfCompareStart(XDummy_If * pIf, MCommand * pCmd)
{
	switch (pIf->m_SecondValue.m_nType)
	{
	case MPT_INT:
		{
			int n, n2;
			if( !pCmd->GetParameter(&n,	pIf->m_nFirstParamIndex, MPT_INT) )	return false;
			sscanf(pIf->m_SecondValue.m_strValue.c_str(), "%d", &n2);
			return IfCompare(n, n2, (XDummy_If::DUMMY_IF_COMPARE)(pIf->m_nCompare));
		}
		break;
	case MPT_UINT:
		{
			unsigned long int unsingn, unsingn2;
			if( !pCmd->GetParameter(&unsingn,	pIf->m_nFirstParamIndex, MPT_UINT) )	return false;
			sscanf(pIf->m_SecondValue.m_strValue.c_str(), "%d", &unsingn2);
			return IfCompare(unsingn, unsingn2, (XDummy_If::DUMMY_IF_COMPARE)(pIf->m_nCompare));
	}
		break;
	case MPT_FLOAT:
		{
			float f, f2;
			if( !pCmd->GetParameter(&f,	pIf->m_nFirstParamIndex, MPT_FLOAT) )	return false;
			sscanf(pIf->m_SecondValue.m_strValue.c_str(), "%f", &f2);
			return IfCompare(f, f2, (XDummy_If::DUMMY_IF_COMPARE)(pIf->m_nCompare));
		}
		break;
	case MPT_DOUBLE:
		{
			double db, db2;
			if( !pCmd->GetParameter(&db,	pIf->m_nFirstParamIndex, MPT_DOUBLE) )	return false;
			sscanf(pIf->m_SecondValue.m_strValue.c_str(), "%d", &db2);
			return IfCompare(db, db2, (XDummy_If::DUMMY_IF_COMPARE)(pIf->m_nCompare));
}
		break;
	case MPT_CHAR:
		{
			char ch, ch2;
			if( !pCmd->GetParameter(&ch,	pIf->m_nFirstParamIndex, MPT_CHAR) )	return false;
			sscanf(pIf->m_SecondValue.m_strValue.c_str(), "%c", &ch2);
			return IfCompare(ch, ch2, (XDummy_If::DUMMY_IF_COMPARE)(pIf->m_nCompare));
		}
		break;
	case MPT_UCHAR:
		{
			unsigned char uch, uch2;
			if( !pCmd->GetParameter(&uch,	pIf->m_nFirstParamIndex, MPT_UCHAR) )	return false;
			sscanf(pIf->m_SecondValue.m_strValue.c_str(), "%c", &uch2);
			return IfCompare(uch, uch2, (XDummy_If::DUMMY_IF_COMPARE)(pIf->m_nCompare));
		}
		break;
	case MPT_SHORT:
		{
			short sh, sh2;
			if( !pCmd->GetParameter(&sh,	pIf->m_nFirstParamIndex, MPT_SHORT) )	return false;
			sscanf(pIf->m_SecondValue.m_strValue.c_str(), "%d", &sh2);
			return IfCompare(sh, sh2, (XDummy_If::DUMMY_IF_COMPARE)(pIf->m_nCompare));
		}
		break;
	case MPT_USHORT:
		{
			unsigned short ush, ush2;
			if( !pCmd->GetParameter(&ush,	pIf->m_nFirstParamIndex, MPT_USHORT) )	return false;
			sscanf(pIf->m_SecondValue.m_strValue.c_str(), "%d", &ush2);
			return IfCompare(ush, ush2, (XDummy_If::DUMMY_IF_COMPARE)(pIf->m_nCompare));
		}
		break;
	case MPT_INT64:
		{
			int64 n64, n642;
			if( !pCmd->GetParameter(&n64,	pIf->m_nFirstParamIndex, MPT_INT64) )	return false;
			sscanf(pIf->m_SecondValue.m_strValue.c_str(), "%d", &n642);
			return IfCompare(n64, n642, (XDummy_If::DUMMY_IF_COMPARE)(pIf->m_nCompare));
		}
		break;
	case MPT_UINT64:
		{
			uint64 un64, un642;
			if( !pCmd->GetParameter(&un64,	pIf->m_nFirstParamIndex, MPT_UINT64) )	return false;
			sscanf(pIf->m_SecondValue.m_strValue.c_str(), "%d", &un642);
			return IfCompare(un64, un642, (XDummy_If::DUMMY_IF_COMPARE)(pIf->m_nCompare));
		}
		break;
	case MPT_BOOL:
		{
			bool b, b2;
			if( !pCmd->GetParameter(&b,	pIf->m_nFirstParamIndex, MPT_BOOL) )	return false;
			sscanf(pIf->m_SecondValue.m_strValue.c_str(), "%d", &b2);
			return IfCompare(b, b2, (XDummy_If::DUMMY_IF_COMPARE)(pIf->m_nCompare));
		}
		break;
	case MPT_STR:
		{
			string str, str2;
			if( !pCmd->GetParameter(&str,	pIf->m_nFirstParamIndex, MPT_STR) )	return false;
			sscanf(pIf->m_SecondValue.m_strValue.c_str(), "%d", &str2);
			return IfCompare(str, str2, (XDummy_If::DUMMY_IF_COMPARE)(pIf->m_nCompare));
		}
		break;
	case MPT_VECTOR:
		{
			vec3 v, v2;
			if( !pCmd->GetParameter(&v,	pIf->m_nFirstParamIndex, MPT_VECTOR) )	return false;
			sscanf(pIf->m_SecondValue.m_strValue.c_str(), "%f %f %f", &v2.x, &v2.y, &v2.z);
			return IfCompare(v, v2, (XDummy_If::DUMMY_IF_COMPARE)(pIf->m_nCompare));
		}
		break;
	case MPT_SVEC:
		{
			svec3 sv;
			vec3 v, v2;
			if( !pCmd->GetParameter(&sv,	pIf->m_nFirstParamIndex, MPT_SVEC) )	return false;
			v = sv;
			sscanf(pIf->m_SecondValue.m_strValue.c_str(), "%f %f %f", &v2.x, &v2.y, &v2.z);
			return IfCompare(v, v2, (XDummy_If::DUMMY_IF_COMPARE)(pIf->m_nCompare));
		}
		break;
	case MPT_SVEC2:
		{
			svec2 sv;
			vec3 v = vec3::ZERO, v2 = vec3::ZERO;
			if( !pCmd->GetParameter(&sv,	pIf->m_nFirstParamIndex, MPT_SVEC2) )	return false;
			v = sv;
			sscanf(pIf->m_SecondValue.m_strValue.c_str(), "%f %f", &v2.x, &v2.y);
			return IfCompare(v, v2, (XDummy_If::DUMMY_IF_COMPARE)(pIf->m_nCompare));
		}
		break;
	case MPT_UID:
		{
			MUID uid, uid2;
			if( !pCmd->GetParameter(&uid,	pIf->m_nFirstParamIndex, MPT_UID) )	return false;
			sscanf(pIf->m_SecondValue.m_strValue.c_str(), "%I64u", &uid2);
			return IfCompare(uid, uid2, (XDummy_If::DUMMY_IF_COMPARE)(pIf->m_nCompare));
		}
		break;
	case MPT_ID:
		{
			MID id, id2;
			if( !pCmd->GetParameter(&id,	pIf->m_nFirstParamIndex, MPT_ID) )	return false;
			sscanf(pIf->m_SecondValue.m_strValue.c_str(), "%d", &id2);
			return IfCompare(id, id2, (XDummy_If::DUMMY_IF_COMPARE)(pIf->m_nCompare));
		}
		break;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
XDummy_If::XDummy_If()
{
	m_pThen			= NULL;
	m_pElse			= NULL;

	m_nCompare		= -1;
	m_nFirstParamIndex = 0;
}

XDummy_If::~XDummy_If()
{
	SAFE_DELETE(m_pThen);
	SAFE_DELETE(m_pElse);
}

bool XDummy_If::ParseConditional( MXmlElement* pElement )
{
	//------------------------------------------------------------------------
	// 조건에 대한 비교 데이터
	string strComplare;
	_Attribute(strComplare, pElement, DUMMYCLIENT_XML_ATTR_COMPARE);
	m_nCompare = GetCompare(strComplare);

	_Attribute(m_nFirstParamIndex, pElement, DUMMYCLIENT_XML_ATTR_PARAMINDEX_1);

	string strType;
	_Attribute(strType, pElement, DUMMYCLIENT_XML_ATTR_TYPE);
	m_SecondValue.m_nType = GetParamType(strType);

	_Attribute(m_SecondValue.m_strValue, pElement, DUMMYCLIENT_XML_ATTR_VALUE_2);

	//------------------------------------------------------------------------
	// 비교에 따른 처리방향
	MXmlElement* pThenElement = pElement->FirstChildElement(DUMMYCLIENT_XML_TAG_THEN);
	if(pThenElement)
	{
		m_pThen							= new XDummy_If_Divide;

		m_pThen->ParseDivide(pThenElement);
	}

	MXmlElement* pElseElement = pElement->FirstChildElement(DUMMYCLIENT_XML_TAG_ELSE);
	if(pElseElement)
	{
		XDummy_If_Divide * m_pElse		= new XDummy_If_Divide;

		m_pElse->ParseDivide(pElseElement);
	}

	//------------------------------------------------------------------------
	// 종료
	ParseEnd(pElement);

	return true;
}

XDummy_If::DUMMY_IF_COMPARE XDummy_If::GetCompare( string strCompare )
{
	if(strCompare == "equal")			return XDummy_If::EQUAL;
	if(strCompare == "large")			return XDummy_If::LARGE;
	if(strCompare == "small")			return XDummy_If::SMALL;
	if(strCompare == "larget_equal")	return XDummy_If::LARGE_EQUAL;
	if(strCompare == "small_equal")		return XDummy_If::SMALL_EQUAL;

	return XDummy_If::ERROR_COMPARE;
}

void XDummy_If::OnCompare(XBirdClient * pClient, MCommand * pCmd)
{
	//------------------------------------------------------------------------
	// 조건식에 의한 검사
	bool bResult = IfCompareStart(this, pCmd);

	//------------------------------------------------------------------------
	// 결과에 대한 다음 명령어에 대한 처리
	XDummy_If_Divide * pResult = NULL;
	if(bResult)
	{
		pResult = m_pThen;
	}
	else
	{
		pResult = m_pElse;
	}

	vector<XDummy_Cmd *>::iterator itNextCmd = pResult->m_vecNextCmd.begin();
	while(itNextCmd != pResult->m_vecNextCmd.end())
	{
		// 보낼 명령어 검사
		if((*itNextCmd)->GetType() == DUMMY_POST)
		{
			(*itNextCmd)->PostStart(pClient);
		}

		itNextCmd++;
	}

}
//////////////////////////////////////////////////////////////////////////
bool XDummy_If_Divide::ParseDivide( MXmlElement* pElement )
{
	ParsePostChild(pElement, &m_vecNextCmd);

	return true;
}

XDummy_If_Divide::~XDummy_If_Divide()
{
	vector<XDummy_Cmd *>::iterator itCmd = m_vecNextCmd.begin();
	while(itCmd != m_vecNextCmd.end())
	{
		SAFE_DELETE(*itCmd);
		itCmd++;
	}
}
//////////////////////////////////////////////////////////////////////////
XDummy_Cmd::XDummy_Cmd()
{
	m_CmdID			= -1;
	m_bEnd			= false;
}

int XDummy_Cmd::GetCommandIDByName( string strCmdName )
{
	MCommandDesc* pCmdDesc = MGetCommandDescMap()->GetCommandDescByName(strCmdName.c_str());
	if(pCmdDesc)
	{
		return pCmdDesc->GetID();
	}

	return -1;
}

void XDummy_Cmd::ParseEnd( MXmlElement* pElement )
{
	// 종료
	MXmlElement* pEnd = pElement->FirstChildElement(DUMMYCLIENT_XML_TAG_END);
	if(pEnd)
	{
		m_bEnd = true;
	}
}
//////////////////////////////////////////////////////////////////////////
bool XDummy_OnCommand::ParseOnCommand( MXmlElement* pElement )
{
	string strCmdID;
	if(!_Attribute(strCmdID, pElement, DUMMYCLIENT_XML_ATTR_ID)) return false;

	m_CmdID = GetCommandIDByName(strCmdID.c_str());
	if(m_CmdID == -1)
	{
		return false;
	}
	
	//------------------------------------------------------------------------
	// 조건
	ParseConditionalCommand(pElement);

	//------------------------------------------------------------------------
	// 다음 컨맨드
	ParseNextCommand(pElement);

	//------------------------------------------------------------------------
	// 종료
	ParseEnd(pElement);

	return true;
}

bool XDummy_OnCommand::ParseNextCommand( MXmlElement* pElement )
{
	ParsePostChild(pElement, &m_vecNextCmd);

	return true;
}

bool XDummy_OnCommand::ParseConditionalCommand( MXmlElement* pElement )
{
	for(MXmlElement* pIf = pElement->FirstChildElement(DUMMYCLIENT_XML_TAG_IF); pIf != NULL; pIf = pIf->NextSiblingElement())
	{
		XDummy_If *			m_pif = new XDummy_If;

		m_pif->ParseConditional(pIf);
		m_vecNextCmd.push_back(m_pif);
	}

	return true;
}

void XDummy_OnCommand::OnCommand(XBirdClient * pClient, MCommand * pCmd)
{
	//------------------------------------------------------------------------
	// 다음 명령어에 대한 처리
	mlog("받은 메세지 ID %d\n", pCmd->GetID());
	vector<XDummy_Cmd *>::iterator itNextCmd = m_vecNextCmd.begin();
	while(itNextCmd != m_vecNextCmd.end())
	{
		// 조건이 있는지 검사
		if((*itNextCmd)->GetType() == DUMMY_IF)
		{
			(*itNextCmd)->OnCompare(pClient, pCmd);
		}

		// 보낼 명령어 검사
		if((*itNextCmd)->GetType() == DUMMY_POST)
		{
			(*itNextCmd)->PostStart(pClient);
		}

		itNextCmd++;
	}

}

XDummy_OnCommand::~XDummy_OnCommand()
{
	vector<XDummy_Cmd *>::iterator itCmd = m_vecNextCmd.begin();
	while(itCmd != m_vecNextCmd.end())
	{
		SAFE_DELETE(*itCmd);
		itCmd++;
	}
}
//////////////////////////////////////////////////////////////////////////
bool XDummy_Post::ParsePost( MXmlElement* pElement )
{
	string strCmdID;
	if(!_Attribute(strCmdID, pElement, DUMMYCLIENT_XML_ATTR_ID)) return false;

	m_CmdID = GetCommandIDByName(strCmdID.c_str());
	if(m_CmdID == -1)
	{
		return false;
	}

	//------------------------------------------------------------------------
	// Param
	ParseParam(pElement);

	//------------------------------------------------------------------------
	// 종료
	ParseEnd(pElement);

	return true;
}

bool XDummy_Post::ParseParam( MXmlElement* pElement )
{
	for(MXmlElement* pParam = pElement->FirstChildElement(DUMMYCLIENT_XML_TAG_PARAM); pParam != NULL; pParam = pParam->NextSiblingElement())
	{
		if(IsSameTag(pParam, DUMMYCLIENT_XML_TAG_PARAM))
		{
			XDummy_ParamValue ParamValue;

			string strType;
			_Attribute(strType, pParam, DUMMYCLIENT_XML_ATTR_TYPE);
			ParamValue.m_nType = GetParamType(strType);

			if(ParamValue.m_nType == MPT_BLOB)
			{
				// BLOB는 따로 처리
				ParamValue.m_nBlobDataSize = 0;
				for(MXmlElement* pBlobData = pParam->FirstChildElement(DUMMYCLIENT_XML_TAG_DATA); pBlobData != NULL; pBlobData = pBlobData->NextSiblingElement())
				{
					if(IsSameTag(pBlobData, DUMMYCLIENT_XML_TAG_DATA))
					{
						XDummy_ParamValue BlobData;

						string strType;
						_Attribute(strType, pBlobData, DUMMYCLIENT_XML_ATTR_TYPE);
						BlobData.m_nType = GetParamType(strType);
						_Contents(BlobData.m_strValue, pBlobData);

						ParamValue.m_nBlobDataSize += GetParameterTypeSize(BlobData.m_nType);

						ParamValue.m_vecBlob.push_back(BlobData);
					}
				}
			}
			else
			{
				_Contents(ParamValue.m_strValue, pParam);
			}

			m_vecPostParam.push_back(ParamValue);
		}
	}

	return true;
}

void XDummy_Post::PostStart( XBirdClient * pClient )
{
	mlog("보내는 메세지 ID %d\n", m_CmdID);

	XBirdClient * pBirdClient = pClient;
	switch (m_vecPostParam.size())
	{
	case 0:
		{
			XDUMMYPOSTCMD0(pBirdClient, m_CmdID);
		}
		break;
	case 1:
		{
			XDUMMYPOSTCMD1(pBirdClient, m_CmdID, GetParameterTransform(&(m_vecPostParam[0])));
		}
		break;
	case 2:
		{
			XDUMMYPOSTCMD2(pBirdClient, m_CmdID, GetParameterTransform(&(m_vecPostParam[0])), 
												 GetParameterTransform(&(m_vecPostParam[1])));
		}
		break;
	case 3:
		{
			XDUMMYPOSTCMD3(pBirdClient, m_CmdID, GetParameterTransform(&(m_vecPostParam[0])),
												 GetParameterTransform(&(m_vecPostParam[1])), 
												 GetParameterTransform(&(m_vecPostParam[2])));
		}
		break;
	case 4:
		{
			XDUMMYPOSTCMD4(pBirdClient, m_CmdID, GetParameterTransform(&(m_vecPostParam[0])), 
												 GetParameterTransform(&(m_vecPostParam[1])), 
												 GetParameterTransform(&(m_vecPostParam[2])), 
												 GetParameterTransform(&(m_vecPostParam[3])));
		}
		break;
	case 5:
		{
			XDUMMYPOSTCMD5(pBirdClient, m_CmdID, GetParameterTransform(&(m_vecPostParam[0])), 
												 GetParameterTransform(&(m_vecPostParam[1])), 
										 		 GetParameterTransform(&(m_vecPostParam[2])), 
												 GetParameterTransform(&(m_vecPostParam[3])),
												 GetParameterTransform(&(m_vecPostParam[4])));
		}
		break;
	case 6:
		{
			XDUMMYPOSTCMD6(pBirdClient, m_CmdID, GetParameterTransform(&(m_vecPostParam[0])), 
												 GetParameterTransform(&(m_vecPostParam[1])), 
												 GetParameterTransform(&(m_vecPostParam[2])), 
												 GetParameterTransform(&(m_vecPostParam[3])),
												 GetParameterTransform(&(m_vecPostParam[4])),
												 GetParameterTransform(&(m_vecPostParam[5])));
		}
		break;
	case 7:
		{
			XDUMMYPOSTCMD7(pBirdClient, m_CmdID, GetParameterTransform(&(m_vecPostParam[0])), 
												 GetParameterTransform(&(m_vecPostParam[1])), 
												 GetParameterTransform(&(m_vecPostParam[2])), 
												 GetParameterTransform(&(m_vecPostParam[3])),
												 GetParameterTransform(&(m_vecPostParam[4])),
												 GetParameterTransform(&(m_vecPostParam[5])),
												 GetParameterTransform(&(m_vecPostParam[6])));
		}
		break;
	}

}

//////////////////////////////////////////////////////////////////////////
XDummyPattern::XDummyPattern()
{
	m_vecStart_Cmd.clear();
	m_strName.clear();
}

XDummyPattern::~XDummyPattern()
{
	vector<XDummy_Cmd *>::iterator itCmd = m_vecStart_Cmd.begin();
	while(itCmd != m_vecStart_Cmd.end())
	{
		SAFE_DELETE(*itCmd);
		itCmd++;
	}
}

bool XDummyPattern::ParseDummyClient( MXmlElement* pElement, MXml* pXml )
{
	if(!_Attribute(m_strName, pElement, DUMMYCLIENT_XML_ATTR_NAME)) return false;

	ParseOnCommandChild(pElement, &m_vecStart_Cmd);
	ParsePostChild(pElement, &m_vecStart_Cmd);

	return true;
}

void XDummyPattern::OnCommand( XBirdClient* pClient, MCommand * pCmd )
{
	vector<XDummy_Cmd *>::iterator itCmd = m_vecStart_Cmd.begin();
	while(itCmd != m_vecStart_Cmd.end())
	{
		if((*itCmd)->GetType() == DUMMY_ONCOMMAND &&
			(*itCmd)->m_CmdID == pCmd->GetID())
		{
			(*itCmd)->OnCommand(pClient, pCmd);
		}
		itCmd++;
	}
}


