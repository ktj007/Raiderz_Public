#pragma once

#include <vector>
#include <string>
#include <map>

#include "M_ToolFunctor.h"

namespace rs3
{
	class RSingletonSceneManager;
};
class CControlMediator;

class CORETOOL_API CRubyManager
{
public:
	inline static CRubyManager& GetReference()
	{
		static CRubyManager rubyScriptController;
		return rubyScriptController;
	}

	//Ruby Script Caller
	void				DoMapBatchScript(const char* _pSzScript, std::vector<std::string>& _refMapFiles);
	void				CallRubyForSortingZoneXml(std::string& _refStrXml);

	void				AddManagedScriptFunctor(CCoreToolFunctor* _pFunctor);
	void				ClearManagedScriptFunctor();

	CCoreToolFunctor* GetBuiltInFunc(const std::string& _strFuncNmae)
	{
		MAP_BUILTIN_FUNCS::iterator itr = m_mapBuiltInFuncs_.find(_strFuncNmae);
		if (itr != m_mapBuiltInFuncs_.end())
			return itr->second;

		return NULL;
	}

private:
	typedef unsigned long VALUE;
	struct CARS_C
	{
		static VALUE testSayHello();
		static VALUE sortZoneXml(VALUE called_rb_self, VALUE filename);

		// engine test script
		static VALUE testZoneEngine(VALUE called_rb_self, VALUE test_type_num, VALUE test_iter_num, VALUE test_parameter_str);

		static VALUE print(VALUE called_rb_self, VALUE strMessage);
	};

	struct CARS_R
	{
		static VALUE CallRubyFunctionFromFile(VALUE _fileName);
	};

	bool IsValidRubyScript_(int _nError);
	CRubyManager(void);
	~CRubyManager(void);

private:
	typedef std::map< std::string, CCoreToolFunctor* > MAP_BUILTIN_FUNCS;
	MAP_BUILTIN_FUNCS m_mapBuiltInFuncs_;
};
