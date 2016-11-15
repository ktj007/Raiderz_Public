#include "stdafx.h"
#include "MCommandLine.h"
#include "MStringUtil.h"

namespace cml2 {

const char* const kSwitchPrefixes[] = {"--", "-", "/"};
const char kSwitchTerminator[] = "--";
const char kSwitchValueSeparator[] = "=";

template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

#define arraysize(array) (sizeof(ArraySizeHelper(array)))


void MCommandLine::ParseFromString( const std::string& command_line )
{
	m_strCommandLine = TrimBlanks(command_line);

	if (m_strCommandLine.empty())
		return;

	int num_args = 0;
	wchar_t** args = NULL;

	std::wstring wsCommandLineString = ASCIIToWide(m_strCommandLine);

	args = CommandLineToArgvW(wsCommandLineString.c_str(), &num_args);

	if (num_args > 0)
	{
		m_strProgram = TrimBlanks(WideToASCII(args[0]));
	}

	bool parse_switches = true;
	for (int i = 1; i < num_args; ++i) 
	{
		std::string arg;
		arg = WideToASCII(args[i]);
		arg = TrimBlanks(arg);

		if (!parse_switches) 
		{
			m_vecLooseValues.push_back(arg);
			continue;
		}

		if (arg == kSwitchTerminator) 
		{
			parse_switches = false;
			continue;
		}

		std::string switch_string;
		std::string switch_value;

		if (IsSwitch(arg, &switch_string, &switch_value)) 
		{
			switches_type[switch_string] = switch_value;
		} 
		else 
		{
			m_vecLooseValues.push_back(arg);
		}
	}

	if (args)
		LocalFree(args);
}


MCommandLine::MCommandLine()
{
	
}

// static
bool MCommandLine::IsSwitch(const StringType& parameter_string,
						   std::string* switch_string,
						   StringType* switch_value) 
{
	switch_string->clear();
	switch_value->clear();

	for (size_t i = 0; i < arraysize(kSwitchPrefixes); ++i) 
	{
	   StringType prefix(kSwitchPrefixes[i]);
	   if (parameter_string.find(prefix) != 0)
		   continue;

	   const size_t switch_start = prefix.length();
	   const size_t equals_position = parameter_string.find(
		   kSwitchValueSeparator, switch_start);
	   StringType switch_native;
	   if (equals_position == StringType::npos) {
		   switch_native = parameter_string.substr(switch_start);
	   } else {
		   switch_native = parameter_string.substr(
			   switch_start, equals_position - switch_start);
		   *switch_value = parameter_string.substr(equals_position + 1);
	   }

	   switch_native = ToLower(switch_native);

	   *switch_string = switch_native;

	   return true;
	}

	return false;
}

std::string MCommandLine::GetSwitchValue( const std::string& switch_string ) const
{
	std::string lowercased_switch = ToLower(switch_string);

	std::map<std::string, StringType>::const_iterator result = switches_type.find(lowercased_switch);

	if (result != switches_type.end()) 
	{
		return result->second;
	} 

	return "";
}

int MCommandLine::GetSwitchValueInt( const std::string& switch_string ) const
{
	string strValue = GetSwitchValue(switch_string);
	int nRet = atoi(strValue.c_str());
	return nRet;
}

bool MCommandLine::HasSwitch( const std::string& switch_string ) const
{
	std::string lowercased_switch = ToLower(switch_string);

	return switches_type.find(lowercased_switch) != switches_type.end();

}

std::vector<std::string> MCommandLine::GetLooseValues() const
{
	return m_vecLooseValues;
}

std::string MCommandLine::PrefixedSwitchString( const std::string& switch_string )
{
	char text[2048];
	sprintf_s(text, "%s%s", kSwitchPrefixes[0], switch_string.c_str());;

	return string(text);
}

std::string MCommandLine::PrefixedSwitchStringWithValue( const std::string& switch_string, const std::string& value_string )
{
	if (value_string.empty()) 
	{
		return PrefixedSwitchString(switch_string);
	}

	char text[2048];
	sprintf_s(text, "%s%s%s%s", kSwitchPrefixes[0], switch_string.c_str(), kSwitchValueSeparator, value_string.c_str());

	return string(text);
}

std::string MCommandLine::GetProgram() const
{
	return m_strProgram;
}

void MCommandLine::ParseFromCommandLine()
{
	ParseFromString(::GetCommandLine());
}

} // namespace cml2 {