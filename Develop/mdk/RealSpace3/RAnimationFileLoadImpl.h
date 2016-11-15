#pragma once

#include "RFile.h"

namespace rs3 {

class RAnimationNode;
class RAnimationFileLoadImpl
{
public:
	virtual bool LoadVertexAni(RFile &rf, RAnimationNode *pANode, DWORD dwVersion) = 0;
	virtual bool LoadBoneAni(RFile &rf, RAnimationNode *pANode, DWORD dwVersion) = 0;

	virtual bool LoadVisibilityKey(RFile &rf, RAnimationNode *pANode, DWORD dwVersion) = 0;
};

class RAnimationFileLoadImpl_v6 : public RAnimationFileLoadImpl
{
private:
	virtual void LoadVertexAniBoundingBox(RFile &rf, RAnimationNode *pANode);

public:
	virtual bool LoadVertexAni(RFile &rf, RAnimationNode *pANode, DWORD dwVersion);
	virtual bool LoadBoneAni(RFile &rf, RAnimationNode *pANode, DWORD dwVersion);

	virtual bool LoadVisibilityKey(RFile &rf, RAnimationNode *pANode, DWORD dwVersion);
};

class RAnimationFileLoadImpl_v7 : public RAnimationFileLoadImpl_v6
{
	virtual void LoadVertexAniBoundingBox(RFile &rf, RAnimationNode *pANode);
};

class RAnimationFileLoadImpl_v9 : public RAnimationFileLoadImpl_v7
{
	virtual bool LoadVisibilityKey(RFile &rf, RAnimationNode *pANode, DWORD dwVersion);
};

class RAnimationFileLoadImpl_v11 : public RAnimationFileLoadImpl_v9
{
	virtual bool LoadBoneAni(RFile &rf, RAnimationNode *pANode, DWORD dwVersion); // dont know what to do it.
	virtual bool LoadVisibilityKey(RFile &rf, RAnimationNode *pANode, DWORD dwVersion);
};

class RAnimationFileLoadImpl_v12 : public RAnimationFileLoadImpl_v11
{
	virtual bool LoadBoneAni(RFile &rf, RAnimationNode *pANode, DWORD dwVersion); // dont know what to do it.
};


};