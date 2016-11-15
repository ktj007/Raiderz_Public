#pragma once

class XFeedbackSender
{
private:
protected:
	struct _In
	{
		wstring		strUserID;
		wstring		strCharName;
		int			nFieldID;
		vec3		vPos;
		vec3		vDir;
		wstring		strTag;
		wstring		strText;
	};

	_In			m_InputData;

	void SaveFile();
	void SendFile();
public:
	XFeedbackSender() {}
	virtual ~XFeedbackSender() {}
	void Send(const wchar_t* szUserID,
			  const wchar_t* szCharName,
			  int nFieldID,
			  vec3 vPos,
			  vec3 vDir,
			  const wchar_t* szTag,
			  const wchar_t* szText);
};