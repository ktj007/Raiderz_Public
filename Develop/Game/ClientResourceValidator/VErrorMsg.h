#pragma once

enum ERROR_MSG_TYPE
{
	EMT_NAME,			// 이름이 없다.
	EMT_LINK,			// 연결된 파일에 이름이 없다.
	EMT_FILE,			// 파일이 없다.
	EMT_STRING,			// 오타
};

struct stERRORMSG
{
	ERROR_MSG_TYPE				eTYpe;
	string						strErrorMsg;
};

class VErrorMsg
{
public:
	vector<stERRORMSG>				m_strErrorMsg;

public:
	VErrorMsg();
	virtual ~VErrorMsg();

	void						Add(ERROR_MSG_TYPE eType, string& strMsg);
	void						Add(VErrorMsg& errorMsg);

	void						AddErrorXml(string& strXmlFileName);
	void						AddErrorFile(string& strFileName);

	void						Clear();

	bool						IsError();
};

