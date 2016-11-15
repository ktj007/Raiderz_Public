#include "gtest/gtest.h"

class NullDeviceEngineCreater
{
public:
	NullDeviceEngineCreater();
	~NullDeviceEngineCreater();
};

class NullDeviceFixture : public testing::Test
{
public:
	NullDeviceFixture()
	{
		m_pNullDeviceEngineCreater = new NullDeviceEngineCreater;
	}

	virtual ~NullDeviceFixture()
	{
		delete m_pNullDeviceEngineCreater;
	}

	void UpdateSceneManager( DWORD dwElapsed = 1 );

private:
	NullDeviceEngineCreater* m_pNullDeviceEngineCreater;
};