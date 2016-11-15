
#pragma once



class ZGlue
{
public:
	ZGlue();
	~ZGlue();

	static ZGlue* GetInstance();


	// Glue functions
	void Log( const char* szLog);
	void Reload();
	void ChangeResolution();
	void Profile();
	void Test();
};
