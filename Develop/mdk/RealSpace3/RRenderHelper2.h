#ifndef _RENDERHELPER2_H_
#define _RENDERHELPER2_H_

#include "RTypes.h"
#include <vector>

namespace rs3 {

class RDevice;
class RTexture;

namespace RenderHelper {
	class RS_API RHelperMesh
	{
	public:
		std::vector<rs3::RVector3> m_kVertices;
		std::vector<WORD> m_kIndices;

	public:
		static void SetRenderState(RDevice *pDevice, bool bRenderWire = false);
		static void ResetRenderState(RDevice *pDevice);

		virtual void Draw(RDevice *pDevice, const RMatrix &matWorld, DWORD dwColor);
	};

	/**
	 	원을 그리는 개체
	 */
	class RS_API RSphere3D : public RHelperMesh
	{
	private:
		float m_fRadius;
		int m_nSeparators;

		void CreateGeomerty();

	public:
		void SetRadius(float fRadius)
		{
			m_fRadius = fRadius;
			CreateGeomerty();
		}

		void SetSeparators(int nSeparators)
		{
			m_nSeparators = nSeparators;
			CreateGeomerty();
		}

		RSphere3D(float radius, int seperators);
	};

	// Function 들
	RS_API void DrawIcon3Ds(RDevice *pDevice, const RVector3 *pPositions, int nCount, float width, float height, RTexture *pTexture, DWORD dwColor);
};

};

#endif
