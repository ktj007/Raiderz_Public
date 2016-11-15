#include "BaseObject.h"
#include "SceneNodeCommonProperty.h"
#include "RPostEffectDistortion.h"
#include "RUtil.h"

using namespace rs3;

class CDistortionProperty
{
public:
	void SetParam(RDistortionParameter* pParam)	{ m_pParam = pParam; }

	void SetDir0X(float fdir) { m_pParam->fDir0X = fdir; }
	void SetDir0Y(float fdir) { m_pParam->fDir0Y = fdir; }
	void SetDir1X(float fdir) { m_pParam->fDir1X = fdir; }
	void SetDir1Y(float fdir) { m_pParam->fDir1Y = fdir; }
	void SetScale(float fScale) { m_pParam->fScale = fScale; }
	void SetSpeed0(float fSpeed) { m_pParam->fSpeed0 = fSpeed; }
	void SetSpeed1(float fSpeed) { m_pParam->fSpeed1 = fSpeed; }
	void SetRenderable(bool bRender)	{ m_pParam->bRender = bRender; }
	void SetRange(float fRange) { m_pParam->fRange = fRange; }
	void SetAttenuationBegin(float fAttenuationBegin) { m_pParam->fAttenuationBegin = fAttenuationBegin; }
	void SetNormalMapFileName(char* strFilePath)
	{
		SplitPath(strFilePath, NULL, m_pParam->strNormalMapFileName);

		m_pParam->ReplaceNormalMap();
	}

	float GetDir0X() { return m_pParam->fDir0X; }
	float GetDir0Y() { return m_pParam->fDir0Y; }
	float GetDir1X() { return m_pParam->fDir1X; }
	float GetDir1Y() { return m_pParam->fDir1Y; }
	float GetScale() { return m_pParam->fScale; }
	float GetSpeed0() { return m_pParam->fSpeed0; }
	float GetSpeed1() { return m_pParam->fSpeed1; }
	bool GetRenderable() { return m_pParam->bRender; }
	char* GetNormalMapFileName() { return m_pParam->strNormalMapFileName; }
	float GetRange() { return m_pParam->fRange; }
	float GetAttenuationBegin() { return m_pParam->fAttenuationBegin; }

private:
	RDistortionParameter* m_pParam;
};

class CDistortionPropertyRef : public CRTTIClass<CDistortionPropertyRef, CBaseObject,SPID_DISTORTION>, public CDistortionProperty
{
public:
	CDistortionPropertyRef() {}
	CDistortionPropertyRef(RDistortionParameter* pParam) { SetParam(pParam); }
	static void RegisterReflection()
	{
		const char* strCategory = "왜곡 속성";
		RegisterProperty<float>("Direction0_X",&GetDir0X,&SetDir0X, 0, 0, "법선맵이 움직이는 첫번째 방향(X축)", strCategory);
		RegisterProperty<float>("Direction0_Y",&GetDir0Y,&SetDir0Y, 0, 0, "법선맵이 움직이는 첫번째 방향(Y축)", strCategory);
		RegisterProperty<float>("Direction1_X",&GetDir1X,&SetDir1X, 0, 0, "법선맵이 움직이는 두번째 방향(X축)", strCategory);
		RegisterProperty<float>("Direction1_Y",&GetDir1Y,&SetDir1Y, 0, 0, "법선맵이 움직이는 두번째 방향(X축)", strCategory);
		RegisterProperty<float>("Scale",&GetScale,&SetScale, 0, 0, "왜곡 정도", strCategory);
		RegisterProperty<float>("Speed0",&GetSpeed0,&SetSpeed0, 0, 0, "법선맵이 움직이는 첫번째 방향에 대한 속도", strCategory);
		RegisterProperty<float>("Speed1",&GetSpeed1,&SetSpeed1, 0, 0, "법선맵이 움직이는 두번째 방향에 대한 속도", strCategory);
		RegisterProperty<bool>("Render?",&GetRenderable,&SetRenderable, 0, 0, "렌더링 여부 설정", strCategory);
		RegisterProperty<char*>("NormalMap",&GetNormalMapFileName,&SetNormalMapFileName, 0, 0, "법선맵 파일", strCategory, PE_FILENAME);
		RegisterProperty<float>("Range (Only EffectActor)",&GetRange,&SetRange, 0, 0, "적용 범위(효과 적용 범위를 이 수치만큼 오브젝트의 법선방향으로 확장시킴)", strCategory);
		RegisterProperty<float>("Attenuation Begin",&GetAttenuationBegin,&SetAttenuationBegin, 0, 0, "Visibility값이 이 값보다 작으면 Visibility값에 따라 효과가 감쇠됨", strCategory);
	}
};