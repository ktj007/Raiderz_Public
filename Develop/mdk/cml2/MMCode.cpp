#include "stdafx.h"
#include "MMCode.h"
#include "MStringUtil.h"
#include <sstream>
#include <ctype.h>
#include <algorithm>

namespace mmcode
{


//
// MTagManager
//

MTagManager::~MTagManager()
{
	for (ControlCodeMap::iterator it = m_ControlCodes.begin(); it != m_ControlCodes.end(); ++it)
	{
		delete ((*it).second);
	}

	for (ReplaceCharsMap::iterator it = m_ReplaceChars.begin(); it != m_ReplaceChars.end(); ++it)
	{
		delete ((*it).second);
	}

	for (TagMap::iterator it = m_Tags.begin(); it != m_Tags.end(); ++it)
	{
		delete ((*it).second);
	}

}

void MTagManager::AddControlCode( const string& strName, MControlCode* pHandler )
{
	RemoveControlCode(strName);
	m_ControlCodes.insert(ControlCodeMap::value_type(strName, pHandler));
}

void MTagManager::RemoveControlCode( const string& name )
{
	ControlCodeMap::iterator it = m_ControlCodes.find(name);

	if (it != m_ControlCodes.end())
	{
		delete ((*it).second);
		m_ControlCodes.erase(it);
	}
}

MControlCode* MTagManager::FindControlCode( const string& name )
{
	ControlCodeMap::iterator it = m_ControlCodes.find(cml2::ToLower(name));
	return (it != m_ControlCodes.end() ? (*it).second : NULL);
}

void MTagManager::AddReplaceChars( const string& strName, MReplaceChars* pHandler )
{
	RemoveReplaceChars(strName);
	m_ReplaceChars.insert(ReplaceCharsMap::value_type(strName, pHandler));

}

void MTagManager::RemoveReplaceChars( const string& name )
{
	ReplaceCharsMap::iterator it = m_ReplaceChars.find(name);

	if (it != m_ReplaceChars.end())
	{
		delete ((*it).second);
		m_ReplaceChars.erase(it);
	}
}

MReplaceChars* MTagManager::FindReplaceChars( const string& name )
{
	ReplaceCharsMap::iterator it = m_ReplaceChars.find(cml2::ToLower(name));
	return (it != m_ReplaceChars.end() ? (*it).second : NULL);
}

void MTagManager::AddTag( const string& strName, MTag* pHandler )
{
	RemoveTag(strName);
	m_Tags.insert(TagMap::value_type(strName, pHandler));
}

void MTagManager::RemoveTag( const string& name )
{
	TagMap::iterator it = m_Tags.find(name);

	if (it != m_Tags.end())
	{
		delete ((*it).second);
		m_Tags.erase(it);
	}
}

MTag* MTagManager::FindTag( const string& name )
{
	TagMap::iterator it = m_Tags.find(cml2::ToLower(name));
	return (it != m_Tags.end() ? (*it).second : NULL);
}

///////////////////////////////////////////////////////////////////////

MControlCodeTransResult MControlCodeTransformer::transform()
{
	MControlCodeTransResult	result;

	string::size_type pos = 0;
	string out;

	out.reserve(m_strIn.length());

	for ( ; pos < m_strIn.length() ; )
	{
		const string::size_type openingBracket = m_strIn.find('{', pos);

		if (openingBracket == string::npos)
		{
			out += string(m_strIn.begin() + pos, m_strIn.end());
			break;
		}

		out += string(m_strIn.begin() + pos, m_strIn.begin() + openingBracket);

		const string::size_type nextOpeningBracket = m_strIn.find('{', openingBracket + 1);
		const string::size_type closingBracket = m_strIn.find('}', openingBracket + 1);

		if (closingBracket == string::npos)
		{
			out += "{";
			pos = openingBracket + 1;
			continue;
		}

		if (nextOpeningBracket != string::npos &&
			nextOpeningBracket < closingBracket)
		{
			out += "{";
			pos = openingBracket + 1;
			continue;
		}

		// 태그 이름 추출
		const string::size_type sep = m_strIn.find_first_of("=\t\r\n}", openingBracket + 1);

		if (sep == string::npos)
		{
			out += "{";
			pos = openingBracket + 1;
			continue;
		}

		const string tagName = cml2::TrimBlanks(string(m_strIn.begin() + openingBracket + 1, m_strIn.begin() + sep));

		// Extract tag parameter: {tag=param}
		string param((sep != closingBracket)
			? string(m_strIn.begin() + sep + 1, m_strIn.begin() + closingBracket)
			: string(""));
		param = cml2::TrimBlanks(param);

		// Process
		{
			MControlCode* t = m_Mgr.FindControlCode(tagName);
			if (t != NULL)
			{
				result.AddCode(t, param);
			}
			else
			{
				out += "{";
				out += tagName;

				if (!param.empty())
				{
					out += "=";
					out += param;
				}
				out += "}";
			}
		}

		// Continue with the remaining text
		pos = closingBracket + 1;
	}

	result.m_strOutput = out;

	return (result);
}



void MControlCodeTransResult::AddCode( MControlCode* pCode, const string& param )
{
	m_Codes.push_back(_Code(pCode, param));
}

void MControlCodeTransResult::Proc( void* pParam/*=0*/ )
{
	for (size_t i = 0; i < m_Codes.size(); ++i)
	{
		m_Codes[i].m_pCode->Proc(m_Codes[i].m_strParam, pParam);
	}
}

void replace(std::string& text, const std::string& find_token, const std::string& replace_token)
{
	size_t i = 0;
	while ((i = text.find(find_token)) != string::npos)
		text.replace(i, find_token.size(), replace_token);
}

std::string MReplaceCharsTransformer::transform(void* pParam)
{
	string::size_type pos = 0;
	string out = m_strIn;

	for (MTagManager::ReplaceCharsMap::iterator it = m_Mgr.ReplaceCharsBegin(); it != m_Mgr.ReplaceCharsEnd(); ++it)
	{
		MReplaceChars* pReplaceChars = (*it).second;
		mmcode::replace(out, (*it).first, pReplaceChars->Proc(pParam));
	}

	return (out);
}

void MSpliterCharsTransformer::Splite( list<string>& strlist )
{
	string splitecode = "[tf]";
	string::size_type pos = 0;

	for ( ; pos < m_strIn.length() ; )
	{
		const string::size_type openingBracket = m_strIn.find(splitecode, pos);
		if (openingBracket== string::npos)
		{
			strlist.push_back(string(m_strIn.begin() + pos, m_strIn.end()));
			break;
		}
		strlist.push_back(string(m_strIn.begin() + pos, m_strIn.begin() + openingBracket));
		pos = openingBracket + splitecode.size();
	}
}

mmcode::MMMCodeTransResult MMMCodeTransformer::transform()
{
	MMMCodeTransResult	result;

	string::size_type pos = 0;
	string out;

	out.reserve(m_strIn.length());

	for ( ; pos < m_strIn.length() ; )
	{
		const string::size_type openingBracket = m_strIn.find('[', pos);

		if (openingBracket == string::npos)
		{
			string text = string(m_strIn.begin() + pos, m_strIn.end());
			result.AddText(text);
			out += text;
			break;
		}

		string text = string(m_strIn.begin() + pos, m_strIn.begin() + openingBracket);

		const string::size_type nextOpeningBracket = m_strIn.find('[', openingBracket + 1);
		const string::size_type closingBracket = m_strIn.find(']', openingBracket + 1);

		if (closingBracket == string::npos)
		{
			text += "[";
			result.AddText(text);

			out += text;
			pos = openingBracket + 1;
			continue;
		}

		if (nextOpeningBracket != string::npos &&
			nextOpeningBracket < closingBracket)
		{
			text += "[";
			result.AddText(text);

			out += text;
			pos = openingBracket + 1;
			continue;
		}

		// 태그 이름 추출
		const string::size_type sep = m_strIn.find_first_of("=\t\r\n]", openingBracket + 1);

		if (sep == string::npos)
		{
			text += "[";
			result.AddText(text);

			out += text;
			pos = openingBracket + 1;
			continue;
		}

		const string tagName = cml2::TrimBlanks(string(m_strIn.begin() + openingBracket + 1, m_strIn.begin() + sep));

		// Find possible closing tag
		string::size_type min = string::npos;
		string::size_type closingTag = m_strIn.find("[/" + tagName + "]", closingBracket);

		if (closingTag == string::npos)
		{
			text += "[" + string(m_strIn.begin() + openingBracket + 1, m_strIn.begin() + closingBracket);
			result.AddText(text);

			out += text;
			pos = closingBracket;
			continue;
		}

		if (text.length() > 0)
		{
			out += text;
			result.AddText(text);
		}

		// Extract tag parameter: [tag=param]
		string param((sep != closingBracket)
			? string(m_strIn.begin() + sep + 1, m_strIn.begin() + closingBracket)
			: string(""));
		param = cml2::TrimBlanks(param);

		// Extract tag contents: [tag]contents[/tag]
		const string contents(m_strIn.begin() + closingBracket + 1, m_strIn.begin() + closingTag);

		// Process
		{
			MTag* t = m_Mgr.FindTag(tagName);
			if (t != NULL)
			{
				out += contents;
				result.AddNode(t, param, contents);
			}
		}

		// Continue with the remaining text
		pos = closingTag + 2 + tagName.length() + 1;
	}

	return (result);
}

}	// namespace mmcode


