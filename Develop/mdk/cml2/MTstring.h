#pragma once

#include <tchar.h>
#include <regex>

namespace std
{
	typedef basic_string<_TCHAR, char_traits<_TCHAR>, 
		allocator<_TCHAR> > tstring;

	typedef basic_stringstream<_TCHAR, char_traits<_TCHAR>,
		allocator<_TCHAR> > tstringstream;


	namespace tr1
	{
		typedef basic_regex<_TCHAR> tregex;
		typedef match_results<const _TCHAR*> tmatch;
		typedef match_results<tstring::const_iterator> tsmatch;
	}
}
