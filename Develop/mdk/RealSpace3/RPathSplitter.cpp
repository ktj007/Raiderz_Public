#include "StdAfx.h"
#include "RPathSplitter.h"

namespace rs3 {

void RPathSplitter::AddPath(const string& strPath)
{
	if (strPath.length() <= 0) return;

	// 마지막에 / (없으면) 추가
	if(strPath[strPath.length()-1]!='/')
		m_strPathList.push_back(strPath+"/");
	else
		m_strPathList.push_back(strPath);
}

void RPathSplitter::SetPath(const string& strPath)
{
	m_strPathList.clear();

	string::size_type i = 0;

	string strTexturePath = strPath;

	// \ 를 / 로 대체한다
	while((i=strTexturePath.find("\\",i)) != -1)
	{
		strTexturePath[i]='/';
	}

	// ; 마다 끊어서 pathlist에 넣는다
	i = 0;
	string::size_type lastSemicolon = 0;
	string path;
	while((i=strTexturePath.find(";",i))!=-1)
	{
		path = strTexturePath.substr(lastSemicolon,i-lastSemicolon);
		AddPath(path);
		i++;
		lastSemicolon = i;
	}

	// 마지막에 ;이 없어도 남은것이 있으면 넣는다.
	path = strTexturePath.substr(lastSemicolon);
	if(!path.empty())
		AddPath(path);

	// 형식에 맞도록 재처리된 합쳐진 패스 기억
	m_strSourcePath.clear();
	for( size_t i=0; i<GetPathCount(); i++ )
	{
		m_strSourcePath+= GetPath(i);
		if( i!= GetPathCount()-1 )
			m_strSourcePath += ";";
	}
}

}