#pragma once

#include <vector>
#include "RSpeedGrass.h"

class RHeightField;

namespace rs3
{

	class RShaderFX;
	class RSceneManager;
	class RDevice;
	class RSceneManager;
	class RSpeedGrassGPUInstance;
	class RTerrain;

	class RGrassSystem_WindStrengthAmplifier
	{
	private:
		float m_fElapsedTime;
		float m_fAmplifier;
		float m_fNextUpdateTime;

	protected:
		float m_fStrengthCoefficient;

		virtual float GetNewAmplifier();
		virtual float GetNewUpdateTime();
	public:
		RGrassSystem_WindStrengthAmplifier();

		float Strength(float fDeltaTime, float fWindStrength);
	};

	class RS_API RGrassSystem
	{
		friend RSpeedGrassGPUInstance;

	protected:
		RSpeedGrass*			m_pGrass;
		RDevice*				m_pDevice;

		float					m_fTime;

		RTexture*				m_hGrassCompositeTexture;
		string					m_strTextureFilename;
		string					m_strTextureInfoFilename;

		bool					m_bRendering;
		bool					m_bNeedUpdate;

		int						m_nRenderGrassCount;

		RSceneManager* m_pSceneManager;
		RGrassSystem_WindStrengthAmplifier	m_WindStrengthAmplifier;
	public:
		string					m_strGrassPath;
		/**
		* GrassSystem을 초기화 한다.
		* @param	dev		RDevice의 포인터
		* @return	초기화 성공시에 true를 리턴한다.
		*/
		bool					Create(RDevice* dev, RSceneManager* pSceneManager);
		/**
		* GrassSystem의 내부 자원을 해제한다.
		*/
		void					Destroy();

		/**
		* GrassSystem의 내부 컬링 단위인 Region을 생성한다. 
		* 각 Region 아래에 풀에 관한 정보가 붙도록 되어 있으므로, 사용하기 전에 반드시 만들어야 한다.
		* @param	row		행 수
		* @param	col		열 수
		* @param	fWidth	열의 길이
		* @param	fHeight 행의 길이
		*/
		void					CreateRegions(int row, int col, float fWidth, float fHeight);
		/**
		* 풀을 추가한다.
		* @param	RGassBlade	풀에 대한 구조체(RGassBlade)
		*/
		bool					AddBlade(const RGrassBlade& b);	// 결과 리턴하게 변경 - 090610, OZ
		/**
		* 일정 영역(원) 안의 풀을 지운다.
		* @param	x	원의 중심 x좌표
		* @param	y	원의 중심 y좌표
		* @param	fRadius		원의 반지름
		*/
		void					DeleteBlade(float x, float y, float fRadius);
		/**
		* 현재 풀의 위치 정보를 가져온다.
		* @param	outBlades	위치 정보를 담을 vector 변수
		*/
		void					GetBladesArray(vector<RGrassBlade> &outBlades);
		/**
		* 모든 풀을 삭제한다.
		*/		
		void					DeleteAllBlade();
		/**
		* 변경된 풀을 반영한다. AddBlde, DeleteBlade후에는 호출하여 반영해주어야 제대로 렌더링이 된다.
		*/
		void					UpdateDXBuffers( RTerrain* pTerrain);

		const char*				GetLoadedTextureName();
		void					LoadTexture(const char* filename);
		void					Render(RSceneManager* pManager, RCameraSceneNode* pCamera);
		void					Update(RSceneManager* pManager);
		int						GetCurrentRenderGrassCount();

		//void					SetLodParam(float farlod, float lodtransition) { m_fFarLod = farlod; m_fLodTransition = lodtransition; }

		bool					SaveToFile(const string& filename);
		bool					LoadFromFile( const string& filename, RTerrain* pTerrain);

		void					SetRenderable(bool r) { m_bRendering = r; }

		void					RenderGStage( RRENDERINGMETHOD method, RCameraSceneNode* pCamera);
		void					RenderMStage( RCameraSceneNode* pCamera);
		/**
		* 풀을 지형 높이에 따라 업데이트
		*/
		class ICallback
		{
		public:
			virtual void OnBlade(RGrassBlade &b) = 0;
		};
		void					UpdateOnCallback(ICallback *piCallback);

		void					LoadGrassListFromXML(MXmlElement &xmlRoot, const char *szNameA, const char *szNameB);

		void					SetWindDirection(const RVector& windDir) { m_pGrass->SetWindDirection(windDir); }
		void					SetWindStrength(float windStr) { m_pGrass->SetWindStrength(windStr); }				// 0(바람없음)~1(최대)


		void					SetGrassJigglingPeriod(float f)		{	m_pGrass->SetGrassJigglingPeriod(f);	}
		void					SetGrassJigglingStrength(float f)	{	m_pGrass->SetGrassJigglingStrength(f);	}
		void					SetGrassIncliningStrength(float f)	{	m_pGrass->SetGrassIncliningStrength(f);	}

		int						GetGrassID(const char *szFilename);

		int						GetGrassWidth(int nIndex);
		int						GetGrassHeight(int nIndex);

		void					SetAmbientColor(RVector4 &vColor);
		RVector4				GetAmbientColor();

		void					SetDiffuseColor(RVector4 &VColor); 
		RVector4				GetDiffuseColor();

		RGrassSystem();
		virtual ~RGrassSystem();
	};
}
