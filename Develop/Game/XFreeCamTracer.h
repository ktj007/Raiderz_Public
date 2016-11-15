#pragma once 



// class : XFreeCamTracer
class XFreeCamTracer
{
private:
	bool				m_bRun;
	vector< vec3>		m_vWayPoints;
	vector< vec3>		m_vSplined;
	float				m_fDistance;
	float				m_fValocity;
	DWORD				m_dwStartTime;



public:
	XFreeCamTracer();
	virtual ~XFreeCamTracer();

	void Run();
	void Stop();
	void Clear();
	void Check();

	void Update();

	bool IsRun() const							{ return m_bRun;						}
	void SetValocity( int nValocity);

	static XFreeCamTracer* GetInstance();
};








// inline : XGetFreeCamTracer
inline XFreeCamTracer* XGetFreeCamTracer()
{
	return XFreeCamTracer::GetInstance();
}
