#pragma once

#include "CSBaseValidator.h"
#include "XValidateLogger.h"

class XBaseValidator : public CSBaseValidator
{
protected:
	bool	ExistFile(const wchar_t* str)
	{
		HANDLE hDir;
		WIN32_FIND_DATA DirData = {0};

		hDir = FindFirstFile(str, &DirData);
		if( hDir == INVALID_HANDLE_VALUE)
			return false;

		return true;
	}

public:
protected:
	XValidateLogger	m_Logger;
};


#define _VLOGGER						m_Logger
#define _VLI(_File)						m_Logger.Init(_File)
#define _VLE(_Tag)						XValidateLogger::ELEMENT vle(_Tag)
#define _VLA(_AttrName, _AttrValue)		vle.Insert(XValidateLogger::ELEMENT::ATTRIBUTE(_AttrName, _AttrValue))
#define _VLP							XValidateLogger::PUSHANDPOPELEMENT vlp(m_Logger, vle)
