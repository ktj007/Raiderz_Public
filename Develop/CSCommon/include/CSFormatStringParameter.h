#pragma once

#define FSP_OPEN		_T("")
#define FSP_CLOSE		_T("")
#define FSP_SEPARATOR	_T("|")

struct FSParam
{
	tstring m_strParam;

	FSParam()	
	{
		// do nothing
	}

	template<typename Arg0>	
	FSParam(Arg0 arg0)
	{
		tstringstream stream;		
		stream << FSP_OPEN;
		stream << arg0;
		stream << FSP_CLOSE;		
		m_strParam = stream.str();
	}

	template<typename Arg0, typename Arg1>
	FSParam(Arg0 arg0, Arg1 arg1)
	{
		tstringstream stream;
		stream << FSP_OPEN;
		stream << arg0 << FSP_SEPARATOR << arg1;
		stream << FSP_CLOSE;
		m_strParam = stream.str();
	}

	template<typename Arg0, typename Arg1, typename Arg2>
	FSParam(Arg0 arg0, Arg1 arg1, Arg2 arg2)
	{
		tstringstream stream;
		stream << FSP_OPEN;
		stream << arg0 << FSP_SEPARATOR << arg1 << FSP_SEPARATOR << arg2;
		stream << FSP_CLOSE;
		m_strParam = stream.str();
	}

	template<typename Arg0, typename Arg1, typename Arg2, typename Arg3>
	FSParam(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3)
	{
		tstringstream stream;
		stream << FSP_OPEN;
		stream << arg0 << FSP_SEPARATOR << arg1 << FSP_SEPARATOR << arg2 << FSP_SEPARATOR << arg3;
		stream << FSP_CLOSE;
		m_strParam = stream.str();
	}

	template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	FSParam(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
	{
		tstringstream stream;
		stream << FSP_OPEN;
		stream << arg0 << FSP_SEPARATOR << arg1 << FSP_SEPARATOR << arg2 << FSP_SEPARATOR << arg3 << FSP_SEPARATOR << arg4;
		stream << FSP_CLOSE;
		m_strParam = stream.str();
	}

	template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	FSParam(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
	{
		tstringstream stream;
		stream << FSP_OPEN;
		stream << arg0 << FSP_SEPARATOR << arg1 << FSP_SEPARATOR << arg2 << FSP_SEPARATOR << arg3 << FSP_SEPARATOR << arg4 << FSP_SEPARATOR << arg5;
		stream << FSP_CLOSE;
		m_strParam = stream.str();
	}

	template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
	FSParam(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6)
	{
		tstringstream stream;
		stream << FSP_OPEN;
		stream << arg0 << FSP_SEPARATOR << arg1 << FSP_SEPARATOR << arg2 << FSP_SEPARATOR << arg3 << FSP_SEPARATOR << arg4 << FSP_SEPARATOR << arg5 << FSP_SEPARATOR << arg6;
		stream << FSP_CLOSE;
		m_strParam = stream.str();
	}

	template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
	FSParam(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7)
	{
		tstringstream stream;
		stream << FSP_OPEN;
		stream << arg0 << FSP_SEPARATOR << arg1 << FSP_SEPARATOR << arg2 << FSP_SEPARATOR << arg3 << FSP_SEPARATOR << arg4 << FSP_SEPARATOR << arg5 << FSP_SEPARATOR << arg6 << FSP_SEPARATOR << arg7;
		stream << FSP_CLOSE;
		m_strParam = stream.str();
	}

	template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
	FSParam(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7, Arg8 arg8)
	{
		tstringstream stream;
		stream << FSP_OPEN;
		stream << arg0 << FSP_SEPARATOR << arg1 << FSP_SEPARATOR << arg2 << FSP_SEPARATOR << arg3 << FSP_SEPARATOR << arg4 << FSP_SEPARATOR << arg5 << FSP_SEPARATOR << arg6 << FSP_SEPARATOR << arg7 << FSP_SEPARATOR << arg8;
		stream << FSP_CLOSE;
		m_strParam = stream.str();
	}

	template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
	FSParam(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9)
	{
		tstringstream stream;
		stream << FSP_OPEN;
		stream << arg0 << FSP_SEPARATOR << arg1 << FSP_SEPARATOR << arg2 << FSP_SEPARATOR << arg3 << FSP_SEPARATOR << arg4 << FSP_SEPARATOR << arg5 << FSP_SEPARATOR << arg6 << FSP_SEPARATOR << arg7 << FSP_SEPARATOR << arg8 << FSP_SEPARATOR << arg9;
		stream << FSP_CLOSE;
		m_strParam = stream.str();
	}
};
