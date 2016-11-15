#pragma once

#include <string>
#include <map>
#include <vector>
#include "RLib.h"
using namespace std;

namespace cml2 {


/// 커맨드 라인 파서
/// 
/// "-switch=value" 이런 형식으로 된 커맨드 라인을 각각의 스위치별 값으로 파싱한다.
class CML2_API MCommandLine
{
private:
	std::string m_strCommandLine;
	std::string m_strProgram;

	typedef std::string StringType;

	static bool IsSwitch(const StringType& parameter_string, std::string* switch_string, StringType* switch_value);

	std::map<std::string, StringType> switches_type;
	std::vector<StringType> m_vecLooseValues;
public:
	MCommandLine();

	void ParseFromString(const std::string& strCommandLine);		///< arguments 스트링를 인자로 넘겨서 파싱
	void ParseFromCommandLine();									///< GetCommandLine api를 이용하여 arguments를 바로 알아와서 파싱

	bool HasSwitch(const std::string& switch_string) const;
	std::string GetSwitchValue(const std::string& switch_string) const;
	int GetSwitchValueInt(const std::string& switch_string) const;

	std::vector<std::string> GetLooseValues() const;

	const std::string& GetCommandLineString() const 
	{
		return m_strCommandLine;
	}
	std::string GetProgram() const;

	static std::string PrefixedSwitchString(const std::string& switch_string);
	static std::string PrefixedSwitchStringWithValue(const std::string& switch_string, const std::string& value_string);
};

/*
 사용법

 GameServer.exe -att=ai_profile -frame=0 -repeat=1 -time=600 -speed=1 --auto


 MCommandLine cl;
 cl.ParseFromCommandLine();

 string strValue = cl.GetSwitchValue("att");

 bool bAuto = cl.HasSwitch("auto");

*/

} // namespace cml2 {