/// @file C_ControlCursor.h
#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <RDeviceD3D.h>
#include "RTerrain.h"
#include "M_ToolSceneManager.h"
#include "MXml.h"

namespace rs3
{
	class RMesh;
	class RActor;
};

class CControlMediator;

/**
	@brief 커서 관리 클래스

	WorkBench 에서 보여줄 커서를 설정하고 보여주는 싱글턴 클래스 입니다.
*/
class CORETOOL_API L3DCursor
{
public:
	enum TYPE
	{
		SPHERE = 0,		// 구
		BOX,			// 상자
		CYLINDER,		// 실린더
		RCONE,			// 거꾸로된 원뿔
		DX_MESH_NUM,	// 여기까지 메시(D3DXMESH)
		AXIS,			// 십자축
		BOX2D,			// 드래그 시 표시되는 상자(선)
		NONE,			// 없음
		NODE,			// RSceneNode 개체
	};
protected:
	L3DCursor();
	~L3DCursor();

	LPD3DXMESH m_pCursorObject[DX_MESH_NUM];
	LPD3DXMESH m_pMesh;

	bool m_bVolumeType;
	D3DXMATRIX matWorld;
	rs3::RVector m_vPos;
	rs3::RVector m_vScale;

	rs3::RToolSceneManager *m_pSceneManager;
	rs3::RSceneNode *m_pCursorNode;

	TYPE m_eType;
	rs3::RDeviceD3D *m_pd3dDevice;
	CControlMediator* m_pControlMediator;

	DWORD m_dwColor;

	void RenderBandMesh(rs3::RDeviceD3D *pd3dDevice);
public:
	void Init(CControlMediator* pControlMediator, rs3::RDeviceD3D *pd3dDevice, rs3::RToolSceneManager *pSceneManager);
	void Release();

	void SetMeshNode(rs3::RSceneNode *pNode);
	void SetTransform(const rs3::RVector &pos, const rs3::RVector &scale);

	void SetType(TYPE eType, bool bVolume);
	TYPE GetType(){ return m_eType; }
	void Render(rs3::RDeviceD3D *pd3dDevice);

	void ClearCursorNode();
	void SetNode(rs3::RSceneNode *pSceneNode);
	void SetNodeFile(const char *szFilename, const char *szNodename);
	rs3::RSceneNode* GetNode() { return m_pCursorNode; }

	rs3::RVector* GetPos()	{	return &m_vPos;		}
	rs3::RVector* GetScale(){	return &m_vScale;	}
	float GetNodeHeight();
	rs3::RVector GetNodeHalfRange();

	rs3::RDeviceD3D *GetDevice()	{	return m_pd3dDevice; }
	void SetColor(DWORD color);

	static L3DCursor* GetInstance()
	{
		static L3DCursor kCursor;
		return &kCursor;
	}
};
