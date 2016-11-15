#pragma once

#include <string>
#include <vector>

class EnumDescription
{
public:
	static bool CreateEnumDescription(const char* szDescription, EnumDescription* pEnumDescription, const char** ppszDescription );

	int			GetValue( const char* szString ) const;
	const char*	GetString( unsigned int nValue ) const;

	unsigned int GetCount() const;

	enum
	{
		UNDEFINED = INT_MIN
	};

protected:
	std::vector<std::string> m_enumStrings;

};

//////////////////////////////////////////////////////////////////////////
// inlines

inline unsigned int EnumDescription::GetCount()	const {	return m_enumStrings.size();	}

