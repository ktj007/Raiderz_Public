#pragma once

ref class ToolUtil
{
public:
	static public String^ ConvertString(const char* szStr)
	{
		return gcnew String(szStr);
	}

};
