#pragma once

#define BOX_EDITOR_FACE_COUNT 6

class TMeshColInfoBoxEditor
{
private:
	class TBoxEditorFace
	{
	private:
		vec3			m_vViewStart;
		vec3			m_vViewEnd;

	public:
		vec3			m_vAxis;
		
		bool			m_bCheckSelect;
		bool			m_bSelected;

		CSColBox		m_boxPick;

	public:
		TBoxEditorFace();
		virtual ~TBoxEditorFace() {}
		
		void			Init(vec3 vAxis);
		void			RenderFace(float fAxisLen, vec3& vOrgCenter, MMatrix& matWorld);
		
		bool			PickTest(RVector2& pos, MRect& rect, float& fMinDist);

		void			CheckSelect();
		void			Selected();
		void			NonSelect();

		vec3			GetMoveValue(vec2 vMove);

		bool			CheckVauleType();
	};


private:
	string				m_strModelName;

	bool				m_bSelectBox;
	CSColBox			m_selectBox;

	int					m_nSelectedBox;
	int					m_nSelectedFaceIndex;

	TBoxEditorFace		m_Face[BOX_EDITOR_FACE_COUNT];

private:
	void				CalAnimationRenderBox(CSColBox * pColBox, CSColBox & outBox);
	vec3				CalEditionBoxFaceMoveData(vec3 vmin, vec3 vmax, vec3 vMove, bool bMinCal);

	void				RenderBox(CSColBox & boxInfo, DWORD dwColor = 0x20ff00ff);
	void				RenderEditionBox(CSColBox & boxInfo, DWORD dwColor);
	void				RenderEditionBoxFace(vec3 vAxis, float fAxisLen, vec3& vCenter, MMatrix& matWorld, DWORD dwColor);

	void				InitSelectBox();

	bool				CheckEditionBoxFaceDataMax(vec3& v1, vec3& v2);

public:
	TMeshColInfoBoxEditor();
	virtual ~TMeshColInfoBoxEditor() {}

	void				RenderColBox( int nSelectColID );

	void				SetModelName(string strModelName) { m_strModelName = strModelName; }

	bool				CheckMousePick(int stat, RVector2& pos, RVector2 prePos);
	bool				CheckMouseDown(int stat, RVector2& pos, RVector2 prePos);

	CSColBox*			GetColBox(int nIndex);
};