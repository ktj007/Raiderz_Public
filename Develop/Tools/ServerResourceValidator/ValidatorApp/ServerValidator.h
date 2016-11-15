#pragma once

class AddConsoleReporter;


class ServerValidator
{
public:
	enum ReturnValue
	{
		VALIDATOR_SUCCESS = 1,
		VALIDATOR_INITIALIZE_FAILED,
		VALIDATOR_FAILED
	};
public:
	ServerValidator();
	virtual ~ServerValidator();

	ReturnValue Check(void);
	bool Log(const char *pFormat, ...);

private:	
	bool Create(void);
	void Destroy(void);

	void _InitializeDependence(void);
	void _InitalizeConst(void);
	void _FinalizeDependence(void);
	bool _InitializeIo(void);
	void _FinalizeIo(void);

	bool _LoadResource(void);

private:	
	ofstream m_ofStream;
	AddConsoleReporter* m_reporter;
};
