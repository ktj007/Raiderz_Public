#pragma once

#include "XTalentInfoValidator.h"
#include "XNPCInfoValidator.h"
#include "XAnimationEventValidator.h"
#include "XIsAnimationValidator.h"
#include "XItemValidator.h"
//#include "XSoundValidator.h"				// <<<rsound3

/// 파일로부터 읽어들이는 각종 정보가 올바른지 체크하는 클래스
class XResourceValidator
{
private:
	XTalentInfoValidator		m_TalentInfoValidator;
	XNPCInfoValidator			m_NPCInfoValidator;
	XAnimationEventValidator	m_AnimationValidator;
	XIsAnimationValidator		m_ISAnimationValidator;
	XItemValidator				m_ItemValidator;
//	XSoundValidator				m_SoundValidator;			<<<rsound3

	void InitOutputWidget( bool ret );
public:
	bool CheckAll();

	bool CheckTalentInfo();
	bool CheckNPCInfo();
	bool CheckAnimaionEvent();
	bool CheckISAnimaion();
	bool CheckItem();
	bool CheckSound();
	bool CheckXML();
};