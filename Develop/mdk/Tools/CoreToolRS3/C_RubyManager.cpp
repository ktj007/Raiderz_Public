#include "StdAfx.h"
#include "C_RubyManager.h"

#include "MainFrm.h"
#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperResource.h"
#include "C_RubyTestSet.h"

#include "ruby.h"
#pragma warning(disable:4311)

//#define _USE_RUBY

//////////////////////////////////////////////////////////////////////////////////////
//
// CARS (CoreTool API for Ruby Script) Glue Function
//
VALUE CRubyManager::CARS_C::testSayHello()
{
	mlog("Say, Hello! \n");
	return 0;
}

VALUE CRubyManager::CARS_C::sortZoneXml(VALUE called_rb_self, VALUE filename)
{
	#ifdef _USE_RUBY
	CControlMediator* pMediator = ((CMainFrame*)AfxGetMainWnd())->GetControlMediator();
	if(pMediator == NULL) return 0;
	
	CControlMediatorHelperResource* pMediatorHelpr = pMediator->GetHelperResource();
	if(pMediatorHelpr == NULL) return 0;

	const char* pSzFileName = STR2CSTR(filename);
	//for test
	//mlog( "%s \n", pSzFileName );

	MXml xmlToSort;
	xmlToSort.LoadFile(pSzFileName);
	pMediatorHelpr->SortZoneXmlAndSave(pSzFileName, xmlToSort);
	#endif
	return 0;
}

VALUE CRubyManager::CARS_C::testZoneEngine(VALUE called_rb_self, VALUE test_type_num, VALUE test_iter_num, VALUE test_parameter_str)
{
	#ifdef _USE_RUBY
	if (CRubyTestSet::GetReference().TestZoneEngine(FIX2INT(test_type_num), FIX2INT(test_iter_num), STR2CSTR(test_parameter_str)) )
		return rb_str_new2( CRubyTestSet::GetReference().GetLastTestLog() );
	else
		return rb_str_new2("");
	#else
	return 0;
	#endif
}

VALUE CRubyManager::CARS_C::print(VALUE called_rb_self, VALUE strMessage)
{
	#ifdef _USE_RUBY
	const char* pSzMessage = STR2CSTR(strMessage);
	mlog("%s", pSzMessage);
	#endif
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// CARS (CoreTool API for Ruby Script) Glue Function Register
//
CRubyManager::CRubyManager(void)
{
	#ifdef _USE_RUBY
	// ruby win32 init
	int argc = 0;
	char **argv = 0;
	NtInitialize(&argc, &argv);

	//루비 환경 만들기
	ruby_init();
	ruby_init_loadpath();

	//모듈로 만들기
	VALUE module = rb_define_module("CARS"); 

	//모듈 함수 등록
	rb_define_module_function(module, "testSayHello",	reinterpret_cast<VALUE (*)(...)>(CARS_C::testSayHello), 0);		// 함수인자 없음
	rb_define_module_function(module, "testZoneEngine",	reinterpret_cast<VALUE (*)(...)>(CARS_C::testZoneEngine), 3);	// 함수인자 3개
	rb_define_module_function(module, "sortZoneXml",	reinterpret_cast<VALUE (*)(...)>(CARS_C::sortZoneXml), 1);		// 함수인자 1개
	rb_define_module_function(module, "print",			reinterpret_cast<VALUE (*)(...)>(CARS_C::print), 1);			// 함수인자 1개
	#endif
}

CRubyManager::~CRubyManager(void)
{
	_ASSERT(m_mapBuiltInFuncs_.empty());

	#ifdef _USE_RUBY
	//ruby_finalize();
	ruby_cleanup(0);
	#endif
}

VALUE CRubyManager::CARS_R::CallRubyFunctionFromFile(VALUE _fileName)
{
	#ifdef _USE_RUBY
	//루비 스크립트 로딩
	//VALUE rubyScriptName = rb_str_new2((char*)_fileName);
	//rb_load(rubyScriptName, 0);
	rb_require((char*)_fileName);

	//루비 함수 호출
	std::string strFuncName((char*)_fileName);
	int nToken = strFuncName.rfind("/") + 1; // -3 : 확장자 제거
	rb_funcall(
		rb_str_new2((char*)_fileName),
		rb_intern( strFuncName.substr(nToken, strFuncName.length() - nToken - 3).c_str() ),
		0
	);
	#endif
	return Qnil;
}

bool CRubyManager::IsValidRubyScript_(int _nError)
{
	#ifdef _USE_RUBY
	if(_nError)
	{
		VALUE mes = rb_inspect(rb_gv_get("$!"));
		mlog("Ruby Script ERROR : %s\n", RSTRING(mes)->ptr);
		rb_gc();
		AfxMessageBox("루비 스크립트 호출중 에러");
		return false;
	}
	#endif
	return true;
}

void CRubyManager::DoMapBatchScript(const char* _pSzScript, std::vector<std::string>& _refMapFiles)
{
	#ifdef _USE_RUBY
	//모래시계 시작
	HCURSOR hOld = SetCursor(LoadCursor(NULL, IDC_WAIT));

	VALUE mapResArray = rb_ary_new();;
	rb_define_variable("$MAP_FILES", &mapResArray);

	for(std::vector<std::string>::iterator itr = _refMapFiles.begin();
		itr != _refMapFiles.end(); ++itr)
		rb_ary_push(mapResArray, rb_str_new2(itr->c_str()));

	int error;
	rb_protect(CARS_R::CallRubyFunctionFromFile, (VALUE)_pSzScript, &error);
	if(IsValidRubyScript_(error))
		rb_gc();

	//모래시계 끝
	SetCursor(hOld);
	#endif
}

void CRubyManager::CallRubyForSortingZoneXml(std::string& _refStrXml)
{
	#ifdef _USE_RUBY
	VALUE strDoc = rb_str_new2(_refStrXml.c_str());
	rb_define_variable("$PRE_SORTED_XML", &strDoc);

	int error;
	rb_protect( CARS_R::CallRubyFunctionFromFile, (VALUE)("ruby/scaffolding/sort_zone_xml_string.rb"), &error);

	if( IsValidRubyScript_(error) )
	{
		_refStrXml = RSTRING(rb_gv_get("$POST_SORTED_XML"))->ptr;
		rb_gc();
	}
	#endif
}

void CRubyManager::AddManagedScriptFunctor( CCoreToolFunctor* _pFunctor )
{
	_ASSERT(_pFunctor);
	if (!_pFunctor)
		return;

	std::string func_name = _pFunctor->GetFuncName();
	_ASSERT(m_mapBuiltInFuncs_.find(func_name) == m_mapBuiltInFuncs_.end());
	if (m_mapBuiltInFuncs_.find(func_name) != m_mapBuiltInFuncs_.end())
		return;

	m_mapBuiltInFuncs_.insert( MAP_BUILTIN_FUNCS::value_type( func_name, _pFunctor) );
}

void CRubyManager::ClearManagedScriptFunctor()
{
	for (MAP_BUILTIN_FUNCS::iterator itr = m_mapBuiltInFuncs_.begin(); itr != m_mapBuiltInFuncs_.end(); ++itr)
	{
		delete itr->second;
	}
	m_mapBuiltInFuncs_.clear();
}
