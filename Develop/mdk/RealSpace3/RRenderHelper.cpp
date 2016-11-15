#include "StdAfx.h"
#include "RRenderHelper.h"
#include "RVType.h"
#include "MOBox.h"
#include "RFont.h"
#include "RSUtil.h"

namespace rs3 {

RRenderHelper::RBatchLineDrawer RRenderHelper::m_batchLineDrawer;

void RRenderHelper::SetRenderState(bool bWireframe)
{
	REngine::GetDevice().SetTexture(0, R_NONE);
	REngine::GetDevice().SetTexture(1, R_NONE);
	REngine::GetDevice().SetBlending( RBLEND_ONE, RBLEND_NONE);

	REngine::GetDevice().SetLighting(false);
	REngine::GetDevice().ShaderOff();

	REngine::GetDevice().SetAlphaTestEnable(false);

	REngine::GetDevice().SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
	REngine::GetDevice().SetTextureStageState(0, RTSS_COLORARG1, RTA_TFACTOR);
	REngine::GetDevice().SetTextureStageState(0, RTSS_ALPHAOP, RTOP_SELECTARG1);
	REngine::GetDevice().SetTextureStageState(0, RTSS_ALPHAARG1, RTA_TFACTOR);
	REngine::GetDevice().SetTextureStageState(1, RTSS_COLOROP, RTOP_DISABLE);
	REngine::GetDevice().SetTextureStageState(1, RTSS_ALPHAOP, RTOP_DISABLE);
	REngine::GetDevice().SetCullMode(RCULL_NONE);
}

void RRenderHelper::EndRenderState()
{
	REngine::GetDevice().SetTexture(0, R_NONE);
	REngine::GetDevice().SetTexture(1, R_NONE);
	REngine::GetDevice().SetBlending(RBLEND_NONE);
	REngine::GetDevice().SetCullMode(RCULL_CW);

	REngine::GetDevice().SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
	REngine::GetDevice().SetTextureStageState(0, RTSS_COLORARG1, RTA_DIFFUSE);
	REngine::GetDevice().SetTextureStageState(0, RTSS_ALPHAOP, RTOP_SELECTARG1);
	REngine::GetDevice().SetTextureStageState(0, RTSS_ALPHAARG1, RTA_DIFFUSE);
	REngine::GetDevice().SetTextureStageState(1, RTSS_COLOROP, RTOP_DISABLE);
	REngine::GetDevice().SetTextureStageState(1, RTSS_ALPHAOP, RTOP_DISABLE);
}

void RRenderHelper::RenderPlane(const RMatrix &matWorld, RVector3 AxisX, RVector3 AxisY, float width, float height
								, RTexture *pTexture)
{
	RVt_pos	Vertices[4];
	Vertices[0].vPos = AxisX * - width + AxisY * -height;
	Vertices[1].vPos = AxisX * + width + AxisY * -height;
	Vertices[2].vPos = AxisX * - width + AxisY * +height;
	Vertices[3].vPos = AxisX * + width + AxisY * +height;

	REngine::GetDevice().SetTexture(0, pTexture);
	REngine::GetDevice().SetTransform(RST_WORLD, matWorld);
	REngine::GetDevice().SetFvF( RVt_pos::FVF );

	REngine::GetDevice().DrawPrimitiveUP( RPT_TRIANGLESTRIP, 2,(void*)Vertices, sizeof(RVt_pos));
}

void RRenderHelper::RenderBox(const RMatrix &matWorld, const RBoundingBox &box, DWORD dwColor, bool bLine)
{
	const static RVector baseVertices[] = 
	{
		RVector(-1, 1, 1),
		RVector( 1, 1, 1),
		RVector(-1,-1, 1),
		RVector( 1,-1, 1),
		RVector(-1, 1,-1),
		RVector( 1, 1,-1),
		RVector(-1,-1,-1),
		RVector( 1,-1,-1)
	}; // 8개 vertex

	const static WORD IndexArray[] =
	{
		0, 1, 1, 3, 3, 2, 2, 0
		,0, 4, 1, 5, 2, 6, 3, 7
		,4, 5, 5, 7, 7, 6, 6, 4
	}; // 12개 line

	const static WORD PolygonIndexArray[] =
	{
		6, 5, 4, 7, 5, 6,	//front
		1, 2, 0, 3, 2, 1,	//back
		4, 1, 0, 4, 5, 1,	//top
		3, 6, 2, 7, 6, 3,	//bottom
		7, 3 ,1, 5, 7, 1,	//left
		2, 4, 0, 6, 4, 2,	//right
	};



	RVt_pos	Vertices[8];

	//		0						1
	//      -----------------------/
	//	   /|                     /|
	//	  / |                    / |
	//	4/----------------------/5 |
	//   |  |                   |  |
	//   |  |                   |  |
	//   |  |                   |  |
	//   |  |                   |  |
	//   |  |2                  |  |
	//   | /--------------------|--/3
	//   |/                     | /
	//   ------------------------/
	//   6						7

	for ( int i = 0; i<8; ++i )
	{
		RVector vec;
		vec.x = ( baseVertices[i].x < 0 ? box.vmin.x : box.vmax.x );
		vec.y = ( baseVertices[i].y < 0 ? box.vmin.y : box.vmax.y );
		vec.z = ( baseVertices[i].z < 0 ? box.vmin.z : box.vmax.z );
//		localMatrix.TransformVect(vec,Vertices[i].vPos);
		Vertices[i].vPos = vec;
//		Vertices[i].dwColor = dwColor;
	}

	REngine::GetDevice().SetTextureFactor(dwColor);
	REngine::GetDevice().SetTransform( RST_WORLD, matWorld);
	REngine::GetDevice().SetFvF( RVt_pos::FVF );

	if(bLine)
		REngine::GetDevice().DrawIndexedPrimitiveUP( RPT_LINELIST, 0, 8, 12,(void*)IndexArray, (void*)Vertices, sizeof(RVt_pos));
	else
		REngine::GetDevice().DrawIndexedPrimitiveUP( RPT_TRIANGLELIST, 0, 8, 12,(void*)PolygonIndexArray, (void*)Vertices, sizeof(RVt_pos));
}

void RRenderHelper::RenderBox( const RMatrix &matWorld, float _fBoxSize, DWORD dwColor /*= 0xffffffff*/, bool bLine /*= true*/ )
{
	RBoundingBox box;
	box.vmax.Set(_fBoxSize, _fBoxSize, _fBoxSize);
	box.vmin.Set( -_fBoxSize, -_fBoxSize, -_fBoxSize);

	RRenderHelper::RenderBox(matWorld, box, dwColor, bLine);
}

void RRenderHelper::RenderObb(const MOBox &obb, DWORD dwColor /* = 0xffffffff */)
{
	const static RVector baseVertices[] = 
	{
		RVector(-1, 1, 1),
		RVector( 1, 1, 1),
		RVector(-1,-1, 1),
		RVector( 1,-1, 1),
		RVector(-1, 1,-1),
		RVector( 1, 1,-1),
		RVector(-1,-1,-1),
		RVector( 1,-1,-1)
	}; // 8개 vertex

	const static WORD IndexArray[] =
	{
		0, 1, 1, 3, 3, 2, 2, 0
		,0, 4, 1, 5, 2, 6, 3, 7
		,4, 5, 5, 7, 7, 6, 6, 4
	}; // 12개 line

	RVt_pos	Vertices[8];
	
	RVector vXAxis = obb.vAxis[0]*obb.vRange;
	RVector vYAxis = obb.vAxis[1]*obb.vRange;
	RVector vZAxis = obb.vAxis[2]*obb.vRange;

	for ( int i = 0; i<8; ++i )
	{
		RVector vec;
		vec.x = DotProduct(baseVertices[i], vXAxis);
		vec.y = DotProduct(baseVertices[i], vYAxis);
		vec.z = DotProduct(baseVertices[i], vZAxis);

		Vertices[i].vPos = vec + obb.vPosition;
	}

	RDevice *pDevice = REngine::GetDevicePtr();

	pDevice->SetTextureFactor(dwColor);
	pDevice->SetTransform( RST_WORLD, RMatrix::IDENTITY);
	pDevice->SetFvF( RVt_pos::FVF );
	pDevice->DrawIndexedPrimitiveUP( RPT_LINELIST, 0, 8, 12,(void*)IndexArray, (void*)Vertices, sizeof(RVt_pos));
}

void RRenderHelper::RenderSphere(const RSphere &sphere, DWORD dwColor /* = 0xffffffff */)
{
	RMatrix mat;
	mat.SetTranslationMatrix(sphere.center);
	RenderSphere(mat, sphere.radius, dwColor);
}

void RRenderHelper::RenderSphere(const RMatrix &matWorld, float fRadius, DWORD dwColor)
{
	const int SPHERE_SEGMENT = 20;

	static bool bInitialized = false;
	static RVt_pos	m_sphereVertices[SPHERE_SEGMENT*SPHERE_SEGMENT*4];

	if(!bInitialized) {
		RVt_pos	*pVer = m_sphereVertices;
		for(int i=0;i<SPHERE_SEGMENT;i++)
		{
			float fAngleZ=i*2*MMath::PI/(float)SPHERE_SEGMENT;
			float fAngleZ2=(i+1)*2*MMath::PI/(float)SPHERE_SEGMENT;
			for(int j=0;j<SPHERE_SEGMENT;j++)
			{
				float fAngle=j*2*MMath::PI/(float)SPHERE_SEGMENT;
				float fAngle2=(j+1)*2*MMath::PI/(float)SPHERE_SEGMENT;

				RVector a=RVector(cos(fAngle)*cos(fAngleZ),sin(fAngle)*cos(fAngleZ),sin(fAngleZ));
				RVector b=RVector(cos(fAngle2)*cos(fAngleZ),sin(fAngle2)*cos(fAngleZ),sin(fAngleZ));
				pVer->vPos = a; pVer++;
				pVer->vPos = b; pVer++;

				b=RVector(cos(fAngle)*cos(fAngleZ2),sin(fAngle)*cos(fAngleZ2),sin(fAngleZ2));

				pVer->vPos = a; pVer++;
				pVer->vPos = b; pVer++;
			}
		}
	}

	RMatrix matScale;
	matScale.SetScaleMatrix(RVector(fRadius,fRadius,fRadius));
	REngine::GetDevice().SetTransform(RST_WORLD,matScale * matWorld);

	REngine::GetDevice().SetTextureFactor(dwColor);
	REngine::GetDevice().SetFvF(RVt_pos::FVF);
	REngine::GetDevice().DrawPrimitiveUP(RPT_LINELIST,SPHERE_SEGMENT*SPHERE_SEGMENT*2,m_sphereVertices,sizeof(RVt_pos));
}

void RRenderHelper::RenderCorn(const RVector& pos, const RVector& dir, float fRadius, float fHeight, DWORD dwColor)
{
	RMatrix matWorld;
	RVector right = fabs(DotProduct(dir,RVector(0,0,1)))>0.99f ? RVector(1,0,0) : RVector(0,0,1);
	RVector up = CrossProduct(right,dir);
	matWorld.SetLocalMatrix(pos, up, dir );
	RenderCorn(matWorld,fRadius,fHeight,dwColor);
}

void RRenderHelper::RenderCorn(const RMatrix &matWorld, float fRadius, float fHeight, DWORD dwColor)
{
	REngine::GetDevice().SetFvF(RVt_pos::FVF);

	const int CYLINDER_SEGMENT = 40;

	static bool bInit = false;
	static RVt_pos	cylinderVertices[CYLINDER_SEGMENT+1];
	if(!bInit)
	{
		bInit = true;
		cylinderVertices[0].vPos = RVector( 0,0,1 );	// 꼭지점
		for( int i=0; i<CYLINDER_SEGMENT; i++ )
		{
			float theta = (2*MMath::PI*i)/(CYLINDER_SEGMENT-1);
			cylinderVertices[i+1].vPos= RVector( sinf(theta), cosf(theta), 0);
		}
	}

	RMatrix matTM;
	matTM.SetScaleMatrix(fRadius,fRadius,fHeight);
	matTM*=matWorld;
	REngine::GetDevice().SetTransform(RST_WORLD,matTM);
	REngine::GetDevice().SetTextureFactor(dwColor);

	REngine::GetDevice().SetBlending( RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
	REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLEFAN, CYLINDER_SEGMENT-1, cylinderVertices, sizeof(RVt_pos));
	REngine::GetDevice().SetBlending( RBLEND_NONE, RBLEND_NONE);
}


void RRenderHelper::RenderCylinder(const RVector& pos, const RVector& dir, float fRadius, float fHeight, DWORD dwColor)
{
	RMatrix matWorld;
	RVector right = fabs(DotProduct(dir,RVector(0,0,1)))>0.99f ? RVector(1,0,0) : RVector(0,0,1);
	RVector up = CrossProduct(right,dir);
	matWorld.SetLocalMatrix(pos, up, dir );
	RenderCylinder(matWorld,fRadius,fHeight,dwColor);
}

void RRenderHelper::RenderCylinder(const RMatrix &matWorld, float fRadius, float fHeight, DWORD dwColor, bool bLine)
{
	REngine::GetDevice().SetFvF(RVt_pos::FVF);

	const int CYLINDER_SEGMENT = 40;

	static bool bInit = false;
	static RVt_pos cylinderVertices[CYLINDER_SEGMENT*2];
	if(!bInit)
	{
		bInit = true;
		for( int i=0; i<CYLINDER_SEGMENT; i++ )
		{
			float theta = (2*MMath::PI*i)/(CYLINDER_SEGMENT-1);
			cylinderVertices[2*i+0].vPos= RVector( sinf(theta), cosf(theta), 0);
			cylinderVertices[2*i+1].vPos= RVector( sinf(theta), cosf(theta), 1);
		}
	}

	RMatrix matTM;
	matTM.SetScaleMatrix(fRadius,fRadius,fHeight);
	matTM*=matWorld;
	REngine::GetDevice().SetTransform(RST_WORLD,matTM);
	REngine::GetDevice().SetTextureFactor(dwColor);

	REngine::GetDevice().SetBlending( RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
	if(bLine)
		REngine::GetDevice().DrawPrimitiveUP(RPT_LINESTRIP, CYLINDER_SEGMENT*2-2, cylinderVertices, sizeof(RVt_pos));
	else
	{
		REngine::GetDevice().SetCullMode(RCULL_CCW);
		REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLESTRIP, CYLINDER_SEGMENT*2-2, cylinderVertices, sizeof(RVt_pos));
		REngine::GetDevice().SetCullMode(RCULL_CW);
		REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLESTRIP, CYLINDER_SEGMENT*2-2, cylinderVertices, sizeof(RVt_pos));
	}

	REngine::GetDevice().SetBlending(RBLEND_NONE);
}
/*
theta = 0:0.1:pi/3;

phi = 0:0.1:pi/2;

[T, P] = meshgrid(theta, phi);

F = sin(T);

X = F .* sin(T) .* cos(P)

Y = F .* sin(T) .* sin(P)

Z = F .* cos(T);

mesh(X, Y, Z);*/

void RRenderHelper::RenderDonut(const RVector& pos, const RVector& dir, float fRadius, float fHeight, DWORD dwColor)
{
	RMatrix matWorld;
	RVector right = fabs(DotProduct(dir,RVector(0,0,1)))>0.99f ? RVector(1,0,0) : RVector(0,0,1);
	RVector up = CrossProduct(right,dir);
	matWorld.SetLocalMatrix(pos, up, dir );
	RenderDonut(matWorld,fRadius,fHeight,dwColor);
}

// TODO: 우선 도넛 그리는 공식을 완성하고 그걸 바탕으로 파티클생성포지션을 구하자,
void RRenderHelper::RenderDonut(const RMatrix &matWorld, float fRadius, float fHeight, DWORD dwColor, bool bLine)
{
	REngine::GetDevice().SetFvF(RVt_pos::FVF);

	const int DONUT_SEGMENT = 40;

	static bool bInit = false;
	static RVt_pos donutVertices[DONUT_SEGMENT*DONUT_SEGMENT];
	if(!bInit)
	{
		bInit = true;
		//for( int j = 0; j < DONUT_SEGMENT; j++)
		//{
		//	for( int i=0; i<DONUT_SEGMENT; i++ )
		//	{
		//		float theta = (MMath::PI*i)/(DONUT_SEGMENT);
		//		donutVertices[j*DONUT_SEGMENT+i].vPos= RVector( sinf(theta)*sinf(theta)*cosf((MMath::PI*j*2)/(DONUT_SEGMENT)),
		//			sinf(theta)*sinf(theta)*sinf((MMath::PI*j*2)/(DONUT_SEGMENT)), sinf(theta)*cosf(theta));
		//	}
		//}

		RVector pos = RVector(0.f, 0.f, 0.f);
		RVector n;
		for( int i=0; i<DONUT_SEGMENT; i+=2 )
		{
			for( int j = 0; j < DONUT_SEGMENT; j++)
			{
				float theta = (MMath::PI*i)/(DONUT_SEGMENT-1) * 2;
				donutVertices[j+DONUT_SEGMENT*(i+0)].vPos= RVector( sinf(theta)*sinf(theta)*cosf((MMath::PI*j*2)/(DONUT_SEGMENT-1)),
					sinf(theta)*sinf(theta)*sinf((MMath::PI*j*2)/(DONUT_SEGMENT-1)),
					sinf(theta)*cosf(theta) );
				donutVertices[j+DONUT_SEGMENT*(i+1)].vPos= RVector( sinf(theta)*sinf(theta)*cosf((MMath::PI*(j+1)*2)/(DONUT_SEGMENT-1)),
					sinf(theta)*sinf(theta)*sinf((MMath::PI*(j+1)*2)/(DONUT_SEGMENT-1)),
					sinf(theta)*cosf(theta) );
			}
		}
	}

	RMatrix matTM;
	matTM.SetScaleMatrix(fRadius,fRadius,fHeight);
	matTM*=matWorld;
	REngine::GetDevice().SetTransform(RST_WORLD,matTM);
	REngine::GetDevice().SetTextureFactor(dwColor);

	REngine::GetDevice().SetBlending( RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
	if(bLine)
		REngine::GetDevice().DrawPrimitiveUP(RPT_LINESTRIP, DONUT_SEGMENT*DONUT_SEGMENT, donutVertices, sizeof(RVt_pos));
	else
		REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLESTRIP, DONUT_SEGMENT*DONUT_SEGMENT, donutVertices, sizeof(RVt_pos));
	REngine::GetDevice().SetBlending(RBLEND_NONE);
}

void RRenderHelper::RenderCircle(const RMatrix &matWorld, float fRadius, DWORD dwColor, bool bRenderCross)
{
	const int DONUT_SEGMENT = 40;

	float d = 1.f;

	static bool bInit = false;
	static RVector donutVertices[DONUT_SEGMENT];
	static RVector crossVertices[4] = {
		RVector( d * sin(0*MMath::PI), 0.f, d * cos(0*MMath::PI)),
		RVector( d * sin(MMath::PI), 0.f,	d * cos(MMath::PI)),
		RVector( d * sin(0.5f*MMath::PI), 0.f,	d * cos(0.5f*MMath::PI)),
		RVector( d * sin(1.5f*MMath::PI), 0.f,	d * cos(1.5f*MMath::PI))
	};
	if(!bInit)
	{
		bInit = true;
		for( int i=0; i<DONUT_SEGMENT; i++ )
		{
			float theta = (2*MMath::PI*i)/(DONUT_SEGMENT-1);
			donutVertices[i] = RVector( d * sin(theta), 0.f, d * cos(theta));
		}
	}

	RMatrix matTM;
	matTM.SetScaleMatrix(fRadius,fRadius,fRadius);
	matTM*=matWorld;
	REngine::GetDevice().SetTransform(RST_WORLD,matTM);
	REngine::GetDevice().SetTextureFactor(dwColor);

	REngine::GetDevice().DrawPrimitiveUP(RPT_LINESTRIP, DONUT_SEGMENT-1, donutVertices, sizeof(RVt_pos));

	if(bRenderCross)
	{
		REngine::GetDevice().DrawPrimitiveUP(RPT_LINELIST, 2, crossVertices, sizeof(RVt_pos));
	}
}

void RRenderHelper::RenderRing(const RMatrix &matWorld, float fRadius, float fHeight, DWORD dwColor, bool bLine)
{
	REngine::GetDevice().SetFvF(RVt_pos::FVF);

	const int DONUT_SEGMENT = 40;

	float d = 1.f;
	float tick = 0.5f;

	static bool bInit = false;
	static RVt_pos donutVertices[DONUT_SEGMENT*2];
	if(!bInit)
	{
		bInit = true;
		for( int i=0; i<DONUT_SEGMENT; i++ )
		{
			float theta = (2*MMath::PI*i)/(DONUT_SEGMENT-1);
			donutVertices[2*i+0].vPos= RVector( d * sin(theta)
				, d * cos(theta)
				, 0.f);
			//donutVertices[2*i+1].vPos= RVector( d * sin(theta)
			//	, d * cos(theta)
			//	, 1.f);
			donutVertices[2*i+1].vPos= RVector( (d-tick) * sin(theta)
				, (d-tick) * cos(theta)
				, 0.f);
		}
	}

	RMatrix matTM;
	matTM.SetScaleMatrix(fRadius,fRadius,fHeight);
	matTM*=matWorld;
	REngine::GetDevice().SetTransform(RST_WORLD,matTM);
	REngine::GetDevice().SetTextureFactor(dwColor);

	REngine::GetDevice().SetBlending( RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
	if(bLine)
		REngine::GetDevice().DrawPrimitiveUP(RPT_LINESTRIP, DONUT_SEGMENT*2-2, donutVertices, sizeof(RVt_pos));
	else
		REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLESTRIP, DONUT_SEGMENT*2-2, donutVertices, sizeof(RVt_pos));
	REngine::GetDevice().SetBlending(RBLEND_NONE);
}

void RRenderHelper::RenderCapsule(const RMatrix &matWorld, float fRadius, float fHeight, DWORD dwColor, bool bLine)
{
	REngine::GetDevice().SetFvF(RVt_pos::FVF);

	const int CYLINDER_SEGMENT = 20;

	static bool bInit = false;
	static RVt_pos cylinderVertices[CYLINDER_SEGMENT*2];
	if(!bInit)
	{
		bInit = true;
		for( int i=0; i<CYLINDER_SEGMENT; i++ )
		{
			float theta = (2*MMath::PI*i)/(CYLINDER_SEGMENT-1);
			cylinderVertices[2*i+0].vPos= RVector( sinf(theta), cosf(theta), 0);
			cylinderVertices[2*i+1].vPos= RVector( sinf(theta), cosf(theta), 1);
		}
	}

	RMatrix matTM;
	matTM.SetScaleMatrix(fRadius,fRadius,fHeight);
	matTM*=matWorld;
	REngine::GetDevice().SetTransform(RST_WORLD,matTM);
	REngine::GetDevice().SetTextureFactor(dwColor);
	REngine::GetDevice().SetBlending( RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);

	REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLESTRIP, CYLINDER_SEGMENT*2-2, cylinderVertices, sizeof(RVt_pos));

	const int SPHERE_SEGMENT = 20;
	static bool bInitialized = false;
	static vector<RVt_pos>	m_sphereVertices1;
	static vector<RVt_pos>	m_sphereVertices2;

	if(!bInitialized) {
		RVt_pos	*pVer;
		for(int i=0;i<SPHERE_SEGMENT/2;i++)
		{
			float fAngleZ=i*2*MMath::PI/(float)SPHERE_SEGMENT;
			float fAngleZ2=(i+1)*2*MMath::PI/(float)SPHERE_SEGMENT;
			for(int j=0;j<SPHERE_SEGMENT/2+1;j++)
			{
				m_sphereVertices1.push_back(RVt_pos());
				pVer = &(m_sphereVertices1.back());
				float fAngle=j*2*MMath::PI/(float)SPHERE_SEGMENT;
				RVector a=RVector(cos(fAngle)*cos(fAngleZ),sin(fAngle)*cos(fAngleZ),sin(fAngleZ));
				pVer->vPos = a;

				if (j == 0)
				{
					m_sphereVertices1.push_back(RVt_pos());
					pVer = &(m_sphereVertices1.back());
					pVer->vPos = a;
				}
	
				m_sphereVertices1.push_back(RVt_pos());
				pVer = &(m_sphereVertices1.back());

				a=RVector(cos(fAngle)*cos(fAngleZ2),sin(fAngle)*cos(fAngleZ2),sin(fAngleZ2));
				pVer->vPos = a;
			}
			m_sphereVertices1.push_back(m_sphereVertices1.back());
		}
		for(int i=SPHERE_SEGMENT/2;i<SPHERE_SEGMENT;i++)
		{
			float fAngleZ=i*2*MMath::PI/(float)SPHERE_SEGMENT;
			float fAngleZ2=(i+1)*2*MMath::PI/(float)SPHERE_SEGMENT;
			for(int j=0;j<SPHERE_SEGMENT/2+1;j++)
			{
				m_sphereVertices2.push_back(RVt_pos());
				pVer = &(m_sphereVertices2.back());
				float fAngle=j*2*MMath::PI/(float)SPHERE_SEGMENT;
				RVector a=RVector(cos(fAngle)*cos(fAngleZ),sin(fAngle)*cos(fAngleZ),sin(fAngleZ));
				pVer->vPos = a;

				if (j == 0)
				{
					m_sphereVertices2.push_back(RVt_pos());
					pVer = &(m_sphereVertices2.back());
					pVer->vPos = a;
				}

				m_sphereVertices2.push_back(RVt_pos());
				pVer = &(m_sphereVertices2.back());

				a=RVector(cos(fAngle)*cos(fAngleZ2),sin(fAngle)*cos(fAngleZ2),sin(fAngleZ2));
				pVer->vPos = a;
			}
			m_sphereVertices2.push_back(m_sphereVertices2.back());
		}
		bInitialized = true;
	}

	matTM.SetScaleMatrix(fRadius,fRadius,fRadius);
	RMatrix tm;
	tm.SetTranslationMatrix(0, 0, fHeight);
	matTM *= tm;
	matTM *= matWorld;

	REngine::GetDevice().SetTransform(RST_WORLD,matTM);

	REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLESTRIP,m_sphereVertices1.size() - 2,&(m_sphereVertices1[0]),sizeof(RVt_pos));

	matTM.SetScaleMatrix(fRadius,fRadius,fRadius);
	matTM*=matWorld;
	REngine::GetDevice().SetTransform(RST_WORLD,matTM);

	REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLESTRIP,m_sphereVertices2.size() - 2,&(m_sphereVertices2[0]),sizeof(RVt_pos));


	REngine::GetDevice().SetBlending(RBLEND_NONE);
}

void RRenderHelper::RenderCapsule(const RMatrix &matWorld, const RCapsule& c, DWORD dwColor)
{
	RVector b, t;
	matWorld.TransformVect(c.bottom, b);
	matWorld.TransformVect(c.top, t);
	
	RCapsule tmp = RCapsule(b, t, c.radius);

	RenderCapsule(tmp, dwColor);
}

void RRenderHelper::RenderCapsule(const RCapsule& c, DWORD dwColor)
{
	RMatrix matWorld;

	RVector dir = c.top - c.bottom;
	if (dir.Length() <= FLT_EPSILON)
	{
		dir = RVector::AXISZ;
	}
	dir.Normalize();
	RVector right = fabs(DotProduct(dir,RVector(0,0,1)))>0.99f ? RVector(1,0,0) : RVector(0,0,1);
	RVector up = CrossProduct(right,dir);
	matWorld.SetLocalMatrix(c.bottom, up, dir );
	RenderCapsule(matWorld,c.radius,c.height,dwColor);
}

void RRenderHelper::RenderGrid(const RMatrix &matWorld, float fSegmentSize, int nSegmentCountX, int nSegmentCountY, DWORD dwColor)
{
	const int MAXSEGMENT = 384;

	if(nSegmentCountX>MAXSEGMENT) nSegmentCountX = 100;
	if(nSegmentCountY>MAXSEGMENT) nSegmentCountY = 100;

	RVt_pos lineVertices[(MAXSEGMENT+1)*2];

	// ASSERTION 추가
	_ASSERT(MAXSEGMENT>=nSegmentCountX);
	_ASSERT(MAXSEGMENT>=nSegmentCountY);

	REngine::GetDevice().SetTransform( RST_WORLD, matWorld);
	REngine::GetDevice().SetFvF( RVt_pos::FVF );
	REngine::GetDevice().SetTextureFactor(dwColor);

	// y축과 평행한 선들
	for(int i=0;i<=nSegmentCountX;i++) {
		lineVertices[i*2].vPos	= RVector(i*fSegmentSize, 0,0);
		lineVertices[i*2+1].vPos = RVector(i*fSegmentSize, fSegmentSize*nSegmentCountY,0);
	}
	REngine::GetDevice().DrawPrimitiveUP( RPT_LINELIST, nSegmentCountX+1, (void*)lineVertices, sizeof(RVt_pos));

	// x축과 평행한 선들
	for(int i=0;i<=nSegmentCountY;i++) {
		lineVertices[i*2].vPos	= RVector(0, fSegmentSize*i,0);
		lineVertices[i*2+1].vPos = RVector(fSegmentSize*nSegmentCountX, fSegmentSize*i,0);
	}
	REngine::GetDevice().DrawPrimitiveUP( RPT_LINELIST, nSegmentCountY+1, (void*)lineVertices, sizeof(RVt_pos));
}

void RRenderHelper::RenderLine(const RMatrix &matWorld, RVector &v1,RVector &v2,DWORD dwColor)
{
	REngine::GetDevice().SetTransform( RST_WORLD, matWorld);
	REngine::GetDevice().SetFvF( RVt_pos_clr::FVF );

	RVt_pos_clr ver[2];
	ver[0].vPos = v1;
	ver[0].dwColor = dwColor;
	ver[1].vPos = v2;
	ver[1].dwColor = dwColor;

	REngine::GetDevice().SetTextureFactor(dwColor);
	REngine::GetDevice().DrawPrimitiveUP(RPT_LINELIST, 1, ver,sizeof(RVt_pos_clr));
}

void RRenderHelper::RenderLineArray(const RMatrix &matWorld, RVector *pVertices, int nCount, DWORD dwColor)
{
	RenderLines(matWorld,pVertices,nCount,dwColor,RPT_LINELIST);
}

void RRenderHelper::RenderLines(const RMatrix &matWorld, const RVector *pVertices, int nCount, DWORD dwColor, RPRIMITIVETYPE type)
{
	REngine::GetDevice().SetTransform( RST_WORLD, matWorld);
	REngine::GetDevice().SetFvF( D3DFVF_XYZ );

	REngine::GetDevice().SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
	REngine::GetDevice().SetTextureStageState(0, RTSS_COLORARG1, RTA_TFACTOR);
	REngine::GetDevice().SetTextureFactor(dwColor);

	REngine::GetDevice().DrawPrimitiveUP(type,nCount,(void*)pVertices,sizeof(RVector));

}


void RRenderHelper::RenderTriangles(const RMatrix &matWorld, RVector *pVertices, int nCount, DWORD dwColor, RPRIMITIVETYPE type)
{
	REngine::GetDevice().SetTransform( RST_WORLD, matWorld);
	REngine::GetDevice().SetFvF( RVt_pos::FVF );

//	REngine::GetDevice().SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
//	REngine::GetDevice().SetTextureStageState(0, RTSS_COLORARG1, RTA_TFACTOR);
	REngine::GetDevice().SetTextureFactor(dwColor);
	REngine::GetDevice().DrawPrimitiveUP(type,nCount,pVertices,sizeof(RVt_pos));
}

void RRenderHelper::RenderArrow(const RVector& pos, const RVector& dir, float fCylinderRadius, float fCylinderLength, float fCornRadius, float fCornLength, DWORD dwColor)
{
	RenderCylinder(pos,dir,fCylinderRadius,fCylinderLength,dwColor);
	RenderCorn(pos+fCylinderLength*dir,dir,fCornRadius,fCornLength,dwColor);
}

void RRenderHelper::RenderAxis(const RMatrix &matWorld, float fSize, float fCylRad, float fCornRad, float fCornLen)
{
	RVector pos(matWorld._41,matWorld._42,matWorld._43);

	RenderArrow(pos,RVector(matWorld._11,matWorld._12,matWorld._13),fCylRad,fSize,fCornRad,fCornLen,0xffff0000);
	RenderArrow(pos,RVector(matWorld._21,matWorld._22,matWorld._23),fCylRad,fSize,fCornRad,fCornLen,0xff00ff00);
	RenderArrow(pos,RVector(matWorld._31,matWorld._32,matWorld._33),fCylRad,fSize,fCornRad,fCornLen,0xff0000ff);

//	RenderLine(matWorld, RVector(0,0,0), RVector(fSize,0,0), 0xffff0000);
//	RenderLine(matWorld, RVector(0,0,0), RVector(0,fSize,0), 0xff00ff00);
//	RenderLine(matWorld, RVector(0,0,0), RVector(0,0,fSize), 0xff0000ff);

}


void RRenderHelper::RenderCenter(const RMatrix &matWorld, float size_w,float size_h,DWORD color)
{
	RVector v[4];
	v[0].x = -size_w/2.f;
	v[0].z = 0.f;
	v[0].y = 0.f;
	v[1].x = size_w/2.f;;
	v[1].z = 0.f;
	v[1].y = 0.f;
	v[2].x = 0.f;
	v[2].z = 0.f;
	v[2].y = size_h/2.f;
	v[3].x = 0.f;
	v[3].z = 0.f;
	v[3].y = -size_h/2.f;

	RenderLineArray(matWorld, v, 2, color);
}


struct screencoord
{
	D3DXVECTOR4	Pos;
	DWORD	Color;
	float	tex[2];
};

static DWORD scfvf = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);

void RRenderHelper::RenderQuad(float x, float y, float width, float height, float tuS, float tvS, float tuE, float tvE)
{
	RPFC_THISFUNC;

	screencoord	coord[4];

	coord[0].Pos.x = x-0.5f;
	coord[0].Pos.y = y-0.5f;

	coord[1].Pos.x = x+width-0.5f;
	coord[1].Pos.y = y-0.5f;

	coord[2].Pos.x = x+width-0.5f;
	coord[2].Pos.y = y+height-0.5f;

	coord[3].Pos.x = x-0.5f;
	coord[3].Pos.y = y+height-0.5f;


	coord[0].Color = coord[1].Color = coord[2].Color = coord[3].Color = 0xFFFFFFFF;
	coord[0].Pos.z = coord[1].Pos.z = coord[2].Pos.z = coord[3].Pos.z = 0;
	coord[0].Pos.w = coord[1].Pos.w = coord[2].Pos.w = coord[3].Pos.w = 1;

	coord[0].tex[0] = tuS;
	coord[0].tex[1] = tvS;

	coord[1].tex[0] = tuE;
	coord[1].tex[1] = tvS;

	coord[2].tex[0] = tuE;
	coord[2].tex[1] = tvE;

	coord[3].tex[0] = tuS;
	coord[3].tex[1] = tvE;

	REngine::GetDevice().SetCullMode(RCULL_CCW);
	REngine::GetDevice().SetVertexFormat( R_NONE);
	REngine::GetDevice().SetFvF(scfvf);
	REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLEFAN, 2, coord, sizeof(screencoord));

	REngine::GetDevice().SetCullMode(RCULL_CW);
}

void RRenderHelper::RenderQuadColor(float x, float y, float width, float height, DWORD dwColorLT, DWORD dwColorRT, DWORD dwColorLB, DWORD dwColorRB )
{
	static DWORD scfvfColor = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

	struct screencoordcolor
	{
		D3DXVECTOR4	Pos;
		DWORD	Color;
	};

	screencoordcolor coord[4];

	coord[0].Pos.x = x-0.5f;
	coord[0].Pos.y = y-0.5f;

	coord[1].Pos.x = x+width-0.5f;
	coord[1].Pos.y = y-0.5f;

	coord[2].Pos.x = x+width-0.5f;
	coord[2].Pos.y = y+height-0.5f;

	coord[3].Pos.x = x-0.5f;
	coord[3].Pos.y = y+height-0.5f;

	coord[0].Color = dwColorLB;
	coord[1].Color = dwColorRB;
	coord[2].Color = dwColorRT;
	coord[3].Color = dwColorLT;

	coord[0].Pos.z = coord[1].Pos.z = coord[2].Pos.z = coord[3].Pos.z = 0;
	coord[0].Pos.w = coord[1].Pos.w = coord[2].Pos.w = coord[3].Pos.w = 1;

	REngine::GetDevice().SetCullMode(RCULL_CCW);
	REngine::GetDevice().SetVertexFormat( R_NONE);
	REngine::GetDevice().SetFvF(scfvfColor);
	REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLEFAN, 2, coord, sizeof(screencoordcolor));

	REngine::GetDevice().SetCullMode(RCULL_CW);
}

void RRenderHelper::RenderTexture(float x, float y, float width, float height, RTexture* hTexture, float tu, float tv, RBLEND blend, RTEXTUREFILTERTYPE filter)
{
	RMatrix t = RMatrix::IDENTITY;
	t.SetTranslation(RVector(0.5, 0.5, 0.5));

	switch(blend)
	{
	case RBLEND_ONE:
		REngine::GetDevice().SetBlending( RBLEND_ONE, RBLEND_NONE);
		break;
	case RBLEND_DESTCOLOR:
		REngine::GetDevice().SetBlending( RBLEND_DESTCOLOR, RBLEND_ZERO);
		break;
	case RBLEND_SRCALPHA:
		REngine::GetDevice().SetBlending( RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
		break;
	default:
		REngine::GetDevice().SetBlending( RBLEND_NONE, RBLEND_NONE);
		break;
	}
	
	REngine::GetDevice().SetFogEnable(false);
	REngine::GetDevice().SetAlphaTestEnable(false);
	REngine::GetDevice().SetLighting(false);
	REngine::GetDevice().ShaderOff();
	REngine::GetDevice().SetTextureFilter(0, filter);
	REngine::GetDevice().SetCullMode(RCULL_NONE);
	REngine::GetDevice().SetTexture(0, hTexture);
	REngine::GetDevice().SetFvF(scfvf);
	REngine::GetDevice().SetTextureStageState( 0,RTSS_COLORARG1, RTA_TEXTURE );
	REngine::GetDevice().SetTextureStageState( 0,RTSS_COLORARG2, RTA_DIFFUSE );
	REngine::GetDevice().SetTextureStageState( 0,RTSS_COLOROP, RTOP_SELECTARG1);
	REngine::GetDevice().SetTextureStageState( 0,RTSS_ALPHAOP, RTOP_MODULATE );

	REngine::GetDevice().SetTextureStageState(0, RTSS_TEXCOORDINDEX, 0 );
	REngine::GetDevice().SetTextureStageState(0, RTSS_TEXTURETRANSFORMFLAGS, RTTFF_DISABLE );
	REngine::GetDevice().SetTextureAddressClamp(0);
	REngine::GetDevice().SetTransform(RST_TEXTURE0, t);

	RenderQuad( x,y,width,height,tu,tv);

	REngine::GetDevice().SetBlending(RBLEND_NONE);
	REngine::GetDevice().SetCullMode(RCULL_CW);
	REngine::GetDevice().SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
	REngine::GetDevice().SetTextureStageState(0, RTSS_COLORARG1, RTA_TFACTOR);
	REngine::GetDevice().SetTextureStageState(0, RTSS_ALPHAOP, RTOP_SELECTARG1);
	REngine::GetDevice().SetTextureStageState(0, RTSS_ALPHAARG1, RTA_TFACTOR);
	REngine::GetDevice().SetTransform(RST_TEXTURE0, RMatrix::IDENTITY);
}

void RRenderHelper::RenderText( const RVector& vPos, const char* fontName, float fSize, DWORD dwColor, const char* chText, const int nOffset_x /*= 0*/, const int nOffset_y /*= 0*/ )
{
	RFont pFont;
	RDevice* pDevice = REngine::GetDevicePtr();

	if( !pFont.Create(pDevice, MLocale::ConvAnsiToUTF16(fontName).c_str(), fSize) )
	{
		return;
	}

	MVector2  vScreen;
	GetScreenFromWorld(&vScreen, &vPos, pDevice);
	if(RFont::BeginFont())
	{
		int nX = vScreen.x;
		int nY = vScreen.y;
		pFont.DrawText_UNICODE(nX + nOffset_x, nY + nOffset_y, MLocale::ConvAnsiToUTF16(chText).c_str(), -1, dwColor);

		RFont::EndFont();
	}
}

void RRenderHelper::RenderPyramid(const RVector& vTop, const RViewFrustum& viewFrustum, DWORD dwColor, bool bLine)
{
	RVt_pos_clr	Vertices[5];
	Vertices[0].vPos = vTop;
	Vertices[0].dwColor =dwColor;
	
	Vertices[1].vPos = viewFrustum.GetPoints()[0];
	Vertices[1].dwColor =dwColor;
	
	Vertices[2].vPos = viewFrustum.GetPoints()[2];
	Vertices[2].dwColor =dwColor;
	
	Vertices[3].vPos = viewFrustum.GetPoints()[6];
	Vertices[3].dwColor =dwColor;
	
	Vertices[4].vPos = viewFrustum.GetPoints()[4];
	Vertices[4].dwColor =dwColor;
	
	const static WORD IndexArray[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 1,
		1, 3, 2,
		1, 4, 3
	};

	REngine::GetDevice().SetTextureFactor(dwColor);
	REngine::GetDevice().SetTransform( RST_WORLD, RMatrix::IDENTITY);
	REngine::GetDevice().SetFvF(RVt_pos_clr::FVF);

	if(bLine)
		REngine::GetDevice().SetFillMode(RFILL_WIRE);
	
	REngine::GetDevice().DrawIndexedPrimitiveUP( RPT_TRIANGLELIST, 0, 5, 6,(void*)IndexArray, (void*)Vertices, sizeof(RVt_pos_clr));
	
	REngine::GetDevice().SetFillMode(RFILL_SOLID);	
}

}
