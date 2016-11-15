#pragma once

#include <vector>
#include <string>
#include <map>

class CORETOOL_API CCoreToolFunctor
{
public:
	CCoreToolFunctor(){}
	virtual ~CCoreToolFunctor(){}

	virtual bool Run() = 0;
	virtual const char* GetFuncName() = 0;

	void AddParam(const std::string& _StrParamName, const std::string& _StrParamValue)
	{
		_ASSERT(m_mapParams_.find(_StrParamName) == m_mapParams_.end());
		m_mapParams_.insert( MAP_PARAM::value_type(_StrParamName, _StrParamValue) );
	}

protected:
	typedef std::map< std::string, std::string > MAP_PARAM;
	MAP_PARAM m_mapParams_;
};
