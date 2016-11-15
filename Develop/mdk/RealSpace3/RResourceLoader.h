#pragma once

#include "RBackgroundWorkUnit.h"
#include "RResource.h"

namespace rs3 {

class RResource;

class RResourceLoader : public RBackgroundWorkUnit
{
public:
	RResourceLoader::RResourceLoader(RResource* pResource, bool bBackground = false);

	virtual ~RResourceLoader() { }

	virtual int				GetDebuggingID()	{ return 10; }
	virtual const char*		GetName();

	virtual bool			Load(bool bBackground);
	bool					IsBackgroundCreated();

	bool					doFill(RResource* pResource);
	RResource::LOAD_RESULT	doLoad(RResource* pResource);
protected:
	virtual int				CalcPriority();
	virtual void			OnBackgroundWork();

	virtual void			OnUpdateCompleteWork();

	RResource*				m_pResource;
	bool					m_bBackgroundCreate;	///< 현재 백그라운드로부터 로딩인가
	std::string				m_strWorkUnitName;
};

//----------------------------------------------------------------------------------------------------
inline bool	RResourceLoader::IsBackgroundCreated()	{ return m_bBackgroundCreate; }

}