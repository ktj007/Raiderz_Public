#pragma once

#include "XGlueFunction.h"

/// lua에서 사용하는 캐릭터 선택,생성,삭제 API
class XGlueCharFunction : public XGlueFunction
{
private:
	bool			CheckCurrFrame();
public:
	virtual void DeclGlueFunc(MWLua* pLua);

	/// 캐릭터 수를 반환한다.
	int				GetCharCount();
	/// 선택한 캐릭터의 이름을 반환한다.
	const char*		GetCharName(int nCharIndex);
	/// 선택한 캐릭터의 레벨을 반환한다.
	int				GetCharLevel(int nCharIndex);
	/// 선택한 캐릭터가 있는 곳의 필드명을 반환한다.
	const char*		GetCharInFieldName( int nCharIndex );
	/// 캐릭터를 선택한다.
	void			SelectCharacter(int nCharIndex);
	/// 캐릭터를 생성한다.
	void			CreateCharacter(const char* szName, MWLua::table t);
	/// 캐릭터를 삭제한다.
	void			DeleteCharacter(const char* szName, int nCharIndex);
	/// 선택한 캐릭터로 게임을 시작한다.
	void			StartGame(int nCharIndex);
	/// 캐릭터 생성시 모델을 변경한다.
	void			ChangeCreationModel(MWLua::table t);
	/// 캐릭터 생성시에 선택할 수 있는 머리 개수를 반환한다.
	int				GetHairMaxCount(int nSex);
	/// 캐릭터 생성시에 선택할 수 있는 얼굴 개수를 반환한다.
	int				GetFaceMaxCount(int nSex);

};
