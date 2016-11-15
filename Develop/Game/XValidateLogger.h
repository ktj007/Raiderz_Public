#ifndef _X_VALIDATE_LOGGER_H_
#define _X_VALIDATE_LOGGER_H_

#include "CSLogListener.h"
#include "CSValidateLogger.h"

class XValidateLogListener : public CSLogListener
{
public:
	virtual void OnLog(const char* szLog);
};

class XValidateLogger : public CSValidateLogger
{
public:
	struct PUSHANDPOPELEMENT
	{
		XValidateLogger& m_Logger;
		PUSHANDPOPELEMENT(XValidateLogger& logger, const XValidateLogger::ELEMENT& element)	
			: m_Logger(logger)
		{
			m_Logger.PushElement(element);
		}

		~ PUSHANDPOPELEMENT()
		{
			m_Logger.PopElement();
		}
	};
private:
	XValidateLogListener	m_Listener;
public:
	XValidateLogger();
	virtual ~XValidateLogger();
};


#endif//_X_VALIDATE_LOGGER_H_