#pragma once

#include "XGlueFunction.h"

/// lua에서 사용하는 내캐릭정보 API
class XGlueMyCharacterFunction : public XGlueFunction
{
public:
	virtual void DeclGlueFunc(MWLua* pLua);

	// 캐릭터 정보
	const char*				GetID() { return gvar.MyInfo.ChrInfo.szName; }					// 이름(ID)
	const char*				GetSurName() { return gvar.MyInfo.ChrInfo.szSurname; }			// First Name에 해당하는 이름
	int						GetLevel() { return gvar.MyInfo.nLevel; }				// 레벨
	int						GetBounty() { return gvar.MyInfo.ChrInfo.nBounty; }				// 바운티(돈)
	int						GetExpPercent() { return gvar.MyInfo.nExpPercent; }				// 경험치 퍼센트
	int						GetExp() { return gvar.MyInfo.ChrInfo.nXP; }					// 경험치

	int						GetItemMaxWeight() { return gvar.MyInfo.ChrInfo.nMaxWeight; }	// 소지 가능한 아이템 총 무게
	int						GetItemWeight() { return gvar.MyInfo.ChrInfo.nWeight; }			// 아이템 무게

	// 능력 정보
	int						GetSTR() { return gvar.MyInfo.ChrInfo.nSTR; }					// 완력
	int						GetDEX() { return gvar.MyInfo.ChrInfo.nDEX; }					// 회피
	int						GetINT() { return gvar.MyInfo.ChrInfo.nINT; }					// 지력
	int						GetCON() { return gvar.MyInfo.ChrInfo.nCON; }					// HP의 최대량을 결정
	int						GetCHA() { return gvar.MyInfo.ChrInfo.nCHA; }					// 카리스마

	// MOD
	int						GetModSTR() { return gvar.MyInfo.ChrInfo.nModSTR; }				// +완력
	int						GetModDEX() { return gvar.MyInfo.ChrInfo.nModDEX; }				// +회피
	int						GetModINT() { return gvar.MyInfo.ChrInfo.nModINT; }				// +지력
	int						GetModCON() { return gvar.MyInfo.ChrInfo.nModCON; }				// +HP의 최대량을 결정
	int						GetModCHA() { return gvar.MyInfo.ChrInfo.nModCHA; }				// +카리스마

	// 저항 정보
	int						GetMR() { return gvar.MyInfo.ChrInfo.nMR; }						// 기본 마법 저항력
	int						GetFR() { return gvar.MyInfo.ChrInfo.nFR; }						// 화속성 저항력
	int						GetCR() { return gvar.MyInfo.ChrInfo.nCR; }						// 수속성 저항력
	int						GetLR() { return gvar.MyInfo.ChrInfo.nLR; }						// 풍속성 저항력
	int						GetPR() { return gvar.MyInfo.ChrInfo.nPR; }						// 지속성 저항력

	// 상태 정보
	int						GetHP() { return gvar.MyInfo.ChrStatus.nHP; }					// 생명력
	int						GetEN() { return gvar.MyInfo.ChrStatus.nEN; }					// 정신력
	int						GetSTA() { return gvar.MyInfo.ChrStatus.nSTA; }					// 스테미너

	int						GetMaxHP() { return gvar.MyInfo.ChrInfo.nMaxHP; }				// 최대 생명력
	int						GetMaxEN() { return gvar.MyInfo.ChrInfo.nMaxEN; }				// 최대 정신력
	int						GetMaxSTA() { return gvar.MyInfo.ChrInfo.nMaxSTA; }				// 최대 스테미너

	// 인벤 정보
	void					UseItem(unsigned int nItemID_High, unsigned int nItemID_Low);	// 아이템 사용
	void					GetItemInfo(unsigned int nItemID_High, unsigned int nItemID_Low);// 아이템 정보
	const char*				GetItemName(unsigned int nItemID_High, unsigned int nItemID_Low);

	const char*				GetInventoryFirstItem();										// 인벤토리 첫번째 아이템
	const char*				GetInventoryNextItem();											// 인벤토리 다음 아이템

	// 장비착용 정보
	const char*				GetEquippedItem(const char* slot);

	// 무기착용 정보

	// 탤런트 정보

	// 아이템 정보

	// 걸린 탤런트

	// 무기 사용에 대한 처리

	// 맵 정보
	const char*				GetFieldName();
};
