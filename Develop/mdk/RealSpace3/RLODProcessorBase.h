#pragma once

namespace rs3
{

class RCameraSceneNode;


// LOD_PROCESSOR_TYPE => LODP
static const DWORD LODP_DISTANCE	= 1;
static const DWORD LODP_FOG			= 1<<1;
static const DWORD LODP_PROJECTION	= 1<<2;


class RS_API RLODProcessorBase
{
private:
	
public:
	RLODProcessorBase();
	virtual ~RLODProcessorBase();

protected:
	DWORD m_dwType;

public:
	void SetTypeID( DWORD dwType)	{ m_dwType = dwType; }
	const DWORD GetTypeID()	{ return m_dwType; }

	virtual int	CalculateLOD( RCameraSceneNode* pCamera, const RSphere* pBoundingSphere) = 0;
};
}