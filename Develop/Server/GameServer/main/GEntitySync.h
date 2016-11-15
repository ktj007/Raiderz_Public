#ifndef _GENTITYSYNC_H
#define _GENTITYSYNC_H

#include "GEntity.h"
#include "GFieldGrid.h"

class ProcessChangedSectorEvent;

class GEntitySync : public GEntity
{
public:
	GEntitySync();
	virtual				~GEntitySync();

	virtual bool		Create(MUID& uid);
	virtual bool		SetPos(const vec3& pos) override;
	virtual bool		IsHuge() const		{ return false; }

	inline MPoint GetSectorPoint()		{ return m_sectorPt; }
	inline int	GetSectorX()			{ return m_sectorPt.x; }
	inline int	GetSectorY()			{ return m_sectorPt.y; }

	// 섹터 질의
	void GetNeighborSectorNPC(vector<MUID>& vecNPC) const;
	void GetNeighborSectorPlayer(vector<MUID>& vecPlayer) const;

	// 메세지 관련
	virtual void RouteInEntity(const vector<MUID>& vecPlayers) {};
	virtual void RouteOutEntity(const vector<MUID>& vecPlayers) {};
	virtual void RouteToThisCell(MCommand* pCommand, MUID uidExcept=0) const;
	virtual void RouteToThisCellExceptMe(MCommand* pCommand) const;
	virtual void RouteToField(MCommand* pCommand) const;
	virtual void CollectEssentialReceiver(vector<MUID>& vecReceiverUID)	{ }

protected:
	virtual void OnEnter() override;
	virtual void OnLeave() override;
	
	// 섹터 입장/퇴장 관련 템플릿메서드들..
public:
	virtual MCommand* CreateFieldInCmd();	// 섹터 진입 시 새로운 섹터의 엔티티들에게 보낼 커맨드 생성
	virtual MCommand* CreateFieldOutCmd();	// 섹터 이탈 시 예전 섹터에 남아있는 엔티티들에게 보낼 커맨드 생성
protected:
	friend class ProcessChangedSectorEvent;
	virtual void OnVisitedNewSectorPC(const MUID& newSightedPC,MCommand* const fieldInCmd);
	virtual void OnVisitedOldSectorPC(const MUID& oldSIghtedPC,MCommand* const fieldOutCmd);
	virtual void OnVisitedNewSectorNPC(const MUID& newSightedNPC,MCommand* const fieldInCmd);
	virtual void OnVisitedOldSectorNPC(const MUID& oldSIghtedNPC,MCommand* const fieldOutCmd);
	virtual void OnSectorChanged(const MPoint& oldSectorPt);

	//todo 제거예정
	virtual void CalcNewCells(vector<GFieldGrid::Sector*>& vecNewCells, MPoint ptOldSec, MPoint ptNewSec) const;
	void CalcOldCells( vector<GFieldGrid::Sector*>& vecOldSectors, MPoint ptOldSec, MPoint ptNewSec ) const;
	virtual void OnSectorChanged( const vector<GFieldGrid::Sector*>& vecNewSectors, const vector<GFieldGrid::Sector*>& vecOldSectors );

private:
	void UpdateFieldCell();
	void ResetFieldCell();
	void RouteEnterField(bool bEnter);
	int  GetCellSize() const;
private:
	MPoint m_sectorPt;
};






#endif