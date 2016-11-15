#pragma once

#include "M_XMLDataStructure.h"
#include <map>
#include "MXml.h"
#include "RTypes.h"
#include "CoreTypes.h"

namespace rs3 { class RViewFrustum; }

class CBaseObject;
class ICommandBuffer;
class CLogicObjectRenderer;
class CControlSceneProperty;	///< TODO: 제거
class CControlMediator;

class CORETOOL_API CLogicObject : public CCoreObject
{
public:
								CLogicObject(void);
	virtual						~CLogicObject(void);

	void						DestroyLogicObject();

	bool						IsEntityLogic() const { return m_bEntityLogic; }

	virtual void				LoadPropertyFromXML(SElementAtXML* _pEntityProperty, MXmlElement* _pElement = NULL);
	inline SElementAtXML*		GetRootElement(){ return m_pLogicObjectElement_; }
	static void					CopyElement(CLogicObject* _pClientObject, SElementAtXML* _pFromElement, SElementAtXML* _pToElement, bool _bBaseCase);
	template< typename T > void	SetRootElementAttributeValue(const char* _pSzAttributeName, T& _value);

	void						Update(){ OnUpdate(); }
	void						Render(){ OnRender(); }

	void						PutToTreeCtrl(CTreeCtrl& _refTreeCtrl);
	typedef						std::map<HTREEITEM, SElementAtXML::AttributeList> EntityAttributeMap;
	static bool					UpdateSelectionInfo(CLogicObject* _pLogicObjectToAdd, CTreeCtrl& _refTreeCtrl, EntityAttributeMap& _refEntityAttrMap);
	
	bool						GetVisible(){ return m_bVisibility_; }
	void						SetVisible(bool _bVisible){ m_bVisibility_ = _bVisible; OnSetVisible(_bVisible); }

	inline const char*			GetObjectName();
	void						SetObjectName(const char* _pSzName) { OnSetObjectName(_pSzName); }
	virtual const char*			GetTypeName();
	inline const char*			GetLayerName() const;
	inline void					SetLayerName(const char* szLayerName) { OnSetLayerName(szLayerName); }

	inline int					GetObjectID();
	void						SetSubInfo(const char* _pSzSubInfo){ m_strSubInfo_ = _pSzSubInfo; }
	const std::string&			GetSubInfo(){ return m_strSubInfo_; }

	inline bool					GetPosition(rs3::RVector& _refVecPos) const { return OnGetPos_(_refVecPos); }
	inline void					SetPosition(const rs3::RVector& _refVecPos){ OnSetPos_(_refVecPos);NeedUpdate(); }
	inline SAttributeAtXML*		GetPosAttribute(){ return OnGetPosAttribute_(); }

	inline bool					GetDirection(rs3::RVector& _refVecDir) const { return OnGetDir_(_refVecDir); }
	inline void					SetDirectionUp(const rs3::RVector& _refVecDir, const rs3::RVector& _refVecUp){ OnSetDirUp_(_refVecDir, _refVecUp);NeedUpdate(); }
	inline SAttributeAtXML*		GetDirAttribute(){ return OnGetDirAttribute_(); }

	inline bool					GetUp(rs3::RVector& _refVecUp) const { return OnGetUp_(_refVecUp); }
	inline SAttributeAtXML*		GetUpAttribute(){ return OnGetUpAttribute_(); }

	inline bool					GetScale(float& _refSize) const { return OnGetScale_(_refSize); }
	inline void					SetScale(float _fScale) { OnSetScale_(_fScale);NeedUpdate(); }
	inline SAttributeAtXML*		GetScaleAttribute(){ return m_pRadiusAttribute_; }

	bool						IsNeedUpdate() { return m_bNeedUpdate; }
	rs3::RBoundingBox&			GetAABB(){ return m_aabb_; }

	bool						GetEditable() { return m_bEditable; }
	void						SetEditable(bool bValue) { m_bEditable = bValue; OnSetEditable(bValue); }

	CLogicObject*				PickForSelection(float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to){ return OnPickForSelection_(_rPrevPickedSqLength, origin, to); }
	void						PickForSelection(std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum){ OnPickForSelection_(_refVecLogicObject, _refFrustum); }
	void						SetSelection(bool bSelect){ OnSelected_(bSelect); }

	virtual CLogicObject*		Clone() { return NULL; }

	virtual ICommandBuffer*		CreateCloneNewCmdBuffer()	{ return NULL; }	// 로직 오브젝트 클론 생성 언두 커맨드
	virtual ICommandBuffer*		CreateDeleteCmdBuffer();						// 삭제 언두 커맨드

	virtual void				GatherLogicObjectsIncluding( std::list< CLogicObject* >& listObjects ) {}	///< 포함하는 로직오브젝트를 리턴한다

	// property 관련
	virtual CControlSceneProperty*	GetProperty() { return NULL; }		///< legacy, TODO: 없애야 한다
	virtual CBaseObject*		GetReflectProperty() { return NULL; }	// Reflective Property 를 얻어낸다
	virtual void				OnPropertyChangedFromUI() {};			// Property 가 UI 로 부터 변경되었을때 불린다

	// view 관련 - renderer, icon 등 
	virtual int					GetIconIndex() { return 0; }	// 기본 아이콘
	virtual CLogicObjectRenderer*	CreateRenderer(CControlMediator* pControlMediator_ ) { return NULL; }	

	void						OnLoadCompleted() { OnLoadCompleted_(); };

protected:

	virtual void				OnRender() = 0;
	virtual CLogicObject*		OnPickForSelection_(float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to) = 0; //{ return NULL; }
	virtual void				OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum) = 0; //{}
	virtual void				OnSelected_(bool bSelected) = 0; //{}

protected:

	virtual void				OnUpdate(){}
	virtual void				OnLoadCompleted_(){}
	virtual void				OnUpdateAABB_(){}

	virtual void				UpdateSpecialBOOL_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase){}
	virtual void				UpdateSpecialUINT_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase){}
	virtual void				UpdateSpecialSTRING_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase){}
	virtual void				UpdateSpecialFLOAT_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase){}

	virtual void				UpdateSpecialPos_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase){}
	virtual void				UpdateSpecialDir_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase){}
	virtual void				UpdateSpecialUp_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase){}

	// Base(LogicObject와 1:1관계에있는) 위치 / 방향 / 스케일 ( Set / Get 호출시에 불린다. )
	virtual void				OnSetObjectName(const char* szName);
	virtual void				OnSetLayerName(const char* szName);
	virtual void				OnSetVisible(bool bVisible) {}
	virtual void				OnSetEditable(bool bEditable) {}
	virtual void				OnSetPos_(const rs3::RVector &_refUpdatePos);
	virtual void				OnSetDirUp_(const rs3::RVector &_refUpdateDir, const rs3::RVector &_refUpdateUp);
	virtual void				OnSetScale_(const float fUpdateRadius);
	virtual bool				OnGetPos_(rs3::RVector& _refVecPos) const;
	virtual bool				OnGetDir_(rs3::RVector& _refVecDir) const;
	virtual bool				OnGetUp_(rs3::RVector& _refVecUp) const;
	virtual bool				OnGetScale_(float& _refSize) const;
	virtual SAttributeAtXML*	OnGetPosAttribute_(){ return m_pPosAttribute_; }
	virtual SAttributeAtXML*	OnGetDirAttribute_(){ return m_pDirAttribute_; }
	virtual SAttributeAtXML*	OnGetUpAttribute_(){ return m_pUpAttribute_; }

	void						UpdateElementWithTreeCtrl_(HTREEITEM hItem, SElementAtXML* _pElement, CTreeCtrl& _refTreeCtrl);
	void						SaveElement_(MXmlElement* _pXmlElement, SElementAtXML* _pLogicObjectElement);
	void						UpdateProperty_(SElementAtXML* _pObjectElement, SElementAtXML* _pObjectProperty, MXmlElement* _pElement, bool _bBaseCase);

	static void					UpdateSelectionInfoRecursion_(SElementAtXML* _pLogicObjectElement, CTreeCtrl& _refTreeCtrl, HTREEITEM hItem , EntityAttributeMap& _refEntityAttrMap);
	void						NeedUpdate() { m_bNeedUpdate = true; }

	bool						m_bEntityLogic;
	SElementAtXML*				m_pLogicObjectElement_;

	double*						m_pdX_;
	double*						m_pdY_;
	double*						m_pdZ_;
	SAttributeAtXML*			m_pPosAttribute_;

	double*						m_pdDirX_;
	double*						m_pdDirY_;
	double*						m_pdDirZ_;
	SAttributeAtXML*			m_pDirAttribute_;

	double*						m_pdUpX_;
	double*						m_pdUpY_;
	double*						m_pdUpZ_;
	SAttributeAtXML*			m_pUpAttribute_;

	double*						m_pdRadius_;
	SAttributeAtXML*			m_pRadiusAttribute_;

	rs3::RBoundingBox			m_aabb_;

	bool						m_bNeedUpdate;	///< 위치, 회전, 스케일이 변경되었을때
	bool						m_bVisibility_;
	bool						m_bEditable;

	std::string*				m_pStrLogicObjectName_;
	double*						m_pdID_;
	std::string					m_strSubInfo_;
	std::string					m_strName;		///< 기본 오브젝트 이름
	std::string					m_strLayer;		///< 속한 Layer 이름
};

inline const char* CLogicObject::GetObjectName()
{
	if(m_pStrLogicObjectName_ == NULL) return "";
	else return m_pStrLogicObjectName_->c_str();
}

inline const char* CLogicObject::GetLayerName() const
{
	 return m_strLayer.c_str();
}

inline int CLogicObject::GetObjectID()
{
	if(m_pdID_ != NULL) return (int)(*m_pdID_);
	else return -1;
}

template< typename T > void	CLogicObject::SetRootElementAttributeValue(const char* _pSzAttributeName, T& _value)
{
	for (SElementAtXML::AttributeList::iterator itr_attr = m_pLogicObjectElement_->m_vecAttribute.begin();
		itr_attr != m_pLogicObjectElement_->m_vecAttribute.end(); ++itr_attr)
	{
		SAttributeAtXML* pAttribute = *itr_attr;
		if (pAttribute->m_strAttrName == _pSzAttributeName)
		{
			pAttribute->m_vGenericNum[0] = (double)_value;
			break;
		}
	}
}
