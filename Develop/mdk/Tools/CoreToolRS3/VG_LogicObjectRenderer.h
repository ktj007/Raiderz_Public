#pragma once
#include "VL_ListenableView.h"

class CLogicObject;
class CLogicObjectRenderer;

class CORETOOL_API VG_LogicObjectRenderer : public CListenableViewImpl_General<VG_LogicObjectRenderer>
{
public:
	VG_LogicObjectRenderer(void);
	~VG_LogicObjectRenderer(void);

	// Listenable View
	virtual void			Listen(CoreMessage& _message);			///< Listen
	virtual const char*		GetViewName(){ return "LogicObjectRenderer"; }	///< View Name

protected:
	void			OnLogicObjectNew(CLogicObject* pLogicObject);
	void			OnLogicObjectDelete(CLogicObject* pLogicObject);
	void			OnLogicObjectMove(CLogicObject* pLogicObject);
	void			OnLogicObjectChanged(CLogicObject* pLogicObject);
	void			OnLogicObjectShow(CLogicObject* pLogicObject);
	void			OnLogicObjectHide(CLogicObject* pLogicObject);
	void			OnLogicObjectRender();
	void			OnLogicObjectTextInfoRender();
	typedef map< CLogicObject*, CLogicObjectRenderer* > LOGIC_RENDERERMAP;
	
	/// 로직오브젝트를 보여줄 renderer 컨테이너
	LOGIC_RENDERERMAP m_mapLogicObjectRenderer;
};
