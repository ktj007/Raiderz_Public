#ifndef _THELPER_H
#define _THELPER_H

#pragma once
#include "stdafx.h"

class TBaseHelper
{
public:
	TBaseHelper()			{}
	virtual ~TBaseHelper()	{}

	static string			GetFileName(string strFullPathFileName)
	{
		int token_path				= strFullPathFileName.rfind("/") + 1;

		return strFullPathFileName.substr(token_path, strFullPathFileName.size() - token_path);

	}

	static string			GetFileNameNoExtension(string strFullPathFileName)
	{
		string strFileName			= GetFileName(strFullPathFileName);

		return strFileName.substr(0, strFileName.find("."));
	}
};



#endif