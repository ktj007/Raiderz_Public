#pragma once

#include "RClothConstraint.h"
#include "RClothParticle.h"
#include "RClothType.h"
#include "RClothParameter.h"

namespace rs3
{
	class RClothGroupTriangles
	{
	public:
		RClothGroupTriangles(void)
		{
			m_theNumberOfTriangles = 0;
			m_pTriangles = 0;
		}
		~RClothGroupTriangles(void)
		{
			releaseATriangleGroup();
		}

		void releaseATriangleGroup(void);
		void allocateForATriangleGroup(unsigned int theNumberOfTriangles);

		unsigned int	m_theNumberOfTriangles;
		RC_TRIANGLE*	m_pTriangles;
	};

	class RClothPartitionedTriangles
	{
	public:
		RClothPartitionedTriangles(void)
		{
			m_theNumberOfTriangleGroup = 0;
			m_pTriangleGroups = 0;
		}
		~RClothPartitionedTriangles(void)
		{
			releaseTriangleGroups();
		}
		void releaseTriangleGroups(void);
		void allocateForTriangleGroups(unsigned int theNumberOfTriangleGroup);
		bool save(FILE *pFile);
		bool load(FILE *pFile);
		bool load(MFile *pFile);

		unsigned int			m_theNumberOfTriangleGroup;
		RClothGroupTriangles*	m_pTriangleGroups;
	};

	class RS_API RClothInfo
	{
	public:
		static char* FM_VERSION;
		static char* FM_PARTICLE;
		static char* FM_TRIANGLE;
		static char* FM_CONSTRAINT;
		static char* FM_PARAMETER;
		static char* FM_PARTITIONEDTRIANGLE;
	public:
		RClothInfo(void);
		~RClothInfo(void);

		void release(void);

	public:
		//[ particles ]
		RClothParticle* allocateSpaceForParticles(int theNumberOfParticles);
		void releaseSpaceForParticles(void);

		//[ triangles ]
		void allocateSpaceForTrianglesAndTypes(int type0_TheNumberOfTriangles,int type1_TheNumberOfTriangles,int type2_TheNumberOfTriangles,int type3_TheNumberOfTriangles);
		void releaseSpaceForTrianlgesAndTypes(void);

		//[ constraints ]
		void allocateSpaceForDistanceConstraints(int theNumberOfConstraints);
		void releaseSpaceForDistanceConstraints(void);

		void allocateSpaceForStretchConstraints(int theNumberOfConstraints);
		void releaseSpaceForStretchConstraints(void);

		//[ save/load ±â´É ]
		bool save(const char* szFileName);
		bool load(const char* szFileName);
		static bool isWorldSimulation(const char* szFileName,bool *pbWorldSimulation);

	public://set,get
		//--[ particles ]
		inline	void					setTheNumberOfParticles(int theNumberOfParticles)	{	m_theNumberOfParticles = theNumberOfParticles;	}
		inline	int						getTheNumberOfParticles(void)	const				{	return m_theNumberOfParticles; }
		inline	RClothParticle*			getParticles(void) 									{	return m_pParticles;	}
		inline	const RClothParticle*	getParticles(void)	const							{	return m_pParticles;	}

		
		//--[ triangles(face) ]
		inline  RC_TRIANGLE*			getType0_Triangles(void)	{ return m_pType0_Triangles;	}
		inline  RC_TRIANGLE*			getType1_Triangles(void)	{ return m_pType1_Triangles;	}
		inline  RC_TRIANGLE*			getType2_Triangles(void)	{ return m_pType2_Triangles;	}
		inline  RC_TRIANGLE*			getType3_Triangles(void)	{ return m_pType3_Triangles;	}

		inline	void					setTheNumberOfType0_Triangles(int theNumberOfTriangles)	{	m_Type0_TheNumberOfTriangles = theNumberOfTriangles;	}
		inline	int						getTheNumberOfType0_Triangles(void)	const				{	return m_Type0_TheNumberOfTriangles;	}
		inline	void					setTheNumberOfType1_Triangles(int theNumberOfTriangles)	{	m_Type1_TheNumberOfTriangles = theNumberOfTriangles;	}
		inline	int						getTheNumberOfType1_Triangles(void)	const				{	return m_Type1_TheNumberOfTriangles;	}
		inline	void					setTheNumberOfType2_Triangles(int theNumberOfTriangles)	{	m_Type2_TheNumberOfTriangles = theNumberOfTriangles;	}
		inline	int						getTheNumberOfType2_Triangles(void)	const				{	return m_Type2_TheNumberOfTriangles;	}
		inline	void					setTheNumberOfType3_Triangles(int theNumberOfTriangles)	{	m_Type3_TheNumberOfTriangles = theNumberOfTriangles;	}
		inline	int						getTheNumberOfType3_Triangles(void)	const				{	return m_Type3_TheNumberOfTriangles;	}

		//--[ constraints ]
		inline	void					setTheNumberOfDistanceConstraints(int theNumberOfConstraints)	{	m_theNumberOfDistanceConstraints = theNumberOfConstraints; }
		inline	int						getTheNumberOfDistanceConstraints(void)	const					{	return m_theNumberOfDistanceConstraints; }
		inline	RClothConstraint*		getDistanceConstraints(void)							{	return m_pDistanceConstraints;	}
		inline	const RClothConstraint*	getDistanceConstraints(void)		const 				{	return m_pDistanceConstraints;	}
				RClothConstraint&		getDistanceConstraint(int index)	const;

		inline	void					setTheNumberOfStretchConstraints(int theNumberOfConstraints)	{	m_theNumberOfStretchConstraints = theNumberOfConstraints; }
		inline	int						getTheNumberOfStretchConstraints(void)	const					{	return m_theNumberOfStretchConstraints; }
		inline	RClothConstraint*		getStretchConstraints(void)							{	return m_pStretchConstraints;	}
		inline	const RClothConstraint*	getStretchConstraints(void)		const 				{	return m_pStretchConstraints;	}
				RClothConstraint&		getStretchConstraint(int index)	const;
		
		//--[ parameter ]
		inline	RClothParameter*		getParameter(void)	{	return &m_parameter; }

		//--[ PartitionedTriangles ]
		void	calculateAABBOfTheGroup(RAABB *pAABB,unsigned int numOfGroup);


	private:
		//--[ particles ]
		RClothParticle				*m_pParticles;
		int							m_theNumberOfParticles;

		//--[ triangles ]
		RC_TRIANGLE					*m_pType0_Triangles;
		RC_TRIANGLE					*m_pType1_Triangles;
		RC_TRIANGLE					*m_pType2_Triangles;
		RC_TRIANGLE					*m_pType3_Triangles;
		int							m_Type0_TheNumberOfTriangles;
		int							m_Type1_TheNumberOfTriangles;
		int							m_Type2_TheNumberOfTriangles;
		int							m_Type3_TheNumberOfTriangles;

		//--[ constraints ]
		RClothConstraint			*m_pDistanceConstraints;
		int							m_theNumberOfDistanceConstraints;

		RClothConstraint			*m_pStretchConstraints;
		int							m_theNumberOfStretchConstraints;

		//--[ parameters ]
		RClothParameter				m_parameter;

	public:
		//--[ PartitionedTriangles ]
		RClothPartitionedTriangles	m_partitionedTriangles;

	};


}
