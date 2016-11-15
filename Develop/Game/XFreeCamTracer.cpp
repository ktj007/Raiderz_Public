#include "stdafx.h"
#include "XFreeCamTracer.h"
#include "MMath.h"
#include "XCameraManager.h"



XFreeCamTracer::XFreeCamTracer()
{
	m_bRun = false;
	m_fValocity = 1000.0f;
	m_dwStartTime = 0;
}


XFreeCamTracer::~XFreeCamTracer()
{
	Clear();
}


void XFreeCamTracer::Run()
{
	if ( m_vWayPoints.size() < 2)
		return;


	m_vSplined.clear();
	m_fDistance = 0.0;

	vector< vec3> _waypoints;
	_waypoints.push_back( m_vWayPoints[ 0]);

	size_t _size = m_vWayPoints.size();
	for ( size_t i = 0;  i < ( _size - 1);  i++)
	{
		vec3 _p1 = m_vWayPoints[ i];
		vec3 _p2 = m_vWayPoints[ i + 1];
		vec3 _len( _p2.x - _p1.x, _p2.y - _p1.y, _p2.z - _p1.z);

		m_fDistance += sqrt( _len.x * _len.x  +  _len.y * _len.y  +  _len.z * _len.z);

		_waypoints.push_back( m_vWayPoints[ i]);
	}

	_waypoints.push_back( m_vWayPoints[ _size - 1]);
	_waypoints.push_back( m_vWayPoints[ _size - 1]);


	int _resolution = max( 5, (int)( m_fDistance / 500.0f /* 0.5 m */));
	MMath::CardinalInterpoltation( _waypoints, 0.7f, _resolution, m_vSplined);

	m_bRun = true;
	m_dwStartTime = timeGetTime();
}


void XFreeCamTracer::Stop()
{
	m_bRun = false;
	m_vSplined.clear();
}


void XFreeCamTracer::Clear()
{
	m_vWayPoints.clear();
}


void XFreeCamTracer::Check()
{
	if ( gg.game == NULL)
		return;


	if ( global.camera  &&  global.camera->GetPrimaryCamera())
	{
		RCameraSceneNode* pCam = global.camera->GetPrimaryCamera();

		vec3 _pos = pCam->GetWorldPosition();
		m_vWayPoints.push_back( _pos);
	}
}


void XFreeCamTracer::Update()
{
	if ( gg.game == NULL  ||  m_bRun == false  ||  global.camera == NULL  ||  global.camera->GetPrimaryCamera() == NULL)
		return;
	
	
	RCameraSceneNode* pCam = global.camera->GetPrimaryCamera();

	DWORD _elapsed = timeGetTime() - m_dwStartTime;
	size_t _size = m_vSplined.size();
	double _t = (double)m_fDistance / ( (double)m_fValocity * 0.001 * (double)_size);
	int _spline = (int)( (double)_elapsed / _t);
	if ( _spline >= (int)( _size - 1))
	{
		pCam->SetPosition( m_vSplined[ _size - 1]);

		m_vSplined.clear();
		m_bRun = false;
		return;
	}

	double _ratio = ( (double)_elapsed - (double)_spline * _t) / _t;
	vec3 _p1 = m_vSplined[ _spline];
	vec3 _p2 = m_vSplined[ _spline + 1];
	vec3 _p3 = ( _spline < (int)( _size - 2))  ?  m_vSplined[ _spline + 2]  :  m_vSplined[ _spline + 1];
	vec3 _dir1( _p2.x - _p1.x, _p2.y - _p1.y, _p2.z - _p1.z);
	vec3 _dir2( _p3.x - _p2.x, _p3.y - _p2.y, _p3.z - _p2.z);
	vec3 _pos( _p1.x + _dir1.x * _ratio, _p1.y + _dir1.y * _ratio, _p1.z + _dir1.z * _ratio);

	_dir1.Normalize();
	_dir1 *= 1.0 - _ratio;
	_dir2.Normalize();
	_dir2 *= _ratio;

	vec3 _dir = _dir1 + _dir2;
	_dir.z *= 0.5f;
	_dir.Normalize();

	pCam->SetTransform( _pos, _dir);
}


void XFreeCamTracer::SetValocity( int nValocity)
{
	m_fValocity = (float)nValocity;
	Stop();
}


XFreeCamTracer* XFreeCamTracer::GetInstance()
{
	static XFreeCamTracer instance;
	return &instance;
}
