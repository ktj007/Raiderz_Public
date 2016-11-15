#pragma once

#include "RActor.h"
#include "RActorNode.h"
#include "RClothSimulator.h"

namespace rs3 
{
	class RS_API RClothActorNode : public RActorNode,public RClothSimulator
	{
		MDeclareRTTI;

	public:
		RClothActorNode(void);
		~RClothActorNode(void);

		static RClothActorNode* Create(std::string meshName,std::string fileName);
		
	//--[ RActorNode의 함수 ]
	public:
		virtual bool	OnCreateDeviceDependentPart(void);
		virtual	void	UpdateTransformAndVertex(void);
		virtual void	SetVertexStream( bool b2ndBufferMng = true ) override;
		virtual	void	RenderPrimitive(int index);

	//--[ RClothSimulator의 함수 ]
	protected:
		virtual		void		simulate(float timeStep) = 0;

	//--[ RClothActorNode의 함수 ]
	protected:
		//시뮬레이션
		void		simulateByFixedTime(void);
		
		//디버그용 노말 그리기
		void			RenderNormal();

		//디버그용 충돌 메쉬 그리기
		virtual void	RenderCollision(int index) {}

		//이 옷의 world, inv world 매트릭스 구하는 함수
		void			getClothWorldTransformMatrix(RMatrix *pWorldTransformMatrix);	
		void			getClothInvWorldTransfromMatrix(RMatrix *pInvWorldTransformMatrix);

		//따라가는 본의 TransformMatrix구하는 함수들
		RActorNode*		getBoneForWorldTransform(void);
		RActorNode*		getBoneForWorldTransformFromBoneName(const string& boneName);
		string			getBoneNameForWorldTransformFromProperty(void);

		bool			isAttachedToBone(void);
	
		void			getLastTranslation(RVector *pLastTranslation);
	
		void		calculateNormals(void);
		void		makeSortedCollision(void);
		void		applyWind(RVector *pWindVector);

		int			isValidModeling(void);
		bool		isValid(void)	{	return m_bValidModel;	}

	public:
		string		getNodeName(void)				{	return m_strNodeName;	}
		void		setFileName(string fileName)	{	m_fileName = fileName;	}
		std::string getFileName(void)				{	return m_fileName;		}


	protected:
		vector<int>	m_vSortedCollisionIndices;
		string		m_fileName;
		float		m_beforeDuaration;
		float		m_currentDuration;

		RActorNode* m_pBoneForWorldTransform;
		bool		m_bValidModel;

		RMatrix		m_beforeClothWorldMatrix;
		RMatrix		m_currentClothWorldMatrix;

		unsigned int m_nLastUpdatedFrame;

		//-[ virtual translation ]
		float	m_virtualTranslationThresholdX;
		float	m_virtualTranslationThresholdY;
		float	m_virtualTranslationThresholdZ;

		float	m_virtualTranslationSizeX;
		float	m_virtualTranslationSizeY;
		float	m_virtualTranslationSizeZ;

	public://--;; cml2 include관계가 요상해서..여기에..
		static	void	AABB2OBB(RBox& aabb,ROBB& obb);
	};
}//rs3