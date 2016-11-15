#ifndef _XTARGET_LINE_H
#define _XTARGET_LINE_H

#include "XTarget.h"

class XTargetLine : public XTarget
{
protected:
	stTargetID*					m_pTarget;

protected:
	// 타겟 업데이트
	void						UpdateTarget(vec3& pos, vec3& dir);

	// 타겟 검출 방법
	bool						CheckLineTarget(XObject* pObject, vec3& pos, vec3& dir, stTargetID& outTargetObject, float& fOutDist);
	virtual bool				CheckLineTargetObj(XObject* pObject, vec3& pos, vec3& dir, stTargetID& outTargetObject, float& fOutDist);

	// 여러가지 검사
	void						CheckExistTargetObj();
	bool						CheckTargetObject(XObject* pObject, vec3& pos);
	bool						CheckOverlapDummy(stTargetID& targetObject);				// 중복 체크
	
public:
	XTargetLine(XObject* pObject);
	virtual ~XTargetLine();

	virtual void				UpdateTarget();
	virtual void				DelTargetAll();

	virtual stTargetID*			GetTargetInfo()									{ return m_pTarget; }
	virtual bool				GetTargetIDByUID(MUID uid, int& nOutGroupID, int& nOutDummyID);
	virtual TARGET_TYPE			GetTargetType()		{ return XTT_LINE; }
};

#endif // _XTARGET_LINE_H