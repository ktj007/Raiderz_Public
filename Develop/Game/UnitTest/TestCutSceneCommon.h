#include "XCutSceneContent.h"
#include "XCutSceneObject.h"
#include "XCutSceneObjectController.h"

#include "MLocale.h"

#include "RCameraSceneNode.h"

// Custom Builder
class TestCutSceneContentBuilder : public XCutSceneContentBuilder
{
public:
	bool OnClapperCreated(XCutSceneClapper* pClapper)
	{
		return true;
	}

	virtual XCutSceneObjectEntity* OnCreateCameraEntity()
	{
		class CameraEntity : public XCutSceneObjectEntity
		{
		public:
			CameraEntity()
			{
				m_pCameraSceneNode = new rs3::RCameraSceneNode;
			}
			virtual ~CameraEntity()
			{
				delete m_pCameraSceneNode;
			}

			virtual void SetTransform(const MMatrix& mat, bool bUsingCollision)
			{
				vec3 vPos(mat._41, mat._42, mat._43);
				vec3 vDir(mat._21, mat._22, mat._23);
				vec3 vUp(mat._31, mat._32, mat._33);

				m_pCameraSceneNode->Set(vPos, vDir, vUp);
			}

			virtual void SetAnimation(const std::wstring& _rAnimationName)
			{

			}

			virtual rs3::RCameraSceneNode* GetRelatedCamera()
			{
				return m_pCameraSceneNode;
			}

		private:
			rs3::RCameraSceneNode* m_pCameraSceneNode;
		};

		return new CameraEntity;
	}

	virtual XCutSceneObjectEntity* OnCreateActorEntity(const std::wstring& _rStrType)
	{
		return NULL;
	}

	virtual XCutSceneObjectEntity* OnCreateEventEntity(MXmlElement* _pEventElement)
	{
		return NULL;
	}

	bool Build(const wchar_t* _pSzXml, XCutSceneContent* _pOutCutSceneContents)
	{
		MXml aXml;
		std::string strXml = MLocale::ConvUTF16ToUTF8(_pSzXml);
		if (!aXml.LoadStream(strXml.c_str()))
		{
			return false;
		}

		MXmlElement* pRootElement = aXml.DocHandle().FirstChildElement("maiet").Element();
		return XCutSceneContentBuilder::Build(pRootElement, _pOutCutSceneContents);
	}
};
