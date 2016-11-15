#include "StdAfx.h"
#include "GLuaThisEnchanter.h"

using namespace stdext;

bool GLuaThisEnchanter::ConvertFromFile( string strFilename, string& outstrBuffer )
{
	outstrBuffer = "";

	std::ifstream ifs;
	ifs.open(strFilename.c_str());

	if (ifs == NULL)
		return false;

	char buf[256];
	while(!ifs.eof() && !ifs.fail())
	{
		ifs.getline(buf, 256);
		outstrBuffer.append(ConvertLine(buf));
		outstrBuffer.append("\n");
	}

	const char* pszTest = outstrBuffer.c_str();

	ifs.close();

	return true;
}

string GLuaThisEnchanter::ConvertLine( string strLine )
{
	if (!IsMatchMemFunc(strLine))
		return strLine;	// 해당되는 멤버함수 없음

	if (!IsDeclFuncLine(strLine))
		return strLine;	// 함수 선언하는 문자열이 아님

	if (!HasColon(strLine))			
		return strLine;	// 콜론이 없으면 멤버함수가 아니므로

	size_t nStart = strLine.find('(');
	size_t nEnd =	strLine.find(')');
	if (nStart == string::npos)	return strLine;
	if (nEnd == string::npos)	return strLine;
	
	bool bWithArgument;
	if (!HasComma(strLine))
	{
		bWithArgument = false;
		for (size_t i=nStart+1; i<nEnd-1; ++i)
		{
			if (strLine.at(i) != ' ')
			{
				bWithArgument = true;
				break;
			}
		}
	}
	else
	{
		bWithArgument = true;
	}
	
	strLine.insert(nStart+1, bWithArgument?"this, ":"this");

	return strLine;
}

void GLuaThisEnchanter::AddMatchMemFunc( string strName )
{
	m_vecMatchMemFuncs.push_back(strName);
}

bool GLuaThisEnchanter::IsMatchMemFunc(string strLine)
{
	for each (const string& each in m_vecMatchMemFuncs)
	{
		if (strLine.find(each) != string::npos)
			return true;
	}

	return false;
}

bool GLuaThisEnchanter::IsDeclFuncLine(string strLine)
{
	return strLine.find("function") != string::npos;
}

bool GLuaThisEnchanter::HasComma(string strLine)
{
	return strLine.find(',') != string::npos;
}

bool GLuaThisEnchanter::HasColon(string strLine)
{
	return strLine.find(':') != string::npos;
}

bool GLuaThisEnchanter::SaveToFile(string strFileName, const string& strText)
{
	std::ofstream ofs;
	ofs.open(strFileName.c_str());

	if (ofs == NULL)
		return false;
	
	ofs.write(strText.c_str(), (std::streamsize)strText.length());

	ofs.close();

	return true;
}


bool GLuaThisEnchanter::ConvertFromStream(string strStream, string& outstrBuffer)
{
	if (strStream.empty())
		return false;

	char	szLine[256];
	size_t	nHead;
	size_t	nEnd;
	size_t	nPos = 0;

	while(true)
	{
		nHead	= nPos;
		nPos	= strStream.find("\n", nPos);
		nEnd	= nPos;

		if (string::npos == nEnd)
			nEnd = strStream.length();

		_ASSERT(256 > (nEnd - nHead));
		if (256 <= (nEnd - nHead))
			return false;

		copy(&strStream[nHead], &strStream[nEnd], checked_array_iterator<char *>(szLine, 256));

		szLine[nEnd - nHead] = '\0';	

		outstrBuffer.append(ConvertLine(szLine));
		outstrBuffer.append("\n");

		if (string::npos == nPos)
			break;

		++nPos;
	}
	
	return true;
}
